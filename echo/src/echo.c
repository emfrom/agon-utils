/**
 * echo for Agon Light
 *
 * Prints arguments to stdout
 *
 * Original by Vasco Costa
 * Modifications by E.M. From
 *
 * Options recognized are -n to not add a newline to output
 * and -h for help
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/**
 * Helper function to print a help message
 *
 */
void show_usage (char *prog_name)
{
    printf ("Usage: %s -[hne] string\n", prog_name);
    printf ("-h show this help message\n");
    printf ("-n supress trailing newline\n");
    printf ("-e interpret \"\\\" escapes\n");
}

/**
 * main() using arguments
 * Extended argument processing (AgDev) is used
 * 
 */
int main (int argc, char *argv[])
{

    //Add newline to end of printout
    bool newline = true;

    //Where to start echo:ing
    // The default is to print nothing
    int first_arg_toprint = argc;

    //Interpret / escapes
    bool escapes = false;

  /** Check if we have arguments */
    if (argc == 1)
    {
        //Nope, print help message
        show_usage (argv[0]);
        return EXIT_SUCCESS;
    }

  /** Check for "-" in leading arguments */
    //For echo it's important that it's only leading arguments
    // since the user might want to echo something like "Type -n for ..."
    for (int i = 1; i < argc; i++)
    {
      /** Check if it's an option */
        //See if the first letter of argument i is a -
        if (*argv[i] != '-')
        {
            //Nope, the rest is stuff to print
            first_arg_toprint = i;
            break;
        }

      /** Handle the options */
        //Get the second letter from the pointer argv[i]
        switch (*(argv[i] + 1))
        {
            //Omit trailing newline
        case 'n':
            newline = false;
            break;;

            //Interpret escapes
        case 'e':
            escapes = true;
            break;;

            // h, user wanted to see help
        case 'h':
            show_usage (argv[0]);
            return EXIT_SUCCESS;

            // something else, we dont support that
        default:
            fprintf (stderr, "Unknown option: %s\n", argv[i]);
            return EXIT_FAILURE;
        }
    }

  /** Check if we have anything to print */
    if (argc == first_arg_toprint)
    {
        //Nope, exit silently
        return EXIT_SUCCESS;
    }


  /** Create a string of the args to print*/
    //This might seem a very roundabout way of doing this
    // Essentially, we do three distinct steps (modules if you wish)
    // 1. Create string
    // 2. Change the string (if we use escapes)
    // 3. Print the string
    //
    // This way the three are indenpendant, aka not intertwined
    //  and therefore easier to debug and maintain

    //String
    char *string;

    //Start with length 0, i.e. with only a string terminating 0(aka NUL) in it.
    // Do malloc(1) because the 0 itself needs a byte to be stored in
    // If we dont add this 0 token, the string is not a valid string of length 0
    if ((string = malloc (1)) == NULL)
    {
        fprintf (stderr, "Out of memory\n");
        return EXIT_FAILURE;
    }
    //Add the terminating 0 token 
    string[0] = '\0';


    //Length of string
    size_t string_length = 0;

    //Create the string
    for (int i = first_arg_toprint; i < argc; i++)
    {
        //Calculatet new size so argv[1] fits in it
        // Add 1 for the space between args
        string_length += strlen (argv[i]) + 1;

        //Get more memory
        // Add 1 for the terminating 0
        if ((string = realloc (string, string_length + 1)) == NULL)
        {
            fprintf (stderr, "Out of memory\n");
            return EXIT_FAILURE;
        }

        //Append argv[i] to the string
        strcat (string, argv[i]);

        //Append a space to the string
        strcat (string, " ");
    }

  /** Remove trailing space */
    string_length--;
    string[string_length] = '\0';


  /** Interpreting escapes ? */
    // This is an essential part of echo
    // Without this you cannot produce formatted output
    //  or multiline messages
    if (escapes)
    {
        //Temp pointer
        char *temp = string;

        //Find next \, if any
        while ((temp = strchr (temp, '\\')) != NULL)
        {

            char escape_code = *(temp + 1);

        /** Check if it's the stop output token */
            if (escape_code == 'c')
            {
                *temp = '\0';

                //Update the length of the string
                string_length = strlen (string);

                //No more processing, on to printing
                break;
            }

        /** Handle the escape code */
            // Write the binary value of the escape sequence into the string

            //Pointer to the rest of the string after the escape sequence
            // Used to overwrite the leftover tokens in the escape
            // Most escapes are 2 long
            // For the other cases, it's dynamic and changes based on the conversion
            char *rest_of_string = temp + 2;

            //Length of string will go down by at least one
            //Correct below in the other two cases
            string_length -= 1;

            //Replace the first \ in the string with the code 
            switch (escape_code)
            {
                // \\     backslash
            case '\\':
                *temp = '\\';
                break;

                // \a     alert (BEL)
            case 'a':
                *temp = '\a';
                break;

                // \b     backspace
            case 'b':
                *temp = '\b';
                break;

                // \e     escape
            case 'e':
                *temp = '\e';
                break;

                // \f     form feed
            case 'f':
                *temp = '\f';
                break;

                // \n     new line
            case 'n':
                *temp = '\n';
                break;

                // \r     carriage return
            case 'r':
                *temp = '\n';
                break;

                // \t     horizontal tab
            case 't':
                *temp = '\t';
                break;

                // \v     vertical tab
            case 'v':
                *temp = '\v';
                break;

            /** For the last two cases we need to do a conversion */
                //string_length needs to be incremented by one to offset
                // the earlier subtraction for the other cases

                //Create a substring by saving a value and replacing it with \0
                // We need to do this since we are only converting one bye
                // i.e. the pattern \xFF00 is supposed to output 3 bytes
                // [0xFF,0x30,0x30] so 255 followed by two ascii 0s
                // but strtol will read it as a two byte hex value
                char temp_save;

                //strtol returns a long
                long value;

                // \0NNN octal number
            case '0':
                //Save a char and replace it with \0
                temp_save = temp[5];
                temp[5] = '\0';

                //Convert the value
                value = strtol (temp + 2, &rest_of_string, 8);

                //Check that it's valid
                if (!value && rest_of_string == temp + 2)
                {
                    fprintf (stderr, "Invalid escape code: %s\n", temp);
                    exit (EXIT_FAILURE);
                }

                //Assign the value
                *temp = (char) value;

                //Restore saved char
                temp[5] = temp_save;

                //Recalculate string length
                string_length -= rest_of_string - temp;
                string_length += 2;     //Correction
                break;

                // \xNN hex number
            case 'x':
                //Save a char and replace it with \0
                temp_save = temp[4];
                temp[4] = '\0';

                //Convert the value
                value = strtol (temp + 2, &rest_of_string, 16);

                //Check that it's valid
                if (!value && rest_of_string == temp + 2)
                {
                    fprintf (stderr, "Invalid escape code: %s\n", temp);
                    exit (EXIT_FAILURE);
                }

                //Assign the value
                *temp = (char) value;

                //Restore saved char
                temp[4] = temp_save;

                //Recalculate string length
                string_length -= rest_of_string - temp;
                string_length += 2;     //Correction
                break;

            /** Invalid escape code */
            default:
                *(temp + 2) = '\0';
                fprintf (stderr, "Invalid escape code: %s\n", temp);
                exit (EXIT_FAILURE);
            }

        /** Move the rest of the string down */
            //Move temp past the escaped char
            // Otherwise a \\ might create a new escape by accident
            temp++;

            //Copy rest of string down
            // We cannot use str* functions since source and dest overlap
            int i;
            for (i = 0; rest_of_string[i] != '\0'; i++)
                temp[i] = rest_of_string[i];

            // Terminate temp
            temp[i] = '\0';
        }
    }



  /** Correct string ending for printing */
    if (newline)
    {
        //There will always be space for this
        // We can overwrite the trailing \0
        // since we use fwrite to print
        string[string_length] = '\n';
        string_length++;
    }

  /** Print the string */
    //Use fwrite(), it's faster
    if (fwrite (string, string_length, 1, stdout) != 1)
    {
        fprintf (stderr, "Standard output error");
        exit (EXIT_FAILURE);;
    }

  /** Free up string */
    free (string);

  /** All done, exiting */
    return EXIT_SUCCESS;
}
