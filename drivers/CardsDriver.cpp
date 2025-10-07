#include "../include/Cards.h"
#include <cassert>

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
    deck.showDeck(deck);
    // Hand is empty for now.
    hand.showHand(hand);

    assert(deck.getCardsOnDeck().size() == 5);
    assert(hand.getCardsOnHand().size() == 0);



    // Drawing Card #1.
    int deckBefore = deck.getCardsOnDeck().size();
    int handBefore = hand.getCardsOnHand().size();
    std::string drawn = deck.draw(hand);

    assert(deck.getCardsOnDeck().size() == deckBefore - 1);
    assert(hand.getCardsOnHand().size() == handBefore + 1);

    // After drawing Card #1, the deck has 4 cards and hand, 1 card.
    deck.showDeck(deck);
    hand.showHand(hand);



    // Drawing Card #2.
    deckBefore = deck.getCardsOnDeck().size();
    handBefore = hand.getCardsOnHand().size();
    drawn = deck.draw(hand);

    assert(deck.getCardsOnDeck().size() == deckBefore - 1);
    assert(hand.getCardsOnHand().size() == handBefore + 1);

    // After drawing Card #2, the deck has 3 cards and the hand, 2 cards.
    deck.showDeck(deck);
    hand.showHand(hand);



    // Playing the Cards on Hand.
    std::cout << "PLAYING..." << std::endl;
    deckBefore = deck.getCardsOnDeck().size();
    handBefore = hand.getCardsOnHand().size();

    // Play the first card.
    Card *cardPlayed = hand.getCardsOnHand().at(0);
    cardPlayed->play(cardPlayed, deck, hand);

    assert(deck.getCardsOnDeck().size() == deckBefore + 1);
    assert(hand.getCardsOnHand().size() == handBefore - 1);
    std::cout <<"----------------------------" << std::endl;

    // The card is removed from Hand, and returned back to the Deck.
    deck.showDeck(deck);
    hand.showHand(hand);

    
    
    
    // Play another card, where Hand should have no cards.
    
    // Play the last card.
    cardPlayed = hand.getCardsOnHand().back();
    cardPlayed->play(cardPlayed, deck, hand);

    assert(hand.getCardsOnHand().empty());

}
