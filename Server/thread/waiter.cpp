#include "sudoThreads.h"

using namespace std;

namespace SudoUno {

    namespace sudoThreads {

        void waiterThreadFunction(network::Socket sk) {
            sk.Write("sudo-uno hello\nplease provide username\n");

            util::log('W', "Waiting for client to provide a username...");
            string protomsg = proto::recieveProtoMessage(sk);

            // Verify we got a real username
            // Message should contain 
            // Lobby.request
            // name = ""
            string lowercase = misc::toLowerCase(protomsg);
            if (lowercase.find("lobby.request") != string::npos) {
                // Now we need to check for the name line
                util::log('W', "They requested a lobby"); // TODO: debug
                // regex str_expr("^name = \"([a-zA-Z0-9]+)\"$");
                // util::log('W', "We made a regex"); // TODO: debug
                // bool isMatch = regex_match(lowercase, str_expr);
                // util::log('W', "We matched a regex"); // TODO: debug
                // cout << isMatch << endl;
            } else {
                // Malformed message
                util::log('W', "Their response was malformed. Dropping connection...");
                sk.Write("malformed response :(\n");
                sk.Close();
            }
        }

    }

}