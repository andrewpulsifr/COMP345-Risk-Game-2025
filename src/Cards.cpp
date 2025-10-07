#include "../include/Cards.h"
#include "../include/Orders.h"

//Implementation of Card.
// Constructor for Card
Card::Card(typeOfCard cardType) : card(cardType) {} 

// Deep copy constructor for Card.
Card::Card(Card const &card) : card(card.card) {}


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
Card::typeOfCard Card::getCard() { 
    return card;
};

//Each card can be played and will be removed from hand and returned to deck after execution
void Card::play(Card* cardPlayed, Deck &deck, Hand &hand) {
    BombOrder* bomb = new BombOrder();
    AdvanceOrder* advance = new AdvanceOrder();
    BlockadeOrder* blockade = new BlockadeOrder();
    AirliftOrder* airlift = new AirliftOrder();
    NegotiateOrder* negotiate = new NegotiateOrder();

    switch(cardPlayed->getCard()) {
        case Bomb:
            bomb->execute();
            
            std::cout << "The Bomb card is played.";
            break;
        case Reinforcement:
            advance->execute();

            std::cout << "The Reinforcement card is played.";
            break;
        case Blockade:
            blockade->execute();

            std::cout << "The Blockade card is played.";
            break;
        case Airlift:
            airlift->execute();

            std::cout << "The Airlift card is played.";
            break;
        case Diplomacy:
            negotiate->execute();

            std::cout << "The Diplomacy card is played.";
            break;
        default:
            std::cout << "The Card is invalid.";
    };

    //After orders are executed, remove the card from Hand and place back into the Deck.
    hand.removeCard(cardPlayed);
    deck.addCard(cardPlayed);
    std::cout << " The Card is now returned to the Deck." << std::endl;
}

//Implementation of Hand.
Hand::Hand() : cardsOnHand() {} // Default constructor.

// Deep copy constructor for Hand.
Hand::Hand(Hand const &hand) {
    // Deep copy: create new Card objects with same enum value
    for(size_t i = 0; i < hand.cardsOnHand.size(); ++i) {
        Card* cardptr = hand.cardsOnHand[i];
        if (cardptr) {
            Card* copy = new Card(cardptr->getCard());
            this->cardsOnHand.push_back(copy);
        }
    }
}

// Stream overloading for card.
std::ostream & operator<<(std::ostream &os, Card::typeOfCard cardType) {
    os << cardToString(cardType);
    return os;
}

// To get the private vector of cards on Hand.
std::vector<Card*> Hand::getCardsOnHand() {
    return cardsOnHand;
}

// Stream overloading for Hand.
std::ostream& operator<<(std::ostream &os, Hand &hand) {
    os << "There are " << hand.getCardsOnHand().size() << " Cards on Hand:" << std::endl;
    for(size_t i = 0; i < hand.getCardsOnHand().size(); i++) {
        os << "  Index " << i << ": " << hand.getCardsOnHand().at(i)->getCard() << std::endl;
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
void Hand::showHand(Hand& hand) {
    if(cardsOnHand.size() == 0) {
        std::cout << "There are no Cards on the Player's Hand." << std::endl;
    } else {
        std::cout << hand; 
    }
    std::cout << "----------------------------" << std::endl;
}

// Destructor for Hand and deletes the Card* of each card in Hand.
Hand::~Hand() {
    for(size_t i = 0; i < cardsOnHand.size(); i++) {
        delete cardsOnHand.at(i);
    }
}

// Imeplementation of Deck.
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

// To add a card to the Deck.
void Deck::addCard(Card* card) {
    cardsOnDeck.push_back(card);

}

//To remove a card from the Deck.
void Deck::removeCard(Card* card) {
    cardsOnDeck.erase(std::remove(cardsOnDeck.begin(), cardsOnDeck.end(), card), cardsOnDeck.end());
}

// Returns the collection of cards in the Deck.
std::vector<Card*> Deck::getCardsOnDeck() {
    return cardsOnDeck;
}


// Stream overloading for Deck.
std::ostream & operator << (std::ostream &os, Deck &deck) {
    os << "There are " << deck.getCardsOnDeck().size() << " Cards on the Deck:" << std::endl;
    
    for(size_t i = 0; i < deck.getCardsOnDeck().size(); i++) {
        os << "  " << deck.getCardsOnDeck().at(i)->getCard() << std::endl;
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
void Deck::showDeck(Deck &deck) {
    if(cardsOnDeck.size() == 0) {
        std::cout << "The Deck is empty." << std::endl;
    } else {
        std::cout << deck; // print out Deck cards.
    }
    
    std::cout << "----------------------------" << std::endl;
}

// Destructor for Deck. Deletes the Card* of each card in the Deck.
Deck::~Deck() {
    for(size_t i = 0; i < cardsOnDeck.size(); i++) {
        delete cardsOnDeck.at(i);
    }
}
