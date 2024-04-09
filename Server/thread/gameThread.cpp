#include "sudoThreads.h"
#include <string>
#include <unistd.h>

namespace SudoUno {
    namespace sudoThreads {
        void gameThreadFunction(int gameThreadIndex) {

            game::Game* myGame = &game::gamesVect[gameThreadIndex];

            // Wait for the game to fill
            util::log('G', "Game lobby opened");

            // Wait for the game player count to increase
            int lastPlayers = myGame->getNumPlayers();

            while (true) {

                // The current (changed) player count
                int currentPlayers = myGame->getNumPlayers();

                // If the number of players has changed
                if (currentPlayers != lastPlayers) {

                    // Log that players have changed
                    util::log('G', "Players have changed for this game!");

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
                    util::log('G', logString + "]");

                    // Update the player count
                    lastPlayers = currentPlayers;
                }

                // We ideallly don't need to recheck this all the time
                // Check every 50 ms
                usleep(500000);
            }
        }
    }
}