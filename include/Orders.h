#pragma once
#include <string>
#include <vector>
#include <iosfwd>

class Player;
class Territory;

// ======================= Base Order =======================
class Order {
protected:
    std::string description;
    std::string effect_;

public:
    explicit Order(std::string desc);
    virtual ~Order();


    virtual bool validate() const = 0;
    virtual void execute() = 0;
    virtual std::string name() const = 0;


    const std::string& effect() const { return effect_; }
    const std::string& getDescription() const { return description; }

    friend std::ostream& operator<<(std::ostream& os, const Order& order);
};

// ======================= Concrete Orders =======================

class DeployOrder : public Order {
public:
    DeployOrder(); 
    DeployOrder(Player* issuer, Territory* target, int amount);

    bool validate() const override;
    void execute() override;
    std::string name() const override { return "Deploy"; }

private:
    Player* issuer_ = nullptr;
    Territory* target_ = nullptr;
    int amount_ = 0;
};

class AdvanceOrder : public Order {
public:
    AdvanceOrder();
    AdvanceOrder(Player* issuer, Territory* source, Territory* target, int amount);

    bool validate() const override;
    void execute() override;
    std::string name() const override { return "Advance"; }

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

    bool validate() const override;
    void execute() override;
    std::string name() const override { return "Bomb"; }

private:
    Player* issuer_ = nullptr;
    Territory* target_ = nullptr;
};

class BlockadeOrder : public Order {
public:
    BlockadeOrder();
    BlockadeOrder(Player* issuer, Territory* target);

    bool validate() const override;
    void execute() override;
    std::string name() const override { return "Blockade"; }

private:
    Player* issuer_ = nullptr;
    Territory* target_ = nullptr;
};

class AirliftOrder : public Order {
public:
    AirliftOrder();
    AirliftOrder(Player* issuer, Territory* source, Territory* target, int amount);

    bool validate() const override;
    void execute() override;
    std::string name() const override { return "Airlift"; }

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

    bool validate() const override;
    void execute() override;
    std::string name() const override { return "Negotiate"; }

private:
    Player* issuer_ = nullptr;
    Player* other_ = nullptr;
};

// ======================= OrdersList =======================
class OrdersList {
private:
    std::vector<Order*> orders;

public:
    ~OrdersList();

    OrdersList() = default;
    OrdersList(const OrdersList&) = delete;
    OrdersList& operator=(const OrdersList&) = delete;

    void add(Order* o);          
    void remove(int index);      
    void move(int from, int to); 
    void print() const;          

    friend std::ostream& operator<<(std::ostream& os, const OrdersList& ol);
};