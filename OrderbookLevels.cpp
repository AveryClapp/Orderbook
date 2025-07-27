#include "OrderbookLevels.h"

void OrderbookLevels::add_ask(Order *ask) {
  // First check if we have this level already
  Level *target_level;
  auto it = level_map_.find(ask->getPrice());
  if (it != level_map_.end()) {
    target_level = it->second;
  } else {
    add_level(ask);
  }
}

void OrderbookLevels::add_bid(Order *bid) {
  Level *target_level;
  auto it = level_map_.find(bid->getPrice());
  if (it != level_map_.end()) {
    target_level = it->second;
  } else {
    add_level(bid);
  }
}

void OrderbookLevels::add_level(Order *order) {
  int flip = order->getDirection() == Direction::Buy ? -1 : 1;
  Level **cur = order->getDirection() == Direction::Buy ? &bids_ : &asks_;
  Price price = order->getPrice() * flip;
  if (!*cur) {
    *cur = new Level{price * flip, nullptr, nullptr, nullptr, order, order};
    return;
  }
  while (true) {
    if (price > (*cur)->price * flip) {
      if (!(*cur)->right_child) {
        (*cur)->right_child =
            new Level{price * flip, *cur, nullptr, nullptr, order, order};
        return;
      }
      cur = &((*cur)->right_child);
    } else {
      if (!(*cur)->left_child) {
        (*cur)->left_child =
            new Level{price * flip, *cur, nullptr, nullptr, order, order};
        return;
      }
      cur = &((*cur)->left_child);
    }
    throw std::runtime_error(
        "Prices should not be equal when inserting at this point");
  }
}
