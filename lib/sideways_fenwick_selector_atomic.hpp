#ifndef SIDEWAYS_FENWICK_RANDOM_SELECTOR_ATOMIC
#define SIDEWAYS_FENWICK_RANDOM_SELECTOR_ATOMIC
////////////////////////////////IMPORTANT/////////////////////////////////////
//must compile this file using the -std=c++20 flag
//compiling testingDistributions: g++ -std=c++20 test/testingDistributions.cpp


#include <utility>
#include <vector>
#include <functional>
#include <type_traits>
#include <iostream>
#include <fstream>
#include <limits>
#include <bit>
#include <bitset>
#include <random>
#include <emmintrin.h>
#include <atomic>
#include <iterator> 


#include "completetree_atomic.hpp"

namespace dense {
namespace stochastic {


  template <
    typename I = size_t, size_t precision = std::numeric_limits<Real>::digits
  >
  class sideways_fenwick_selector_atomic :
    //Extends a complete tree...
    protected complete_tree<I, std::atomic<Real> >
  {
    public:

      using size_type = std::ptrdiff_t;
      using index_type = I;
      using This = sideways_fenwick_selector_atomic<index_type, precision>;
      using node_type = index_type;
      using value_type = Real;
      using entry_type = Real;
      using iterator = value_type*;
      using const_iterator = value_type const*;
      using reference = value_type&;
      using const_reference = value_type const&;
      using BaseTree = complete_tree<node_type, std::atomic<value_type>>;
      using BaseTree::atomic_load;
      using BaseTree::atomic_store;
      using BaseTree::atomic_fetch_add;


      sideways_fenwick_selector_atomic() = delete;

      template<typename InputIt>
      sideways_fenwick_selector_atomic(InputIt first, InputIt last) :
        BaseTree(last-first) 
      {
        //std::cout<<"__________constructor__________"<<std::endl;
        // size_t n = static_cast<size_t>(last - first);
        node_type idx = BaseTree::root(); 
        for (InputIt it = first; it != last; ++it) {
          atomic_store(idx, static_cast<Real>(*it), std::memory_order_seq_cst);
          ++idx;
        }
        node_type lastNonLeaf = (BaseTree::entry_count())/2;
        if (BaseTree::entry_count() % 2 == 0) {
          Real v = atomic_load(lastNonLeaf, std::memory_order_seq_cst);
          v += atomic_load(BaseTree::left_of(lastNonLeaf), std::memory_order_seq_cst);
          atomic_store(lastNonLeaf, v, std::memory_order_seq_cst);
        } else {
            Real v = atomic_load(lastNonLeaf, std::memory_order_seq_cst);
            v += atomic_load(BaseTree::left_of(lastNonLeaf),  std::memory_order_seq_cst);
            v += atomic_load(BaseTree::right_of(lastNonLeaf), std::memory_order_seq_cst);
            atomic_store(lastNonLeaf, v, std::memory_order_seq_cst);
        }

        // internal nodes accumulate children
        for (node_type node = lastNonLeaf - 1; node > 0; --node) {
          Real v = atomic_load(node, std::memory_order_seq_cst);
          v += atomic_load(BaseTree::left_of(node),  std::memory_order_seq_cst);
          v += atomic_load(BaseTree::right_of(node), std::memory_order_seq_cst);
          atomic_store(node, v, std::memory_order_seq_cst);
        }

        // total_weight snapshot (local var only)
        total_weight.store(atomic_load(this->root(), std::memory_order_seq_cst),std::memory_order_seq_cst);

        // subtract right subtree on the way down
        for (node_type node = BaseTree::root(); node < lastNonLeaf; ++node) {
          Real v = atomic_load(node, std::memory_order_seq_cst);
          v -= atomic_load(BaseTree::right_of(node), std::memory_order_seq_cst);
          atomic_store(node, v, std::memory_order_seq_cst);
        }
        if (BaseTree::entry_count() % 2 == 1) {
          Real v = atomic_load(lastNonLeaf, std::memory_order_seq_cst);
          v -= atomic_load(BaseTree::right_of(lastNonLeaf), std::memory_order_seq_cst);
          atomic_store(lastNonLeaf, v, std::memory_order_seq_cst);
        }
      }

      sideways_fenwick_selector_atomic(sideways_fenwick_selector_atomic const&) = default;

      sideways_fenwick_selector_atomic(sideways_fenwick_selector_atomic &&) = default;

      sideways_fenwick_selector_atomic& operator=(sideways_fenwick_selector_atomic const&) = default;

      sideways_fenwick_selector_atomic& operator=(sideways_fenwick_selector_atomic &&) = default;

      ~sideways_fenwick_selector_atomic() = default;


      // //Methods of WeightSum we want to make available
      
      template<class URNG>
      index_type operator()(URNG& g) {
        Real TW = total_weight.load(std::memory_order_seq_cst);
        Real target = std::generate_canonical<Real, precision, URNG>(g) * TW;
        node_type node = this->root();
        node_type lastNonLeaf = BaseTree::entry_count()/2;
        //std::cout<<"last non leaf is "<<lastNonLeaf<< " target is "<<target<<std::endl;
        while(node<lastNonLeaf){
          Real left = atomic_load(node, std::memory_order_seq_cst);
          if (target<left){
            node = BaseTree::left_of(node);
          }
          else{
            target -=left;
            node=BaseTree::right_of(node);
          }
        }
        //this is to make sure that there is a right child (if I did this in the loop it would check that there is a right child every time which is unecessary)
        if (node==lastNonLeaf){
          //std::cout<<"node is last non leaf"<<std::endl;
          Real left = atomic_load(node, std::memory_order_seq_cst);
          if (target<left){
            node = BaseTree::left_of(node);
          }
          else if(BaseTree::right_of(node)<BaseTree::size()){
            //std::cout<<"going right from last non leaf"<<std::endl;
            target -=left;
            node=BaseTree::right_of(node);
            //std::cout<<"went right node is "<<node<<" and target is "<<target<<std::endl;
          }
        }
        Real left = atomic_load(node, std::memory_order_seq_cst);
        if (target<left){
            return id_of(node);
          }
        else{
          return id_of(nextNode(node));
          
        }
      }

      void update_weight(index_type i, Real new_weight) {
        update_weight_of_node(node_of(i),new_weight);
      }

      Real get_weight(index_type i) {
        return weight_of(node_of(i));
      }

    //   Real total_weight() const { return WeightSum::total_weight(); }

      void push_back(const entry_type& e) {
        value_type v = e;
        BaseTree::add_entry(0);
        update_weight_of_node(BaseTree::last(),v);
      }
      void push_back(const entry_type&& e) {
        value_type v = e;
        BaseTree::add_entry(0);
        update_weight_of_node(BaseTree::last(),v);
      }
      void pop_back() {
        update_weight_of_node(BaseTree::last(),0);
        BaseTree::pop_back();
      }

    private:

      std::atomic<Real> total_weight{0};
      //helper function to return the next node to update
      node_type nextNode(node_type currentNode){
        return currentNode>>(((std::countr_one(currentNode)))+1);
      }
      //returns the sum of the left subtree and the node itself
      // Real& weightsum_of(node_type n) {
      //   return this->value_of(n);
      // }

      // const Real& weightsum_of(node_type n) const {
      //   return const_cast<This*>(this)->weightsum_of(n);
      // }

      
      void update_weight_of_node(node_type givenNode, Real new_weight) {
        Real old_w = weight_of(givenNode);               // uses atomic_load() now
        Real delta = new_weight - old_w;
        if (delta == 0) return;
        total_weight.fetch_add(delta, std::memory_order_seq_cst);
        node_type node = givenNode;
        bool add_here = true;     
        while (node >= BaseTree::root()) {
          if (add_here) {
            if (node == BaseTree::root()) {
              // publish point (when we flip to real atomics later)
              atomic_fetch_add(node, delta, std::memory_order_seq_cst);
            } else {
              atomic_fetch_add(node, delta, std::memory_order_seq_cst);
            }
          }
          node_type parent = BaseTree::parent_of(node);
          if (node == BaseTree::root()) break;          // done after processing root
          // for the parent: add only if we came from its LEFT child
          add_here = ((node % 2) == 0);                 // even index => left child
          node = parent;
        }
      }               

    private:

      //helper function to return the next node to update
      void add_entry(const entry_type& e) {
        value_type v = e;
        BaseTree::add_entry(0);
        update_weight(BaseTree::entry_count(),v);
      }
      void add_entry(const entry_type&& e) {
        value_type v = e;
        BaseTree::add_entry(0);
        update_weight(BaseTree::entry_count(),v);
      }

      Real weight_of(node_type n) {
        Real val = atomic_load(n, std::memory_order_relaxed);
        for (auto i = BaseTree::left_of(n); i <this->size(); i=BaseTree::right_of(i)) {
          val -= atomic_load(i, std::memory_order_relaxed);
        }
        return val;
	    }

      // const Real& weight_of(node_type n) const {
      //   return const_cast<This*>(this)->weight_of(n);
      // }
      void pop_entry() {
        update_weight(BaseTree::last(),0);
        BaseTree::pop_entry();
      }

      sideways_fenwick_selector_atomic const& const_this() const {
        return static_cast<This const&>(*this);
      }
//Helper functions to make sure that I don't mess up the offsets (since, from the user's perspectice, id's start at 0, but the nodes are indexed starting at 1)
      index_type id_of(node_type node) {
        return node-1;
      }

      node_type node_of(index_type index){
        return index+1;
      }

 };}};

 #endif