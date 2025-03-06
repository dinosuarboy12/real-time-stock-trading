#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <random>
#include <chrono>
#include <mutex>

const int MAX_ORDERS = 10000;
const int NUM_TICKERS = 1024;

struct Order {
    double price;
    int orderId;
    int quantity;
    bool buy_or_sell;
    int ticker;

    Order() : price(0.0), orderId(0), quantity(0), buy_or_sell(false), ticker(0) {}

    Order(int orderId, bool isBuy, int ticker, int quantity, double price)
        : price(price), orderId(orderId), quantity(quantity), buy_or_sell(isBuy), ticker(ticker) {}
};

std::atomic<int> orderCount(0);
Order orderBook[MAX_ORDERS];

std::mutex coutMutex;

void addOrder(bool isBuy, int ticker, int quantity, double price) {
    int index = orderCount.fetch_add(1, std::memory_order_relaxed);
    if (index >= MAX_ORDERS) { return; }

    orderBook[index] = Order(index, isBuy, ticker, quantity, price);

    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "Add Order ID: " << orderBook[index].orderId
                  << " | Buy or Sell: " << (orderBook[index].buy_or_sell ? "Buy" : "Sell")
                  << " | Ticker: " << orderBook[index].ticker
                  << " | Quantity: " << orderBook[index].quantity
                  << " | Price: " << orderBook[index].price << "\n";
    }
}

void simulateOrders(int numOrders, int threadId) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> buy_or_sell_distribution(0, 1);
    std::uniform_int_distribution<> ticker_distribution(0, 1023);
    std::uniform_int_distribution<> quantity_distribution(1, 1000);
    std::uniform_real_distribution<> price_distribution(1.0, 1000.0);
    std::uniform_int_distribution<> latency_distribution(5, 50);     // random latency delay like in real time orders

    for (int i = 0; i < numOrders; ++i) {
        bool isBuy = (buy_or_sell_distribution(gen) == 1);
        int ticker = ticker_distribution(gen);
        int quantity = quantity_distribution(gen);
        double price = price_distribution(gen);

        addOrder(isBuy, ticker, quantity, price);

        int sleepTime = latency_distribution(gen);

        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    }
}

struct Match {
    Order buy;
    Order sell;
};

std::vector<Match> matchOrder() {

    double lowestSellPrice[NUM_TICKERS];
    int lowestSellIndex[NUM_TICKERS];
    for (int c = 0; c < NUM_TICKERS; ++c) {
        lowestSellPrice[c] = 1e9;
        lowestSellIndex[c] = -1;
    }

    int totalOrders = orderCount.load(std::memory_order_relaxed);

    // first pass:
    for (int i = 0; i < totalOrders; ++i) {
        Order order = orderBook[i];
        if (!order.buy_or_sell) {
            int t = order.ticker;
            if (order.price < lowestSellPrice[t]) {
                lowestSellPrice[t] = order.price;
                lowestSellIndex[t] = i;
            }
        }
    }
    std::vector<Match> matches;

    // second pass:
    for (int i = 0; i < totalOrders; ++i) {
        Order buyOrder = orderBook[i];
        if (buyOrder.buy_or_sell) {
            int t = buyOrder.ticker;
            if (lowestSellIndex[t] != -1 && buyOrder.price >= lowestSellPrice[t]) {
                Match m = { buyOrder, orderBook[lowestSellIndex[t]] };
                matches.push_back(m);

            }
        }
    }

    return matches;
}

int main() {

    int ordersPerThread = 50;
    int numThreads = 6;

    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i) {
        threads.push_back(std::thread(simulateOrders, ordersPerThread, i));
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Total orders: "
              << orderCount.load() << "\n";

    std::vector<Match> matches = matchOrder();
    std::cout << "Matches: " << matches.size() << "\n";
    for (std::size_t i = 0; i < matches.size(); i++) {
        std::cout << "Match: " << i << " Buy Order ID " << matches[i].buy.orderId << ", ticker " << matches[i].buy.ticker << ", price " << matches[i].buy.price
                  << " matched with Sell Order ID " << matches[i].sell.orderId << ", ticker " << matches[i].sell.ticker << ", price " << matches[i].sell.price << "\n";
    }
    return 0;
}
