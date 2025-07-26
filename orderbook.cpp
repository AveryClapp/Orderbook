#include "Orderbook.h"

/**
 * Upon receiving an order, send it to a function that handles its direction.
 */
void Orderbook::receive_message(Order &order) {
  /* TODO: Handle CANCEL orders */
  if (order.getDirection() == Direction::Buy) {
    handle_buy(order);
  } else if (order.getDirection() == Direction::Sell) {
    handle_sell(order);
  } else {
    throw std::runtime_error("Impossible Dud");
  }
}

/**
 * Upon receiving a buy, we match all asks that fulfill the condition
 * and then add the buy to the bids
 */
void Orderbook::handle_buy(Order &buy_order) {
  Order *cur = this->lowest_sell_;
  Price buy_price = buy_order.getPrice();
  while (cur && -cur->getPrice() <= buy_price) {
    Quantity max_quantity = std::min(remaining_quantity, level.second);
    Quantity order_remaining = buy_order.reduceQuantity(max_quantity);
    Quantity ask_remaining = cur.reduceQuantity(max_quantity);
    if (ask_remaining == 0) {
      /* If cur is fulfilled, move to next node and update level */
      cur = cur.getNextOrder();
    }
    if (order_remaining == 0) {
      break;
    }
  }
  // If we have unused quantity so far, then add it to the bids
  if (buy_order.getRemainingQuantity() > 0) {
    levels_.add_bid(buy_order);
  }
}

void Orderbook::handle_sell(Order &sell_order) {
  auto &bids = levels_.get_bids();
  std::vector<std::pair<Price, Quantity>> updates{};
  for (const auto &level : bids) {
    Price level_price = level.first;
    Quantity remaining_quantity = sell_order.getRemainingQuantity();

    if (level_price >= sell_order.getPrice()) {
      Quantity max_quantity = std::min(remaining_quantity, level.second);

      sell_order.remaining_quantity_ -= max_quantity;
      updates.emplace_back({level_price, level.second - max_quantity)};
      if (sell_order.remaining_quantity == 0) {
        break;
      }
    } else {
      break;
    }
  }

  for (const auto &update : updates) {
    if (update.second == 0) {
      bids.erase(update.first);
    } else {
      bids[update.first] = update.second;
    }
  }

  if (sell_order.getRemainingQuantity() > 0) {
    levels_.add_ask(sell_order);
  }
}
