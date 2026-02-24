
#ifndef RUBIKS_ALGOS_HEADER
#define RUBIKS_ALGOS_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IMPORTANT_RETURN
#define IMPORTANT_RETURN
#endif

#include <stddef.h>




/**
 * The different options available
 */
typedef enum RubiksScrambleOption
{
	NO_OPTIONS = 0,

	/**
	 * Wide moves applies on 2 slayers instead of 1:
	 * 	- for left/right layers, middle layer is also rotated,
	 * 	- for top/bottom layers, equator layer is also rotated,
	 * 	- for front/back layers, standing layer is also rotated
	 */
	USE_WIDE_MOVES = 1
} RubiksScrambleOption;




/**
 * Generates a scramble sequence, guaranteed to contain no
 * 	repetitions (ie. <F F> or <R2 R'>)
 *
 * @param length - the length of the sequence to generate
 *
 * @return char * - the generated sequence, or NULL if any allocation failed
 */
IMPORTANT_RETURN char * rubiks_generate_scramble(
	size_t length,
	RubiksScrambleOption flags);




#ifdef __cplusplus
}
#endif

#endif /* RUBIKS_ALGOS_HEADER */
