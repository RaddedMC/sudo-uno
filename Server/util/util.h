#ifndef UTIL_H
#define UTIL_H

// TODO: This header file could probably be broken into several smaller ones
#include <ctime>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <vector>
#include <string>

using namespace std;

namespace SudoUno {

    namespace util {
        void ascii();
        void log(char level, const string& msg);
    }

    namespace network {

        // TODO: needed?
        class ByteArray {
            public:
                vector<char> v;

                // TODO: needed?
                string ToString(void) const
                {
                    string returnValue  ;
                    for (int i = 0; i < v.size(); i++)
                        returnValue.push_back(c: v[i]);
                    return returnValue;
                }

                // Constructors
                ByteArray(void){} // Create from nothing
                ByteArray(string const & input) { // Create from string
                    for (int i = 0; i < input.size(); i++)
                        v.push_back(x: input[i]);
                }
                ByteArray(void * pointer, int size) { // Create from char*
                    char * temp = (char*) pointer;
                    for (int i = 0; i < size; i++)
                        v.push_back(x: temp[i]);
                }
        }

        // TODO: Blockable needs to be included
        class Socket : public Blockable {
            private:
                sockaddr_in socketDescriptor;
                
        }
    }
}

#endif