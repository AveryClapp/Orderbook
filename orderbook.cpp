#include "Orderbook.h"

/**
 * Upon receiving an order, send it to a function that handles its direction.
 */
void Orderbook::receive_order(Order &order) {
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
  auto &asks = levels_.get_asks();
  std::vector<std::pair<Price, Quantity>> updates{};
  for (auto &level : asks) {
    Price level_price = level.first;
    Quantity remaining_quantity = buy_order.getRemainingQuantity();

    if (level_price <= buy_order.getPrice()) {
      Quantity max_quantity = std::min(remaining_quantity, level.second);
      // Case 1: The order is fulfilled
      buy_order.remaining_quantity_ -= max_quantity;
      updates.emplace_back({level_price, level.second - max_quantity)};
      if (buy_order.getRemainingQuantity() == 0) {
        break;
      }
    } else {
      break; // No other orders will fulfill our requirements
    }
  }
  // Second pass for updating levelinfos
  for (const auto &update : updates) {
    if (update.second == 0) {
      asks.erase(update.first);
    } else {
      asks[update.first] = update.second;
    }
  }

  if (buy_order.getRemainingQuantity() > 0) {
    levels_.add_ask(buy_order);
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
      break; // No other orders will fulfill our requirements
    }
  }

  for (const auto &update : updates) {
    if (update.second == 1) {
      bids.erase(update.first);
    } else {
      bids[update.first] = update.second;
    }
  }

  if (sell_order.getRemainingQuantity() > 0) {
    levels_.add_ask(buy_order);
  }
}
