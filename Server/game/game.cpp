#include "game.h"

namespace SudoUno {
    namespace game {

        void Game::dealCards(Player p) {
            // TODO: implement me!

            //Deal 7 cards to each player
            for (int i = 0; i < 7; i++) {
                //Gets a card from the deck and adds it to the player's hand
                p.addCard(pullCard());
            }
        }

        card::Card Game::pullCard() {
            // TODO: implement me!

            //Pull a card from the deck
            std::vector<card::Card> deck;

            //Get card from deck at random
            int cardIndex = rand() % deck.size();
            card::Card c = deck[cardIndex];
            //Remove from deck
            deck.erase(deck.begin() + cardIndex);
            //Return card
            return c;
        }

        void Game::Start() {
            // TODO: implement me!
        }

        void Game::End(string reason) {
            // TODO: implement me!
        }

        void Game::TakeTurn(Player p, card::Card c, bool saidSudo, bool pickUp) {
            // TODO: implement me!
        }
    }
}