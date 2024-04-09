#include "game.h"
#include <vector>
#include <algorithm>

using namespace std;
namespace SudoUno {
    namespace game {

        namespace card {
            map<CardColor, string> ColorNames = {
                {red, "Red"},
                {green, "Green"},
                {blue, "Blue"},
                {yellow, "Yellow"},
                {black, "Black"}
            };

            map<CardType, string> TypeNames = {
                {zero, "0"},
                {one, "1"},
                {two, "2"},
                {three, "3"},
                {four, "4"},
                {five, "5"},
                {six, "6"},
                {seven, "7"},
                {eight, "8"},
                {nine, "9"},
                {rev, "Rev"},
                {skip, "Skip"},
                {pltwo, "PL2"},
                {wild4, "Wild4"},
                {wild, "Wild"}
            };
        }

        map <GameState, string> GameStateNames = {
            {playing, "Playing"},
            {waiting, "Waiting"},
            {finished, "Finished"}
        };

        Game::Game(Player p, int i) : currentPlayer(p), state(GameState::waiting){
            addPlayer(p);
            index = i;
        }

        void Game::dealCards(Player p) {
            // TODO: implement me!
            for (int j = 0; j < 7; j++){
                //taking cards from the end of the vector and deleting them from the deck the game holds 
                //(I think this is how we want to do things?)
                p.getHand().push_back(cards.back());
                cards.pop_back();
            }
        }

        card::Card Game::pullCard() {
            // TODO: implement me!
        }

        void Game::Start() {
            // -- Create card deck -- //
            //work here
            //make deck
            //make vector to hold all cards
            util::log(index, "Game started!");
            vector<card::Card> decktor = createDeck();
            util::log(index, "Deck created");

            //game has a cards field- assign old vector to this
            cards = decktor;

            //get all players 
            vector<Player> gamePlayers = players;

            //create turn order- will reassign to game with new player order so other
            //methods can just use players

            //if this doesn't work, std::shuffle <- this requires a random generator, need to import more stuff
            //shuffle all players, from start to end of vector
            random_shuffle(gamePlayers.begin(), gamePlayers.end());
            util::log(index, "Player order set");

            players = gamePlayers;
            
            //shuffle cards, 
            random_shuffle(cards.begin(), cards.end());
            util::log(index, "Cards shuffled");

            // -- Deal cards to players -- //
            //give 7 to each player
            //loop through all four players
            int position;
             for (int i = 0; i < 4; i++){
                //give seven cards
                dealCards(players[i]);
            }
            util::log(index, "Cards dealt");

            //one to be top of deck card
            //is this just checked by checking the top of the array?
            
            // -- Set game state to PLAYING -- //
            state = playing;
        }

        void Game::End(string reason) {
            // TODO: implement me!
        }

        void Game::TakeTurn(Player p, card::Card c, bool saidSudo, bool pickUp) {
            // TODO: implement me!
        }

        vector<card::Card> Game::createDeck()
        {
            // Initialize an empty deck
            vector<card::Card> deck;

            // Loop over each of the 4 colors
            for (int i = 0; i < 4; i++)
            {
                // Loop over each of the 13 types
                for (int j = 0; j < 13; j++)
                {
                    // Add a card of the current color and type to the deck
                    //(card::CardColor)i <- casting i to CardColor enum
                    deck.push_back(card::Card((card::CardColor)i, (card::CardType)j));

                    // If the card type is not 0, add a second card of the same color and type
                    if (j != 0)
                    {
                        deck.push_back(card::Card((card::CardColor)i, (card::CardType)j));
                    }
                }
            }

            // Add 4 wild and 4 wild4 cards to the deck
            for (int i = 0; i < 4; i++)
            {
                deck.push_back(card::Card(card::black, card::wild));
                deck.push_back(card::Card(card::black, card::wild4));
            }

            // Return the completed deck
            return deck;
        }
    }
}