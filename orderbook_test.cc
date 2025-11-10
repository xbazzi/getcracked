#include <algorithm>
#include <cassert>
#include <cctype>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>
#include <vector>

using Id       = size_t;
using Price    = long;
using Quantity = int;

// Order class (same as in Orderbook.cc)
class Order {
   public:
    enum class Side { BUY, SELL };

   private:
    Id id_;
    Price level_;
    bool is_buy_;
    Quantity qty_;
    Side side_;

   public:
    Order(Id orderId, Price level, bool isBuy, Quantity quantity)
        : id_{ orderId },
          level_{ level },
          is_buy_{ isBuy },
          qty_{ quantity },
          side_{ isBuy ? Side::BUY : Side::SELL } {}

    friend std::ostream& operator<<(std::ostream& os, const Order& o) {
        os << "id: " << o.get_id() << ", "
           << "level: " << o.get_level() << ", "
           << "isbuy: " << o.is_buy() << ", "
           << "qty: " << o.get_qty();
        return os;
    }

    bool is_buy() const noexcept { return is_buy_; }

    Id OrderId() const { return id_; }

    Side get_side() const noexcept { return side_; }

    Quantity get_qty() const { return qty_; }

    Id get_id() const { return id_; }

    Price get_level() const { return level_; }

    void set_qty(Quantity new_qty) { qty_ = static_cast<Quantity>(new_qty); }
};

using Orders = std::vector<Order>;

struct Trade
{
    Id OrderIdA;
    Id OrderIdB;
    Id AggressorOrderId;
    bool AggressorIsBuy;
    Price Level;
    Quantity Size;
};

using Trades = std::vector<Trade>;

class Orderbook {
    Orders bids_{};
    Orders asks_{};

   public:
    static constexpr inline std::uint64_t reserved_size_ = 20UL;

    Orderbook() {
        bids_.reserve(reserved_size_);
        asks_.reserve(reserved_size_);
    }

    bool can_match(const Order& buy, const Order& sell) {
        return (buy.get_level() >= sell.get_level());
    }

    bool is_dupe(const Order& order) {
        auto is_match = [&order](const Order& o)
        { return (o.get_id() == order.get_id()); };

        return std::ranges::any_of(bids_, is_match) ||
               std::ranges::any_of(asks_, is_match);
    }

    void insert_order(const Order& order) {
        if (is_dupe(order)) return;
        if (order.is_buy())
        {
            auto pos =
                std::ranges::lower_bound(bids_, order.get_level(),
                                         std::less<Price>(), &Order::get_level);
            bids_.insert(pos, order);
            return;
        }
        auto pos = std::ranges::lower_bound(
            asks_, order.get_level(), std::greater<Price>(), &Order::get_level);
        asks_.insert(pos, order);
    }

    [[nodiscard]]
    Trades AddOrder(const Order& incoming) {
        Trades trades;
        if (is_dupe(incoming)) return trades;

        auto& opposite_side = incoming.is_buy() ? asks_ : bids_;

        Quantity remaining = incoming.get_qty();

        while (!opposite_side.empty() && remaining > 0)
        {
            Order& best = opposite_side.back();
            bool match  = incoming.is_buy() ? can_match(incoming, best)
                                            : can_match(best, incoming);
            if (!match) break;

            Quantity trade_size = std::min(best.get_qty(), remaining);
            Price trade_price   = best.get_level();

            trades.emplace_back(Trade{ best.get_id(), incoming.get_id(),
                                       incoming.get_id(), incoming.is_buy(),
                                       trade_price, trade_size });

            best.set_qty(best.get_qty() - trade_size);
            remaining -= trade_size;

            if (best.get_qty() == 0) opposite_side.pop_back();
        }

        if (remaining > 0)
        {
            Order corrected_incoming = incoming;
            corrected_incoming.set_qty(remaining);
            insert_order(corrected_incoming);
        }
        return trades;
    }

    void CancelOrder(Id order_id) {
        auto id_match = [=](const Order& o) { return o.get_id() == order_id; };
        auto bid_it   = std::ranges::find_if(bids_, id_match);
        if (bid_it != std::end(bids_))
        {
            bids_.erase(bid_it);
            return;
        }
        auto asks_it = std::ranges::find_if(asks_, id_match);
        if (asks_it != std::end(asks_))
        {
            asks_.erase(asks_it);
            return;
        }
    }

    // Helper methods for testing
    size_t bids_count() const { return bids_.size(); }
    size_t asks_count() const { return asks_.size(); }
    const Orders& get_bids() const { return bids_; }
    const Orders& get_asks() const { return asks_; }
};

// Test utilities
void assert_true(bool condition, const std::string& test_name,
                 const std::string& message) {
    if (!condition)
    {
        std::cerr << "FAILED: " << test_name << " - " << message << std::endl;
        std::exit(1);
    }
}

void print_test_passed(const std::string& test_name) {
    std::cout << "✓ PASSED: " << test_name << std::endl;
}

// Test 1: one-order - Add a single order, no trades expected
void test_one_order() {
    const std::string test_name = "one-order";
    Orderbook book;

    Order buy_order{ 1, 100, true, 10 };
    Trades trades = book.AddOrder(buy_order);

    assert_true(trades.empty(), test_name, "No trades should occur");
    assert_true(book.bids_count() == 1, test_name, "Should have 1 bid");
    assert_true(book.asks_count() == 0, test_name, "Should have 0 asks");

    print_test_passed(test_name);
}

// Test 2: two-orders - Add a buy then a sell order that matches
void test_two_orders() {
    const std::string test_name = "two-orders";
    Orderbook book;

    // Add buy order at price 100
    Order buy_order{ 1, 100, true, 10 };
    Trades trades1 = book.AddOrder(buy_order);
    assert_true(trades1.empty(), test_name, "First order should not trade");

    // Add sell order at price 100 (should match)
    Order sell_order{ 2, 100, false, 10 };
    Trades trades2 = book.AddOrder(sell_order);

    assert_true(trades2.size() == 1, test_name, "Should have exactly 1 trade");
    assert_true(trades2[0].OrderIdA == 1, test_name,
                "Trade OrderIdA should be 1");
    assert_true(trades2[0].OrderIdB == 2, test_name,
                "Trade OrderIdB should be 2");
    assert_true(trades2[0].AggressorOrderId == 2, test_name,
                "Aggressor should be order 2");
    assert_true(trades2[0].AggressorIsBuy == false, test_name,
                "Aggressor should be sell");
    assert_true(trades2[0].Level == 100, test_name,
                "Trade level should be 100");
    assert_true(trades2[0].Size == 10, test_name, "Trade size should be 10");
    assert_true(book.bids_count() == 0, test_name,
                "Should have 0 bids after match");
    assert_true(book.asks_count() == 0, test_name,
                "Should have 0 asks after match");

    print_test_passed(test_name);
}

// Test 3: two-orders-no-match - Add two orders with prices that do not cross
void test_two_orders_no_match() {
    const std::string test_name = "two-orders-no-match";
    Orderbook book;

    // Add buy order at price 95
    Order buy_order{ 1, 95, true, 10 };
    Trades trades1 = book.AddOrder(buy_order);
    assert_true(trades1.empty(), test_name, "First order should not trade");

    // Add sell order at price 105 (should NOT match - spread is too wide)
    Order sell_order{ 2, 105, false, 10 };
    Trades trades2 = book.AddOrder(sell_order);

    assert_true(trades2.empty(), test_name, "Should have no trades");
    assert_true(book.bids_count() == 1, test_name, "Should have 1 bid");
    assert_true(book.asks_count() == 1, test_name, "Should have 1 ask");

    print_test_passed(test_name);
}

// Test 4: three-orders-partial-match - Add multiple orders with partial fills
void test_three_orders_partial_match() {
    const std::string test_name = "three-orders-partial-match";
    Orderbook book;

    // Add first buy order: 10 @ 100
    Order buy1{ 1, 100, true, 10 };
    Trades trades1 = book.AddOrder(buy1);
    assert_true(trades1.empty(), test_name, "First order should not trade");

    // Add second buy order: 5 @ 100
    Order buy2{ 2, 100, true, 5 };
    Trades trades2 = book.AddOrder(buy2);
    assert_true(trades2.empty(), test_name, "Second order should not trade");
    assert_true(book.bids_count() == 2, test_name, "Should have 2 bids");

    // Add sell order: 12 @ 100 (should match both buys partially)
    // Should trade 10 with order 1, then 2 with order 2, leaving 0 remaining
    Order sell{ 3, 100, false, 12 };
    Trades trades3 = book.AddOrder(sell);

    assert_true(trades3.size() == 2, test_name, "Should have 2 trades");

    // First trade: full fill of buy1 (10 qty)
    assert_true(trades3[0].OrderIdA == 1, test_name,
                "First trade resting order should be 1");
    assert_true(trades3[0].Size == 10, test_name,
                "First trade size should be 10");

    // Second trade: partial fill of buy2 (2 qty out of 5)
    assert_true(trades3[1].OrderIdA == 2, test_name,
                "Second trade resting order should be 2");
    assert_true(trades3[1].Size == 2, test_name,
                "Second trade size should be 2");

    // Remaining quantity of buy2 (3) should still be in the book
    assert_true(book.bids_count() == 1, test_name,
                "Should have 1 bid remaining");
    assert_true(book.asks_count() == 0, test_name, "Should have 0 asks");
    assert_true(book.get_bids()[0].get_qty() == 3, test_name,
                "Remaining bid qty should be 3");

    print_test_passed(test_name);
}

// Test 5: two-same-order-ids - Adding two orders with the same ID
void test_two_same_order_ids() {
    const std::string test_name = "two-same-order-ids";
    Orderbook book;

    // Add first order with ID 1
    Order buy1{ 1, 100, true, 10 };
    Trades trades1 = book.AddOrder(buy1);
    assert_true(trades1.empty(), test_name, "First order should not trade");
    assert_true(book.bids_count() == 1, test_name, "Should have 1 bid");

    // Try to add second order with same ID 1 (should be rejected)
    Order buy2{ 1, 105, true, 15 };
    Trades trades2 = book.AddOrder(buy2);

    assert_true(trades2.empty(), test_name, "Duplicate order should not trade");
    assert_true(book.bids_count() == 1, test_name,
                "Should still have only 1 bid");
    assert_true(book.get_bids()[0].get_qty() == 10, test_name,
                "Original order should be unchanged");
    assert_true(book.get_bids()[0].get_level() == 100, test_name,
                "Original order price should be unchanged");

    print_test_passed(test_name);
}

// Test 6: cancel-order - Cancel an order before adding its match
void test_cancel_order() {
    const std::string test_name = "cancel-order";
    Orderbook book;

    // Add buy order at price 100
    Order buy_order{ 1, 100, true, 10 };
    Trades trades1 = book.AddOrder(buy_order);
    assert_true(trades1.empty(), test_name, "First order should not trade");
    assert_true(book.bids_count() == 1, test_name, "Should have 1 bid");

    // Cancel the buy order
    book.CancelOrder(1);
    assert_true(book.bids_count() == 0, test_name,
                "Should have 0 bids after cancel");

    // Add sell order at price 100 (would have matched, but buy is cancelled)
    Order sell_order{ 2, 100, false, 10 };
    Trades trades2 = book.AddOrder(sell_order);

    assert_true(trades2.empty(), test_name, "Should have no trades");
    assert_true(book.bids_count() == 0, test_name, "Should have 0 bids");
    assert_true(book.asks_count() == 1, test_name, "Should have 1 ask");

    print_test_passed(test_name);
}

int main() {
    std::cout << "\n=== Running Orderbook Tests ===" << std::endl;

    test_one_order();
    test_two_orders();
    test_two_orders_no_match();
    test_three_orders_partial_match();
    test_two_same_order_ids();
    test_cancel_order();

    std::cout << "\n✓ All tests passed!" << std::endl;
    return 0;
}
