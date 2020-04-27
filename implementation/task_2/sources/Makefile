SHELL := /bin/bash
N := 100

build:
	g++ -o matrix_generator matrix_generator.cpp
	g++ -o vector_generator vector_generator.cpp

run:
	./matrix_generator $(N) $(N) >> A.txt
	./vector_generator $(N) >> b.txt

clean:
	rm A.txt b.txt x.txt