#include "OrderbookLevels.h"

OrderbookLevels::add_ask(const &Order ask) {
  asks_.emplace({ask.getPrice(), ask.getRemainingQuantity()});
}

OrderbookLevels::add_bid(const &Order bid) {
  bids_.emplace({bid.getPrice(), bid.getRemainingQuantity()});
}
