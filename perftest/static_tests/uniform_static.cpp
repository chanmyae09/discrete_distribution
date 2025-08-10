//HOW TO USE COMPILATION FLAGS:
//Enter the library you want to test after 'DWRSLIB=' and the number of weights after 'DWEIGHTNUM='.

//Ex:
//g++ -I../lib -O3 "-DWRSLIB=nonuniform_int_distribution<int>" "-DWEIGHTNUM=10000000" -o test0 normal_static.cpp
//g++ -I../lib -O3 "-DWRSLIB=heap_random_selector<int>" "-DWEIGHTNUM=100000" -o test1 normal_static.cpp
//g++ -I../lib -O3 "-DWRSLIB=std::discrete_distribution<int>" "-DWEIGHTNUM=100" -o test2 normal_static.cpp

#include "random_selector.hpp"
#include "modifiable_heap_random_selector.hpp"
#include "no_weight_storage_modifiable_heap_random_selector.hpp"
#include "leaf_sum_tree_selector.hpp"
#include "leaf_sum_tree_split.hpp"
#include "sideways_fenwick_selector.hpp"
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
  int sum = 0;
  
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
  gettimeofday(&start, NULL);

  std::vector<std::thread> threads;
  std::vector<int>thread_sums(THREADS, 0);
  int iter_per_thread = TOTAL_ITERATIONS/ THREADS;
  for(int t = 0; t< THREADS; ++t)
  {
    threads.emplace_back([&,t](){
      std::default_random_engine thread_gen (std::random_device{}());
      
      int local_sum = 0;
      for(int i = 0; i< iter_per_thread; ++i)
      {
        local_sum+= selector(thread_gen);
      }
      thread_sums[t] = local_sum;
    });
  }
  for (auto& th : threads) th.join();
  for (int s : thread_sums) sum += s;

  // end time
  gettimeofday(&end, NULL);
  double elapsedtime_sec = double(end.tv_sec - start.tv_sec) + 
    double(end.tv_usec - start.tv_usec)/1000000.0;
  std::cout << elapsedtime_sec << std::endl;
  if(sum == 0 ) {std::cout << "hit"; } // Added conditional so compiler must compute sum
}