import telnetlib


class Connection:
    def __init__(self, host, port):
        self.host = host
        self.port = port
        self.connection = telnetlib.Telnet(self.host, self.port)
        self.receive()

    def send(self, data):
        self.connection.write((data).encode("ascii"))
        self.connection.write(("\n").encode("ascii"))
        self.connection.write((".fin").encode("ascii"))
        self.receive()

    def receive(self):
        data = self.connection.read_some().decode("ascii")
        print(f"Response: {data}")
        return data

    def close(self):
        self.connection.close()
        self.receive()
