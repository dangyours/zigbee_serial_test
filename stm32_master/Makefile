# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = -lserial

# --- Project Structure ---

# The final executable name
TARGET = master_serial_control

# Source files are all .cpp files in the src directory
SRCS = $(wildcard src/*.cpp)

# Object files are generated from source files and placed in the build directory
# e.g., src/main.cpp becomes build/main.o
BUILD_DIR = build
OBJS = $(patsubst src/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# --- Rules ---

# The default goal when you run `make`
# .PHONY ensures this rule runs even if a file named "all" exists
.PHONY: all
all: $(TARGET)

# Rule to link the final executable from the object files
# Depends on all object files being created first
$(TARGET): $(OBJS)
	@echo "Linking..."
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build finished: $(TARGET)"

# Pattern rule to compile a .cpp file from src/ into a .o file in build/
# This runs once for each source file
# The `| $(BUILD_DIR)` is an order-only prerequisite to ensure the build directory exists
$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to create the build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Rule to clean up all generated files
# .PHONY ensures this rule runs even if a file named "clean" exists
.PHONY: clean
clean:
	@echo "Cleaning up..."
	@rm -rf $(TARGET) $(BUILD_DIR)
	@echo "Clean complete."