import sys
import matplotlib.pyplot as plt
from matplotlib.pyplot import figure

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: ./plot_benchmark.py (benchmark output file)")
        exit(1)

    benchmark_file_name = sys.argv[1]

    with open(benchmark_file_name) as f:
        while not f.readline().startswith("name,iterations"):
            cols = f.readline()

        benches = {}
        while line := f.readline():
            split_line = line.split(',')
            name = split_line[0]
            time = split_line[2]

            benches[name] = time


        figure(figsize=(30, 30), dpi=80)

        plt.bar(range(len(benches)), list(benches.values()), align='center')
        plt.xticks(range(len(benches)), list(benches.keys()))
        plt.savefig('benchmark.png')


        



