
all: cctree

cctree: simulator.cpp cctree.cpp cctree.h heap.cpp heap.h
	g++ -g -o cctree simulator.cpp cctree.cpp heap.cpp

rsync: simulator.cpp cctree.cpp cctree.h heap.cpp heap.h
	rsync -av . sguyer@parkstreet:simulator/
