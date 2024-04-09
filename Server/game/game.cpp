#include "game.h"
#include <random>
#include <vector>
#include <algorithm>
#include <random>

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

            Card parseCardFromString(string colorString, string typeString) {
                CardColor color = black;
                CardType type = one;

                for(auto const& pair: ColorNames) {
                    if(pair.second == colorString) {
                        color = pair.first;
                        break;
                    }
                }

                for(auto const& pair: TypeNames) {
                    if(pair.second == typeString) {
                        type = pair.first;
                        break;
                    }
                }

                return Card(color, type);
            }
        }

        map <GameState, string> GameStateNames = {
            {playing, "Playing"},
            {waiting, "Waiting"},
            {finished, "Finished"}
        };

        // Initializes the game
        Game::Game(Player p, int i) : currentPlayer(p),
            state(GameState::waiting),
            currentCard(card::Card((card::CardColor)0, (card::CardType)0))
        {
            addPlayer(p);
            index = i;
        }

        // Deals 7 cards to a player.
        void Game::dealCards(Player * p) {
            //Deal 7 cards to each player
            for (int i = 0; i < 7; i++) {
                //Gets a card from the deck and adds it to the player's hand
                p->addCard(pullCard());
            }
        }

        // Returns the topmost card from the deck.
        card::Card Game::pullCard() {
            card::Card topCard = cards.back();
            cards.pop_back();
            return topCard;
        }

        // Begins the game.
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
            currentPlayer = gamePlayers[0];
            
            //shuffle cards, 
            random_shuffle(cards.begin(), cards.end());
            util::log(index, "Cards shuffled");

            // -- Deal cards to players -- //
            //give 7 to each player
            //loop through all four players
            int position;
            for (int i = 0; i < 4; i++){
                //give seven cards
                dealCards(&players[i]);
            }
            util::log(index, "Cards dealt to players");

            //one to be top of deck card
            //is this just checked by checking the top of the array?
            currentCard = pullCard();

            // TODO: see above about referencing enums in a way that isn't silly
            if (currentCard.getColor() == 4) {
                // TODO: Test this edge case
                uniform_int_distribution<> colours(0,3);
                random_device rd;
                mt19937 gen(rd());
                // TODO: there must be a better way to do this
                card::CardColor randomColor = static_cast<card::CardColor>(colours(gen));
                card::CardType wildType = static_cast<card::CardType>(14);
                currentCard = card::Card(randomColor, wildType);
            }

            util::log(index, "Top card set!");

            // -- Set game state to PLAYING -- //
            state = playing;
        }

        // Terminates the game given some reason.
        // Reason is either the name of the player that won or "error"
        void Game::End(string reason) {
            // TODO: implement me!

            //End the game by disconnecting all players in session then closing the server thread
            for (int i = 0; i < this->players.size(); i++) {
                if(players[i].getName() == reason) {
                    //Send a message to the player
                    players[i].sendToSocket("Game Over: You won!");
                } else if(reason != "error"){
                    //Send a message to the player
                    players[i].sendToSocket("Game over: " + reason + " won!");
                } else {
                    //Send a message to the player
                    players[i].sendToSocket("Game over: An error occurred.");
                }

                //Close the player's socket
                // TODO: add a CloseSocket method for a player
                players[i].closeSocket();
            }

            //Change game state to finished
            state = GameState::finished;

            //Add additional code to close the server thread if done here
        }

        // Ran when a player takes a turn that does not involve picking up a card.
        void Game::TakeTurn(Player p, card::Card c, bool saidSudo, bool pickUp) {
            // TODO: implement me!
        }

        // Creates the card deck and shuffles it.
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