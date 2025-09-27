#include "../include/Cards.h"

void testCards() {
    // Can move the code from main() to here, but works on execution when in main().
}

int main() {
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
    deck.showDeck();
    // Shuffling the deck will change the order of the cards.
    deck.shuffleCards();
    std::cout <<"----------------------------" << std::endl;
    deck.showDeck();
    // Hand is empty for now.
    hand.showHand();
    
    // Drawing a card from the deck. Should be removed from deck and added to the hand.
    std::string drawnCard = deck.draw(hand);
    std::cout <<"----------------------------" << std::endl;
    std::cout <<"The 1st Drawn card is: " << drawnCard << std::endl;
    std::cout <<"----------------------------" << std::endl;
    deck.showDeck();
    hand.showHand();

    // Drawing a card again.
    std::string drawnCardAgain = deck.draw(hand);
    std::cout <<"The 2nd Drawn card is: " << drawnCardAgain << std::endl;
    std::cout <<"----------------------------" << std::endl;
    deck.showDeck();
    hand.showHand();

    // Playing the first card in the hand. After played, the card will be returned to the deck.
    std::cout << "PLAYING..." << std::endl;
    hand.getCardsOnHand().at(0)->play(hand.getCardsOnHand().at(0), deck, hand);
    std::cout <<"----------------------------" << std::endl;
    
    deck.showDeck();
    hand.showHand();
    
    return 0;
}
