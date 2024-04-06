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

            map<CardColor, string> ColorNames = {
                {red, "Red"},
                {green, "Green"},
                {blue, "Blue"},
                {yellow, "Yellow"},
                {black, "Black"}
            };

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
        };

        enum GameState {
            playing,
            waiting,
            finished
        };

        map <GameState, string> GameStateNames = {
            {playing, "Playing"},
            {waiting, "Waiting"},
            {finished, "Finished"}
        };

        class Game {
            private:
                Player currentPlayer;
                vector<Player> players;
                vector<card::Card> cards;
                GameState state;
            public:
                Player getCurrentPlayer() {return currentPlayer;};
                void addPlayer(Player p) {
                    players.push_back(p);
                    dealCards(p);
                }
                void dealCards(Player p);
                card::Card pullCard();
                void Start();
                void End(string reason);
                void TakeTurn(Player p, card::Card c, bool saidSudo, bool pickUp);
        };
    };
}

#endif