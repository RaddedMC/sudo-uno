#include <string>
using namespace std;

namespace SudoUno {
    namespace misc {
        string toLowerCase(string input) {
            string returnValue = "";

            for (int i = 0; i < input.length(); i++) 
            {
                if (input[i] >= 0x41 || input[i] <= 0x5A) {
                    returnValue[i] = (input[i] + 0x20);
                } else {
                    returnValue[i] = input[i];
                }
            }

            return returnValue;
        }
    }
}