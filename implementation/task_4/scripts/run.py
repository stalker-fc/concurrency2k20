import subprocess
import time
from pathlib import Path
from typing import List

import matplotlib.pyplot as plt
import numpy as np
from dataclasses import dataclass
from scipy import spatial

EXECUTABLE_FILE = Path(__file__).parent.parent / 'build' / 'main'

TEST_CASES = {
    0: 100,
    1: 1_000,
    2: 10_000,
    3: 100_000,
}
MODES = {
    1: "sequential dijkstra",
    2: "parallel dijkstra",
}

MODE_COLORS = {
    1: '#FB707F',
    2: '#008ECC',
}

SERIAL_MODE = 1

GRAPH_DATA_PATH = 'graph.txt'
LEFT, BOTTOM, RIGHT, TOP = 0, 0, 100_000, 100_000
SOURCE_NODE = 0


@dataclass
class BenchmarkResult:
    test_case: int
    mode: int
    execution_time_sec: float


def generate_graph(n_vertices: int, path_to_save: str):
    source = np.array((0, 0))
    destination = np.array((1, 1))
    points = np.random.rand(n_vertices - 2, 2)
    points = np.vstack((source, destination, points))
    deltas = np.array([RIGHT - LEFT, TOP - BOTTOM])
    points = points * deltas + np.array([LEFT, BOTTOM])
    points = points.astype(float)

    mesh = spatial.Delaunay(points, incremental=True)
    edges = np.vstack([np.array([(a, b), (b, c), (c, a)]) for (a, b, c) in mesh.simplices])
    edges.sort(axis=1)
    edges = np.unique(edges, axis=0)

    weights = np.linalg.norm(points[edges[:, 0]] - points[edges[:, 1]], axis=1).astype(int)
    data = np.zeros(shape=(n_vertices, n_vertices), dtype=int)
    data[edges[:, 0], edges[:, 1]] = weights
    with open(path_to_save, 'w') as out:
        out.write(f'{n_vertices}\n')
        out.write('\n'.join([' '.join(map(str, row)) for row in data]))


def benchmark():
    benchmark_results = []
    for test_case, length in TEST_CASES.items():
        generate_graph(length, GRAPH_DATA_PATH)
        for mode in MODES:
            st = time.time()
            exit_code = subprocess.call([EXECUTABLE_FILE, GRAPH_DATA_PATH, str(mode)])
            if exit_code != 0:
                print(f'Error in benchmarking. Test Case `{test_case}`, mode `{mode}`')
            execution_time_sec = time.time() - st
            benchmark_results.append(BenchmarkResult(
                test_case,
                mode,
                execution_time_sec
            ))

    plot_results(benchmark_results)


def plot_results(benchmark_results: List[BenchmarkResult]):
    plot_time_graphics(benchmark_results)
    plot_efficiency_graphics(benchmark_results)

    groupped_by_test_cases_benchmark_result = {test_case: [] for test_case in TEST_CASES.keys()}
    for res in benchmark_results:
        groupped_by_test_cases_benchmark_result[res.test_case].append(res)

    for test_case, group in groupped_by_test_cases_benchmark_result.items():
        group = list(sorted(group, key=lambda x: x.mode))
        print(f'\n### Test Case {test_case} '
              f'[{TEST_CASES[test_case]}]'
              )
        for res in group:
            print(f'- {MODES[res.mode]}: {res.execution_time_sec} сек')


def plot_time_graphics(benchmark_results: List[BenchmarkResult]):
    plt.figure(figsize=(4, 4))
    left, width = 0.11, 0.85
    bottom, height = 0.1, 0.85

    ax = plt.axes([left, bottom, width, height])
    ax.set_title('Производительность алгоритмов поиска пути в графе', fontsize=7)
    ax.set_xlabel('Тестовые случаи, [количество вершин в графе]', fontsize=6)
    ax.set_ylabel('Время работы программы, сек', fontsize=6)
    ax.set_xticks(list(TEST_CASES.keys()))
    ax.set_xticklabels([f'{n_elems}' for n_elems in TEST_CASES.values()],
                       fontdict={
                           'fontsize': 5
                       })

    legends = []
    for mode in MODES:
        legends.append(
            plt.Line2D((0, 1), (0, 0), color=MODE_COLORS[mode], label=MODES[mode])
        )
    ax.legend(handles=legends, loc='best', fontsize=6)

    groupped_by_modes_benchmark_results = {mode: [] for mode in MODES}
    for res in benchmark_results:
        groupped_by_modes_benchmark_results[res.mode].append(res)

    for mode, group in groupped_by_modes_benchmark_results.items():
        group = list(sorted(group, key=lambda x: x.test_case))
        execution_times = [res.execution_time_sec for res in group]
        test_cases = [res.test_case for res in group]
        ax.plot(test_cases, execution_times, marker='o', color=MODE_COLORS[mode])

    plt.savefig('task_4_time_report.png', format='png', dpi=200)


def plot_efficiency_graphics(benchmark_results: List[BenchmarkResult]):
    plt.figure(figsize=(4, 4))
    left, width = 0.12, 0.85
    bottom, height = 0.1, 0.85

    ax = plt.axes([left, bottom, width, height])
    ax.set_title('Производительность алгоритмов поиска пути в графе', fontsize=7)
    ax.set_xlabel('Тестовые случаи, [количество элементов в массиве]', fontsize=6)
    ax.set_ylabel('Прирост в скорости, раз', fontsize=6)
    ax.set_xticks(list(TEST_CASES.keys()))
    ax.set_xticklabels([f'{n_elems}' for n_elems in TEST_CASES.values()],
                       fontdict={
                           'fontsize': 5
                       })

    legends = []
    for mode, description in MODES.items():
        legends.append(
            plt.Line2D((0, 1), (0, 0), color=MODE_COLORS[mode], label=description)
        )
    ax.legend(handles=legends, loc='best', fontsize=6)

    groupped_by_modes_benchmark_results = {mode: [] for mode in MODES}
    for res in benchmark_results:
        groupped_by_modes_benchmark_results[res.mode].append(res)

    serial_mode_times = [res.execution_time_sec for res in
                         list(sorted(groupped_by_modes_benchmark_results[SERIAL_MODE], key=lambda x: x.test_case))]
    for mode, group in groupped_by_modes_benchmark_results.items():
        group = list(sorted(group, key=lambda x: x.test_case))
        execution_times = [serial_time / res.execution_time_sec for res, serial_time in zip(group, serial_mode_times)]
        test_cases = [res.test_case for res in group]
        ax.plot(test_cases, execution_times, marker='o', color=MODE_COLORS[mode])

    plt.savefig('task_4_efficiency_report.png', format='png', dpi=200)


if __name__ == '__main__':
    benchmark()
