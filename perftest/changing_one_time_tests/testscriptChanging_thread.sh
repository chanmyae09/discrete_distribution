#!/bin/bash
# HOW TO USE:
# Ex:
# bash testscript.sh [num tests] [wightnum]

# Script to test variations of the weighted random selections
echo "Number of tests: $1"
echo "Weightnum: $2"
echo "Threadnum: $3"

echo "uniform_changing_sideways_fenwick, uniform_changing_sideways_fenwick_lock" > results_${2}_${3}.csv



# Uniform distribution static weight multithread tests
g++ -std=c++20 -I../../lib -O3 -pthread "-DWRSLIB=dense::stochastic::sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" "-DTHREADNUM=$3" -o test0 uniform_changing.cpp
g++ -std=c++20 -I../../lib -O3 -pthread "-DWRSLIB=dense::stochastic::sideways_fenwick_selector_lock<>" "-DWEIGHTNUM=$2" "-DTHREADNUM=$3" -o test1 uniform_changing.cpp
# Initialize sums
sum0=0
sum1=0

for ((i=1; i<=$1; i++)); do
    # result0=$(./test0 | tee -a results_${2}_${3}.csv | grep -E '^[0-9.]+$')
    # sum0=$(awk "BEGIN { printf \"%.6f\", $sum0 + $result0 }")

    result0=$(./test0)
    result1=$(./test1)

    echo "$result0,$result1" >> results_${2}_${3}.csv

    sum0=$(awk "BEGIN { printf \"%.6f\", $sum0 + $result0 }")
    sum1=$(awk "BEGIN { printf \"%.6f\", $sum1 + $result1 }")
done

average0=$(awk "BEGIN { printf \"%.6f\", $sum0 / $1 }")
average1=$(awk "BEGIN { printf \"%.6f\", $sum1 / $1 }")

echo " " >> results_${2}_${3}.csv
echo "$average0,$average1" >> results_${2}_${3}.csv
    

