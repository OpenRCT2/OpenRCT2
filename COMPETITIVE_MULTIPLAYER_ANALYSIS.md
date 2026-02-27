# Competitive Multiplayer Analysis for OpenRCT2

## Executive Summary

OpenRCT2 **already has extensive multiplayer functionality** including:
- ✅ Client-Server networking architecture
- ✅ Player management and permissions system
- ✅ Real-time synchronization
- ✅ Master server for server discovery
- ✅ Authentication and security
- ✅ Chat system
- ✅ Action tracking per player

**However**, it **LACKS** dedicated competitive features such as:
- ❌ Separate player-owned parks/zones
- ❌ Competition scoring/ranking system
- ❌ Win/lose conditions for multiplayer scenarios
- ❌ Real-time leaderboards
- ❌ Timed competitive challenges
- ❌ Player vs Player objectives

---

## Current Multiplayer Architecture

### 1. Network Infrastructure (`src/openrct2/network/`)

**Files:**
- `NetworkBase.h/cpp` - Core networking logic (2,500+ lines)
- `NetworkServer.h/cpp` - Server-side implementation
- `NetworkClient.h/cpp` - Client-side implementation
- `NetworkPlayer.h/cpp` - Player data and tracking
- `NetworkConnection.h/cpp` - Connection handling
- `NetworkPacket.h/cpp` - Data serialization
- `ServerList.h/cpp` - Server browser
- `NetworkServerAdvertiser.h/cpp` - Master server integration

**Key Features:**
```cpp
// Current player tracking (NetworkPlayer.h)
class Player {
    uint8_t Id;
    std::string Name;
    uint16_t Ping;
    uint8_t Group;              // Permission group
    money64 MoneySpent;         // Total money spent by player
    uint32_t CommandsRan;       // Number of actions performed
    int32_t LastAction;         // Last game action
    CoordsXYZ LastActionCoord;  // Location of last action
    std::string KeyHash;        // Authentication
};
```

**Network Commands:**
- Authentication & Authorization
- Map synchronization
- Chat messages
- Game actions (building, demolishing, etc.)
- Tick synchronization (real-time state sync)
- Player list updates
- Ping/latency monitoring

### 2. Game State Management

**Current Synchronization:**
```cpp
// From NetworkBase.cpp
void NetworkBase::SendTick() {
    // Sends game tick to all clients for synchronization
    // Includes checksums for desync detection
}

bool NetworkBase::CheckDesynchronizaton() {
    // Detects if clients are out of sync
    // Uses SRAND checksums
}
```

**Limitations for Competition:**
- Single shared game state (all players affect same park)
- No isolation between players
- Collaborative mode only

### 3. Scenario & Objectives System

**Existing Objectives (`src/openrct2/scenario/ScenarioObjective.h`):**
```cpp
enum class ObjectiveType : uint8_t {
    none,
    guestsBy,                    // Reach X guests by year Y
    parkValueBy,                 // Reach park value by year Y
    haveFun,                     // Sandbox mode
    buildTheBest,                // Build best ride
    tenRollercoasters,           // Build 10 coasters
    guestsAndRating,             // Guests + park rating
    monthlyRideIncome,           // Monthly income target
    tenRollercoastersLength,     // 10 coasters with length
    finishFiveRollercoasters,    // 5 coasters with excitement
    repayLoanAndParkValue,       // Financial goal
    monthlyFoodIncome,           // Food income target
};
```

**Current Limitation:**
- Objectives are **per-scenario**, not **per-player**
- No multi-player objective tracking
- No competitive win conditions

### 4. Player Statistics Tracking

**Currently Tracked:**
```cpp
// Per-player metrics (already available)
- money64 MoneySpent          // Total construction costs
- uint32_t CommandsRan        // Number of actions
- LastAction & LastActionTime // Recent activity
- CoordsXYZ LastActionCoord   // Where they're building
```

**Park-wide Statistics:**
```cpp
// Available from Park.cpp
- money64 CalculateParkValue()
- money64 CalculateCompanyValue()
- Guest count, happiness, ratings
- Ride values and income
```

---

## What Needs to be Added for Competitive Multiplayer

### 1. **Player-Owned Zones/Territories System** 🆕

**Concept:** Divide the map into territories that each player owns and develops.

**Implementation Requirements:**

```cpp
// New file: src/openrct2/world/PlayerTerritory.h
class PlayerTerritory {
public:
    uint8_t OwnerId;                    // Player who owns this territory
    std::vector<TileCoordsXY> Tiles;    // Tiles in this territory
    money64 TerritoryValue;             // Value of this territory
    uint32_t GuestCount;                // Guests in this territory
    RatingFixed TerritoryRating;        // Average rating
    
    bool CanPlayerBuildHere(uint8_t playerId, TileCoordsXY coord);
    void UpdateTerritoryStats();
    money64 CalculateTerritoryValue();
};

// Extend NetworkPlayer with competitive stats
class Player {
    // Existing fields...
    
    // NEW COMPETITIVE FIELDS:
    std::unique_ptr<PlayerTerritory> Territory;
    CompetitiveStats Stats;
    uint32_t Score;
    uint32_t Rank;
};
```

**Required Changes:**
- **Game Actions:** Modify all construction actions to check territory ownership
- **Rendering:** Add visual boundaries showing player territories
- **Permissions:** Restrict players to only build in their territory
- **Map Generation:** Create pre-divided competitive maps

### 2. **Competitive Scoring System** 🆕

**New File:** `src/openrct2/competition/CompetitionManager.h`

```cpp
enum class CompetitionType : uint8_t {
    ParkValue,           // Highest park value wins
    GuestCount,          // Most guests wins
    Happiness,           // Highest guest happiness
    RideExcitement,      // Best rides win
    Profit,              // Most profitable
    Speed,               // Fastest to objective
    Hybrid,              // Multiple criteria
};

struct CompetitiveStats {
    money64 TotalValue;
    money64 TotalProfit;
    uint32_t TotalGuests;
    uint32_t HighestRating;
    money64 TotalIncome;
    money64 TotalExpenses;
    uint32_t RidesBuilt;
    RatingFixed BestRideExcitement;
    std::chrono::seconds TimeElapsed;
};

class CompetitionManager {
public:
    void StartCompetition(CompetitionType type, uint32_t durationMinutes);
    void UpdatePlayerScores();
    void CheckWinConditions();
    void BroadcastLeaderboard();
    void EndCompetition();
    
private:
    std::vector<PlayerCompetitionData> _playerData;
    CompetitionType _type;
    std::chrono::steady_clock::time_point _startTime;
    uint32_t _duration;
    bool _active;
};
```

**Integration Points:**
- Hook into `NetworkBase::Tick()` to update scores every game tick
- Track per-player financial transactions
- Monitor ride construction and ratings per player
- Broadcast score updates to all clients

### 3. **Real-Time Leaderboard System** 🆕

**New File:** `src/openrct2-ui/windows/Leaderboard.cpp`

```cpp
class LeaderboardWindow : public Window {
private:
    struct LeaderboardEntry {
        uint8_t PlayerId;
        std::string PlayerName;
        uint32_t Score;
        uint32_t Rank;
        money64 ParkValue;
        uint32_t GuestCount;
        // ... other stats
    };
    
    std::vector<LeaderboardEntry> _entries;
    
public:
    void RefreshLeaderboard();
    void SortByScore();
    void DrawLeaderboard(RenderTarget& rt);
};

// Add to Network.h
namespace OpenRCT2::Network {
    void SendLeaderboardUpdate();
    void BroadcastScoreChange(uint8_t playerId, uint32_t newScore);
}
```

**Network Packet:**
```cpp
// Add to NetworkTypes.h
enum class Command : uint32_t {
    // ... existing commands
    leaderboardUpdate,    // New: Send leaderboard data
    scoreUpdate,          // New: Individual score change
    competitionStart,     // New: Start competition
    competitionEnd,       // New: End competition with results
};
```

### 4. **Competition Game Modes** 🆕

**New File:** `src/openrct2/competition/CompetitionModes.h`

```cpp
// Race Mode: First to achieve objective wins
class RaceMode : public CompetitionMode {
public:
    ObjectiveType Objective;
    std::optional<uint8_t> Winner;
    
    void CheckPlayerObjective(uint8_t playerId);
    void DeclareWinner(uint8_t playerId);
};

// Timed Mode: Highest score when timer expires
class TimedMode : public CompetitionMode {
public:
    uint32_t DurationSeconds;
    std::chrono::steady_clock::time_point EndTime;
    
    void Update();
    void OnTimeExpired();
};

// Survival Mode: Last player standing
class SurvivalMode : public CompetitionMode {
public:
    money64 MinimumCash;
    
    void CheckBankruptcy();
    void EliminatePlayer(uint8_t playerId);
};

// Tournament Mode: Multiple rounds, elimination
class TournamentMode : public CompetitionMode {
public:
    uint32_t RoundNumber;
    std::vector<uint8_t> EliminatedPlayers;
    
    void StartNextRound();
    void EliminateLowestScorers(uint32_t count);
};
```

### 5. **Enhanced Network Protocol** 🆕

**Extended Network Commands:**

```cpp
// Add to NetworkBase.h
void ServerSendCompetitionStart(const CompetitionConfig& config);
void ServerSendScoreUpdate(uint8_t playerId, const CompetitiveStats& stats);
void ServerSendLeaderboard(const std::vector<LeaderboardEntry>& entries);
void ServerSendCompetitionEnd(const CompetitionResults& results);
void ServerSendTerritoryInfo();

void ClientHandleCompetitionStart(Connection& connection, Packet& packet);
void ClientHandleScoreUpdate(Connection& connection, Packet& packet);
void ClientHandleLeaderboard(Connection& connection, Packet& packet);
void ClientHandleCompetitionEnd(Connection& connection, Packet& packet);
```

### 6. **Competitive Map Types** 🆕

**New File:** `src/openrct2/world/CompetitiveMapGenerator.h`

```cpp
class CompetitiveMapGenerator {
public:
    struct TerritoryConfig {
        uint32_t NumPlayers;
        ScreenSize TerritorySize;
        uint32_t SeparationDistance;
        bool BalancedResources;
    };
    
    // Generate fair, divided maps for competition
    void GenerateDividedMap(TerritoryConfig config);
    void AssignTerritoriesToPlayers();
    void BalanceStartingConditions();
};
```

### 7. **Competition Lobby System** 🆕

**New File:** `src/openrct2-ui/windows/CompetitionLobby.cpp`

```cpp
class CompetitionLobby : public Window {
private:
    CompetitionConfig _config;
    std::vector<PlayerReadyStatus> _players;
    
public:
    void SetCompetitionType(CompetitionType type);
    void SetDuration(uint32_t minutes);
    void SetMaxPlayers(uint32_t count);
    void SetMapType(std::string mapName);
    void TogglePlayerReady(uint8_t playerId);
    void StartCountdown();
    void LaunchCompetition();
};

// Network integration
namespace OpenRCT2::Network {
    void ServerSendLobbyUpdate(const CompetitionConfig& config);
    void ServerStartCompetitionCountdown(uint32_t seconds);
}
```

---

## Implementation Roadmap

### Phase 1: Foundation (2-4 weeks)
1. ✅ Design territory system architecture
2. ✅ Extend `NetworkPlayer` with competitive stats
3. ✅ Create `CompetitionManager` core class
4. ✅ Add new network packet types
5. ✅ Implement basic score tracking

### Phase 2: Territory System (3-4 weeks)
1. Implement `PlayerTerritory` class
2. Modify game actions to check territory ownership
3. Add visual territory boundaries
4. Create territory assignment system
5. Test territory validation

### Phase 3: Scoring & Leaderboard (2-3 weeks)
1. Implement `CompetitiveStats` tracking
2. Create scoring algorithms for different modes
3. Build real-time leaderboard UI
4. Add network synchronization for scores
5. Test score accuracy

### Phase 4: Competition Modes (3-4 weeks)
1. Implement Race Mode
2. Implement Timed Mode
3. Implement Survival Mode
4. Add win/loss conditions
5. Create competition end-game UI

### Phase 5: UI & Polish (2-3 weeks)
1. Create Competition Lobby UI
2. Add in-game competition HUD
3. Implement player notifications
4. Add competition statistics screen
5. Create replay/spectator features

### Phase 6: Maps & Balance (2-3 weeks)
1. Create competitive map templates
2. Implement map generator for balanced territories
3. Balance starting conditions
4. Add scenario editor for competitive maps
5. Test various player counts

### Phase 7: Testing & Refinement (2-4 weeks)
1. Network stress testing
2. Balance testing
3. Bug fixes
4. Performance optimization
5. Documentation

**Total Estimated Time: 16-25 weeks (4-6 months)**

---

## Technical Challenges

### 1. **State Synchronization**
- **Challenge:** Keeping per-player statistics synchronized across all clients
- **Solution:** Extend existing tick-based synchronization with player-specific packets

### 2. **Territory Validation**
- **Challenge:** Preventing players from building outside their territory
- **Solution:** Modify all `GameAction` classes to validate territory ownership before execution

### 3. **Fair Resource Distribution**
- **Challenge:** Ensuring each player starts with equal opportunities
- **Solution:** Procedural map generation with balanced terrain, entrance placement, and starting resources

### 4. **Performance**
- **Challenge:** Additional per-player calculations may impact performance
- **Solution:** Calculate scores asynchronously, cache results, update every N ticks

### 5. **Backward Compatibility**
- **Challenge:** Maintaining compatibility with standard multiplayer mode
- **Solution:** Make competitive mode opt-in, keep standard cooperative mode as default

---

## Key Files to Modify

### Core Network Files:
- `src/openrct2/network/NetworkBase.h/cpp` - Add competition commands
- `src/openrct2/network/NetworkPlayer.h/cpp` - Add competitive stats
- `src/openrct2/network/NetworkTypes.h` - Add competition enums/structs

### New Files to Create:
- `src/openrct2/competition/CompetitionManager.h/cpp`
- `src/openrct2/competition/CompetitionModes.h/cpp`
- `src/openrct2/competition/CompetitiveStats.h/cpp`
- `src/openrct2/world/PlayerTerritory.h/cpp`
- `src/openrct2/world/CompetitiveMapGenerator.h/cpp`
- `src/openrct2-ui/windows/Leaderboard.cpp`
- `src/openrct2-ui/windows/CompetitionLobby.cpp`

### UI Files to Modify:
- `src/openrct2-ui/windows/Multiplayer.cpp` - Add competition tab
- `src/openrct2-ui/windows/Windows.h` - Add window types

### Game Action Files:
- `src/openrct2/actions/*` - Add territory validation to all construction actions

---

## Example Code Snippets

### Starting a Competition

```cpp
// Server-side
void NetworkBase::StartCompetition(CompetitionConfig config) {
    auto* competitionMgr = GetContext()->GetCompetitionManager();
    
    // Initialize territories
    for (auto& player : player_list) {
        auto territory = std::make_unique<PlayerTerritory>();
        territory->OwnerId = player->Id;
        territory->Tiles = GeneratePlayerTerritory(player->Id);
        player->Territory = std::move(territory);
    }
    
    // Start competition
    competitionMgr->StartCompetition(config.Type, config.Duration);
    
    // Notify all clients
    ServerSendCompetitionStart(config);
}
```

### Updating Scores

```cpp
// In GameState tick update
void CompetitionManager::Tick() {
    if (!_active) return;
    
    // Update every 100 ticks (about 2.5 seconds)
    if (GetCurrentTick() % 100 == 0) {
        for (auto& player : Network::GetPlayers()) {
            UpdatePlayerScore(player);
        }
        
        BroadcastLeaderboard();
        CheckWinConditions();
    }
}

void CompetitionManager::UpdatePlayerScore(Player* player) {
    auto& stats = player->Stats;
    
    // Calculate based on competition type
    switch (_type) {
        case CompetitionType::ParkValue:
            stats.TotalValue = CalculatePlayerParkValue(player);
            player->Score = stats.TotalValue / 100; // Convert to score
            break;
            
        case CompetitionType::GuestCount:
            stats.TotalGuests = CountGuestsInTerritory(player->Territory.get());
            player->Score = stats.TotalGuests;
            break;
            
        // ... other types
    }
}
```

### Territory Validation

```cpp
// In GameAction::Execute()
bool GameAction::ValidateTerritoryOwnership() {
    if (!Network::IsCompetitionActive()) {
        return true; // No restrictions in normal mode
    }
    
    auto* player = Network::GetPlayerByID(GetPlayerId());
    if (!player || !player->Territory) {
        return false;
    }
    
    // Check if action location is within player's territory
    auto actionCoords = GetActionLocation();
    return player->Territory->CanPlayerBuildHere(player->Id, actionCoords);
}
```

---

## Alternative Approaches

### Approach 1: Separate Server Instances Per Player
- Each player gets their own park instance
- Scores compared at the end
- **Pros:** Simpler, no territory system needed
- **Cons:** Less interactive, no real-time competition

### Approach 2: Plugin/Script System
- Use OpenRCT2's plugin API to implement competition
- **Pros:** No core code changes
- **Cons:** Limited access to internals, performance issues

### Approach 3: Turn-Based Competition
- Players take turns in timed segments
- **Pros:** Easier to implement, fair time allocation
- **Cons:** Less exciting, longer games

**Recommended:** Approach in this document (real-time territories)

---

## Conclusion

OpenRCT2 has a **robust multiplayer foundation** but requires significant additions for competitive gameplay:

✅ **Strengths:**
- Solid network infrastructure
- Player tracking system
- Synchronization framework
- Extensible game action system

❌ **Missing Components:**
- Player territories/zones
- Competitive scoring system
- Leaderboards
- Competition-specific game modes
- Win/loss conditions

**Feasibility:** ⭐⭐⭐⭐ (4/5)
The existing architecture is well-suited for extension. Most competitive features can be added without major refactoring.

**Effort Required:** Medium-High (4-6 months for full implementation)

**Recommended Next Steps:**
1. Prototype territory system in a branch
2. Implement basic scoring for one competition type
3. Add simple leaderboard UI
4. Test with small player groups
5. Iterate based on feedback
