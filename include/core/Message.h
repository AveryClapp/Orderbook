#pragma once
#include "include/core/Direction.h"
#include "include/core/Using.h"

enum class MessageType : uint8_t { Cancel, Order };
struct NewOrderData {
  ID id;               // unsigned long -> 8 bytes
  Price price;         // 4 bytes
  Quantity quantity;   // 4 bytes
  Direction direction; // 1 byte
} __attribute__((packed));

struct CancelData {
  ID order_id; // 8 bytes
} __attribute__((packed));

struct Message {
  MessageType type;
  union {
    NewOrderData new_order;
    CancelData cancel;
  } data;
} __attribute__((packed));
