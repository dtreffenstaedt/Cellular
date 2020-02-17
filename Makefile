CXX = g++
LDFLAGS = -std=c++17 -lSDL2
CXXFLAGS = -Wall -Werror -Wextra -pedantic -O3

SOURCES = main.cpp
TARGET = cellular

all: main.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(SOURCES) -o $(TARGET)

