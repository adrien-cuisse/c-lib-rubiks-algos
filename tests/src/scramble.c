
#include <string.h>

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

	/* sentinel */
	NULL
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
	size_t size = BIG_SIZE;

	// given
	char * scramble = rubiks_generate_scramble(size);

	// then
	size_t valid_length = strspn(scramble, valid_symbols);
	cr_assert_eq(
		valid_length,
		strlen(scramble),
		"invalid symbol [%c] at position %zu in [%s]",
		scramble[valid_length],
		valid_length,
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
	size_t actual_spaces_count = count_occurrences(scramble, ' ');

	// then: it should be 1 between each move
	cr_assert_eq(
		expected_spaces_count,
		actual_spaces_count,
		"expected scramble to contain %ld spaces, found %ld",
		expected_spaces_count,
		actual_spaces_count);
}


Test(scramble, doesnt_contain_consecutive_moves_on_same_layer)
{
	// given
	size_t big_size = BIG_SIZE;

	// when
	char * scramble = rubiks_generate_scramble(big_size);
	char const * repetition = find_repeated_layers(scramble);

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
		scramble,
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
 * Parameterized tests arguments for scrambles
 */
typedef struct ScrambleTestParams
{
	/**
	 * The scramble to test
	 */
	char * scramble;

	/**
	 * The fragment to include in the error message on failure
	 */
	char * error_message_fragment;
} ScrambleTestParams;




/**
 * Called by Criterion, if specified in cr_make_param_array()
 * Deallocates strings which were allocated for a parameterized test
 * The array itself is NOT deallocated
 *
 * @param crp - provided by Criterion
 */
static void free_scramble_tests_params(struct criterion_test_params * crp)
{
	ScrambleTestParams * params = crp->params;

	for (size_t index = 0; index < crp->length; index++)
	{
		cr_free(params[index].scramble);
		cr_free(params[index].error_message_fragment);
	}
}


/**
 * Duplicates a string to be used in a parameterized test
 * Memory from libc malloc() is unavailable in sub-processes because of ASLR
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


ParameterizedTestParameters(scramble, check_finds_layers_repetition)
{
	static ScrambleTestParams params[19];

	/* only simple repetitions */
	params[0] = (ScrambleTestParams) { cr_strdup("U U"), cr_strdup("U U") };
	params[1] = (ScrambleTestParams) { cr_strdup("F F R"), cr_strdup("F F") };
	params[2] = (ScrambleTestParams) { cr_strdup("L D D"), cr_strdup("D D") };
	params[3] = (ScrambleTestParams) { cr_strdup("F U U L"), cr_strdup("U U") };

	/* with quotes */
	params[4] = (ScrambleTestParams) { cr_strdup("U' U'"), cr_strdup("U' U'") };
	params[5] = (ScrambleTestParams) { cr_strdup("F' F' R"), cr_strdup("F' F'") };
	params[6] = (ScrambleTestParams) { cr_strdup("L D' D'"), cr_strdup("D' D'") };
	params[7] = (ScrambleTestParams) { cr_strdup("F U' U' L"), cr_strdup("U' U'") };

	/* withs 2's */
	params[8] = (ScrambleTestParams) { cr_strdup("U2 U2"), cr_strdup("U2 U2") };
	params[9] = (ScrambleTestParams) { cr_strdup("F2 F2 R"), cr_strdup("F2 F2") };
	params[10] = (ScrambleTestParams) { cr_strdup("L D2 D2"), cr_strdup("D2 D2") };
	params[11] = (ScrambleTestParams) { cr_strdup("F U2 U2 L"), cr_strdup("U2 U2") };

	/* combinations of simple and modifiers */
	params[12] = (ScrambleTestParams) { cr_strdup("U U'"), cr_strdup("U U'") };
	params[13] = (ScrambleTestParams) { cr_strdup("R R2"), cr_strdup("R R2") };
	params[14] = (ScrambleTestParams) { cr_strdup("B' B"), cr_strdup("B' B") };
	params[15] = (ScrambleTestParams) { cr_strdup("D' D2"), cr_strdup("D' D2") };
	params[16] = (ScrambleTestParams) { cr_strdup("L2 L"), cr_strdup("L2 L") };
	params[17] = (ScrambleTestParams) { cr_strdup("F2 F'"), cr_strdup("F2 F'") };
	params[18] = (ScrambleTestParams) { cr_strdup("L' L B2"), cr_strdup("L' L") };
	params[18] = (ScrambleTestParams) { cr_strdup("D' R R'"), cr_strdup("R R'") };
	params[18] = (ScrambleTestParams) { cr_strdup("D' F2 F B"), cr_strdup("F2 F") };

	return cr_make_param_array(
		ScrambleTestParams,
		params,
		19,
		free_scramble_tests_params);
}


ParameterizedTest(
	ScrambleTestParams * params,
	scramble,
	check_finds_layers_repetition)
{
	// given: a scramble with a repetition

	// when: checking for repetitions
	char const * repetition = find_repeated_layers(params->scramble);

	// then they should be found
	cr_assert_not_null(
		repetition,
		"repetition not found, expected [%s] in [%s]",
		params->error_message_fragment,
		params->scramble);
}


ParameterizedTestParameters(scramble, check_finds_invalid_move)
{
	static ScrambleTestParams params[23];

	/* complete junk */
	params[0] = (ScrambleTestParams) { cr_strdup("not even a scramble"), cr_strdup("not") };

	/* invalid symbol */
	params[1] = (ScrambleTestParams) { cr_strdup("a B L"), cr_strdup("a") };
	params[2] = (ScrambleTestParams) { cr_strdup("B' c L2"), cr_strdup("c") };
	params[3] = (ScrambleTestParams) { cr_strdup("L R g"), cr_strdup("g") };

	/* repeated move */
	params[4] = (ScrambleTestParams) { cr_strdup("LL R U2"), cr_strdup("LL") };
	params[5] = (ScrambleTestParams) { cr_strdup("L' B'B' R"), cr_strdup("B'B'") };
	params[6] = (ScrambleTestParams) { cr_strdup("L' R2 F2F2"), cr_strdup("F2F2") };

	/* not properly delimited */
	params[7] = (ScrambleTestParams) { cr_strdup("BD' U' R'"), cr_strdup("BD'") };
	params[8] = (ScrambleTestParams) { cr_strdup("L' FD' F'"), cr_strdup("FD'") };
	params[9] = (ScrambleTestParams) { cr_strdup("U' F' D'L2"), cr_strdup("D'L2") };

	/* standalone modifiers */
	params[10] = (ScrambleTestParams) { cr_strdup("' L F2"), cr_strdup("'") };
	params[11] = (ScrambleTestParams) { cr_strdup("2 D' L'"), cr_strdup("2") };
	params[12] = (ScrambleTestParams) { cr_strdup("U ' R"), cr_strdup("'") };
	params[13] = (ScrambleTestParams) { cr_strdup("R' 2 U"), cr_strdup("2") };
	params[14] = (ScrambleTestParams) { cr_strdup("F L' '"), cr_strdup("'") };
	params[15] = (ScrambleTestParams) { cr_strdup("B2 U2 2"), cr_strdup("2") };

	/* several modifiers */
	params[16] = (ScrambleTestParams) { cr_strdup("D'2 L2 F'"), cr_strdup("D'2") };
	params[17] = (ScrambleTestParams) { cr_strdup("F2 D2' U"), cr_strdup("D2'") };
	params[18] = (ScrambleTestParams) { cr_strdup("R' D R22"), cr_strdup("R22") };
	params[19] = (ScrambleTestParams) { cr_strdup("R' D R''"), cr_strdup("R''") };

	/* prefixed modifiers */
	params[20] = (ScrambleTestParams) { cr_strdup("'R D' F"), cr_strdup("'R") };
	params[21] = (ScrambleTestParams) { cr_strdup("U' ''L R'"), cr_strdup("''L") };
	params[22] = (ScrambleTestParams) { cr_strdup("L2 U 2'B"), cr_strdup("2'B") };

	return cr_make_param_array(
		ScrambleTestParams,
		params,
		23,
		free_scramble_tests_params);
}


ParameterizedTest(
	ScrambleTestParams * params,
	scramble,
	check_finds_invalid_move)
{
	// given: a scramble with invalid moves

	// when: trying to find them
	char const * invalid_move = find_invalid_move(
		params->scramble,
		valid_moves);

	// then they should be found
	cr_assert_not_null(
		invalid_move,
		"invalid move not detected, expected [%s] in [%s]",
		params->error_message_fragment,
		params->scramble);
}


ParameterizedTestParameters(scramble, check_finds_repeated_axis)
{
	static ScrambleTestParams params[19];

	/* only repeated axis, no modifiers */
	params[0] = (ScrambleTestParams) { cr_strdup("L M"), cr_strdup("L M") };
	params[1] = (ScrambleTestParams) { cr_strdup("M R"), cr_strdup("M R") };
	params[2] = (ScrambleTestParams) { cr_strdup("L R"), cr_strdup("L R") };
	params[3] = (ScrambleTestParams) { cr_strdup("U E"), cr_strdup("U E") };
	params[4] = (ScrambleTestParams) { cr_strdup("E D"), cr_strdup("E D") };
	params[5] = (ScrambleTestParams) { cr_strdup("U D"), cr_strdup("U D") };
	params[6] = (ScrambleTestParams) { cr_strdup("F S"), cr_strdup("F S") };
	params[7] = (ScrambleTestParams) { cr_strdup("S B"), cr_strdup("S B") };
	params[8] = (ScrambleTestParams) { cr_strdup("F B"), cr_strdup("F B") };

	/* only repeated axis, with modifiers */
	params[9] = (ScrambleTestParams) { cr_strdup("L' R'"), cr_strdup("L' R'") };
	params[10] = (ScrambleTestParams) { cr_strdup("U2 D2"), cr_strdup("U2 D2") };
	params[11] = (ScrambleTestParams) { cr_strdup("F' B2"), cr_strdup("F' B2") };
	params[12] = (ScrambleTestParams) { cr_strdup("L2 M'"), cr_strdup("L2 M'") };

	/* repeating axis is at the beginning */
	params[13] = (ScrambleTestParams) { cr_strdup("D U M B"), cr_strdup("D U") };
	params[14] = (ScrambleTestParams) { cr_strdup("D' E2 F2 E R'"), cr_strdup("D' E2") };

	/* repeating axis is at the end */
	params[15] = (ScrambleTestParams) { cr_strdup("R E D"), cr_strdup("E D") };
	params[16] = (ScrambleTestParams) { cr_strdup("R' E2 D"), cr_strdup("E2 D") };

	/* repeating axis is in the middle */
	params[17] = (ScrambleTestParams) { cr_strdup("M E D S"), cr_strdup("E D") };
	params[18] = (ScrambleTestParams) { cr_strdup("M U2 R' D' E2 R2"), cr_strdup("D' E2") };

	return cr_make_param_array(
		ScrambleTestParams,
		params,
		19,
		free_scramble_tests_params);
}


ParameterizedTest(
	ScrambleTestParams * params,
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
		params->error_message_fragment,
		params->scramble);
}

#endif /* CHECK_HELPERS */
