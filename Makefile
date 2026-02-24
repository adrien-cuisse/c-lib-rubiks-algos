
CC=gcc

# Common structure
SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin
LIB_DIR=lib
TESTS_DIR=tests

# Target and versioning
LIB_NAME=rubiks-algos
LIB_MAJOR_VERSION=0
LIB_MINOR_VERSION=1
LIB_PATCH_VERSION=0
STATIC_LIBRARY_NAME=lib$(LIB_NAME)-$(LIB_MAJOR_VERSION)-$(LIB_MINOR_VERSION)-$(LIB_PATCH_VERSION).a
SHARED_LIB_LINKER_NAME=lib$(LIB_NAME).so
SHARED_LIB_SONAME=$(SHARED_LIB_LINKER_NAME).$(LIB_MAJOR_VERSION)
SHARED_LIB_REAL_NAME=$(SHARED_LIB_SONAME).$(LIB_MINOR_VERSION).$(LIB_PATCH_VERSION)

# Release sources compilation
RELEASE_SRC=$(shell find $(SRC_DIR)/ -type f -name '*.c')
RELEASE_OBJ=$(subst $(SRC_DIR),$(OBJ_DIR),$(RELEASE_SRC:.c=.o))
RELEASE_CFLAGS=-fpic -O3 -Wall -Wextra -Werror -ansi -pedantic#-fvisibility=hidden
RELEASE_LDFLAGS=-fpic -shared -Wl,-soname,$(SHARED_LIB_LINKER_NAME)

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
# Criterion is not C89 compliant
TESTS_CFLAGS=$(subst -ansi,-std=c99,$(RELEASE_CFLAGS))
TESTS_LDFLAGS=-lcriterion -L$(LIB_DIR)/ -l$(LIB_NAME)
TESTS_BINS=$(subst $(TESTS_SRC_DIR),$(TESTS_BIN_DIR),$(TESTS_SRC:.c=))


default: run-tests

rebuild: clean-all run-tests shared-library


# Tests are run with local build
.PHONY: run-tests
run-tests: shared-library $(TESTS_BINS)
	@for TEST_BIN in $(TESTS_BINS) ; do \
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
$(TESTS_BIN_DIR)/%: $(TESTS_OBJ_DIR)/%.o $(TESTS_UTILS_OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(TESTS_LDFLAGS) $^ -o $@

# Static library local build
static-library: $(LIB_DIR)/$(STATIC_LIBRARY_NAME)
$(LIB_DIR)/$(STATIC_LIBRARY_NAME): $(RELEASE_OBJ)
	@mkdir -p $(LIB_DIR)/
	ar -rcs $@ $^
	strip --discard-all $@


# Shared library local build
shared-library: $(LIB_DIR)/$(SHARED_LIB_REAL_NAME)
$(LIB_DIR)/$(SHARED_LIB_REAL_NAME): $(RELEASE_OBJ)
	@mkdir -p $(LIB_DIR)/
	$(CC) $(RELEASE_LDFLAGS) -o $@ $^
	strip --discard-all $@
	cd $(LIB_DIR) && ln -sf $(SHARED_LIB_REAL_NAME) $(SHARED_LIB_SONAME)
	cd $(LIB_DIR) && ln -sf $(SHARED_LIB_SONAME) $(SHARED_LIB_LINKER_NAME)

# Don't delete intermediate objects when binaries are made
.SECONDARY: $(RELEASE_OBJ) $(TESTS_UTILS_OBJ) $(TESTS_OBJ)

.PHONY: clean
clean:
	rm -rf $(RELEASE_OBJ) $(TESTS_OBJ) $(TESTS_UTILS_OBJ)

.PHONY: clean-all
clean-all: clean
	rm -rf $(TESTS_BINS) $(LIB_DIR)/*
