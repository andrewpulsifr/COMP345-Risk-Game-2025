#include "../include/Orders.h"

// ===== Base Order =====
Order::Order(std::string desc) : description(desc) {}
Order::~Order() {}

std::ostream& operator<<(std::ostream& os, const Order& order) {
    os << order.description;  // TODO: later also print "effect" once implemented
    return os;
}

// ===== Derived Orders (stubs only for now) =====
DeployOrder::DeployOrder() : Order("Deploy Order") {}
bool DeployOrder::validate() const { return true; }   // TODO: implement logic
void DeployOrder::execute() {}                        // TODO: implement logic

AdvanceOrder::AdvanceOrder() : Order("Advance Order") {}
bool AdvanceOrder::validate() const { return true; }  // TODO
void AdvanceOrder::execute() {}                       // TODO

BombOrder::BombOrder() : Order("Bomb Order") {}
bool BombOrder::validate() const { return true; }     // TODO
void BombOrder::execute() {}                          // TODO

BlockadeOrder::BlockadeOrder() : Order("Blockade Order") {}
bool BlockadeOrder::validate() const { return true; } // TODO
void BlockadeOrder::execute() {}                      // TODO

AirliftOrder::AirliftOrder() : Order("Airlift Order") {}
bool AirliftOrder::validate() const { return true; }  // TODO
void AirliftOrder::execute() {}                       // TODO

NegotiateOrder::NegotiateOrder() : Order("Negotiate Order") {}
bool NegotiateOrder::validate() const { return true; } // TODO
void NegotiateOrder::execute() {}                      // TODO

// ===== OrdersList =====
OrdersList::~OrdersList() {
    for (auto o : orders) delete o;
}

void OrdersList::add(Order* o) {
    orders.push_back(o);
}

void OrdersList::remove(int index) {
    if (index >= 0 && index < (int)orders.size()) {
        delete orders[index];
        orders.erase(orders.begin() + index);
    }
}

void OrdersList::move(int from, int to) {
    if (from >= 0 && from < (int)orders.size() &&
        to >= 0 && to < (int)orders.size()) {
        auto temp = orders[from];
        orders.erase(orders.begin() + from);
        orders.insert(orders.begin() + to, temp);
    }
}

void OrdersList::print() const {
    std::cout << "OrdersList:\n";
    for (size_t i = 0; i < orders.size(); i++) {
        std::cout << i << ": " << *orders[i] << "\n";
    }
}
