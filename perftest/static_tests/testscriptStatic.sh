#!/bin/bash
# HOW TO USE:
# Ex:
# bash testscript.sh [num tests] [wightnum]

# Script to test variations of the weighted random selections
echo "Number of tests: $1"
echo "Weightnum: $2"
echo "Threadnum: $3"

echo "uniform_static_sideways_fenwick" > results_${2}_${3}.csv

# Normal distribution static weight tests
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=nonuniform_int_distribution<>"       "-DWEIGHTNUM=$2" -o test0 normal_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=fast_random_selector<>"             "-DWEIGHTNUM=$2" -o test1 normal_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=std::discrete_distribution<>"       "-DWEIGHTNUM=$2" -o test2 normal_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=nonuniform_int_distribution<>" "-DWEIGHTNUM=$2" -o test3 normal_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=leaf_sum_tree<>"          "-DWEIGHTNUM=$2" -o test4 normal_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=leaf_sum_tree_split<>"    "-DWEIGHTNUM=$2" -o test5 normal_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" -o test6 normal_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=low_storage_selector<>" "-DWEIGHTNUM=$2" -o test7 normal_static.cpp
#g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=low_storage_selector<>" "-DWEIGHTNUM=$2" -o test7 normal_static.cpp

#g++ -std=c++20 -I/home/nfs/burnsa/Documents/GitHub/discrete_distribution/lib -O3 ... 

# Uniform distribution static weight tests
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=nonuniform_int_distribution<>"       "-DWEIGHTNUM=$2" -o test8 uniform_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=fast_random_selector<>"             "-DWEIGHTNUM=$2" -o test9 uniform_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=std::discrete_distribution<>"       "-DWEIGHTNUM=$2" -o test10 uniform_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=nonuniform_int_distribution<>" "-DWEIGHTNUM=$2" -o test11 uniform_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=leaf_sum_tree<>"          "-DWEIGHTNUM=$2" -o test12 uniform_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=leaf_sum_tree_split<>"    "-DWEIGHTNUM=$2" -o test13 uniform_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" -o test14 uniform_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=low_storage_selector<>"                "-DWEIGHTNUM=$2" -o test15 uniform_static.cpp

# Weibull distribution static weight tests
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=nonuniform_int_distribution<>"       "-DWEIGHTNUM=$2" -o test16 weibull_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=fast_random_selector<>"             "-DWEIGHTNUM=$2" -o test17 weibull_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=std::discrete_distribution<>"       "-DWEIGHTNUM=$2" -o test18 weibull_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=nonuniform_int_distribution<>" "-DWEIGHTNUM=$2" -o test19 weibull_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=leaf_sum_tree<>"          "-DWEIGHTNUM=$2" -o test20 weibull_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=leaf_sum_tree_split<>"    "-DWEIGHTNUM=$2" -o test21 weibull_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" -o test22 weibull_static.cpp
# g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=low_storage_selector<>"                "-DWEIGHTNUM=$2" -o test23 weibull_static.cpp


# Uniform distribution static weight multithread tests
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" "-DTHREADNUM=$3" -o test0 uniform_static.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" "-DTHREADNUM=$3" -o test1 uniform_static.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" "-DTHREADNUM=$3" -o test2 uniform_static.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" "-DTHREADNUM=$3" -o test3 uniform_static.cpp
g++ -std=c++20 -I../../lib -O3 "-DWRSLIB=sideways_fenwick_selector<>" "-DWEIGHTNUM=$2" "-DTHREADNUM=$3" -o test4 uniform_static.cpp
# Initialize sums
sum0=0
sum1=0
sum2=0
sum3=0
sum4=0
# sum5=0
# sum6=0
# sum7=0
# sum8=0
# sum9=0
# sum10=0
# sum11=0
# sum12=0
# sum13=0
# sum14=0
# sum15=0
# sum16=0
# sum17=0
# sum18=0
# sum19=0
# sum20=0
# sum21=0
# sum22=0
# sum23=0


for ((i=1; i<=$1; i++)); do
    result0=$(./test0)
    result1=$(./test1)
    result2=$(./test2)
    result3=$(./test3)
    result4=$(./test4)
    # result5=$(./test5)
    # result6=$(./test6)
    # result7=$(./test7)
    # result8=$(./test8)
    # result9=$(./test9)
    # result10=$(./test10)
    # result11=$(./test11)
    # result12=$(./test12)
    # result13=$(./test13)
    # result14=$(./test14)
    # result15=$(./test15)
    # result16=$(./test16)
    # result17=$(./test17)
    # result18=$(./test18)
    # result19=$(./test19)
    # result20=$(./test20)
    # result21=$(./test21)
    # result22=$(./test22)
    # result23=$(./test23)

    echo "$result0, $result1, $result2, $result3, $result4" >> results_${2}_${3}.csv

    sum0=$(awk "BEGIN { printf \"%.6f\", $sum0 + $result0 }")
    sum1=$(awk "BEGIN { printf \"%.6f\", $sum1 + $result1 }")
    sum2=$(awk "BEGIN { printf \"%.6f\", $sum2 + $result2 }")
    sum3=$(awk "BEGIN { printf \"%.6f\", $sum3 + $result3 }")
    sum4=$(awk "BEGIN { printf \"%.6f\", $sum4 + $result4 }")
    # sum5=$(awk "BEGIN { printf \"%.6f\", $sum5 + $result5 }")
    # sum6=$(awk "BEGIN { printf \"%.6f\", $sum6 + $result6 }")
    # sum7=$(awk "BEGIN { printf \"%.6f\", $sum7 + $result7 }")
    # sum8=$(awk "BEGIN { printf \"%.6f\", $sum8 + $result8 }")
    # sum9=$(awk "BEGIN { printf \"%.6f\", $sum9 + $result9 }")
    # sum10=$(awk "BEGIN { printf \"%.6f\", $sum10 + $result10 }")
    # sum11=$(awk "BEGIN { printf \"%.6f\", $sum11 + $result11 }")
    # sum12=$(awk "BEGIN { printf \"%.6f\", $sum12 + $result12 }")
    # sum13=$(awk "BEGIN { printf \"%.6f\", $sum13 + $result13 }")
    # sum14=$(awk "BEGIN { printf \"%.6f\", $sum14 + $result14 }")
    # sum15=$(awk "BEGIN { printf \"%.6f\", $sum15 + $result15 }")
    # sum16=$(awk "BEGIN { printf \"%.6f\", $sum16 + $result16 }")
    # sum17=$(awk "BEGIN { printf \"%.6f\", $sum17 + $result17 }")
    # sum18=$(awk "BEGIN { printf \"%.6f\", $sum18 + $result18 }")
    # sum19=$(awk "BEGIN { printf \"%.6f\", $sum19 + $result19 }")
    # sum20=$(awk "BEGIN { printf \"%.6f\", $sum20 + $result20 }")
    # sum21=$(awk "BEGIN { printf \"%.6f\", $sum21 + $result21 }")
    # sum22=$(awk "BEGIN { printf \"%.6f\", $sum22 + $result22 }")
    # sum23=$(awk "BEGIN { printf \"%.6f\", $sum23 + $result23 }")
done

average0=$(awk "BEGIN { printf \"%.6f\", $sum0 / $1 }")
average1=$(awk "BEGIN { printf \"%.6f\", $sum1 / $1 }")
average2=$(awk "BEGIN { printf \"%.6f\", $sum2 / $1 }")
average3=$(awk "BEGIN { printf \"%.6f\", $sum3 / $1 }")
average4=$(awk "BEGIN { printf \"%.6f\", $sum4 / $1 }")
# average5=$(awk "BEGIN { printf \"%.6f\", $sum5 / $1 }")
# average6=$(awk "BEGIN { printf \"%.6f\", $sum6 / $1 }")
# average7=$(awk "BEGIN { printf \"%.6f\", $sum7 / $1 }")
# average8=$(awk "BEGIN { printf \"%.6f\", $sum8 / $1 }")
# average9=$(awk "BEGIN { printf \"%.6f\", $sum9 / $1 }")
# average10=$(awk "BEGIN { printf \"%.6f\", $sum10 / $1 }")
# average11=$(awk "BEGIN { printf \"%.6f\", $sum11 / $1 }")
# average12=$(awk "BEGIN { printf \"%.6f\", $sum12 / $1 }")
# average13=$(awk "BEGIN { printf \"%.6f\", $sum13 / $1 }")
# average14=$(awk "BEGIN { printf \"%.6f\", $sum14 / $1 }")
# average15=$(awk "BEGIN { printf \"%.6f\", $sum15 / $1 }")
# average16=$(awk "BEGIN { printf \"%.6f\", $sum16 / $1 }")
# average17=$(awk "BEGIN { printf \"%.6f\", $sum17 / $1 }")
# average18=$(awk "BEGIN { printf \"%.6f\", $sum18 / $1 }")
# average19=$(awk "BEGIN { printf \"%.6f\", $sum19 / $1 }")
# average20=$(awk "BEGIN { printf \"%.6f\", $sum20 / $1 }")
# average21=$(awk "BEGIN { printf \"%.6f\", $sum21 / $1 }")
# average22=$(awk "BEGIN { printf \"%.6f\", $sum22 / $1 }")
# average23=$(awk "BEGIN { printf \"%.6f\", $sum23 / $1 }")

echo " " >> results_${2}_${3}.csv
echo "$average0, $average1, $average2, $average3, $average4" >> results_${2}_${3}.csv
    

