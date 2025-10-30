#include "../include/Cards.h"
#include "../include/Player.h"
#include "../include/Orders.h"
#include <algorithm>

//Implementation of Card.
// Constructor for Card
Card::Card(typeOfCard cardType) : card(cardType) {} 

// Deep copy constructor for Card.
Card::Card(Card const &card) : card(card.card) {}

// Assignment operator for Card.
Card& Card::operator=(const Card& other) {
    if (this != &other) {
        card = other.card;
    }
    return *this;
}

// Cards are converted into a string to be displayed when printed.
std::string cardToString(Card::typeOfCard cardType) {
    switch(cardType) {
        case Card::Bomb:
            return "Bomb";
        case Card::Reinforcement:
            return "Reinforcement";
        case Card::Blockade:
            return "Blockade";
        case Card::Airlift:
            return "Airlift";
        case Card::Diplomacy:
            return "Diplomacy";
        default: 
            return "Invalid Card";
    }
}

//Returns the type of card.
Card::typeOfCard Card::getCard() const { 
    return card;
};

//Each card can be played and will be removed from hand and returned to deck after execution
void Card::play(Player& player, Deck &deck, Hand &hand) {
    Order* order = nullptr;

    switch(this->getCard()) {
        case Bomb:
            order = new BombOrder(); // player will be set when added to player's list
            std::cout << "The Bomb card is played.";
            break;
        case Reinforcement:
            order = new AdvanceOrder(); // details will be set when added to player's list
            std::cout << "The Reinforcement card is played.";
            break;
        case Blockade:
            order = new BlockadeOrder(); // target will be set when added to player's list
            std::cout << "The Blockade card is played.";
            break;
        case Airlift:
            order = new AirliftOrder(); // details will be set when added to player's list
            std::cout << "The Airlift card is played.";
            break;
        case Diplomacy:
            order = new NegotiateOrder(); // other player will be set when added to player's list
            std::cout << "The Diplomacy card is played.";
            break;
        default:
            std::cout << "The Card is invalid.";
            return; // Don't process invalid cards
    };

    // Add the created order to the player's orders list
    if (order != nullptr) {
        player.issueOrder(order);
    }

    //After order is created and added to player's list, remove the card from Hand and place back into the Deck.
    hand.removeCard(this);
    deck.addCard(this);
    std::cout << " The card is now returned to the deck." << std::endl;
}

//Implementation of Hand.
Hand::Hand() : cardsOnHand() {} // Default constructor.

// Deep copy constructor for Hand.
Hand::Hand(Hand const &hand) {
    // Deep copy: create new Card objects with same enum value
    for(size_t i = 0; i < hand.cardsOnHand.size(); i++) {
        Card* cardptr = hand.cardsOnHand[i];
        if (cardptr) {
            Card* copy = new Card(cardptr->getCard());
            this->cardsOnHand.push_back(copy);
        }
    }
}

// Assignment operator for Hand.
Hand& Hand::operator=(const Hand& other) {
    if (this != &other) {
        // Delete existing cards to prevent memory leak
        for (Card* card : cardsOnHand) {
            delete card;
        }
        cardsOnHand.clear();
        
        // Deep copy: create new Card objects
        for (size_t i = 0; i < other.cardsOnHand.size(); i++) {
            Card* cardptr = other.cardsOnHand[i];
            if (cardptr) {
                Card* copy = new Card(cardptr->getCard());
                this->cardsOnHand.push_back(copy);
            }
        }
    }
    return *this;
}

// Stream overloading for Card.
std::ostream & operator<<(std::ostream &os, const Card &card) {
    os << cardToString(card.getCard());
    return os;
}

// To get the private vector of cards on Hand.
std::vector<Card*> Hand::getCardsOnHand() const {
    return cardsOnHand;
}

// Stream overloading for Hand.
std::ostream& operator<<(std::ostream &os, const Hand &hand) {
    os << "There are " << hand.getCardsOnHand().size() << " Cards on Hand:" << std::endl;
    for(size_t i = 0; i < hand.getCardsOnHand().size(); i++) {
        os << "  Index " << i << ": " << cardToString(hand.getCardsOnHand().at(i)->getCard()) << std::endl;
    }
    return os;
}

// To add a card to Hand.
void Hand::addCard(Card* card) {
    cardsOnHand.push_back(card);
}

// To remove a card from Hand.
void Hand::removeCard(Card* card) {
    cardsOnHand.erase(std::remove(cardsOnHand.begin(), cardsOnHand.end(), card), cardsOnHand.end());
}

// Show what cards are in and.
void Hand::showHand() {
    if(cardsOnHand.size() == 0) {
        std::cout << "There are no Cards on the Player's Hand." << std::endl;
    } else {
        std::cout << *this; // print out Hand cards.
    }
    std::cout << "----------------------------" << std::endl;
}

// Destructor for Hand and deletes the Card* of each card in Hand.
Hand::~Hand() {
    for(size_t i = 0; i < cardsOnHand.size(); i++) {
        delete cardsOnHand.at(i);
    }

    cardsOnHand.clear();
}

// Implementation of Deck.
Deck::Deck() : cardsOnDeck() {}

// Deep copy constructor for Deck.
Deck::Deck(Deck const &deck){
    for(size_t i = 0; i < deck.cardsOnDeck.size(); i++) {
        Card* cardptr = deck.cardsOnDeck.at(i);
        if (cardptr) {
            Card* cardCopied = new Card(cardptr->getCard());
            this->cardsOnDeck.push_back(cardCopied);
        }
    }
}

// Assignment operator for Deck.
Deck& Deck::operator=(const Deck& other) {
    if (this != &other) {
        // Delete existing cards to prevent memory leak
        for (Card* card : cardsOnDeck) {
            delete card;
        }
        cardsOnDeck.clear();
        
        // Deep copy: create new Card objects
        for (size_t i = 0; i < other.cardsOnDeck.size(); i++) {
            Card* cardptr = other.cardsOnDeck.at(i);
            if (cardptr) {
                Card* cardCopied = new Card(cardptr->getCard());
                this->cardsOnDeck.push_back(cardCopied);
            }
        }
    }
    return *this;
}

// To add a card to the Deck.
void Deck::addCard(Card* card) {
    cardsOnDeck.push_back(card);

}

//To remove a card from the Deck.
void Deck::removeCard(Card* card) {
    cardsOnDeck.erase(std::remove(cardsOnDeck.begin(), cardsOnDeck.end(), card), cardsOnDeck.end());
}

// Returns the collection of cards in the Deck.
std::vector<Card*> Deck::getCardsOnDeck() const {
    return cardsOnDeck;
}


// Stream overloading for Deck.
std::ostream & operator << (std::ostream &os, const Deck &deck) {
    os << "There are " << deck.getCardsOnDeck().size() << " Cards on the Deck:" << std::endl;
    
    for(size_t i = 0; i < deck.getCardsOnDeck().size(); i++) {
        os << "  " << cardToString(deck.getCardsOnDeck().at(i)->getCard()) << std::endl;
    }

    return os;
}



// To Draw a card from the Deck and place it in Hand.
std::string Deck::draw(Hand &hand) {
    Card* cardDrawn = nullptr;
    std::string cardDrawnString = "";
    
    if(cardsOnDeck.size() > 0) {

         //Generating a random index and drawing the card from that index.
        srand(time(0));
        int randomIndex = rand() % cardsOnDeck.size();
        cardDrawn = cardsOnDeck.at(randomIndex);

        // Erase card after drawing it.
        cardsOnDeck.erase(cardsOnDeck.begin() + randomIndex);
        cardDrawnString = cardToString(cardDrawn->getCard());
        
        // Add drawn card to hand.
        hand.addCard(cardDrawn);

        std::cout << "The " << cardDrawnString << " Card is drawn from the Deck, and added to the Player's Hand." << std::endl;
    } else {
        std::cout << "The Deck is empty." << std::endl;
       
    }
    
    return cardDrawnString;
}


// Show and print the cards that are in the Deck.
void Deck::showDeck() {
    if(cardsOnDeck.size() == 0) {
        std::cout << "The Deck is empty." << std::endl;
    } else {
        std::cout << *this; // print out Deck cards.
    }
    
    std::cout << "----------------------------" << std::endl;
}

// Destructor for Deck. Deletes the Card* of each card in the Deck.
Deck::~Deck() {

    // Delete the Card objects.
    for(size_t i = 0; i < cardsOnDeck.size(); i++) {
        delete cardsOnDeck.at(i);
    }

    // Remove/Clear the address pointers of all Card objects on Deck.
    cardsOnDeck.clear();
}
