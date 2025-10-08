/**
 * @file CardsDriver.cpp
 * @brief Test driver for the Cards system functionality
 * @details This driver tests the Card, Hand, and Deck classes to demonstrate:
 *          - Deck creation and card loading
 *          - Card drawing from deck to player hand
 *          - Hand management and display
 *          - Card playing with Order generation
 *          - Proper memory management and Rule of Three compliance
 * @author Andrew Pulsifer
 * @date January 2025
 * @version 1.0
 */

#include "../include/Cards.h"
#include "../include/Player.h"
#include <cassert>

using namespace std;

void testCards() {
    // ======================= Test Setup =======================
    // Initialize deck and player with empty hand
    Deck deck;
    Player playerOne;
    Hand playersHand = *playerOne.getPlayerHand();
    

    // ======================= Deck Population =======================
    // Create one card of each type and add to deck
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

    // ======================= Initial State Display =======================
    // Show deck contents and empty hand state
    cout <<"----------------------------" << endl;
    deck.showDeck(deck);
    // Display player's initial empty hand
    playersHand.showHand(playersHand);

    // Verify initial state: deck has 5 cards, hand has 0 cards
    assert(deck.getCardsOnDeck().size() == 5);
    assert(playersHand.getCardsOnHand().size() == 0);

    // ======================= Card Drawing Testing =======================
    // Test individual card drawing with state verification
    size_t deckBefore = deck.getCardsOnDeck().size();
    size_t handBefore = playersHand.getCardsOnHand().size();
    string drawn = deck.draw(playersHand);

    assert(deck.getCardsOnDeck().size() == deckBefore - 1);
    assert(playersHand.getCardsOnHand().size() == handBefore + 1);

    // Display state after first card draw
    deck.showDeck(deck);
    playersHand.showHand(playersHand);

    // Draw second card to further test drawing mechanism
    deckBefore = deck.getCardsOnDeck().size();
    handBefore = playersHand.getCardsOnHand().size();
    drawn = deck.draw(playersHand);

    assert(deck.getCardsOnDeck().size() == deckBefore - 1);
    assert(playersHand.getCardsOnHand().size() == handBefore + 1);

    // Display state after second card draw
    deck.showDeck(deck);
    playersHand.showHand(playersHand);

    // ======================= Hand Population =======================
    // Fill the hand with remaining cards to test complete card playing
    cout << "Drawing remaining cards to fill the hand." << endl;
    while (deck.getCardsOnDeck().size() > 0 && playersHand.getCardsOnHand().size() < 5) {
        deck.draw(playersHand);
    }

    // Display full hand state before card playing demonstration
    cout << "Hand is now full. Showing final state before playing:" << endl;
    deck.showDeck(deck);
    playersHand.showHand(playersHand);

    // ======================= Card Playing Testing =======================
    // Test playing all cards in hand with Order generation
    cout << "----- Playing all cards in hand -----" << endl;
    deckBefore = deck.getCardsOnDeck().size();
    size_t initialHandSize = playersHand.getCardsOnHand().size();
    
    // Play all cards in hand using loop-based approach
    while (!playersHand.getCardsOnHand().empty()) {
        Card* cardToPlay = playersHand.getCardsOnHand().back();
        cout << "Playing card from hand (remaining: " << playersHand.getCardsOnHand().size() << ")..." << endl;
        cardToPlay->play(playerOne, deck, playersHand);
        
        // Display state after each card play to show progression
        deck.showDeck(deck);
        playersHand.showHand(playersHand);
        cout << "----------------------------" << endl;
    }

    // ======================= Test Verification =======================
    // Verify proper card playing: all cards returned to deck, hand emptied
    assert(playersHand.getCardsOnHand().empty());
    assert(deck.getCardsOnDeck().size() == deckBefore + initialHandSize);
    
    cout << "TEST PASSED: All " << initialHandSize << " cards were played and returned to deck." << endl;    // Ending message.
    cout << "=== End Cards Test ===" << endl;
}
