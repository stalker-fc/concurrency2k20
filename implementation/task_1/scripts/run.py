import subprocess
import time
from dataclasses import dataclass
from pathlib import Path
from typing import List

import matplotlib.pyplot as plt
import numpy as np

EXECUTABLE_FILE = Path(__file__).parent.parent / 'build' / 'main'

TEST_CASES = {
    0: (100, 100),
    1: (250, 250),
    2: (500, 500),
    3: (1000, 1000),
    4: (200, 300),
    5: (600, 800),
    6: (1, 1000),
    7: (1, 10000),
}
MODES = [1, 2]


@dataclass
class BenchmarkResult:
    test_case: int
    mode: int
    execution_time_sec: float


def generate_random_matrix(n_rows: int, n_columns: int, path_to_save: str):
    matrix = np.random.randint(low=0, high=1000, size=(n_rows, n_columns))
    with open(path_to_save, 'w') as out:
        out.write(f'{n_rows} {n_columns}\n')
        out.write('\n'.join([' '.join(map(str, row)) for row in matrix]))


def benchmark():
    benchmark_results = []
    for test_case, (n_rows, n_columns) in TEST_CASES.items():
        generate_random_matrix(n_rows, n_columns, 'A.txt')
        generate_random_matrix(n_columns, n_rows, 'B.txt')
        for mode in MODES:
            st = time.time()
            subprocess.call([EXECUTABLE_FILE, 'A.txt', 'B.txt', str(mode)])
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
        1: "последовательное умножение матриц",
        2: "умножение матриц с использованием OpenMP"
    }
    plt.figure(figsize=(4, 4))
    left, width = 0.1, 0.85
    bottom, height = 0.1, 0.85

    ax = plt.axes([left, bottom, width, height])
    ax.set_title('Производительность разных методов умножения матриц', fontsize=7)
    ax.set_xlabel('Тестовые случаи, [количество строк]х[количество столбцов]', fontsize=6)
    ax.set_ylabel('Время работы программы, сек', fontsize=6)
    ax.set_xticks(list(TEST_CASES.keys()))
    ax.set_xticklabels([f'{n_rows} x {n_columns}' for (n_rows, n_columns) in TEST_CASES.values()],
                       fontdict={
                           'fontsize': 5
                       })

    legends = []
    for mode in MODES:
        legends.append(
            plt.Line2D((0, 1), (0, 0), color=mode_colors[mode], label=mode_descriptions[mode])
        )
    ax.legend(handles=legends, loc='best', fontsize=6)

    groupped_benchmark_results = {mode: [] for mode in MODES}
    for res in benchmark_results:
        groupped_benchmark_results[res.mode].append(res)
    for mode, group in groupped_benchmark_results.items():
        group = list(sorted(group, key=lambda x: x.test_case))
        execution_times = [res.execution_time_sec for res in group]
        test_cases = [res.test_case for res in group]
        ax.plot(test_cases, execution_times, marker='o', color=mode_colors[mode])
        print(f'\n\n### {mode} {mode_descriptions[mode]}')
        for res in group:
            print(f'- Test Case №{res.test_case} '
                  f'[{TEST_CASES[res.test_case][0]} x {TEST_CASES[res.test_case][1]}] * '
                  f'[{TEST_CASES[res.test_case][1]} x {TEST_CASES[res.test_case][0]}]: '
                  f'{res.execution_time_sec} sec')

    plt.savefig('task_1_report.png', format='png', dpi=200)


if __name__ == '__main__':
    benchmark()
