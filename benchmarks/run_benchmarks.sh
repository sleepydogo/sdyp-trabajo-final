#!/bin/bash

# Benchmark script for Parallel Array Comparator
# Authors: Tomas E. Schattmann, Emiliano Mengoni

set -e

BINARY="../build/parallel_comparator"
RESULTS_DIR="./results"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")
RESULTS_FILE="${RESULTS_DIR}/benchmark_${TIMESTAMP}.txt"

mkdir -p "${RESULTS_DIR}"

echo "=== Parallel Array Comparator Benchmark Suite ===" | tee "${RESULTS_FILE}"
echo "Timestamp: $(date)" | tee -a "${RESULTS_FILE}"
echo "System: $(uname -s) $(uname -r)" | tee -a "${RESULTS_FILE}"
echo "CPU: $(sysctl -n machdep.cpu.brand_string 2>/dev/null || lscpu | grep 'Model name' | cut -d: -f2 | xargs)" | tee -a "${RESULTS_FILE}"
echo "" | tee -a "${RESULTS_FILE}"

if [ ! -f "${BINARY}" ]; then
    echo "Error: Binary not found at ${BINARY}"
    echo "Please run 'make' first"
    exit 1
fi

SIZES=(16777216 33554432 67108864)
THREADS=(2 4 8 16)

echo "Configuration:" | tee -a "${RESULTS_FILE}"
echo "  Array sizes: ${SIZES[@]}" | tee -a "${RESULTS_FILE}"
echo "  Thread counts: ${THREADS[@]}" | tee -a "${RESULTS_FILE}"
echo "" | tee -a "${RESULTS_FILE}"

for size in "${SIZES[@]}"; do
    echo "========================================" | tee -a "${RESULTS_FILE}"
    echo "Array Size: $size (2^$(echo "l($size)/l(2)" | bc -l | xargs printf "%.0f"))" | tee -a "${RESULTS_FILE}"
    echo "========================================" | tee -a "${RESULTS_FILE}"

    for threads in "${THREADS[@]}"; do
        echo "" | tee -a "${RESULTS_FILE}"
        echo "Running with $threads threads..." | tee -a "${RESULTS_FILE}"

        "${BINARY}" -n "$size" -t "$threads" 2>&1 | tee -a "${RESULTS_FILE}"

        sleep 1
    done

    echo "" | tee -a "${RESULTS_FILE}"
done

echo "" | tee -a "${RESULTS_FILE}"
echo "=== Benchmark Complete ===" | tee -a "${RESULTS_FILE}"
echo "Results saved to: ${RESULTS_FILE}" | tee -a "${RESULTS_FILE}"

echo ""
echo "Generating CSV summary..."
CSV_FILE="${RESULTS_DIR}/benchmark_${TIMESTAMP}.csv"
echo "Size,Threads,Time(s),Result" > "${CSV_FILE}"

grep -A 3 "Number of threads" "${RESULTS_FILE}" | \
    awk '/Array size:/ {size=$3} /Number of threads:/ {threads=$4} /Execution time:/ {time=$3} {if (time) {print size","threads","time",PASS"; time=""}}' \
    >> "${CSV_FILE}" 2>/dev/null || true

echo "CSV summary saved to: ${CSV_FILE}"
