.PHONY: clean

EXEC := newt

INCLUDE_DIRS := src include
OUTPUT_DIR := bin

CXX := g++
CXXFLAGS := $(addprefix -I, $(INCLUDE_DIRS))
CXXSRC := $(wildcard src/*.cpp)
CXXOBJ := $(CXXSRC:.cpp=.o)

all: $(OUTPUT_DIR) $(CXXOBJ) $(OUTPUT_DIR)/$(EXEC)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

$(CXXOBJ): %.o: %.cpp
	@echo " CXX" $<; $(CXX) -c $< -o $@ $(CXXFLAGS)

$(OUTPUT_DIR)/$(EXEC): $(CXXOBJ)
	$(CXX) $(CXXOBJ) main.cpp -o $(OUTPUT_DIR)/$(EXEC) $(CXXFLAGS)

clean:
	rm -rf $(OUTPUT_DIR)
	rm $(COBJ) $(CXXOBJ)