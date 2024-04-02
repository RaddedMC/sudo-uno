#include "../util.h"
namespace SudoUno {

    namespace proto {
        
        string recieveProtoMessage(network::Socket sk) {

            string msgFull = "";

            while (true) {
                // Read the next incoming message from the client
                network::ByteArray msg;
                sk.Read(msg);

                string msgAsString = msg.ToString();

                msgFull.append(msgAsString);
                
                // If the user inputs a dot on an empty line, the message has ended
                if (msgFull.find(".fin") != string::npos) {
                    util::log('S', "Message: \n" + msgFull + "End of message");
                    break;
                }
            }

            return msgFull;
        }

    }
}