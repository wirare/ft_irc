# Compiler and flags
CXX := clang++
CXXFLAGS := -Wall -Wextra -Werror -std=c++98 -Iincludes -g

# Directories
SRCDIR := srcs
INCDIR := includes
OBJDIR := .objs

# Target binary
TARGET := ft_irc

# Source and object files
SRCFILES := $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(SRCDIR)/Commands/*.cpp)
OBJFILES := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCFILES))

# Default target
all: $(TARGET)
	make -C bot all

# Link object files into final binary
$(TARGET): $(OBJFILES)
	@echo "Linking $(TARGET)..."
	$(CXX) $(OBJFILES) -o $(TARGET)

# Compile .cpp to .o
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create object directory if it doesn't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(OBJDIR)/Commands

# Clean build files
clean:
	rm -rf $(OBJDIR)
	make -C bot clean

# Clean everything including binary
fclean: clean
	rm -f $(TARGET)
	make -C bot fclean

# Rebuild from scratch
re: fclean all
	make -C bot re
	clear
	

.PHONY: all clean fclean re

