# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = -lraylib -lm

# macOS specific flags
ifeq ($(shell uname), Darwin)
    LDFLAGS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
endif

# Linux specific flags
ifeq ($(shell uname), Linux)
    LDFLAGS += -lGL -lpthread -ldl -lrt -lX11
endif

# Source and output
SRC = main.c
OUT = bloxorz3d

# Default target
all: $(OUT)

# Compile the game
$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(OUT)

# Install raylib (macOS with Homebrew)
install-raylib-mac:
	brew install raylib

# Install raylib (Linux - Ubuntu/Debian)
install-raylib-linux:
	sudo apt-get update
	sudo apt-get install -y libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev
	git clone https://github.com/raysan5/raylib.git raylib-temp
	cd raylib-temp/src && make PLATFORM=PLATFORM_DESKTOP
	sudo make install -C raylib-temp/src
	rm -rf raylib-temp

# Install raylib (manual build from source)
install-raylib-source:
	git clone https://github.com/raysan5/raylib.git raylib-temp
	cd raylib-temp/src && make PLATFORM=PLATFORM_DESKTOP
	sudo make install -C raylib-temp/src
	rm -rf raylib-temp

# Clean build files
clean:
	rm -f $(OUT)

# Run the game
run: $(OUT)
	./$(OUT)

# Help
help:
	@echo "Available targets:"
	@echo "  make                    - Compile the game"
	@echo "  make install-raylib-mac - Install raylib on macOS (Homebrew)"
	@echo "  make install-raylib-linux - Install raylib on Linux"
	@echo "  make install-raylib-source - Install raylib from source"
	@echo "  make clean              - Remove compiled files"
	@echo "  make run                - Compile and run the game"

.PHONY: all clean run install-raylib-mac install-raylib-linux install-raylib-source help