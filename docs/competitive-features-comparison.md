# OpenRCT2 Competitive Multiplayer - Feature Comparison

## Current vs. Proposed Features

| Feature | Current Status | Proposed Enhancement | Priority |
|---------|---------------|---------------------|----------|
| **Networking** |
| Client-Server Architecture | ✅ Fully Implemented | No changes needed | - |
| Master Server Integration | ✅ Working | Add competition server filtering | Low |
| Player Authentication | ✅ With Key System | No changes needed | - |
| Real-time Synchronization | ✅ Tick-based sync | No changes needed | - |
| **Player Management** |
| Multiple Players | ✅ Up to 255 players | No changes needed | - |
| Player Permissions | ✅ Group-based system | Add competition-specific permissions | Medium |
| Player Tracking | ✅ Actions & spending | Add competition stats tracking | **High** |
| Player Territories | ❌ Not Implemented | **NEW: Territory ownership system** | **High** |
| **Gameplay** |
| Cooperative Building | ✅ Full support | Keep as default mode | - |
| Shared Resources | ✅ Single park | **NEW: Per-player resources** | **High** |
| Individual Objectives | ❌ Not Implemented | **NEW: Per-player objectives** | **High** |
| Win/Loss Conditions | ❌ Not Implemented | **NEW: Competition victory conditions** | **High** |
| **Scoring & Statistics** |
| Money Tracking | ✅ Per-player spending | Enhance with profit/income tracking | Medium |
| Action Counting | ✅ Commands ran | Enhance with detailed action stats | Low |
| Park Value | ✅ Global only | **NEW: Per-player park value** | **High** |
| Guest Count | ✅ Global only | **NEW: Per-player guest count** | **High** |
| Ride Statistics | ✅ Global | **NEW: Per-player ride stats** | Medium |
| Leaderboards | ❌ Not Implemented | **NEW: Real-time leaderboards** | **High** |
| Rankings | ❌ Not Implemented | **NEW: Dynamic player rankings** | **High** |
| **UI Features** |
| Multiplayer Window | ✅ Info, Players, Groups, Options | Add Competition tab | Medium |
| Player List | ✅ Shows all players | Add rank/score columns | Medium |
| Chat System | ✅ Full chat | Add competition announcements | Low |
| Leaderboard Window | ❌ Not Implemented | **NEW: Dedicated leaderboard UI** | **High** |
| Competition HUD | ❌ Not Implemented | **NEW: Timer, rank, score display** | **High** |
| Victory Screen | ❌ Not Implemented | **NEW: End-game results UI** | Medium |
| **Game Modes** |
| Sandbox Mode | ✅ Implemented | Keep existing | - |
| Scenario Mode | ✅ Implemented | Keep existing | - |
| Cooperative Mode | ✅ Current multiplayer | Keep as default | - |
| Competitive Mode | ❌ Not Implemented | **NEW: Multiple competitive modes** | **High** |
| Tournament Mode | ❌ Not Implemented | **NEW: Multi-round elimination** | Low |
| **Competition Types** |
| Park Value Race | ❌ Not Implemented | **NEW: Highest value wins** | **High** |
| Guest Count Race | ❌ Not Implemented | **NEW: Most guests wins** | **High** |
| Income Challenge | ❌ Not Implemented | **NEW: Highest profit wins** | Medium |
| Speed Challenge | ❌ Not Implemented | **NEW: First to goal wins** | Medium |
| Survival Mode | ❌ Not Implemented | **NEW: Last player standing** | Low |
| **Map Features** |
| Standard Maps | ✅ Many available | No changes | - |
| Scenario Editor | ✅ Full featured | Add territory tools | Medium |
| Competitive Maps | ❌ Not Implemented | **NEW: Pre-divided maps** | **High** |
| Balanced Generation | ❌ Not Implemented | **NEW: Fair territory generation** | Medium |

---

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                         CLIENT SIDE                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                   │
│  ┌──────────────────┐     ┌──────────────────┐                 │
│  │  Leaderboard UI  │     │ Competition HUD  │                 │
│  │  - Rankings      │     │ - Timer          │                 │
│  │  - Scores        │     │ - Current Rank   │                 │
│  │  - Stats         │     │ - Score          │                 │
│  └────────┬─────────┘     └────────┬─────────┘                 │
│           │                         │                            │
│           └─────────┬───────────────┘                            │
│                     ▼                                            │
│         ┌───────────────────────┐                               │
│         │   Competition Manager │                               │
│         │   - Track local state │                               │
│         │   - Display updates   │                               │
│         └───────────┬───────────┘                               │
│                     │                                            │
│                     ▼                                            │
│         ┌───────────────────────┐                               │
│         │   Network Client      │                               │
│         │   - Receive updates   │                               │
│         │   - Send actions      │                               │
│         └───────────┬───────────┘                               │
│                     │                                            │
└─────────────────────┼────────────────────────────────────────────┘
                      │
                      │ Network (TCP/IP)
                      │
┌─────────────────────┼────────────────────────────────────────────┐
│                     │          SERVER SIDE                        │
├─────────────────────┼────────────────────────────────────────────┤
│                     ▼                                            │
│         ┌───────────────────────┐                               │
│         │   Network Server      │                               │
│         │   - Manage clients    │                               │
│         │   - Broadcast updates │                               │
│         └───────────┬───────────┘                               │
│                     │                                            │
│                     ▼                                            │
│         ┌───────────────────────┐                               │
│         │  Competition Manager  │                               │
│         │  - Track all players  │                               │
│         │  - Calculate scores   │                               │
│         │  - Update rankings    │                               │
│         │  - Check win conds    │                               │
│         └───────────┬───────────┘                               │
│                     │                                            │
│         ┌───────────┴──────────┬──────────────────┐            │
│         ▼                       ▼                  ▼             │
│  ┌─────────────┐    ┌─────────────────┐  ┌──────────────┐     │
│  │  Territory  │    │   Score         │  │  Player      │     │
│  │  Manager    │    │   Calculator    │  │  Stats       │     │
│  │             │    │                 │  │  Tracker     │     │
│  │ - Validate  │    │ - Park Value    │  │              │     │
│  │   ownership │    │ - Guest Count   │  │ - Money      │     │
│  │ - Check     │    │ - Income        │  │ - Guests     │     │
│  │   bounds    │    │ - Custom        │  │ - Rides      │     │
│  └─────────────┘    └─────────────────┘  └──────────────┘     │
│         │                       │                  │             │
│         └───────────┬───────────┴──────────────────┘            │
│                     ▼                                            │
│         ┌───────────────────────┐                               │
│         │    Game State         │                               │
│         │    - Park data        │                               │
│         │    - Rides            │                               │
│         │    - Guests           │                               │
│         │    - Finances         │                               │
│         └───────────────────────┘                               │
│                                                                   │
└───────────────────────────────────────────────────────────────────┘
```

---

## Data Flow Diagrams

### 1. Competition Start Flow

```
Player 1 (Host)                Server                  Other Players
     │                           │                           │
     │─── Start Competition ────>│                           │
     │                           │                           │
     │                           │── Competition Start ─────>│
     │                           │   (Config, Map Data)      │
     │                           │                           │
     │<────── Acknowledged ──────│                           │
     │                           │<──── Acknowledged ────────│
     │                           │                           │
     │                           │── Initialize Territories >│
     │                           │                           │
     │<── Territory Assigned ────│                           │
     │                           │── Territory Assigned ────>│
     │                           │                           │
     │                           │── Start Timer ───────────>│
     │                           │                           │
```

### 2. Real-time Score Update Flow

```
  Client                        Server                   Other Clients
     │                           │                           │
     │─── Build Ride ───────────>│                           │
     │                           │                           │
     │                           │ [Validate Territory]      │
     │                           │ [Update Stats]            │
     │                           │ [Calculate Score]         │
     │                           │                           │
     │<─── Action Success ───────│                           │
     │                           │                           │
     │                           │── Score Update ──────────>│
     │                           │   (Player ID, New Score)  │
     │                           │                           │
     │                           │── Leaderboard Update ────>│
     │                           │   (All Rankings)          │
     │                           │                           │
```

### 3. Competition End Flow

```
  Server                                               All Clients
     │                                                      │
     │ [Timer Expired / Win Condition Met]                 │
     │                                                      │
     │─── Calculate Final Scores ────>                     │
     │                                                      │
     │─── Determine Winner ─────────>                      │
     │                                                      │
     │─────────── Competition End ────────────────────────>│
     │            (Winner ID, Final Scores,                │
     │             Final Rankings, Stats)                  │
     │                                                      │
     │<──────────── Acknowledged ──────────────────────────│
     │                                                      │
     │─────────── Victory Screen Data ────────────────────>│
     │            (Detailed Stats, Achievements)           │
     │                                                      │
```

---

## Competitive Mode Types - Detailed Specifications

### 1. Park Value Race 🏆

**Objective:** Build the highest value park within time limit

**Scoring:**
```
Score = (Park Value + Ride Values + Guest Value) / 100
```

**Win Condition:**
- Highest score when timer expires

**Duration:** 20-60 minutes

**Best For:** Players who enjoy building impressive parks

---

### 2. Guest Count Challenge 👥

**Objective:** Attract the most guests to your park

**Scoring:**
```
Score = Current Guest Count × 10 + (Average Happiness / 10)
```

**Win Condition:**
- Most guests when timer expires
- OR first to reach target guest count

**Duration:** 15-45 minutes

**Best For:** Players who focus on park management and guest satisfaction

---

### 3. Profit Master 💰

**Objective:** Generate the highest profit

**Scoring:**
```
Score = (Total Income - Total Expenses) / 10
```

**Win Condition:**
- Highest profit when timer expires

**Special Rules:**
- Loan interest applies
- Negative profit = negative score

**Duration:** 30-60 minutes

**Best For:** Business-minded players

---

### 4. Speed Challenge ⚡

**Objective:** First to complete scenario objective

**Scoring:**
```
Score = 10000 - (Completion Time in Seconds)
Bonus: +500 for each 1000 park value above minimum
```

**Win Condition:**
- First player to complete objective wins immediately

**Example Objectives:**
- Reach 1000 guests
- Build 5 roller coasters with 6.00+ excitement
- Reach $50,000 park value

**Duration:** Variable (ends when someone wins)

**Best For:** Competitive speed-runners

---

### 5. Survival Mode 💀

**Objective:** Don't go bankrupt, eliminate opponents

**Scoring:**
```
Score = Time Survived (seconds) + (Current Cash / 100)
```

**Win Condition:**
- Last player not bankrupt wins

**Special Rules:**
- Bankruptcy = Elimination (Cash < -$10,000)
- No loans allowed
- Starting cash: $10,000

**Duration:** Until one player remains

**Best For:** Players who enjoy resource management pressure

---

### 6. Ride Master 🎢

**Objective:** Build the best roller coasters

**Scoring:**
```
Score = Sum of (Excitement Rating × 100) for all roller coasters
Bonus: +200 for each award-winning ride
```

**Win Condition:**
- Highest total excitement when timer expires

**Special Rules:**
- Only roller coasters count
- Minimum 6.00 excitement to count
- Must be operating (not broken/closed)

**Duration:** 30-60 minutes

**Best For:** Coaster enthusiasts

---

## Implementation Phases - Timeline

```
Phase 1: Foundation (4 weeks)
├─ Week 1: Competition Manager core
├─ Week 2: Network protocol extensions  
├─ Week 3: Player stats tracking
└─ Week 4: Testing & bug fixes

Phase 2: Territory System (4 weeks)
├─ Week 1: Territory data structures
├─ Week 2: Ownership validation
├─ Week 3: Visual boundaries
└─ Week 4: Map generation

Phase 3: Scoring (3 weeks)
├─ Week 1: Score calculation algorithms
├─ Week 2: Real-time updates
└─ Week 3: Leaderboard sync

Phase 4: UI (3 weeks)
├─ Week 1: Leaderboard window
├─ Week 2: Competition HUD
└─ Week 3: Victory/defeat screens

Phase 5: Game Modes (4 weeks)
├─ Week 1: Park Value & Guest Count modes
├─ Week 2: Profit & Speed modes
├─ Week 3: Survival mode
└─ Week 4: Mode balancing

Phase 6: Polish (3 weeks)
├─ Week 1: Lobby system
├─ Week 2: Notifications & effects
└─ Week 3: Final testing

Phase 7: Beta Testing (3 weeks)
├─ Week 1: Internal testing
├─ Week 2: Community beta
└─ Week 3: Bug fixes & adjustments

Total: ~24 weeks (6 months)
```

---

## Technical Requirements

### Server Requirements
- **CPU:** 2+ cores recommended for >4 players
- **RAM:** 4GB+ for large maps with many players
- **Network:** 10 Mbps upload minimum (for 8 players)
- **Tick Rate:** 40 ticks/second (current standard)

### Client Requirements
- **Same as current OpenRCT2 requirements**
- **Additional:** ~50MB RAM for competition tracking
- **Network:** 5 Mbps download minimum

### Bandwidth Estimate
- **Base multiplayer:** ~50 KB/s per player
- **Competition mode:** +20 KB/s per player (for score updates)
- **For 8 players:** ~560 KB/s server bandwidth

---

## Compatibility Matrix

| Feature | Single Player | Cooperative MP | Competitive MP |
|---------|---------------|----------------|----------------|
| Save/Load | ✅ | ✅ | ✅ |
| Scenarios | ✅ | ✅ | ✅* |
| Sandbox | ✅ | ✅ | ❌ |
| Plugins | ✅ | ✅ | ⚠️** |
| Cheats | ✅ | ⚠️*** | ❌ |
| Custom Objects | ✅ | ✅ | ✅ |
| Replays | ✅ | ⚠️*** | ✅ |

\* Requires competition-specific scenarios
\*\* Plugins must be competition-compatible
\*\*\* Server-controlled

---

## Performance Considerations

### CPU Usage
```
Current Multiplayer:  ~5-15% CPU
+ Competition Mode:   +2-5% CPU
  └─ Score calculation:  1-2%
  └─ Territory validation: 1-2%
  └─ Leaderboard updates: <1%
```

### Memory Usage
```
Current Multiplayer:  ~200-500MB
+ Competition Mode:   +30-50MB per player
  └─ Territory data:     10-20MB
  └─ Stats tracking:     10-20MB
  └─ Leaderboard cache:  5-10MB
```

### Network Traffic
```
Current:              ~50 KB/s per player
+ Competition:        +15-25 KB/s per player
  └─ Score updates:      5-10 KB/s
  └─ Leaderboard:        5-10 KB/s
  └─ Territory data:     5-10 KB/s (initial)
```

---

## Backwards Compatibility

✅ **Fully Compatible:**
- Existing multiplayer servers remain functional
- Competition mode is opt-in
- Standard cooperative play unchanged
- Existing saves work normally

⚠️ **Partial Compatibility:**
- Competition saves require competition-aware clients
- Territory data not readable by older versions

❌ **Incompatible:**
- Old clients cannot join competition servers
- Competition-specific packets will be ignored by old clients

**Migration Strategy:**
1. Competition mode is a separate server type
2. Master server shows server type clearly
3. Client shows warning when joining competition server
4. Fallback to cooperative mode if needed

---

## Future Enhancements (Post-Launch)

### Short Term (3-6 months)
- [ ] More competition modes (team-based, relay races)
- [ ] Achievement system
- [ ] Statistics dashboard
- [ ] Replay system with competition bookmarks
- [ ] Spectator mode improvements

### Medium Term (6-12 months)
- [ ] Ranked matchmaking
- [ ] Seasonal leaderboards
- [ ] Custom competition editor
- [ ] Tournament organization tools
- [ ] Mobile companion app

### Long Term (12+ months)
- [ ] AI opponents
- [ ] Cross-platform tournaments
- [ ] Integrated streaming features
- [ ] Professional esports support
- [ ] VR spectator mode

---

## Community Features

### Leaderboards
- **Global Rankings:** Track top players worldwide
- **Seasonal Rankings:** Reset every 3 months
- **Mode-Specific:** Separate rankings per competition type
- **Regional:** Country/continent-based rankings

### Tournaments
- **Official Events:** Monthly/quarterly tournaments
- **Community Events:** Player-organized competitions
- **Prize Support:** In-game rewards, titles, badges
- **Streaming Integration:** Twitch/YouTube integration

### Social Features
- **Friend Lists:** Easy matchmaking with friends
- **Clans/Teams:** Form competitive teams
- **Replay Sharing:** Share best performances
- **Statistics Sharing:** Compare stats with friends

---

## Conclusion

This competitive multiplayer system builds upon OpenRCT2's solid foundation to add exciting new ways to play. The modular design ensures:

✅ **Minimal disruption** to existing features
✅ **Optional participation** - cooperative play remains default
✅ **Extensible architecture** for future enhancements
✅ **Community-driven** development and testing

**Ready to build this?** Start with Phase 1 implementation guide! 🚀
