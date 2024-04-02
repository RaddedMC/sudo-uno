#include "util/util.h"
#include <thread>

using namespace SudoUno;
using namespace std;

// As i've done for all the labs, a vector to hold the threads to keep them referenced until shutdown
vector <thread> threads;

int main() {
    util::log('I', "init sudo-uno-server");
    util::ascii();

    // TODO: Command-line args to set port number and interface bind
    const string bind_if = "0.0.0.0";
    const int bind_port = 6969;

    util::log('I', "Using default settings. Will bind to " + bind_if + ":" + to_string(bind_port));

    // Instantiate the socket server
    // TODO: can we even change the bind if? do we need to?
    SocketServer serveSock(bind_port);
    util::log('I', "Socket opened on port " + to_string(bind_port));

    // Loop indefinitely for each request
    while (true) {
        util::log('S', "Waiting for a player to connect...");
        Socket clientSocket(serveSock.Accept());
        util::log('S', "Connection established! Isolating...");

        // Always wrap connections into their own thread
    }
}