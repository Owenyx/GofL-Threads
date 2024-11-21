# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -g

# Program files
FILE_1 = main

# Default target: build both files seperately
all: $(FILE_1)

# Rule to build file_1 executable
$(FILE_1): $(FILE_1).cpp
	$(CXX) $(CXXFLAGS) $(FILE_1).cpp -o $(FILE_1) -lncurses

# Clean up generated files
clean:
	rm -f $(FILE_1)

# Phony targets to avoid file conflicts
.PHONY: all clean $(FILE_1)