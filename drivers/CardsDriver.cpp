#include "../include/Cards.h"

void testCards() {
    Deck deck;
    Hand hand;

    // Create and load each card type into the deck.
    Card* reinforcement = new Card(Card::Reinforcement);
    Card* bomb = new Card(Card::Bomb);
    Card* blockade = new Card(Card::Blockade);
    Card* airlift = new Card(Card::Airlift);
    Card* diplomacy = new Card(Card::Diplomacy);

    deck.addCard(reinforcement);
    deck.addCard(bomb);
    deck.addCard(blockade);
    deck.addCard(airlift);
    deck.addCard(diplomacy);


    //After adding the cards, the deck has 5 cards.
    std::cout <<"----------------------------" << std::endl;
    deck.showDeck();
    // Hand is empty for now.
    hand.showHand();
    
    // Drawing a card from the deck. Should be removed from deck and added to the hand.
    std::string drawnCard = deck.draw(hand);
    std::cout <<"----------------------------" << std::endl;
    
    // After drawing a card, the deck has 4 cards and hand, 1 card.
    deck.showDeck();
    hand.showHand();

    // Drawing a card again.
    std::string drawnCardAgain = deck.draw(hand);
    std::cout <<"The 2nd Drawn card is: " << drawnCardAgain << std::endl;
    std::cout <<"----------------------------" << std::endl;

    // After drawing a card again, the deck has 3 cards and the hand, 2 cards.
    deck.showDeck();
    hand.showHand();

    // Playing the first card in the hand. After played, the card will be returned to the deck.
    std::cout << "PLAYING..." << std::endl;
    hand.getCardsOnHand().at(0)->play(hand.getCardsOnHand().at(0), deck, hand);
    std::cout <<"----------------------------" << std::endl;
    
    deck.showDeck();
    hand.showHand();

    // Destructor called for Deck and Hand, to delete Card* pointers.
    deck.~Deck();
    hand.~Hand();
}