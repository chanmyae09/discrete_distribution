//HOW TO USE COMPILATION FLAGS:
//Enter the library you want to test after 'DWRSLIB=' and the number of weights after 'DWEIGHTNUM='.

//Ex:
//g++ -I../lib -O3 "-DWRSLIB=nonuniform_int_distribution<int>" "-DWEIGHTNUM=10000000" -o test0 uniform_changing.cpp
//g++ -I../lib -O3 "-DWRSLIB=heap_random_selector<int>" "-DWEIGHTNUM=100000" -o test1 uniform_changing.cpp

#include "random_selector.hpp"
#include "modifiable_heap_random_selector.hpp"
#include "no_weight_storage_modifiable_heap_random_selector.hpp"
#include "leaf_sum_tree_selector.hpp"
#include "leaf_sum_tree_split.hpp"
#include "sideways_fenwick_selector.hpp"
#include "old_sideways_fenwick_selector.hpp"
#include "sideways_fenwick_selector_bitcast.hpp"
#include "sideways_fenwick_selector_atomic.hpp"
#include "incremental_leaf_sum_tree.hpp"
#include <sys/time.h>
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <thread>     
using namespace dense::stochastic;

int main() {
  int THREADS = THREADNUM;
  int TOTAL_ITERATIONS = 1200000;
  std::uniform_real_distribution<float> d(1,10); 
  std::default_random_engine generator;
  std::vector<float> weights = {};
  weights.reserve(WEIGHTNUM);
  for(int i = 0; i < WEIGHTNUM; i++){
    weights.push_back(d(generator));
  }	      

  float minweight = *std::min_element(weights.begin(), weights.end());
  for(int i = 0; i < WEIGHTNUM; i++){
    weights[i] -= minweight;
  }	      

  //start time
  struct timeval start, end;
  WRSLIB selector(weights.begin(), weights.end());
  selector.PrintTreePublic();
  gettimeofday(&start, NULL);

  std::vector<std::thread> threads;
  std::vector<int>thread_sums(THREADS, 0);
  int iter_per_thread = TOTAL_ITERATIONS/ THREADS;
  
  for (int t = 0; t < THREADS; ++t) {
    threads.emplace_back([&, t]() {
      std::default_random_engine thread_gen(std::random_device{}());
      for (int i = 0; i < iter_per_thread; ++i) {
        int index = selector(thread_gen);
        // selector.update_weight(index, std::max<float>(0.0f, d(thread_gen) - minweight));
      }
    });
  }
  for (auto& th : threads) th.join();
  
  // end time
  gettimeofday(&end, NULL);
  double elapsedtime_sec = double(end.tv_sec - start.tv_sec) + 
    double(end.tv_usec - start.tv_usec)/1000000.0;
  std::cout << elapsedtime_sec << std::endl;
  selector.PrintTreePublic();
  
}

 