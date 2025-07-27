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

void OrderbookLevels::cancel_order(const ID id) {
  Level *target_order;
  auto it = order_map_.find(id);
  if (it != order_map_.end()) {
    target_order = it->second;
  } else {
    throw std::runtime_error("Can not remove non-existant order id");
  }
  Order *next_order = target_order.getNextOrder();
  if (!next) {
    Level *next_level = target_order.getNextLevel();
    remove_level(target_order->getLevel());
  }
}

void remove_level(Level *level) {
  if (!level->tail) {
    delete level;
    return;
  }
  Order *node = level->tail;
  while (node) {
    Order *temp = node.getNextOrder();
    order_map_ delete node;
    node = temp;
  }
}
