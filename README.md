# Orderbook

- Levels wth price and quantity, given by the incoming data
- Orders came in, they have a price, quantity, id, direction, type associated
- Orders get matched based on order type with the orderbook
- If the order succeeds, update
- If the order fails, rollback
- The orderbook must have access/maintain context of all of the levels here
