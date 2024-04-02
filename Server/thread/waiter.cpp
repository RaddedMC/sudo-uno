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

            if (misc::toLowerCase("") == "") {
                
            } else {
                // Malformed message
                util::log('W', "Their response was malformed. Dropping connection...");
                sk.Write("malformed response :(");
                sk.Close();
            }
        }

    }

}