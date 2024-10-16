/**
 * grep for Agon Light
 *
 * Searches files for lines matching a pattern
 *
 * Pattern is text, not regexp
 *
 * Original by Vasco Costa
 * Modifications by E.M. From
 *
 * Options recognized are:
 * -h show help
 * -e Do case insensitive search
 *
 */
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Maximum lenght of one line
 *
 */
#ifndef LINEMAX
#define LINEMAX 16384
#endif

/**
 * Helper function to print a help message
 *
 */
void show_usage (char *prog_name)
{
    printf ("Usage: %s [-hi] pattern filename\r\n", prog_name);
    printf ("-h show this help message\r\n");
    printf ("-i case insensitive matching\r\n");
}

/**
 * Search for a substring in a string
 * case insensitive
 *
 */
const char *strcasestr (const char *haystack, const char *needle)
{

  /** Check valid inputs */
    if (haystack == NULL || needle == NULL)
    {
        return NULL;
    }

  /** Searching for an empty string ? */
    if (*needle == '\0')
    {
        //Yup
        //Empty sring matches everything
        return haystack;
    }

  /** Serach for needle in haystack */
    //Do one search for needle for every character in haystack
    for (; *haystack; haystack++)
    {
        //Search variables
        const char *h = haystack;
        const char *n = needle;

    /** Compare each character in a case-insensitive manner */
        while (*h && *n
               && (tolower ((unsigned char) *h) ==
                   tolower ((unsigned char) *n)))
        {
            h++;
            n++;
        }

    /** Match found ? */
        //If we are at the end of needle, a match was found
        if (!*n)
        {
            // Match found
            return haystack;
        }
    }

    // No match found
    return NULL;
}

/**
 * Go over a file and print lines that matches pattern
 * Case insensitive if insensitive is true
 *
 */
void match_pattern (bool insensitive, char *pattern, FILE * file)
{
    //Storage for lines read
    char line[LINEMAX];

  /** Get all lines and look for matches */
    while (fgets (line, sizeof (line), file) != NULL)
    {
        bool match_found;


      /** Case insensitive search ?*/
        if (insensitive)
        {
            //Look for pattern ignoring case
            match_found = (strcasestr (line, pattern) != NULL);
        }
        else
        {
            //Look for pattern
            match_found = (strstr (line, pattern) != NULL);
        }

      /** Match found ? */
        if (match_found)
        {
            //Print line
            printf ("%s", line);
        }
    }
}


/**
 * main() using arguments
 * Extended argument processing (AgDev) is used
 * 
 */
int main (int argc, char *argv[])
{
    FILE *file;
    bool insensitive = false;
    char *pattern = NULL;
    char *filename = NULL;

  /** Argument processing */
    for (int i = 1; i != argc; i++)
    {
      /** User requested help */
        if (strcmp (argv[i], "-h") == 0)
        {
            show_usage (argv[0]);

            return EXIT_SUCCESS;
        }

      /** User wants case insensitive search */
        else if (strcmp (argv[i], "-i") == 0)
        {
            insensitive = true;
        }

      /** Fist non option is pattern to search for */
        else if (!pattern)
        {
            pattern = argv[i];
        }
      /** Second non option is filename */
        else if (!filename)
        {
            filename = argv[i];
        }
      /** Too many arguments */
        else
        {
            show_usage (argv[0]);

            return EXIT_FAILURE;
        }

    }

  /** Verify we have a pattern */
    if (pattern == NULL)
    {
        show_usage (argv[0]);

        return EXIT_FAILURE;;
    }

  /** Open text source */
    if (filename == NULL)
    {
        //No filename means read from stdin
        file = stdin;
    }
    else if ((file = fopen (filename, "r")) == NULL)
    {
        fprintf (stderr, "Error opening file");
        return EXIT_FAILURE;
    }


  /** Search file for patterns */
    match_pattern (insensitive, pattern, file);


    //Finish up
    fclose (file);

  /** All done, exiting */
    return EXIT_SUCCESS;
}
