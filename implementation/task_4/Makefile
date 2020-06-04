SHELL := /bin/bash

install:
	mkdir -p task_4_venv
	python3 -m venv task_4_venv && source task_4_venv/bin/activate && pip3 install -r scripts/requirements.txt

build:
	mkdir -p build
	javac -d build -sourcepath sources/src sources/src/*.java

run: install build
	source task_4_venv/bin/activate && python3 scripts/run.py

clean:
	rm -rf build
	rm -rf task_4_venv