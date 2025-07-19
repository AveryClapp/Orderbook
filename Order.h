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
  ID id_;

public:
  Order(Price price, OrderType type, Direction direction,
        Quantity quantity unsigned long id)
      : price_{price}, m_type{type}, direction_{direction},
        initial_quantity_{quantity}, remaining_quantity_{quantity}, id_{id} {}

  const Price getPrice const { return price_; }
  const OrderType getType const { return type_; }
  const Quantity getInitialQuantity const { return initial_quantity_; }
  const Quantity getRemainingQuantity const { return remaining_quantity_; }
  const ID getID const { return id_; }
}
