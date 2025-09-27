#include "../include/Cards.h"



//Implementation of Card.
// Constructor for Card
Card::Card(typeOfCard cardType) : card(cardType) {} 

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
    }
}

//Returns the type of card.
Card::typeOfCard Card::getCard() { 
    return card;
};

//Each card can be played and will be removed from hand and returned to deck after execution
void Card::play(Card* cardPlayed, Deck &deck, Hand &hand) {

    switch(cardPlayed->getCard()) {
        case Bomb:
            std::cout << "Bomb card played." << std::endl;
            //Implement logic
            break;
        case Reinforcement:
            std::cout << "Reinforcement card played." << std::endl;
            //Implement logic
            break;
        case Blockade:
            std::cout << "Blockade card played." << std::endl;
            //Implement logic
            break;
        case Airlift:
            std::cout << "Airlift card played." << std::endl;
            //Implement logic
            break;
        case Diplomacy:
            std::cout << "Diplomacy card played." << std::endl;
            //Implement logic
            break;
    };

    //After orders are executed, remove the card from Hand and place back into the Deck.
    hand.removeCard(cardPlayed);
    deck.addCard(cardPlayed);
}


//Implementation of Hand.
Hand::Hand() : cardsOnHand() {}

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
    std::cout << "The hand includes the cards:" << std::endl;
    if(cardsOnHand.size() == 0) {
        std::cout << "There are no cards on hand." << std::endl;
    } else {
        int index = 0;
        for(Card* card : cardsOnHand) {
            std::cout << index << " " << cardToString(card->getCard()) << std::endl;
            index++;
        }
    }
    std::cout << "----------------------------" << std::endl;
}



// Imeplementation of Deck.
Deck::Deck() : cardsOnDeck() {}

// To add a card to the Deck.
void Deck::addCard(Card* card) {
    cardsOnDeck.push_back(card);
}

//To remove a card from the Deck.
void Deck::removeCard(Card* card) {
    cardsOnDeck.erase(std::remove(cardsOnDeck.begin(), cardsOnDeck.end(), card), cardsOnDeck.end());
}

// To shuffle the cards in a Deck in a random order.
void Deck::shuffleCards() {
    // Generating a random number.
    std::random_device randomNum;
    std::mt19937 g(randomNum());

    // Shuffle the deck.
    shuffle(cardsOnDeck.begin(), cardsOnDeck.end(),g);
    std::cout << "Deck is shuffled." << std::endl;
}

// Returns the collection of cards in the Deck.
std::vector<Card*> Deck::getCardsOnDeck() {
    return cardsOnDeck;
}

// To Draw a card from the Deck and place it in Hand.
std::string Deck::draw(Hand &hand) {
    Card* cardDrawn = nullptr;
    
    if(cardsOnDeck.size() > 0) {
        cardDrawn = cardsOnDeck.back();
        hand.addCard(cardDrawn);
        cardsOnDeck.pop_back();
        std::cout << "1 card is drawn from the deck." << std::endl;
    } else {
        std::cout << "The deck is empty." << std::endl;
    }

    //Shuffle deck after a card is drawn.
    Deck::shuffleCards();

    //If deck is empty, return nullptr, if not, return a card.
    std::string card = cardToString(cardDrawn->getCard());
    return card;
}


// Show and print the cards that are in the Deck.
void Deck::showDeck() {
    std::cout << "The deck includes the cards:" << std::endl;
    if(cardsOnDeck.size() == 0) {
        std::cout << "The deck is empty." << std::endl;
    } else {
         for(Card* card : cardsOnDeck) {
        std::cout << cardToString(card->getCard()) << std::endl;
        }
    }
    
    std::cout << "----------------------------" << std::endl;
}

