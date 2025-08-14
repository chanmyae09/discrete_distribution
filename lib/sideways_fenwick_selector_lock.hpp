#ifndef SIDEWAYS_FENWICK_RANDOM_SELECTOR_LOCK
#define SIDEWAYS_FENWICK_RANDOM_SELECTOR_LOCK
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
#include <cstdio>   // for fprintf


#include "completetree_array.hpp"


namespace dense {
namespace stochastic {
  struct SpinLock
  {
    std::atomic_flag flag;
    SpinLock() = default;
    SpinLock(const SpinLock&)            = delete;
    SpinLock& operator=(const SpinLock&) = delete;
    void lock() noexcept
    {
      while(flag.test_and_set(std::memory_order_acquire)){}
    }
    void unlock() noexcept
    {
      flag.clear(std::memory_order_release);
    }
  };
  

  template <
    typename I = size_t, size_t precision = std::numeric_limits<Real>::digits
  >
  class sideways_fenwick_selector_lock :
    //Extends a complete tree...
    protected complete_tree<I, std::atomic<Real> >
  {


    public:

      using size_type = std::ptrdiff_t;
      using index_type = I;
      using This = sideways_fenwick_selector_lock<index_type, precision>;
      using node_type = index_type;
      using value_type = Real;
      using entry_type = Real;
      using iterator = value_type*;
      using const_iterator = value_type const*;
      using reference = value_type&;
      using const_reference = value_type const&;
      using BaseTree = complete_tree<node_type, std::atomic<value_type>>;
      static constexpr unsigned BAND_LEVELS = 5;

      sideways_fenwick_selector_lock() = delete;

      template<typename InputIt>
      sideways_fenwick_selector_lock(InputIt first, InputIt last) :
          BaseTree(static_cast<node_type>(std::distance(first, last))) 
      {
        //std::cout<<"__________constructor__________"<<std::endl;
        
        for (InputIt it = first; it != last; ++it) {
          Real w = *it;
          BaseTree::add_entry(Real(w));
        }
        //std::cout<<"all entries added ";
        //this->PrintTree();
        //go through entire tree and change entries into weightsums of ENTIRE tree
        //std::cout<<"tree before summing: ";
        //this->PrintTree();
        node_type lastNonLeaf = (BaseTree::entry_count())/2;
        //std::cout<<"last non leaf is "<<lastNonLeaf<<std::endl;
        if (BaseTree::entry_count()%2==0){
            this->value_of(lastNonLeaf)+=(this->value_of(BaseTree::left_of(lastNonLeaf)));
            //std::cout<<"last non leaf has 1 child   last non leaf is "<<lastNonLeaf<<std::endl;
        }
        else{
            this->value_of(lastNonLeaf)+=(this->value_of(BaseTree::left_of(lastNonLeaf)))+(this->value_of(BaseTree::right_of(lastNonLeaf)));
        }
        //std::cout<<"lastNonLeaf summed"<<std::endl;
        for (node_type node = lastNonLeaf-1;node>0;node--){
          //std::cout<<"left val: "<<this->value_of(BaseTree::left_of(node))<<"    right val "<<this->value_of(BaseTree::right_of(node))<<std::endl;
            this->value_of(node)+=((this->value_of(BaseTree::left_of(node))+(this->value_of(BaseTree::right_of(node)))));
            //std::cout<<"node "<<node<<" summed    new value is "<<this->value_of(node)<<"   new value variable hols"<<newVal<<std::endl;
        }

        //std::cout<<"weightsum tree: ";
        //this->PrintTree();
        total_weight.store(this->value_of(this->root()), std::memory_order_relaxed);
        //std::cout<<"total weight is "<<total_weight<<std::endl;
        //go through entire tree again (this time from the top) and subtract the weight of the right subtree
        for(node_type node = BaseTree::root();node<lastNonLeaf;node++){
            this->value_of(node)-=(this->value_of(BaseTree::right_of(node)));
        }
        //std::cout<<"right subtrees subtracted except in lastNonLeaf"<<std::endl;
        if (BaseTree::entry_count()%2==1){ //bc size in complete tree returns the size including the 0 index
            this->value_of(lastNonLeaf)-=(this->value_of(BaseTree::right_of(lastNonLeaf)));
        }
        // std::cout<<"tree after constructing: ";
        //this->PrintTree();

      }
      void PrintTreePublic() const {
        this->PrintTree();
      }

      sideways_fenwick_selector_lock(sideways_fenwick_selector_lock const&) = delete;

      sideways_fenwick_selector_lock(sideways_fenwick_selector_lock &&) = default;

      sideways_fenwick_selector_lock& operator=(sideways_fenwick_selector_lock const&) = delete;

      sideways_fenwick_selector_lock& operator=(sideways_fenwick_selector_lock &&) = default;

      ~sideways_fenwick_selector_lock() = default;


      // //Methods of WeightSum we want to make available
      
      template<class URNG>
      index_type operator()(URNG& g) {
        auto tw = total_weight.load(std::memory_order_seq_cst); 
        Real target =  std::generate_canonical<Real, precision, URNG>(g)*tw;

        node_type node = this->root();
        node_type lastNonLeaf = BaseTree::entry_count()/2;
        //std::cout<<"last non leaf is "<<lastNonLeaf<< " target is "<<target<<std::endl;
        while(node<lastNonLeaf){
          if (target<(this->value_of(node))){
            node = BaseTree::left_of(node);
          }
          else{
            target -=this->value_of(node);
            node=BaseTree::right_of(node);
          }
        }
        //this is to make sure that there is a right child (if I did this in the loop it would check that there is a right child every time which is unecessary)
        if (node==lastNonLeaf){
          //std::cout<<"node is last non leaf"<<std::endl;
          if (target<(this->value_of(node))){
            node = BaseTree::left_of(node);
          }
          else if(BaseTree::right_of(node)<BaseTree::size()){
            //std::cout<<"going right from last non leaf"<<std::endl;
            target -=this->value_of(node);
            node=BaseTree::right_of(node);
            //std::cout<<"went right node is "<<node<<" and target is "<<target<<std::endl;
          }
        }
        if (target<(this->value_of(node))){
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


      // void push_back(const entry_type& e) {
      //   value_type v = e;
      //   BaseTree::add_entry(0);
      //   update_weight_of_node(BaseTree::last(),v);
      // }
      // void push_back(const entry_type&& e) {
      //   value_type v = e;
      //   BaseTree::add_entry(0);
      //   update_weight_of_node(BaseTree::last(),v);
      // }
      // void pop_back() {
      //   update_weight_of_node(BaseTree::last(),0);
      //   BaseTree::pop_back();
      // }

    private:
      SpinLock sp_lock;
      static constexpr node_type LOCK_NODE_INDEX = (node_type(1) << BAND_LEVELS ) -1;

      std::atomic<Real> total_weight=0;
      //helper function to return the next node to update
      node_type nextNode(node_type currentNode){
        return currentNode>>(((std::countr_one(currentNode)))+1);
      }
      //returns the sum of the left subtree and the node itself
      Real& weightsum_of(node_type n) {
        return this->value_of(n);
      }

      const Real& weightsum_of(node_type n) const {
        return const_cast<This*>(this)->weightsum_of(n);
      }

      void update_weight_of_node(node_type givenNode, Real new_weight) {
        auto node = givenNode;
        Real weightDifference =  new_weight - this->weight_of(node);

        while(node>=LOCK_NODE_INDEX){
          this->value_of(node)+=weightDifference;
          node = nextNode(node);
        }
        sp_lock.lock();
        while(node >=BaseTree::root())
        {
          this->value_of(node)+=weightDifference;
          node = nextNode(node);
        }
        total_weight.fetch_add(weightDifference, std::memory_order_relaxed);
        sp_lock.unlock();
      } 

      //helper function to return the next node to update
      
      static inline bool in_band(node_type n)
      {
        return n < LOCK_NODE_INDEX;
      }
      static inline unsigned level_of (node_type n)
      {
        return std::bit_width(n) -1;
      }  
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
        auto val = this->value_of(n);
        for (auto i = BaseTree::left_of(n); i <this->size(); i=BaseTree::right_of(i)) {
          val -= this->value_of(i);
        }
        return val;
	    }

      // const Real& weight_of(node_type n) const {
      //   return const_cast<This*>(this)->weight_of(n);
      // }
      /* This is returning a reference to a temporary. it calls return by value, which is a temporary, const cast to this value doesn't make sense cuz it's
      thrown away when the function ends, also sideway fenwick tree stores its weight and left subtree, what does it mean to 
      return by const reference  */


      // void pop_entry() {
      //   update_weight(BaseTree::last(),0);
      //   BaseTree::pop_entry(); // only pop_back
      // }
      /* Should be delted , cuz it's delted in completetree_atomic*/
      sideways_fenwick_selector_lock const& const_this() const {
        return static_cast<This const&>(*this);
      }

      //Helper functions to make sure that I don't mess up the offsets (since, from the user's perspectice, id's start at 0, but the nodes are indexed starting at 1)
      index_type id_of(node_type node) {
        return node-1;
      }

      node_type node_of(index_type index){
        return index+1;
      }
 };
}};

 #endif