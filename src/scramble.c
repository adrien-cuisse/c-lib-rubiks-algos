
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


/**
 * The maximum length for a single move notation, eg. U2
 */
#define MOVE_MAX_LENGTH 2




/**
 * All the valid moves we'll use to generate a scramble
 * Not all of them have the same odds to be picked
 */
static char const * valid_moves[] =
{
    /* base moves should twice the normal rate */
    "L",  "R",  "U",  "D",  "F",  "B",
    "L",  "R",  "U",  "D",  "F",  "B",
    "L'", "R'", "U'", "D'", "F'", "B'",
    "L'", "R'", "U'", "D'", "F'", "B'",

    /* center slices moves are lame, normal rate */
    "M",  "E",  "S",
    "M'", "E'", "S'",

    /* wide moves aren't natural, normal rate */
    "L2", "R2", "U2", "D2", "F2", "B2",
    "M2",  "E2",  "S2",
};
static int valid_moves_count = sizeof(valid_moves) / sizeof(* valid_moves);



/**
 * Chooses a random valid move
 *
 * @return char const * - the chosen valid move
 */
static char const * pick_random_move()
{
	int index = rand() % valid_moves_count;

	return valid_moves[index];
}


/**
 * Checks if apply both moves is like doing nothing at all, eg. :
 * 	- 360° turn: <F2> <F2>
 *  - 0° turn: <L> <L'> or <U'> <U>
 *
 * @param first - the first move to check
 *
 * @param second - the second move to check
 *
 * @return int - 1 if both move cancels, 0 otherwise
 */
static int moves_cancel_each_other(char const * first, char const * second)
{
	if (first[0] != second[0])
		return 0;

	if (first[1] == '2' && second[1] == '2')
		return 1;

	if (first[1] == '\0' && second[1] == '\'')
		return 1;

	if (first[1] == '\'' && second[1] == '\0')
		return 1;

	return 0;
}


/**
 * Picks a random move which is guaranteed to be compatible with the
 * previous one either by combining them, or as a completely new move
 *
 * @param previous_move - the move to pick a new one compatible with
 *
 * @return char * - the picked new move
 */
static char const * pick_compatible_random_move(char const * previous_move)
{
	char const * new_move;

	do new_move = pick_random_move();
	while (moves_cancel_each_other(previous_move, new_move));

	return new_move;
}


/**
 * Stores a copy of [moves] in [moves]
 *
 * @param moves - the array of string to write the [move] in
 *
 * @param move -  the move to write in the array, if it's
 * 	simple (eg. <F> or <R>), it's guaranteed to be long enough to be
 * 	extended at a later point (eg. into <F'> or <R2>)
 *
 * @param index - the index where to write in the array
 *
 * @return int - always returns 1
 */
static int add_move(char ** moves, char const * move, int index)
{
	moves[index] = malloc(MOVE_MAX_LENGTH + 1);
	strcpy(moves[index], move);

	return 1;
}


/**
 * Checks if both moves can NOT be combined into a single one, ie. they don't
 * 	rotate the same slice
 *
 * @param first - the first move to check
 *
 * @param second - the second move to check
 *
 * @return int - 1 if moves can NOT be combined, 0 if they can
 */
static int moves_cannot_be_combined(char const * first, char const * second)
{
	return first[0] != second[0];
}


/**
 * Applies a modifier on the move (eg. <'> or <2>), removing any previous one
 *
 * @param move - the move to apply the modifier to
 *
 * @param symbol - the modifier to apply
 *
 * @return int - always returns 0
 */
static int apply_move_modifier(char * move, char modifier)
{
	move[1] = modifier;
	move[2] = '\0';

	return 0;
}


/**
 * Applies the double move modifier (eg., <F> -> <F2>)
 *
 * @param move - the move to apply the modifier on
 *
 * @return int - always returns 0
 */
static int apply_double_move_modifier(char * move)
{
	return apply_move_modifier(move, '2');
}


/**
 * Applies the reverse move modifier (eg., <R> -> <R'>)
 *
 * @param move - the move to apply the modifier on
 *
 * @return int - always returns 0
 */
static int apply_reverse_move_modifier(char * move)
{
	return apply_move_modifier(move, '\'');
}


/**
 * Removes the modifier on the move (eg. <'> or <2>)
 *
 * @param move - the move to remove the modifier from
 *
 * @return int - always returns 0
 */
static int strip_move_modifier(char * move)
{
	move[1] = '\0';

	return 0;
}


/**
 * Checks if both moves can be combined into a double one
 *
 * @param first - the first move to check the modifier
 *
 * @param second - the second move to check the modifier from
 *
 * @return int - 1 if moves can be combined into a double one, 0 otherwise
 */
static int makes_double_move(char const * first, char const * second)
{
	if (first[1] == '\0' && second[1] == '\0')
		return 1;

	if (first[1] == '\'' && second[1] == '\'')
		return 1;

	return 0;
}


/**
 * Checks if both moves can be combined to a reverse one (they sum up to 270°)
 *
 * @param first - the first move to check the modifier
 *
 * @param second - the second move to check the modifier from
 *
 * @return int - 1 if moves can be combined into a reverse one, 0 otherwise
 */
static int makes_reverse_move(char const * first, char const * second)
{
	if (first[1] == '2' && second[1] == '\0')
		return 1;

	if (first[1] == '\0' && second[1] == '2')
		return 1;

	return 0;
}


/**
 * Checks if the modifiers of both moves are opposite
 * A double move followed by the same reverse move turns the first move into
 * a simple one
 *
 * @param first - the first move to check the modifier from
 *
 * @param second - the second move to check the modifier from
 *
 * @return int - 1 if modifiers cancel each other, 0 otherwise
 */
static int modifiers_cancel_each_other(char const * first, char const * second)
{
	if (first[1] == '\'' && second[1] == '2')
		return 1;

	if (first[1] == '2' && second[1] == '\'')
		return 1;

	return 0;
}


/**
 * Adds a new random move in the array, or combines it with the last one
 * 	if possible
 *
 * @param moves - the array to add a new move in
 *
 * @param moves_count - how many moves are currently in the array
 *
 * @return int - 1 if a new move was stored in the array, 0 if the last one
 * 	was simply updated (ie., last move and new one could be combined)
 */
static int add_random_move(char ** moves, size_t moves_count)
{
	char * previous_move;
	char const * new_move;

	if (moves_count == 0)
		return add_move(moves, pick_random_move(), 0);

	previous_move = moves[moves_count - 1];
	new_move = pick_compatible_random_move(previous_move);

	if (moves_cannot_be_combined(previous_move, new_move))
		return add_move(moves, new_move, moves_count);

	if (makes_double_move(previous_move, new_move))
		return apply_double_move_modifier(previous_move);

	if (modifiers_cancel_each_other(previous_move, new_move))
		return strip_move_modifier(previous_move);

	if (makes_reverse_move(previous_move, new_move))
		return apply_reverse_move_modifier(previous_move);

	/* can't be reached, if we handled cases properly */

	return 0;
}


/**
 * Computes the length of the final scramble
 *
 * @param moves - the array of moves that will be in the scramble
 *
 * @param moves_count - the number of moves in the arry
 *
 * @return size_t - the length of the scramble, without NULL-terminating byte
 */
static size_t compute_scramble_length(char ** moves, size_t moves_count)
{
	size_t spaces_count = moves_count - 1;
	size_t moves_length = 0;

	size_t index;
	for (index = 0; index < moves_count; index++)
		moves_length += strlen(moves[index]);

	return moves_length + spaces_count;
}


/**
 * Allocates the buffer to write to complete scramble
 *
 * @param moves - array of moves that will be writen in the scramble
 *
 * @param moves_count - how many moves are in the array
 *
 * @return char * - the allocated buffer, including room for the
 * 	NULL-terminating byte
 */
static char * allocate_scramble(char ** moves, size_t moves_count)
{
	size_t scramble_length = compute_scramble_length(moves, moves_count);

	return malloc(scramble_length + 1);
}


/**
 * Concatenates all moves in the array, making a space-delimited scramble
 * 	sequence
 *
 * @param moves - the array of moves to join
 *
 * @param moves_count - how many moves are in the array
 *
 * @return char * - the concatenated moves
 */
static char * join_moves(char ** moves, size_t moves_count)
{
	size_t scramble_index = 0;
	size_t move_index = 0;
	size_t move_length;

	char * scramble = allocate_scramble(moves, moves_count);
	if (scramble == NULL)
		return NULL;

	move_length = strlen(moves[0]);
	memcpy(scramble, moves[0], move_length);
	scramble_index = move_length;

	for (move_index = 1; move_index < moves_count; move_index++)
	{
		scramble[scramble_index] = ' ';
		move_length = strlen(moves[move_index]);
		memcpy(scramble + scramble_index + 1, moves[move_index], move_length);
		scramble_index += move_length + 1;
	}

	scramble[scramble_index] = '\0';

	return scramble;
}


/**
 * Generates an array of moves, guaranteed to contain no
 * 	repetitions (ie. <F F> or <R2 R'>)
 *
 * @param size - the number of moves to generate
 *
 * @return char ** - the generated moves
 */
static char ** generate_moves(size_t size)
{
    char ** moves;
    size_t added_moves = 0;

    moves = malloc(size * sizeof(* moves));
	if (moves == NULL)
		return NULL;

	while (added_moves < size)
		added_moves += add_random_move(moves, added_moves);

    return moves;
}


/**
 * Deallocates the move and sets it to NULL
 *
 * @param move - the move to delete
 */
static void delete_move(char ** move)
{
	free(* move);
	* move = NULL;
}


/**
 * Deallocates the moves and the array itself, setting everything to NULL
 *
 * @param moves - the array of moves to delete
 *
 * @param size - the number of moves stored in the array
 */
static void delete_moves(char *** moves, size_t size)
{
    size_t index;

    for (index = 0; index < size; index++)
		delete_move(& (* moves)[index]);

    free(* moves);
    * moves = NULL;
}


char * rubiks_generate_scramble(size_t size)
{
	char * scramble;
	char ** moves;

	if (size == 0)
		return NULL;

    moves = generate_moves(size);
	if (moves == NULL)
		return NULL;

	scramble = join_moves(moves, size);
    delete_moves(& moves, size);

	return scramble;
}
