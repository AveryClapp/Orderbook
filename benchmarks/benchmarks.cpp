#include "include/core/Orderbook.h"
#include "tests/helpers/test_utils.h"
#include <benchmark/benchmark.h>
#include <random>
#include <vector>

// Benchmark adding orders to an empty orderbook
static void BM_AddOrdersEmpty(benchmark::State &state) {
  for (auto _ : state) {
    state.PauseTiming();
    Orderbook orderbook;
    auto order = test_utils::create_order(1, 100, 50, Direction::Buy);
    Message msg{order};
    state.ResumeTiming();

    orderbook.receive_message(msg);

    benchmark::DoNotOptimize(orderbook);
  }
}
BENCHMARK(BM_AddOrdersEmpty);

// Benchmark adding orders to a populated orderbook
static void BM_AddOrdersPopulated(benchmark::State &state) {
  const int book_depth = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();
    Orderbook orderbook;

    // Pre-populate the orderbook
    for (int i = 0; i < book_depth; ++i) {
      auto buy_order =
          test_utils::create_order(i * 2, 100 - i, 50, Direction::Buy);
      auto sell_order =
          test_utils::create_order(i * 2 + 1, 105 + i, 50, Direction::Sell);
      orderbook.receive_message(Message{buy_order});
      orderbook.receive_message(Message{sell_order});
    }

    auto new_order =
        test_utils::create_order(book_depth * 2 + 1, 102, 25, Direction::Buy);
    Message msg{new_order};
    state.ResumeTiming();

    orderbook.receive_message(msg);

    benchmark::DoNotOptimize(orderbook);
  }
}
BENCHMARK(BM_AddOrdersPopulated)->Range(1, 1000);

// Benchmark order matching performance
static void BM_OrderMatching(benchmark::State &state) {
  for (auto _ : state) {
    state.PauseTiming();
    Orderbook orderbook;

    // Add a sell order
    auto sell_order = test_utils::create_order(1, 100, 100, Direction::Sell);
    orderbook.receive_message(Message{sell_order});

    // Create matching buy order
    auto buy_order = test_utils::create_order(2, 100, 50, Direction::Buy);
    Message msg{buy_order};
    state.ResumeTiming();

    orderbook.receive_message(msg);

    benchmark::DoNotOptimize(orderbook);
  }
}
BENCHMARK(BM_OrderMatching);

// Benchmark best bid/ask retrieval
static void BM_BestBidAsk(benchmark::State &state) {
  Orderbook orderbook;

  // Pre-populate with some orders
  for (int i = 0; i < 100; ++i) {
    auto buy_order =
        test_utils::create_order(i * 2, 100 - i, 50, Direction::Buy);
    auto sell_order =
        test_utils::create_order(i * 2 + 1, 105 + i, 50, Direction::Sell);
    orderbook.receive_message(Message{buy_order});
    orderbook.receive_message(Message{sell_order});
  }

  for (auto _ : state) {
    auto best_bid = orderbook.get_best_bid();
    auto best_ask = orderbook.get_best_ask();

    benchmark::DoNotOptimize(best_bid);
    benchmark::DoNotOptimize(best_ask);
  }
}
BENCHMARK(BM_BestBidAsk);

// Benchmark order cancellation
static void BM_OrderCancellation(benchmark::State &state) {
  const int order_count = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();
    Orderbook orderbook;
    std::vector<ID> order_ids;

    // Add orders to cancel
    for (int i = 0; i < order_count; ++i) {
      auto order = test_utils::create_order(i, 100, 50, Direction::Buy);
      order_ids.push_back(order->id);
      orderbook.receive_message(Message{order});
    }

    // Choose a random order to cancel
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, order_ids.size() - 1);
    ID cancel_id = order_ids[dis(gen)];
    Message cancel_msg = test_utils::create_cancel_message(cancel_id);
    state.ResumeTiming();

    orderbook.receive_message(cancel_msg);

    benchmark::DoNotOptimize(orderbook);
  }
}
BENCHMARK(BM_OrderCancellation)->Range(1, 1000);

BENCHMARK_MAIN();
