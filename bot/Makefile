CXX = clang++

OBJDIR := .objs
SRCDIR := srcs

SRCFILES := $(wildcard $(SRCDIR)/*.cpp)
OBJFILES := $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCFILES))
TARGET = bot
IFLAG := -I includes/

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CXX) $(OBJFILES) -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(IFLAG) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

re : fclean all

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -rf $(TARGET)

.PHONY: all clean fclean re
