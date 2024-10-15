/**
 * tail for Agon Light
 *
 * Show the N last lines in a textfile
 *
 * Original by Vasco Costa
 * Modifications by E.M. From
 *
 * This tail implementation uses fseek() to go to the end of the file
 * and from there read blocks backwards until it has found enough lines to print.
 *
 * The size of the blocks are guestimated by assuming 20 characters per line,
 *  making each block 20 * the number of lines to print bytes long.
 *
 * The blocks of text are stored on a stack and printed in reverse order.
 *
 * This approach significantly increases the speed, particularily on large files,
 *  since as few lines as is reasonaby possible are read from the sdcard.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * An (un)educated guess of how long an average text line is
 *
 * Setting this to 1 will make the program load the minimum possible
 * from file and push a lot of data onto the stack
 * Setting it to something big (like 0x1000) will make it load
 * a lot of unneccesary stuff and probably not use the stack at all
 * (with the added bonus of running out of memory real quick)
 */
#define LINE_LENGTH_GUESSTIMATE 20

/**
 * Helper funcion to print a help message
 *
 */
void show_usage (char *prog_name)
{
    printf ("Usage: %s [-hn] filename\r\n", prog_name);
    printf ("-h show this help message\r\n");
    printf ("-n print the last n lines (default: 10)\r\n");
}

/**
 * Helper function to exit with an error message
 * Makes the code a lot more readable
 *
 */
void exit_with_error (char *message)
{
    fprintf (stderr, "%s\n", message);
    exit (EXIT_FAILURE);
}

/**
 * As we are reading the file backwards looking for
 * lines to print, using a stack makes the
 * job a lot easier. The last block we read will be
 * the first one that is printed. (LIFO)
 *
 * The stack is the data structure to save the day
 *
 * https://en.wikipedia.org/wiki/Stack_(abstract_data_type)
 * (or ask ChatGPT or any programming LLM to explain it)
 *
 */
//We declare the pointer to the data structure first
//C allows creating pointers to types it doesnt know.
// This is since all pointers are the same (in the CPU)
typedef struct text_stack_s *text_stack;

//That way, we can include a pointer to the data structure
// inside the structure itself. Very handy.
struct text_stack_s
{
    //Pointer to the entry below us on the stack
    text_stack below_this;

    //Data to store on the stack
    char *text;
    size_t size;
};

/**
 * Push a block of text onto a stack
 *
 * i.e. add this block of text to the top of the stack
 */
text_stack text_stack_push (text_stack stack, char *text, size_t size)
{

  /** Allocate space */
    text_stack new_entry;

    new_entry = malloc (sizeof (struct text_stack_s));
    if (new_entry == NULL)
        exit_with_error ("Could not allocate memory");

  /** Assign variables */
    //The old top of the stack is now below what we just added
    new_entry->below_this = stack;

    //Our data
    new_entry->text = text;
    new_entry->size = size;

    return new_entry;
}

/**
 * Pop a segment of text off of a stack
 *
 * i.e. remove the top block of text from the stack
 *
 */
text_stack text_stack_pop (text_stack stack, char **text, size_t *size)
{

  /** Empty stack? */
    if (stack == NULL)
    {
        return NULL;
    }

  /**Assign text and size if they are non-NULL */
    if (text != NULL)
        *text = stack->text;

    if (size != NULL)
        *size = stack->size;

  /** Free up the stack entry */
    //Store the new top of the stack
    text_stack temp;

    temp = stack->below_this;
    free (stack);

  /** Return the new top of the stack */
    return temp;
}

/**
 * Display the last N lines from file
 * 
 */
void show_lines (FILE * file, int lines)
{

  /** Determine file size */
    long filesize;

    //Move read pointer to end of file
    if (0 != fseek (file, 0L, SEEK_END))
        exit_with_error ("Filesystem error");

    //Get the file position we are at
    // since we are at the end of the fils,
    // position is equal to the size of the file
    filesize = ftell (file);
    if (filesize == -1)
        exit_with_error ("Filesystem error");

  /** Implicit newline at end of file? */
    // If last char is not a newline, we add one so that
    //  lines will print properly
    int implicit_newline = 0;

    //Move read pointer to last character in file
    if (0 != fseek (file, -1L, SEEK_END))
        exit_with_error ("Filesystem error");

    //Check last character
    if (fgetc (file) != '\n')
    {
        // As the last line will not count as a line in the code
        //  below, remove one line to look for
        lines--;
        implicit_newline = 1;
    }

  /** Guesstimate how much to load */
    size_t loadsize = lines * LINE_LENGTH_GUESSTIMATE;

  /** Variables needed */
    //Keep track of how much text has been loaded
    size_t size_loaded = 0;

    //Amount of line endings found, aka'\n' 
    // Since we are reading backwards this means
    // lines we have found - 1
    int line_endings_found = 0;

    //Stack for the loaded text
    // NULL in this case means we start with an empty stack
    text_stack stack = NULL;

    //Offset of the first line to print in the
    // first block to print
    int offset;

  /** Loop until we have as many lines as we want 
      OR we run out of file to load */
    while (line_endings_found <= lines && size_loaded < filesize)
    {

        //Check that there is enough text left in the file
        // for us to load an entire block
        if (size_loaded + loadsize > filesize)
            // If not, read the rest of the file
            loadsize = filesize - size_loaded;

    /** Allocate a buffer */
        char *text_buffer;

        text_buffer = malloc (loadsize);
        if (text_buffer == NULL)
            exit_with_error ("Could not allocate memory");

    /** Reposition the read point in the file */
        if (0 != fseek (file, 0L - (size_loaded + loadsize), SEEK_END))
            exit_with_error ("Filesystem error");

    /** Fill the buffer from file */
        if (1 != fread (text_buffer, loadsize, 1, file))
            exit_with_error ("Filesystem error");

        //Update how much we have loaded
        size_loaded += loadsize;

    /** Count lines in the text_buffer */
        // On exit, offset will point to the last newline before the first line to print
        // Which is used as is (I.e. print a newline before the lines)
        // This is to avoid odd cases where the only char that's in a block is a
        // newline that will not be printed
        //
        // Note: There are two conditions in the for:loop
        for (offset = loadsize;
             offset >= 0 && line_endings_found <= lines; offset--)
            if (text_buffer[offset] == '\n')
                line_endings_found++;

    /** Push onto the stack */
        // i.e. put this block on the top
        stack = text_stack_push (stack, text_buffer, loadsize);

    }

  /** Make sure offset is valid */
    //If all data loaded will be printed
    //(or by happenstance exactly at the start of a buffer)
    // we set offset to 0
    if (offset == -1)
        offset = 0;

  /** Print the lines found */
    // NULL mean the stack is empty
    while (stack != NULL)
    {

    /** Get the text block to print from the stack */
        char *text;
        size_t size;

        stack = text_stack_pop (stack, &text, &size);

    /** Check if we are printing the first block */
        // If offset is non-zero, this is the first block
        // and we offset printing so that only the lines
        // that we want are printed
        if (offset)
        {
            //Print with fwrite because it's (probably) faster
            if (1 != fwrite (text + offset, size - offset, 1, stdout))
                exit_with_error ("Standard output error");

            //offset = 0 means the rest of the blocks will be printed
            // as is
            offset = 0;
        }
        else
            //For blocks other than the first block, print everything
        if (1 != fwrite (text, size, 1, stdout))
            exit_with_error ("Standard output error");

    /** Free up the text buffer */
        free (text);
    }

  /** Write implicit newline, if needed */
    if (implicit_newline)
        printf ("\n");
}

/**
 * main() using arguments
 * Extended argument processing (AgDev) is used
 * 
 */
int main (int argc, char *argv[])
{
    FILE *file = NULL;
    char *filename = NULL;
    int parsed_lines = 0;
    size_t lines = 10;

  /** Argument processing */
    for (int i = 1; i != argc; i++)
    {

    /** User requested help */
        if (strcmp (argv[i], "-h") == 0)
        {
            show_usage (argv[0]);
            return EXIT_SUCCESS;
        }

    /** User specified number of lines as -n N */
        else if (strncmp (argv[i], "-n", 2) == 0)
        {
            parsed_lines = atoi (argv[++i]);

            //Check that we have a valid N
            //atoi returns 0 on error
            if (parsed_lines <= 0)
                exit_with_error ("The number of lines must be positive");

            lines = parsed_lines;
        }

    /** User specified number of lines as -N*/
        else if (sscanf (argv[i], "-%i", &parsed_lines) == 1)
        {
            if (parsed_lines <= 0)
                exit_with_error ("The number of lines must be positive");

            lines = parsed_lines;
        }

    /** First non option argument is (probably) filename */
        //Check that we dont already have a filename
        else if (!filename)
        {
            filename = argv[i];
        }

    /** Invalid command */
        else
        {
            show_usage (argv[0]);
            return EXIT_FAILURE;
        }
    }

  /** Check that we have a filename */
    if (filename == NULL)
    {
        show_usage (argv[0]);
        return EXIT_SUCCESS;
    }

  /** Open input file */
    if (!(file = fopen (filename, "r")))
        exit_with_error ("Error opening file");

  /** Show last N lines in file */
    show_lines (file, lines);

  /** All done, exiting */
    fclose (file);
    return EXIT_SUCCESS;;
}
