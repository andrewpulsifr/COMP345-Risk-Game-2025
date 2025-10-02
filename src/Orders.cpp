#include <algorithm>
#include <iostream>
#include <sstream>
#include "../include/Orders.h"
#include "../include/Map.h"
#include "../include/Player.h"

// ===== Base Order =====
Order::Order(std::string desc) : description(desc) {}
Order::~Order() {}

std::ostream& operator<<(std::ostream& os, const Order& order) {
    os << order.description;  // TODO: later also print "effect" once implemented
    return os;
}

// ---------- Deploy ----------
bool DeployOrder::validate() const {
    if (!issuer_ || !target_ || amount_ <= 0) return false;
    return target_->getOwner() == issuer_;
}

void DeployOrder::execute() {
    if (!validate()) { effect_ = "Invalid deploy"; return; }
    std::ostringstream ss;
    ss << "Deploy " << amount_ << " to " << target_->getName()
       << " (owner: " << target_->getOwner()->getPlayerName() << ")";
    effect_ = ss.str();
}

std::string DeployOrder::name() const { return "Deploy"; }

// ---------- Advance ----------
static bool areAdjacent(const Territory* a, const Territory* b) {
    if (!a || !b) return false;
    const auto& adjs = a->getAdjacents();
    return std::find(adjs.begin(), adjs.end(), b) != adjs.end();
}

bool AdvanceOrder::validate() const {
    if (!issuer_ || !source_ || !target_ || amount_ <= 0) return false;
    if (source_->getOwner() != issuer_) return false;
    return areAdjacent(source_, target_);
}

void AdvanceOrder::execute() {
    if (!validate()) { effect_ = "Invalid advance"; return; }
    std::ostringstream ss;
    ss << "Advance " << amount_ << " from " << source_->getName()
       << " to " << target_->getName();
    effect_ = ss.str();
}

std::string AdvanceOrder::name() const { return "Advance"; }

// ---------- Bomb ----------
bool BombOrder::validate() const {
    if (!issuer_ || !target_) return false;
    return target_->getOwner() != issuer_;
}

void BombOrder::execute() {
    if (!validate()) { effect_ = "Invalid bomb"; return; }
    std::ostringstream ss;
    ss << "Bomb " << target_->getName() << " (owner: "
       << (target_->getOwner() ? target_->getOwner()->getPlayerName() : "none") << ")";
    effect_ = ss.str();
}

std::string BombOrder::name() const { return "Bomb"; }

// ---------- Blockade ----------
bool BlockadeOrder::validate() const {
    if (!issuer_ || !target_) return false;
    return target_->getOwner() == issuer_;
}

void BlockadeOrder::execute() {
    if (!validate()) { effect_ = "Invalid blockade"; return; }
    std::ostringstream ss;
    ss << "Blockade on " << target_->getName();
    effect_ = ss.str();
}

std::string BlockadeOrder::name() const { return "Blockade"; }

// ---------- Airlift ----------
bool AirliftOrder::validate() const {
    if (!issuer_ || !source_ || !target_ || amount_ <= 0) return false;
    return source_->getOwner() == issuer_;
}

void AirliftOrder::execute() {
    if (!validate()) { effect_ = "Invalid airlift"; return; }
    std::ostringstream ss;
    ss << "Airlift " << amount_ << " from " << source_->getName()
       << " to " << target_->getName();
    effect_ = ss.str();
}

std::string AirliftOrder::name() const { return "Airlift"; }

// ---------- Negotiate ----------
bool NegotiateOrder::validate() const {
    if (!issuer_ || !other_) return false;
    return issuer_ != other_;
}

void NegotiateOrder::execute() {
    if (!validate()) { effect_ = "Invalid negotiate"; return; }
    std::ostringstream ss;
    ss << "Negotiate truce between " << issuer_->getPlayerName()
       << " and " << other_->getPlayerName();
    effect_ = ss.str();
}

std::string NegotiateOrder::name() const { return "Negotiate"; }

// ----- OrdersList -----
OrdersList::~OrdersList() {
    for (Order* o : orders) delete o;
    orders.clear();
}

void OrdersList::add(Order* o) {
    if (!o) return;
    orders.push_back(o);
}

void OrdersList::remove(int index) {
    if (index < 0 || index >= static_cast<int>(orders.size())) return;
    delete orders[static_cast<size_t>(index)];
    orders.erase(orders.begin() + index);
}

void OrdersList::move(int from, int to) {
    if (from < 0 || to < 0) return;
    if (from >= static_cast<int>(orders.size()) || to >= static_cast<int>(orders.size())) return;
    if (from == to) return;
    Order* tmp = orders[static_cast<size_t>(from)];
    orders.erase(orders.begin() + from);
    orders.insert(orders.begin() + to, tmp);
}

void OrdersList::print() const {
    std::cout << "[OrdersList: " << orders.size() << " items]\n";
    for (size_t i = 0; i < orders.size(); ++i) {
        std::cout << "  " << i << ") " << *orders[i] << "\n";
    }
}

std::ostream& operator<<(std::ostream& os, const OrdersList& ol) {
    os << "[OrdersList: " << ol.orders.size() << " items]\n";
    for (size_t i = 0; i < ol.orders.size(); ++i) {
        os << "  " << i << ") " << *ol.orders[i] << "\n";
    }
    return os;
}