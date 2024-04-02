#include "util/util.h"
#include "thread/sudoThreads.h"
#include <thread>

using namespace SudoUno;
using namespace std;

// As i've done for all the labs, a vector to hold the threads to keep them referenced until shutdown
vector <thread> threads;

// Spawns a waiter thread to wait for the player to enter a username
void spawnWaiterThread(network::Socket sk) {
    thread tr(sudoThreads::waiterThreadFunction, sk);
    threads.push_back(move(tr));
}

int main() {
    util::log('I', "init sudo-uno-server");

    // TODO: Command-line args to set port number and interface bind
    const string bind_if = "0.0.0.0";
    const int bind_port = 6969;

    util::log('I', "Using default settings. Will bind to " + bind_if + ":" + to_string(bind_port));

    // Instantiate the socket server
    // TODO: can we even change the bind if? do we need to?
    try {
        network::SocketServer serveSock(6969);

        util::log('I', "Socket opened on port " + to_string(bind_port));
        util::ascii();

        // Loop indefinitely for each request
        while (true) {
            util::log('S', "Waiting for a player to connect...");
            network::Socket clientSocket(serveSock.Accept());
            util::log('S', "Connection established! Isolating...");

            // Always wrap connections into their own thread
            spawnWaiterThread(clientSocket);
        }
    } catch (string e) {
        util::log('E', "That port is currently bound to another process.");
        exit(1);
    }
}