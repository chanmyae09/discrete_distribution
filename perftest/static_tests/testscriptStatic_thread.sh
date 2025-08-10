#!/bin/bash
# HOW TO USE:
# Ex:
# bash testscript.sh [num tests] [wightnum]

# Script to test variations of the weighted random selections
echo "Number of tests: $1"
echo "Weightnum: $2"
echo "Threadnum: $3"

echo "uniform_static_sideways_fenwick" > results_${2}_${3}.csv



# Uniform distribution static weight multithread tests
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" "-DTHREADNUM=$3" -o test0 uniform_static.cpp

# Initialize sums
sum0=0

for ((i=1; i<=$1; i++)); do
    result0=$(./test0)

    echo "$result0" >> results_${2}_${3}.csv

    sum0=$(awk "BEGIN { printf \"%.6f\", $sum0 + $result0 }")
done

average0=$(awk "BEGIN { printf \"%.6f\", $sum0 / $1 }")

echo " " >> results_${2}_${3}.csv
echo "$average0" >> results_${2}_${3}.csv
    

