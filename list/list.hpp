#pragma once
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>

template <typename T, typename Allocator = std::allocator<T>>
class List {
  class node;
  class FakeNode;

 public:
  using alloc_memory =
      typename std::allocator_traits<Allocator>::template rebind_alloc<node>;
  using alloc_traits = std::allocator_traits<alloc_memory>;

  template <bool IsConst>
  class common_iterator;

  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using allocator_type = Allocator;
  using value_type = T;

  //
  List() : size_(0), fake_node_(node()) {}

  List(size_t count, const T& value, const Allocator& alloc = Allocator())
      : size_(0), alloc_(alloc) {
    for (size_t index = 0; index < count; ++index) {
      try {
        push_back(value);
      } catch (...) {
        destruct();
        throw;
      }
    }
  }

  void node_change() {
    if (size_ == 0) {
      fake_node_.prev = static_cast<node*>(&fake_node_);
      fake_node_.next = static_cast<node*>(&fake_node_);
    }
  }

  void replace() {
    fake_node_.prev = static_cast<node*>(&fake_node_);
    fake_node_.next = static_cast<node*>(&fake_node_);
  }

  explicit List(size_t count, const Allocator& alloc = Allocator())
      : size_(0), alloc_(alloc) {
    for (; size_ < count; ++size_) {
      node_change();
      node* ptr = alloc_traits::allocate(alloc_, 1);
      try {
        alloc_traits::construct(alloc_, ptr);
      } catch (...) {
        destruct();
        alloc_traits::deallocate(alloc_, ptr, 1);
        throw;
      }
      ptr->prev = fake_node_.prev;
      ptr->next = static_cast<node*>(&fake_node_);

      fake_node_.prev->next = ptr;
      fake_node_.prev = ptr;
    }
  }

  List(const List& other)
      : alloc_(
            alloc_traits::select_on_container_copy_construction(other.alloc_)),
        size_(0) {
    auto nig = other.begin();
    for (size_t index = 0; index < other.size(); ++index) {
      try {
        push_back(*(nig++));
      } catch (...) {
        destruct();
        throw;
      }
    }
  }

  List(std::initializer_list<T> init, const Allocator& alloc = Allocator())
      : alloc_(alloc), size_(0) {
    auto nig = init.begin();
    for (size_t index = 0; index < init.size(); ++index) {
      try {
        push_back(*(nig++));
      } catch (...) {
        destruct();
        throw;
      }
    }
  }

  //

  ~List() { destruct(); }

  void swapy(List& other) {
    std::swap(size_, other.size_);
    std::swap(fake_node_.prev, other.fake_node_.prev);
    std::swap(fake_node_.next, other.fake_node_.next);
  }

  //
  List& operator=(const List& other) {
    if (alloc_traits::propagate_on_container_copy_assignment::value) {
      alloc_ = other.alloc_;
    }
    List tmp(other);
    swapy(tmp);
    return *this;
  }

  //
  iterator begin() { return iterator(fake_node_.next); }

  iterator end() { return iterator(static_cast<node*>(&fake_node_)); }

  const_iterator cbegin() { return const_iterator(fake_node_.next); }

  const_iterator cend() {
    return const_iterator(static_cast<node*>(&fake_node_));
  }

  const_iterator begin() const { return const_iterator(fake_node_.next); }

  const_iterator end() const {
    return const_iterator(static_cast<node*>(&fake_node_));
  }

  reverse_iterator rbegin() {
    return std::make_reverse_iterator(fake_node_.next);
  }

  reverse_iterator rend() {
    return std::make_reverse_iterator(static_cast<node*>(&fake_node_));
  }

  const_reverse_iterator rbegin() const {
    return std::make_reverse_iterator(const_iterator(fake_node_.next_));
  }

  const_reverse_iterator rend() const {
    return std::make_reverse_iterator(
        const_iterator(static_cast<node*>(&fake_node_)));
  }

  alloc_memory& get_allocator() { return alloc_; }

  const alloc_memory& get_allocator() const { return alloc_; }

  //
  T& front() { return *(fake_node_.next); }

  const T& front() const { return *(fake_node_.next); }

  T& back() { return *(fake_node_.prev); }

  const T& back() const { return *(fake_node_.prev); }

  //
  bool empty() const { return size_ == 0; }

  size_t size() const { return size_; }

  //
  void push_back(const T& value) {
    node_change();
    node* ptr = alloc_traits::allocate(alloc_, 1);
    try {
      alloc_traits::construct(alloc_, ptr, value, fake_node_.prev,
                              static_cast<node*>(&fake_node_));
    } catch (...) {
      alloc_traits::deallocate(alloc_, ptr, 1);
      throw;
    }
    fake_node_.prev->next = ptr;
    fake_node_.prev = ptr;
    ++size_;
  }
  //
  void push_back(T&& value) {
    node_change();
    node* ptr = alloc_traits::allocate(alloc_, 1);
    alloc_traits::construct(alloc_, ptr, std::move(value), fake_node_.prev,
                            static_cast<node*>(&fake_node_));
    fake_node_.prev->next = ptr;
    fake_node_.prev = ptr;
    ++size_;
  }
  //
  void pop_back() {
    if (size_ == 0) {
      return;
    }
    if (size_ == 1) {
      alloc_traits::destroy(alloc_, fake_node_.prev);
      alloc_traits::deallocate(alloc_, fake_node_.prev, 1);
      replace();
      --size_;
      return;
    }
    node* ptr = fake_node_.prev->prev;
    alloc_traits::destroy(alloc_, fake_node_.prev);
    alloc_traits::deallocate(alloc_, fake_node_.prev, 1);
    fake_node_.prev = ptr;
    ptr->next = static_cast<node*>(&fake_node_);
    --size_;
  }

  void push_front(const T& value) {
    node_change();
    exeption(value);
    ++size_;
  }

  void exeption(const T& value) {
    node* ptr = alloc_traits::allocate(alloc_, 1);
    try {
      alloc_traits::construct(alloc_, ptr, value,
                              static_cast<node*>(&fake_node_), fake_node_.next);
    } catch (...) {
      alloc_traits::deallocate(alloc_, ptr, 1);
      throw;
    }
    fake_node_.next->prev = ptr;
    fake_node_.next = ptr;
  }
  //
  void push_front(T&& value) {
    node_change();
    node* ptr = alloc_traits::allocate(alloc_, 1);
    alloc_traits::construct(alloc_, ptr, std::move(value),
                            static_cast<node*>(&fake_node_), fake_node_.next);
    fake_node_.next->prev = ptr;
    fake_node_.next = ptr;
    ++size_;
  }
  //
  void pop_front() { erase(); }

  void erase() {
    if (size_ == 0) {
      return;
    }
    if (size_ == 1) {
      alloc_traits::destroy(alloc_, fake_node_.prev);
      alloc_traits::deallocate(alloc_, fake_node_.prev, 1);
      replace();
      --size_;
      return;
    }
    node* tmp = fake_node_.next->next;
    alloc_traits::destroy(alloc_, fake_node_.next);
    alloc_traits::deallocate(alloc_, fake_node_.next, 1);
    fake_node_.next = tmp;
    tmp->prev = static_cast<node*>(&fake_node_);
    --size_;
  }

 private:
  void destruct() {
    while (size_ != 0) {
      pop_back();
    }
  }

  struct FakeNode {
    node* prev;
    node* next;
    FakeNode() = default;
    FakeNode(node* prev_2, node* next_2) : prev(prev_2), next(next_2) {}
    FakeNode(const FakeNode& nod) : prev(nod.prev), next(nod.next) {}
  };

  struct node : public FakeNode {
    T value;
    node() : FakeNode(nullptr, nullptr) {}
    node(const T& value_2, node* prev_2, node* next_2)
        : FakeNode(prev_2, next_2), value(value_2) {}
    node(const node& nod) : FakeNode(nod.prev, nod.next), value(nod.value) {}
  };

  mutable FakeNode fake_node_;
  alloc_memory alloc_;
  size_t size_;
};

template <typename T, typename Allocator>
template <bool IsConst>
class List<T, Allocator>::common_iterator {
 public:
  using type = std::conditional_t<IsConst, const T, T>;
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = int64_t;
  using value_type = T;
  using pointer = type*;
  using reference = type&;

  reference operator*() const { return ptr_->value; }

  pointer operator->() const { return &(ptr_->value); }

  common_iterator() = default;

  common_iterator(node* ptr) : ptr_(ptr) {}

  common_iterator<IsConst>& operator++() {
    ptr_ = ptr_->next;
    return *this;
  }

  common_iterator<IsConst> operator++(int) {
    common_iterator copy = *this;
    ++(*this);
    return copy;
  }

  common_iterator<IsConst>& operator--() {
    ptr_ = ptr_->prev;
    return *this;
  }

  common_iterator<IsConst> operator--(int) {
    common_iterator copy = *this;
    --(*this);
    return copy;
  }

  bool operator==(const common_iterator& other) const {
    return ptr_ == other.ptr_;
  }

  bool operator!=(const common_iterator& other) const {
    return ptr_ != other.ptr_;
  }

  friend class List;

 private:
  node* ptr_;
};