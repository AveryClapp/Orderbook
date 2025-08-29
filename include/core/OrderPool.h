#pragma once
#include "include/core/Order.h"
#include <vector>

class OrderPool {
private:
  std::vector<Order> pool_;
  std::vector<size_t> free_list_;

  void grow();
  std::ptrdiff_t get_index(Order *order);

public:
  OrderPool(size_t num);
  ~OrderPool() = default;

  Order *get();

  void release(Order *order);
};
