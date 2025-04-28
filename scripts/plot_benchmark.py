import sys
import matplotlib.pyplot as plt
from matplotlib.pyplot import figure

def strip_quotes(s):
    return s.replace("'", "").replace('"', '')

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
            print(split_line[0])
            name_parts = split_line[0].split('/')

            if len(name_parts) > 1:
                name = strip_quotes(name_parts[0])
                sample_size = strip_quotes(name_parts[1])
                
                time = split_line[2]

                bench_times = benches.get(name, {}) 
                bench_times[sample_size] = time

                benches[name] = bench_times

            print(benches)


        figure(figsize=(30, 30), dpi=80)

        for bench_name, bench_results in benches.items():
            plt.bar(range(len(bench_results)), list(bench_results.values()), align='center')
            plt.xticks(range(len(bench_results)), list(bench_results.keys()))
            plt.savefig(f'{bench_name}_benchmark.png')

        


