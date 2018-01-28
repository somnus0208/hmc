WORKDIR = $(pwd)
PROGRAM = hmc
OBJS = main.o huffman_stream.o huffman_tree.o huffman_tree_item.o
CXX = g++
CXXFLAGS = -c -std=c++11
LDFLAGS =  -fopenmp

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) -o $(PROGRAM) $(LDFLAGS) $(OBJS) 
	@echo "executable file $(PROGRAM) is built in $(PWD)" 

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

huffman_stream.o: huffman_stream.cpp
	$(CXX) $(CXXFLAGS) huffman_stream.cpp
huffman_tree.o: huffman_tree.cpp
	$(CXX) $(CXXFLAGS) huffman_tree.cpp
huffman_tree_item.o: huffman_tree_item.cpp
	$(CXX) $(CXXFLAGS) huffman_tree_item.cpp

.PHONY: clean
clean:
	-rm $(OBJS) $(PROGRAM)
