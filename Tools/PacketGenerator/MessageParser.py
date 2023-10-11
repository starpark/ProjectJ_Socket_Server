class MessageParser():
    def __init__(self, start_id, recv_prefix, send_prefix):
        self.recv_pkt = []
        self.send_pkt = []
        self.total_pkt = []
        self.start_id = start_id
        self.id = start_id
        self.recv_prefix = recv_prefix
        self.send_prefix = send_prefix

    def parse_message(self, path):
        f = open(path, 'r')
        lines = f.readlines()

        for line in lines:
            if line.startswith('message') == False:
                continue

            pkt_name = line.split()[1].upper()
            if pkt_name.startswith(self.recv_prefix) == True:
                self.recv_pkt.append(Packet(pkt_name, self.id))
            elif pkt_name.startswith(self.send_prefix) == True:
                self.send_pkt.append(Packet(pkt_name, self.id))
            else:
                continue

            self.total_pkt.append(Packet(pkt_name, self.id))
            self.id += 1

        f.close()

class Packet:
    def __init__(self, name, id):
        self.name = name
        self.id = id
