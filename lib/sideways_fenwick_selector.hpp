#ifndef SIDEWAYS_FENWICK_RANDOM_SELECTOR
#define SIDEWAYS_FENWICK_RANDOM_SELECTOR
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

#include "completetree.hpp"


namespace dense {
namespace stochastic {


  template <
    typename I = size_t, size_t precision = std::numeric_limits<Real>::digits
  >
  class sideways_fenwick_selector :
    //Extends a complete tree...
    protected complete_tree<I, Real >
  {


    public:

      using size_type = std::ptrdiff_t;
      using index_type = I;
      using This = sideways_fenwick_selector<index_type, precision>;
      using node_type = index_type;
      using value_type = Real;
      using entry_type = Real;
      using iterator = value_type*;
      using const_iterator = value_type const*;
      using reference = value_type&;
      using const_reference = value_type const&;
      using BaseTree = complete_tree<node_type, value_type>;

      sideways_fenwick_selector() = delete;

      template<typename InputIt>
      sideways_fenwick_selector(InputIt first, InputIt last) :
          BaseTree() 
      {
        //std::cout<<"__________constructor__________"<<std::endl;
        size_t n = static_cast<size_t>(last - first);
        
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
        total_weight = this->value_of(this->root());
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

      sideways_fenwick_selector(sideways_fenwick_selector const&) = default;

      sideways_fenwick_selector(sideways_fenwick_selector &&) = default;

      sideways_fenwick_selector& operator=(sideways_fenwick_selector const&) = default;

      sideways_fenwick_selector& operator=(sideways_fenwick_selector &&) = default;

      ~sideways_fenwick_selector() = default;


      // //Methods of WeightSum we want to make available
      
      template<class URNG>
      index_type operator()(URNG& g) {
        Real target =  std::generate_canonical<Real, precision, URNG>(g)*total_weight;

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
        // std::cout<<"________________updating weight_____________"<<std::endl;
        // auto node = node_of(i);
        // Real weightDifference =  new_weight - this->weight_of(node);
        // total_weight+=weightDifference;
        // std::cout<<"updating node "<<node<<" to contain "<<new_weight<<" instead of "<<this->weight_of(node)<<" which is a difference of "<<weightDifference<<std::endl;
        // while(node>BaseTree::root()){
        //     this->value_of(node)+=weightDifference;
        //     //std::cout<<"about to change node ";
        //     //this->PrintTree();
        //     node = nextNode(node);
        //     //std::cout<<"just changed node ";
        //     //this->PrintTree();

        //     //std::cout<<"new node is "<<node<<std::endl;
            
        // }
        // this->value_of(node)+=weightDifference;
        // //std::cout<<"ending tree is ";
        // //this->PrintTree();
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

      Real total_weight=0;
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
        size_t node = givenNode;
        __m128d weightDifference =  _mm_set_pd(new_weight - weight_of(node), 0.0);
        //total_weight += weightDifference;
        __m128d fromLeft;
        __m128i ssenode = _mm_set_epi64x(node, 0);
        fromLeft = _mm_castsi128_pd(_mm_set_epi64x(-1, 0));
        //int depthDiff=std::countl_zero(node) - std::countl_zero(BaseTree::entry_count());
        //std::cout << "Node before: " << std::bitset<64>(node) << std::endl;
        // node = ~node;
        // //std::cout << "Node after flipping: " << std::bitset<64>(node) << std::endl;

        // node=node << depthDiff;
        // //std::cout << "Node afrer shifting: " << std::bitset<64>(node) << std::endl;

        // node=~node;
        
        // //std::cout << "Node after: " << std::bitset<64>(node) << std::endl;

        // //std::cout << "node is " << node << "and entry count is " << BaseTree::entry_count() << std::endl;
        // // if(node > BaseTree::entry_count()) {
        // //   node = BaseTree::parent_of(node);
        // // }
        // //std::cout << "node is " << node << "and entry count is " << BaseTree::entry_count() << std::endl;
        // node = node >> (node > BaseTree::entry_count());
        

        while(node>=BaseTree::root()){
            //auto oldval = this->value_of(node);
            __m128d masked_real;
            masked_real = _mm_and_pd(weightDifference, fromLeft);
            //masked_real = reinterpret_cast<int64_t&>(weightDifference) & fromLeft;


            _mm_store_sd(&(this->value_of(node)), _mm_add_sd(_mm_load_sd(&(this->value_of(node))), masked_real));
            fromLeft = _mm_castsi128_pd(_mm_sub_epi64(_mm_and_si128(ssenode, _mm_set_epi64x(1, 0)), _mm_set_epi64x(1, 0)));
            ssenode = _mm_srli_epi64(ssenode, 1);
            node = BaseTree::parent_of(node);
            
        }
      }

      
     ///*


      // void update_weight_of_node(node_type givenNode, Real new_weight) {
      //   //std::cout<<"________________updating weight_____________"<<std::endl;
      //   auto node = givenNode;
      //   Real weightDifference =  new_weight - this->weight_of(node);
      //   total_weight+=weightDifference;
      //   //std::cout<<"updating node "<<node<<" to contain "<<new_weight<<" instead of "<<this->weight_of(node)<<" which is a difference of "<<weightDifference<<std::endl;
      //   auto nextUpdate = nextNode(node);
      //   while(node>=BaseTree::root()){
            
      //       this->value_of(node)+=weightDifference; // SKIP VERSION
      //       node = nextNode(node); //SKIP VERSION


      //       //std::cout<<"just changed node ";
      //       //this->PrintTree();

      //       //std::cout<<"new node is "<<node<<std::endl;
            
      //   }
      //   //this->value_of(node)+=weightDifference;
      //   //std::cout<<"ending tree is ";
      //   //this->PrintTree();
      // }
        



//}
        
        

      

    //   Real total_weight() const { return WeightSum::total_weight(); }

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
            auto val = this->value_of(n);
            for (auto i = BaseTree::left_of(n); i <this->size(); i=BaseTree::right_of(i)) {
              val -= this->value_of(i);
            }
            return val;
	    }

      const Real& weight_of(node_type n) const {
        return const_cast<This*>(this)->weight_of(n);
      }

      

      void pop_entry() {
        update_weight(BaseTree::last(),0);
        BaseTree::pop_entry();
      }

      sideways_fenwick_selector const& const_this() const {
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