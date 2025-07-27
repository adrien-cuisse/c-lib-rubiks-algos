
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "scramble.h"




void init_random(void)
{
	srand(time(NULL));
}


size_t count_occurences(char const * haystack, char needle)
{
	size_t count = 0;

	for (int index = 0; haystack[index] != '\0'; index++)
	{
		if (haystack[index] == needle)
			count++;
	}

	return count;
}


char const * find_repeated_moves(char const * scramble)
{
	char const * previous_move;

	while (1)
	{
		previous_move = scramble;
		scramble = strchr(scramble + 1, ' ');

		if (scramble == NULL)
			return NULL;

		scramble++;

		if (previous_move[0] == scramble[0])
			return previous_move;
	}

	return NULL;
}


/**
 * Checks if a string is in an array
 *
 * @param haystack - the array to search in, expected to be NULL-terminated
 *
 * @param needle - the string to find
 *
 * @return int - 1 if the string is stored in the array, 0 otherwise
 */
static int string_in_array(char const ** haystack, char * needle)
{
	for (int index = 0; haystack[index] != NULL; index++)
	{
		if (strcmp(haystack[index], needle) == 0)
			return 1;
	}

	return 0;
}


char * find_invalid_move(char * scramble, char const ** valid_moves)
{
	char * token = strtok(scramble, " ");

	while (token != NULL)
	{
		if (! string_in_array(valid_moves, token))
			return token;

		token = strtok(NULL, " ");
	}

	return NULL;
}
