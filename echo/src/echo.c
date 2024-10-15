/**
 * echo for Agon Light
 *
 * Prints arguments to stdout
 *
 * Original by Vasco Costa
 * Modifications by E.M. From
 *
 * Very straight forward. Options recognized are -n to not add a newline to output
 * and -h for help
 *
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

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
    bool newline = true;

  /** Loop over all arguments */
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (strcmp (argv[i], "-h") == 0)
            {
                show_usage (argv[0]);

                return 0;
            }

            if (strcmp (argv[i], "-n") == 0)
            {
                newline = false;

                continue;
            }

            printf ("%s", argv[i]);

            if (i < argc - 1)
            {
                printf (" ");
            }
        }
    }

  /** Print a newline if needed */
    if (newline)
    {
        printf ("\r\n");
    }

    return 0;
}
