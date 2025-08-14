COMPILER = g++
CFLAGS = -Wall -Wextra -fsanitize=address
LIBS = 
INCLUDE = -Idgenerate -Iggenerate

SRC_FILES = $(wildcard *.cpp)
OBJ_FILES = $(patsubst %.cpp, object/%.o, $(SRC_FILES))
EXPANDED_FILES = $(patsubst %.cpp, expanded/%.i, $(SRC_FILES))
TARGET = g--

all: $(TARGET)

$(TARGET): $(OBJ_FILES) #$(EXPANDED_FILES)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ $^ $(LIBS)

# expanded/%.i: %.cpp
# 	@mkdir -p expanded
# 	$(COMPILER) -E $(CFLAGS) $(INCLUDE) -c $< -o $@

object/%.o: %.cpp
	@mkdir -p object
	$(COMPILER) $(CFLAGS) $(INCLUDE) -c $< -o $@


clean:
	rm -rf object $(TARGET)

run: all
	./main || reset

.PHONY: all clean run