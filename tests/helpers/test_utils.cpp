#include "tests/helpers/test_utils.h"

NewOrderData test_utils::create_order(ID id, Price price, Quantity qty,
                                      Direction dir, OrderType type) {
  return NewOrderData{id, price, qty, dir, type};
}

CancelData test_utils::create_cancel_message(ID id) { return CancelData{id}; }
