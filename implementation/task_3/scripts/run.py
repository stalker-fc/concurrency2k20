import subprocess
import time
from dataclasses import dataclass
from pathlib import Path
from typing import List

import matplotlib.pyplot as plt
import numpy as np

EXECUTABLE_FILE = Path(__file__).parent.parent / 'build' / 'main'

TEST_CASES = {
    0: 100_000,
    1: 500_000,
    2: 1_000_000,
    3: 5_000_000,
    4: 10_000_000,
    5: 20_000_000,
}
MODES = {
    1: "sequential",
    2: "mpi 2 processes",
    3: "mpi 4 processes",
}

MODE_COLORS = {
    1: '#FB707F',
    2: '#008ECC',
    3: '#3CB371',
}

SERIAL_MODE = 1

ARRAY_PATH = 'array.txt'

NUM_OF_PROCESSES = {
    1: 1,
    2: 2,
    3: 4,
}


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
        generate_random_array(length, ARRAY_PATH)
        for mode in MODES:
            for i in range(5):
                st = time.time()
                script_mode = 1 if mode < 2 else 2
                exit_code = subprocess.call(["mpiexec", "-n", str(NUM_OF_PROCESSES[mode]),
                                             EXECUTABLE_FILE, ARRAY_PATH, str(script_mode)])
                if exit_code != 0:
                    print(f'Error in benchmarking. Test Case `{test_case}`, mode `{mode}`')
                execution_time_sec = time.time() - st
                benchmark_results.append(BenchmarkResult(
                    test_case,
                    mode,
                    execution_time_sec
                ))
            print()
        print()

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
    ax.set_title('Производительность методов быстрой сортировки', fontsize=7)
    ax.set_xlabel('Тестовые случаи, [количество элементов в массиве]', fontsize=6)
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

    plt.savefig('task_3_time_report.png', format='png', dpi=200)


def plot_efficiency_graphics(benchmark_results: List[BenchmarkResult]):
    plt.figure(figsize=(4, 4))
    left, width = 0.14, 0.85
    bottom, height = 0.1, 0.85

    ax = plt.axes([left, bottom, width, height])
    ax.set_title('Производительность методов быстрой сортировки', fontsize=7)
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

    plt.savefig('task_3_efficiency_report.png', format='png', dpi=200)


if __name__ == '__main__':
    benchmark()
