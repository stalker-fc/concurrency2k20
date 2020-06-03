SHELL := /bin/bash

install:
	mkdir -p task_3_venv
	python3 -m venv task_3_venv && source task_3_venv/bin/activate && pip3 install -r scripts/requirements.txt

build:
	mkdir -p build
	cd build &&	cmake ../sources &&	make

run: install build
	source task_3_venv/bin/activate && python3 scripts/run.py

clean:
	rm -rf build
	rm -rf task_3_venv