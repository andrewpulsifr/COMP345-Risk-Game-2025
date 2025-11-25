#pragma once
#include <vector>
#include <string>
#include <iosfwd>
#include <iostream>


class Player;
class Territory;
class Order;

// ======================= Player Strategies =======================

/**
 * @brief Abstract base class for player strategies
 */
class PlayerStrategy {
	
	public:
        PlayerStrategy();
		PlayerStrategy(Player* player);
		virtual ~PlayerStrategy();
        virtual PlayerStrategy* clone() const = 0;
		// Called at the start of each issuing-phase to allow strategies
		// to reset per-round state (e.g., Cheater acts only once per round).
		virtual void resetForNewRound() {}
		
		virtual bool issueOrder() = 0;
        virtual bool issueOrder(Order* orderIssued) = 0;
		virtual std::vector<Territory*> toAttack() = 0;
		virtual std::vector<Territory*> toDefend() = 0;

		//Rule of three: virtual destructor, copy constructor, copy assignment operator
		PlayerStrategy(const PlayerStrategy& other);
		PlayerStrategy& operator=(const PlayerStrategy& other);
		friend std::ostream& operator<<(std::ostream& os, const PlayerStrategy& ps);

        Player* getPlayer() const;
        void setPlayer(Player* player);

	protected: 
		Player * player_;
};

/**
 * @brief Human player strategy implementation
 * Allows user input to decide actions
 */
class HumanPlayerStrategy : public PlayerStrategy {
    
	public:
		HumanPlayerStrategy();
		~HumanPlayerStrategy() override;
		PlayerStrategy* clone() const override;
		
		bool issueOrder() override;
		bool issueOrder(Order* orderIssued) override;
		std::vector<Territory*> toAttack() override;
		std::vector<Territory*> toDefend() override;

		HumanPlayerStrategy(const HumanPlayerStrategy& other);
		HumanPlayerStrategy& operator=(const HumanPlayerStrategy& other); 
        friend std::ostream& operator<<(std::ostream& os, const HumanPlayerStrategy& ps);
};

/**
 * @brief Aggressive player strategy implementation
 * Focuses on attacking the strongest territories.
 */
class AggressivePlayerStrategy : public PlayerStrategy {
	
	public:
		AggressivePlayerStrategy();
		~AggressivePlayerStrategy() override;
		PlayerStrategy* clone() const override;

		bool issueOrder() override;
		bool issueOrder(Order* orderIssued) override;
		std::vector<Territory*> toAttack() override;
		std::vector<Territory*> toDefend() override;

        AggressivePlayerStrategy(const AggressivePlayerStrategy& other);
        AggressivePlayerStrategy& operator=(const AggressivePlayerStrategy& other); 
        friend std::ostream& operator<<(std::ostream& os, const AggressivePlayerStrategy& ps);

};

/**
 * @brief Benevolent player strategy implementation
 * Focuses on defending its own territories.
 * Does not issue attack orders.
 */
class BenevolentPlayerStrategy : public PlayerStrategy {
	
	public:
		BenevolentPlayerStrategy();
		~BenevolentPlayerStrategy() override;
		PlayerStrategy* clone() const override;

		bool issueOrder() override;
		bool issueOrder(Order* orderIssued) override;
		std::vector<Territory*> toAttack() override;
		std::vector<Territory*> toDefend() override;

        BenevolentPlayerStrategy(const BenevolentPlayerStrategy& other);
        BenevolentPlayerStrategy& operator=(const BenevolentPlayerStrategy& other); 
        friend std::ostream& operator<<(std::ostream& os, const BenevolentPlayerStrategy& ps);

		// Reset per-issuing-phase state (used to control one-time prints)
		void resetForNewRound() override;

	    private:
	        bool handShownThisRound_ = false;
};


/**
 * @brief Neutral player strategy implementation
 * Does not issue any orders or attack any territories
 * If attacked, becomes aggressive
 */
class NeutralPlayerStrategy : public PlayerStrategy {
	
	public:
		NeutralPlayerStrategy();
		~NeutralPlayerStrategy() override;
		PlayerStrategy* clone() const override;

		bool issueOrder() override;
		bool issueOrder(Order* orderIssued) override;
		std::vector<Territory*> toAttack() override;
		std::vector<Territory*> toDefend() override;

        NeutralPlayerStrategy(const NeutralPlayerStrategy& other);
        NeutralPlayerStrategy& operator=(const NeutralPlayerStrategy& other); 
        friend std::ostream& operator<<(std::ostream& os, const NeutralPlayerStrategy& ps);
};

/**
 * @brief Cheater player strategy implementation
 * Automatically conquers adjacent territories without combat
 */
class CheaterPlayerStrategy : public PlayerStrategy {
	
	public:
		CheaterPlayerStrategy();
		~CheaterPlayerStrategy() override;
		PlayerStrategy* clone() const override;

		bool issueOrder() override;
		bool issueOrder(Order* orderIssued) override;
		std::vector<Territory*> toAttack() override;
		std::vector<Territory*> toDefend() override;

        CheaterPlayerStrategy(const CheaterPlayerStrategy& other);
        CheaterPlayerStrategy& operator=(const CheaterPlayerStrategy& other); 
        friend std::ostream& operator<<(std::ostream& os, const CheaterPlayerStrategy& ps);
		// Reset per-issuing-phase state
		void resetForNewRound() override;

	private:
		// When true, the cheater has already performed its automatic conquest this issuing-phase
		bool actedThisRound_ = false;
};

void testPlayerStrategies();