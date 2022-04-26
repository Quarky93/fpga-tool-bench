# FpgaToolBench
Drag racing computers with FPGA design tools.

## Xilinx Vitis + Vivado Benchmarks
Ran with `-in_memory` flag.

| CPU                         | RAM                             | Disk                | OS                               | Blake {HLS + Synth + PnR} (minutes) |
|-----------------------------|---------------------------------|---------------------|----------------------------------|-------------------------------------|
| Intel i9-12900K - 5.2GHz OC | 128GB-DDR5-4800MT-CL36-36-36-77 | Samsung 980 Pro 2TB | Windows + WSL (Ubuntu 20.04 LTS) | 10:17                               |
| Intel i9-12900K - 5.0GHz    | 128GB-DDR5-4800MT-CL36-36-36-77 | Samsung 980 Pro 2TB | Windows + WSL (Ubuntu 20.04 LTS) | 10:39                               |
| Intel i9-12900K - 5.0GHz    | 128GB-DDR4-3600MT-CL18-22-22-42 | Samsung 980 Pro 2TB | Windows + WSL (Ubuntu 20.04 LTS) | 10:46                               |

Extra observations:
- WSL2 vs bare metal Linux made no noticable difference (within margin of error).
- Vivado installed into ramdisk made no difference.
- Working directory on ramdisk made no difference.
