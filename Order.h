#pragma once

#include "Direction.h"
#include "OrderType.h"
#include "Using.h"

class Order {
private:
  Price m_price;
  Quantity m_quantity;
  OrderType m_type;
  Direction m_direction;
  unsigned long m_id;

public:
  Order(Price price, Quantity quanitiy, OrderType type, Direction direction,
        unsigned long id)
      : m_price{price}, m_quantity{quantity}, m_type{type},
        m_direction{direction}, m_id{id} {}
}
