# Define the compiler
CXX = g++

# Define C++ flags (feel free to add optimization flags)
CXXFLAGS = -Wall -g

# Define the output executable name
TARGET = sudo-uno-server

# Define object files
# FIXME: Can we make this automatically grab all .cpp files?
# I discovered that if you do ".o", it will only compile changed CPP files which makes compilation faster
# How did I discover this when modern systems compile so fast? Because I've been devving this project in a code-server VM on my NAS, where my VMs are stored in hard drives that are slow :O
# Make doesn't seem to be able to recognize changes in header files automatically
OBJECTS = sudo-uno-server.o util/logger/logger.o util/network/blockable.o util/network/socket.o util/network/socketServer.o thread/waiter.o thread/gameThread.o util/proto/recieveProtoMessage.o util/misc/stringManip.o game/game.o

# Rule to compile a CPP file to an object file
.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to link object files into the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

# Rule to clean up object files and the executable
clean:
	rm -f $(OBJECTS) $(TARGET)

