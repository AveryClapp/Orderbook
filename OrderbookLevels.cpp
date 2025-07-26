#include "OrderbookLevels.h"

void OrderbookLevels::add_ask(const &Order ask) {
  // First check if we have this level already
  Level *target_level;
  auto it = level_map_.find(ask.price);
  if (it != orders_.end()) {
    target_level = it->second;
  } else {
  }

  asks_.emplace({ask.getPrice(), ask.getRemainingQuantity()});
}

void OrderbookLevels::add_bid(const &Order bid) {
  bids_.emplace({bid.getPrice(), bid.getRemainingQuantity()});
}

void OrderbookLevels::add_level(Order *order) {
int case = order.getDirection() == Direction::Buy ? -1:
  1;
  Level **cur = order.getDirection() == Direction::Buy ? &bids_ : &asks_;
  Price price *= order.getPrice() * case;
  if (!*cur) {
    *cur = new Level{price * case, nullptr, nullptr, nullptr, order, order);
    return;
  }
  while (true) {
    if (price > (*cur)->price * case) {
      if (!(*cur)->right_child) {
        (*cur)->right_child = new Level{price * case, *cur, nullptr, nullptr, order, order};
        return;
      }
      cur = &((*cur)->right_child);
    } else {
      if (!(*cur)->left_child) {
        (*cur)->left_child = new Level{price * case, *cur, nullptr, nullptr, order, order};
        return;
      }
      cur = &((*cur)->left_child);
    }
  }
}
