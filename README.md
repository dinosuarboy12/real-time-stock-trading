# real-time-stock-trading
Lock-free stock order matching system in C++ using atomic operations and multi-threading. The simulation creates a fixed-size order book, where multiple threads concurrently add randomly generated buy and sell orders. After orders are submitted we match buy orders with sell orders based on defined criteria.

## Features

- **Lock-Free Order Book:**  
  Uses an atomic counter to insert/write orders into a fixed-size array without locks.

- **Multi-Threaded Order Generation:**  
  Simulates concurrent order submissions from multiple threads, in the code it is set to generate 50 * 6 = 300 total orders by default

- **Randomized Order Parameters:**  
  Each order is assigned a random ticker (0-1023), quantity (1-1000) , price (between 1$-1000$), and buy/sell type. Sleep times between order submissions are also randomized to represent real time random latency.

- **O(n) Matching Algorithm:**  
  The matching function scans the order book in linear time to match buy orders with the lowest available sell order for the same ticker (as specified).

## Files

- `main.cpp`  
  Contains the complete source code including:
  - The `Order` structure with an optimized memory layout.
  - The `addOrder` function to add orders into the order book.
  - The `simulateOrders` function to generate random orders.
  - The `matchOrder` function that matches buy and sell orders.
  - The `main` function which launches threads, simulates order creation, and outputs match results.

## Expected Output
The program will output debug messages for each order added.
After all threads have finished, it prints the total number of orders submitted.
Finally, it outputs the list of matched buy and sell orders (if any), along with their order IDs, ticker values, and prices.

## Code Overview

- Order Structure:
Defines an order with optimized memory layout. The ticker is represented as an integer between 0 and 1023.

- Order Book:
Uses a fixed-size array (orderBook) and an atomic counter (orderCount) to ensure thread-safe, lock-free order insertion.

- Order Generation:
The simulateOrders function randomly generates order parameters and submits orders concurrently via multiple threads.

- Matching Function:
The matchOrder function scans the order book in two passes:
  1. Records the lowest sell price for each ticker.
  2. Matches buy orders with available sell orders if the buy price is at least as high as the lowest sell price.
     
Matches then get recorded and all matches get returned at the end

## Example Output

```plaintext
C:\Users\danst\CLionProjects\Test\cmake-build-debug\Test.exe
Add Order ID: 0 | Buy or Sell: Sell | Ticker: 390 | Quantity: 237 | Price: 890.365
Add Order ID: 1 | Buy or Sell: Sell | Ticker: 329 | Quantity: 285 | Price: 198.227
...
Add Order ID: 296 | Buy or Sell: Buy | Ticker: 92 | Quantity: 235 | Price: 898.656
Add Order ID: 297 | Buy or Sell: Sell | Ticker: 70 | Quantity: 877 | Price: 163.287
Add Order ID: 298 | Buy or Sell: Buy | Ticker: 1014 | Quantity: 399 | Price: 599.988
Add Order ID: 299 | Buy or Sell: Buy | Ticker: 564 | Quantity: 446 | Price: 425.732
Total orders: 300
Matches: 8
Match: 0 Buy Order ID 40, ticker 58, price 761.061 matched with Sell Order ID 192, ticker 58, price 397.543
Match: 1 Buy Order ID 124, ticker 112, price 912.3 matched with Sell Order ID 64, ticker 112, price 410.269
Match: 2 Buy Order ID 145, ticker 98, price 761.199 matched with Sell Order ID 35, ticker 98, price 122.502
Match: 3 Buy Order ID 177, ticker 470, price 817.861 matched with Sell Order ID 36, ticker 470, price 724.158
Match: 4 Buy Order ID 266, ticker 604, price 753.529 matched with Sell Order ID 198, ticker 604, price 554.741
Match: 5 Buy Order ID 292, ticker 68, price 725.873 matched with Sell Order ID 132, ticker 68, price 195.634
Match: 6 Buy Order ID 295, ticker 523, price 972.114 matched with Sell Order ID 256, ticker 523, price 287.413
Match: 7 Buy Order ID 296, ticker 92, price 898.656 matched with Sell Order ID 130, ticker 92, price 79.8941
```

## Future Improvements to be done-
- Order Updates and Removals:
Implement atomic updates to handle partial order fills or order removals.
- Error Handling:
Enhance error handling for a full order book or other edge cases.
- Improved Matching Logic:
Incorporate more sophisticated matching criteria and mechanisms for real-world trading scenarios. Ex. greater than or equal to buy price + transaction cost(such as slippage/market impact) 
- After Matching we could also delete the buy/sell orders that match based on the number of buy orders.
