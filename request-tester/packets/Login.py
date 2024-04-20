from .Packet import Packet

class Login(Packet):
    def __init__(self, conn):
        super().__init__(1, conn.session_id)