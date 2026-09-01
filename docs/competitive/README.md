# Native competitive multiplayer

This branch implements competitive multiplayer inside OpenRCT2. It does not use the retired JavaScript plugin or a separate companion process.

This document is the current source of truth. `PLUGIN_MIGRATION_CHECKLIST.md` is retained only as the historical feature inventory used during the native migration.

## Player flow

1. Open **Multiplayer** from the title screen and choose the **Competitive** tab.
2. A host chooses **Host competition**, selects a scenario, and configures match and rival-action rules before the lobby is created.
3. Players choose the advertised competition from the same browser. The host's exact scenario identity is authoritative; a player cannot compete using another scenario.
4. Each competitor receives a fresh, paused copy of the scenario and readies up in the lobby.
5. Every competitor runs an independent park and local calendar. Normal OpenRCT2 scenario finances and guest generation continue normally.
6. A park advances only while its player is online. Rival actions can target only online, unfinished competitors.

There is no shared map, entrance ownership, town guest pool, or guest allocation between parks.

## Time and victory

- Players may use any speed up to the host's configured maximum. No player waits for another player's calendar.
- Deadline matches snapshot each park's score on that park's own deadline date. Players may continue beyond it while other competitors finish, but later play does not alter the snapshot.
- Target matches support first to park rating, guests, competitive cash, park value, or cumulative competitive points, with the deadline as a fallback.
- The normal scenario objective does not decide the competitive result.
- The host may forfeit an offline, unfinished park or close the result early when an abandoned player will not return.

## Economy and fair play

Competitive cash is a separate authoritative match resource. The host configures starting cash and income rates. Native game-action interception accounts for construction spending without replacing a scenario's ordinary money rules; both money and no-money scenarios are supported.

While an unfinished competitor is in the lobby or a running match, OpenRCT2 cheat state is reset continuously. Cheat, scenario-editing, tile-editing, date-changing, map-resizing, peep-spawn, and ride-rating-freeze actions are rejected, cheat/debug toolbar access is hidden, and the configured speed cap is enforced in the game action itself. This is fair-play enforcement, not cryptographic anti-tamper against a deliberately modified executable.

## Current rival actions

Every current action has a host-configurable enable switch, competitive-cash cost, exact attacker-local-day cooldown, victim-local duration, and potency. The default cooldown is 256 local days (one RCT year). Cost is reserved by the host, then spent and put on cooldown only after the online victim acknowledges delivery; failed delivery is refunded.

### Vandal

The victim receives a named angry guest governed by normal path-addition vandalism and normal security-guard range. Default potency is four attempts. A successful break or an attempt stopped by security consumes one attempt, so security prevents damage and sends the vandal home sooner. The action also has a maximum victim-local lifetime.

### Misinformation

The victim is notified of an active campaign. It cancels future ordinary guest arrivals probabilistically, using a configurable reverse-equivalent of OpenRCT2's half-price-entry campaign strength. It does not move guests between parks, cap a park's natural guest count, or alter other competitors' generation.

### Poisoning

The attacker selects an open food or drink stall from the victim's reported park state. Only while that exact stall is poisoned, the native successful-purchase path tests each exact food/drink buyer against the configured percentage. A selected buyer receives maximum nausea and then follows OpenRCT2's normal nausea and vomiting behaviour; there is no delayed forced-vomit timer and no proximity/customer inference. The default is a 25% chance for seven victim-local days.

The purchase hook returns immediately when there are no local effects. Full guest, ride, and stall metrics are collected only when the local day changes, not on every game update.

## Disconnects, saves, and the host

- A disconnected competitor is marked offline and their park is forced paused.
- On first loss of a running host connection, the client creates a recovery autosave, explains why the park was paused, and keeps reconnecting.
- Competitive identity, authoritative state, local effects, and connection details are stored in native `.park` saves.
- Loading a client recovery save reconnects while paused. Loading the host's latest competitive save reopens the competition listener, restores the match, marks remote parks offline, and allows them to reclaim their seats.
- There is currently no automatic live host migration. A host crash is recovered from the host's latest save/autosave. This avoids silently transferring authority to a client that may be unreachable or have stale state.

## Observation and park spectating

The title-screen **Observe match** action joins the live competition state and leaderboard without claiming a competing park. From the in-game leaderboard, select an online competitor and use **Watch park** to connect to their actual park through OpenRCT2's native multiplayer map stream.

Every solo competitive park automatically opens an unadvertised watch endpoint. Watch connections explicitly request a dedicated competitive-spectator group; that group has chat permission and no game-action permissions. Automatically created watch servers force every non-host connection into that read-only group. Competitive membership is omitted from streamed park data, so a watcher cannot inherit or impersonate the watched park's competition seat.

For a competitor watching a rival, OpenRCT2 first writes a dedicated return save under `saves/competitive-return`, then marks their own park offline and switches to the rival stream. The globe menu remains available and changes to **Return to my park**. Returning closes the watch connection, reloads the saved park, restores its competitive membership, reconnects it to the host, and removes the temporary return save. If OpenRCT2 closes while watching, the return save remains available for manual recovery.

Leaderboard-only observers keep their competition-state connection while watching and use the same globe-menu route to leave the park stream and return to the leaderboard.

The endpoint is a normal direct OpenRCT2 server connection. LAN play works without a relay; internet viewers still require the target's selected watch port to be reachable through its firewall/NAT. Relay support remains useful future work for networks that cannot accept inbound connections.

## Proposed native rival actions

These are specifications for later implementation, not current buttons. They should use the same online-only acknowledgement, refund, cost, exact-day cooldown, duration, and potency controls as existing actions.

### Toilet Bomber

Select a currently open toilet. At detonation, identify occupants from the ride/shop interaction state, create the ordinary explosion effects, remove those occupants, apply one ride-fatality-equivalent casualty event, and demolish the toilet through the normal ride-demolition action so map and ride references are cleaned correctly.

### Agitator

Send an otherwise normal guest. At a bounded interval, only guests sharing its tile are affected, and each victim is affected at most once. They receive a dedicated “Another guest was rude to me” thought and a happiness penalty balanced against the existing crowding penalty. A configurable affected-guest quota and lifetime prevent global guest scans and unbounded work.

### Saboteur

Select a valid open ride that can break down. The agent visits and rides it normally. An exact ride-completion hook then schedules a supported breakdown and sends the agent home. Closing or demolishing the ride before completion must end or reject the action deterministically rather than silently choosing another ride.

### Hitman

The visual cue is the ordinary handheld guest-camera animation: `PeepActionType::takePhoto` / `PeepAnimationType::takePhoto`. It is **not** an on-ride-photo flash. The agent selects a valid ordinary guest deterministically, faces them, performs the camera animation, and at a defined animation frame marks that exact victim for the engine's existing guest explosion effect. The park then receives exactly one on-ride-fatality-equivalent casualty consequence and the Hitman leaves. Competitive agents, guests already leaving, and guests currently on rides are not valid victims.

Costs, cooldowns, quotas, and durations for these proposed actions need balance testing before defaults are fixed.

## Known remaining work

- Optional relayed discovery/transport for hosts who cannot accept an inbound connection.
- Live host migration, if a safe authority election and reachable replacement endpoint can be designed.
- The proposed native rival actions above.
