#include "../util.h"
namespace SudoUno {

    namespace proto {

        // Helper to handle recieving a message from the client. Checks for '.fin', blocks until the response is fully sent, merges messages if they are sent in multiple packets
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
                    util::log('S', "Message: \n" + msgFull);
                    break;
                }
            }

            return msgFull;
        }

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

    }
}