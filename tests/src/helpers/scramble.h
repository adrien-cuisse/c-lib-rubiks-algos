
#ifndef RUBIKS_SCRAMBLE_HELPERS_HEADER
#define RUBIKS_SCRAMBLE_HELPERS_HEADER

#include <stddef.h>




/**
 * Inits the random generator
 */
void init_random(void);


/**
 * Counts the number of occurrences of [needle] in [haystack]
 *
 * @param haystack - the string to find [needle] in
 *
 * @param needle - the character to find
 *
 * @return size_t - how many times [needle] was found in [haystack]
 */
size_t count_occurences(char const * haystack, char needle);


/**
 * Checks if the [scramble] contains twice the same move in a row,
 * 	eg. <U U>, <L' L>, <F F2>, <R2 R'>, etc.
 *
 * @param scramble - the scramble to check
 *
 * @return char * - the position of the first repeating move, NULL if none
 */
char const * find_repeated_moves(char const * scramble);


/**
 * Checks if the [scramble] contains consecutive moves on the same axis
 * eg. <[R L] U>, <F' [U2 E2] L' S>, <D R' U' [M M2] E' R S'>, etc.
 *
 * @param scramble - the scramble to check
 *
 * @return - the first repeated axis found, NULL if none
 */
char const * find_repeated_axis(char const * scramble);


/**
 * Returns the first invalid move of [scramble], ie. not in [valid_moves]
 *
 * @param scramble - the scramble to search in, will be overwritten (strtok)
 *
 * @param valid_moves - the array of valid moves, if current move in [scramble]
 * 	is not in this array, it will be returned as invalid
 * 	this array must be NULL-terminated
 *
 * @return char * - the first invalid move, as a single token
 */
char * find_invalid_move(char * scramble, char const ** valid_moves);




#endif /* RUBIKS_SCRAMBLE_HELPERS_HEADER */
