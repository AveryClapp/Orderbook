#include "include/core/OrderPool.h"
#include <cstddef>
#include <vector>

OrderPool::OrderPool(size_t num) {
  pool_.reserve(num);
  free_list_.reserve(num);
  for (size_t i = num; i > 0; --i) {
    pool_.emplace_back();
    free_list_.push_back(i - 1);
  }
}

Order *OrderPool::get() {
  if (free_list_.empty()) [[unlikely]] {
    grow();
  }
  size_t next_available = free_list_.back();
  free_list_.pop_back();
  return &pool_[next_available];
}

void OrderPool::release(Order *order) {
  free_list_.push_back(static_cast<size_t>(get_index(order)));
}

std::ptrdiff_t OrderPool::get_index(Order *order) {
  // C++ implicitly divides by sizeof(Order) here
  return order - pool_.data();
}

void OrderPool::grow() {
  size_t old_size = pool_.size();
  size_t new_size = old_size * 2;

  pool_.resize(new_size);
  free_list_.reserve(new_size);

  for (size_t i = new_size; i > old_size; --i) {
    free_list_.push_back(i - 1);
  }
}
