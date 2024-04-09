import telnetlib
import select


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
        data = ""
        while ".fin" not in data:
            ready_to_read, _, _ = select.select([self.connection], [], [], 1)
            if ready_to_read:
                data += self.connection.read_some().decode("ascii")
            else:
                return "No data"

        # remove the .fin from the data string and return the data
        data = data.replace(".fin", "")
        return data

    def close(self):
        self.connection.close()
        self.receive()
