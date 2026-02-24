
#include <stdlib.h>

#include "../include/rubiks_algos.h"




/**
 * The masks to extract group of options
 */
typedef enum OptionMask
{
	/**
	 * The mask to extract the range of moves to use during generation
	 */
	LAYERS_RANGE_MASK = 0x1
} OptionMask;


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
 * Picks a random layer
 *
 * @return - a random layer
 */
static Layer random_layer(RubiksScrambleOption layers_range)
{
	if (layers_range == USE_WIDE_MOVES)
		return wide_layers[rand() % 15];

	return base_layers[rand() % 9];
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
 * Generates a random move, without restriction
 *
 * @return - a random move
 */
static Move generate_random_move(RubiksScrambleOption layers_range)
{
	return random_layer(layers_range) | random_modifier();
}


/**
 * Generates the first random move of the scramble
 *
 * @return - the first move of the scramble
 */
static Move generate_first_random_move(RubiksScrambleOption layers_range)
{
	return generate_random_move(layers_range);
}


/**
 * Generates a random move, guaranteed to be on a different axis than the
 * excluded one
 *
 * @param excluded_axis - the axis to exclude
 *
 * @return - a random move
 */
static Move generate_next_random_move(
	Axis excluded_axis,
	RubiksScrambleOption layers_range)
{
	Move next_move;

	do next_move = generate_random_move(layers_range);
	while ((next_move & AXIS_MASK) == excluded_axis);

	return next_move;
}


/**
 * Generates every moves of the scramble
 *
 * @param moves - the buffer to insert generated moved to
 * @param count - the number of moves to generate
 */
static void generate_random_moves(
	Move moves[],
	size_t count,
	RubiksScrambleOption layers_range)
{
	size_t added_moves = 0;

	moves[added_moves++] = generate_first_random_move(layers_range);

	while (added_moves < count)
	{
		Axis previous_axis = moves[added_moves - 1] & AXIS_MASK;
		moves[added_moves++] = generate_next_random_move(
			previous_axis,
			layers_range);
	}
}


/**
 * Computes the length of the scramble string, including spacing between
 * each move
 *
 * @param moves - the moves composing the scramble
 * @param count - the number of moves in the scramble
 *
 * @return - the length of the scramble string
 */
static size_t compute_scramble_string_length(Move const moves[], size_t count)
{
	size_t string_length = 0;
	size_t index;

	for (index = 0; index < count; index++)
	{
		/* 1 character for the layer */
		string_length++;

		/* 1 character for spacing between moves */
		if (index > 0)
			string_length++;

		/* 1 character for the modifier, if any */
		if ((moves[index] & MODIFIER_MASK) != NO_MODIFIER)
			string_length++;
	}

	return string_length;
}


/**
 * Returns the symbol of the given layer
 *
 * @param layer - the layer to get the symbol for
 *
 * @return - the symbol to write the layer, or '?' if unknown layer
 */
static char layer_symbol(Layer layer)
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

		case LEFT_LAYERS: return 'l';
		case RIGHT_LAYERS: return 'r';
		case TOP_LAYERS: return 'u';
		case BOTTOM_LAYERS: return 'd';
		case FRONT_LAYERS: return 'f';
		case BACK_LAYERS: return 'b';
		default: return '?';
	}
}


/**
 * Returns the symbol of the given modifier
 *
 * @param layer - the modifier to get the symbol for
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
 * Writes a move at the beginning of the scramble string
 *
 * @param move - the move to write
 * @param scramble - the scramble string to write to
 *
 * @return - the number of writen bytes
 */
static size_t write_move(Move move, char * scramble)
{
	size_t writen_bytes = 0;

	* (scramble + writen_bytes++) = layer_symbol(move & LAYER_MASK);

	if ((move & MODIFIER_MASK) != NO_MODIFIER)
		* (scramble + writen_bytes++) = modifier_symbol(move & MODIFIER_MASK);

	return writen_bytes;
}


/**
 * Writes every move in the scramble string
 *
 * @param moves - the moves to write in the scramble string
 * @param count - the number of moves to write
 * @param scramble - the scramble string to write to, must be wide enough
 * 	to contain layers, modifiers, spacing between moves and null-terminating
 * 	byte
 */
static void write_scramble(Move const moves[], size_t count, char * scramble)
{
	size_t move_index;

	scramble += write_move(moves[0], scramble);

	for (move_index = 1; move_index < count; move_index++)
	{
		* scramble++ = ' ';
		scramble += write_move(moves[move_index], scramble);
	}

	* scramble = '\0';
}


/**
 * Creates the scramble string, based on the given moves
 * The caller is in charge of the allocated memory
 *
 * @param moves - the moves composing the scramble
 * @param count - the number of moves
 *
 * @return - the scramble string
 */
static char * create_scramble_string(Move const moves[], size_t count)
{
	size_t string_length = compute_scramble_string_length(moves, count);
	char * scramble = malloc(string_length + 1);

	if (scramble == NULL)
		return NULL;

	write_scramble(moves, count, scramble);

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

	generate_random_moves(moves, length, flags & LAYERS_RANGE_MASK);
	scramble = create_scramble_string(moves, length);

	free(moves);

	return scramble;
}
