// Minimal self-hosted master server for competitive mode, replacing servers.openrct2.io.
// Implements exactly what NetworkServerAdvertiser (register/heartbeat) and ServerList
// (GET listing) expect - see docs/competitive/README.md for why this exists: the real
// public OpenRCT2 master server has a fixed schema and silently drops the custom
// gameMode/competitiveProtocol/matchId fields this fork needs, so competitive listings
// never showed up there.
'use strict';
const http = require('http');
const crypto = require('crypto');

const PORT = 8080;
const ENTRY_TIMEOUT_MS = 90 * 1000; // a bit over the client's 60s heartbeat interval

// token -> { ip, port, lastSeen, ...fields from the register/heartbeat body }
const servers = new Map();

function readJsonBody(req) {
    return new Promise((resolve, reject) => {
        let data = '';
        req.on('data', (chunk) => (data += chunk));
        req.on('end', () => {
            try {
                resolve(data ? JSON.parse(data) : {});
            } catch (err) {
                reject(err);
            }
        });
        req.on('error', reject);
    });
}

function sweepExpired() {
    const now = Date.now();
    for (const [token, entry] of servers) {
        if (now - entry.lastSeen > ENTRY_TIMEOUT_MS)
            servers.delete(token);
    }
}

function clientIp(req) {
    // Behind no proxy here, so the raw socket address is the real one.
    return req.socket.remoteAddress.replace(/^::ffff:/, '');
}

function sendJson(res, status, body) {
    const payload = JSON.stringify(body);
    res.writeHead(status, { 'Content-Type': 'application/json' });
    res.end(payload);
}

const server = http.createServer(async (req, res) => {
    try {
        if (req.method === 'GET') {
            sweepExpired();
            const list = Array.from(servers.values()).map((entry) => ({
                ip: { v4: [entry.ip], v6: [] },
                port: entry.port,
                version: entry.version || 'unknown',
                requiresPassword: !!entry.requiresPassword,
                players: entry.players || 0,
                maxPlayers: entry.maxPlayers || 0,
                name: entry.name || 'Competitive match',
                description: entry.description || '',
                gameMode: entry.gameMode || 'competitive',
                competitiveProtocol: entry.competitiveProtocol || 0,
                phase: entry.phase || '',
                matchId: entry.matchId || '',
                scenarioName: entry.scenarioName || '',
                scenarioFileName: entry.scenarioFileName || '',
                scenarioHash: entry.scenarioHash || '',
                victory: entry.victory || '',
                allowLateJoin: !!entry.allowLateJoin,
            }));
            sendJson(res, 200, { servers: list });
            return;
        }

        if (req.method === 'POST') {
            // Registration: { key, port, address? }
            const body = await readJsonBody(req);
            const token = crypto.randomBytes(16).toString('hex');
            servers.set(token, {
                ip: body.address || clientIp(req),
                port: body.port,
                lastSeen: Date.now(),
            });
            sendJson(res, 200, { status: 200, token });
            return;
        }

        if (req.method === 'PUT') {
            // Heartbeat: { token, players, gameInfo: {...} }
            const body = await readJsonBody(req);
            const entry = servers.get(body.token);
            if (!entry) {
                sendJson(res, 200, { status: 401 });
                return;
            }
            entry.lastSeen = Date.now();
            entry.players = body.players;
            Object.assign(entry, body.gameInfo || {});
            sendJson(res, 200, { status: 200 });
            return;
        }

        sendJson(res, 404, { status: 404 });
    } catch (err) {
        sendJson(res, 500, { status: 500, message: String(err) });
    }
});

server.listen(PORT, () => {
    console.log(`Competitive master server listening on :${PORT}`);
});
