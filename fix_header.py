#!/usr/bin/env python3
"""
Fix the missing comment header in NetworkCompetition.cpp
"""

file_path = r"src/openrct2/network/NetworkCompetition.cpp"

# Read the file
with open(file_path, 'r', encoding='utf-8') as f:
    content = f.read()

# Fix the header - it's missing the opening /***
if not content.startswith('/***'):
    content = '/*****************************************************************************\n' + content[content.find(' * Copyright'):]

# Write back
with open(file_path, 'w', encoding='utf-8') as f:
    f.write(content)

print("Header fixed!")
