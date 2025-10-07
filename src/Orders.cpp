#include <algorithm>
#include <iostream>
#include <sstream>
#include "../include/Orders.h"
#include "../include/Map.h"
#include "../include/Player.h"

// ===== Base Order =====

// @brief Constructs an order with a description
// @param desc The description of the order
Order::Order(std::string desc) : description(desc) {}

// @brief Virtual destructor for the Order class
Order::~Order() {}

// @brief Gets the effect string after order execution
// @return const std::string& Description of what the order did
const std::string& Order::effect() const { return effect_; }

// @brief Gets the order's description
// @return const std::string& The description of this order
const std::string& Order::getDescription() const { return description; }

// @brief Stream output operator for Order objects
// @param os Output stream to write to
// @param order Order to output
// @return std::ostream& The output stream
std::ostream& operator<<(std::ostream& os, const Order& order) {
    os << order.getDescription();
    if (!order.effect().empty()) {
        os << " | effect: " << order.effect();
    }
    return os;
}

// ---------- Deploy ----------

// @brief Default constructor for Deploy order
DeployOrder::DeployOrder() : Order("Deploy") {}

// @brief Constructs a Deploy order with parameters
// @param issuer Player issuing the deploy order
// @param target Territory to deploy armies to
// @param amount Number of armies to deploy
DeployOrder::DeployOrder(Player* issuer, Territory* target, int amount)
    : Order("Deploy"), issuer_(issuer), target_(target), amount_(amount) {}

DeployOrder::DeployOrder(const DeployOrder&) = default;

// @brief Validates if the deploy order is valid
// @return bool True if target is owned by issuer and amount > 0
bool DeployOrder::validate() const {
    if (!issuer_ || !target_ || amount_ <= 0) return false;
    return target_->getOwner() == issuer_;
}

// @brief Executes the deploy order if valid
void DeployOrder::execute() {
    if (!validate()) { effect_ = "Invalid deploy"; return; }
    std::ostringstream ss;
    ss << "Deploy " << amount_ << " to " << target_->getName()
       << " (owner: " << target_->getOwner()->getPlayerName() << ")";
    effect_ = ss.str();
}

// @brief Gets the name of this order type
// @return std::string The order name "Deploy"
std::string DeployOrder::name() const { return "Deploy"; }

// @brief Creates a copy of this order
// @return Order* Pointer to a new copy of this order
Order* DeployOrder::clone() const { return new DeployOrder(*this); }

// ---------- Advance ----------

// @brief Default constructor for Advance order
AdvanceOrder::AdvanceOrder() : Order("Advance") {}

// @brief Constructs an Advance order with parameters
// @param issuer Player issuing the advance order
// @param source Territory armies are moving from
// @param target Territory armies are moving to
// @param amount Number of armies to move
AdvanceOrder::AdvanceOrder(Player* issuer, Territory* source, Territory* target, int amount)
    : Order("Advance"), issuer_(issuer), source_(source), target_(target), amount_(amount) {}

AdvanceOrder::AdvanceOrder(const AdvanceOrder&) = default;

// @brief Validates if the advance order is valid
// @return bool True if source is owned by issuer, territories are adjacent, and amount > 0
bool AdvanceOrder::validate() const {
    if (!issuer_ || !source_ || !target_ || amount_ <= 0) return false;
    if (source_->getOwner() != issuer_) return false;
    return source_->isAdjacentTo(target_);
}

// @brief Executes the advance order if valid
void AdvanceOrder::execute() {
    if (!validate()) { effect_ = "Invalid advance"; return; }
    std::ostringstream ss;
    ss << "Advance " << amount_ << " from " << source_->getName()
       << " to " << target_->getName();
    effect_ = ss.str();
}

// @brief Gets the name of this order type
// @return std::string The order name "Advance"
std::string AdvanceOrder::name() const { return "Advance"; }

// @brief Creates a copy of this order
// @return Order* Pointer to a new copy of this order
Order* AdvanceOrder::clone() const { return new AdvanceOrder(*this); }

// ---------- Bomb ----------

// @brief Default constructor for Bomb order
BombOrder::BombOrder() : Order("Bomb") {}

// @brief Constructs a Bomb order with parameters
// @param issuer Player issuing the bomb order
// @param target Territory to bomb
BombOrder::BombOrder(Player* issuer, Territory* target)
    : Order("Bomb"), issuer_(issuer), target_(target) {}

BombOrder::BombOrder(const BombOrder&) = default;

// @brief Validates if the bomb order is valid
// @return bool True if target is not owned by issuer and exists
bool BombOrder::validate() const {
    if (!issuer_ || !target_) return false;
    return target_->getOwner() != issuer_;
}

// @brief Executes the bomb order if valid
void BombOrder::execute() {
    if (!validate()) { effect_ = "Invalid bomb"; return; }
    std::ostringstream ss;
    ss << "Bomb " << target_->getName() << " (owner: "
       << (target_->getOwner() ? target_->getOwner()->getPlayerName() : "none") << ")";
    effect_ = ss.str();
}

// @brief Gets the name of this order type
// @return std::string The order name "Bomb"
std::string BombOrder::name() const { return "Bomb"; }

// @brief Creates a copy of this order
// @return Order* Pointer to a new copy of this order
Order* BombOrder::clone() const { return new BombOrder(*this); }

// ---------- Blockade ----------

// @brief Default constructor for Blockade order
BlockadeOrder::BlockadeOrder() : Order("Blockade") {}

// @brief Constructs a Blockade order with parameters
// @param issuer Player issuing the blockade order
// @param target Territory to blockade
BlockadeOrder::BlockadeOrder(Player* issuer, Territory* target)
    : Order("Blockade"), issuer_(issuer), target_(target) {}

BlockadeOrder::BlockadeOrder(const BlockadeOrder&) = default;

// @brief Validates if the blockade order is valid
// @return bool True if target is owned by issuer and exists
bool BlockadeOrder::validate() const {
    if (!issuer_ || !target_) return false;
    return target_->getOwner() == issuer_;
}

// @brief Executes the blockade order if valid
void BlockadeOrder::execute() {
    if (!validate()) { effect_ = "Invalid blockade"; return; }
    std::ostringstream ss;
    ss << "Blockade on " << target_->getName();
    effect_ = ss.str();
}

// @brief Gets the name of this order type
// @return std::string The order name "Blockade"
std::string BlockadeOrder::name() const { return "Blockade"; }

// @brief Creates a copy of this order
// @return Order* Pointer to a new copy of this order
Order* BlockadeOrder::clone() const { return new BlockadeOrder(*this); }

// ---------- Airlift ----------

// @brief Default constructor for Airlift order
AirliftOrder::AirliftOrder() : Order("Airlift") {}

// @brief Constructs an Airlift order with parameters
// @param issuer Player issuing the airlift order
// @param source Territory to airlift armies from
// @param target Territory to airlift armies to
// @param amount Number of armies to airlift
AirliftOrder::AirliftOrder(Player* issuer, Territory* source, Territory* target, int amount)
    : Order("Airlift"), issuer_(issuer), source_(source), target_(target), amount_(amount) {}

AirliftOrder::AirliftOrder(const AirliftOrder&) = default;

// @brief Validates if the airlift order is valid
// @return bool True if source is owned by issuer and amount > 0
bool AirliftOrder::validate() const {
    if (!issuer_ || !source_ || !target_ || amount_ <= 0) return false;
    return source_->getOwner() == issuer_;
}

// @brief Executes the airlift order if valid
void AirliftOrder::execute() {
    if (!validate()) { effect_ = "Invalid airlift"; return; }
    std::ostringstream ss;
    ss << "Airlift " << amount_ << " from " << source_->getName()
       << " to " << target_->getName();
    effect_ = ss.str();
}

// @brief Gets the name of this order type
// @return std::string The order name "Airlift"
std::string AirliftOrder::name() const { return "Airlift"; }

// @brief Creates a copy of this order
// @return Order* Pointer to a new copy of this order
Order* AirliftOrder::clone() const { return new AirliftOrder(*this); }

// ---------- Negotiate ----------

// ---- NegotiateOrder ----

// @brief Default constructor for Negotiate order
NegotiateOrder::NegotiateOrder() : Order("Negotiate") {}

// @brief Constructs a Negotiate order with parameters
// @param issuer Player issuing the negotiate order
// @param other Player to negotiate with
NegotiateOrder::NegotiateOrder(Player* issuer, Player* other)
    : Order("Negotiate"), issuer_(issuer), other_(other) {}

NegotiateOrder::NegotiateOrder(const NegotiateOrder&) = default;

// @brief Validates if the negotiate order is valid
// @return bool True if both players exist and are different
bool NegotiateOrder::validate() const {
    if (!issuer_ || !other_) return false;
    return issuer_ != other_;
}

// @brief Executes the negotiate order if valid
void NegotiateOrder::execute() {
    if (!validate()) { effect_ = "Invalid negotiate"; return; }
    std::ostringstream ss;
    ss << "Negotiate truce between " << issuer_->getPlayerName()
       << " and " << other_->getPlayerName();
    effect_ = ss.str();
}

// @brief Gets the name of this order type
// @return std::string The order name "Negotiate"
std::string NegotiateOrder::name() const { return "Negotiate"; }

// @brief Creates a copy of this order
// @return Order* Pointer to a new copy of this order
Order* NegotiateOrder::clone() const { return new NegotiateOrder(*this); }

// ----- OrdersList -----

// @brief Destructor that cleans up all orders in the list
OrdersList::~OrdersList() {
    for (Order* order : orders) delete order;
    orders.clear();
}

// @brief Copy constructor performing deep copy of all orders
// @param other OrdersList to copy from
OrdersList::OrdersList(const OrdersList& other) {
    orders.reserve(other.orders.size());
    for (Order* order : other.orders) {
        orders.push_back(order ? order->clone() : nullptr);
    }
}

// @brief Assignment operator performing deep copy
// @param other OrdersList to copy from
// @return OrdersList& Reference to this list after assignment
OrdersList& OrdersList::operator=(const OrdersList& other) {
    if (this == &other) return *this;
    std::vector<Order*> tmp;
    tmp.reserve(other.orders.size());
    for (Order* order : other.orders) {
        tmp.push_back(order ? order->clone() : nullptr);
    }
    for (Order* order : orders) delete order;
    orders.swap(tmp);
    return *this;
}

// @brief Move constructor
// @param other OrdersList to move from
OrdersList::OrdersList(OrdersList&& other) noexcept
    : orders(std::move(other.orders)) {
    other.orders.clear();
}

// @brief Move assignment operator
// @param other OrdersList to move from
// @return OrdersList& Reference to this list after move
OrdersList& OrdersList::operator=(OrdersList&& other) noexcept {
    if (this != &other) {
        for (Order* order : orders) delete order;
        orders = std::move(other.orders);
        other.orders.clear();
    }
    return *this;
}

// @brief Adds an order to the end of the list
// @param order Order to add to the list
void OrdersList::add(Order* order) {
    if (!order) return;
    orders.push_back(order);
}

// @brief Removes and deletes an order at the specified index
// @param index Index of the order to remove
void OrdersList::remove(int index) {
    if (index < 0 || index >= static_cast<int>(orders.size())) return;
    delete orders[static_cast<size_t>(index)];
    orders.erase(orders.begin() + index);
}

// @brief Moves an order from one position to another
// @param from Source index of the order
// @param to Destination index for the order
void OrdersList::move(int from, int to) {
    if (from < 0 || to < 0) return;
    if (from >= static_cast<int>(orders.size()) || to >= static_cast<int>(orders.size())) return;
    if (from == to) return;
    Order* tmp = orders[static_cast<size_t>(from)];
    orders.erase(orders.begin() + from);
    orders.insert(orders.begin() + to, tmp);
}

// --- debug/printing ---

// @brief Prints the orders list to standard output
void OrdersList::print() const {
    std::cout << "[OrdersList: " << orders.size() << " items]\n";
    for (std::size_t index = 0; index < orders.size(); ++index) {
        std::cout << "  " << index << ") " << *orders[index] << "\n";
    }
}

// @brief Stream output operator for OrdersList
// @param os Output stream to write to
// @param ordersList OrdersList to output
// @return std::ostream& Reference to the output stream
std::ostream& operator<<(std::ostream& os, const OrdersList& ordersList) {
    os << "[OrdersList: " << ordersList.orders.size() << " items]\n";
    for (std::size_t index = 0; index < ordersList.orders.size(); ++index) {
        os << "  " << index << ") " << *ordersList.orders[index] << "\n";
    }
    return os;
}
