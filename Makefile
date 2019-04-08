#SHELL=C:/Windows/System32/cmd.exe
SRC_GAME = $(shell find . -type f -name "*.cpp" | grep -v old | grep -v "Adventure.cpp" | grep -v "convert" | grep -v "printmap" | sort)
SRC_GAME_O = $(SRC_GAME:.cpp=.o)
INCLUDES = $(shell find . -type d | grep -v "git" | grep -v "AdventureMaps" | grep -v "Utils" | grep -v "Save" | sed 's/^/-I/' | sort)
MAIN = Adventure.cpp

CXXFLAGS = -O0 -g -Wall -std=c++17

ifeq ($(OS),Windows_NT)
    uname_S := Windows
else
    uname_S := $(shell uname -s)
endif

ifeq ($(uname_S), Windows)
    CXXLIBFLAGS =
endif
ifeq ($(uname_S), Linux)
    CXXLIBFLAGS =  -lncurses
endif

CXXLIBCAMP = Content

BUILDDIR=.
EXE=sadelica

.PHONY: clean

all: game

run: all
	./$(EXE)

game: $(EXE)

$(EXE): $(SRC_GAME_O) $(MAIN:.cpp=.o)
	g++ $(CXXFLAGS) $(INCLUDES) -o $(BUILDDIR)/$@ $(SRC_GAME_O) $(MAIN:.cpp=.o) $(CXXLIBFLAGS)

%.o: %.cpp
	g++ $(CXXFLAGS) $(INCLUDES) -c $< -o $@ $(CXXLIBFLAGS)

src:
	echo $(SRC_GAME)

clean:
	rm -f Content/*.o
	rm -f *.exe
	rm -f *.gch
	rm -f *.o
	rm -f *.tmp