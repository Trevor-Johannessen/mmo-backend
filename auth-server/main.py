from fastapi import FastAPI
from google.oauth2 import id_token
from google.auth.transport import requests
from fastapi.middleware.cors import CORSMiddleware
import random
import string
import socket

CLIENT_ID = '134391433938-2elausgjet3vtgbq2s6lu08b5h8qs52p.apps.googleusercontent.com'
AUTH_RECIEVER_PORT = 12000

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.connect(("localhost", AUTH_RECIEVER_PORT))
app = FastAPI()
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"]
)

@app.get("/")
async def root():
    return {"message": "Auth Server"}

@app.post("/authenticate")
async def authenticate(jwt: str):
    print(f"Got {jwt}")
    # Recieve Google JWT
    # Verify JWT is valid
    try:
        id_info = id_token.verify_oauth2_token(jwt, requests.Request(), CLIENT_ID)
        account_id = id_info['sub']
    except  ValueError:
        return "JWT is invalid."

    token=None
    while True: # Loop incase token is already taken (statistically improbable)
        # Generate token
        token = ''.join(random.SystemRandom().choice(string.ascii_lowercase + string.ascii_uppercase + string.digits) for _ in range(255))
        binary_token = token.encode('utf-8') # why am i not just generating a binary token
        print(f'Token is {token}.')

        # Convert ID to bstring
        binary_account_id = account_id.encode('utf-8') # why am i not just generating a binary token
        print(f'Account ID is {account_id}.')

        # Send token and account_id to backend server
        server.sendall(binary_token)
        server.sendall(binary_account_id)

        # Get port server is using to communicate with this client
        port = 0

        # If all was successful, break from loop. No need to retry.
        break
    # Send token and port back to client
    return {'token': token, 'port': port}


@app.post("/unsafe/authenticate")
async def authenticate(account_id=None):
    if not account_id:
        account_id = str(random.randrange(0,2000000000))
    token=None
    while True: # Loop incase token is already taken (statistically improbable)
        # Generate token
        token = ''.join(random.SystemRandom().choice(string.ascii_lowercase + string.ascii_uppercase + string.digits) for _ in range(255))
        binary_token = token.encode('utf-8') # why am i not just generating a binary token
        print(f'Token is {token}.')

        # Convert ID to bstring
        binary_account_id = account_id.encode('utf-8') # why am i not just generating a binary token
        print(f'Account ID is {account_id}.')

        # Send token and account_id to backend server
        server.sendall(binary_token)
        server.sendall(binary_account_id)

        # If all was successful, break from loop. No need to retry.
        break
    # Send token and port back to client
    return token