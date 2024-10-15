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
 * Helper funcion to print a help message
 *
 */
void show_usage (char *prog_name)
{
    printf ("Usage: %s [-hn] string\r\n", prog_name);
    printf ("-h show this help message\r\n");
    printf ("-n supress trailing newline\r\n");
}

/**
 * main() using arguments
 * Extended argument processing (AgDev) is not used
 * 
 */
int main (int argc, char *argv[])
{
    //Add newline to end of printout
    bool newline = true;

    //Start on which argument
    int first_arg_toprint = 1;

  /** Check if we have arguments */
    if (argc == 1)
    {
        //Nope, print help message
        show_usage (argv[0]);
        return EXIT_SUCCESS;
    }

  /** Check for "-" in first argument */
    if (*argv[1] == '-')
    {
        //Check if it's -n
        if (tolower (*(argv[1] + 1)) == 'n')
        {                       //*(argv[1] + ) means 2nd letter in argv[1]
            newline = false;
        }
        else
        {
            //Any other letter after - means show help
            // If it's h, user wanted to see help
            // If it's something else, we dont support that
            show_usage (argv[0]);
            return EXIT_SUCCESS;
        }

        //Start printing from 2nd argument
        first_arg_toprint = 2;
    }


  /** Print the remaining arguments */
    for (int i = first_arg_toprint; i < argc; i++)
    {
        printf ("%s", argv[i]);

        //Add spaces, unless we are printing last argument
        if (i < argc - 1)
        {
            printf (" ");
        }
    }

  /** Print a newline if wanted */
    if (newline)
    {
        printf ("\r\n");
    }

    return 0;
}
