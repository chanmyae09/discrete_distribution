#ifndef COMPLETE_TREE_ARRAY
#define COMPLETE_TREE_ARRAY

#include <utility>
#include <vector>
#include <functional>
#include <type_traits>
#include <iostream>
#include <sys/mman.h>
#include <atomic>
#include <stdexcept>
#include <memory>
#include <cassert>
#include "completetree.hpp"

namespace dense {
namespace stochastic {

   typedef double Real;
   template <typename P, typename T>
   class complete_tree; 

   template <typename P,typename U>
   class complete_tree <P, std::atomic<U>>{
   public:
      using entry_type = U;
      using storage_type = std::atomic<U>;
      using size_type = size_t;
      using position_type = P;
      size_type _capacity;
   private:
      position_type _next;
      std::unique_ptr<storage_type[]> _tree;
      
   public:
      explicit complete_tree(position_type reserve = 0) {
         _capacity= static_cast<size_type>(reserve+1);
         _tree = std::unique_ptr<storage_type[]>(new storage_type [_capacity]{});
         _next = root();
      }

      complete_tree(complete_tree const&) = delete;
      complete_tree& operator=(complete_tree const&) = delete;
      complete_tree(complete_tree &&) = default;
      complete_tree& operator=(complete_tree &&) = default;
      ~complete_tree()= default;

      //Tree property methods

      size_type size() const {
        return _capacity;
      }
      size_type entry_count() const noexcept { 
         return _next - 1;
      }
      bool empty() const {
        return entry_count()==0;
      }
      void PrintTree()const{
         std::cout<<"printing tree ";
         for(size_t i=1;i<_capacity;i++){
            std::cout << _tree[i].load(std::memory_order_seq_cst) << ", ";
         }
         std::cout<<std::endl;
      }

      static constexpr position_type root() { return 1; }
      position_type last() const { return static_cast<position_type> (root() + entry_count() - 1); }

      static position_type parent_of(position_type node) {
        return ((node) >> 1);
      }

      static position_type left_of(position_type node) {
         return (node << 1);
      }

      static position_type right_of(position_type node) {
         return  (node << 1) + 1;
      }
      class reference
      {
         public:
            reference(storage_type* p) : p_(p){}
            operator entry_type() const { return p_->load(std::memory_order_seq_cst);}
            reference& operator=(entry_type v) { p_->store(v, std::memory_order_seq_cst); return *this;}
            reference& operator+= (entry_type d) {p_->fetch_add(d, std::memory_order_seq_cst); return *this;}
            reference& operator-= (entry_type d){p_->fetch_add(-d, std::memory_order_seq_cst); return * this;}
         private:
            storage_type* p_;
      };
      reference value_of(position_type i)       { return reference(&_tree[i]); }
      entry_type value_of(position_type i) const {  return _tree[i].load(std::memory_order_seq_cst); }

      reference operator[](position_type i) { return reference(&_tree[i]);}
      entry_type operator[](position_type i) const { return _tree[(i)].load(std::memory_order_seq_cst); }

      entry_type top(std::memory_order order = std::memory_order_seq_cst) const {
         return _tree[static_cast<size_type>(root())].load(order);
      }
      entry_type at(position_type i, std::memory_order order = std::memory_order_seq_cst) const {
         return _tree[static_cast<size_type>(i)].load(order);
      }
      void set(position_type i, entry_type v, std::memory_order order = std::memory_order_seq_cst) {
         _tree[static_cast<size_type>(i)].store(v,order);
      }

   protected:
      void add_entry(entry_type v) {
         if(_next < root() || _next >= _capacity)
            throw std::length_error("complete_tree_atomic full");
         set(_next, v);
         ++_next;
      }
      template <typename... Args>
      void emplace_entry(Args&&... args) {
         add_entry(entry_type(std::forward<Args>(args)...));
      }
      void pop_back() = delete;
      void resize(size_type new_size, entry_type value)= delete;
   
  };

}
}
#endif