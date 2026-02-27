# Competitive Multiplayer Implementation Guide

## Quick Start Guide for Developers

This guide provides practical steps to implement competitive multiplayer features in OpenRCT2.

---

## Phase 1: Setup - Add Competition Infrastructure

### Step 1.1: Create Competition Manager

Create new files:

**File: `src/openrct2/competition/CompetitionManager.h`**

```cpp
#pragma once

#include "../core/Money.hpp"
#include <chrono>
#include <memory>
#include <vector>

namespace OpenRCT2::Competition
{
    enum class CompetitionType : uint8_t
    {
        None,
        ParkValue,      // Highest park value wins
        GuestCount,     // Most guests wins
        RideIncome,     // Highest ride income wins
        FastestGoal,    // First to achieve objective
        Survival,       // Last player standing
    };

    enum class CompetitionStatus : uint8_t
    {
        Lobby,
        Countdown,
        Active,
        Finished,
    };

    struct CompetitionConfig
    {
        CompetitionType Type = CompetitionType::None;
        uint32_t DurationMinutes = 30;
        uint32_t MaxPlayers = 8;
        bool EnableChat = true;
        bool ShowLiveScores = true;
        std::string MapName;
    };

    struct CompetitiveStats
    {
        money64 ParkValue = 0.00_GBP;
        money64 TotalProfit = 0.00_GBP;
        money64 TotalIncome = 0.00_GBP;
        money64 TotalExpenses = 0.00_GBP;
        uint32_t GuestCount = 0;
        uint32_t RidesBuilt = 0;
        uint32_t StaffCount = 0;
        RatingFixed ParkRating = 0;
        uint32_t Score = 0;
    };

    struct PlayerCompetitionData
    {
        uint8_t PlayerId;
        std::string PlayerName;
        CompetitiveStats Stats;
        uint32_t Rank = 0;
        bool IsEliminated = false;
    };

    class CompetitionManager
    {
    public:
        CompetitionManager() = default;

        // Competition lifecycle
        void Initialize(const CompetitionConfig& config);
        void Start();
        void Update();
        void End();
        void Reset();

        // Player management
        void AddPlayer(uint8_t playerId, const std::string& name);
        void RemovePlayer(uint8_t playerId);
        PlayerCompetitionData* GetPlayerData(uint8_t playerId);

        // Score management
        void UpdatePlayerStats(uint8_t playerId);
        void CalculateScores();
        void UpdateRankings();
        std::vector<PlayerCompetitionData> GetLeaderboard() const;

        // State queries
        bool IsActive() const { return _status == CompetitionStatus::Active; }
        CompetitionStatus GetStatus() const { return _status; }
        CompetitionType GetType() const { return _config.Type; }
        uint32_t GetTimeRemaining() const;
        uint8_t GetWinner() const { return _winnerId; }

    private:
        CompetitionConfig _config;
        CompetitionStatus _status = CompetitionStatus::Lobby;
        std::vector<PlayerCompetitionData> _playerData;
        std::chrono::steady_clock::time_point _startTime;
        std::chrono::steady_clock::time_point _endTime;
        uint8_t _winnerId = 0;

        void CheckWinConditions();
        void CalculateParkValueScore(PlayerCompetitionData& data);
        void CalculateGuestCountScore(PlayerCompetitionData& data);
        void CalculateRideIncomeScore(PlayerCompetitionData& data);
    };

} // namespace OpenRCT2::Competition
```

**File: `src/openrct2/competition/CompetitionManager.cpp`**

```cpp
#include "CompetitionManager.h"
#include "../Context.h"
#include "../GameState.h"
#include "../network/Network.h"
#include "../world/Park.h"

namespace OpenRCT2::Competition
{
    void CompetitionManager::Initialize(const CompetitionConfig& config)
    {
        _config = config;
        _status = CompetitionStatus::Lobby;
        _playerData.clear();
        _winnerId = 0;
    }

    void CompetitionManager::Start()
    {
        _status = CompetitionStatus::Active;
        _startTime = std::chrono::steady_clock::now();
        _endTime = _startTime + std::chrono::minutes(_config.DurationMinutes);

        // Initialize all player stats
        for (auto& data : _playerData)
        {
            data.Stats = CompetitiveStats{};
            data.Rank = 0;
            data.IsEliminated = false;
        }
    }

    void CompetitionManager::Update()
    {
        if (_status != CompetitionStatus::Active)
            return;

        // Check if time expired
        auto now = std::chrono::steady_clock::now();
        if (now >= _endTime)
        {
            End();
            return;
        }

        // Update stats every game tick (called from network tick)
        for (auto& data : _playerData)
        {
            if (!data.IsEliminated)
            {
                UpdatePlayerStats(data.PlayerId);
            }
        }

        CalculateScores();
        UpdateRankings();
        CheckWinConditions();
    }

    void CompetitionManager::End()
    {
        _status = CompetitionStatus::Finished;
        
        // Determine winner
        if (!_playerData.empty())
        {
            auto winner = std::max_element(
                _playerData.begin(), 
                _playerData.end(),
                [](const auto& a, const auto& b) { return a.Stats.Score < b.Stats.Score; }
            );
            _winnerId = winner->PlayerId;
        }
    }

    void CompetitionManager::AddPlayer(uint8_t playerId, const std::string& name)
    {
        PlayerCompetitionData data;
        data.PlayerId = playerId;
        data.PlayerName = name;
        data.Stats = CompetitiveStats{};
        _playerData.push_back(data);
    }

    void CompetitionManager::RemovePlayer(uint8_t playerId)
    {
        _playerData.erase(
            std::remove_if(
                _playerData.begin(), 
                _playerData.end(),
                [playerId](const auto& data) { return data.PlayerId == playerId; }
            ),
            _playerData.end()
        );
    }

    PlayerCompetitionData* CompetitionManager::GetPlayerData(uint8_t playerId)
    {
        for (auto& data : _playerData)
        {
            if (data.PlayerId == playerId)
                return &data;
        }
        return nullptr;
    }

    void CompetitionManager::UpdatePlayerStats(uint8_t playerId)
    {
        auto* data = GetPlayerData(playerId);
        if (!data)
            return;

        // TODO: Calculate per-player stats (requires territory system)
        // For now, this is a placeholder that would need territory-based calculations
        
        // Example: If we had player territories
        // data->Stats.ParkValue = CalculatePlayerParkValue(playerId);
        // data->Stats.GuestCount = CountGuestsInPlayerTerritory(playerId);
    }

    void CompetitionManager::CalculateScores()
    {
        for (auto& data : _playerData)
        {
            if (data.IsEliminated)
                continue;

            switch (_config.Type)
            {
                case CompetitionType::ParkValue:
                    CalculateParkValueScore(data);
                    break;
                case CompetitionType::GuestCount:
                    CalculateGuestCountScore(data);
                    break;
                case CompetitionType::RideIncome:
                    CalculateRideIncomeScore(data);
                    break;
                default:
                    break;
            }
        }
    }

    void CompetitionManager::CalculateParkValueScore(PlayerCompetitionData& data)
    {
        // Score = Park Value / 100
        data.Stats.Score = static_cast<uint32_t>(data.Stats.ParkValue / 100);
    }

    void CompetitionManager::CalculateGuestCountScore(PlayerCompetitionData& data)
    {
        // Score = Guest Count * 10
        data.Stats.Score = data.Stats.GuestCount * 10;
    }

    void CompetitionManager::CalculateRideIncomeScore(PlayerCompetitionData& data)
    {
        // Score = Total Income / 10
        data.Stats.Score = static_cast<uint32_t>(data.Stats.TotalIncome / 10);
    }

    void CompetitionManager::UpdateRankings()
    {
        // Sort by score descending
        std::sort(
            _playerData.begin(),
            _playerData.end(),
            [](const auto& a, const auto& b) { return a.Stats.Score > b.Stats.Score; }
        );

        // Assign ranks
        for (size_t i = 0; i < _playerData.size(); ++i)
        {
            _playerData[i].Rank = static_cast<uint32_t>(i + 1);
        }
    }

    std::vector<PlayerCompetitionData> CompetitionManager::GetLeaderboard() const
    {
        return _playerData;
    }

    uint32_t CompetitionManager::GetTimeRemaining() const
    {
        if (_status != CompetitionStatus::Active)
            return 0;

        auto now = std::chrono::steady_clock::now();
        auto remaining = std::chrono::duration_cast<std::chrono::seconds>(_endTime - now);
        return static_cast<uint32_t>(remaining.count());
    }

    void CompetitionManager::CheckWinConditions()
    {
        // For FastestGoal mode, check if any player achieved objective
        if (_config.Type == CompetitionType::FastestGoal)
        {
            for (auto& data : _playerData)
            {
                // TODO: Check if player completed objective
                // If yes, set them as winner and end competition
            }
        }

        // For Survival mode, check for bankruptcy
        if (_config.Type == CompetitionType::Survival)
        {
            for (auto& data : _playerData)
            {
                // TODO: Check if player is bankrupt
                // If yes, eliminate them
            }

            // If only one player remains, they win
            auto activeCount = std::count_if(
                _playerData.begin(),
                _playerData.end(),
                [](const auto& d) { return !d.IsEliminated; }
            );

            if (activeCount <= 1)
            {
                End();
            }
        }
    }

} // namespace OpenRCT2::Competition
```

### Step 1.2: Integrate with Context

**Modify: `src/openrct2/Context.h`**

```cpp
// Add forward declaration
namespace OpenRCT2::Competition
{
    class CompetitionManager;
}

// In IContext interface, add:
public:
    virtual Competition::CompetitionManager* GetCompetitionManager() = 0;
```

**Modify: `src/openrct2/Context.cpp`**

```cpp
#include "competition/CompetitionManager.h"

// In Context class declaration, add member:
private:
    std::unique_ptr<Competition::CompetitionManager> _competitionManager;

// In Context::Context() constructor:
_competitionManager = std::make_unique<Competition::CompetitionManager>();

// Implement getter:
Competition::CompetitionManager* Context::GetCompetitionManager()
{
    return _competitionManager.get();
}
```

---

## Phase 2: Network Integration

### Step 2.1: Add Network Commands

**Modify: `src/openrct2/network/NetworkTypes.h`**

```cpp
enum class Command : uint32_t
{
    // ... existing commands ...
    
    // NEW: Competition commands
    competitionStart,
    competitionUpdate,
    competitionEnd,
    scoreUpdate,
    leaderboardUpdate,
    
    max,
    invalid = static_cast<uint32_t>(-1),
};
```

### Step 2.2: Extend NetworkPlayer

**Modify: `src/openrct2/network/NetworkPlayer.h`**

```cpp
#include "../competition/CompetitionManager.h"

class Player final
{
public:
    // ... existing fields ...
    
    // NEW: Competition data
    Competition::CompetitiveStats CompetitionStats;
    uint32_t CompetitionScore = 0;
    uint32_t CompetitionRank = 0;
    
    // ... existing methods ...
};
```

### Step 2.3: Add Network Handlers

**Modify: `src/openrct2/network/NetworkBase.h`**

```cpp
// Server functions
void ServerSendCompetitionStart(const Competition::CompetitionConfig& config);
void ServerSendScoreUpdate(uint8_t playerId);
void ServerSendLeaderboard();
void ServerSendCompetitionEnd(uint8_t winnerId);

// Client handlers
void ClientHandleCompetitionStart(Connection& connection, Packet& packet);
void ClientHandleScoreUpdate(Connection& connection, Packet& packet);
void ClientHandleLeaderboard(Connection& connection, Packet& packet);
void ClientHandleCompetitionEnd(Connection& connection, Packet& packet);
```

**Implement in: `src/openrct2/network/NetworkBase.cpp`**

```cpp
void NetworkBase::ServerSendCompetitionStart(const Competition::CompetitionConfig& config)
{
    Packet packet;
    packet << static_cast<uint32_t>(Command::competitionStart);
    packet << static_cast<uint8_t>(config.Type);
    packet << config.DurationMinutes;
    packet << config.MaxPlayers;
    
    SendPacketToClients(packet);
}

void NetworkBase::ServerSendScoreUpdate(uint8_t playerId)
{
    auto* player = GetPlayerByID(playerId);
    if (!player)
        return;

    Packet packet;
    packet << static_cast<uint32_t>(Command::scoreUpdate);
    packet << playerId;
    packet << player->CompetitionScore;
    packet << player->CompetitionRank;
    
    SendPacketToClients(packet);
}

void NetworkBase::ServerSendLeaderboard()
{
    auto* competitionMgr = GetContext()->GetCompetitionManager();
    auto leaderboard = competitionMgr->GetLeaderboard();

    Packet packet;
    packet << static_cast<uint32_t>(Command::leaderboardUpdate);
    packet << static_cast<uint32_t>(leaderboard.size());
    
    for (const auto& entry : leaderboard)
    {
        packet << entry.PlayerId;
        packet << entry.Stats.Score;
        packet << entry.Rank;
    }
    
    SendPacketToClients(packet);
}

void NetworkBase::ClientHandleCompetitionStart(Connection& connection, Packet& packet)
{
    Competition::CompetitionConfig config;
    uint8_t typeValue;
    packet >> typeValue;
    config.Type = static_cast<Competition::CompetitionType>(typeValue);
    packet >> config.DurationMinutes;
    packet >> config.MaxPlayers;
    
    auto* competitionMgr = GetContext()->GetCompetitionManager();
    competitionMgr->Initialize(config);
    competitionMgr->Start();
    
    // Show notification to player
    // TODO: Add UI notification
}

void NetworkBase::ClientHandleScoreUpdate(Connection& connection, Packet& packet)
{
    uint8_t playerId;
    uint32_t score, rank;
    
    packet >> playerId >> score >> rank;
    
    auto* player = GetPlayerByID(playerId);
    if (player)
    {
        player->CompetitionScore = score;
        player->CompetitionRank = rank;
    }
}
```

---

## Phase 3: Simple Leaderboard UI

### Step 3.1: Create Leaderboard Window

**Create: `src/openrct2-ui/windows/Leaderboard.cpp`**

```cpp
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/competition/CompetitionManager.h>
#include <openrct2/network/Network.h>
#include <openrct2/drawing/Drawing.h>

using namespace OpenRCT2::Competition;

namespace OpenRCT2::Ui::Windows
{
    enum {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_SCROLL,
    };

    static constexpr ScreenSize kWindowSize = { 300, 400 };

    static constexpr auto LeaderboardWidgets = makeWidgets(
        makeWindowShim(STR_LEADERBOARD, kWindowSize),
        makeWidget({3, 43}, {294, 354}, WidgetType::scroll, WindowColour::secondary, SCROLL_VERTICAL)
    );

    class LeaderboardWindow final : public Window
    {
    public:
        void OnOpen() override
        {
            widgets = LeaderboardWidgets;
            WindowInitScrollWidgets(*this);
        }

        void OnUpdate() override
        {
            frame_no++;
            WidgetInvalidate(*this, WIDX_SCROLL);
        }

        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
        {
            auto* competitionMgr = GetContext()->GetCompetitionManager();
            auto leaderboard = competitionMgr->GetLeaderboard();
            
            int32_t height = static_cast<int32_t>(leaderboard.size()) * 24 + 6;
            return { 0, height };
        }

        void OnScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            auto* competitionMgr = GetContext()->GetCompetitionManager();
            auto leaderboard = competitionMgr->GetLeaderboard();

            auto screenCoords = ScreenCoordsXY{ 3, 3 };

            for (const auto& entry : leaderboard)
            {
                // Draw rank
                auto ft = Formatter();
                ft.Add<uint32_t>(entry.Rank);
                DrawTextBasic(rt, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

                // Draw player name
                screenCoords.x += 30;
                ft = Formatter();
                ft.Add<const char*>(entry.PlayerName.c_str());
                DrawTextBasic(rt, screenCoords, STR_STRING, ft);

                // Draw score
                screenCoords.x += 150;
                ft = Formatter();
                ft.Add<uint32_t>(entry.Stats.Score);
                DrawTextBasic(rt, screenCoords, STR_WINDOW_COLOUR_2_STRINGID, ft);

                screenCoords.x = 3;
                screenCoords.y += 24;
            }
        }
    };

    WindowBase* LeaderboardOpen()
    {
        auto* window = WindowBringToFrontByClass(WindowClass::Leaderboard);
        if (window == nullptr)
        {
            window = WindowCreate<LeaderboardWindow>(
                WindowClass::Leaderboard,
                kWindowSize,
                WF_RESIZABLE | WF_AUTO_POSITION
            );
        }
        return window;
    }

} // namespace OpenRCT2::Ui::Windows
```

**Add to: `src/openrct2-ui/windows/Windows.h`**

```cpp
WindowBase* LeaderboardOpen();
```

---

## Phase 4: Testing

### Step 4.1: Enable Competition in Debug Menu

**Modify: `src/openrct2-ui/windows/DebugPaint.cpp`** (or create a test menu)

```cpp
// Add button to start test competition
if (WidgetIsPressed(*this, WIDX_START_COMPETITION))
{
    auto* competitionMgr = GetContext()->GetCompetitionManager();
    
    Competition::CompetitionConfig config;
    config.Type = Competition::CompetitionType::ParkValue;
    config.DurationMinutes = 10;
    config.MaxPlayers = 4;
    
    competitionMgr->Initialize(config);
    competitionMgr->Start();
    
    // Open leaderboard
    Windows::LeaderboardOpen();
}
```

### Step 4.2: Hook into Game Tick

**Modify: `src/openrct2/network/NetworkBase.cpp`**

```cpp
void NetworkBase::Tick()
{
    // ... existing tick code ...
    
    // Update competition if active
    auto* competitionMgr = GetContext()->GetCompetitionManager();
    if (competitionMgr && competitionMgr->IsActive())
    {
        competitionMgr->Update();
        
        // Broadcast leaderboard every 5 seconds (200 ticks at 40 ticks/sec)
        if (GetServerTick() % 200 == 0)
        {
            ServerSendLeaderboard();
        }
    }
}
```

---

## Building and Testing

### Build the Project

```powershell
# From project root
msbuild openrct2.sln /p:Configuration=Release /p:Platform=x64 /m
```

### Test Locally

1. **Start Server:**
   ```
   openrct2.exe host --port 11753
   ```

2. **Start Competition from Console:**
   - Use debug tools or game console
   - Type: `start_competition park_value 10`

3. **Connect Clients:**
   ```
   openrct2.exe join localhost:11753
   ```

4. **Open Leaderboard:**
   - Add keybinding or menu item
   - Or use console: `open_leaderboard`

---

## Next Steps

After implementing the foundation:

1. **Add Territory System** - See main analysis document
2. **Implement Per-Player Statistics** - Track stats per territory
3. **Create Competition Maps** - Design balanced competitive maps
4. **Add Win Conditions** - Implement different victory conditions
5. **Polish UI** - Add countdown timer, notifications, end-game screen

---

## Common Issues & Solutions

### Issue: Scores Not Updating
**Solution:** Ensure `CompetitionManager::Update()` is called in network tick

### Issue: Leaderboard Not Showing
**Solution:** Check that `WindowClass::Leaderboard` is added to `WindowClass` enum

### Issue: Network Desync
**Solution:** Ensure all competition state changes go through network packets

### Issue: Players Can't Join During Competition
**Solution:** Add lobby phase before competition starts

---

## API Reference

### Starting a Competition (Server)

```cpp
auto* competitionMgr = GetContext()->GetCompetitionManager();

Competition::CompetitionConfig config;
config.Type = Competition::CompetitionType::ParkValue;
config.DurationMinutes = 30;

competitionMgr->Initialize(config);
competitionMgr->Start();

Network::ServerSendCompetitionStart(config);
```

### Querying Competition State

```cpp
auto* competitionMgr = GetContext()->GetCompetitionManager();

if (competitionMgr->IsActive())
{
    auto timeLeft = competitionMgr->GetTimeRemaining();
    auto leaderboard = competitionMgr->GetLeaderboard();
    auto winner = competitionMgr->GetWinner();
}
```

### Getting Player Score

```cpp
auto* player = Network::GetPlayerByID(playerId);
uint32_t score = player->CompetitionScore;
uint32_t rank = player->CompetitionRank;
```

---

## Resources

- **Main Analysis:** `COMPETITIVE_MULTIPLAYER_ANALYSIS.md`
- **Network Protocol:** `src/openrct2/network/NetworkTypes.h`
- **Existing Multiplayer:** `src/openrct2/network/NetworkBase.cpp`
- **UI Examples:** `src/openrct2-ui/windows/Player.cpp`

