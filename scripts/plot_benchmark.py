import sys
import matplotlib.pyplot as plt
from matplotlib.pyplot import figure


# in order to generate benchmark output file,
# do BENCHMARK_FORMAT=csv mpirun -n (num processes) (benchname) 

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

        plt.ylabel("Runtime")

        xticks = [10 ** x for x in range(1, 13)]

        for bench_name, bench_results in benches.items():
            plt.plot(xticks, bench_results, label=bench_name)

        plt.savefig(f'{benchmark_file_name}.png')

        


