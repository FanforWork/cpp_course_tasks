#pragma once
#include <cstddef>
#include <vector>
class RingBuffer {
 public:
  explicit RingBuffer(size_t capacity) {
    capacity_ = capacity;
    ringBufer_ = std::vector<int>(capacity);
  }
  size_t Size() const { return size_; }

  bool Empty() const { return (size_ == 0); }

  bool TryPush(int element) {
    if (size_ == capacity_) {
      return false;
    };
    ringBufer_[head_] = element;
    ++size_;
    ++head_;
    head_ %= capacity_;
    return true;
  }
  bool TryPop(int* element) {
    if (Empty()) {
      return false;
    };
    *element = ringBufer_[tail_];
    --size_;
    ++tail_;
    tail_ %= capacity_;
    return true;
  }

 private:
  size_t capacity_;
  size_t tail_ = 0;
  size_t size_ = 0;
  size_t head_ = 0;
  std::vector<int> ringBufer_;
};
