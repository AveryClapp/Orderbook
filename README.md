# Orderbook

A C++ limit orderbook implementation optimized for throughput, achieving **1.89M+ orders/second** with microsecond latency.

## Features

- **Limit Orders**: Buy and sell orders with price-time priority matching
- **Object Pooling**: Pre-allocated order memory to avoid heap allocations
- **Optimized Matching Engine**: Branch prediction hints and flattened loops

## Performance

| Operation       | Throughput        |
| --------------- | ----------------- |
| Order Insertion | ~1.89M orders/sec |

_Benchmarked on 8x2400MHz CPU with 64KB L1 cache_

## Quick Start

```bash
# Install dependencies
conan install . --output-folder=build --build=missing

# Build
cmake --preset conan-default
cmake --build --preset conan-release

# Run benchmarks
./build/benchmarks
```

## Architecture

```
Orderbook
├── OrderbookLevels    # Price level management
├── OrderPool          # Memory pool for orders
├── Level              # Price level with order queue
└── Order              # Individual order representation
```

**Key Data Structures:**

- `std::map` for price levels
- `std::deque` for order queues at each price
- `std::unordered_map` for order ID lookups

## Usage

```cpp
Orderbook orderbook;

// Create and send order
NewOrderData order = {1, 100, 50, Direction::Buy}; // id, price, qty, side
Message msg{MessageType::Order, {.new_order = order}};
orderbook.receive_message(msg);

// Cancel order
CancelData cancel = {1}; // order id
Message cancel_msg{MessageType::Cancel, {.cancel = cancel}};
orderbook.receive_message(cancel_msg);

// Get best prices
auto best_bid = orderbook.get_best_bid();
auto best_ask = orderbook.get_best_ask();
```

## Dependencies

- **Google Benchmark**: Performance measurement
- **GTest**: Unit testing
- **CMake + Conan**: Build system

## License

MIT
