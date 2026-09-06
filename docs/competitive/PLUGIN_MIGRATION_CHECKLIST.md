# Historical competitive multiplayer migration inventory

> This is an archived feature inventory from the plugin-to-engine migration. It is not current implementation or user documentation. See [README.md](README.md) for authoritative native behaviour, limitations, and proposed work. In particular, the plugin's parallel competitive-cash model was deliberately rejected: the native fork uses real park cash, while no-money scenarios use cooldowns without cash charges.

This document is the migration contract from `openrct2-competitive` v0.9.0
into the native `competitive-multiplayer` OpenRCT2 branch. A feature is not
finished merely because a window or packet exists: the player-facing behaviour,
fairness invariant, recovery behaviour, validation, and feedback must all be
present and tested.

Disposition labels:

- **PRESERVE**: retain the existing behaviour in native code.
- **IMPROVE**: retain the intent while replacing a confusing or fragile flow.
- **SUPERSEDE**: native engine support makes the plugin mechanism unnecessary.
- **REMOVE**: remove only the named implementation artefact, not its useful
  player-facing outcome.

## Product and fairness invariants

- [ ] **PRESERVE** One competitor owns one complete, independent park
  simulation and local calendar. Competitive mode must never turn the parks
  into regions of one shared map.
- [ ] **PRESERVE** A park may be played solo or by a normal OpenRCT2 multiplayer
  team without changing its competitive identity or score ownership.
- [ ] **PRESERVE** Players never wait for another park to complete a tick, day,
  month, or year.
- [ ] **PRESERVE** Pausing one park never pauses another park.
- [ ] **PRESERVE** Once a match starts, local pausing remains available. The
  optional host speed cap rejects only speeds above the configured limit.
- [ ] **PRESERVE** An offline competitor's park is paused locally; online parks
  continue normally.
- [ ] **PRESERVE** Competitive actions can target only an online, unfinished,
  non-forfeited rival and are never queued for an offline park.
- [ ] **PRESERVE** Every park retains normal OpenRCT2 guest generation. There is
  no shared town population, quota, ownership, balancing, or guest transfer.
- [ ] **PRESERVE** A deadline score is frozen at that park's configured local end
  date. Continued play cannot alter the competition result.
- [ ] **PRESERVE** A deadline match resolves when every non-forfeited competitor
  has a frozen result, unless the host explicitly uses an early-resolution
  control.
- [ ] **PRESERVE** A target race resolves as soon as the first valid report meets
  the target; the configured local deadline remains the fallback.

## Native multiplayer entry and lobby flow

- [ ] **IMPROVE** Put Cooperative and Competitive choices in OpenRCT2's native
  multiplayer browser rather than exposing a separate title-screen plugin
  window.
- [ ] **IMPROVE** Use OpenRCT2's player identity, socket implementation, LAN
  discovery, favourites, direct-connect fallback, and server-list conventions.
- [ ] **REMOVE** Eliminate the Node.js companion, localhost plugin gateway,
  hidden process launcher, and companion status language completely.
- [ ] **PRESERVE** The normal path is a proper discoverable game list; direct
  hostname/port entry is an advanced fallback, never a config-file requirement.
- [ ] **PRESERVE** Browser rows show competition name, host, phase, player count,
  victory rule, and required scenario.
- [ ] **IMPROVE** Selecting a competition opens one review/lobby flow with
  explicit **Join as Park** and **Spectate** actions.
- [ ] **IMPROVE** Host setup happens before the competition is advertised or the
  park clock can advance.
- [ ] **PRESERVE** Host setup includes park/player name and competition name.
- [ ] **PRESERVE** Roles are host, competing park, and spectator. Spectators do
  not occupy competitor seats, report metrics, receive effects, or appear in
  victory eligibility.
- [ ] **IMPROVE** The required scenario is selected once by the host and loaded
  through the native flow for joiners. Joining with another map must be
  impossible, not merely warned about later.
- [ ] **IMPROVE** Use a stable scenario/content fingerprint, not only filename
  and map dimensions. Support official, custom, Money, and No Money scenarios.
- [ ] **PRESERVE** A joining competing park starts at the same canonical local
  scenario day as the lobby; a mismatched or progressed save cannot ready up.
- [ ] **PRESERVE** Each competing park is paused as it loads into lobby/setup.
- [ ] **PRESERVE** Every competitor explicitly readies up; any lobby rule change
  clears ready state.
- [ ] **PRESERVE** Start validation requires at least two parks, all active parks
  online and ready, identical scenario identity/date, a future deadline, and a
  target above all starting values when using target mode.
- [ ] **PRESERVE** Late join is host-configurable and forbidden unless enabled.
- [ ] **IMPROVE** In-park competitive UI is the continuation of the same native
  lobby/session, never a second conflicting connection system.

## Match rules

- [ ] **PRESERVE** Victory mode: highest metric at local deadline.
- [ ] **PRESERVE** Victory mode: first to target, with local deadline fallback.
- [ ] **IMPROVE** Ranking/target metrics: points, park rating, guest count,
  actual scenario cash, and park value.
- [ ] **IMPROVE** Configurable deadline year, maximum competing parks, late
  join, and maximum game speed. Scenario cash is not replaced.
- [ ] **PRESERVE** Host can enable/disable every sabotage type independently.
- [ ] **PRESERVE** Per-action host controls for cost, source-local-year cooldown,
  potency/quota, and target-local duration.
- [ ] **IMPROVE** Starting defaults are Year 20, eight parks, no late join, and
  a 1x speed cap. There is no separate starting balance. Suggested money-scenario
  action prices remain Vandal £2,500/one year/four items/two months;
  Misinformation £1,800/one year/two weeks; Poisoning £2,200/one year/seven
  days/25%.
- [ ] **IMPROVE** Rules use normal currency formatting and controls with clear
  labels, valid ranges, disabled-state explanations, and a readable summary
  before joining.
- [ ] **PRESERVE** Rules lock when the match starts.

## Presence, reconnects, and host controls

- [ ] **PRESERVE** The roster distinguishes online, offline, ready, playing,
  finished, forfeited, and spectator states.
- [ ] **PRESERVE** A transient disconnect marks a seat offline and clears ready
  state without forfeiting it.
- [ ] **PRESERVE** Reconnecting with the same saved park/identity reclaims the
  same seat and resumes the locally forced pause correctly.
- [ ] **IMPROVE** Use native OpenRCT2 identity and persistent competitive save
  data instead of plugin-generated instance IDs stored in plugin storage.
- [ ] **PRESERVE** The host can forfeit only an offline, unfinished competitor;
  this removes the seat from victory eligibility and completion requirements.
- [ ] **PRESERVE** Forfeit and early-close controls require confirmation.
- [ ] **PRESERVE** The host can close a running competition early, freezing every
  unfinished park at its latest accepted report.
- [ ] **PRESERVE** The host can create a new lobby after a finished competition
  while retaining connected players and resetting scores/effects/readiness.
- [ ] **IMPROVE** Host departure must not corrupt or strand the in-park UI.
- [ ] **PRESERVE** Authoritative state has epoch/revision ordering and every peer
  receives a recoverable mirror.
- [ ] **IMPROVE** Replace companion endpoint probing and manual backup assignment
  with a native, clearly reported host-migration strategy. If automatic
  migration cannot be guaranteed for a topology, retain mirrored results and
  give the host an explicit safe fallback.
- [ ] **PRESERVE** A host can resolve abandoned-player cases without forcing
  active players to wait indefinitely.

## Scoring and results

- [ ] **IMPROVE** Live score data includes points, rating, guests, park value,
  actual scenario cash, local year, and frozen year.
- [ ] **PRESERVE** v0.9.0 points are cumulative daily `round(guests *
  meanHappiness / 255)` until frozen.
- [ ] **IMPROVE** Clearly label cumulative points so they are not mistaken for a
  current snapshot metric; document or replace the formula only through an
  explicit rules/version decision.
- [ ] **PRESERVE** Leaderboard ordering uses the configured metric, then points,
  then stable seat ID as deterministic tie-breakers.
- [ ] **PRESERVE** The leaderboard displays placement, park/player name,
  online/offline status, local year, configured score metric, rating, guests,
  cash, and finished/forfeited state without requiring a spatial grid.
- [ ] **PRESERVE** Final winner and early-close status are visible to competitors
  and spectators.
- [ ] **PRESERVE** Reports are monotonic/revisioned so retries and reconnects do
  not duplicate score, income, spend, cooldowns, or actions.

## Competitive economy

- [ ] **PRESERVE** Both Money and No Money scenarios are accepted; the match win
  condition takes precedence over the scenario's normal objective.
- [ ] **PRESERVE** Each competitor owns an independent, comparable competitive
  cash account.
- [ ] **PRESERVE** Competitive income ledgers count configured income per native
  guest arrival, ride customer, and stall customer.
- [ ] **PRESERVE** Successful positive construction/action costs are charged once
  using engine-level action results. Normal multiplayer teams contribute to the
  same park ledger.
- [ ] **IMPROVE** Native action interception replaces the plugin's financial
  action-name allowlist and `Player.moneySpent` inference.
- [ ] **PRESERVE** Demolition refunds are not credited unless the native economy
  implementation can account for them exactly and fairly.
- [x] **SUPERSEDE** Ordinary construction uses the scenario's normal finances;
  it is never gated by a second competitive currency.
- [ ] **IMPROVE** Native loans, entrance-fee changes, ordinary marketing, and
  research-funding changes retain their normal scenario behaviour. Competitive
  mode blocks cheats and unauthorised scenario/date manipulation.
- [x] **SUPERSEDE** Real park cash is the only balance in money scenarios.
- [ ] **PRESERVE** Action costs are reserved before sabotage delivery and commit
  only after the online victim acknowledges receipt; failed delivery refunds the
  reservation and does not begin cooldown.

## Vandal action

- [ ] **PRESERVE** Vandal targets one online rival and creates a named guest in
  the victim's park with minimum happiness and the native angry flag.
- [ ] **PRESERVE** Normal OpenRCT2 security-guard and angry-guest logic determines
  whether vandalism succeeds; no artificial security bypass is added.
- [ ] **PRESERVE** Default destruction quota is four path additions.
- [ ] **PRESERVE** Quota and maximum target-local lifetime are host-configurable.
- [ ] **IMPROVE** Count vandal damage from the exact responsible guest in engine
  code rather than attributing newly broken nearby additions heuristically.
- [ ] **PRESERVE** At quota/timeout, clear anger and send the guest home through
  normal leaving-park behaviour.
- [ ] **PRESERVE** Victim receives a notification naming the source and explaining
  that normal security behaviour applies.

## Misinformation action

- [ ] **PRESERVE** Misinformation targets one online rival and notifies the
  victim on start and end.
- [ ] **PRESERVE** Strength is the fixed inverse of a full half-price-entry
  campaign: `200 / 65,535` per victim simulation tick, independent of whether
  that park charges entry.
- [ ] **PRESERVE** A successful misinformation roll cancels a future ordinary
  native arrival; it never redistributes that guest to another park.
- [ ] **PRESERVE** Concurrent misinformation campaigns against the same park do
  not stack.
- [ ] **PRESERVE** Duration is measured against the victim's local calendar;
  default two weeks.
- [ ] **IMPROVE** Represent Misinformation in the victim's normal marketing/
  advertising UI as an active negative campaign, with source and remaining
  duration.

## Poisoning action

- [ ] **PRESERVE** The attacker chooses an open food/drink stall reported by the
  online victim park.
- [ ] **PRESERVE** The victim is notified with source park, exact stall, chance,
  and duration.
- [ ] **PRESERVE** Only a guest completing a successful purchase transaction at
  that exact ride/stall is eligible; neighbouring stalls, proximity, inventory,
  price matching, and cash-difference inference are never used.
- [x] **SUPERSEDE** The engine exposes the exact successful purchase primitive,
  including guest, ride/stall, item, price, amount paid, and voucher use.
- [ ] **PRESERVE** Only food/drink products are eligible.
- [ ] **PRESERVE** Each eligible purchase receives the configured independent
  roll; default 25%.
- [ ] **PRESERVE** A hit immediately sets current and target nausea to maximum,
  leaving normal OpenRCT2 vomiting behaviour in control.
- [ ] **PRESERVE** Duration is measured against the victim's local calendar;
  default seven days.
- [ ] **PRESERVE** Repeated poisoning of the same stall is rejected while active;
  multiple applicable effects use the strongest chance rather than stacking
  rolls.

## UI and feedback

- [ ] **IMPROVE** Use one native multiplayer browser/lobby before park load and
  one native in-game competition window after load; both are views of the same
  session state.
- [ ] **PRESERVE** In-game tabs/views cover leaderboard, rival actions, host
  controls, and connection/session status where relevant.
- [ ] **IMPROVE** Connection details are shown as normal game state (connecting,
  online, reconnecting, offline), never as infrastructure jargon.
- [ ] **IMPROVE** Rival action controls show available park cash, action
  description, cost, cooldown availability, valid online targets, poisonable
  stalls, delivery result, and active effects with end dates.
- [ ] **PRESERVE** Disabled controls always have an adjacent reason: wrong phase,
  offline target, cooldown, insufficient funds, disabled rule, full lobby,
  incompatible content, or missing selection.
- [ ] **PRESERVE** Host UI shows start-readiness problems before the Start button
  can be used.
- [ ] **IMPROVE** Avoid oversized decorative sprites, overlapping tab art, dead
  fields, duplicated settings, and grey controls with no explanation.
- [ ] **PRESERVE** Notifications use appropriate native news categories and
  link to affected guests/rides where possible.

## Persistence, compatibility, and verification

- [ ] **IMPROVE** Store competitive identity, match membership, frozen score,
  ledger baselines, applied effects, and mirrored authority state as native park
  data with a versioned schema.
- [ ] **PRESERVE** Loading a competition park automatically restores or attempts
  its session; loading an ordinary park never joins a competition accidentally.
- [ ] **PRESERVE** Expired effects and stale undelivered reservations are cleaned
  safely during recovery.
- [ ] **PRESERVE** Unknown/malformed packets, invalid metrics, invalid stalls,
  over-capacity joins, seat theft, stale epochs/revisions, and protocol/version
  mismatches are rejected without corrupting the match.
- [ ] **IMPROVE** The browser explains fork/protocol incompatibility before join.
- [ ] **PRESERVE** Core model tests cover deadline calculation, score freezing,
  target victory, deterministic winner ordering, offline targeting, scenario
  matching, misinformation probability, poison probability, and idempotent
  economy deltas.
- [ ] **IMPROVE** Add native transport, lobby validation, save/load, host-drop,
  reconnect, action-delivery, and multi-instance integration tests.
- [ ] **REMOVE** The installed JavaScript plugin and all launcher behaviour that
  automatically starts a background process once the native replacement passes
  its migration tests.
