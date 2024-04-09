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
#include <unistd.h>
#include <stdarg.h>
#include <sys/time.h>
#include <sys/select.h>
#include <algorithm>
#include <random>
#include <regex>
#include "proc/Semaphore.h"

using namespace std;

// The SudoUno namespace will be used to maintain all of the additional functions and components of the server application.
namespace SudoUno {

    // The misc namespace is for miscellaneous utilities.
    namespace misc {
        // TODO: remove?
    }

    // The util namespace will handle utilities, like loggers.
    namespace util {
        void ascii();
        void log(char level, const string& msg);
        void log(int index, const string& msg);
    }

    // The network namespace will handle network functionalities like sockets.
    namespace network {

        typedef int TerminationException;

        class ByteArray {
            public:
                vector<char> v;

                string ToString(void) const
                {
                    string returnValue;
                    for (int i = 0; i < v.size(); i++)
                        returnValue.push_back(v[i]);
                    return returnValue;
                }

                // Constructors
                ByteArray(void){} // Create from nothing
                ByteArray(string const & input) { // Create from string
                    for (int i = 0; i < input.size(); i++)
                        v.push_back(input[i]);
                }
                ByteArray(void * pointer, int size) { // Create from char*
                    char * temp = (char*) pointer;
                    for (int i = 0; i < size; i++)
                        v.push_back(temp[i]);
                }
        };

        class Blockable {
            protected:
                int fd;
            public:
                Blockable(int f=0):fd(f){;}
                Blockable(Blockable const & b) : fd(dup(b.fd)){;}
                virtual ~Blockable(void){;}
                operator int(void)const {return fd;}
                void SetFD(int f){fd =f;}
                int GetFD(void) const {return fd;}
        };
        extern Blockable cinWatcher;

        class PipeUser : public Blockable {
            private:
                int sender; //The receiver is the base class
            protected:
                PipeUser(void);
                PipeUser(PipeUser const &);
                PipeUser & operator=(PipeUser const &);
                void Assign(PipeUser const &);
                ~PipeUser(void);
                void BlockForByte(void);
                void WriteByte(char c='P');
                char ConsumeByte(void);
        };

        class Event : public PipeUser {
            public:
                Event(void){;}
                ~Event(){;}
                Event (Event const &);
                Event & operator=(Event const &);
                void Trigger(void);
                void Wait(void);
                void Reset(void);
        };

        class Socket : public Blockable {
            private:
                sockaddr_in socketDescriptor;
                bool open;
                Event terminator;
            public:
                Socket(string const & ipAddress, unsigned int port);
                Socket(int socketFD);
                Socket(Socket const & s);
                Socket & operator=(Socket const & s);
                ~Socket(void);

                int Open(void);
                int Write(ByteArray const & buffer);
                int Write(string const & input);
                int Read(ByteArray & buffer);
                void Close(void);
        };

        class SocketServer : public Blockable
        {
            private:
                int pipeFD[2];
                Event terminator;
                sockaddr_in socketDescriptor;
            public:
                SocketServer(int port);
                ~SocketServer();
                Socket Accept(void);
                void Shutdown(void);
        };

        class FlexWait {
            public:
                static const int FOREVER; //==-1
                static const int POLL; // == 0
            private:
                std::vector<Blockable*> v;
            public:
                FlexWait(int n,...);
                Blockable * Wait(int timeout=-1);
        };

        class ThreadSem : public PipeUser {
            public:
                ThreadSem(int initialState=0);
                ThreadSem (ThreadSem const &);
                ~ThreadSem(){;}
                ThreadSem & operator=(ThreadSem const &);
                void Wait(void);
                void Signal(void);
        };
        
    };

    // The proto namespace will handle functionalities that involve our custom TCP protocol.
    namespace proto {
        string recieveProtoMessage(network::Socket sk);
        void malformedRequest(network::Socket sk);
        string retrieveOneLineItem(string key, string msg);
        vector<string> retrievePairedLineItem(string key, string msg);
        int retrieveBoolLineItem(string key, string msg);
    }
};

#endif