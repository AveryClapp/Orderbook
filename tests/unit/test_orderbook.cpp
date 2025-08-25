#include "include/core/Orderbook.h"
#include "tests/helpers/test_utils.h"
#include <gtest/gtest.h>

class OrderbookCoreTest : public ::testing::Test {
protected:
  void SetUp() override { orderbook = std::make_unique<Orderbook>(); }

  std::unique_ptr<Orderbook> orderbook;
};

TEST_F(OrderbookCoreTest, TestAddSingleBuyOrder) {
  NewOrderData buy_msg = test_utils::create_order(1, 100, 50, Direction::Buy);
  Message message{MessageType::Order, {.new_order = buy_msg}};
  orderbook->receive_message(message);
  auto result = orderbook->get_best_bid();
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ((std::pair<int, size_t>{100, 1}), result.value());

  ASSERT_FALSE(orderbook->get_best_ask().has_value());
}

TEST_F(OrderbookCoreTest, TestAddSingleSellOrder) {
  NewOrderData sell_msg = test_utils::create_order(1, 105, 30, Direction::Sell);
  Message message{MessageType::Order, {.new_order = sell_msg}};
  orderbook->receive_message(message);

  auto ask_result = orderbook->get_best_ask();
  ASSERT_TRUE(ask_result.has_value());
  EXPECT_EQ((std::pair<int, size_t>{105, 1}), ask_result.value());

  ASSERT_FALSE(orderbook->get_best_bid().has_value());
}

TEST_F(OrderbookCoreTest, TestMultipleBuyOrdersSamePrice) {
  NewOrderData buy1 = test_utils::create_order(1, 100, 25, Direction::Buy);
  NewOrderData buy2 = test_utils::create_order(2, 100, 35, Direction::Buy);
  Message message1{MessageType::Order, {.new_order = buy1}};
  Message message2{MessageType::Order, {.new_order = buy2}};

  orderbook->receive_message(message1);
  orderbook->receive_message(message2);

  auto bid_result = orderbook->get_best_bid();
  ASSERT_TRUE(bid_result.has_value());
  EXPECT_EQ((std::pair<int, size_t>{100, 2}), bid_result.value());
}

TEST_F(OrderbookCoreTest, TestMultipleBuyOrdersDifferentPrices) {
  NewOrderData buy_high_data =
      test_utils::create_order(1, 102, 25, Direction::Buy);
  NewOrderData buy_low_data =
      test_utils::create_order(2, 98, 35, Direction::Buy);
  Message buy_high{MessageType::Order, {.new_order = buy_high_data}};
  Message buy_low{MessageType::Order, {.new_order = buy_low_data}};
  orderbook->receive_message(buy_high);
  orderbook->receive_message(buy_low);

  auto bid_result = orderbook->get_best_bid();
  ASSERT_TRUE(bid_result.has_value());
  EXPECT_EQ((std::pair<int, size_t>{102, 1}), bid_result.value());
}

TEST_F(OrderbookCoreTest, TestMultipleSellOrdersDifferentPrices) {
  NewOrderData sell_low_data =
      test_utils::create_order(1, 105, 25, Direction::Sell);
  NewOrderData sell_high_data =
      test_utils::create_order(2, 110, 35, Direction::Sell);

  Message sell_high{MessageType::Order, {.new_order = sell_high_data}};
  Message sell_low{MessageType::Order, {.new_order = sell_low_data}};
  orderbook->receive_message(sell_low);
  orderbook->receive_message(sell_high);

  auto ask_result = orderbook->get_best_ask();
  ASSERT_TRUE(ask_result.has_value());
  EXPECT_EQ((std::pair<int, size_t>{105, 1}), ask_result.value());
}

TEST_F(OrderbookCoreTest, TestExactMatch) {
  NewOrderData sell_data =
      test_utils::create_order(1, 100, 50, Direction::Sell);
  Message sell_msg{MessageType::Order, {.new_order = sell_data}};
  orderbook->receive_message(sell_msg);
  NewOrderData buy_data = test_utils::create_order(2, 100, 50, Direction::Buy);
  Message buy_msg{MessageType::Order,
                  {.new_order = buy_data}}; // Fixed: was using sell_data
  orderbook->receive_message(buy_msg);

  ASSERT_FALSE(orderbook->get_best_bid().has_value());
  ASSERT_FALSE(orderbook->get_best_ask().has_value());
}

TEST_F(OrderbookCoreTest, TestPartialFillBuyAggressive) {
  NewOrderData sell_data =
      test_utils::create_order(1, 100, 100, Direction::Sell);
  Message sell_msg{MessageType::Order, {.new_order = sell_data}};
  orderbook->receive_message(sell_msg);

  NewOrderData buy_data = test_utils::create_order(2, 100, 30, Direction::Buy);
  Message buy_msg{MessageType::Order, {.new_order = buy_data}};
  orderbook->receive_message(buy_msg);

  auto ask_result = orderbook->get_best_ask();
  ASSERT_TRUE(ask_result.has_value());
  EXPECT_EQ((std::pair<int, size_t>{100, 1}), ask_result.value());

  ASSERT_FALSE(orderbook->get_best_bid().has_value());
}

TEST_F(OrderbookCoreTest, TestPartialFillSellAggressive) {
  NewOrderData buy_data = test_utils::create_order(1, 100, 100, Direction::Buy);
  Message buy_msg{MessageType::Order, {.new_order = buy_data}};
  orderbook->receive_message(buy_msg);

  NewOrderData sell_data =
      test_utils::create_order(2, 100, 40, Direction::Sell);
  Message sell_msg{MessageType::Order, {.new_order = sell_data}};
  orderbook->receive_message(sell_msg);

  auto bid_result = orderbook->get_best_bid();
  ASSERT_TRUE(bid_result.has_value());
  EXPECT_EQ((std::pair<int, size_t>{100, 1}), bid_result.value());

  ASSERT_FALSE(orderbook->get_best_ask().has_value());
}

TEST_F(OrderbookCoreTest, TestMultipleLevelConsumption) {
  NewOrderData sell1 = test_utils::create_order(1, 100, 25, Direction::Sell);
  NewOrderData sell2 = test_utils::create_order(2, 101, 30, Direction::Sell);
  NewOrderData sell3 = test_utils::create_order(3, 102, 20, Direction::Sell);

  Message sell1_msg{MessageType::Order, {.new_order = sell1}};
  Message sell2_msg{MessageType::Order, {.new_order = sell2}};
  Message sell3_msg{MessageType::Order, {.new_order = sell3}};
  orderbook->receive_message(sell1_msg);
  orderbook->receive_message(sell2_msg);
  orderbook->receive_message(sell3_msg);

  auto ask_result = orderbook->get_best_ask();
  ASSERT_TRUE(ask_result.has_value());
  EXPECT_EQ((std::pair<int, size_t>{100, 1}), ask_result.value());

  ASSERT_EQ(std::nullopt, orderbook->get_best_bid());
}

TEST_F(OrderbookCoreTest, TestPriceImprovement) {
  NewOrderData sell_data =
      test_utils::create_order(1, 105, 50, Direction::Sell);
  Message sell_msg{MessageType::Order, {.new_order = sell_data}};
  orderbook->receive_message(sell_msg);

  NewOrderData buy_data = test_utils::create_order(2, 110, 25, Direction::Buy);
  Message buy_msg{MessageType::Order, {.new_order = buy_data}};
  orderbook->receive_message(buy_msg);

  auto ask_result = orderbook->get_best_ask();
  ASSERT_TRUE(ask_result.has_value());
  EXPECT_EQ((std::pair<int, size_t>{105, 1}), ask_result.value());

  // Bid quantity should be completely matched, no leftover bid
  EXPECT_EQ(std::nullopt, orderbook->get_best_bid());
}

TEST_F(OrderbookCoreTest, TestFillOrKillInvalid) {
  NewOrderData sell_data =
      test_utils::create_order(1, 105, 50, Direction::Sell);
  Message sell_msg{MessageType::Order, {.new_order = sell_data}};
  orderbook->receive_message(sell_msg);

  NewOrderData buy_data = test_utils::create_order(2, 110, 51, Direction::Buy,
                                                   OrderType::FillOrKill);
  Message buy_msg{MessageType::Order, {.new_order = buy_data}};
  orderbook->receive_message(buy_msg);

  auto ask_result = orderbook->get_best_ask();
  ASSERT_TRUE(ask_result.has_value());
  EXPECT_EQ((std::pair<int, size_t>{105, 1}), ask_result.value());

  // Bid quantity should be completely matched, no leftover bid
  EXPECT_EQ(std::nullopt, orderbook->get_best_bid());
}

TEST_F(OrderbookCoreTest, TestFillOrKillValid) {
  NewOrderData sell_data =
      test_utils::create_order(1, 105, 50, Direction::Sell);
  Message sell_msg{MessageType::Order, {.new_order = sell_data}};
  orderbook->receive_message(sell_msg);

  NewOrderData buy_data = test_utils::create_order(2, 110, 50, Direction::Buy,
                                                   OrderType::FillOrKill);
  Message buy_msg{MessageType::Order, {.new_order = buy_data}};
  orderbook->receive_message(buy_msg);

  EXPECT_EQ(std::nullopt, orderbook->get_best_ask());
  EXPECT_EQ(std::nullopt, orderbook->get_best_bid());
}
