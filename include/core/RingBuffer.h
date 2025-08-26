#include "include/core/Order.h"
#include <array>

template <size_t N = 1024> class RingBuffer {
  std::array<Order *, N> orders_;
  std::array<bool, N> valid_;
  size_t head_ = 0, tail_ = 0, size_ = 0;

public:
  void push_back(Order *order) {
    orders_[tail_] = order;
    valid_[tail_] = true;
    order->level_position = tail_;
    tail_ = (tail_ + 1) % N;
    ++size_;
  }

  void erase(size_t pos) {
    valid_[pos] = false;
    --size_;
  }

  Order *front() {
    while (head_ != tail_ && !valid_[head_]) {
      head_ = (head_ + 1) % N;
    }
    return valid_[head_] ? orders_[head_] : nullptr;
  }

  void pop_front() {
    if (Order *order = front()) {
      valid_[head_] = false;
      head_ = (head_ + 1) % N;
      --size_;
    }
  }

  size_t size() const { return size_; }

  bool empty() { return size_ == 0; }
};
