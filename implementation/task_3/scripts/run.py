import subprocess
import time
from dataclasses import dataclass
from pathlib import Path
from typing import List

import matplotlib.pyplot as plt
import numpy as np

EXECUTABLE_FILE = Path(__file__).parent.parent / 'build' / 'main'

TEST_CASES = {
    0: 1000,
    # 1: 100000,
    # 2: 1000000,
}
MODES = [1, ]


@dataclass
class BenchmarkResult:
    test_case: int
    mode: int
    execution_time_sec: float


def generate_random_array(length: int, path_to_save: str):
    array = np.random.randint(low=0, high=length, size=length)
    with open(path_to_save, 'w') as out:
        out.write(' '.join(map(str, array)))


def benchmark():
    benchmark_results = []
    for test_case, length in TEST_CASES.items():
        generate_random_array(length, 'array.txt')
        for mode in MODES:
            st = time.time()
            subprocess.call([EXECUTABLE_FILE, 'array.txt', str(mode)])
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
        1: "Последовательная быстрая сортировка",
        2: "Быстрая сортировка с использованием MPI"
    }
    plt.figure(figsize=(4, 4))
    left, width = 0.1, 0.85
    bottom, height = 0.1, 0.85

    ax = plt.axes([left, bottom, width, height])
    ax.set_title('Производительность разных методов быстрой сортировки', fontsize=7)
    ax.set_xlabel('Тестовые случаи, [количество элементов в массиве]', fontsize=6)
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
              f'{TEST_CASES[test_case]} элементов'
              )
        for res in group:
            print(f'- {mode_descriptions[res.mode]}: {res.execution_time_sec} сек')

    plt.savefig('task_3_report.png', format='png', dpi=200)


if __name__ == '__main__':
    benchmark()
