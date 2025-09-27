#pragma once
#include <iostream>
#include <vector>
#include <random>
#include <string>
//using namespace std;

// Three classes are implemented in Cards.
class Card;
class Deck;
class Hand;

// Deck class holds a collection of cards.
class Deck {

    public:
        Deck();
        void addCard(Card* card);
        void removeCard(Card* card);
        void shuffleCards();
        std::vector<Card*> getCardsOnDeck();
        std::string draw(Hand &hand);
        void showDeck();
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

        Card(typeOfCard cardType);
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
        std::vector<Card*> getCardsOnHand();
        void addCard(Card* card);
        void removeCard(Card* card);
        void showHand();

    private:
        std::vector<Card*> cardsOnHand;
};