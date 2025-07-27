
#ifndef RUBIKS_MOVES_HEADER
#define RUBIKS_MOVES_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IMPORTANT_RETURN
#define IMPORTANT_RETURN
#endif




/**
 * Generates a scramble sequence, guaranteed to contain no
 * 	repetitions (ie. <F F> or <R2 R'>)
 *
 * @param length - the length of the sequence to generate
 *
 * @return char * - the generated sequence, or NULL if any allocation failed
 */
IMPORTANT_RETURN char * rubiks_generate_scramble(size_t length);




#ifdef __cplusplus
}
#endif

#endif /* RUBIKS_MOVES_HEADER */
