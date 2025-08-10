#ifndef COMPLETE_TREE_ATOMIC
#define COMPLETE_TREE_ATOMIC

#include <utility>
#include <vector>
#include <functional>
#include <type_traits>
#include <iostream>
#include <sys/mman.h>
#include <atomic>

namespace dense {
namespace stochastic {

   typedef double Real;
   template <typename P, typename T>
   class complete_tree; 

   template <typename P,typename U>
   class complete_tree <P, std::atomic<U>>{
      public:
      using value_type = U;
      using entry_type = U;
      using storage_type = std::atomic<U>;
      using size_type = size_t;
      using position_type = P;
      
      // using iterator = entry_type*;
      // using const_iterator = entry_type const*;
      // using reference = entry_type&;
      // using const_reference = entry_type const&;

      explicit complete_tree(position_type reserve = 0) {
        _tree.reserve(static_cast<size_type>(reserve) + 1);
        _tree.emplace_back();
      }

      complete_tree(complete_tree const&) = delete;
      complete_tree& operator=(complete_tree const&) = delete;
      complete_tree(complete_tree &&) = default;

      complete_tree& operator=(complete_tree &&) = default;

      ~complete_tree() = default;

      //Tree property methods

      size_type size() const {
        return _tree.size();
      }
      size_type entry_count() const noexcept { 
         return size() - 1;
      }

      bool empty() const {
        return entry_count()==0;
      }
      void PrintTree()const{
         std::cout<<"printing tree ";
         for(size_t i=1;i<_tree.size();i++){
            std::cout << _tree[i].load(std::memory_order_seq_cst) << ", ";
         }
         std::cout<<std::endl;
      }
   protected:
      entry_type atomic_load(position_type node, std::memory_order order = std::memory_order_seq_cst) const noexcept {
         return _tree[node].load(order);
      }

      void atomic_store(position_type node, entry_type v, std::memory_order order = std::memory_order_seq_cst) noexcept {
         _tree[node].store(v,order);
      }

      entry_type atomic_fetch_add(position_type node, entry_type delta, std::memory_order order = std::memory_order_seq_cst) noexcept {
         return _tree[node].fetch_add(delta, order);
      }

      void add_entry(value_type v) {
      _tree.emplace_back(v); 
      }

      template <typename... Args>
      void emplace_entry(Args&&... args) {
         add_entry(entry_type(std::forward<Args>(args)...));
      }

      void pop_back() {
         if (size() <= 1) return;
         _tree.pop_back();
      }
      void resize(size_type new_size, value_type value) {
         size_type old = _tree.size();
        _tree.resize(new_size);
        for(size_type i = old; i < new_size; ++i)
        {
         _tree[i].store(value,std::memory_order_seq_cst);
        }
      }
   

    public:
      //Position methods
      static constexpr position_type root() { return 1; }
      position_type last() const { return static_cast<position_type> (root() + entry_count() - 1); }

      static position_type parent_of(position_type node) {
        return ((node) >> 1);
      }

      static position_type left_of(position_type node) {
        return (node << 1);
      }

      static position_type right_of(position_type node) {
        return (node << 1) + 1;
      }
      value_type top(std::memory_order order = std::memory_order_seq_cst) const {
      return atomic_load(root(), order);
      }
      value_type at(position_type i, std::memory_order order = std::memory_order_seq_cst) const {
         return atomic_load(i, order);
      }
      void set(position_type i, value_type v,
            std::memory_order order = std::memory_order_seq_cst) {
         atomic_store(i, v, order);
      }
      private:
         std::vector<storage_type> _tree;

   // //Element access
   // const_reference top() const {
   //    return value_of(root());
   // }

   // const_reference operator[](position_type i) const {
   //    return _tree[i];
   // }

   // const_reference at(position_type i) const {
   //    return _tree[i];
   // }


   // complete_tree const& const_this() const {
   //    return static_cast<complete_tree const&>(*this);
   // }

   // const_reference value_of(position_type node) const {
   //    return *iterator_for(node);
   // }

   //Iterator methods
   // public:
   // const_iterator begin() const {
   //    return iterator_for(root());
   // }

   // const_iterator end() const {
   //    return _tree.end();
   // }

   // const_iterator iterator_for(position_type node) const {
   //    return _tree.data() + node;
   // }

   // protected:
   // iterator iterator_for(position_type node) {
   //    return const_cast<iterator>(const_this().iterator_for(node));
   // }

   // iterator begin() {
   //    return iterator_for(root());
   // }

   // iterator end() {
   //    return _tree.end();
   // }
   // protected:
   // adding resize function for leaf_sum uses
   // reference value_of(position_type node) {
   //    return const_cast<reference>(const_this().value_of(node));
   // }

   // reference operator[](position_type i) {
   //    return _tree[i];
   // }

   // reference at(position_type i) {
   //    return _tree[i];
   // }
  };

}
}
#endif