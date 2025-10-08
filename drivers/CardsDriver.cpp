#include "../include/Cards.h"
#include "../include/Player.h"
#include <cassert>

void testCards() {
    Deck deck;

    // A player is declared, which has an object Hand.
    Player playerOne;
    Hand playersHand = *playerOne.getPlayerHand();
    

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
    // The Player's Hand is empty for now.
    playersHand.showHand(playersHand);

    assert(deck.getCardsOnDeck().size() == 5);
    assert(playersHand.getCardsOnHand().size() == 0);



    // Drawing Card #1.
    int deckBefore = deck.getCardsOnDeck().size();
    int handBefore = playersHand.getCardsOnHand().size();
    std::string drawn = deck.draw(playersHand);

    assert(deck.getCardsOnDeck().size() == deckBefore - 1);
    assert(playersHand.getCardsOnHand().size() == handBefore + 1);

    // After drawing Card #1, the Deck has 4 cards and the Player's Hand , 1 card.
    deck.showDeck(deck);
    playersHand.showHand(playersHand);


    // Drawing Card #2.
    deckBefore = deck.getCardsOnDeck().size();
    handBefore = playersHand.getCardsOnHand().size();
    drawn = deck.draw(playersHand);

    assert(deck.getCardsOnDeck().size() == deckBefore - 1);
    assert(playersHand.getCardsOnHand().size() == handBefore + 1);

    // After drawing Card #2, the Deck has 3 cards and the Player's Hand , 2 cards.
    deck.showDeck(deck);
    playersHand.showHand(playersHand);


    // Playing the Cards on the Player's Hand .
    std::cout << "PLAYING..." << std::endl;
    deckBefore = deck.getCardsOnDeck().size();
    handBefore = playersHand.getCardsOnHand().size();

    // Play the first Card.
    Card *cardPlayed = playersHand.getCardsOnHand().at(0);
    cardPlayed->play(playerOne, deck, playersHand);

    assert(deck.getCardsOnDeck().size() == deckBefore + 1);
    assert(playersHand.getCardsOnHand().size() == handBefore - 1);
    std::cout <<"----------------------------" << std::endl;

    // The Card is removed from the Player's Hand , and returned back to the Deck.
    deck.showDeck(deck);
    playersHand.showHand(playersHand);
    
    
    
    // Play another Card, where Hand should have no Cards.
    
    // Play the last Card.
    cardPlayed = playersHand.getCardsOnHand().back();
    cardPlayed->play(playerOne, deck, playersHand);

    assert(playersHand.getCardsOnHand().empty());

    // Ending message.
    std::cout << "=== End Cards Test ===" << std::endl;
}
