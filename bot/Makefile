CXX = clang++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iincludes -g

OBJDIR := .objs
SRCDIR := srcs

SRCFILES := $(wildcard $(SRCDIR)/*.cpp)
OBJFILES := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCFILES))
TARGET = bot

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CXX) $(OBJFILES) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

re : fclean all

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(TARGET)

.PHONY: all clean fclean re
