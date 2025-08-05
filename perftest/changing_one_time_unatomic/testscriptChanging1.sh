#!/bin/bash
# HOW TO USE:
# Ex:
# bash testscript.sh [num tests] [wightnum]

# Script to test variations of the weighted random selections
echo "Number of tests: $1"
echo "Weightnum: $2"


echo "normal_changing_SF_bitcast, normal_changing_incremental_LS, normal_changing_leafsum, normal_changing_leaf_sum_split, normal_changing_sf_jump, uniform_changing_SF_bitcast, uniform_changing_incremental_LS, uniform_changing_leafsum, uniform_changing_leaf_sum_split, uniform_changing_sf_jump, weibull_changing_SF_bitcast, weibull_changing_incremental_LS, weibull_changing_leafsum, weibull_changing_leaf_sum_split, weibull_changing_SF_jump" > resultsC1_$2.csv

# Normal distribution changing weight tests
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=sideways_fenwick_selector_bitcast<>"       "-DWEIGHTNUM=$2" -o test0 normal_changing.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=incremental_leaf_sum_tree<>"             "-DWEIGHTNUM=$2" -o test1 normal_changing.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=leaf_sum_tree<>"          "-DWEIGHTNUM=$2" -o test2 normal_changing.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=leaf_sum_tree_split<>"    "-DWEIGHTNUM=$2" -o test3 normal_changing.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=old_sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" -o test4 normal_changing.cpp
#g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=low_storage_selector<>" "-DWEIGHTNUM=$2" -o test5 normal_changing.cpp
#g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=low_storage_selector<>" "-DWEIGHTNUM=$2" -o test7 normal_changing.cpp

#g++ -std=c++20 -I/home/nfs/burnsa/Documents/GitHub/discrete_distribution/lib -O3 ... 

# Uniform distribution changing weight tests
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=sideways_fenwick_selector_bitcast<>"       "-DWEIGHTNUM=$2" -o test6 uniform_changing.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=incremental_leaf_sum_tree<>"             "-DWEIGHTNUM=$2" -o test7 uniform_changing.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=leaf_sum_tree<>"          "-DWEIGHTNUM=$2" -o test8 uniform_changing.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=leaf_sum_tree_split<>"    "-DWEIGHTNUM=$2" -o test9 uniform_changing.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=old_sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" -o test10 uniform_changing.cpp
#g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=low_storage_selector<>"                "-DWEIGHTNUM=$2" -o test11 uniform_changing.cpp

# Weibull distribution changing weight tests
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=sideways_fenwick_selector_bitcast<>"       "-DWEIGHTNUM=$2" -o test12 weibull_changing.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=incremental_leaf_sum_tree<>"             "-DWEIGHTNUM=$2" -o test13 weibull_changing.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=leaf_sum_tree<>"          "-DWEIGHTNUM=$2" -o test14 weibull_changing.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=leaf_sum_tree_split<>"    "-DWEIGHTNUM=$2" -o test15 weibull_changing.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=old_sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" -o test16 weibull_changing.cpp
#g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=low_storage_selector<>"                "-DWEIGHTNUM=$2" -o test17 weibull_changing.cpp




# Initialize sums
sum0=0
sum1=0;
sum2=0
sum3=0
sum4=0

sum6=0
sum7=0;
sum8=0
sum9=0
sum10=0

sum12=0
sum13=0;
sum14=0
sum15=0
sum16=0




for ((i=1; i<=$1; i++)); do
    result0=$(./test0)
    result1=$(./test1)
    result2=$(./test2)
    result3=$(./test3)
    result4=$(./test4)
    result6=$(./test6)
    result7=$(./test7)
    result8=$(./test8)
    result9=$(./test9)
    result10=$(./test10)
    result12=$(./test12)
    result13=$(./test13)
    result14=$(./test14)
    result15=$(./test15)
    result16=$(./test16)
 

    echo "$result0, $result1, $result2, $result3, $result4, $result6, $result7, $result8, $result9, $result10, $result12, $result13, $result14, $result15, $result16" >> resultsC1_$2.csv

    sum0=$(awk "BEGIN { printf \"%.6f\", $sum0 + $result0 }")
    sum1=$(awk "BEGIN { printf \"%.6f\", $sum1 + $result1 }")
    sum2=$(awk "BEGIN { printf \"%.6f\", $sum2 + $result2 }")
    sum3=$(awk "BEGIN { printf \"%.6f\", $sum3 + $result3 }")
    sum4=$(awk "BEGIN { printf \"%.6f\", $sum4 + $result4 }")
    sum6=$(awk "BEGIN { printf \"%.6f\", $sum6 + $result6 }")
    sum7=$(awk "BEGIN { printf \"%.6f\", $sum7 + $result7 }")
    sum8=$(awk "BEGIN { printf \"%.6f\", $sum8 + $result8 }")
    sum9=$(awk "BEGIN { printf \"%.6f\", $sum9 + $result9 }")
    sum10=$(awk "BEGIN { printf \"%.6f\", $sum10 + $result10 }")
    sum12=$(awk "BEGIN { printf \"%.6f\", $sum12 + $result12 }")
    sum13=$(awk "BEGIN { printf \"%.6f\", $sum13 + $result13 }")
    sum14=$(awk "BEGIN { printf \"%.6f\", $sum14 + $result14 }")
    sum15=$(awk "BEGIN { printf \"%.6f\", $sum15 + $result15 }")
    sum16=$(awk "BEGIN { printf \"%.6f\", $sum16 + $result16 }")
done

average0=$(awk "BEGIN { printf \"%.6f\", $sum0 / $1 }")
average1=$(awk "BEGIN { printf \"%.6f\", $sum1 / $1 }")
average2=$(awk "BEGIN { printf \"%.6f\", $sum2 / $1 }")
average3=$(awk "BEGIN { printf \"%.6f\", $sum3 / $1 }")
average4=$(awk "BEGIN { printf \"%.6f\", $sum4 / $1 }")
average6=$(awk "BEGIN { printf \"%.6f\", $sum6 / $1 }")
average7=$(awk "BEGIN { printf \"%.6f\", $sum7 / $1 }")
average8=$(awk "BEGIN { printf \"%.6f\", $sum8 / $1 }")
average9=$(awk "BEGIN { printf \"%.6f\", $sum9 / $1 }")
average10=$(awk "BEGIN { printf \"%.6f\", $sum10 / $1 }")
average12=$(awk "BEGIN { printf \"%.6f\", $sum12 / $1 }")
average13=$(awk "BEGIN { printf \"%.6f\", $sum13 / $1 }")
average14=$(awk "BEGIN { printf \"%.6f\", $sum14 / $1 }")
average15=$(awk "BEGIN { printf \"%.6f\", $sum15 / $1 }")
average16=$(awk "BEGIN { printf \"%.6f\", $sum16 / $1 }")


echo " " >> resultsC1_$2.csv
echo "$average0, $average1, $average2, $average3, $average4, $average6, $average7, $average8, $average9, $average10, $average12, $average13, $average14, $average15, $average16" >> resultsC1_$2.csv
echo "Test completed. Results saved in resultsC1_$2.csv"

