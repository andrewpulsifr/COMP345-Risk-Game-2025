#include "../include/Cards.h"
#include "../include/Player.h"
#include <cassert>

using namespace std;

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
    cout <<"----------------------------" << endl;
    deck.showDeck(deck);
    // The Player's Hand is empty for now.
    playersHand.showHand(playersHand);

    assert(deck.getCardsOnDeck().size() == 5);
    assert(playersHand.getCardsOnHand().size() == 0);

    // Drawing Card #1.
    size_t deckBefore = deck.getCardsOnDeck().size();
    size_t handBefore = playersHand.getCardsOnHand().size();
    string drawn = deck.draw(playersHand);

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

    // After drawing Card #2, the Deck has 3 cards and the Player's Hand has 2 cards.
    deck.showDeck(deck);
    playersHand.showHand(playersHand);

    // Fill the hand with more cards to demonstrate playing all cards
    cout << "Drawing 5 cards to fill the hand." << endl;
    while (deck.getCardsOnDeck().size() > 0 && playersHand.getCardsOnHand().size() < 5) {
        deck.draw(playersHand);
    }
    
    cout << "Hand is now full. Showing final state before playing:" << endl;
    deck.showDeck(deck);
    playersHand.showHand(playersHand);

    cout << "----- Playing all cards in hand ----_" << endl;
    deckBefore = deck.getCardsOnDeck().size();
    size_t initialHandSize = playersHand.getCardsOnHand().size();
    
    // Play all cards in hand
    while (!playersHand.getCardsOnHand().empty()) {
        Card* cardToPlay = playersHand.getCardsOnHand().back();
        cout << "Playing card from hand (remaining: " << playersHand.getCardsOnHand().size() << ")..." << endl;
        cardToPlay->play(playerOne, deck, playersHand);
        
        // Show state after each card is played
        deck.showDeck(deck);
        playersHand.showHand(playersHand);
        cout << "----------------------------" << endl;
    }

    // Verify all cards returned to deck and hand is empty
    assert(playersHand.getCardsOnHand().empty());
    assert(deck.getCardsOnDeck().size() == deckBefore + initialHandSize);
    
    cout << "TEST PASSED: All " << initialHandSize << " cards were played and returned to deck." << endl;

    // Ending message.
    cout << "=== End Cards Test ===" << endl;
}
