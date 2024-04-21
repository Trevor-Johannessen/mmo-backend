from websocket import create_connection
from datetime import datetime
import inspect
import requests
import packets

aliases = {}
connections = {}
smallest_id = 1

def findSmallestId():
    global smallest_id
    old_id = smallest_id
    smallest_id+=1
    while str(smallest_id) in connections:
        smallest_id+=1
    return old_id

class WebSocket:
    def __init__(self, id):
        self.id = id
        self.ws = create_connection("ws://localhost:12001/")

class Connection:
    def __init__(self, alias):
        # get new id
        self.id = findSmallestId()
        self.aliases = []
        self.packets_sent = 0
        self.time_created = datetime.now()

        # create new alias
        if alias:
            self.aliases.append(alias)
            aliases[alias] = self.id

        # contact auth server to get token
        res = requests.post('http://localhost:8000/unsafe/authenticate')
        self.session_id = res.text[1:-1]

        # create new websocket connection
        self.ws = WebSocket(self.id)

        # display infomation
        if alias:
            print(f"Connected web-socket {self.id} with alias {self.alias}.")
        else:
            print(f"Connected web-socket {self.id}.")
        
    def alias(self, alias, verbose=True):
        if alias.isnumeric() and int(alias) > 0:
            print("Error: Alias cannot be conflict with connection ids.")
            return
        
        if alias in self.aliases:
            self.aliases.remove(alias)
            del aliases[alias]
            print(f"Removed alias {alias} from connection {self.id}.")
            if len(self.aliases) > 0:
                print(f"Remaining aliases: {self.aliases}")
            return
        
        if alias in aliases:
            print(f"Error: Alias {alias} is already assigned to connection {aliases[alias]}.")
            return
        
        aliases[alias] = str(self.id)
        self.aliases.append(alias)
        print(f"Aliased connection {self.id} as {alias}.")

    def close(self, verbose=True):
        global smallest_id
        for alias in self.aliases:
            self.alias(alias)
        if smallest_id > self.id:
            smallest_id = self.id
        del connections[str(self.id)]
        self.ws.ws.close()
        if(verbose):
            print(f"Closing connection {self.id}.")

    def send(self, type, verbose=True):
        type = f"{type[0].upper()}{type[1:].lower()}"
        packet_class = None
        for obj in inspect.getmembers(packets,inspect.isclass):
            if obj[0] == type:
                packet_class = obj[1]
                break
        if not packet_class:
            print(f"Error: Invalid packet type {type}.")
            return
        self.packets_sent+=1
        packet_class(conn=self).send(self.ws.ws)

    def recv(self, verbose=True):
        print(self.ws.ws.recv_data()) # need to fix this to recieve bytes

def printHelp():
    print("Valid commands:")
    print("\tHelp - Displays the help menu.")
    print("\tConnect {alias?} - Creates a new connection.")
    print("\tAlias {id} {alias} - Creates an alias for a connection id.")
    print("\tSend {id} - Sends a packet to the given id.")
    print("\tClose {id} - Closes the connection with the given id.")


def printList():
    print("ID\tOpened\t\tRequests Sent\tAliases")
    for id in connections:
        conn = connections[id]
        print(f"{id}\t{conn.time_created.strftime('%H:%M:%S')}\t{conn.packets_sent}\t\t{conn.aliases}")

while True:
    cmd = input("Packet > ")
    args = cmd.split()

    # Check if empty input
    if len(args) < 1:
        continue
    args[0].lower()

    # Check for exit
    if args[0] == 'exit':
        for id in list(connections.keys()):
            connections[id].close(False)
        break

    # Check for help
    if args[0] == 'help':
        printHelp()
        continue

    if args[0] == 'list':
        printList()
        continue

    # Check for connect:
    if args[0] == 'connect':
        if len(args)-1 > 1:
            print("Error: Invalid number of arguments. (Expected 0 or 1)")
            continue
        new_connection = Connection((args[1:]))
        connections[str(new_connection.id)] = new_connection
        continue

    # Check if command is valid
    if not (hasattr(Connection, args[0]) and type(getattr(Connection, args[0])).__name__ == 'function'):
        print("Error: Invalid command. Enter 'help' for a list of valid commands.")
        continue

    # Check if the number of arguments is correct
    expected_args = len(inspect.signature(getattr(Connection, args[0])).parameters)-1
    if len(args)-1 != expected_args:
        print(f"Error: Invalid number of arguments. (Expected {expected_args}, Got {len(args)-1})")
        continue
    
    # Check identifier
    if len(args) > 1:
        identifier = args[1]
        if identifier not in connections:
            if identifier not in aliases:
                print("Error: Invalid identifier.")
                continue
            identifier = aliases[identifier]

    # Execute command
    getattr(connections[identifier], args[0])(*tuple(args[2:]))
