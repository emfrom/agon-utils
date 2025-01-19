/**
 * wc for Agon Light
 *
 * Count characters, words and/or lines in a file
 *
 * Original by Vasco Costa
 * Modifications by E.M. From
 */


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

/**
 * Helper funcion to print a help message
 *
 */
void show_usage (char *prog_name)
{
    printf ("Usage: %s [-chlw] filename\r\n", prog_name);
    printf ("-c print the characters count\r\n");
    printf ("-l print the lines count\r\n");
    printf ("-w print the words count\r\n");
    printf ("-h show this help message\r\n");
}

/**
 * Count lines, words & characters from a stream
 *
 */
void count(FILE *input, long *lines, long *words, long *chars) {
  //Set the count to zero before we start
  *lines = 0;
  *words = 0;
  *chars = 0;

  //Storage to load into
  //We make it 513, the 512th and last position have to have a non null valid value
  // This is so we can tell if the 
  
  char storage[513];
  char *end_of_storage = storage + 511;

  //we start by pointing at the very last element
  //this is used to trigger the loading of a new block
  char *temp_pointer = end_of_storage + 1; 

  //set the very last element to anything other than \0
  *temp_pointer = ' '; 
  
  //Keep track of if we are in a word or not
  //We start "outside" a word
  int in_word = 0;
  
  
  //Check every character in the buffer
  while(*temp_pointer != '\0') {

    //do we need to load a new block?
    if(temp_pointer > end_of_storage) {
      long chars_read = fread(storage, 1, 512, input);

      //Check for empty read
      if(chars_read == 0) 
        if(feof(input)) //No more data
	  break;        //Leave the while loop

      

      //we got a complete 512 block?
      if(chars_read != 512) {
	//error?
	//this will also catch an empty read that is an error 
	if(ferror(input)) {
	  fprintf(stderr, "Error reading from file\n");
	  exit(EXIT_FAILURE);
	}

	//nul terminate the string we did get
	storage[chars_read] = '\0';
      }

      temp_pointer = storage;
    }

    // Increment character count
    // This can include control characters but wc's behaviour on unix(by posix standard)
    // is to count them as well. Seems reasonable.
    *chars += 1;

    
    //The dos style \r line ending screws things up, so we just ignore it
    if(*temp_pointer == '\r') {
      temp_pointer++;
      continue; //Go back to the while loop and "continue"
    }
    
    //check if we have a printable character
    if(!isprint(*temp_pointer)) {
      //nope, control char
      //control characters are counted as part of a word if there other printable chars in the word
      //and not if they appear on their own.
      //
      //
      temp_pointer++;
      continue;
    }

    
    //Whitespace?
    if(isspace(*temp_pointer)) {
      //Set inside of word marker to 0
      in_word = 0;

      //See if it was a newline
      if(*temp_pointer == '\n')
	*lines += 1;

      temp_pointer++;
      continue;
    }

    /* We have a non-whitespace printable character */

    //new word?
    if(!in_word) {
      //Set the inside word marker and increment word count
      in_word = 1;
      *words += 1;
    }

    //All done, go to next char
    temp_pointer++;
    continue;
  }

}

/**
 * Show the counts requested by the user
 *
 */
void show_counts (long lines, long words, long chars)
{
  if(lines >= 0)
    printf("  %ld", lines);

  if(words >= 0)
    printf("  %ld", words);

  if(chars >= 0)
    printf("  %ld", chars);

  printf("\n");
      
}

int main (int argc, char *argv[])
{
    FILE *file = NULL;
    char *filename = NULL;
    bool show_lines = false;
    bool show_words = false;
    bool show_chars = false;

    for (int i = 1; i != argc; i++)
    {
        if (strcmp (argv[i], "-h") == 0)
        {
            show_usage (argv[0]);

            return 0;
        }
        else if (strcmp (argv[i], "-l") == 0)
        {
            show_lines = true;
        }
        else if (strcmp (argv[i], "-w") == 0)
        {
            show_words = true;
        }
        else if (strcmp (argv[i], "-c") == 0)
        {
            show_chars = true;
        }
        else if (!filename)
        {
            filename = argv[i];
        }
        else
        {
            show_usage (argv[0]);

            return 0;
        }
    }

    if (filename == NULL) {
      show_usage(argv[0]);
      return EXIT_SUCCESS;
    }


    if (!(file = fopen (filename, "r")))
    {
        fprintf (stderr, "Error opening file");

        return EXIT_FAILURE;
    }

    //If user did not indicate what to display display everything
    if(!(show_chars | show_words | show_lines))
      show_chars = show_words = show_lines = true; //Set all three to true
	 

    long lines;
    long words;
    long chars;

    //Count lines, words and chars in file
    //All are counted since it costs very little extra to do so
    count(file, &lines, &words, &chars);
    fclose (file);
    
    if(!show_lines)
      lines = -1;

    if(!show_words)
      words = -1;

    if(!show_chars)
      chars = -1;

    // Display the counts
    show_counts (lines, words, chars);
    

    return EXIT_SUCCESS;
}
