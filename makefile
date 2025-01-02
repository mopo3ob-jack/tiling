CXX:=g++
CC:=gcc
LD:=ld

CFLAGS:=-O2
CXXFLAGS:=-O2 -std=c++20
LDFLAGS:=-lGL -lGLU -lsfml-graphics -lsfml-window -lsfml-system

SOURCES:=$(shell find src/ -name "*.c*")
OUTPUTS:=$(SOURCES:src/%.cpp=lib/%.o)
EXECUTABLE:=tiling

default: $(EXECUTABLE)

$(EXECUTABLE): $(OUTPUTS)
	$(CXX) $^ $(LDFLAGS) -o $@

lib/%.o:src/%.c
	$(CC) -c $(CFLAGS) $^ -o $@

lib/%.o:src/%.cpp
	$(CXX) -c $(CXXFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -rf ./lib/**
