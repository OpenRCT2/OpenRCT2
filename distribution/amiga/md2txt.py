#!/usr/bin/env python3
"""TESTER-GUIDE.md -> TESTER-GUIDE.txt: 78 columns, Latin-1, plain text for the Amiga side."""
import re, sys, textwrap
src = open(sys.argv[1], encoding="utf-8").read(); W = 78; out = []
def inline(t):
    t = re.sub(r'\*\*(.+?)\*\*', r'\1', t); t = re.sub(r'(?<!\*)\*(?!\*)(.+?)\*(?!\*)', r'\1', t)
    t = re.sub(r'`([^`]*)`', r'\1', t); t = re.sub(r'\[(.+?)\]\((.+?)\)', r'\1 (\2)', t)
    for a, b in (('—', '--'), ('–', '-'), ('×', 'x'), ('…', '...'), ('’', "'"), ('“', '"'), ('”', '"')): t = t.replace(a, b)
    return t
lines = src.splitlines(); i = 0; table = []
def flush_table():
    global table
    rows = [[inline(c.strip()) for c in r.strip().strip('|').split('|')] for r in table if not re.match(r'^\s*\|?\s*-', r)]
    rows = [r for r in rows if r and r[0]]
    if rows:
        w0 = max(len(r[0]) for r in rows)
        for r in rows:
            head = (r[0] + ':').ljust(w0 + 2); body = textwrap.wrap(r[1] if len(r) > 1 else '', W - len(head)) or ['']
            out.append(head + body[0]); out.extend(' ' * len(head) + b for b in body[1:])
        out.append('')
    table = []
while i < len(lines):
    l = lines[i]
    if l.strip().startswith('```'):
        flush_table(); indent = '        ' if l.startswith('   ') else '    '; i += 1
        while i < len(lines) and not lines[i].strip().startswith('```'): out.append(indent + lines[i].strip()); i += 1
        out.append(''); i += 1; continue
    if l.strip().startswith('|'): table.append(l); i += 1; continue
    flush_table()
    m = re.match(r'^(#+)\s+(.*)', l)
    if m:
        t = inline(m.group(2)); lvl = len(m.group(1)); out += ['', t.upper() if lvl == 1 else t, ('=' if lvl == 1 else '-') * len(t), '']
    elif l.strip() == '---': out.append('-' * W)
    elif re.match(r'^\s*[-*]\s+', l):
        body = inline(re.sub(r'^\s*[-*]\s+', '', l))
        while i + 1 < len(lines) and lines[i + 1].startswith('  ') and not re.match(r'^\s*[-*]\s+', lines[i + 1]) and not lines[i + 1].strip().startswith('```'):
            i += 1; body += ' ' + inline(lines[i].strip())
        w = textwrap.wrap(body, W - 3); out.append(' * ' + w[0]); out.extend('   ' + x for x in w[1:])
    elif re.match(r'^\s*\d+\.\s+', l):
        n = re.match(r'^\s*(\d+)\.\s+(.*)', l); body = inline(n.group(2))
        while i + 1 < len(lines) and lines[i + 1].startswith('   ') and not re.match(r'^\s*\d+\.\s+', lines[i + 1]) and not lines[i + 1].strip().startswith('```'):
            i += 1; body += ' ' + inline(lines[i].strip())
        w = textwrap.wrap(body, W - 4); out.append(f' {n.group(1)}. ' + w[0]); out.extend('    ' + x for x in w[1:]); out.append('')
    elif l.strip() == '': out.append('')
    else:
        para = inline(l.strip())
        while i + 1 < len(lines) and lines[i + 1].strip() and not lines[i + 1].startswith(('#', '```', '|', '-', '*', ' ')) and not re.match(r'^\d+\.', lines[i + 1]):
            i += 1; para += ' ' + inline(lines[i].strip())
        out.extend(textwrap.wrap(para, W))
    i += 1
flush_table()
txt = re.sub(r'\n{3,}', '\n\n', '\n'.join(out)).strip() + '\n'
open(sys.argv[2], 'w', encoding='latin-1', errors='replace').write(txt)
