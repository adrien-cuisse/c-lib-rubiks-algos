
CC=gcc

# Common structure
SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin
LIB_DIR=lib
TESTS_DIR=tests

LIBRARY=rubiks-algos

# Release sources compilation
RELEASE_SRC=$(shell find $(SRC_DIR)/ -type f -name '*.c')
RELEASE_OBJ=$(subst $(SRC_DIR),$(OBJ_DIR),$(RELEASE_SRC:.c=.o))
RELEASE_CFLAGS=-Wall -Wextra -Werror -ansi -pedantic -fpic #-fvisibility=hidden -O3
RELEASE_LDFLAGS=

# Tests only structure
TESTS_SRC_DIR=$(addprefix $(TESTS_DIR)/,$(SRC_DIR))
TESTS_OBJ_DIR=$(addprefix $(TESTS_DIR)/,$(OBJ_DIR))
TESTS_BIN_DIR=$(addprefix $(TESTS_DIR)/,$(BIN_DIR))

# Test utils (assertions, helpers), which won't generate binaries
TESTS_UTILS_SRC=$(shell find $(TESTS_SRC_DIR)/helpers/ -type f -name '*.c')
TESTS_UTILS_OBJ=$(subst $(TESTS_SRC_DIR),$(TESTS_OBJ_DIR),$(TESTS_UTILS_SRC:.c=.o))

# Test sources compilation
TESTS_SRC=$(shell find $(TESTS_SRC_DIR) -type f -name '*.c')
TESTS_SRC:=$(filter-out $(TESTS_UTILS_SRC),$(TESTS_SRC))
TESTS_OBJ=$(subst $(TESTS_SRC_DIR),$(TESTS_OBJ_DIR),$(TESTS_SRC:.c=.o))
TESTS_CFLAGS=$(subst -ansi,,$(RELEASE_CFLAGS)) # Criterion is not C89 compliant
TESTS_LDFLAGS=-lcriterion -L$(LIB_DIR)/ -l$(LIBRARY)
TESTS_BINS=$(subst $(TESTS_SRC_DIR),$(TESTS_BIN_DIR),$(TESTS_SRC:.c=))


default: run-tests

rebuild: clean-all run-tests lib

lib: library-algorithms

.PHONY: run-tests
run-tests: lib $(TESTS_BINS)
	@for TEST_BIN in $(TESTS_BINS) ; do   \
		LD_LIBRARY_PATH=$(LIB_DIR)/ ./$$TEST_BIN; \
	done

# Release objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(RELEASE_CFLAGS) -c $^ -o $@

# Test objects
$(TESTS_OBJ_DIR)/%.o: $(TESTS_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(TESTS_CFLAGS) -c $^ -o $@

# Test binaries
.PHONY: tests-binaries
tests-binaries: $(TESTS_BINS)
$(TESTS_BIN_DIR)/%: $(TESTS_OBJ_DIR)/%.o $(TESTS_UTILS_OBJ)
	@echo "Make binaries..."
	@echo "$(TESTS_UTILS_SRC)"
	@echo "$(TESTS_UTILS_OBJ)"
	@mkdir -p $(dir $@)
	$(CC) $(TESTS_LDFLAGS) $^ -o $@

# Don't delete objects when binaries are made
.PRECIOUS: $(OBJ_DIR)/%.o $(TESTS_OBJ_DIR)/%.o

library-algorithms: $(LIB_DIR)/lib$(LIBRARY).so
$(LIB_DIR)/lib$(LIBRARY).so: $(RELEASE_OBJ)
	@mkdir -p $(LIB_DIR)/
	gcc -shared -o $@ $^
	strip --discard-all $@

.PHONY: clean
clean:
	rm -rf $(RELEASE_OBJ) $(TESTS_OBJ) $(TESTS_UTILS_OBJ)

.PHONY: clean-all
clean-all: clean
	rm -rf $(TESTS_BINS) $(LIB_DIR)/lib$(LIBRARY).so
