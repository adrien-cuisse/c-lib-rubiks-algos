
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <criterion/criterion.h>
#include <criterion/parameterized.h>

#include "../../include/rubiks_moves.h"

#include "helpers/scramble.h"


/**
 * Some tests need a very long sequence to check some specs, it mitigates the
 * 	random nature of the generation
 */
#define BIG_SIZE 8192


/**
 * Scramble tests rely on some helpers to be more concise, better make sure
 * 	they still work when modified
 */
// #define CHECK_HELPERS




/**
 * Every symbol that may appear in a scramble sequence
 */
static char valid_symbols[] = "LRUDFB EMS'2";


/**
 * All possible moves for a scramble
 */
static char const * valid_moves[] =
{
	"L",  "R",  "U",  "D",  "F",  "B",  "E",  "M",  "S",
	"L'", "R'", "U'", "D'", "F'", "B'", "E'", "M'", "S'",
	"L2", "R2", "U2", "D2", "F2", "B2", "E2", "M2", "S2",

	NULL /* sentinel */
};




/* Init random generator before running any test */
TestSuite(scramble, .init = init_random);


Test(scramble, returns_null_on_invalid_size)
{
	// given
	size_t invalid_size = 0;

	// when
	char * scramble = rubiks_generate_scramble(invalid_size);

	// then
	cr_assert_null(scramble, "scramble of length 0 makes no sense");
}


Test(scramble, scramble_is_only_made_of_valid_characters)
{
	// given
	size_t size = 1;

	// given
	char * scramble = rubiks_generate_scramble(size);

	// then
	size_t invalid_symbol_index = strspn(scramble, valid_symbols);
	cr_assert_eq(
		invalid_symbol_index,
		strlen(scramble),
		"invalid symbol [%c] in %s",
		scramble[invalid_symbol_index],
		scramble);
}


ParameterizedTestParameters(scramble, moves_are_space_separated)
{
	static size_t sizes[] = { 2, BIG_SIZE };

    return cr_make_param_array(size_t, sizes, 2);
}


ParameterizedTest(size_t * size, scramble, moves_are_space_separated)
{
	// given: a scramble size
	size_t expected_spaces_count = * size - 1;

	// when: generating the scramble and checking separations
	char * scramble = rubiks_generate_scramble(* size);
	size_t actual_spaces_count = count_occurences(scramble, ' ');

	// then: it should be 1 between each move
	cr_assert_eq(
		expected_spaces_count,
		actual_spaces_count,
		"expected scramble to contain %ld spaces, found %ld",
		expected_spaces_count,
		actual_spaces_count);
}


Test(scramble, doesnt_contain_same_move_twice_in_a_row)
{
	// given
	size_t big_size = BIG_SIZE;

	// when
	char * scramble = rubiks_generate_scramble(big_size);
	char const * repetition = find_repeated_moves(scramble);

	// then
	cr_assert_null(
		repetition,
		"the same move appears twice in a row at [%s] in [%s]",
		repetition,
		scramble);
}


Test(scramble, scramble_is_only_made_of_valid_moves)
{
	// given
	size_t big_size = BIG_SIZE;

	// when
	char * scramble = rubiks_generate_scramble(big_size);
	char * first_invalid_move = find_invalid_move(
		strdup(scramble),
		valid_moves);

	// then
	cr_assert_null(
		first_invalid_move,
		"invalid move [%s] found in [%s]",
		first_invalid_move,
		scramble);
}




#ifdef CHECK_HELPERS

/**
 * Called by Criterion, if specified in cr_make_param_array()
 * Deallocates strings which were allocated for a parameterized test
 * The array itself is NOT deallocated
 *
 * @param crp - provided by Criterion
 */
void free_strings(struct criterion_test_params * crp)
{
    char ** strings = (char **) crp->params;

	for (size_t index = 0; index < crp->length; index++)
        cr_free(strings[index]);
}


/**
 * Duplicates a string to be used in a parameterized test
 * Strings from libc's malloc() are unavailable in sub-processes because
 * 	of ASLR
 *
 * @param string - the string to duplicate
 *
 * @return char * - the copy of the [string]
 */
char * cr_strdup(char const * string)
{
    char * copy = cr_malloc(strlen(string) + 1);
	if (copy != NULL)
		strcpy(copy, string);


	return copy;
}


ParameterizedTestParameters(scramble, check_finds_repetition)
{
	static char * params[36];

	/* only letters */
	params[0] = cr_strdup("U U"); /* only repetition */
	params[1] = cr_strdup("F F R"); /* at the beginning */
	params[2] = cr_strdup("L D D"); /* at the end */
	params[3] = cr_strdup("F U U L"); /* in the middle */

	/* with quotes */
	params[4] = cr_strdup("U' U'"); /* only repetition */
	params[5] = cr_strdup("F' F' R"); /* at the beginning */
	params[6] = cr_strdup("L D' D'"); /* at the end */
	params[7] = cr_strdup("F U' U' L"); /* in the middle */

	/* withs 2's */
	params[8] = cr_strdup("U2 U2"); /* only repetition */
	params[9] = cr_strdup("F2 F2 R"); /* at the beginning */
	params[10] = cr_strdup("L2 L2 D"); /* at the end */
	params[11] = cr_strdup("F U2 U2 L"); /* in the middle */

	/* combinations of simple and quotes/2's */
	params[12] = cr_strdup("U U'"); /* only repetition */
	params[13] = cr_strdup("U U2"); /* only repetition */
	params[14] = cr_strdup("U' U"); /* only repetition */
	params[15] = cr_strdup("U' U2"); /* only repetition */
	params[16] = cr_strdup("U2 U"); /* only repetition */
	params[17] = cr_strdup("U2 U'"); /* only repetition */
	params[18] = cr_strdup("U U' D2"); /* at the beginning */
	params[19] = cr_strdup("U U2 R"); /* at the beginning */
	params[20] = cr_strdup("U' U L"); /* at the beginning */
	params[21] = cr_strdup("U' U2 E"); /* at the beginning */
	params[22] = cr_strdup("U2 U M"); /* at the beginning */
	params[23] = cr_strdup("U2 U' S"); /* at the beginning */
	params[24] = cr_strdup("D2 U U'"); /* at the end */
	params[25] = cr_strdup("R U U2"); /* at the end */
	params[26] = cr_strdup("L U' U"); /* at the end */
	params[27] = cr_strdup("E U' U2"); /* at the end */
	params[28] = cr_strdup("M U2 U"); /* at the end */
	params[29] = cr_strdup("S U2 U'"); /* at the end */
	params[30] = cr_strdup("F U U' S"); /* in the middle */
	params[31] = cr_strdup("R U U2 M"); /* in the middle */
	params[32] = cr_strdup("L U' U E"); /* in the middle */
	params[33] = cr_strdup("E U' U2 L"); /* in the middle */
	params[34] = cr_strdup("M U2 U R"); /* in the middle */
	params[35] = cr_strdup("S U2 U' F"); /* in the middle */

    return cr_make_param_array(char *, params, 36, free_strings);
}


ParameterizedTest(char ** bad_scramble, scramble, check_finds_repetition)
{
    // given: a scramble with a repetition

	// when: checking for repetitions
	char const * repetition = find_repeated_moves(* bad_scramble);

	// then they should be found
	cr_assert_not_null(
		repetition,
		"repetition not found, expected [%s] in [%s]",
		repetition,
		* bad_scramble);
}


ParameterizedTestParameters(scramble, check_finds_invalid_move)
{
	static char * params[23];

	/* complete junk */
	params[0] = cr_strdup("not even a scramble");

	/* invalid symbol */
	params[1] = cr_strdup("a B L");
	params[2] = cr_strdup("B' c L2");
	params[3] = cr_strdup("L R g");

	/* repeated move */
	params[4] = cr_strdup("LL R U2");
	params[5] = cr_strdup("L' B'B' R");
	params[6] = cr_strdup("L' R2 F2F2");

	/* not properly delimited */
	params[7] = cr_strdup("BD' U' R'");
	params[8] = cr_strdup("L' FD' F'");
	params[9] = cr_strdup("U' F' D'L2");

	/* standalone modifiers */
	params[10] = cr_strdup("' L F2");
	params[11] = cr_strdup("2 D' L'");
	params[12] = cr_strdup("U ' R");
	params[13] = cr_strdup("R' 2 U");
	params[14] = cr_strdup("F L' '");
	params[15] = cr_strdup("B2 U2 2");

	/* several modifiers */
	params[16] = cr_strdup("D'2 L2 F'");
	params[17] = cr_strdup("F2 D2' U");
	params[18] = cr_strdup("R' D R22");
	params[19] = cr_strdup("R' D R''");

	/* prefixed modifiers */
	params[20] = cr_strdup("'R D' F");
	params[21] = cr_strdup("U' ''L R'");
	params[22] = cr_strdup("L2 U 2'B");

    return cr_make_param_array(char *, params, 23, free_strings);
}


ParameterizedTest(char ** invalid_scramble, scramble, check_finds_invalid_move)
{
    // given: a scramble with invalid moves

	// when: trying to find them
	char const * invalid_move = find_invalid_move(
		* invalid_scramble,
		valid_moves);

	// then they should be found
	cr_assert_not_null(
		invalid_move,
		"invalid move not detected, expected [%s] in [%s]",
		invalid_move,
		* invalid_scramble);
}

#endif /* CHECK_FIND_REPETITION */
