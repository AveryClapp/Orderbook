#pragma once

#include "Direction.h"
#include "OrderType.h"
#include "Using.h"
#include <cstdint>

class Order {
private:
  Price price_;
  OrderType type_;
  Direction direction_;
  Quantity initial_quantity_;
  Quantity remaining_quantity_;
  Time time_;
  Order *next_order_;
  Order *prev_order_;
  ID id_;

public:
  Order(Price price, OrderType type, Direction direction, Quantity quantity,
        Time time, Order *prev_order, Order *next_order, ID id)
      : price_{price}, type_{type}, direction_{direction},
        initial_quantity_{quantity}, remaining_quantity_{quantity},
        prev_order_{prev_order}, next_order_{next_order}, id_{id} {
    time_ = std::chrono::system_clock::now();
  }

  const Price getPrice() const { return price_; }
  const OrderType getType() const { return type_; }
  const Direction getDirection() const { return direction_; }
  const Quantity getInitialQuantity() const { return initial_quantity_; }
  const Quantity getRemainingQuantity() const { return remaining_quantity_; }
  const Time getTime() const { return time_; }
  const ID getID() const { return id_; }

  Order *getNextOrder() const;
  Order *getPrevOrder() const;

  int8_t reduceQuantity(Quantity delta);
};
