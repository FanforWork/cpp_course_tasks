#pragma once
#include <stdint.h>

#include <iterator>
#include <type_traits>
template <typename T>
class Deque {
 public:
  template <bool IsConst>
  class common_iterator;

  using iterator = common_iterator<false>;
  using const_iterator = common_iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  Deque()
      : arr_{nullptr},
        start_{nullptr},
        mid_{nullptr},
        begin_{iterator()},
        end_{iterator()},
        size_{0},
        sizes_(0) {}

  Deque(size_t count) : Deque() {
    if (count == 0) {
      return;
    }
    //
    size_ = count;
    int64_t chunk_number = (size_ + kHoriz - 1) / kHoriz;
    start_ = new T*[3 * chunk_number];
    nulpr();
    arr_ = new T**(start_);
    mid_ = new int64_t(chunk_number / 2);
    begin_ = (iterator(arr_, nullptr, mid_, -*mid_));
    end_ = (iterator(arr_, nullptr, mid_, chunk_number - *mid_ - 1));
    sizes_ = (3 * chunk_number);
    cast();
    //
    if (!empty()) {
      begin_.ptr_ = start_[*mid_ + begin_.chunk_];
      end_.ptr_ = start_[*mid_ + end_.chunk_] + (count % kHoriz - 1) % kHoriz;
      ++end_;
      if constexpr (std::is_default_constructible_v<T>) {
        catch_exeption_1();
      }
    }
  }

  void nulpr() {
    int64_t chunk_number = (size_ + kHoriz - 1) / kHoriz;
    //
    for (int64_t num = 0; num < 3 * chunk_number; ++num) {
      start_[num] = nullptr;
    }
  }

  void cast() {
    for (int64_t i = begin_.chunk_ + *mid_; i <= end_.chunk_ + *mid_; ++i) {
      reinpr_cast(i);
    }
  }

  void reinpr_cast(int64_t ind) {
    start_[ind] = reinterpret_cast<T*>(new uint8_t[kHoriz * sizeof(T*)]);
  }

  Deque(size_t size, const T& value) : Deque(size) { catch_exeption_2(value); }

  Deque(const Deque& deq) : Deque(deq.size_) {
    iterator iter = deq.begin_;
    catch_exeption_3(iter);
  }

  ~Deque() { destruct(); }

  void swapy(Deque& copy) {
    std::swap(arr_, copy.arr_);
    std::swap(start_, copy.start_);
    std::swap(mid_, copy.mid_);
    std::swap(begin_, copy.begin_);
    std::swap(end_, copy.end_);
    std::swap(size_, copy.size_);
    std::swap(sizes_, copy.sizes_);
  }

  Deque& operator=(const Deque& other) {
    Deque tmp(other);
    swapy(tmp);
    return *this;
  }

  iterator begin() { return begin_; }

  iterator end() { return end_; }

  const_iterator cbegin() {
    return const_iterator(begin_.arr_, begin_.ptr_, begin_.mid_, begin_.chunk_);
  }

  const_iterator cend() {
    return const_iterator(end_.arr_, end_.ptr_, end_.mid_, end_.chunk_);
  }

  const_iterator begin() const {
    return const_iterator(begin_.arr_, begin_.ptr_, begin_.mid_, begin_.chunk_);
  }

  const_iterator end() const {
    return const_iterator(end_.arr_, end_.ptr_, end_.mid_, end_.chunk_);
  }

  reverse_iterator rbegin() { return std::make_reverse_iterator(end_); }

  reverse_iterator rend() { return std::make_reverse_iterator(begin_); }

  const_reverse_iterator crbegin() {
    return std::make_reverse_iterator(
        const_iterator(end_.arr_, end_.ptr_, end_.mid_, end_.chunk_));
  }

  const_reverse_iterator crend() {
    return std::make_reverse_iterator(
        const_iterator(begin_.arr_, begin_.ptr_, begin_.mid_, begin_.chunk_));
  }

  const_reverse_iterator rbegin() const {
    return std::make_reverse_iterator(
        const_iterator(end_.arr_, end_.ptr_, end_.mid_, end_.chunk_));
  }

  const_reverse_iterator rend() const {
    return std::make_reverse_iterator(
        const_iterator(begin_.arr_, begin_.ptr_, begin_.mid_, begin_.chunk_));
  }

  size_t size() const { return size_; }

  bool empty() const { return size_ == 0; }

  T& operator[](size_t value) { return *(begin_ + value); }

  const T& operator[](size_t value) const { return *(begin_ + value); }

  T& at(size_t value) {
    if (value >= size_) {
      throw std::out_of_range("out_of_range");
    }
    return *(begin_ + value);
  }

  const T& at(size_t value) const {
    if (value >= size_) {
      throw std::out_of_range("out_of_range");
    }
    return *(begin_ + value);
  }

  void push_back(const T& value) {
    if (arr_ == nullptr || size_ == 0) {
      Deque<T> other(1, value);
      *this = other;
      return;
    }
    if (end_.chunk_ + *mid_ == sizes_ - 1) {
      re_allocate();
    }
    if (end_.ptr_ == (*arr_)[*mid_ + end_.chunk_]) {
      start_[*mid_ + end_.chunk_] =
          reinterpret_cast<T*>(new uint8_t[kHoriz * sizeof(T)]);
      end_.ptr_ = start_[*mid_ + end_.chunk_];
    }
    new (end_.ptr_) T(value);
    ++end_;
    ++size_;
  }

  void pop_back() {
    --end_;
    end_->~T();
    if (end_.ptr_ == (*arr_)[*mid_ + end_.chunk_]) {
      delcast();
    }
    if (begin_ == end_) {
      delcast();
      delete[] start_;
      delete arr_;
      delete mid_;
    }
    --size_;
  }

  void delcast() {
    delete[] reinterpret_cast<uint8_t*>((*arr_)[*mid_ + end_.chunk_]);
  }

  void push_front(const T& value) {
    if (arr_ == nullptr || size_ == 0) {
      Deque<T> other(1, value);
      *this = other;
      return;
    }
    if (-begin_.chunk_ == *mid_) {
      re_allocate();
    }
    if (begin_.ptr_ - (*arr_)[*mid_ + begin_.chunk_] == 0) {
      start_[*mid_ + begin_.chunk_ - 1] =
          reinterpret_cast<T*>(new uint8_t[kHoriz * sizeof(T)]);
    }
    --begin_;
    try {
      new (begin_.ptr_) T(value);
    } catch (...) {
      int64_t sgn = end_.ptr_ == start_[*mid_ + end_.chunk_] ? 1 : 0;
      end_ -= sgn;
      for (int64_t val = begin_.chunk_; val <= end_.chunk_; ++val) {
        delete[] reinterpret_cast<uint8_t*>(start_[*mid_ + val]);
      }
      delete arr_;
      delete[] start_;
      delete mid_;
      throw;
    }
    ++size_;
  }

  void pop_front() {
    begin_->~T();
    ++begin_;
    if (begin_.ptr_ - (*arr_)[*mid_ + begin_.chunk_] == kHoriz - 1 ||
        begin_ == end_) {
      delete[]((*arr_)[*mid_ + begin_.chunk_]);
    }
    if (begin_ == end_) {
      delcast();
      delete[] start_;
      delete arr_;
      delete mid_;
    }
    --size_;
  }

  void insert(iterator iter, const T& value) {
    if (!this->empty() && iter != end_) {
      this->push_back(*(this->end_ - 1));
      for (auto num = this->end() - 1; num > iter; --num) {
        new (num.ptr_) T(*(num - 1));
      }
      new (iter.ptr_) T(value);
    } else {
      this->push_back(value);
    }
  }

  void erase(iterator iter) {
    for (auto val = iter; val < end() - 1; ++val) {
      *val = *(val + 1);
    }
    pop_back();
  }

 private:
  T*** arr_;
  T** start_;
  int64_t* mid_;
  iterator begin_;
  iterator end_;
  size_t size_;  // размер заполненных горизонтальных клеток дека
  static const int64_t kHoriz = 24;  // размер(капасити) горизонтального массива
  // bool If_destruct_ = true;
  static const int64_t kNumchunk = 51;
  int64_t sizes_;

  void re_allocate() {
    T** new_start = new T*[kNumchunk * ((size_ + kHoriz - 1) / kHoriz)];
    sizes_ = kNumchunk * ((size_ + kHoriz - 1) / kHoriz);
    for (size_t num = 0; num < kNumchunk * ((size_ + kHoriz - 1) / kHoriz);
         ++num) {
      new_start[num] = nullptr;
    }
    int64_t new_mid = (kNumchunk * ((size_ + kHoriz - 1) / kHoriz) / 2);
    for (int64_t index = begin_.chunk_; index <= end_.chunk_; ++index) {
      new_start[new_mid + index] = (start_)[*mid_ + index];
    }
    delete[] start_;
    start_ = new_start;
    *arr_ = start_;
    *mid_ = new_mid;
  }

  void destruct() {
    if (size_ != 0) {
      for (iterator iter = begin_; iter < end_; ++iter) {
        iter->~T();
      }
      int64_t sgn = end_.ptr_ == start_[*mid_ + end_.chunk_] ? 1 : 0;
      end_ -= sgn;
      for (int64_t val = begin_.chunk_; val <= end_.chunk_; ++val) {
        delete[] reinterpret_cast<uint8_t*>(start_[*mid_ + val]);
      }
      end_ += sgn;
      delete[] start_;
      delete arr_;
      delete mid_;
    }
  }

  void catch_exeption_1() {
    for (iterator iter = begin_; iter != end_; ++iter) {
      try {
        new (iter.ptr_) T();
      } catch (...) {
        int64_t sgn = end_.ptr_ == start_[*mid_ + end_.chunk_] ? 1 : 0;
        end_ -= sgn;
        for (int64_t val = begin_.chunk_; val <= end_.chunk_; ++val) {
          delete[] reinterpret_cast<uint8_t*>(start_[*mid_ + val]);
        }
        delete arr_;
        delete[] start_;
        delete mid_;
        // If_destruct_ = false;
        throw;
      }
    }
  }

  void catch_exeption_2(const T& value) {
    for (iterator iter = begin_; iter < end_; ++iter) {
      try {
        new (iter.ptr_) T(value);
      } catch (...) {
        int64_t sgn = (end_.ptr_ == start_[*mid_ + end_.chunk_] ? 1 : 0);
        end_ -= sgn;
        for (int64_t val = begin_.chunk_; val <= end_.chunk_; ++val) {
          delete[] reinterpret_cast<uint8_t*>(start_[*mid_ + val]);
        }
        delete arr_;
        delete[] start_;
        delete mid_;
        size_ = 0;
        // If_destruct_ = false;
        throw;
      }
    }
  }

  void catch_exeption_3(iterator& iter) {
    for (iterator it = begin_; it < end_; ++it) {
      try {
        new (it.ptr_) T(*(iter++));
      } catch (...) {
        int64_t sgn = end_.ptr_ == start_[*mid_ + end_.chunk_] ? 1 : 0;
        end_ -= sgn;
        for (int64_t val = begin_.chunk_; val <= end_.chunk_; ++val) {
          delete[] reinterpret_cast<uint8_t*>(start_[*mid_ + val]);
        }
        delete arr_;
        delete[] start_;
        delete mid_;
        // If_destruct_ = false;
        throw;
      }
    }
  }
};

template <typename T>
template <bool IsConst>
class Deque<T>::common_iterator {
 public:
  using type = std::conditional_t<IsConst, const T, T>;
  using iterator_category = std::random_access_iterator_tag;
  using difference_type = int64_t;
  using value_type = T;
  using pointer = type*;
  using reference = type&;

  type& operator*() const { return *ptr_; }

  type* operator->() const { return ptr_; }

  common_iterator() = default;

  common_iterator(T*** arr, T* ptr, int64_t* mid, int64_t chunk)
      : arr_(arr), ptr_(ptr), mid_(mid), chunk_(chunk) {}

  common_iterator(const common_iterator& itr)
      : arr_(itr.arr_), ptr_(itr.ptr_), mid_(itr.mid_), chunk_(itr.chunk_) {}

  common_iterator& operator=(const common_iterator& other) = default;

  common_iterator& operator++() {
    if (ptr_ - (*arr_)[*mid_ + chunk_] == (kHoriz - 1)) {
      ++chunk_;
      ptr_ = (*arr_)[*mid_ + chunk_];
    } else {
      ++ptr_;
    }
    return *this;
  }

  common_iterator operator++(int) {
    common_iterator index;
    index = *this;
    ++(*this);
    return index;
  }

  common_iterator& operator--() {
    if (ptr_ == (*arr_)[*mid_ + chunk_]) {
      --chunk_;
      ptr_ = (*arr_)[*mid_ + chunk_] + kHoriz - 1;
    } else {
      --ptr_;
    }
    return *this;
  }

  common_iterator operator--(int) {
    common_iterator index;
    index = *this;
    --(*this);
    return index;
  }

  common_iterator& operator+=(long long count) {
    if (count == 0) {
      return *this;
    }
    if (count > 0) {
      int64_t dist = ptr_ - (*arr_)[*mid_ + chunk_] + 1;
      int64_t chunk_shift =
          count / kHoriz + (kHoriz - dist >= count % kHoriz ? 0 : 1);
      int64_t ptr_shift = (count % kHoriz + dist - 1) % kHoriz;
      chunk_ += chunk_shift;
      ptr_ = (*arr_)[*mid_ + chunk_] + ptr_shift;
    }
    if (count < 0) {
      int64_t dist = ptr_ - (*arr_)[*mid_ + chunk_];
      int64_t val = count - (kHoriz - dist) + 1;
      int64_t chunk_shift = (-val) / kHoriz;
      int64_t ptr_shift = (-val) % kHoriz;
      chunk_ -= chunk_shift;
      ptr_ = (*arr_)[*mid_ + chunk_] + kHoriz - 1 - ptr_shift;
    }
    return *this;
  }

  common_iterator& operator-=(int64_t count) {
    *this += (-count);
    return *this;
  }

  common_iterator operator+(int64_t count) const {
    common_iterator copy = *this;
    copy += count;
    return copy;
  }

  common_iterator operator-(int64_t count) const {
    common_iterator copy = *this;
    copy -= count;
    return copy;
  }

  int64_t operator-(const common_iterator& other) const {
    if (arr_ == nullptr) {
      return 0;
    }
    int64_t val1 = ptr_ - (*arr_)[*mid_ + chunk_];
    int64_t val2 = other.ptr_ - (*arr_)[*mid_ + other.chunk_];
    int64_t chunk_shift = chunk_ - other.chunk_;
    int64_t ptr_shift = (val1 + kHoriz - val2);
    int64_t count = (ptr_shift + (chunk_shift - 1) * kHoriz);
    if (chunk_shift == 0) {
      int64_t count = val1 - val2;
    }
    if (chunk_shift < 0) {
      int64_t count = -((val2 + kHoriz - val1) + (-(chunk_shift)-1) * kHoriz);
    }
    return count;
  }

  bool operator<(const common_iterator& other) const {
    return ptr_ - other.ptr_ < 0;
  }
  bool operator>(const common_iterator& other) const { return other < *this; }

  bool operator>=(const common_iterator& other) const {
    return !(*this < other);
  }

  bool operator<=(const common_iterator& other) const {
    return !(*this > other);
  }

  bool operator==(const common_iterator& other) const {
    return ptr_ == other.ptr_;
  }

  bool operator!=(const common_iterator& other) const {
    return !(*this == other);
  }
  friend class Deque;

 private:
  T*** arr_;
  T* ptr_;
  int64_t* mid_;
  int64_t chunk_;
};