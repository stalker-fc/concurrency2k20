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
    0: 30,
    # 1: 10000,
    # 2: 1000000,
}
MODES = [1, 2]

GRAPH_PATH = 'graph.txt'
LEFT, BOTTOM, RIGHT, TOP = 0, 0, 100_000, 100_000
N_NEAREST_NEIGHBOURS = 5


@dataclass
class BenchmarkResult:
    test_case: int
    mode: int
    execution_time_sec: float


def generate_graph(n_vertices: int, path_to_save: str):
    points = np.random.rand(n_vertices, 2)
    deltas = np.array([RIGHT - LEFT, TOP - BOTTOM])
    points = points * deltas + np.array([LEFT, BOTTOM])
    points = points.astype(float)

    kdtree = spatial.cKDTree(data=points)
    inds = kdtree.query(points, k=N_NEAREST_NEIGHBOURS + 1)[1][:, 1:]
    points_indices = np.arange(points.shape[0]).reshape(-1, 1)
    pair_indices = np.stack([np.repeat(
        points_indices,
        N_NEAREST_NEIGHBOURS, axis=-1), inds], axis=-1).reshape(-1, 2)
    pair_indices.sort(axis=1)
    pair_indices = np.unique(pair_indices, axis=0)
    edges = pair_indices.tolist()
    weights = np.linalg.norm(points[pair_indices[:, 0]] - points[pair_indices[:, 1]], axis=1).tolist()
    with open(path_to_save, 'w') as out:
        out.write(f'{len(edges)}\n')
        for (u, v), weight in zip(edges, weights):
            out.write(f'{u} {v} {weight}\n')


def benchmark():
    benchmark_results = []
    for test_case, length in TEST_CASES.items():
        generate_graph(length, GRAPH_PATH)
        for mode in MODES:
            st = time.time()
            exit_code = subprocess.call([EXECUTABLE_FILE, GRAPH_PATH, str(mode)])
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
    mode_colors = {
        1: '#FB707F',
        2: '#008ECC'
    }
    mode_descriptions = {
        1: "Алгоритм Дейкстры",
        2: "Параллельный алгоритм Дейкстры"
    }
    plt.figure(figsize=(4, 4))
    left, width = 0.1, 0.85
    bottom, height = 0.1, 0.85

    ax = plt.axes([left, bottom, width, height])
    ax.set_title('Производительность разных методов поиска кратчайшего пути в графе', fontsize=7)
    ax.set_xlabel('Тестовые случаи, [количество ребер]', fontsize=6)
    ax.set_ylabel('Время работы программы, сек', fontsize=6)
    ax.set_xticks(list(TEST_CASES.keys()))
    ax.set_xticklabels([f'[{length}]' for length in TEST_CASES.values()],
                       fontdict={
                           'fontsize': 5
                       })

    legends = []
    for mode in MODES:
        legends.append(
            plt.Line2D((0, 1), (0, 0), color=mode_colors[mode], label=mode_descriptions[mode])
        )
    ax.legend(handles=legends, loc='best', fontsize=6)

    groupped_by_modes_benchmark_results = {mode: [] for mode in MODES}
    for res in benchmark_results:
        groupped_by_modes_benchmark_results[res.mode].append(res)

    for mode, group in groupped_by_modes_benchmark_results.items():
        group = list(sorted(group, key=lambda x: x.test_case))
        execution_times = [res.execution_time_sec for res in group]
        test_cases = [res.test_case for res in group]
        ax.plot(test_cases, execution_times, marker='o', color=mode_colors[mode])

    groupped_by_test_cases_benchmark_result = {test_case: [] for test_case in TEST_CASES.keys()}
    for res in benchmark_results:
        groupped_by_test_cases_benchmark_result[res.test_case].append(res)

    for test_case, group in groupped_by_test_cases_benchmark_result.items():
        group = list(sorted(group, key=lambda x: x.mode))
        print(f'\n### Test Case {test_case} '
              f'{TEST_CASES[test_case]} вершин, {TEST_CASES[test_case] * N_NEAREST_NEIGHBOURS} рёбер'
              )
        for res in group:
            print(f'- {mode_descriptions[res.mode]}: {res.execution_time_sec} сек')

    plt.savefig('task_4_report.png', format='png', dpi=200)


if __name__ == '__main__':
    benchmark()
