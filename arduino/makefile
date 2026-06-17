PORT ?= /dev/ttyACM0
BOARD ?= esp32:esp32:esp32
BAUD ?= 115200

.PHONY: compile upload list serial clean	

compile: build/arduino.ino.bin
	@echo "compilation done"

upload: compile
	arduino-cli upload -p $(PORT) --fqbn $(BOARD) . --build-path ./build && echo "upload done"

serial: upload
	arduino-cli monitor -p $(PORT) --config baudrate=$(BAUD)

list: 
	arduino-cli board list

clean:
	@rm -rf build
	@echo "cleanup done"

build/arduino.ino.bin: $(wildcard *.ino) $(wildcard *.hpp) $(wildcard *.cpp)
	arduino-cli compile --fqbn $(BOARD) . --build-path ./build

