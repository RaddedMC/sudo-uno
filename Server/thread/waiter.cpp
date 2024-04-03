#include "sudoThreads.h"
#include <algorithm>
#include <string>
#include <random>

using namespace std;

namespace SudoUno {

    namespace sudoThreads {

        // Runs when a malformed request is sent. Disconnects the client and logs
        void malformedRequest(network::Socket sk) {
                util::log('S', "Their response was malformed. Dropping connection...");
                sk.Write("malformed response :(\n");
                sk.Close();
        }

        // Retrieves a one-line encoded data item from a message on the protocol
        string retrieveOneLineItem(string key, string msg) {
            regex str_expr("[^]+name[ ]*=[ ]*\"([a-zA-Z0-9 ]+)\"[^]+");
            smatch requestedItem;
            bool isMatch = regex_search(msg, requestedItem, str_expr);
            if (isMatch) {
                return requestedItem[1];
            } else {
                return "";
            }
        }

        // Waiter thread main.
        // Thread count = 1 per client
        // Waits for the client to request a lobby and provide a username
        void waiterThreadFunction(network::Socket sk) {
            // Greet the client
            sk.Write("sudo-uno hello\nplease provide username\n");

            // Recieve their response
            util::log('W', "Waiting for client to provide a username...");
            string protomsg = proto::recieveProtoMessage(sk);

            // Verify we got a real username
            // Message should contain 
            // lobby.request
            // name = ""
            // .fin
            string lowercase = protomsg;
            transform(lowercase.begin(), lowercase.end(), lowercase.begin(), ::tolower);
            if (lowercase.find("lobby.request") != string::npos) {
                // Now we need to check for the name line
                string name = retrieveOneLineItem("name", protomsg);
                if (name.compare("")) {
                    // What is their name?
                    util::log('W', "They provided username '" + name + "'");

                    // We must now add our randomly-generated characters
                    // Okay, I used ChatGPT for this one.
                    const string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
                    mt19937 mt(random_device{}());

                    uniform_int_distribution<int> distribution(0, characters.size() - 1);
                    name.append("-");
                    for (int i = 0; i < 5; i++) {
                        name += characters[distribution(mt)];
                    }

                    // We've modified their username! Time to tell them.
                    util::log('W', "Modified their username to '" + name + "'");

                    sk.Write("lobby.request.approved\n\tname=\"" + name + "\"\n\tplayers=\n.fin\n");
                } else {
                    malformedRequest(sk);
                }
            } else {
                // Malformed message
                malformedRequest(sk);
            }
        }

    }

}