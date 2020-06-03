SHELL := /bin/bash

install:
	rm -rf task_1_venv
	mkdir task_1_venv
	python3 -m venv task_1_venv && source task_1_venv/bin/activate && pip3 install -r scripts/requirements.txt

build:
	rm -rf build
	mkdir build
	cd build &&	cmake ../sources &&	make

run: install build
	source task_1_venv/bin/activate && python3 scripts/run.py
