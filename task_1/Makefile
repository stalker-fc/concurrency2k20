SHELL := /bin/bash

build:
	g++ -o matrix_generator matrix_generator.cpp
	g++ -o muliplication multiplication.cpp
	g++ -fopenmp -o openmp_multiplication openmp_multiplication.cpp

run:
	./matrix_generator
	./muliplication
	./openmp_multiplication