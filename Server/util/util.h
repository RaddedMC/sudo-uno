#ifndef UTIL_H
#define UTIL_H

#include <ctime>
#include <iostream>

using namespace std;

namespace SudoUno {
    namespace util {
        void ascii();
        void log(char level, const string& msg);
    }
}

#endif