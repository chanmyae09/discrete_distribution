#!/bin/bash
# HOW TO USE:
# Ex:
# bash testscript.sh [num tests] [wightnum]

# Script to test variations of the weighted random selections
echo "Number of tests: $1"
echo "Weightnum: $2"

echo "uniform_changing_sideways_fenwick_11select_1change, uniform_changing_sideways_fenwick_11thread_1thread, uniform_changing_sideways_fenwick_11thread_1thread_dummy" > results_${2}.csv



# Uniform distribution static weight multithread tests
g++ -std=c++20 -I../../lib -O3 -pthread "-DWRSLIB=dense::stochastic::sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" -o test0 uniform_changing_11select_1change.cpp
g++ -std=c++20 -I../../lib -O3 -pthread "-DWRSLIB=dense::stochastic::sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" -o test1 uniform_changing_11thread_1thread.cpp
g++ -std=c++20 -I../../lib -O3 -pthread "-DWRSLIB=dense::stochastic::sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" -o test2 uniform_changing_11thread_1thread_dummy.cpp
# Initialize sums
sum0=0
sum1=0
sum1=0

for ((i=1; i<=$1; i++)); do
    # result0=$(./test0 | tee -a results_${2}_${3}.csv | grep -E '^[0-9.]+$')
    # sum0=$(awk "BEGIN { printf \"%.6f\", $sum0 + $result0 }")

    result0=$(./test0)
    result1=$(./test1)
    result2=$(./test2)

    echo "$result0,$result1,$result2" >> results_${2}.csv

    sum0=$(awk "BEGIN { printf \"%.6f\", $sum0 + $result0 }")
    sum1=$(awk "BEGIN { printf \"%.6f\", $sum1 + $result1 }")
    sum2=$(awk "BEGIN { printf \"%.6f\", $sum2 + $result2 }")
done

average0=$(awk "BEGIN { printf \"%.6f\", $sum0 / $1 }")
average1=$(awk "BEGIN { printf \"%.6f\", $sum1 / $1 }")
average2=$(awk "BEGIN { printf \"%.6f\", $sum2 / $1 }")

echo " " >> results_${2}.csv
echo "$average0,$average1,$average2" >> results_${2}.csv
    

