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
            // TODO: filter pipe characters from the name if necessary
            regex str_expr("[^]+"+key+"[ ]*=[ ]*\"([a-zA-Z0-9| ]+)\"[^]+");
            smatch requestedItem;
            bool isMatch = regex_search(msg, requestedItem, str_expr);
            if (isMatch) {
                return requestedItem[1];
            } else {
                return "";
            }
        }

        // Retrieves a one-line encoded data pair where the pair is separated by a pipe character '|'
        vector<string> retrievePairedLineItem(string key, string msg) {
            string lineItem = retrieveOneLineItem(key, msg);
            // We need to break the pair
            vector<string> result;
            stringstream ss(lineItem);
            string item;

            while (getline(ss, item, '|')) {
                result.push_back(item);
            }

            return result;
        }

        // 0 = FALSE, 1 = TRUE, -1 = NO MATCH
        int retrieveBoolLineItem(string key, string msg) {
            // TODO: filter pipe characters from the name if necessary
            regex str_expr("[^]+"+key+"[ ]*=[ ]*(true|false)[^]+");
            smatch requestedItem;
            bool isMatch = regex_search(msg, requestedItem, str_expr);
            if (isMatch) {
                if (!requestedItem[1].compare("true")) return true;
                // We know that it must be 'false' because otherwise we would've returned -1
                else return false;
            } else {
                return -1;
            }
        }

    }
}