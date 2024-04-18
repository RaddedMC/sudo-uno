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

            extern CardType NumberTypes[10];
            extern CardType ActionTypes[5] ;

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

                    // Returns true if this card is a number card
                    bool isNumber() {
                        auto iter = find(begin(NumberTypes), end(NumberTypes), this->getType());
                        return iter != end(NumberTypes);
                    }

                    // Returns true if this card is an action card
                    bool isAction() {
                        auto iter = find(begin(ActionTypes), end(ActionTypes), this->getType());
                        return iter != end(ActionTypes);
                    }

                    bool operator==(const Card& other) {
                        return (color == other.color) && (type == other.type);
                    }
            };

            Card parseCardFromString(string colorString, string typeString);
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

                    vector<card::Card>::iterator iter;  

                    // Guard clauses to ensure wildcards are removed
                    if (c.getType() == card::CardType::wild) {
                        iter = find(hand.begin(), hand.end(), card::Card(card::CardColor::black, card::CardType::wild));
                    } else if (c.getType() == card::CardType::wild4) {
                        iter = find(hand.begin(), hand.end(), card::Card(card::CardColor::black, card::CardType::wild4));
                    } else {
                        iter = find(hand.begin(), hand.end(), c);
                    }

                    // Card is in player's hand, remove it
                    if (iter != hand.end()) {
                        hand.erase(iter);
                    }
                };

                // Returns true if the specified card is in the player's hand
                bool hasCard(card::Card c) {
                    // Find the first occurrence of the card
                    for (int i = 0; i < hand.size(); i++) {
                        cout << "Comparing " << c.getCardEncoding() << " to " << hand[i].getCardEncoding() << endl;
                        if(c.getType() == card::CardType::wild || c.getType() == card::CardType::wild4) {
                            return true;
                        }
                        if (c.getColor() == hand[i].getColor() && c.getType() == hand[i].getType()) {
                            return true;
                        }
                    }
                    return false;
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

                bool operator==(const Player& other) {
                    // Players are only the same if their mem address matches
                    return this == &other;
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
                Player *currentPlayer;
                vector<card::Card> cards;
                GameState state;
                int index;
                card::Card currentCard;
                int currentPlayerIndex;
                bool isPlayerOrderForward;
                bool isSkipPlayed;
            public:
                vector<Player> players;
                Game(Player p, int i);
                Player* getCurrentPlayer() {return currentPlayer;};
                void moveToNextPlayer() {
                    currentPlayerIndex = (isPlayerOrderForward ? (currentPlayerIndex + 1) : (currentPlayerIndex - 1)) % 4;
                    currentPlayer = &players[currentPlayerIndex];
                }
                Player* getNextPlayer() {
                    int nextPlayerIndex = (isPlayerOrderForward ? (currentPlayerIndex + 1) : (currentPlayerIndex - 1)) % 4;
                    return &players[nextPlayerIndex];
                }
                void addPlayer(Player p) {
                    players.push_back(p);
                }
                void dealCards(Player * p);
                card::Card pullCard();
                void Start();
                void End(string reason);
                bool TakeTurn(card::Card c, bool saidSudo, bool pickUp);
                int getNumPlayers() {return players.size();}
                vector<card::Card> refreshDeck();
                vector<card::Card> createDeck();
                GameState getState() {return state;}
                card::Card getCurrentCard() {return currentCard;}
        };

        extern vector<Game> gamesVect;
    };
}

#endif