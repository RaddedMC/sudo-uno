#ifndef GAME_H
#define GAME_H

#include <map>
#include <string>
#include <vector>

#include "../util/util.h"

using namespace std;

namespace SudoUno {
    namespace game {

        namespace card {
            enum CardColor {
                red,
                green,
                blue,
                yellow,
                black
            };

            extern map<CardColor, string> ColorNames;

            enum CardType {
                zero,
                one,
                two,
                three,
                four,
                five,
                six,
                seven,
                eight,
                nine,
                rev,
                skip,
                pltwo,
                wild4,
                wild
            };

            extern map<CardType, string> TypeNames;

            class Card {
                private:
                    CardColor color;
                    CardType type;
                
                public:
                    Card (CardColor ccolor, CardType ctype) {
                        color = ccolor;
                        type = ctype;
                    };

                    CardColor getColor() {return color;};
                    CardType getType() {return type;};
                    string getCardEncoding() {
                        return ColorNames[color] + "|" + TypeNames[type];
                    };
                    
                    // Returns true if card c2 can be played on top of card c1
                    static bool canPlace(Card c1, Card c2) {
                        // Wild can be played on any card
                        if (c2.getType() == CardType::wild || c2.getType() == CardType::wild4) {
                            return true;
                        }

                        // Same type on same type
                        if (c1.getType() == c2.getType()) {
                            return true;
                        }

                        // Same colour on same colour
                        if (c1.getColor() == c2.getColor()) {
                            return true;
                        }

                        return false;
                    }
            };
        }

        class Player {
            private:
                vector<card::Card> hand;
                network::Socket sk;
                string name;

            public:
                // Default constructor. Adds the name and socket.
                Player(string const & playerName, network::Socket playerSocket) : name(playerName), sk(playerSocket) {
                    name = playerName;
                    sk = playerSocket;
                };

                // Retrieves the player's name
                string getName() {
                    return name;
                };

                // Adds a card to the player's card hand.
                void addCard(card::Card c) {
                    hand.push_back(c);
                };

                // Removes a card from the player's hand if it exists
                void removeCard(card::Card c) {
                    // Find the first occurrence of the card
                    vector<card::Card>::iterator iter = find(hand.begin(), hand.end(), c);

                    // Card is in player's hand, remove it
                    if (iter != hand.end()) {
                        hand.erase(iter);
                    }
                };

                // Returns true if the specified card is in the player's hand
                bool hasCard(card::Card c) {
                    // Find the first occurrence of the card
                    vector<card::Card>::iterator iter = find(hand.begin(), hand.end(), c);

                    // Was the card found?
                    return iter != hand.end();
                };

                // Listens to the player's socket. Returns their response.
                string listen() { return proto::recieveProtoMessage(sk);}

                // Sends a message to the player's socket.
                void sendToSocket(string msg) {
                    sk.Write(msg);
                };

                // Closes a player's socket
                void closeSocket() {
                    sk.Close();
                }

                // Retrieve the player's hand
                vector<card::Card> getHand() {
                    return hand;
                }
        };

        enum GameState {
            playing,
            waiting,
            finished
        };

        extern map <GameState, string> GameStateNames;

        class Game {
            private:
                Player currentPlayer;
                vector<card::Card> cards;
                GameState state;
                int index;
                card::Card currentCard;
            public:
                vector<Player> players;
                Game(Player p, int i);
                Player getCurrentPlayer() {return currentPlayer;};
                Player getNextPlayer() {
                    // Get the index of the current player and retrieve the player at index+1
                    vector<Player>::iterator iter = find(players.begin(), players.end(), currentPlayer);
                    if (iter != players.end()) {
                        int index = iter - players.begin();
                        return players[(index+1) % players.size()];
                    }
                }
                void addPlayer(Player p) {
                    players.push_back(p);
                }
                void dealCards(Player * p);
                card::Card pullCard();
                void Start();
                void End(string reason);
                void TakeTurn(Player p, card::Card c, bool saidSudo, bool pickUp);
                int getNumPlayers() {return players.size();}
                vector<card::Card> createDeck();
                GameState getState() {return state;}
                card::Card getCurrentCard() {return currentCard;}
        };

        extern vector<Game> gamesVect;
    };
}

#endif