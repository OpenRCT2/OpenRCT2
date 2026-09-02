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
- Target matches support first to park rating, guests, actual scenario cash, park value, or cumulative competitive points, with the deadline as a fallback.
- The normal scenario objective does not decide the competitive result.
- The host may forfeit an offline, unfinished park or close the result early when an abandoned player will not return.

## Economy and fair play

There is no parallel competitive currency. In money scenarios, the park's real cash pays for both ordinary play and rival actions, and the cash victory metric reads that same balance. In no-money scenarios, rival actions have no cash charge and are governed by their host-configured cooldowns and effect limits. Both money and no-money scenarios are supported.

While an unfinished competitor is in the lobby or a running match, OpenRCT2 cheat state is reset continuously. Cheat, scenario-editing, tile-editing, date-changing, map-resizing, peep-spawn, and ride-rating-freeze actions are rejected, cheat/debug toolbar access is hidden, and the configured speed cap is enforced in the game action itself. This is fair-play enforcement, not cryptographic anti-tamper against a deliberately modified executable.

## Current rival actions

Every action has a host-configurable enable switch, real park-cash price, exact attacker-local-day cooldown, and victim-local time limit or duration. Strength is configurable where the effect has a variable strength. The real park-cash price is charged when the action is submitted, refunded if delivery fails, and retained only after the online victim acknowledges it. The cooldown begins on that acknowledgement. Prices are ignored in no-money scenarios.

Open the in-park multiplayer window to reach the competition leaderboard. The **Attack rival...** button is visible but disabled in the lobby, where its tooltip explains that attacks begin with the match. During a running match, select an online, unfinished rival in the leaderboard, then choose from the seven configured actions. When no valid rival is selected, the disabled button explains what is required instead of disappearing.

### Vandal

The victim receives a named angry guest governed by normal path-addition vandalism and normal security-guard range. Default potency is four attempts. A successful break or an attempt stopped by security consumes one attempt, so security prevents damage and sends the vandal home sooner. The action also has a maximum victim-local lifetime.

### Misinformation

The victim is notified of an active campaign. It cancels future ordinary guest arrivals probabilistically, using a configurable reverse-equivalent of OpenRCT2's half-price-entry campaign strength. It does not move guests between parks, cap a park's natural guest count, or alter other competitors' generation.

### Poisoning

The attacker selects an open food or drink stall from the victim's reported park state. Only while that exact stall is poisoned, the native successful-purchase path tests each exact food/drink buyer against the configured percentage. A selected buyer receives maximum nausea and then follows OpenRCT2's normal nausea and vomiting behaviour; there is no delayed forced-vomit timer and no proximity/customer inference. The default is a 25% chance for seven victim-local days.

The purchase hook returns immediately when there are no local effects. Full guest, ride, and stall metrics are collected only when the local day changes, not on every game update.

### Toilet Bomber

The attacker selects an open toilet reported by the victim. A named operative enters normally and heads to that exact toilet. Once inside, ordinary explosion visuals kill every ordinary guest currently using it and the normal ride-demolition path removes the building and cleans its references. Any demolition refund is neutralised, and occupied detonation applies one normal accident-equivalent park-rating consequence.

### Agitator

A named guest otherwise behaves normally. While the effect is active, ordinary guests sharing the agitator's tile are affected at most once: they receive the dedicated “Another guest was rude to me” thought and the host-configured happiness-target penalty. The check and affected-guest tracking exist only while an agitator is active.

### Saboteur

The attacker selects an open ride that supports breakdowns. A named operative visits and rides that exact attraction normally. Its native ride-exit hook schedules a breakdown type supported by the ride, completes the assignment, and sends the operative home.

### Hitman

A named operative behaves normally until an eligible ordinary guest comes close. The hitman uses the handheld guest-camera animation—not an on-ride-photo flash—then marks that exact guest for OpenRCT2's existing explosion effect. Exactly one normal single-accident park-rating consequence is applied and the hitman leaves. Competitive operatives, departing guests, and guests currently on rides are not eligible victims.

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

## Known remaining work

- Optional relayed discovery/transport for hosts who cannot accept an inbound connection.
- Live host migration, if a safe authority election and reachable replacement endpoint can be designed.
- Playtesting and balance passes for action prices, cooldowns, strengths, and time limits.
