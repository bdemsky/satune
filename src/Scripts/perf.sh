#!/bin/bash
echo "Start profiling: $@"
echo 0 > /proc/sys/kernel/nmi_watchdog
perf stat -e page-faults,L1-dcache-loads:HG,L1-dcache-load-misses:HG,LLC-loads:HG,LLC-load-misses:HG $@
perf stat -e LLC-loads:HG,LLC-load-misses:HG $@
echo 1 > /proc/sys/kernel/nmi_watchdog
