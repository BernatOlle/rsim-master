FLAGS := -std=gnu++11

SRCDIR := ./src
BUILDDIR := ./bin

C_FILES := $(shell find $(SRCDIR) ! -name "main.cpp" -name "*.cpp")
OBJS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(C_FILES:.cpp=.o))

default: Project

Project: $(SRCDIR)/main.cpp $(OBJS)
	g++ $(FLAGS) $(OBJS) $(SRCDIR)/main.cpp -o $@

./bin/%.o : $(SRCDIR)/%.cpp
	g++ $(FLAGS) -c $< -o $@

./bin/%.o : $(SRCDIR)/%.cpp $(SRCDIR)/%.hpp
	g++ $(FLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR)/*.o
	rm -rf Project
