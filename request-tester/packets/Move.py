from .Packet import Packet

class Move(Packet):
    def __init__(self, conn):
        super().__init__(5, conn.session_id)
        x = input("Input x position:")
        y = input("Input y position:")
        self.data = bytearray(8)
        self.data[0:4] = int(x).to_bytes(4)
        self.data[4:8] = int(y).to_bytes(4)