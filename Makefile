UNAME_S := $(shell uname -s)
CC = gcc
CFLAGS = -Wall -O2 -Iinc
ifeq ($(UNAME_S),Linux)
    LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
    PKG_MANAGER = apt-get
    INSTALL_CMD = sudo apt-get update && sudo apt-get install -y libraylib-dev
endif

ifeq ($(UNAME_S),Darwin)
    LDFLAGS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
    PKG_MANAGER = brew
    INSTALL_CMD = brew install raylib
    CC = clang
endif

TARGET = bloxorz
SOURCES = main.c src/render.c src/map.c src/box.c src/utils.c
OBJECTS = $(SOURCES:.c=.o)

all: check_raylib $(TARGET)

check_raylib:
	@pkg-config --exists raylib 2>/dev/null || (echo "Raylib not found. Installing..." && $(MAKE) install_raylib)

install_raylib:
ifeq ($(UNAME_S),Darwin)
	@echo "Installing raylib via Homebrew..."
	@command -v brew >/dev/null 2>&1 || (echo "Error: Homebrew not found. Install from https://brew.sh" && exit 1)
	@$(INSTALL_CMD)
else ifeq ($(UNAME_S),Linux)
	@echo "Installing raylib via $(PKG_MANAGER)..."
	@$(INSTALL_CMD) 2>/dev/null || \
	(echo "Package manager failed, building from source..." && \
	wget -q https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz && \
	tar -xzf 5.0.tar.gz && \
	cd raylib-5.0/src && make PLATFORM=PLATFORM_DESKTOP && sudo make install && \
	cd ../.. && rm -rf raylib-5.0 5.0.tar.gz && sudo ldconfig && \
	echo "Raylib installed successfully!")
else
	@echo "Unsupported OS. Please install raylib manually from: https://www.raylib.com/"
	@exit 1
endif


%.o: %.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	@$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "✓ Build complete! Run with: ./$(TARGET)"

clean:
	@echo "Cleaning..."
	@rm -f $(OBJECTS) $(TARGET)
	@echo "✓ Clean complete!"

run: $(TARGET)
	@./$(TARGET)

rebuild: clean all

help:
	@echo "Build System"
	@echo "Detected OS: $(UNAME_S)"
	@echo ""
	@echo "Available targets:"
	@echo "  make         - Build the project"
	@echo "  make run     - Build and run"
	@echo "  make clean   - Remove build files"
	@echo "  make rebuild - Clean and rebuild"
	@echo "  make help    - Show this message"

.PHONY: all check_raylib install_raylib clean run rebuild help