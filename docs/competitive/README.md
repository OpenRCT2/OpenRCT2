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
- Target matches finish when a park first reaches the target score, with the deadline as a fallback.
- The normal scenario objective does not decide the competitive result.
- The host may forfeit an offline, unfinished park or close the result early when an abandoned player will not return.
- **Optional real-time limit.** The host can set a wall-clock cap as a secondary ender. It counts
  only real time while the match is actually live (host playing, unpaused, connected) - a host pause
  or suspend freezes it - and when it elapses every unfinished park's score is frozen and the winner
  calculated, exactly like "close early".

## Scoring

The competition score is a weighted blend of genuine park stats, not a bespoke metric. The host
assigns a percentage weight (totalling 100%) to any of: **park rating, guest happiness, guest
count, park value, cash**. Each match, each weighted stat is normalised to the current leader on
that stat (your value / the best value = 0..1), the normalised values are blended by weight, and
the result is scaled to a **0-1000 score**. A single stat at 100% is just "highest <stat> wins" and
the leaderboard shows that stat's raw value instead of a 0-1000 score. Because the score is
relative to the field, a park's displayed score can fall when a rival pulls ahead. Spectators and
forfeited parks are excluded from the normalisation and score 0. Ties are broken by the earlier
(lower) participant id. In target mode, `target` is a score on the same 0-1000 scale.

## Economy and fair play

There is no parallel competitive currency. In money scenarios, the park's real cash pays for both ordinary play and rival actions, and the cash victory metric reads that same balance. In no-money scenarios, rival actions have no cash charge and are governed by their host-configured frequency limits (minimum gap and uses per year) and effect limits. Both money and no-money scenarios are supported.

While an unfinished competitor is in the lobby or a running match, OpenRCT2 cheat state is reset continuously. Cheat, scenario-editing, tile-editing, date-changing, map-resizing, peep-spawn, and ride-rating-freeze actions are rejected, cheat/debug toolbar access is hidden, and the configured speed cap is enforced in the game action itself. This is fair-play enforcement, not cryptographic anti-tamper against a deliberately modified executable.

## Current rival actions

Every action has a host-configurable enable switch and real park-cash price. Actions with a continuous effect (misinformation, poisoning, agitator) also expose a host-set victim-local duration; single-shot operatives (toilet bomber, saboteur, hitman) act once and then leave, so they have only a hidden give-up backstop, not a host knob. Strength is configurable where the effect has a variable strength. The real park-cash price is charged when the action is submitted, refunded if delivery fails, and retained only after the online victim acknowledges it. Prices are ignored in no-money scenarios.

Each action also has two independent host-configured frequency limits:

- **Minimum gap** — the shortest attacker-local-day wait between two uses of the action. It starts on the victim's acknowledgement of the previous use. Adjusted in whole in-game months (an in-game year is 8 months / 256 competitive days).
- **Uses per year** — a hard cap on how many times a participant may use the action per attacker-local in-game year. The counter resets when the participant's local year rolls over. Set to unlimited to rely on the minimum gap alone.

Misinformation is the one exception to host-set price and duration: it is always a fixed four-week campaign priced at 1.5x the equivalent four-week half-price-entry marketing campaign.

Every named operative or infiltrating guest sent by a rival action - including the Karen and Stoner group guests - is a "bad actor": it cannot be picked up with the pincers tool and it ignores the victim's no-entry path signs, so no-entry signs cannot be used to fence rival guests out.

Most actions can be piled on: several allied players can each run a vandal, agitator, hitman, misinformation campaign, or Karen/Stoner party against the same rival at once, and stacked misinformation strengths add up. Per-ride actions (poisoning, toilet bomber, saboteur) still clash only on the same ride, and research sabotage and union disruption cannot stack because they toggle a single piece of park state.

Open the in-park multiplayer window to reach the competition leaderboard. The **Attack rival...** button is visible but disabled in the lobby, where its tooltip explains that attacks begin with the match. During a running match, select an online, unfinished rival in the leaderboard, then choose from the configured actions. When no valid rival is selected, the disabled button explains what is required instead of disappearing.

### Vandal

The victim receives a named angry guest governed by normal path-addition vandalism and normal security-guard range. Default potency is four attempts. A successful break or an attempt stopped by security consumes one attempt, so security prevents damage and sends the vandal home sooner. The action also has a maximum victim-local lifetime.

### Misinformation

The victim is notified of an active campaign. It cancels future ordinary guest arrivals probabilistically, using a configurable reverse-equivalent of OpenRCT2's half-price-entry campaign strength. It does not move guests between parks, cap a park's natural guest count, or alter other competitors' generation. Its duration is fixed at four weeks and its price is fixed at 1.5x the equivalent four-week half-price-entry campaign; the host controls only the enable switch, strength, and the two frequency limits.

### Poisoning

The attacker selects an open food or drink stall from the victim's reported park state. Only while that exact stall is poisoned, the native successful-purchase path tests each exact food/drink buyer against the configured percentage. A selected buyer receives maximum nausea and then follows OpenRCT2's normal nausea and vomiting behaviour; there is no delayed forced-vomit timer and no proximity/customer inference. The default is a 25% chance for seven victim-local days.

The purchase hook returns immediately when there are no local effects. Full guest, ride, and stall metrics are collected only when the local day changes, not on every game update.

### Toilet Bomber

The attacker selects an open toilet reported by the victim. A named operative enters normally and heads to that exact toilet. It is a suicide bomber: once inside it detonates and dies along with every ordinary guest currently using the toilet, and the normal ride-demolition path removes the building and cleans its references. Any demolition refund is neutralised. The bomber's own death always counts, so one normal accident-equivalent park-rating consequence and a death notice apply on every detonation, empty toilet or not. If the operative never reaches the toilet it gives up and leaves after the hidden backstop with no detonation.

### Agitator

A named guest otherwise behaves normally. While the effect is active, ordinary guests sharing the agitator's tile are affected at most once: they receive the dedicated “Another guest was rude to me” thought and the host-configured happiness-target penalty. The check and affected-guest tracking exist only while an agitator is active.

### Saboteur

The attacker selects an open ride that supports breakdowns. A named operative visits and rides that exact attraction normally. Its native ride-exit hook schedules a breakdown type supported by the ride, completes the assignment, and sends the operative home.

### Research sabotage

No operative is sent. The victim's research funding is forced to "None" for the configured duration and the victim is notified. If the victim raises funding while the effect is active it is pushed straight back to None. When the effect ends, funding is restored to whatever level the victim last chose, so no manual reset is needed. Default price GBP 200, once per year, one month.

### Union disruption

No operative is sent. The victim's staff wage payments (weekly wages and the finance graph's staff-cost line) are doubled for the configured duration and the victim is notified. The default duration is deliberately longer than an in-game month so a full month of wages is doubled regardless of when in the month the effect lands. Default price GBP 250, once per year.

### Karens

A coach party (default 20, host-configurable) of guests arrives in the victim's park at once. They spawn thirsty, needing the toilet, and in a low mood, and they carry their own umbrella so they never buy one - but low happiness still makes them complain often, including in the rain. While a Karen shares a tile with a staff member, that staff member is frozen in place for about ten seconds ("demanding to speak to a manager") and cannot continue its duties; the Karen gets the matching thought. In every other respect they behave as ordinary guests and leave of their own accord. The action completes once the whole party has left; there is no host-set duration.

### Stoners

A coach party (default 20, host-configurable) of guests arrives at once, spawned hungry and lethargic (low energy). They ignore prices - they never raise the "I'm not paying that much" thought and are not deterred by low happiness - and they are far more likely than ordinary guests to stop and stare at anything: rides, food/drink stalls, or scenery, thinking their own vacant thoughts while they do. Every few minutes each Stoner puffs a small cloud of smoke; an ordinary guest sharing that tile takes a minor mood hit and thinks "It smells strongly of weed here!". They behave normally otherwise and leave of their own accord, completing the action when the party has gone.

### Hitman

A named operative behaves normally until an eligible ordinary guest comes close. The hitman uses the handheld guest-camera animation—not an on-ride-photo flash—then marks that exact guest for OpenRCT2's existing explosion effect. Exactly one normal single-accident park-rating consequence is applied and the hitman leaves. Competitive operatives, departing guests, and guests currently on rides are not eligible victims.

## Chat

Because every park is its own instance, ordinary in-park chat would only reach that park's own spectators. Competitive chat is instead relayed through the coordinator: a chat line typed in any competing park (or by an observer) is forwarded to the host and broadcast to every connected park, shown there as a `[Competition]` line. It falls back silently if a park's coordinator link is momentarily down.

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

## Internet play

For a host to be reachable over the internet (not just LAN), forward these on the host's router:

- The lobby port (default `11755`, TCP).
- The watch/spectate port (fixed default `12000`, TCP) - every solo park's local watch server tries this port first, and only falls back to a small local offset range to avoid collisions when running multiple parks on one dev machine.

### Public game list

The **Public game list** checkbox (host setup) registers the match with a master server so it appears in other players' server browser, using the same registration/heartbeat mechanism as vanilla OpenRCT2 multiplayer.

The official master server (`servers.openrct2.io`) has a fixed schema for its listing data and currently drops the extra fields a competitive listing needs (`gameMode`, `competitiveProtocol`, `matchId`, etc.) - so a competitive match registered there alone will not show up tagged as competitive on another client's Competitive tab. This branch still registers with it regardless, in the hope upstream eventually preserves those fields.

In the meantime, `tools/competitive-master-server.exe` is a minimal, standalone, self-hosted listing server that speaks the same protocol and preserves every field a competitive listing needs. It has no runtime dependencies (no Node/Python install required) - just double-click it and leave the window open while hosting. Setting `master_server_url` in `config.ini` does **not** replace the official server - both the host's advertiser and every client's server browser query the official server and the configured one together, merging results.

It listens on port `8080` by default; forward that too if you want internet clients (not just LAN) to see the listing. Every machine that wants to see or publish to it (both host and joining clients) needs the same `master_server_url` set in their `config.ini`.

`tools/competitive-master-server.js` is an equivalent Node implementation of the same protocol, kept for reference/non-Windows use - the `.exe` (built from `competitive-master-server.cpp` via `tools/build-competitive-master-server.cmd`) is the one meant for end users.

## Known remaining work

- Optional relayed discovery/transport for hosts who cannot accept an inbound connection.
- Live host migration, if a safe authority election and reachable replacement endpoint can be designed.
- Playtesting and balance passes for action prices, cooldowns, strengths, and time limits.
