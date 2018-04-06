
all: simulator

simulator: main.cpp simulator.cpp cctree.cpp cctree.h heap.cpp heap.h
	g++ -g -o simulator main.cpp simulator.cpp cctree.cpp heap.cpp


