############################################################################
# Definition of the project directories.
############################################################################

INCLUDE_DIR=include
SRC_DIR=src
OBJECT_DIR=obj

############################################################################
# Definition of the compiler and its flags.
############################################################################

CC=gcc
CFLAGS=-I$(INCLUDE_DIR) -Wall -Werror -g
LIBS=

############################################################################
# Definition of the project files.
############################################################################

EXEC     = btree_properties_test

INCLUDES = $(INCLUDE_DIR)/btree_properties.h \
           $(INCLUDE_DIR)/btree_utils.h

OBJECTS  = $(OBJECT_DIR)/btree_properties.o \
           $(OBJECT_DIR)/btree_utils.o \
           $(OBJECT_DIR)/btree_properties_test.o

############################################################################
# Definitions of the build commands.
############################################################################

$(OBJECT_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDES)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

$(EXEC): $(OBJECTS)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

all: $(EXEC) $(OBJECTS)

############################################################################
# Definition of the cleanup and run task.
############################################################################

.PHONY: run clean

run:
	./$(EXEC)

clean:
	rm -f $(OBJECT_DIR)/*.o
	rm -f $(SRC_DIR)/*.c~
	rm -f $(INCLUDE_DIR)/*.h~
	rm -f $(EXEC)
