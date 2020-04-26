SHELL := /bin/bash

build:
	g++ -o matrix_generator matrix_generator.cpp
	g++ -o muliplication multiplication.cpp
#	g++ -fopenmp -o openmp_multiplication openmp_multiplication.cpp

run:
	rm A.txt B.txt
	./matrix_generator 1000 1000 >> A.txt
	./matrix_generator 1000 1000 >> B.txt
	./muliplication A.txt B.txt
#	./openmp_multiplication