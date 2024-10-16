/** Keyboard layout changer for Agon Light
 *
 * By E.M. From
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <agon/vdp_vdu.h>

/**
 * Helper function to print a help message
 *
 */
void show_usage (char *prog_name)
{
    printf ("Usage: %s layout\n\n", prog_name);
    printf ("Where layout is one of:\n");
    printf ("0     UK     UK\n");
    printf ("1     US     US\n");
    printf ("2     DE     German\n");
    printf ("3     IT     Italian\n");
    printf ("4     ES     Spanish\n");
    printf ("5     FR     French\n");
    printf ("6     BE     Belgian\n");
    printf ("7     NO     Norwegian\n");
    printf ("8     JP     Japanese \n");
    printf ("9     USI    US International\n");
    printf ("10    USA    US International Alternate\n");
    printf ("11    CHD    Swiss German\n");
    printf ("12    CHF    Swiss French\n");
    printf ("13    DK     Danish\n");
    printf ("14    SV     Swedish\n");
    printf ("15    PT     Portuguese\n");
    printf ("16    BR     Brazilian Portuguese\n");
    printf ("17    DV     Dvorak\n");
}

/** Short names of the layouts */
const char *layout_names[] = {
    "UK",
    "US",
    "DE",
    "IT",
    "ES",
    "FR",
    "BE",
    "NO",
    "JP",
    "USI",
    "USA",
    "CHD",
    "CHF",
    "DK",
    "SV",
    "PT",
    "BR",
    "DV"
};

/** Number of layouts currenty supported */
const int number_layouts = 18;

/**
 * main() using arguments
 * Extended argument processing (AgDev) is used
 * 
 */
int main (int argc, char *argv[])
{
    //Check if we have and argument
    if (argc != 2)
    {
        show_usage (argv[0]);
        return EXIT_SUCCESS;
    }

    //Layout wanted (if we find one)
    int locale = -1;

  /** Parse arguments */
    //First by short name
    for (int i = 0; i < number_layouts; i++)
    {
        if (0 == strcmp (argv[1], layout_names[i]))
        {
            locale = i;
            break;
        }

        char number[3];
        sprintf (number, "%d", i);

        if (0 == strcmp (argv[1], number))
        {
            locale = i;
            break;
        }
    }

    if (locale == -1)
    {
        show_usage (argv[0]);
        return EXIT_FAILURE;
    }

    vdp_set_keyboard_locale (locale);

    printf ("\nLayout set: %s\n", layout_names[locale]);
}
