#include <string>
#include <iostream>
using namespace std;

namespace SudoUno {
    namespace misc {
        string toLowerCase(string input) {
            string returnValue = "";

            for (int i = 0; i < input.length(); i++) 
            {
                if (input[i] >= 65 && input[i] <= 90) {
                    returnValue[i] = (input[i] + 0x20);
                } else {
                    returnValue[i] = input[i];
                }
            }

            return returnValue;
        }
    }
}