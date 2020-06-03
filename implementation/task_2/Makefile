SHELL := /bin/bash

install:
	mkdir -p task_2_venv
	python3 -m venv task_2_venv && source task_2_venv/bin/activate && pip3 install -r scripts/requirements.txt

build:
	mkdir -p build
	cd build &&	cmake ../sources &&	make

run: install build
	source task_2_venv/bin/activate && python3 scripts/run.py

clean:
	rm -rf build
	rm -rf task_2_venv