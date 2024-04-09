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
                void addPlayer(Player p) {
                    players.push_back(p);
                }
                void dealCards(Player p);
                card::Card pullCard();
                void Start();
                void End(string reason);
                void TakeTurn(Player p, card::Card c, bool saidSudo, bool pickUp);
                int getNumPlayers() {return players.size();}
                vector<card::Card> createDeck();
        };

        extern vector<Game> gamesVect;
    };
}

#endif