#include "sudoThreads.h"
#include <unistd.h>

namespace SudoUno {
    namespace sudoThreads {
        void gameThreadFunction(game::Game g) {
            // Wait for the game to fill
            util::log('G', "Game lobby opened");

            // Wait for the game player count to increase
            int lastPlayers = g.getNumPlayers();
            while (true) {
                int currentPlayers = g.getNumPlayers();
                // If the number of players changes
                if (currentPlayers != lastPlayers) {
                    // Tell the players that their playercount changed
                    util::log('G', "Players have changed for this game!");
                    for (int i = 0; i < currentPlayers; i++) {
                        string msgToPlayer = "lobby.request.approved\n\tname=\"" + g.players[i].getName() + "\"\n\tplayers=\n";
                        string logString = "Players: [";
                        for (int otherPlayer = 0; otherPlayer < currentPlayers; otherPlayer++) {
                            logString += g.players[otherPlayer].getName() + ",";
                            if (otherPlayer != i) {
                                msgToPlayer += "\"" + g.players[otherPlayer].getName() + "\"\n";
                            }
                        }
                        msgToPlayer += ".fin\n";
                        util::log('G', logString + "]");
                        g.players[i].sendToSocket(msgToPlayer);
                    }
                }

                // We ideallly don't need to recheck this all the time
                // Check every 50 ms
                usleep(500000);
            }
        }
    }
}