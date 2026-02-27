#!/usr/bin/env python3
"""
Script to add lobby handler registrations to NetworkBase.cpp
"""
import re

file_path = r"src/openrct2/network/NetworkBase.cpp"

# Read the file
with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Add client handlers after gameState handler
client_handlers = '''client_command_handlers[Command::gameState] = &NetworkBase::Client_Handle_GAMESTATE;
        // Lobby client handlers
        client_command_handlers[Command::lobbyStateSync] = &NetworkBase::Client_Handle_LOBBY_STATE_SYNC;
        client_command_handlers[Command::lobbyJoin] = &NetworkBase::Client_Handle_LOBBY_PLAYER_JOINED;
        client_command_handlers[Command::lobbyLeave] = &NetworkBase::Client_Handle_LOBBY_PLAYER_LEFT;
        client_command_handlers[Command::lobbyPlayerReady] = &NetworkBase::Client_Handle_LOBBY_PLAYER_READY;'''

content = content.replace(
    'client_command_handlers[Command::gameState] = &NetworkBase::Client_Handle_GAMESTATE;',
    client_handlers
)

# Add server handlers after heartbeat handler
server_handlers = '''server_command_handlers[Command::heartbeat] = &NetworkBase::ServerHandleHeartbeat;
        // Lobby server handlers
        server_command_handlers[Command::lobbyJoin] = &NetworkBase::ServerHandleLobbyJoin;
        server_command_handlers[Command::lobbyLeave] = &NetworkBase::ServerHandleLobbyLeave;
        server_command_handlers[Command::lobbyPlayerReady] = &NetworkBase::ServerHandleLobbyPlayerReady;
        server_command_handlers[Command::lobbyStartRequest] = &NetworkBase::ServerHandleLobbyStartRequest;'''

content = content.replace(
    'server_command_handlers[Command::heartbeat] = &NetworkBase::ServerHandleHeartbeat;',
    server_handlers
)

# Write back
with open(file_path, 'w', encoding='utf-8') as f:
    f.write(content)

print("Handler registrations added successfully!")
