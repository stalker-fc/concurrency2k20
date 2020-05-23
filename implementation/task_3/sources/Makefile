SHELL := /bin/bash
N := 20

build:
	g++ -o array_generator array_generator.cpp
	mpicxx -o quicksort quicksort.cpp

run:
	#rm -f array.txt sorted_array.txt
	#./array_generator $(N) >> array.txt
	./quicksort array.txt
