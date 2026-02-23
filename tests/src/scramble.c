
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
 * Parameterized test argument for scrambles containing repetitions
 */
struct scramble_repetition
{
	/**
	 * The scramble to check
	 */
	char * scramble;

	/**
	 * The repetition inside the scramble to include in the error message
	 */
	char * repetition;
};




/**
 * Called by Criterion, if specified in cr_make_param_array()
 * Deallocates strings which were allocated for a parameterized test
 * The array itself is NOT deallocated
 *
 * @param crp - provided by Criterion
 */
static void free_scramble_repetition_params(struct criterion_test_params * crp)
{
	struct scramble_repetition * params = crp->params;

	for (size_t index = 0; index < crp->length; index++)
	{
		cr_free(params[index].scramble);
		cr_free(params[index].repetition);
	}
}


/**
 * Called by Criterion, if specified in cr_make_param_array()
 * Deallocates strings which were allocated for a parameterized test
 * The array itself is NOT deallocated
 *
 * @param crp - provided by Criterion
 */
static void free_strings(struct criterion_test_params * crp)
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
 * @return char * - the copy of the string
 */
static char * cr_strdup(char const * string)
{
	char * copy = cr_malloc(strlen(string) + 1);
	if (copy != NULL)
		strcpy(copy, string);

	return copy;
}


ParameterizedTestParameters(scramble, check_finds_repetition)
{
	static char * params[19];

	/* only simple repetitions */
	params[0] = cr_strdup("U U");
	params[1] = cr_strdup("F F R");
	params[2] = cr_strdup("L D D");
	params[3] = cr_strdup("F U U L");

	/* with quotes */
	params[4] = cr_strdup("U' U'");
	params[5] = cr_strdup("F' F' R");
	params[6] = cr_strdup("L D' D'");
	params[7] = cr_strdup("F U' U' L");

	/* withs 2's */
	params[8] = cr_strdup("U2 U2");
	params[9] = cr_strdup("F2 F2 R");
	params[10] = cr_strdup("L D2 D2");
	params[11] = cr_strdup("F U2 U2 L");

	/* combinations of simple and modifiers */
	params[12] = cr_strdup("U U'");
	params[13] = cr_strdup("R R2");
	params[14] = cr_strdup("B' B");
	params[15] = cr_strdup("D' D2");
	params[16] = cr_strdup("L2 L");
	params[17] = cr_strdup("F2 F'");
	params[18] = cr_strdup("L' L B2");
	params[18] = cr_strdup("D' R R'");
	params[18] = cr_strdup("D' F2 F B");

	return cr_make_param_array(char *, params, 19, free_strings);
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


ParameterizedTestParameters(scramble, check_finds_repeated_axis)
{
	static struct scramble_repetition params[19];

	/* only repeated axis, no modifiers */
	params[0] = (struct scramble_repetition) { cr_strdup("L M"), cr_strdup("L M") };
	params[1] = (struct scramble_repetition) { cr_strdup("M R"), cr_strdup("M R") };
	params[2] = (struct scramble_repetition) { cr_strdup("L R"), cr_strdup("L R") };
	params[3] = (struct scramble_repetition) { cr_strdup("U E"), cr_strdup("U E") };
	params[4] = (struct scramble_repetition) { cr_strdup("E D"), cr_strdup("E D") };
	params[5] = (struct scramble_repetition) { cr_strdup("U D"), cr_strdup("U D") };
	params[6] = (struct scramble_repetition) { cr_strdup("F S"), cr_strdup("F S") };
	params[7] = (struct scramble_repetition) { cr_strdup("S B"), cr_strdup("S B") };
	params[8] = (struct scramble_repetition) { cr_strdup("F B"), cr_strdup("F B") };

	/* only repeated axis, with modifiers */
	params[9] = (struct scramble_repetition) { cr_strdup("L' R'"), cr_strdup("L' R'") };
	params[10] = (struct scramble_repetition) { cr_strdup("U2 D2"), cr_strdup("U2 D2") };
	params[11] = (struct scramble_repetition) { cr_strdup("F' B2"), cr_strdup("F' B2") };
	params[12] = (struct scramble_repetition) { cr_strdup("L2 M'"), cr_strdup("L2 M'") };

	/* repeating axis is at the beginning */
	params[13] = (struct scramble_repetition) { cr_strdup("D U M B"), cr_strdup("D U") };
	params[14] = (struct scramble_repetition) { cr_strdup("D' E2 F2 E R'"), cr_strdup("D' E2") };

	/* repeating axis is at the end */
	params[15] = (struct scramble_repetition) { cr_strdup("R E D"), cr_strdup("E D") };
	params[16] = (struct scramble_repetition) { cr_strdup("R' E2 D"), cr_strdup("E2 D") };

	/* repeating axis is in the middle */
	params[17] = (struct scramble_repetition) { cr_strdup("M E D S"), cr_strdup("E D") };
	params[18] = (struct scramble_repetition) { cr_strdup("M U2 R' D' E2 R2"), cr_strdup("D' E2") };

	return cr_make_param_array(
		struct scramble_repetition,
		params,
		19,
		free_scramble_repetition_params);
}


ParameterizedTest(
	struct scramble_repetition * params,
	scramble,
	check_finds_repeated_axis)
{
	// given: a scramble with consecutive repeating axis

	// when: trying to find them
	char const * repeated_axis = find_repeated_axis(params->scramble);

	// then they should be found
	cr_assert_not_null(
		repeated_axis,
		"repeated axis not found, expected [%s] in [%s]",
		params->repetition,
		params->scramble);
}

#endif /* CHECK_HELPERS */
