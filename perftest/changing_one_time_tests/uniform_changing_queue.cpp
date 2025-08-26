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
#include "sideways_fenwick_selector_lock.hpp"
#include "old_sideways_fenwick_selector.hpp"
#include "sideways_fenwick_selector_bitcast.hpp"
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
  int TOTAL_ITERATIONS =1200000 ;
  int weight_per_thread = WEIGHTNUM/ THREADS;
  std::uniform_real_distribution<double>d(1,10); 
  std::default_random_engine generator;
  std::vector<double> weights = {};
  weights.reserve(WEIGHTNUM);
  for(int i = 0; i < WEIGHTNUM; i++){
    weights.push_back(d(generator));
  }	      

  double minweight = *std::min_element(weights.begin(), weights.end());
  for(int i = 0; i < WEIGHTNUM; i++){
    weights[i] -= minweight;
  }	      

  //start time
  struct timeval start, end;
  std::vector<double>glb_weight(THREADS, 1);
  std::unique_ptr<WRSLIB> Tselector;
  if (THREADS > 1) {
    Tselector = std::make_unique<WRSLIB>(glb_weight.begin(), glb_weight.end());
}

  // WRSLIB selector(weights.begin(), weights.end());
  std::vector<std::unique_ptr<WRSLIB>> Lselectors;
  Lselectors.reserve(THREADS);
  int base = WEIGHTNUM / THREADS;
  int rem  = WEIGHTNUM % THREADS;

  int offset = 0;
  for(int i = 0; i< THREADS; ++i){
    int chunk = base ; //+ (i==0 ? rem:0);
    auto first = weights.begin()+ offset;
    auto last = first+ chunk;
    Lselectors.emplace_back(std::make_unique<WRSLIB>(first, last));
    offset+=chunk;
  }


  std::vector<std::atomic<std::size_t>> mock_queue(THREADS);
  for (auto &a : mock_queue) {
      a.store(1, std::memory_order_relaxed);
  }


  gettimeofday(&start, NULL);

  std::vector<std::thread> threads;
  int iter_per_thread = TOTAL_ITERATIONS/ THREADS;
  
  for (int t = 0; t < THREADS; ++t) {
    threads.emplace_back([&,t]() {
      std::default_random_engine thread_gen(std::random_device{}());
      for (int i = 0; i < iter_per_thread; ++i) {
        int i0 = (THREADS > 1) ? (*Tselector)(thread_gen) : 0;
        int i1 = (*Lselectors[i0])(thread_gen);
        mock_queue[i1%THREADS].fetch_add(i0);
        int i2 = (*Lselectors[t])(thread_gen);
        Lselectors[t]->update_weight(i2, std::max<double>(0.0f, d(thread_gen) - minweight));
      }
    });
  }
  for (auto& th : threads) th.join();
  
  // end time
  gettimeofday(&end, NULL);
  double elapsedtime_sec = double(end.tv_sec - start.tv_sec) + 
  double(end.tv_usec - start.tv_usec)/1000000.0;
  std::cout << elapsedtime_sec << std::endl;

}
