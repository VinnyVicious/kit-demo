CXX          := g++
#CXXFLAGS     := -std=c++14 -Wall -Wextra -Wpedantic -Wno-unused-parameter -O0 -DKIT_DEBUG -g
CXXFLAGS     := -std=c++14 -Wall -Wextra -Wpedantic -Wno-unused-parameter -O2 -g
#LDFLAGS      := -ldl -pthread
REQLIBS      := kit
LIBS         := $(shell pkg-config --libs $(REQLIBS))
DEPFLAGS     := $(shell pkg-config --cflags $(REQLIBS))
OUT_BINARY   := demo
BUILDDIR     := build

SOURCEDIR    := src
INCLUDEDIR   := include

SOURCES      := $(shell find $(SOURCEDIR) -name '*.cpp')
OBJECTS      := $(addprefix $(BUILDDIR)/,$(SOURCES:%.cpp=%.o))

$(OUT_BINARY): $(OBJECTS)
	$(shell mkdir bin)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) $(LDFLAGS) $(LIBS) $(OBJECTS) -o dist/$(OUT_BINARY)

$(BUILDDIR)/%.o: %.cpp
	@echo 'Building ${notdir $@} ...'
	$(shell mkdir -p  "${dir $@}")
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -I$(INCLUDEDIR) -c $< -o $@
	
clean:
	$(shell rm -rf ./build)
	$(shell rm -rf ./bin)
