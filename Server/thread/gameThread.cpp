#include "sudoThreads.h"
#include <string>
#include <unistd.h>

namespace SudoUno {
    namespace sudoThreads {

        void updateGameState(game::Game* myGame) {
            // TODO: Implement me!
        }

        void gameThreadFunction(int gameThreadIndex) {

            game::Game* myGame = &game::gamesVect[gameThreadIndex];

            // Wait for the game to fill
            util::log(gameThreadIndex, "Game lobby opened");

            // Wait for the game player count to increase
            int lastPlayers = myGame->getNumPlayers();

            while (true) {

                // The current (changed) player count
                int currentPlayers = myGame->getNumPlayers();

                // If the number of players has changed
                if (currentPlayers != lastPlayers) {

                    // Log that players have changed
                    util::log(gameThreadIndex, "Players have changed for this game!");

                    // Begin logging the new player list
                    string logString = "Players: [";

                    // For each player
                    for (int i = 0; i < currentPlayers; i++) {

                        // Add them to the new player list log
                        logString += myGame->players[i].getName() + ",";

                        // Tell each player that the player list has changed by:
                        string msgToPlayer = "lobby.request.approved\n\tname=\"" + myGame->players[i].getName() + "\"\n\tplayers=\n";

                        // For each player that is not the player we are notifying
                        for (int otherPlayer = 0; otherPlayer < currentPlayers; otherPlayer++) {
                            if (otherPlayer != i) {
                                // Add that players name to the message
                                msgToPlayer += "\t\"" + myGame->players[otherPlayer].getName() + "\"\n";
                            }
                        }

                        // End the message with a .fin and send
                        msgToPlayer += ".fin\n";
                        myGame->players[i].sendToSocket(msgToPlayer);
                    }

                    // Log the player list
                    util::log(gameThreadIndex, logString + "]");

                    // Update the player count
                    lastPlayers = currentPlayers;
                }

                // Now, check if the lobby is full
                if (currentPlayers == 4) {
                    break;
                }

                // We ideallly don't need to recheck this all the time
                // Check every 50 ms
                usleep(500000); // TODO: If players join too quickly I don't think they will all be added
            }

            // The lobby is now full!

            // Notify everyone and prepare the game...

            for (int playerNum = 0; playerNum < 4; playerNum++) {

                string msg = "lobby.start\n\tname = \"" + myGame->players[playerNum].getName() + "\"\n\tplayers =";

                // lobby.start
                //     name = "yourname"
                //     players =
                //         "player1"
                //         "player2"
                //         "player3"
                // .fin
                for (int i = 0; i < 4; i++) {
                    if (i != playerNum) {
                        msg += "\n\t\"" + myGame->players[i].getName() + "\"";
                    }
                }

                myGame->players[playerNum].sendToSocket(msg + "\n.fin\n");
            }

            // Game is now started!

            myGame->Start();
        }
    }
}