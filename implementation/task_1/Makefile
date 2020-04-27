SHELL := /bin/bash
N := 100

build:
	g++ -o matrix_generator matrix_generator.cpp
	g++ -o multiplication multiplication.cpp
	g++ -fopenmp -o openmp_multiplication openmp_multiplication.cpp

run:
	rm A.txt B.txt
	./matrix_generator $(N) $(N) >> A.txt
	./matrix_generator $(N) $(N) >> B.txt
	./multiplication A.txt B.txt
	./openmp_multiplication A.txt B.txt
