#include "../include/Cards.h"



// // Stream overloading for Hand.
// std::ostream & operator << (std::ostream &os, std::vector<Card*> cardsOnHand) {
//     for(int i = 0; i < cardsOnHand.size(); i++) {
//         os << "Index " << i << ": " << cardsOnHand[i]->getCard() << std::endl;
//     }
//     return os;
// }

//Implementation of Card.
// Constructor for Card
Card::Card(typeOfCard cardType) : card(cardType) {} 

// Copy constructor for Card.
Card::Card(Card &card) {
    this->card = card.getCard();
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

// Stream overloading for card.
std::ostream & operator << (std::ostream &os, Card::typeOfCard cardType) {
    os << cardToString(cardType);
    return os;
}

//Returns the type of card.
Card::typeOfCard Card::getCard() { 
    return card;
};

//Each card can be played and will be removed from hand and returned to deck after execution
void Card::play(Card* cardPlayed, Deck &deck, Hand &hand) {

    switch(cardPlayed->getCard()) {
        case Bomb:
            std::cout << "The Bomb card played.";
            //Implement logic
            break;
        case Reinforcement:
            std::cout << "The Reinforcement card played.";
            //Implement logic
            break;
        case Blockade:
            std::cout << "The Blockade card played.";
            //Implement logic
            break;
        case Airlift:
            std::cout << "The Airlift card played.";
            //Implement logic
            break;
        case Diplomacy:
            std::cout << "The Diplomacy card played.";
            //Implement logic
            break;
        default:
            std::cout << "The card is invalid.";
    };

    //After orders are executed, remove the card from Hand and place back into the Deck.
    hand.removeCard(cardPlayed);
    deck.addCard(cardPlayed);
    std::cout << " The card is now returned to the deck." << std::endl;
}


//Implementation of Hand.
Hand::Hand() : cardsOnHand() {} // Default constructor.

Hand::Hand(Hand &hand) {
    for(int i = 0; i < hand.cardsOnHand.size(); i++) {
        this->cardsOnHand[i] = hand.getCardsOnHand()[i];
    }
}



// To get the private vector of cards on Hand.
std::vector<Card*> Hand::getCardsOnHand() {
    return cardsOnHand;
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
        std::cout << "There are no cards on Hand." << std::endl;
    } else {
        std::cout << "There are " << cardsOnHand.size() << " cards on Hand:" << std::endl;
        for(int i = 0; i < cardsOnHand.size(); i++) {
            std::cout << "  (Index: " << i << ") " << cardsOnHand[i]->getCard() << std::endl;
        }
    }
    std::cout << "----------------------------" << std::endl;
}

// Destructor for Hand and deletes the Card* of each card in Hand.
Hand::~Hand() {
    for(int i = 0; i < cardsOnHand.size(); i++) {
        delete cardsOnHand[i];
    }
}



// Imeplementation of Deck.
Deck::Deck() : cardsOnDeck() {}

// Copy constructor for Deck.
Deck::Deck(Deck &deck){
    for(int i = 0; i < deck.cardsOnDeck.size(); i++) {
        this->cardsOnDeck[i] = deck.getCardsOnDeck()[i];
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

// To Draw a card from the Deck and place it in Hand.
std::string Deck::draw(Hand &hand) {
    Card* cardDrawn = nullptr;
    std::string cardDrawnString = "";
    
    if(cardsOnDeck.size() > 0) {

         //Generating a random index and drawing the card from that index.
        srand(time(0));
        int randomIndex = rand() % cardsOnDeck.size();
        cardDrawn = cardsOnDeck[randomIndex];

        // Erase card after drawing it.
        cardsOnDeck.erase(cardsOnDeck.begin() + randomIndex);
        cardDrawnString = cardToString(cardDrawn->getCard());
        
        // Add drawn card to hand.
        hand.addCard(cardDrawn);

        std::cout << "The " << cardDrawnString << " card is drawn from the deck, and added to hand." << std::endl;
    } else {
        std::cout << "The deck is empty." << std::endl;
       
    }
    
    return cardString;
}


// Show and print the cards that are in the Deck.
void Deck::showDeck() {
    if(cardsOnDeck.size() == 0) {
        std::cout << "The deck is empty." << std::endl;
    } else {
        std::cout << "There are " << cardsOnDeck.size() << " cards on the Deck:" << std::endl;
        for(int i = 0; i < cardsOnDeck.size(); i++) {
        std::cout << "  " << cardsOnDeck[i]->getCard() << std::endl;
        }
    }
    
    std::cout << "----------------------------" << std::endl;
}

// Destructor for Deck. Deletes the Card* of each card in the Deck.
Deck::~Deck() {
    for(int i = 0; i < cardsOnDeck.size(); i++) {
        delete cardsOnDeck[i];
    }
}

