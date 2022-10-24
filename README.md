# FpgaToolBench
Drag racing computers with FPGA design tools.

## Vivado 2022.1
Lower runtime is better!
| CPU                      | RAM                | OS           | Runtime (seconds) |
|--------------------------|--------------------|--------------|-------------------|
| Intel i9-13900K (5.8GHz) | DDR5 4800MT/s CL36 | Ubuntu 20.04 | 8,954             |
| Intel i9-12900K (5.2GHz) | DDR5 4800MT/s CL36 | Ubuntu 20.04 | 9,833             |
| AMD R9-5950X (4.9GHz)    | DDR4 3600MT/s CL18 | Ubuntu 20.04 | 13,779            |
| AMD EYPC 7313P (3.7GHz)  | DDR4 3200MT/s CL22 | Ubuntu 20.04 | 16,501            |

# Instructions
## Prerequisites
Please use a OS supported by the Vivado version you are using. E.g. Ubuntu 20.04 for Vivado 2022.1.

Install perf tools:
```bash
sudo apt install linux-tools-`uname -r`
```

Set perf permissions (do this at your own risk):
```bash
sudo sh -c 'echo -1 >/proc/sys/kernel/perf_event_paranoid'
```

Install python dependencies:
```bash
pip3 install py-markdown-table
```

Install Vivado and Vitis HLS.
You must select both of the following options during installation:
- "Install devices for Alveo and Xilinx edge acceleration platforms"
- "Virtex UltraScale+ HBM"

Make sure vivado and vitis_hls are in the path:
```bash
source /tools/Xilinx/Vivado/2022.1/settings64.sh
```

## Running the benchmarks
Run the benchmarks:
```bash
make benchmarks
```

Note: do not run the make command in parallel mode.
Note: for hybrid CPUs such as Intel Alderlake, pin the process to the p-cores for correct performance counter metrics:
```bash
# For 8 p-cores (first 16 threads)
taskset -c 0-15 make benchmarks
```

## Commit results
Create the directory:
```bash
mkdir -p results/vivado_{MAJOR}_{MINOR}/{CPU_VENDOR}_{CPU_PART}_{CPU_MAX_FREQ}_{DDR_VERSION}_{DDR_SPEED}_{DDR_CAS_LATENCY}
# For example:
mkdir -p results/vivado_2022_1/intel_12900k_5_2ghz_ddr5_4800mt_cl36
```

Copy the output files to the above directory:
```bash
build/README.md
build/*.json
build/*.csv
```

Attach a link to your geekbench5 score in the README.md within your results directory.
