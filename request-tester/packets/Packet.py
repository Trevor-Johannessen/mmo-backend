from websocket import create_connection

class Packet:
    def __init__(self, opcode: int = 0, data: str = ""):
        self.opcode = opcode
        self.data = data
        self.length = len(data)

    def send(self, ws):
        bin = bytearray(9+len(self.data))
        bin[0] = self.opcode
        bin[1:9] = len(self.data).to_bytes(8)
        if isinstance(self.data, str):
            bin[9:] = bytes(self.data, 'utf-8')
        else:
            bin[9:] = self.data
        print(f"Sending {bin}")
        ws.send_binary(bin)