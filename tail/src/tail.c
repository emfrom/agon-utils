/**
 * tail for Agon Light / MOS
 *
 * Show the N last lines in a textfile
 *
 * Original by Vasco Costa
 * Modifications by E.M. From
 *
 *
 * @file main.c
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/**
 * Print a help message
 * 
 * @param param1 Name of the program 
 * @return none
 */
void show_usage(char *prog_name)
{
	printf("Usage: %s [-hn] filename\r\n", prog_name);
	printf("-h show this help message\r\n");
	printf("-n print the last n lines (default: 10)\r\n");
}

/**
 * Helper function to exit with an error message
 * Makes the code a lot more readable
 *
 * @param param1 Message to display
 * @return none
 */
void exit_with_error(char *message) {
  fprintf(stderr,message);
  exit(EXIT_FAILURE);
}



/**
 * Display the last N lines from file
 * 
 * @param param1 File to read text from
 * @param param2 Number of lines to display
 * @return none
 */
void show_lines(FILE *file, int lines)
{

}

/**
 * main using agruments
 * Argument processing (AgDev) is not used
 * 
 * @param param1 Number of arguments
 * @param param2 Array of arguments
 * @return Exit status of the program
 */
int main(int argc, char *argv[])
{
	FILE *file = NULL;
	char *filename = NULL;
	int parsed_lines = 0;
	size_t lines = 10;

	for (int i = 1; i != argc; i++)
	{
		if (strcmp(argv[i], "-h") == 0)
		{
			show_usage(argv[0]);
			return EXIT_SUCCESS;
		}
		else if (strncmp(argv[i], "-n", 2) == 0)
		{
			parsed_lines = atoi(argv[++i]);

			if (parsed_lines <= 0)
			  exit_with_error("The number of lines must be positive");


			lines = parsed_lines;
		}
		else if (!filename)
		{
			filename = argv[i];
		}
		else
		{
			show_usage(argv[0]);
			return EXIT_SUCCESS;
		}
	}

	if (filename == NULL)
	{
		show_usage(argv[0]);
		return EXIT_SUCCESS;
	}

	if (!(file = fopen(filename, "r")))
	  exit_with_error("Error opening file");


	show_lines(file, lines);
	fclose(file);

	return EXIT_SUCCESS;;
}
