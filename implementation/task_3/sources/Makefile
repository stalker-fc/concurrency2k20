SHELL := /bin/bash
N := 20

build:
	g++ -o array_generator array_generator.cpp
	mpicxx -o quick_sort_mpi quick_sort_mpi.cpp

run:
	#rm -f array.txt sorted_array.txt
	#./array_generator $(N) >> array.txt
	./quick_sort_mpi array.txt
