#include "include/core/OrderbookLevels.h"
#include <cassert>

void OrderbookLevels::add_ask(Order *ask) {
  auto &level = asks_[ask->price];
  level.price = ask->price;
  level.orders.push_back(ask);
  level.total_quantity += ask->quantity;
  ask->list_iterator = std::prev(level.orders.end());
}

void OrderbookLevels::add_bid(Order *bid) {
  auto &level = bids_[bid->price];
  level.price = bid->price;
  level.orders.push_back(bid);
  level.total_quantity += bid->quantity;
  bid->list_iterator = std::prev(level.orders.end());
}
