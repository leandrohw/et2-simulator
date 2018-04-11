
all: simulator

simulator: main.cpp simulator.cpp cctree.cpp cctree.h ccnode.cpp ccnode.h heap.cpp heap.h
	g++ -g -o simulator main.cpp simulator.cpp cctree.cpp ccnode.cpp heap.cpp


