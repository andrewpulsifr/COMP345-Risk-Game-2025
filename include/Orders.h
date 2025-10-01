#pragma once
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Base class representing a Warzone Order.
 * 
 * Each order must implement validate() and execute().
 * For now, these are the skeleton classes for all of the orders (TODO: flesh out logic in later parts).
 */
class Order {
protected:
    std::string description; // Short text label for debugging
public:
    Order(std::string desc);
    virtual ~Order();

    // ====== Core API ======
    virtual bool validate() const = 0;   // TODO: add real validation rules
    virtual void execute() = 0;          // TODO: implement execution behavior

    // For printing order details
    friend std::ostream& operator<<(std::ostream& os, const Order& order);
};

// ====== Derived Orders ======
// NOTE: All validate() and execute() are stubs for now.
class DeployOrder : public Order {
public:
    DeployOrder();
    bool validate() const override;  // TODO
    void execute() override;         // TODO
};

class AdvanceOrder : public Order {
public:
    AdvanceOrder();
    bool validate() const override;  // TODO
    void execute() override;         // TODO
};

class BombOrder : public Order {
public:
    BombOrder();
    bool validate() const override;  // TODO
    void execute() override;         // TODO
};

class BlockadeOrder : public Order {
public:
    BlockadeOrder();
    bool validate() const override;  // TODO
    void execute() override;         // TODO
};

class AirliftOrder : public Order {
public:
    AirliftOrder();
    bool validate() const override;  // TODO
    void execute() override;         // TODO
};

class NegotiateOrder : public Order {
public:
    NegotiateOrder();
    bool validate() const override;  // TODO
    void execute() override;         // TODO
};

/**
 * @brief Container for maintaining a list of Orders.
 * 
 * Has logic for adding, removing, and reordering (move).
 */
class OrdersList {
private:
    std::vector<Order*> orders; // Owning raw pointers for now
public:
    ~OrdersList();

    void add(Order* o);             // Add new order
    void remove(int index);         // Remove at index
    void move(int from, int to);    // Reorder within the list

    void print() const;             // Debug print all orders
};