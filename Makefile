UNAME_S := $(shell uname -s)
CC = gcc
CFLAGS = -Wall -O2 -Iinc
TARGET = bloxorz
SOURCES = main.c src/render.c src/map.c src/box.c src/utils.c
OBJECTS = $(SOURCES:.c=.o)

RAYLIB_DIR = deps/raylib
RAYLIB_SRC = $(RAYLIB_DIR)/src
RAYLIB_LIB = $(RAYLIB_SRC)/libraylib.a
CFLAGS += -I$(RAYLIB_SRC)

ifeq ($(UNAME_S),Linux)
    LDFLAGS = -L$(RAYLIB_SRC) $(RAYLIB_LIB) -lGL -lm -lpthread -ldl -lrt -lX11
    PKG_MANAGER = apt-get
    INSTALL_CMD = sudo apt-get update && sudo apt-get install -y libraylib-dev
endif

ifeq ($(UNAME_S),Darwin)
    CC = clang
    LDFLAGS = $(RAYLIB_LIB) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
endif

all: raylib_check $(TARGET)

raylib_check:
ifeq ($(UNAME_S),Darwin)
	@if [ ! -f $(RAYLIB_LIB) ]; then \
		echo "Raylib not found. Downloading and building locally..."; \
		mkdir -p deps; \
		if [ ! -d $(RAYLIB_DIR) ]; then \
			git clone --branch 5.0 https://github.com/raysan5/raylib.git $(RAYLIB_DIR); \
		fi; \
		cd $(RAYLIB_SRC) && make PLATFORM=PLATFORM_DESKTOP; \
	fi
endif
ifeq ($(UNAME_S),Linux)
	@ldconfig -p | grep libraylib >/dev/null 2>&1 || ( \
		echo "Raylib not found. Installing via apt..."; \
		$(INSTALL_CMD) || ( \
			echo "Building raylib from source..."; \
			mkdir -p deps; \
			if [ ! -d $(RAYLIB_DIR) ]; then \
				wget -q https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz -O deps/raylib-5.0.tar.gz; \
				tar -xzf deps/raylib-5.0.tar.gz -C deps; \
				mv deps/raylib-5.0 $(RAYLIB_DIR); \
			fi; \
			cd $(RAYLIB_SRC) && make PLATFORM=PLATFORM_DESKTOP; \
		) \
	)
endif

%.o: %.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

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
	@echo "  make         - Build the project (auto-download raylib if missing)"
	@echo "  make run     - Build and run"
	@echo "  make clean   - Remove build files"
	@echo "  make rebuild - Clean and rebuild"
	@echo "  make help    - Show this message"

.PHONY: all raylib_check clean run rebuild help
