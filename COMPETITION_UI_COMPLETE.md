# Competition Multiplayer UI Entry Point - Implementation Complete! 🎮

## Overview

Successfully added menu entries and windows for accessing the competitive multiplayer features!

---

## What Was Added

### 1. **New Window Classes**
**File:** `src/openrct2/interface/WindowClasses.h`

Added two new window classes:
```cpp
competitionLeaderboard = 140,
competitionSetup = 141,
```

### 2. **Network Menu Entries**
**File:** `src/openrct2-ui/windows/TopToolbar.cpp`

Added competition options to the network dropdown menu:
- **Competition Leaderboard** - View live rankings
- **Competition Setup** - Configure and start competitions

```cpp
enum TopToolbarNetworkDdidx {
    DDIDX_MULTIPLAYER = 0,
    DDIDX_MULTIPLAYER_RECONNECT = 1,
    DDIDX_COMPETITION_LEADERBOARD = 2,  // NEW
    DDIDX_COMPETITION_SETUP = 3,         // NEW
    TOP_TOOLBAR_NETWORK_COUNT,
};
```

### 3. **Competition Setup Window**
**File:** `src/openrct2-ui/windows/CompetitionSetup.cpp`

A full setup window with:
- Competition type dropdown (6 types)
- Duration spinner (5-180 minutes)
- Start Competition button
- View Leaderboard button
- Status display

### 4. **Competition Leaderboard Window**
**File:** `src/openrct2-ui/windows/CompetitionLeaderboard.cpp`

Updated with:
- Scrollable player list
- Rank, Player Name, Score columns
- Time remaining display
- Refresh button
- Auto-update

### 5. **String IDs**
**File:** `src/openrct2/localisation/StringIds.h`

Added 35+ new string IDs for competition UI:
```cpp
STR_COMPETITION_LEADERBOARD = 7100,
STR_COMPETITION_SETUP = 7101,
STR_NO_ACTIVE_COMPETITION = 7102,
// ... and more
```

### 6. **English Strings**
**File:** `data/language/competition-strings-en-GB.txt`

All English translations for the new strings.

### 7. **Window Manager Integration**
**File:** `src/openrct2-ui/WindowManager.cpp`

Added cases to open competition windows:
```cpp
case WindowClass::competitionLeaderboard:
    return CompetitionLeaderboardOpen();
case WindowClass::competitionSetup:
    return CompetitionSetupOpen();
```

### 8. **Window Declarations**
**File:** `src/openrct2-ui/windows/Windows.h`

Added function declarations:
```cpp
WindowBase* CompetitionLeaderboardOpen();
WindowBase* CompetitionSetupOpen();
```

---

## How to Access

### From the Game

1. **Start/Join a multiplayer game**
2. **Click the Network button** in the top toolbar (globe icon)
3. **Select from the dropdown:**
   - "Competition Leaderboard" - View rankings
   - "Competition Setup" - Configure and start a competition

### Menu Flow

```
Top Toolbar
    └─> Network Button (Globe)
        └─> Dropdown Menu
            ├─> Multiplayer (existing)
            ├─> Reconnect (existing)
            ├─> Competition Leaderboard (NEW)
            └─> Competition Setup (NEW)
```

---

## Competition Setup Window Features

### Configuration Options

1. **Competition Type** (dropdown)
   - Park Value Race
   - Guest Count Challenge
   - Ride Income Master
   - Speed Challenge
   - Survival Mode
   - Ride Master

2. **Duration** (spinner)
   - Range: 5-180 minutes
   - Step: 5 minutes
   - Default: 30 minutes

3. **Buttons**
   - Start Competition
   - View Leaderboard

### Starting a Competition

1. Open "Competition Setup" from network menu
2. Select competition type from dropdown
3. Adjust duration with spinner
4. Click "Start Competition"
5. Leaderboard window opens automatically

---

## Competition Leaderboard Window Features

### Display Information

- **Rank** - Player's current position
- **Player Name** - Network player name
- **Score** - Current competition score
- **Guest Count** - Number of guests in territory

### Additional Info

- **Time Remaining** - Displayed at bottom
- **Auto-refresh** - Updates every frame
- **Manual Refresh** - Refresh button available

---

## Files Modified/Created

### Created (4 files)
1. `src/openrct2-ui/windows/CompetitionSetup.cpp`
2. `src/openrct2-ui/windows/CompetitionLeaderboard.cpp` (updated)
3. `data/language/competition-strings-en-GB.txt`
4. This documentation file

### Modified (5 files)
1. `src/openrct2/interface/WindowClasses.h` - Added window classes
2. `src/openrct2/localisation/StringIds.h` - Added string IDs
3. `src/openrct2-ui/windows/TopToolbar.cpp` - Added menu entries
4. `src/openrct2-ui/windows/Windows.h` - Added declarations
5. `src/openrct2-ui/WindowManager.cpp` - Added window cases

---

## String Integration

To complete the string integration, add the contents of `data/language/competition-strings-en-GB.txt` to `data/language/en-GB.txt`.

---

## Testing the UI

### Quick Test Steps

1. Build the project
2. Start OpenRCT2
3. Load any scenario
4. Enable multiplayer or open in editor
5. Click Network button in toolbar
6. Verify "Competition Leaderboard" and "Competition Setup" appear
7. Open Competition Setup
8. Select a competition type
9. Click Start Competition
10. Verify Leaderboard opens

### Expected Behavior

- Dropdown shows 4 items (Multiplayer, Reconnect, Leaderboard, Setup)
- Setup window opens with type dropdown and duration spinner
- Clicking Start initializes competition
- Leaderboard shows current players and scores
- Time counts down correctly

---

## Technical Notes

### Window Lifecycle

```cpp
// Opening Competition Setup
ContextOpenWindow(WindowClass::competitionSetup);

// Opening Leaderboard
ContextOpenWindow(WindowClass::competitionLeaderboard);
```

### Competition Integration

The Setup window interfaces with CompetitionManager:
```cpp
auto* competitionMgr = ctx->GetCompetitionManager();
competitionMgr->Initialize(config);
competitionMgr->AddPlayer(playerId, playerName);
competitionMgr->InitializeTerritories(256, 256);
competitionMgr->Start();
```

---

## Future Enhancements

### Short Term
- [ ] Add competition type descriptions
- [ ] Show more detailed stats in leaderboard
- [ ] Add sound effects for competition events
- [ ] Add territory color preview

### Medium Term
- [ ] Competition history window
- [ ] Statistics viewer
- [ ] Custom objective configuration
- [ ] Team setup interface

### Long Term
- [ ] Tournament bracket view
- [ ] Spectator mode UI
- [ ] Replay viewer
- [ ] Achievement display

---

## Summary

✅ **Menu Entry Point Added**
- Network dropdown extended with competition options
- Easy access from top toolbar

✅ **Setup Window Created**
- Full configuration UI
- Type selection, duration, start button
- Integrated with CompetitionManager

✅ **Leaderboard Window Updated**
- Live rankings display
- Auto-refresh capability
- Time remaining display

✅ **String Support Added**
- 35+ new strings defined
- English translations provided

✅ **Window Manager Integrated**
- Both windows registered
- Can be opened via ContextOpenWindow

---

**Status:** ✅ **UI ENTRY POINT COMPLETE**  
**Access:** Network Menu → Competition Setup / Leaderboard  
**Ready For:** Build and Testing  

🎮 Competition UI is now accessible from the game! 🚀
