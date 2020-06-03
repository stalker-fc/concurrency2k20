SHELL := /bin/bash

install:
	mkdir -p task_1_venv
	python3 -m venv task_1_venv && source task_1_venv/bin/activate && pip3 install -r scripts/requirements.txt

build:
	mkdir -p build
	cd build &&	cmake ../sources &&	make

run: install build
	source task_1_venv/bin/activate && python3 scripts/run.py

clean:
	rm -rf build
	rm -rf task_1_venv