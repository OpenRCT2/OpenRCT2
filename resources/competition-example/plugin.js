/// <reference path="../../distribution/openrct2.d.ts" />

var COMPETITION_DURATION_DAYS = 10;

function main() {
    // Only run on server.
    if (network.mode !== 'server') return;

    // Start competition when server has >= 2 players.
    context.subscribe('network.join', function () {
        if (network.numPlayers >= 2 && competition.status === 'idle') {
            competition.start({
                name: 'Park Value Race',
                metric: 'parkValue',
                durationDays: COMPETITION_DURATION_DAYS,
                updateIntervalTicks: 100,
            });
            network.sendMessage('Competition started! Build the highest-value park in ' + COMPETITION_DURATION_DAYS + ' days!');
        }
    });

    // Log leaderboard each tick.
    context.subscribe('competition.tick', function () {
        var board = competition.leaderboard;
        var msg = 'Leaderboard (' + competition.timeRemaining + ' ticks left):\n';
        for (var i = 0; i < board.length; i++) {
            var entry = board[i];
            msg += (i + 1) + '. ' + entry.playerName + ' - ' + entry.score + '\n';
        }
        network.sendMessage(msg);
    });

    // Announce winner.
    context.subscribe('competition.end', function () {
        var board = competition.leaderboard;
        if (board.length > 0) {
            var winner = board[0];
            network.sendMessage('Competition over! Winner: ' + winner.playerName + ' with score ' + winner.score + '!');
        }
    });
}

registerPlugin({
    name: 'Competition Example',
    version: '1.0',
    authors: ['YashVyas95'],
    type: 'remote',
    licence: 'GPL-3.0',
    targetApiVersion: 111,
    main: main,
});
