#pragma once

#include "Direction.h"
#include "OrderType.h"
#include "Using.h"

class Order {
private:
  Price price_;
  OrderType type_;
  Direction direction_;
  Quantity initial_quantity_;
  Quantity remaining_quantity_;
  Time time_;
  ID id_;

public:
  Order(Price price, OrderType type, Direction direction, Quantity quantity,
        Time time, ID id)
      : price_{price}, type_{type}, direction_{direction},
        initial_quantity_{quantity}, remaining_quantity_{quantity}, id_{id} {
    time_ = std::chrono::system_clock::now();
  }

  const Price getPrice() const { return price_; }
  const OrderType getType() const { return type_; }
  const Direction getDirection() const { return direction_; }
  const Quantity getInitialQuantity() const { return initial_quantity_; }
  const Quantity getRemainingQuantity() const { return remaining_quantity_; }
  const Time getTime() const { return time_; }
  const ID getID() const { return id_; }
};
