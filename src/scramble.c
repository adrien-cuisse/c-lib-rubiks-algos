
#include <stdlib.h>

#include "../include/rubiks_algos.h"




/**
 * The modifiers which can be applied to a move
 */
typedef enum Modifier
{
	/**
	 * Plain layer rotation
	 */
	NO_MODIFIER = 0x0,

	/**
	 * Rotate the layer in opposite direction
	 */
	REVERSE_MODIFIER = 0x1,

	/**
	 * Rotate the layer twice
	 */
	DOUBLE_MODIFIER = 0x2,

	/**
	 * Bit-mask to extract the modifier from a move
	 */
	MODIFIER_MASK = 0x3
} Modifier;


/**
 * The 3 orthogonal axes the layers can rotate around
 */
typedef enum Axis
{
	X_AXIS = 0x4,
	Y_AXIS = 0x8,
	Z_AXIS = 0x10,

	/**
	 * Bit-mask to extract the axis from a move
	 */
	AXIS_MASK = 0x1C
} Axis;


/**
 * The 9 layers composing the cube, the corresponding axis in embedded inside
 */
typedef enum Layer
{
	LEFT_LAYER = 0x20 | X_AXIS,
	MIDDLE_LAYER = 0x40 | X_AXIS,
	RIGHT_LAYER = 0x80 | X_AXIS,

	TOP_LAYER = 0x100 | Y_AXIS,
	EQUATOR_LAYER = 0x200 | Y_AXIS,
	BOTTOM_LAYER = 0x400 | Y_AXIS,

	FRONT_LAYER = 0x800 | Z_AXIS,
	STANDING_LAYER = 0x1000 | Z_AXIS,
	BACK_LAYER = 0x2000 | Z_AXIS,

	/* If USE_WIDE_MOVES option is enabled */
	LEFT_LAYERS = LEFT_LAYER | MIDDLE_LAYER,
	RIGHT_LAYERS = RIGHT_LAYER | MIDDLE_LAYER,
	TOP_LAYERS = TOP_LAYER | EQUATOR_LAYER,
	BOTTOM_LAYERS = BOTTOM_LAYER | EQUATOR_LAYER,
	FRONT_LAYERS = FRONT_LAYER | STANDING_LAYER,
	BACK_LAYERS = BACK_LAYER | STANDING_LAYER,

	LAYER_MASK = 0x3FE0 | AXIS_MASK
} Layer;


/**
 * At least 16 bits
 */
typedef unsigned int Move;




/**
 * Available layers when no options enabled
 */
static Layer base_layers[] =
{
	LEFT_LAYER, MIDDLE_LAYER, RIGHT_LAYER,
	TOP_LAYER, EQUATOR_LAYER, BOTTOM_LAYER,
	FRONT_LAYER, STANDING_LAYER, BACK_LAYER,
};


/**
 * Available layers when USE_WIDE_MOVES enabled
 */
static Layer wide_layers[] =
{
	LEFT_LAYER, MIDDLE_LAYER, RIGHT_LAYER,
	TOP_LAYER, EQUATOR_LAYER, BOTTOM_LAYER,
	FRONT_LAYER, STANDING_LAYER, BACK_LAYER,
	LEFT_LAYERS, RIGHT_LAYERS,
	TOP_LAYERS, BOTTOM_LAYERS,
	FRONT_LAYERS, BACK_LAYERS,
};




/**
 * Counts the number of bits set to 1 in [number]
 *
 * @param number - the number to count bits from
 *
 * @return - the number of bits set
 */
static size_t count_bits_set(unsigned long number)
{
	size_t count;

	for (count = 0; number > 0; count++)
		number = number & (number - 1);

	return count;
}


/**
 * Checks whether or not the given move is made of several layers
 *
 * @param move - the move to check
 *
 * @return - 1 if the move contains at least 2 layers, 0 otherwise
 */
static int is_wide_move(Move move)
{
	return count_bits_set(move & LAYER_MASK) > 2;
}


/**
 * Picks a random base layer, wide moves are excluded
 *
 * @return - a random base layer
 */
static Layer random_base_layer(void)
{
	return base_layers[rand() % 9];
}


/**
 * Picks a random layer, wide moves are included
 *
 * @return - a random layer
 */
static Layer random_extended_layer(void)
{
	return wide_layers[rand() % 15];
}


/**
 * Picks a random modifier
 *
 * @return - a random modifier
 */
static Modifier random_modifier(void)
{
	return rand() % MODIFIER_MASK;
}


/**
 * Picks a random base move, which may contain a modifier
 *
 * @return - a random base move
 */
static Move generate_random_base_move(void)
{
	return random_base_layer() | random_modifier();
}


/**
 * Picks a random move, which may contain a modifier
 *
 * @return - a random move, may be a wide one
 */
static Move generate_random_wide_move(void)
{
	return random_extended_layer() | random_modifier();
}


/**
 * Picks a random base move, without restriction
 *
 * @return - a random base move
 */
static Move generate_first_random_base_move(void)
{
	return generate_random_base_move();
}


/**
 * Picks a random move, without restriction
 *
 * @return - a random move, may be a wide one
 */
static Move generate_first_random_wide_move(void)
{
	return generate_random_wide_move();
}


/**
 * Picks a base move on another axis than the given one
 *
 * @param excluded_axis - the axis to exclude from picking
 *
 * @return - a base move, guaranteed to be on a new axis
 */
static Move generate_next_random_base_move(Axis excluded_axis)
{
	Move next_move;

	do next_move = generate_random_base_move();
	while ((next_move & AXIS_MASK) == excluded_axis);

	return next_move;
}


/**
 * Picks a move on another axis than the given one, may be a wide move
 *
 * @param excluded_axis - the axis to exclude from picking
 *
 * @return - a move, guaranteed to be on a new axis
 */
static Move generate_next_random_wide_move(Axis excluded_axis)
{
	Move next_move;

	do next_move = generate_random_wide_move();
	while ((next_move & AXIS_MASK) == excluded_axis);

	return next_move;
}


/**
 * Generates every base move of the scramble
 *
 * @param moves - the buffer to insert generated moved to
 *
 * @param count - the number of moves to generate
 */
static void generate_random_base_moves(Move moves[], size_t count)
{
	size_t added_moves = 0;

	moves[added_moves++] = generate_first_random_base_move();

	while (added_moves < count)
	{
		Axis previous_axis = moves[added_moves - 1] & AXIS_MASK;
		moves[added_moves++] = generate_next_random_base_move(previous_axis);
	}
}


/**
 * Generates every move of the scramble, which may contain wide moves
 *
 * @param moves - the buffer to insert generated moved to
 *
 * @param count - the number of moves to generate
 */
static void generate_random_wide_moves(Move moves[], size_t count)
{
	size_t added_moves = 0;

	moves[added_moves++] = generate_first_random_wide_move();

	while (added_moves < count)
	{
		Axis previous_axis = moves[added_moves - 1] & AXIS_MASK;
		moves[added_moves++] = generate_next_random_wide_move(previous_axis);
	}
}


/**
 * Computes the number of bytes requires to store the given move, using
 * singmaster notation
 *
 * @param move - the move to compute the length for
 *
 * @param index - the index of the move
 *
 * @return - the number of bytes requires to write the given move
 */
static size_t singmaster_notation_move_length(Move move, size_t index)
{
	size_t length = 1; /* 1 character for the layer */

	/* 1 character for spacing between moves */
	if (index > 0)
		length++;

	/* 1 character for the modifier, if any */
	if ((move & MODIFIER_MASK) != NO_MODIFIER)
		length++;

	return length;
}


/**
 * Computes the length of the string required to store the scramble using
 * singmaster notation
 *
 * @param moves - the moves composing the scramble
 *
 * @param count - the number of moves in the scramble
 *
 * @return - the required length of the string, without NULL-terminating byte
 */
static size_t compute_singmaster_scramble_string_length(Move const moves[], size_t count)
{
	size_t string_length = 0;
	size_t index;

	for (index = 0; index < count; index++)
		string_length += singmaster_notation_move_length(moves[index], index);

	return string_length;
}


/**
 * Computes the length of the string required to store the scramble using
 * WCA notation
 *
 * @param moves - the moves composing the scramble
 *
 * @param count - the number of moves in the scramble
 *
 * @return - the required length of the string, without NULL-terminating byte
 */
static size_t compute_wca_scramble_string_length(Move const moves[], size_t count)
{
	size_t string_length = 0;
	size_t index;

	for (index = 0; index < count; index++)
	{
		string_length += singmaster_notation_move_length(moves[index], index);

		/* 1 character for the 'w' if wide move */
		if (is_wide_move(moves[index]))
			string_length++;
	}

	return string_length;
}


/**
 * Matches the base layer to a symbol
 *
 * @param layer - the layer to get the symbol for
 *
 * @return - the symbol of the layer
 */
static char base_layer_symbol(Layer layer)
{
	switch (layer)
	{
		case LEFT_LAYER: return 'L';
		case MIDDLE_LAYER: return 'M';
		case RIGHT_LAYER: return 'R';
		case TOP_LAYER: return 'U';
		case EQUATOR_LAYER: return 'E';
		case BOTTOM_LAYER: return 'D';
		case FRONT_LAYER: return 'F';
		case STANDING_LAYER: return 'S';
		case BACK_LAYER: return 'B';
		default: return '?';
	}
}


/**
 * Matches the wide layer to a symbol in singmaster notation
 *
 * @param layer - the layer to get the symbol for
 *
 * @return - the symbol of the layer
 */
static char singmaster_layer_symbol(Layer layer)
{
	switch (layer)
	{
		case LEFT_LAYERS: return 'l';
		case RIGHT_LAYERS: return 'r';
		case TOP_LAYERS: return 'u';
		case BOTTOM_LAYERS: return 'd';
		case FRONT_LAYERS: return 'f';
		case BACK_LAYERS: return 'b';
		default: return base_layer_symbol(layer);
	}
}


/**
 * Matches the wide layer to a symbol in WCA notation
 *
 * @param layer - the layer to get the symbol for
 *
 * @return - the symbol of the layer
 */
static char wca_layer_symbol(Layer layer)
{
	switch (layer)
	{
		case LEFT_LAYERS: return 'L';
		case RIGHT_LAYERS: return 'R';
		case TOP_LAYERS: return 'U';
		case BOTTOM_LAYERS: return 'D';
		case FRONT_LAYERS: return 'F';
		case BACK_LAYERS: return 'B';
		default: return base_layer_symbol(layer);
	}
}


/**
 * Returns the symbol of the given modifier
 *
 * @param modifier - the modifier to get the symbol for
 *
 * @return - the symbol to write the modifier, or '?' if unknown modifier,
 * or '?' if NO_MODIFIER
 */
static char modifier_symbol(Modifier modifier)
{
	if (modifier == REVERSE_MODIFIER)
		return '\'';
	if (modifier == DOUBLE_MODIFIER)
		return '2';

	return '?';
}


/**
 * Writes a move at the beginning of the scramble string, using singmaster
 * notation
 *
 * @param move - the move to write
 *
 * @param scramble - the scramble string to write to
 *
 * @return - the number of writen bytes
 */
static size_t write_singmaster_move(Move move, char * scramble)
{
	size_t writen_bytes = 0;
	Layer layer_bits = move & LAYER_MASK;

	* (scramble + writen_bytes++) = singmaster_layer_symbol(layer_bits);

	if ((move & MODIFIER_MASK) != NO_MODIFIER)
		* (scramble + writen_bytes++) = modifier_symbol(move & MODIFIER_MASK);

	return writen_bytes;
}


/**
 * Writes a move at the beginning of the scramble string, using WCA notation
 *
 * @param move - the move to write
 *
 * @param scramble - the scramble string to write to
 *
 * @return - the number of writen bytes
 */
static size_t write_wca_move(Move move, char * scramble)
{
	size_t writen_bytes = 0;

	* (scramble + writen_bytes++) = wca_layer_symbol(move & LAYER_MASK);

	if ((is_wide_move(move)))
		* (scramble + writen_bytes++) = 'w';

	if ((move & MODIFIER_MASK) != NO_MODIFIER)
		* (scramble + writen_bytes++) = modifier_symbol(move & MODIFIER_MASK);

	return writen_bytes;
}


/**
 * Writes the scramble in the given string, using singmaster notation
 *
 * @param moves - the moves composing the scramble
 *
 * @param count - the number of moves in the scramble
 *
 * @param scramble - the writen scramble
 */
static void write_singmaster_scramble(Move const moves[], size_t count, char * scramble)
{
	size_t move_index;

	scramble += write_singmaster_move(moves[0], scramble);

	for (move_index = 1; move_index < count; move_index++)
	{
		* scramble++ = ' ';
		scramble += write_singmaster_move(moves[move_index], scramble);
	}

	* scramble = '\0';
}


/**
 * Writes the scramble in the given string, using WCA notation
 *
 * @param moves - the moves composing the scramble
 *
 * @param count - the number of moves in the scramble
 *
 * @param scramble - the writen scramble
 */
static void write_wca_scramble(Move const moves[], size_t count, char * scramble)
{
	size_t move_index;

	scramble += write_wca_move(moves[0], scramble);

	for (move_index = 1; move_index < count; move_index++)
	{
		* scramble++ = ' ';
		scramble += write_wca_move(moves[move_index], scramble);
	}

	* scramble = '\0';
}


/**
 * Creates a scramble string from the given moves, using singmaster notation
 * The caller is in charge of the memory
 *
 * @param moves - the moves composing the scramble
 *
 * @param count - the number of moves in the scramble
 *
 * @return - the created scramble
 */
static char * create_singmaster_scramble_string(Move const moves[], size_t count)
{
	size_t string_length = compute_singmaster_scramble_string_length(moves, count);
	char * scramble = malloc(string_length + 1);

	if (scramble == NULL)
		return NULL;

	write_singmaster_scramble(moves, count, scramble);

	return scramble;
}


/**
 * Creates a scramble string from the given moves, using WCA notation
 * The caller is in charge of the memory
 *
 * @param moves - the moves composing the scramble
 *
 * @param count - the number of moves in the scramble
 *
 * @return - the created scramble
 */
static char * create_wca_scramble_string(Move const moves[], size_t count)
{
	size_t string_length = compute_wca_scramble_string_length(moves, count);
	char * scramble = malloc(string_length + 1);

	if (scramble == NULL)
		return NULL;

	write_wca_scramble(moves, count, scramble);

	return scramble;
}


char * rubiks_generate_scramble(size_t length, RubiksScrambleOption flags)
{
	Move * moves;
	char * scramble;

	if (length == 0)
		return NULL;

	moves = malloc(sizeof(Move) * length);
	if (moves == NULL)
		return NULL;

	if (flags & USE_WIDE_MOVES)
		generate_random_wide_moves(moves, length);
	else
		generate_random_base_moves(moves, length);

	if (flags & USE_WCA_NOTATION)
		scramble = create_wca_scramble_string(moves, length);
	else
		scramble = create_singmaster_scramble_string(moves, length);

	free(moves);

	return scramble;
}
