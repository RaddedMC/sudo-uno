#include "sudoThreads.h"

namespace SudoUno {
    namespace sudoThreads {
        void waiterThreadFunction(network::Socket sk) {

            string msgFull = "";

            util::log('S', "Waiting for client to provide a username...");
            while (true) {
                // Read the next incoming message from the client
                network::ByteArray msg;
                sk.Read(msg);

                string msgAsString = msg.ToString();
                cout << msgAsString.compare("."); // TODO: debug
                if (!msgAsString.compare(".")) {
                    // We received a new line. Message over!
                    util::log('S', "Message complete.");
                    break;
                }

                msgFull.append(msgAsString);
                util::log('S', "Message: \n" + msgFull);
            }
        }
    }
}