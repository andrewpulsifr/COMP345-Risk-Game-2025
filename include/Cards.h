#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <cstdlib>
//using namespace std;

// Three classes are implemented in Cards.
class Card;
class Deck;
class Hand;

// Deck class holds a collection of cards.
class Deck {
    public:
        Deck();
        Deck(Deck const &deck); // Copy constructor for Deck.
        void addCard(Card* card);
        void removeCard(Card* card);
        std::vector<Card*> getCardsOnDeck();
        std::string draw(Hand &hand);
        void showDeck(Deck &deck);
        ~Deck(); // Destructor for Card*.
    private:
        std::vector<Card*> cardsOnDeck;
    
};


class Card {
    public:
        enum typeOfCard { // The different types a card can be.
            Bomb,
            Reinforcement,
            Blockade,
            Airlift,
            Diplomacy
        };

        Card(typeOfCard cardType); // Constructor.
        Card(Card const & card); // Copy constructor for Card.
        std::string cardToString(typeOfCard cardType); //typeOfCard returns a number value, so we have to convert to string.
        typeOfCard getCard();
        void play(Card* cardPlayed, Deck &deck, Hand &hand); // Cards can be played, but they will be removed from hand and returned to deck afterwards.

    private:
        typeOfCard card;
};


// Hand class holds cards that have been drawn from the deck.
class Hand {
    public:
        Hand();
        Hand(Hand const &hand); // Copy constructor for Hand.
        std::vector<Card*> getCardsOnHand();
        void addCard(Card* card);
        void removeCard(Card* card);
        void showHand(Hand& hand);
        ~Hand(); // Destructor.

    private:
        std::vector<Card*> cardsOnHand;
};


// Stream overloaders for each class: Cards, Hand, and Deck.
std::ostream & operator << (std::ostream &os, const Card &card); // stream overloading for Cards.
std::ostream & operator << (std::ostream &os, const Hand &hand); // stream overloading for Hand.
std::ostream & operator << (std::ostream &os, const Deck &deck); // stream overloading for Deck.
