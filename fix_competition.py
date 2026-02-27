#!/usr/bin/env python3
"""
Script to fix GetContext() usage in NetworkCompetition.cpp
GetContext() returns a reference, not a pointer
"""

file_path = r"src/openrct2/network/NetworkCompetition.cpp"

# Read the file
with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Replace auto* ctx = GetContext(); with auto& ctx = GetContext();
content = content.replace('auto* ctx = GetContext();', 'auto& ctx = GetContext();')

# Replace ctx-> with ctx. 
content = content.replace('ctx->GetCompetitionManager()', 'ctx.GetCompetitionManager()')

# Also fix connection.Player to connection.player (lowercase)
content = content.replace('connection.Player', 'connection.player')

# Write back
with open(file_path, 'w', encoding='utf-8') as f:
    f.write(content)

print("NetworkCompetition.cpp fixed!")
