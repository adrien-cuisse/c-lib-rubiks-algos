
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "scramble.h"




/**
 * The axes the cube's layers can rotate around
 * Values don't matter, we just need to group layers by axis
 */
typedef enum Axis
{
	AXIS_1, AXIS_2, AXIS_3
} Axis;




void init_random(void)
{
	srand(time(NULL));
}


size_t count_occurrences(char const * haystack, char needle)
{
	size_t count = 0;

	for (int index = 0; haystack[index] != '\0'; index++)
	{
		if (haystack[index] == needle)
			count++;
	}

	return count;
}


/**
 * Returns the next move after the one pointed by [scramble]
 *
 * @param scramble - the scramble to get the next move from
 *
 * @return - the next move in the scramble, NULL if none
 */
static char const * get_next_move(char const * scramble)
{
	char const * next_move;

	if (scramble == NULL)
		return NULL;

	next_move = strchr(scramble + 1, ' ');
	if (next_move == NULL)
		return NULL;

	return next_move + 1;
}


char const * find_repeated_moves(char const * scramble)
{
	char const * previous_move = scramble;

	while ((scramble = get_next_move(scramble)) != NULL)
	{
		if (previous_move[0] == scramble[0])
			return previous_move;

		previous_move = scramble;
	}

	return NULL;
}


/**
 * Returns the axis of the given [move]
 *
 * @param move - the move to get the axis from
 *
 * @return - the axis of the [move], or -1 if unknown move
 */
static Axis get_move_axis(char move)
{
	if ((move == 'L') || (move == 'M') || (move == 'R'))
		return AXIS_1;
	if ((move == 'U') || (move == 'E') || (move == 'D'))
		return AXIS_2;
	if ((move == 'F') || (move == 'S') || (move == 'B'))
		return AXIS_3;

	fprintf(stderr, "WARNING: get_move_axis(): unknown move %c\n", move);

	return -1;
}


char const * find_repeated_axis(char const * scramble)
{
	char const * current_move = scramble;
	Axis current_axis = get_move_axis(* current_move);
	char const * next_move;
	Axis next_axis;

	while ((next_move = get_next_move(current_move)) != NULL)
	{
		next_axis = get_move_axis(* next_move);
		if (current_axis == next_axis)
			return current_move;

		current_move = next_move;
		current_axis = next_axis;
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


char * duplicate_string(char const * string)
{
	char * copy;

	if (string == NULL)
		return NULL;

	copy = malloc(strlen(string) + 1);
	if (copy != NULL)
		strcpy(copy, string);

	return copy;
}
