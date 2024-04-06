#ifndef SUDOTHREADS_H
#define SUDOTHREADS_H

#include "../util/util.h"

using namespace std;

namespace SudoUno {
    namespace sudoThreads {
        void waiterThreadFunction(network::Socket sk);
    }
}

#endif