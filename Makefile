#Author: Adam Kucera (kucerad5@fit.cvut.cz)

#Makefile for Simple XML editor in NCurses

CXX = g++
CL = g++
CXXFLAGS = -Wall -pedantic -Wno-long-long -O0 -ggdb
LIBS = -lncurses -lmenu
BINARY = kucerad5
RM=rm -rf
OBJECTS = bin/objects/main.o bin/objects/CXML.o bin/objects/CException.o bin/objects/CAttribute.o bin/objects/CNode.o bin/objects/functions.o bin/objects/CTagStack.o bin/objects/CGUI.o bin/objects/CSearchTree.o
DOC=Doxyfile

all: $(OBJECTS) $(DOC)
	make compile
	make doc

compile: $(BINARY)

run: $(BINARY)
	./$(BINARY)

clean:
	$(RM) bin doc $(BINARY)
	
doc: $(DOC) src/*
	( cd src | doxygen $(DOC) 2> /dev/null > /dev/null )

#------------------------------------------------------------------

$(BINARY): $(OBJECTS)
	$(CL) $(CXXFLAGS) $(OBJECTS) -o $(BINARY) $(LIBS)

bin/objects/main.o: src/main.cpp src/CXML.h src/CException.h src/CGUI.h
	mkdir -p bin/objects
	$(CXX) $(CXXFLAGS) src/main.cpp -c -o bin/objects/main.o $(LIBS)

bin/objects/CXML.o: src/CXML.cpp src/CXML.h src/CException.h src/functions.h src/CTagStack.h src/CGUI.h src/CNode.h src/CSearchTree.h
	mkdir -p bin/objects
	$(CXX) $(CXXFLAGS) src/CXML.cpp -c -o bin/objects/CXML.o $(LIBS)
	
bin/objects/CException.o: src/CException.cpp src/CException.h src/CGUI.h
	mkdir -p bin/objects
	$(CXX) $(CXXFLAGS) src/CException.cpp -c -o bin/objects/CException.o $(LIBS)
	
bin/objects/CNode.o: src/CNode.cpp src/CNode.h src/CAttribute.h src/CException.h src/functions.h src/CGUI.h src/CSearchTree.h
	mkdir -p bin/objects
	$(CXX) $(CXXFLAGS) src/CNode.cpp -c -o bin/objects/CNode.o $(LIBS)
	
bin/objects/CAttribute.o: src/CAttribute.cpp src/CAttribute.h src/functions.h src/CException.h
	mkdir -p bin/objects
	$(CXX) $(CXXFLAGS) src/CAttribute.cpp -c -o bin/objects/CAttribute.o $(LIBS)
	
bin/objects/functions.o: src/functions.cpp src/functions.h
	mkdir -p bin/objects
	$(CXX) $(CXXFLAGS) src/functions.cpp -c -o bin/objects/functions.o $(LIBS)
	
bin/objects/CTagStack.o: src/CTagStack.cpp src/CTagStack.h
	mkdir -p bin/objects
	$(CXX) $(CXXFLAGS) src/CTagStack.cpp -c -o bin/objects/CTagStack.o $(LIBS)
	
bin/objects/CGUI.o: src/CGUI.cpp src/CGUI.h src/CNode.h src/CXML.h src/CException.h
	mkdir -p bin/objects
	$(CXX) $(CXXFLAGS) src/CGUI.cpp -c -o bin/objects/CGUI.o $(LIBS)

bin/objects/CSearchTree.o: src/CSearchTree.cpp src/CSearchTree.h src/CNode.h
	mkdir -p bin/objects
	$(CXX) $(CXXFLAGS) src/CSearchTree.cpp -c -o bin/objects/CSearchTree.o $(LIBS)