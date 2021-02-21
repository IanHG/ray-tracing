CXX=g++
CXXSTD=--std=c++17
CXXOPTIMFLAGS=-O3
CXXDEBUGFLAGS=-O0 -g -rdynamic
CXXFLAGS=-Wall $(CXXOPTIMFLAGS)
LIBS=-lncurses

# find source files
SOURCEDIR := $(shell pwd)
BUILDDIR := $(shell pwd)
SOURCES := $(shell find $(SOURCEDIR) -path $(SOURCEDIR)/benchmark -prune -o -name '*.cpp' -print)
#OBJECTS := $(addprefix $(BUILDDIR)/,$(notdir $(SOURCES:.cpp=.o)))
OBJECTS := $(SOURCES:.cpp=.o)

# link
main.x: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o main.x $(LIBS)

# pull dependencies for existing .o files
-include $(OBJECTS:.o=.d)

# compile and generate dependency info
%.o: %.cpp %.d
	$(CXX) $(CXXSTD) -c $(CXXFLAGS) $*.cpp -o $*.o
	$(CXX) $(CXXSTD) -MM $(CXXFLAGS) $*.cpp > $*.d

# empty rule for dependency files
%.d: ;

clean:
	rm -f *core *.o *.d src/*.o src/*.d
