COMPILER = iverilog
CFLAGS = -g2005

SRC_FILES = $(wildcard *.v)
TARGET = processor

all: $(TARGET)

$(TARGET): $(SRC_FILES)
	$(COMPILER) $(CFLAGS)  -Wall -Wno-timescale -o $@ $^