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

            CardType NumberTypes[10] = {zero, one, two, three, four, five, six, seven, eight, nine};
            CardType ActionTypes[5] = {rev, skip, pltwo, wild4, wild};

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
        Game::Game(Player p, int i) : currentPlayer(&p),
            state(GameState::waiting),
            currentCard(card::Card((card::CardColor)0, (card::CardType)0))
        {
            addPlayer(p);
            index = i;
            isPlayerOrderForward = true;
            currentPlayerIndex = 0;
            isSkipPlayed = false;
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
            // Deck is empty, make a new one
            if (cards.size() == 0) {
                util::log(index, "No cards left in the deck, making a new one...");
                cards = refreshDeck();
                util::log(index, "Deck refreshed (" + to_string(cards.size()) + " cards available)");
            }

            // Remove and return the topmost card from the deck
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
            cards = createDeck();
            util::log(index, "Deck created");

            //create turn order- will reassign to game with new player order so other
            //methods can just use players

            //if this doesn't work, std::shuffle <- this requires a random generator, need to import more stuff
            //shuffle all players, from start to end of vector
            random_shuffle(players.begin(), players.end());
            util::log(index, "Player order set");

            // players = gamePlayers;
            currentPlayer = &players[0];
            
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
            // TODO: something seems off here
            //End the game by disconnecting all players in session then closing the server thread
            for (int i = 0; i < this->players.size(); i++) {
                players[i].sendToSocket("lobby.end\n\tfriendly_msg = \"" + reason + "\"");

                //Close the player's socket
                // TODO: add a CloseSocket method for a player
                players[i].closeSocket();
            }

            //Change game state to finished
            state = GameState::finished;

            //Add additional code to close the server thread if done here
        }

        // Ran when a player takes a turn that does not involve picking up a card.
        bool Game::TakeTurn(card::Card c, bool saidSudo, bool pickUp) {
            // Two valid actions: pick up a card from the deck, place a card from their hand

            util::log(index, "Player " + currentPlayer->getName() + " has entered their turn");
            util::log(index, "Num of cards in deck: " + to_string(cards.size()));

            // Player picks up a card from the deck
            if (pickUp) {
                card::Card pickedCard = pullCard();
                currentPlayer->addCard(pickedCard);
                util::log(index, "Player " + currentPlayer->getName() + " picked up a " + pickedCard.getCardEncoding());

                // Update player that they picked up a card
                string msg = "turn.approve\n.fin\n";
                currentPlayer->sendToSocket(msg);
                util::log(index, "Player " + currentPlayer->getName() + " turn has ended");

                // Move control to next player
                moveToNextPlayer();
                util::log(index, "Next turn: Player " + currentPlayer->getName());
                return true;
            }

            // Player wants to place a card from their hand

            // Does the player have the card that they want to use?
            if (!currentPlayer->hasCard(c)) {
                util::log(index, "Player " + currentPlayer->getName() + " tried to place a " + c.getCardEncoding() + " but this card is not in their hand!");                
                string msg = "turn.reject\n\treason: \"You don't have a " + c.getCardEncoding() + "\"\n.fin\n";
                currentPlayer->sendToSocket(msg);
                return false;
            }

            // Check if the move is valid
            bool isValid = card::Card::canPlace(currentCard, c);
            if (isValid) {
                string msg = "turn.approve\n";

                // Consume card
                currentCard = c;

                // Perform the action associated with the card if it's an action card
                if (c.isAction()) {
                    card::CardType type = c.getType();

                    // Next player gets +4 cards
                    if (type == card::CardType::wild4) {
                        Player* next = getNextPlayer();
                        util::log(index, "Added +4 cards to " + next->getName());
                        util::log(index, "Colour is now " + card::ColorNames[c.getColor()]);
                        for (int i = 0; i < 4; i++) {
                            next->addCard(pullCard());
                        }
                    }

                    // Next player gets +2 cards
                    if (type == card::CardType::pltwo) {
                        Player* next = getNextPlayer();
                        util::log(index, "Added +2 cards to " + next->getName());
                        for (int i = 0; i < 2; i++) {
                            next->addCard(pullCard());
                        }
                    }

                    // Reverse direction of play
                    if (type == card::CardType::rev) {
                        isPlayerOrderForward = !isPlayerOrderForward;
                        util::log(index, *"Player order is " + (isPlayerOrderForward ? "forward" : "reversed"));
                    }

                    // Skip next player's turn
                    if (type == card::CardType::skip) {
                        isSkipPlayed = true;
                        Player* next = getNextPlayer();
                        util::log(index, "Skipping " + next->getName() + "'s turn");
                    } else {
                        isSkipPlayed = false;
                    }

                    // Log on colour change
                    if (type == card::CardType::wild) {
                        util::log(index, "Colour is now " + card::ColorNames[c.getColor()]);
                    }
                }
                else if (c.isNumber()) {
                    util::log(index, "Played a number card");
                }
                else { // This should never happen
                    string err = c.getCardEncoding() + " is neither a number card nor an action card";
                    throw err;
                }

                currentPlayer->removeCard(c);

                // Was this the player's last card?
                if (currentPlayer->getHand().size() == 0) {
                    util::log(index, "Player " + currentPlayer->getName() + " placed down their last card (" + c.getCardEncoding() + ")");
                    // Player wins, end the game
                    End("The game ended because player " + currentPlayer->getName() + " placed their last card and won!");
                    return true;
                }

                util::log(index, "Player " + currentPlayer->getName() + " placed down a " + c.getCardEncoding());

                // Did the player place down their second-last card without saying SUDO?
                if (currentPlayer->getHand().size() == 1 && !saidSudo) {
                    // Add two cards to the player's hand
                    for (int i = 0; i < 2; i++) {
                        currentPlayer->addCard(pullCard());                        
                    }
                    msg += "\tYou forgot to say sudo.\n";
                    util::log(index, "Player " + currentPlayer->getName() + " didn't say SUDO - two cards were added to their hand");
                } 

                msg += ".fin\n";
                currentPlayer->sendToSocket(msg);
                util::log(index, "Player " + currentPlayer->getName() + " turn has ended");
                // Move control to next player (or next next player if a skip was played)
                moveToNextPlayer();
                if (isSkipPlayed) {
                    moveToNextPlayer();
                }
                util::log(index, "Next turn: Player " + currentPlayer->getName());      
                return true;          
            }
            else {
                util::log(index, "Player " + currentPlayer->getName() + " made an invalid move (" + c.getCardEncoding() + " ==> " + currentCard.getCardEncoding() + ")");
                string msg = "turn.reject\n\treason: \"A " + c.getCardEncoding() + " cannot be played on top of a " + currentCard.getCardEncoding() + "\"\n.fin\n";
                currentPlayer->sendToSocket(msg);
                return false;
            }
        }

        // Creates a shuffled deck that excludes the cards that are already in play
        vector<card::Card> Game::refreshDeck() {
            // TODO: regenerate/reshuffle deck based on cards that are not in play (top card or player hands)

            // Which cards are already in play?
            vector<card::Card> cardsInPlay;
            cardsInPlay.push_back(currentCard);
            for (int i = 0; i < players.size(); i++) {
                vector<card::Card> hand = players[i].getHand();
                for (int j = 0; j < hand.size(); j++) {
                    cardsInPlay.push_back(hand[j]);
                }
            }

            util::log(index, "There are " + to_string(cardsInPlay.size()) + " cards in play");

            vector<card::Card> newDeck;

            // Loop over each of the 4 colors
            for (int i = 0; i < 4; i++) {
                // Loop over each of the 13 types
                for (int j = 0; j < 13; j++) {
                    //(card::CardColor)i <- casting i to CardColor enum
                    auto possibleCard = card::Card((card::CardColor)i, (card::CardType)j);

                    if (find(cardsInPlay.begin(), cardsInPlay.end(), possibleCard) != cardsInPlay.end()) {
                        // Card is already in play; if there is only one occurrence of this card and it is not 0, add it to the deck
                        int numCards = count(cardsInPlay.begin(), cardsInPlay.end(), possibleCard);
                        cout << "OCCURRENCES: " << numCards << endl; // DEBUG
                        if (numCards == 1 && j != 0) {
                            newDeck.push_back(possibleCard);
                        }
                    } else {
                        // Card is not in play; if it is not 0 add two copies to the deck
                        newDeck.push_back(possibleCard);
                        if (j != 0) {
                            newDeck.push_back(possibleCard);
                        }
                    }
                }
            }

            // Add 4 wild and 4 wild4 cards to the deck
            for (int i = 0; i < 4; i++)
            {
                // WILD
                auto possibleCard = card::Card(card::black, card::wild);

                int numCards = count(cardsInPlay.begin(), cardsInPlay.end(), possibleCard);
                cout << "OCCURRENCES WILD: " << numCards << endl; // DEBUG
                
                for (int j = numCards; j < 4; j++) {
                    newDeck.push_back(possibleCard);
                }

                // WILD +4
                possibleCard = card::Card(card::black, card::wild4);

                numCards = count(cardsInPlay.begin(), cardsInPlay.end(), possibleCard);
                cout << "OCCURRENCES WILD +4: " << numCards << endl; // DEBUG
                
                for (int j = numCards; j < 4; j++) {
                    newDeck.push_back(possibleCard);
                }
            }

            // Shuffle the deck
            random_shuffle(cards.begin(), cards.end());
            util::log(index, "Shuffled new deck");
            return newDeck;
        }

        // Creates the card deck
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
                    // There is only one zero in uno
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