#include "sudoThreads.h"

namespace SudoUno {
    namespace sudoThreads {
        void gameThreadFunction(game::Game g) {
            cout << "le game thread says hemlo";
            // Wait for the game to fill
            util::log('G', "Game started!");

            while (true) {
                // do nothing
            }
        }
    }
}