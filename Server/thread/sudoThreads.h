#ifndef SUDOTHREADS_H
#define SUDOTHREADS_H

#include "../util/util.h"
#include "../game/game.h"

using namespace std;

namespace SudoUno {
    namespace sudoThreads {
        void waiterThreadFunction(network::Socket sk);
        void gameThreadFunction(int gameThreadIndex);
    }
}

#endif