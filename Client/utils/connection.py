import telnetlib
import select
import threading
import time


class Connection:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.connection = telnetlib.Telnet(self.host, self.port)
        self.receive()

    def send(self, protocol):
        for data in protocol:
            self.connection.write(data.encode("ascii"))

        self.connection.write(b"\n.fin")
        data = self.receive()
        return data

    def receive(self):
        try:
            data = self.connection.read_until(".fin".encode(), timeout=None)
            data = data.decode("ascii").replace(".fin", "")
            return data
        except EOFError:
            print("Connection closed.")
            return ""

    def close(self):
        self.connection.close()
        self.receive()

