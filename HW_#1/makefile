# Compiler
CXX = g++

# Compiler Flags
CXXFLAGS = -std=c++17 -Wall

# Target executable
TARGET = a.out

# Source files
SRCS = main.cpp AnsiTerminal.cpp Cell.cpp Spreadsheet.cpp FileHandler.cpp FormulaParser.cpp

# Object files (derived from source files)
OBJS = $(SRCS:.cpp=.o)

# Default rule
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile .cpp files into .o files
%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	@rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)