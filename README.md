

# 📚 lib rubiks-algos

A rubik's cube shared library writen in **C**


## Features

- scramble generation without repetions (like [R R2] or [L' L])


## Features to come

- optional camera rotations in scrambles (eg., [U D'] = [E y])
- wide moves in scrambles (eg., [U E] = [u])
- solving


## 🫨 Why ?

This library is intended to be a foundation for a 3D rubik's cube solver (GUI),
it's responsible for shuffling and resolving the cube


## 📦 Building it and checking

```
make lib
make run-tests
```


## 🤔 How to use

Include headers located in the `include/` directory in your project and link the library
```
gcc [your sources].c -Llib/ -lrubiks-algos -o [your program]
LD_LIBRARY_PATH=lib/ ./[your program]
```


## 👇 Usage example, generating a scramble

```C
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "include/rubiks_moves.h"

int main(void)
{
	char * scramble;

	/* init the random seed */
	srand(time(NULL));

	scramble = rubiks_generate_scramble(16);
	if (scramble == NULL)
		return EXIT_FAILURE;

	printf("scramble: [%s]\n", scramble);
	free(scramble);

	return EXIT_SUCCESS;
}

```
```bash
gcc example.c -o example -Llib/ -lrubiks-algos && LD_LIBRARY_PATH=lib/ ./example
```


## 😨 Found a bug ?

Create a pull request with a failing test, I'll make it pass
