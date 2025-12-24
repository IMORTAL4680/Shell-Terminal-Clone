# Makefile for C++ Terminal Emulator (cpp-shell) - Windows
# Requires C++17 or later and MinGW/g++

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -lshell32

# Target executable name
TARGET = cpp_shell.exe

# Source files
SRCS = cpp_shell.cpp

# Default target
all: $(TARGET)

# Compile and link
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

# Clean build artifacts
clean:
	del /Q $(TARGET) 2>nul

# Run the shell
run: $(TARGET)
	$(TARGET)

# Phony targets
.PHONY: all clean run
