
CPPFLAGS = -I./src -I./src/syntax/
CXX = clang++
CXXFLAGS = -std=c++20 -pedantic -Wall -Wextra -g -ftrapv

SRCS = $(wildcard src/*)

all : kaleidoscope

kaleidoscope : $(SRCS)
	$(CXX) -o bin/kaleidoscope src/unity.cc $(CXXFLAGS) $(CPPFLAGS)

clean :
	rm bin/* 

# HEADERS = $(wildcard src/*.hh)
# SOURCES = $(wildcard src/*.cc)
# OBJECTS = $(wildcard bin/*.o)

# all : linkem

# linkem : $(OBJECTS) $(HEADERS) $(SOURCES)
# 	$(CXX) $(CPPFLAGS) -o bin/kaleidoscope $(OBJECTS) $(CXXFLAGS)

# bin/kaleidoscope.o : src/ui.hh bin/ui.o bin/parser.o bin/ast.o
# 	$(CXX) $(CPPFLAGS) -o bin/kaleidoscope.o -c src/kaleidoscope.cc $(CXXFLAGS)

# bin/ui.o : src/ast.hh src/uicmd.hh
# 	$(CXX) $(CPPFLAGS) -o bin/ui.o -c src/ui.cc $(CXXFLAGS)

# bin/parser.o : src/ast.hh src/compilesettings.hh src/uicmd.hh
# 	$(CXX) $(CPPFLAGS) -o bin/parser.o -c src/parser.cc $(CXXFLAGS)

# bin/ast.o : src/ast.hh
# 	$(CXX) $(CPPFLAGS) -o bin/ast.o -c src/ast.cc $(CXXFLAGS)


