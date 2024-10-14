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
 * Display the last N lines from file
 * 
 * @param param1 File to read text from
 * @param param2 Number of lines to display
 * @return none
 */
void show_lines(FILE *file, int lines)
{
	char **buf = malloc(sizeof(char *) * lines);

	for (int i = 0; i < lines; i++)
		buf[i] = malloc(sizeof(char) * 1024);

	int lc = 0;

	while (fgets(buf[lc % lines], 1024, file) != NULL)
	{
		lc++;
	}

	int start = lc > lines ? lc % lines : 0;
	int count = lc > lines ? lines : lc;

	for (int i = 0; i < count; i++)
	{
		printf("%s", buf[(start + i) % lines]);
	}

	for (int i = 0; i < lines; i++)
		free(buf[i]);

	free(buf);
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
			{
				fprintf(stderr, "The number of lines must be positive");
				return EXIT_FAILURE;
			}

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
	{
		fprintf(stderr, "Error opening file");
		return EXIT_FAILURE;
	}

	show_lines(file, lines);
	fclose(file);

	return EXIT_SUCCESS;;
}
