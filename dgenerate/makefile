COMPILER = gcc
CFLAGS = -g -Wall -Wextra -fopenmp
LIBS = 

SRC_FILES = $(wildcard *.c)
OBJ_FILES = $(patsubst %.c, object/%.o, $(SRC_FILES))
TARGET = main

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(COMPILER) $(CFLAGS) -o $@ $^ $(LIBS)

object/%.o: %.c
	@mkdir -p object
	$(COMPILER) $(CFLAGS) -c $< -o $@

clean:
	rm -rf object $(TARGET)

run: all
	./dgen.py

debug: all
	./main 2> stderr.log || reset && cat stderr.log

.PHONY: all clean run debug