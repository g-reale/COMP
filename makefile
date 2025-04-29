COMPILER = gcc
CFLAGS = -g -Wall -Wextra -fsanitize=address
LIBS = 
INCLUDE = -Idgenerate -Iggenerate

SRC_FILES = $(wildcard *.c)
OBJ_FILES = $(patsubst %.c, object/%.o, $(SRC_FILES))
TARGET = g--

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ $^ $(LIBS)

object/%.o: %.c
	@mkdir -p object
	$(COMPILER) $(CFLAGS) $(INCLUDE) -c $< -o $@

clean:
	rm -rf object $(TARGET)

run: all
	./main || reset

debug: all
	./main 2> stderr.log || reset && cat stderr.log

remake: clean
	cd dgenerate && ./dgen.py
	cd ggenerate && ./ggen.py
	make all

.PHONY: all clean run debug remake