
#include <stdlib.h>

#include "../include/rubiks_moves.h"




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
 * The 9 layers composing the cube
 */
typedef enum Layer
{
	LEFT_LAYER = 0x4,
	MIDDLE_LAYER = 0x8,
	RIGHT_LAYER = 0x10,

	TOP_LAYER = 0x20,
	EQUATOR_LAYER = 0x40,
	BOTTOM_LAYER = 0x80,

	FRONT_LAYER = 0x100,
	STANDING_LAYER = 0x200,
	BACK_LAYER = 0x400,

	LAYER_MASK = 0x7FC
} Layer;


/**
 * At least 16 bits
 */
typedef unsigned int Move;




/**
 * Picks a random layer
 *
 * @return - a random layer
 */
static Layer random_layer(void)
{
	Layer layers[] =
	{
		LEFT_LAYER, MIDDLE_LAYER, RIGHT_LAYER,
		TOP_LAYER, EQUATOR_LAYER, BOTTOM_LAYER,
		FRONT_LAYER, STANDING_LAYER, BACK_LAYER,
	};

	return layers[rand() % 9];
}


/**
 * Picks a random modifier
 *
 * @return - a random modifier
 */
static Modifier random_modifier(void)
{
	Modifier modifiers[] =
	{
		NO_MODIFIER, REVERSE_MODIFIER, DOUBLE_MODIFIER
	};

	return modifiers[rand() % 3];
}


/**
 * Generates a random move, without restriction
 *
 * @return - a random move
 */
static Move generate_random_move(void)
{
	return random_layer() | random_modifier();
}


/**
 * Generates the first random move of the scramble
 *
 * @return - the first move of the scramble
 */
static Move generate_first_random_move(void)
{
	return generate_random_move();
}


/**
 * Generates a random move, guaranteed to be on a different layer than the
 * excluded one
 *
 * @param excluded_layer - the layer to exclude
 *
 * @return - a random move
 */
static Move generate_next_random_move(Layer excluded_layer)
{
	Move next_move;

	do next_move = generate_random_move();
	while ((next_move & LAYER_MASK) == excluded_layer);

	return next_move;
}


/**
 * Generates every moves of the scramble
 *
 * @param moves - the buffer to insert generated moved to
 * @param count - the number of moves to generate
 */
static void generate_random_moves(Move moves[], size_t count)
{
	size_t added_moves = 0;

	moves[added_moves++] = generate_first_random_move();

	while (added_moves < count)
	{
		Layer previous_layer = moves[added_moves - 1] & LAYER_MASK;
		moves[added_moves++] = generate_next_random_move(previous_layer);
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


char * rubiks_generate_scramble(size_t length)
{
	Move * moves;
	char * scramble;

	if (length == 0)
		return NULL;

	moves = malloc(sizeof(Move) * length);
	if (moves == NULL)
		return NULL;

	generate_random_moves(moves, length);
	scramble = create_scramble_string(moves, length);

	free(moves);

	return scramble;
}
