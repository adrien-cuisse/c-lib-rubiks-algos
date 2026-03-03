
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
enum rba_option
{
	NO_OPTIONS = 0,

	/**
	 * Wide moves apply on 2 layers instead of 1:
	 * 	- for left/right layer, middle layer is also rotated,
	 * 	- for top/bottom layer, equator layer is also rotated,
	 * 	- for front/back layer, standing layer is also rotated
	 * 	If USE_WCA_NOTATION is not set, defaults to singmaster notation,
	 * 	which means wide moves are lowercase layers,
	 * 	eg., [r'] = right layers anticlockwise
	 */
	USE_WIDE_MOVES = 1,

	/**
	 * A 'w' is appended between the layer and the modifier to indicate
	 * a wide move, eg., [Rw'] = right layers anticlockwise
	 * Has no effect when USE_WIDE_MOVES is not set
	 */
	USE_WCA_NOTATION = 2
};




/**
 * Generates a scramble sequence, guaranteed to contain no more than 1 move
 * per axis
 * The caller is in charge of the memory
 *
 * @param length - the length of the sequence to generate
 *
 * @return char * - the generated sequence, or NULL if any allocation failed
 */
IMPORTANT_RETURN char * rba_generate_scramble(
	size_t length,
	enum rba_option flags);




#ifdef __cplusplus
}
#endif

#endif /* RUBIKS_ALGOS_HEADER */
