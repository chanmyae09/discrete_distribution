#ifndef COMPLETE_TREE
#define COMPLETE_TREE

#include <utility>
#include <vector>
#include <functional>
#include <type_traits>
#include <iostream>
#include <sys/mman.h>

namespace dense {
namespace stochastic {

  typedef double Real;
  /* This complete tree class is meant to be subclassed to build various 
   * data structures.  Because most of those data structures require 
   * updates when values are modified, only const access to elements is 
   * granted publicly, while there are protected methods for non-const 
   * element access.
   */
  template <
    typename P,
    typename T
  >
  class complete_tree {

    public:

      using size_type = size_t;
      using position_type = P;
      using entry_type = T;
      using iterator = entry_type*;
      using const_iterator = entry_type const*;
      using reference = entry_type&;
      using const_reference = entry_type const&;

      complete_tree(position_type reserve = 0) {
        _tree.reserve(reserve + 1);
        _tree.emplace_back();
      }

      complete_tree(complete_tree const&) = default;

      complete_tree(complete_tree &&) = default;

      complete_tree& operator=(complete_tree const&) = default;

      complete_tree& operator=(complete_tree &&) = default;

      ~complete_tree() = default;

      //Tree property methods

      size_type size() const {
        return _tree.size();
      }

      bool empty() const {
        return _tree.empty();
      }

      void PrintTree()const{
        std::cout<<"printing tree ";
        for(int i=1;i<_tree.size();i++){
          std::cout<<_tree[i]<<", ";
        }
        std::cout<<std::endl;
      }

      //Tree modification methods
      //
    protected:

      

      void add_entry(entry_type&& entry) {
        _tree.push_back(entry);
      }

      void add_entry(const entry_type& entry) {
        _tree.push_back(entry);
      }

      template <typename... Args>
      void emplace_entry(Args&&... args) {
        add_entry(entry_type(std::forward<Args>(args)...));
      }

      void pop_back() {
        if (size() <= 1) return;
        _tree.pop_back();
      }

      //Iterator methods
    public:
      const_iterator begin() const {
        return iterator_for(root());
      }

      const_iterator end() const {
        return _tree.end();
      }

      const_iterator iterator_for(position_type node) const {
        return _tree.data() + node;
      }

    protected:
      iterator iterator_for(position_type node) {
        return const_cast<iterator>(const_this().iterator_for(node));
      }

      iterator begin() {
        return iterator_for(root());
      }

      iterator end() {
        return _tree.end();
      }

    public:
      //Position methods
      static constexpr position_type root() { return 1; }
      position_type last() const { return root() + entry_count() - 1; }

      static position_type parent_of(position_type node) {
        return ((node) >> 1);
      }

      static position_type left_of(position_type node) {
        return (node << 1);
      }

      static position_type right_of(position_type node) {
        return (node << 1) + 1;
      }

      //Element access
      const_reference top() const {
        return value_of(root());
      }

      const_reference operator[](position_type i) const {
        return _tree[i];
      }

      const_reference at(position_type i) const {
        return _tree[i];
      }


      complete_tree const& const_this() const {
        return static_cast<complete_tree const&>(*this);
      }

      const_reference value_of(position_type node) const {
        return *iterator_for(node);
      }


    protected:
    //adding resize function for leaf_sum uses
      void resize(size_type new_size, entry_type value) {
        _tree.resize(new_size, value);
      }
      
      reference value_of(position_type node) {
        return const_cast<reference>(const_this().value_of(node));
      }

      reference operator[](position_type i) {
        return _tree[i];
      }

      reference at(position_type i) {
        return _tree[i];
      }

      size_type entry_count() const { return size() - 1; }
      
    private:
      std::vector<entry_type> _tree;
  };

}
}
#endif