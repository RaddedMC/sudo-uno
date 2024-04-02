# Define the compiler
CXX = g++

# Define C++ flags (feel free to add optimization flags)
CXXFLAGS = -Wall -g

# Define the output executable name
TARGET = sudo-uno-server

# Define object files
# FIXME: Can we make this automatically grab all .cpp files?
OBJECTS = sudo-uno-server.o util/logger/logger.o util/network/blockable.o util/network/socket.o util/network/socketServer.o thread/waiter.o util/proto/recieveProtoMessage.cpp util/misc/stringManip.cpp

# Rule to compile a CPP file to an object file
.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to link object files into the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

# Rule to clean up object files and the executable
clean:
	rm -f $(OBJECTS) $(TARGET)

