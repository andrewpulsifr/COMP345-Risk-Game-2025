/**
 * @file Orders.cpp
 * @brief Assignment 1 – Part 3 (Warzone): Orders & OrdersList implementation
 *
 * @details
 * Implements the Orders API declared in Orders.h:
 *  - Concrete orders’ constructors, validate(), execute(), name(), clone()
 *  - OrdersList Rule of 5 and mutators (add/remove/move/print)
 *  - Stream output operator to display description and effect text
 *
 * Implementation notes:
 *  - AdvanceOrder::validate() defers adjacency to Map API
 *  - execute() methods set effect strings
 */

#pragma once
#include <string>
#include <vector>
#include <iosfwd>
#include "LoggingObserver.h"

class Player;
class Territory;

// ======================= Base Order =======================
class Order : public ILoggable , public Subject {
protected:
    std::string description;
    std::string effect_;

    // Ensure no implicit conversions from string to Order
    explicit Order(std::string desc);

    // Delete copy/assignment at Order level since Subject can't be copied
    Order(const Order&) = delete;
    Order& operator=(const Order&) = delete;

public:
    virtual ~Order();
    virtual Order* clone() const = 0; //For copying orderslists mostly

    virtual bool validate() const = 0;
    virtual void execute() = 0;
    virtual std::string name() const = 0;

    // ILoggable interface implementation
    std::string stringToLog() const override;

    const std::string& effect() const;
    const std::string& getDescription() const;

    friend std::ostream& operator<<(std::ostream& os, const Order& order);
};

// ======================= Concrete Orders =======================

class DeployOrder : public Order {
public:
    DeployOrder(); 
    DeployOrder(Player* issuer, Territory* target, int amount);
    DeployOrder(const DeployOrder&);
    Order* clone() const override;

    bool validate() const override;
    void execute() override;
    std::string name() const override;

private:
    Player* issuer_ = nullptr;
    Territory* target_ = nullptr;
    int amount_ = 0;
};

class AdvanceOrder : public Order {
public:
    AdvanceOrder();
    AdvanceOrder(Player* issuer, Territory* source, Territory* target, int amount);
    AdvanceOrder(const AdvanceOrder&);    
    Order* clone() const override;

    bool validate() const override;
    void execute() override;
    std::string name() const override;

private:
    Player* issuer_ = nullptr;
    Territory* source_ = nullptr;
    Territory* target_ = nullptr;
    int amount_ = 0;
};

class BombOrder : public Order {
public:
    BombOrder();
    BombOrder(Player* issuer, Territory* target);
    BombOrder(const BombOrder&);    
    Order* clone() const override;

    bool validate() const override;
    void execute() override;
    std::string name() const override;

private:
    Player* issuer_ = nullptr;
    Territory* target_ = nullptr;
};

class BlockadeOrder : public Order {
public:
    BlockadeOrder();
    BlockadeOrder(Player* issuer, Territory* target);
    BlockadeOrder(const BlockadeOrder&);    
    Order* clone() const override;

    bool validate() const override;
    void execute() override;
    std::string name() const override;

private:
    Player* issuer_ = nullptr;
    Territory* target_ = nullptr;
};

class AirliftOrder : public Order {
public:
    AirliftOrder();
    AirliftOrder(Player* issuer, Territory* source, Territory* target, int amount);
    AirliftOrder(const AirliftOrder&);    
    Order* clone() const override;

    bool validate() const override;
    void execute() override;
    std::string name() const override;
private:
    Player* issuer_ = nullptr;
    Territory* source_ = nullptr;
    Territory* target_ = nullptr;
    int amount_ = 0;
};

class NegotiateOrder : public Order {
public:
    NegotiateOrder();
    NegotiateOrder(Player* issuer, Player* other);
    NegotiateOrder(const NegotiateOrder&);    
    Order* clone() const override;

    bool validate() const override;
    void execute() override;
    std::string name() const override;

private:
    Player* issuer_ = nullptr;
    Player* other_ = nullptr;
};

// ======================= OrdersList =======================
class OrdersList : public ILoggable, public Subject {
private:
    std::vector<Order*> orders; 

public:
    OrdersList() = default;
    ~OrdersList();

    OrdersList(const OrdersList& other);             
    OrdersList& operator=(const OrdersList& other);     
    OrdersList(OrdersList&& other) noexcept;            
    OrdersList& operator=(OrdersList&& other) noexcept;  

    void add(Order* o);
    void remove(int index);
    void move(int from, int to);
    void print() const;

    // ILoggable interface implementation
    std::string stringToLog() const override;

    friend std::ostream& operator<<(std::ostream& os, const OrdersList& ol);
};