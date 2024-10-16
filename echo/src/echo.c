
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
 * Extended argument processing (AgDev) is used
 * 
 */
int main (int argc, char *argv[])
{
    //Add newline to end of printout
    bool newline = true;

    //Where to start echo:ing
    int first_arg_toprint = 1;

  /** Check if we have arguments */
    if (argc == 1)
    {
        //Nope, print help message
        show_usage (argv[0]);
        return EXIT_SUCCESS;
    }

  /** Check for "-" in leading arguments */
    //For echo it's important that it's only leading argument
    // since the user might want to echo something like "Type -n for ..."
    for (int i = 1; i < argc; i++)
    {
      /** Check if it's an option */
        //Do this by seeing if the first letter of argument i is a -
        if (*argv[i] != '-')
        {
            //Nope, move on to printing
            first_arg_toprint = i;
            break;
        }

      /** Get the option letter */
        char option;
        //Get the second letter from the pointer argv[i]
        option = *(argv[i] + 1);
        //Change to lowercase
        option = tolower (option);


      /** Handle the options */
        switch (option)
        {
            //Omit trailing newline
        case n:
            newline = false;
            break;;

            //Any other char means show help
            // h, user wanted to see help
        case h:
            show_usage (argv[0]);
            return EXIT_SUCCESS;

            // something else, we dont support that
        default:
            show_usage (argv[0]);
            return EXIT_FAILUTE;
        }
    }


  /** Print the remaining arguments */
    //If argc == first_arg_toprint this will not be run
    // i.e. nothing to print
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
