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
    os << order.description;  // TODO: in later assignment will print effect here
    return os;
}

// ---------- Deploy ----------
DeployOrder::DeployOrder() : Order("Deploy") {}
DeployOrder::DeployOrder(Player* issuer, Territory* target, int amount)
    : Order("Deploy"), issuer_(issuer), target_(target), amount_(amount) {}
    
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
Order* DeployOrder::clone()   const { return new DeployOrder(*this); }

// ---------- Advance ----------
AdvanceOrder::AdvanceOrder() : Order("Advance") {}
AdvanceOrder::AdvanceOrder(Player* issuer, Territory* source, Territory* target, int amount)
    : Order("Advance"), issuer_(issuer), source_(source), target_(target), amount_(amount) {}

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
Order* AdvanceOrder::clone()  const { return new AdvanceOrder(*this); }

// ---------- Bomb ----------
BombOrder::BombOrder() : Order("Bomb") {}
BombOrder::BombOrder(Player* issuer, Territory* target)
    : Order("Bomb"), issuer_(issuer), target_(target) {}

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
Order* BombOrder::clone()     const { return new BombOrder(*this); }

// ---------- Blockade ----------
BlockadeOrder::BlockadeOrder() : Order("Blockade") {}
BlockadeOrder::BlockadeOrder(Player* issuer, Territory* target)
    : Order("Blockade"), issuer_(issuer), target_(target) {}

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
Order* BlockadeOrder::clone() const { return new BlockadeOrder(*this); }

// ---------- Airlift ----------
AirliftOrder::AirliftOrder() : Order("Airlift") {}
AirliftOrder::AirliftOrder(Player* issuer, Territory* source, Territory* target, int amount)
    : Order("Airlift"), issuer_(issuer), source_(source), target_(target), amount_(amount) {}

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
Order* AirliftOrder::clone()  const { return new AirliftOrder(*this); }

// ---------- Negotiate ----------

// ---- NegotiateOrder ----
NegotiateOrder::NegotiateOrder() : Order("Negotiate") {}
NegotiateOrder::NegotiateOrder(Player* issuer, Player* other)
    : Order("Negotiate"), issuer_(issuer), other_(other) {}

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
Order* NegotiateOrder::clone()const { return new NegotiateOrder(*this); }

// ----- OrdersList -----

// --- destructor ---
OrdersList::~OrdersList() {
    for (auto* o : orders) delete o;
    orders.clear();
}

// --- copy constructor (deep), using clone for ambiguity between order typing ---
OrdersList::OrdersList(const OrdersList& other) {
    orders.reserve(other.orders.size());
    for (auto* o : other.orders) orders.push_back(o ? o->clone() : nullptr);
}

// --- copy assign (deep copy) ---
OrdersList& OrdersList::operator=(const OrdersList& other) {
    if (this == &other) return *this;
    std::vector<Order*> tmp;
    tmp.reserve(other.orders.size());
    for (auto* o : other.orders) tmp.push_back(o ? o->clone() : nullptr);
    for (auto* o : orders) delete o;
    orders.swap(tmp);
    return *this;
}

// --- move ctor ---
OrdersList::OrdersList(OrdersList&& other) noexcept
    : orders(std::move(other.orders)) {
    other.orders.clear();
}

// --- move assign ---
OrdersList& OrdersList::operator=(OrdersList&& other) noexcept {
    if (this != &other) {
        for (auto* o : orders) delete o;
        orders = std::move(other.orders);
        other.orders.clear();
    }
    return *this;
}

// --- mutators ---
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

// --- debug/printing ---
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
