#include "include/core/Orderbook.h"
#include "tests/helpers/test_utils.h"
#include <benchmark/benchmark.h>
#include <cstdint>
#include <iomanip>
#include <random>
#include <vector>

// Benchmark adding orders to an empty orderbook
static void BM_AddOrdersEmpty(benchmark::State &state) {
  for (auto _ : state) {
    state.PauseTiming();
    Orderbook orderbook;
    NewOrderData order = test_utils::create_order(1UL, 100, 50, Direction::Buy);
    Message msg{MessageType::Order, {.new_order = order}};
    state.ResumeTiming();

    orderbook.receive_message(msg);

    benchmark::DoNotOptimize(orderbook);
  }
}
BENCHMARK(BM_AddOrdersEmpty);

// Benchmark adding orders to a populated orderbook
static void BM_AddOrdersPopulated(benchmark::State &state) {
  int64_t book_depth = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();
    Orderbook orderbook;

    // Pre-populate the orderbook
    for (int i = 0; i < book_depth; ++i) {
      NewOrderData buy_order = test_utils::create_order(
          static_cast<unsigned long>(i) * 2UL, 100 - i, 50, Direction::Buy);
      NewOrderData sell_order =
          test_utils::create_order(static_cast<unsigned long>(i) * 2UL + 1,
                                   105 + i, 50, Direction::Sell);
      orderbook.receive_message(
          Message{MessageType::Order, {.new_order = buy_order}});
      orderbook.receive_message(
          Message{MessageType::Order, {.new_order = sell_order}});
    }

    auto new_order = test_utils::create_order(
        static_cast<unsigned long>(book_depth) * 2UL + 1UL, 102, 25,
        Direction::Buy);
    Message msg{MessageType::Order, {.new_order = new_order}};
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
    auto sell_order = test_utils::create_order(1UL, 100, 100, Direction::Sell);
    orderbook.receive_message(
        Message{MessageType::Order, {.new_order = sell_order}});

    // Create matching buy order
    auto buy_order = test_utils::create_order(2UL, 100, 50, Direction::Buy);
    Message msg{MessageType::Order, {.new_order = buy_order}};
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
    auto buy_order = test_utils::create_order(
        static_cast<unsigned long>(i) * 2UL, 100 - i, 50, Direction::Buy);
    auto sell_order = test_utils::create_order(
        static_cast<unsigned long>(i) * 2UL + 1, 105 + i, 50, Direction::Sell);
    orderbook.receive_message(
        Message{MessageType::Order, {.new_order = buy_order}});
    orderbook.receive_message(
        Message{MessageType::Order, {.new_order = sell_order}});
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
  int64_t order_count = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();
    Orderbook orderbook;
    std::vector<ID> order_ids;

    // Add orders to cancel
    for (int i = 0; i < order_count; ++i) {
      auto order = test_utils::create_order(static_cast<unsigned long>(i), 100,
                                            50, Direction::Buy);
      order_ids.push_back(order.id);
      orderbook.receive_message(
          Message{MessageType::Order, {.new_order = order}});
    }

    // Choose a random order to cancel
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned long> dis(0UL,
                                                     order_ids.size() - 1UL);
    ID cancel_id = order_ids[static_cast<unsigned long>(dis(gen))];
    CancelData cancel_msg = test_utils::create_cancel_message(cancel_id);
    Message cancel_message{MessageType::Cancel, {.cancel = cancel_msg}};
    state.ResumeTiming();

    orderbook.receive_message(cancel_message);

    benchmark::DoNotOptimize(orderbook);
  }
}
BENCHMARK(BM_OrderCancellation)->Range(1, 1000);
// Throughput measurement benchmark
static void BM_OrderThroughput(benchmark::State &state) {
  const int64_t num_orders = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();

    Orderbook orderbook;
    std::vector<Message> orders;
    orders.reserve(num_orders);

    std::random_device rd;
    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<Price> price_dist(9900, 10100);
    std::uniform_int_distribution<Quantity> qty_dist(1, 1000);
    std::uniform_int_distribution<int> direction_dist(0, 1);

    // Pre-generate all orders to avoid timing overhead
    for (int64_t i = 0; i < num_orders; ++i) {
      NewOrderData order_data = test_utils::create_order(
          static_cast<ID>(i), price_dist(gen), qty_dist(gen),
          direction_dist(gen) ? Direction::Buy : Direction::Sell);
      orders.emplace_back(
          Message{MessageType::Order, {.new_order = order_data}});
    }

    state.ResumeTiming();

    // Process all orders
    for (const auto &msg : orders) {
      orderbook.receive_message(msg);
    }

    state.counters["OrdersPerSecond"] = benchmark::Counter(
        static_cast<double>(num_orders), benchmark::Counter::kIsRate);
    state.counters["OrdersProcessed"] = static_cast<double>(num_orders);
  }
}
BENCHMARK(BM_OrderThroughput)
    ->Range(1000, 100000)
    ->Unit(benchmark::kMicrosecond);

// Realistic market simulation benchmark
static void BM_RealisticMarketThroughput(benchmark::State &state) {
  const int64_t num_orders = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();

    Orderbook orderbook;
    std::vector<Message> orders;
    orders.reserve(num_orders);

    std::mt19937 gen(42);
    std::normal_distribution<double> price_dist(
        10000.0, 50.0); // More realistic price clustering
    std::exponential_distribution<double> qty_dist(
        0.01); // Smaller orders more common
    std::bernoulli_distribution direction_dist(0.5);
    std::bernoulli_distribution cancel_dist(0.1); // 10% cancellations

    std::vector<ID> active_order_ids;

    for (int64_t i = 0; i < num_orders; ++i) {
      if (!active_order_ids.empty() && cancel_dist(gen)) {
        // Generate cancel message
        std::uniform_int_distribution<size_t> cancel_idx(
            0, active_order_ids.size() - 1);
        ID cancel_id = active_order_ids[cancel_idx(gen)];
        active_order_ids.erase(active_order_ids.begin() + cancel_idx(gen));

        CancelData cancel_data = test_utils::create_cancel_message(cancel_id);
        orders.emplace_back(
            Message{MessageType::Cancel, {.cancel = cancel_data}});
      } else {
        // Generate new order
        Price price = static_cast<Price>(std::max(1.0, price_dist(gen)));
        Quantity qty =
            static_cast<Quantity>(std::max(1.0, qty_dist(gen) * 100));

        NewOrderData order_data = test_utils::create_order(
            static_cast<ID>(i), price, qty,
            direction_dist(gen) ? Direction::Buy : Direction::Sell);
        active_order_ids.push_back(static_cast<ID>(i));
        orders.emplace_back(
            Message{MessageType::Order, {.new_order = order_data}});
      }
    }

    state.ResumeTiming();

    for (const auto &msg : orders) {
      orderbook.receive_message(msg);
    }

    state.counters["MessagesPerSecond"] = benchmark::Counter(
        static_cast<double>(num_orders), benchmark::Counter::kIsRate);
  }
}
BENCHMARK(BM_RealisticMarketThroughput)
    ->Range(1000, 50000)
    ->Unit(benchmark::kMicrosecond);

// Memory pressure benchmark
static void BM_MemoryPressureThroughput(benchmark::State &state) {
  const int64_t order_count = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();

    Orderbook orderbook;
    std::mt19937 gen(42);
    std::uniform_int_distribution<Price> price_dist(9000, 11000); // Wide spread
    std::uniform_int_distribution<Quantity> qty_dist(1, 100);

    // Create orders that will create many price levels (worst case for
    // flat_map)
    std::vector<Message> orders;
    orders.reserve(order_count);

    for (int64_t i = 0; i < order_count; ++i) {
      NewOrderData order_data = test_utils::create_order(
          static_cast<ID>(i),
          price_dist(gen), // Each order likely gets its own price level
          qty_dist(gen), (i % 2 == 0) ? Direction::Buy : Direction::Sell);
      orders.emplace_back(
          Message{MessageType::Order, {.new_order = order_data}});
    }

    state.ResumeTiming();

    for (const auto &msg : orders) {
      orderbook.receive_message(msg);
    }

    state.counters["OrdersPerSecond"] = benchmark::Counter(
        static_cast<double>(order_count), benchmark::Counter::kIsRate);
  }
}
BENCHMARK(BM_MemoryPressureThroughput)->Range(1000, 20000);

// Latency percentiles benchmark
static void BM_LatencyDistribution(benchmark::State &state) {
  Orderbook orderbook;

  // Pre-populate orderbook
  std::mt19937 gen(42);
  std::uniform_int_distribution<Price> setup_price_dist(9950, 10050);
  std::uniform_int_distribution<Quantity> setup_qty_dist(10, 100);

  for (int i = 0; i < 1000; ++i) {
    NewOrderData order_data = test_utils::create_order(
        static_cast<ID>(i + 100000), // Avoid ID conflicts
        setup_price_dist(gen), setup_qty_dist(gen),
        (i % 2 == 0) ? Direction::Buy : Direction::Sell);
    Message msg{MessageType::Order, {.new_order = order_data}};
    orderbook.receive_message(msg);
  }

  // Measure individual order processing latency
  std::vector<double> latencies;
  std::uniform_int_distribution<Price> price_dist(9990,
                                                  10010); // Aggressive orders
  std::uniform_int_distribution<Quantity> qty_dist(1, 50);

  for (auto _ : state) {
    NewOrderData order_data = test_utils::create_order(
        static_cast<ID>(state.iterations()), price_dist(gen), qty_dist(gen),
        Direction::Buy);
    Message msg{MessageType::Order, {.new_order = order_data}};

    auto start = std::chrono::high_resolution_clock::now();
    orderbook.receive_message(msg);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration =
        std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    latencies.push_back(duration.count());
  }

  // Calculate percentiles
  if (!latencies.empty()) {
    std::sort(latencies.begin(), latencies.end());
    state.counters["P50_ns"] = latencies[latencies.size() * 50 / 100];
    state.counters["P95_ns"] = latencies[latencies.size() * 95 / 100];
    state.counters["P99_ns"] = latencies[latencies.size() * 99 / 100];
    state.counters["Max_ns"] = latencies.back();
  }
}
BENCHMARK(BM_LatencyDistribution)->Iterations(10000);

BENCHMARK_MAIN();
