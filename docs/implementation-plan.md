# Competition System — Implementation Plan for End-to-End Playability

**Status:** Planned | **Author:** Zoro — Mr. Vyas's AI Agent | **Date:** April 7, 2026

---

## Overview

The competition engine back-end and plugin API are production-ready — scoring, lifecycle, and network broadcast all work. The remaining gaps are on the **presentation and persistence** side: no UI window, no save/load persistence, missing JS property getters, and incorrect client status propagation.

This plan closes every gap to make the competition system **fully playable in a real multiplayer session**.

---

## Phase 1 — Engine Fixes

> Sequential. Blocks Phase 3 (UI plugin depends on `name` / `metric` getters).

### Step 1: Add `name` getter to ScCompetition

The `CompetitionState.Name` field exists but is not exposed to JavaScript. Plugins cannot display the competition title.

| File | Change |
|---|---|
| `src/openrct2/scripting/bindings/game/ScCompetition.hpp` | Add declaration: `static JSValue name_get(JSContext*, JSValue)` |
| `src/openrct2/scripting/bindings/game/ScCompetition.cpp` | Implement `name_get` reading `getGameState().Competition.Name` via `JSFromStdString`; register `JS_CGETSET_DEF("name", ScCompetition::name_get, nullptr)` |
| `distribution/openrct2.d.ts` | Add `readonly name: string;` to the `Competition` interface |

### Step 2: Add `metric` getter to ScCompetition

Same gap as `name` — plugins have no way to know what metric a competition is using.

| File | Change |
|---|---|
| `src/openrct2/scripting/bindings/game/ScCompetition.hpp` | Add declaration: `static JSValue metric_get(JSContext*, JSValue)` |
| `src/openrct2/scripting/bindings/game/ScCompetition.cpp` | Implement `metric_get` using a reverse of `MetricFromString()` to return the metric as a string (`"parkValue"`, `"guestCount"`, etc.); register `JS_CGETSET_DEF("metric", ScCompetition::metric_get, nullptr)` |
| `distribution/openrct2.d.ts` | Add `readonly metric: string;` to the `Competition` interface |

### Step 3: Propagate `Finished` status to clients

Currently `Client_Handle_COMPETITION_UPDATE()` blindly sets `comp.Status = Active` regardless of the actual server-side state. When the server stops or expires a competition, clients never learn it's finished.

| File | Change |
|---|---|
| `src/openrct2/network/NetworkBase.cpp` — `ServerSendCompetitionUpdate()` (~line 4098) | Add `uint8_t statusByte = static_cast<uint8_t>(comp.Status)` to the packet, serialized before the existing name/scores fields |
| `src/openrct2/network/NetworkBase.cpp` — `Client_Handle_COMPETITION_UPDATE()` (~line 4126) | Read `statusByte` from the packet; set `comp.Status` from it instead of hardcoding `Active`; fire `competitionEnd` hook when status is `Finished`, fire `competitionTick` otherwise |

---

## Phase 2 — Save/Load Serialization

> Parallel with Phase 3. Independent of UI work.

### Step 4: Serialize CompetitionState in ParkFile

Competition state embedded in `GameState_t` is not wired into the `.park` file serializer. If the server saves and reloads mid-competition, all state (name, scores, timer) is lost.

| File | Change |
|---|---|
| `src/openrct2/park/ParkFile.cpp` — chunk constants (~line 105) | Add `constexpr uint32_t COMPETITION = 0x3A;` (after `PREVIEW = 0x39`) |
| `src/openrct2/park/ParkFile.cpp` — new function | Create `ReadWriteCompetitionChunk(GameState_t& gameState, OrcaStream& os)` following the `ReadWriteParkChunk()` pattern |
| `src/openrct2/park/ParkFile.cpp` — `Import()` (~line 174) | Call `ReadWriteCompetitionChunk(gameState, os)` after `ReadWritePluginStorageChunk()` |
| `src/openrct2/park/ParkFile.cpp` — `Save()` (~line 210) | Call `ReadWriteCompetitionChunk(gameState, os)` after `ReadWritePluginStorageChunk()` |

**Chunk fields to serialize:**

```
Name              (string)
Status            (uint8_t)
Metric            (uint8_t)
DurationTicks     (uint32_t)
StartTick         (uint32_t)
UpdateIntervalTicks (uint32_t)
Scores count      (uint8_t)
  For each score:
    PlayerId      (uint8_t)
    PlayerName    (string)
    Score         (int64_t)
    Rank          (int32_t)
```

**Guards:** Entire chunk wrapped in `#ifdef ENABLE_SCRIPTING`.

**Backward compatibility:** Old parks simply skip the unknown `0x3A` chunk. New parks with no competition write an empty/idle chunk.

---

## Phase 3 — UI Plugin

> Parallel with Phase 2. Depends on Phase 1 (needs `name` and `metric` getters).

### Step 5: Rewrite competition-example plugin with full UI

Replace the current chat-only reference plugin with a polished leaderboard window.

**File:** `resources/competition-example/plugin.js`

#### 5a. Menu & Shortcut Registration

```javascript
ui.registerMenuItem("Competition", function() { toggleCompetitionWindow(); });
ui.registerShortcut({
    id: "competition:toggle",
    text: "Toggle Competition Window",
    bindings: ["CTRL+SHIFT+C"],
    callback: function() { toggleCompetitionWindow(); }
});
```

#### 5b. Leaderboard Window

```
┌─────────────────────────────────────────┐
│  Competition: Park Value Race           │
│  Status: ● Active    Time: 3d 14h 22m  │
├─────────────────────────────────────────┤
│  #  │ Player          │ Score           │
│  1  │ Alice           │ 1,250,000       │
│  2  │ Bob             │ 1,100,000       │
│  3  │ Charlie         │ 980,000         │
├─────────────────────────────────────────┤
│  [ Start ]  [ Stop ]                    │  ← server-only
└─────────────────────────────────────────┘
```

**Widgets:**

| Widget | Type | Purpose |
|---|---|---|
| Competition name | `label` | Displays `competition.name` |
| Status badge | `label` | Colour-coded: green=active, grey=idle, red=finished |
| Time remaining | `label` | Countdown updated in `onUpdate` callback |
| Leaderboard | `listview` | Columns: Rank, Player, Score — striped, scrollable, column headers shown |
| Start button | `button` | Opens start dialog; hidden when `network.mode !== 'server'` |
| Stop button | `button` | Calls `competition.stop()`; hidden when not server |

**Auto-refresh:** The `onUpdate` callback (fires every UI tick ~40ms) reads `competition.leaderboard`, `competition.timeRemaining`, and `competition.status` to update all labels and the listview items.

#### 5c. Start Dialog (Server Only)

Second window opened by the "Start" button:

| Widget | Type | Purpose |
|---|---|---|
| Name | `textbox` | Competition name (default: "Park Value Race") |
| Metric | `dropdown` | Items: parkValue, guestCount, parkRating, rideCount, moneyEarned, custom |
| Duration (days) | `spinner` | Integer days (default: 10, range 1–365) |
| Update interval | `spinner` | Ticks between broadcasts (default: 100) |
| Go button | `button` | Calls `competition.start({...})` and closes dialog |

#### 5d. Hook Subscriptions

| Hook | Behaviour |
|---|---|
| `competition.start` | Auto-open the leaderboard window; show notification |
| `competition.end` | Update status to "Finished"; announce winner via `park.postMessage()` |
| `competition.tick` | (Handled by `onUpdate` auto-refresh; optionally log to chat) |

#### 5e. Client Compatibility

- Works on both server and client
- Start/Stop buttons hidden when `network.mode !== 'server'`
- Single-player: all features work, just one player on the leaderboard

---

## Phase 4 — Build & Test

> Sequential. Depends on all previous phases.

### Step 6: Rebuild and run automated tests

```powershell
msbuild openrct2.sln /t:Build /p:Configuration=Debug /p:Platform=x64 /m
.\bin\tests.exe --gtest_filter="Competition*"    # 13/13 must pass
.\bin\tests.exe --gtest_filter="Competition*:S6Exporter*:Language*:Sawyerenc*:FormattedString*:String*:CodepointView*"  # Full non-park suite
```

### Step 7: Manual multiplayer smoke test

| Test | Expected Result |
|---|---|
| Launch server (`openrct2.exe host`) | Server starts, plugin loaded |
| Connect client | Client joins, sees menu item "Competition" |
| Server clicks Start → fills dialog → Go | Competition starts; client window auto-opens with leaderboard |
| Wait for ticks | Time countdown updates on both server and client; scores refresh |
| Server clicks Stop | Both server and client see status "Finished"; winner announced |
| Server saves mid-competition, reloads | Competition state restored: name, scores, timer intact |

### Step 8: Update documentation

- Update LaTeX document (`docs/pr-competition-system.tex`) with new files, UI description, updated changeset table
- Update markdown document (`docs/pr-competition-system.md`) to match

---

## Files Summary

### Modify

| File | Changes |
|---|---|
| `src/openrct2/scripting/bindings/game/ScCompetition.hpp` | Add `name_get`, `metric_get` declarations |
| `src/openrct2/scripting/bindings/game/ScCompetition.cpp` | Implement getters, register in `Register()` |
| `src/openrct2/network/NetworkBase.cpp` | Add status byte to packet send/receive; fix client status handling |
| `src/openrct2/park/ParkFile.cpp` | New `COMPETITION` chunk constant + `ReadWriteCompetitionChunk()` |
| `distribution/openrct2.d.ts` | Add `name`, `metric` readonly properties to `Competition` interface |
| `resources/competition-example/plugin.js` | Full rewrite with UI window, start dialog, hooks, menu item |

### Reference (patterns to follow)

| Pattern | File | Purpose |
|---|---|---|
| Save/load chunk | `ParkFile.cpp` → `ReadWriteParkChunk()` | Serialization via `cs.readWrite()` |
| Client hook firing | `NetworkBase.cpp` → `Client_Handle_CHAT()` | Hook call after packet processing |
| Plugin window | `openrct2.d.ts` → `WindowDesc` interface | `ui.openWindow()` widget structure |

---

## Verification Checklist

- [ ] `msbuild` — 0 errors
- [ ] `tests.exe --gtest_filter="Competition*"` — 13/13 pass
- [ ] Full non-park test suite — 46/46 pass, zero regressions
- [ ] Manual: server + client, start competition, leaderboard populates on both sides
- [ ] Manual: save mid-competition, reload, state restored
- [ ] Manual: stop competition → clients see "Finished" status
- [ ] Manual: menu item "Competition" appears, keyboard shortcut CTRL+SHIFT+C works

---

## Key Design Decisions

| Decision | Rationale |
|---|---|
| UI is a **plugin** (not native C++ window) | Consistent with plugin-driven design; faster to iterate; no engine recompile needed |
| Save/load uses **new chunk type** `0x3A` | Backward compatible — older builds skip unknown chunks |
| Status byte added to network packet | Breaking change for in-flight packets — acceptable since the feature is unreleased |
| `name` and `metric` are **read-only** getters | Set via `start()` options; no setter needed |

---

*Authored by Zoro — Mr. Vyas's AI Agent | OpenRCT2 Competition System | [github.com/yashvyas95/OpenRCT2](https://github.com/yashvyas95/OpenRCT2)*
