# MMO NAME HERE

## Authentication

## Connection

## Communication
Communication is done by sending packets through the connected websocket. 

### Packet Format
| 8      | 64         | n    |
|--------|------------|------|
| opcode | length (n) | data |

### Packet IDs
| ID | Code |
|---|---|
| 0 | ERROR |
| 1 | LOGIN_PACKET |
| 2 | LOGOUT_PACKET |
| 3 | SUCCESS_PACKET |
| 4 | FAILURE_PACKET |
| 5 | MOVE_PACKET |
| 6 | MOVED_PACKET |
| 7 | BAD_STATE_PACKET |

## BUGS
1. Headerlist crashes if a header is given that exceeds the buffer size of the buffered reader.
2. Mutliple people can log into the same player at once
3. Invalid packets over websocket lead to segfault
4. Removing awaiting enteries results in segfault occassionally
5. Players can sometimes walk through other players (but not vice-versa)