#include "sudoThreads.h"
#include <string>
#include <unistd.h>
#include <algorithm>

namespace SudoUno {
    namespace sudoThreads {

        void updateGameState(game::Game* myGame, int gameIndex) {
            // Tell each player what's up
            
            // To send to everyone:
            // Game.state.update
            //     turn = "playername"
            //     players =
            //          "playername|6"
            //          "playertwo|6"
            //          "playerthree|6"
            //          "playerfour|6"
            //     current_card = "Red|5"
            string publicInfo = "game.state.update\n";
            util::log(gameIndex, "Status update:");
            
            // Add player name
            publicInfo += "\tturn = \"" + myGame->getCurrentPlayer().getName() + "\"\n";
            util::log(gameIndex, "It is " + myGame->getCurrentPlayer().getName() + "'s turn.");

            // Get player card counts
            publicInfo += "\tplayers = \n";
            for (int i = 0; i < myGame->getNumPlayers(); i++) {
                publicInfo += "\t\t\"" + myGame->players[i].getName() + "|" + to_string(myGame->players[i].getHand().size()) + "\"\n";
                
                string cardLogString = myGame->players[i].getName() + " Has cards:\n";
                for (int cardIndex = 0; cardIndex < myGame->players[i].getHand().size(); cardIndex++) {
                    cardLogString += "\t" + myGame->players[i].getHand()[cardIndex].getCardEncoding() + "\n";
                }
                util::log(gameIndex, cardLogString);
            }

            // Get current card
            publicInfo += "\tcurrent_card = \"" + myGame->getCurrentCard().getCardEncoding() + "\"\n";
            util::log(gameIndex, "The current card is " + myGame->getCurrentCard().getCardEncoding());

            // Your cards prefix
            publicInfo += "\tyour_cards =\n";

            // To send to a specific player:

            //     your_cards =
            //         "Red|5"
            //         "Green|6"
            //         "Yellow|Swap"
            //         ...
            // Your turn
            // .fin

            for (int playerIndex = 0; playerIndex < myGame->getNumPlayers(); playerIndex++) {
                game::Player workingPlayer = myGame->players[playerIndex];
                string privateInfo = publicInfo;
                for (int cardIndex = 0; cardIndex < workingPlayer.getHand().size(); cardIndex++) {
                    privateInfo += "\t\t\"" + workingPlayer.getHand()[cardIndex].getCardEncoding() + "\"\n";
                }
                if (!workingPlayer.getName().compare(myGame->getCurrentPlayer().getName())) {
                    privateInfo += "Your turn\n";
                }
                privateInfo += ".fin\n";
                workingPlayer.sendToSocket(privateInfo);
                util::log(gameIndex, "Notified player " + workingPlayer.getName());
            }

            util::log(gameIndex, "Game status update complete");
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

            while (myGame->getState() == game::GameState::playing) {
                // Update the state for the players
                updateGameState(myGame, gameThreadIndex);

                // Listen for the current player's move
                while (true) {
                    util::log(gameThreadIndex, "Waiting for " + myGame->getCurrentPlayer().getName() + " to make their move...");
                    string nextMove = myGame->getCurrentPlayer().listen();

                    // Move should be something along the lines of
                    // turn.take
                    //     card = "Red|6" -- or blank
                    //     pick = true    -- or false
                    //     sudo = false   -- or true

                    // First we should verify that the player sent 'turn.take'
                    string lowercaseMove = nextMove;
                    transform(lowercaseMove.begin(), lowercaseMove.end(), lowercaseMove.begin(), ::tolower);
                    if (lowercaseMove.find("turn.take") != string::npos) {
                        // They said 'turn.take'
                        // We need to ensure that they include "card, pick, sudo"
                        try {
                            // Extract their card, if present
                            vector<string> cardInfo = proto::retrievePairedLineItem("card", nextMove);

                            // Did they choose to pick up a card?
                            int chosePick = proto::retrieveBoolLineItem("pick", nextMove);

                            // Did they say SUDO?
                            int saidSudo = proto::retrieveBoolLineItem("sudo", nextMove);

                            // If there was an error extracting SUDO, or PICK, malformed!
                            if (saidSudo == -1 || chosePick == -1) {
                                myGame->getCurrentPlayer().sendToSocket("turn.reject\n\treason: \"You must include SUDO and PICK as booleans.\"\n.fin\n");
                                util::log(gameThreadIndex, "Their booleans were corrupt.");
                                continue;
                            }

                            // If they did not choose to pick up and the card | did not match, malformed!
                            if (cardInfo.size() != 2 && !chosePick) {
                                // Something went wrong
                                myGame->getCurrentPlayer().sendToSocket("turn.reject\n\treason: \"You did not provide correct card info. You must pick up a card if you cannot play one.\"\n.fin\n");
                                util::log(gameThreadIndex, "They did not provide correct card info and did not pick up.");
                                continue;

                            // Their move is valid
                            } else {

                                myGame->TakeTurn(myGame->getCurrentPlayer(), game::card::parseCardFromString(cardInfo[0], cardInfo[1]), saidSudo, chosePick);
                            }
                        }
                        // If something goes wrong here
                        catch (string e) {
                            myGame->getCurrentPlayer().sendToSocket("turn.reject\n\treason: \"Your message was malformed.\"\n.fin\n");
                            util::log(gameThreadIndex, "Their message was malformed and raised an exception.");
                            continue;
                        }

                    } else {
                        // They have malformed their message.
                        // We'll try again.
                        myGame->getCurrentPlayer().sendToSocket("turn.reject\n\treason: \"Your message was malformed.\"\n.fin\n");
                        util::log(gameThreadIndex, "Their message was malformed.");
                        continue;
                    }
                }
            }

            // The game has ended!
        }
    }
}