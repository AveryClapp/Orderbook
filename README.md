# Orderbook

## Overview

This order matching engine is designed for high-frequency trading environments where microsecond latency matters. It implements multiple matching algorithms and supports various order types commonly used in modern electronic exchanges. Currently, this is just for Limit order books.

## Iterative Benchmarking

With the goal of making this as fast as possible, I figured it would be a good idea to document the journey from making this fast as possible starting from a working version. This starts with no explicit optimizations and then goes through step by step what I refined to improve performance. For the sake of brevity, I will analyze just the first (smallest) case for each benchmark output.

Some Specs:

- L1 Data Cache: 64 KiB
- L1 Instruction Cache: 124 KiB
- L2 Data Cache: 4096 KiB
- 8 x 2400 MHz CPUs (more on this later)

### Iteration 1: The Starting Point

AddOrdersEmpty: 1162 ns  
AddOrdersPopulated: 1264 ns  
OrderMatching: 989 ns  
BestBidAsk: 1.19 ns  
OrderCancellation: 913 ns

### Iteration 2: Order Pooling

AddOrdersEmpty: 1089 ns
AddOrdersPopulated: 1214 ns  
OrderMatching: 914 ns  
BestBidAsk: 1.19 ns  
OrderCancellation: 890 ns

### Iteration 3: Cache Friendliness
