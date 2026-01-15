UNAME_S := $(shell uname -s)
CC = gcc
CFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -O2 -Iinc

TARGET = bloxorz
SOURCES = $(wildcard *.c src/*.c)
OBJECTS = $(SOURCES:.c=.o)

RAYLIB_VERSION = 5.0
RAYLIB_DIR = deps/raylib
RAYLIB_SRC = $(RAYLIB_DIR)/src
RAYLIB_LIB = $(RAYLIB_SRC)/libraylib.a

CFLAGS += -I$(RAYLIB_SRC)

ifeq ($(UNAME_S),Linux)
    LDFLAGS = -L$(RAYLIB_SRC) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    
    ifneq ($(shell which apt-get 2>/dev/null),)
        PKG_MGR = apt-get
        INSTALL_CMD = sudo apt-get update && sudo apt-get install -y build-essential git libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev
    else ifneq ($(shell which dnf 2>/dev/null),)
        PKG_MGR = dnf
        INSTALL_CMD = sudo dnf install -y git alsa-lib-devel libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel mesa-libGL-devel mesa-libGLU-devel
    else ifneq ($(shell which pacman 2>/dev/null),)
        PKG_MGR = pacman
        INSTALL_CMD = sudo pacman -S --noconfirm git base-devel alsa-lib libx11 libxrandr libxi libxcursor libxinerama mesa glu
    else ifneq ($(shell which zypper 2>/dev/null),)
        PKG_MGR = zypper
        INSTALL_CMD = sudo zypper install -y git alsa-devel libX11-devel libXrandr-devel libXi-devel libXcursor-devel libXinerama-devel Mesa-libGL-devel glu
    endif
endif

ifeq ($(UNAME_S),Darwin)
    CC = clang
    LDFLAGS = -L$(RAYLIB_SRC) -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    
    ifneq ($(shell which brew 2>/dev/null),)
        PKG_MGR = brew
        INSTALL_CMD = brew install git
    endif
endif

all: libs $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	@$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "✓ Build complete! Run with: ./$(TARGET)"

%.o: %.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

libs: $(RAYLIB_LIB)

$(RAYLIB_LIB):
	@echo "Checking for Raylib..."
	@mkdir -p deps
	@if [ ! -d "$(RAYLIB_DIR)" ]; then \
		echo "Raylib not found. Cloning from source..."; \
		git clone --depth 1 --branch $(RAYLIB_VERSION) https://github.com/raysan5/raylib.git $(RAYLIB_DIR); \
	fi
	@echo "Building Raylib local static library..."
	@cd $(RAYLIB_SRC) && $(MAKE) PLATFORM=PLATFORM_DESKTOP

install_deps:
	@echo "Detected OS: $(UNAME_S)"
ifdef PKG_MGR
	@echo "Detected Package Manager: $(PKG_MGR)"
	@echo "Installing system build dependencies..."
	@$(INSTALL_CMD)
	@echo "✓ Dependencies installed."
else
	@echo "Warning: No known package manager found. Please install git and development headers manually."
	@echo "Required: libasound2, libx11, libxrandr, libxi, libgl1-mesa, libglu1-mesa, libxcursor, libxinerama"
endif

clean:
	@echo "Cleaning project files..."
	@rm -f $(OBJECTS) $(TARGET)
	@echo "✓ Clean complete."

clean_libs: clean
	@echo "Removing local libs..."
	@rm -rf deps
	@echo "✓ Libraries removed."

run: all
	@./$(TARGET)

rebuild: clean all

help:
	@echo "Bloxorz Build System"
	@echo "--------------------"
	@echo "Targets:"
	@echo "  make           - Build the game (automatically builds local Raylib)"
	@echo "  make run       - Build and run the game"
	@echo "  make install_deps - Install system development libraries (requires sudo)"
	@echo "  make clean     - Remove game object files and executable"
	@echo "  make clean_libs - Remove game files AND the local 'deps' folder"
	@echo "  make help      - Show this help message"
	@echo ""
	@echo "Configuration:"
	@echo "  OS: $(UNAME_S)"
	@echo "  Compiler: $(CC)"

.PHONY: all libs install_deps clean clean_libs run rebuild help
