/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/UiStringIds.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/competition/CompetitionManager.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2::Competition;
using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    enum WindowLeaderboardWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_CONTENT_PANEL,
        WIDX_HEADER_RANK,
        WIDX_HEADER_NAME,
        WIDX_HEADER_SCORE,
        WIDX_SCROLL,
    };

    static constexpr ScreenSize kWindowSize = { 350, 300 };
    static constexpr int32_t kRowHeight = 12;

    // clang-format off
    static constexpr auto _leaderboardWidgets = makeWidgets(
        makeWindowShim(STR_COMPETITION_LEADERBOARD, kWindowSize),
        makeWidget({  0,  43}, {350, 257}, WidgetType::resize,      WindowColour::secondary                   ),
        makeWidget({  3,  46}, { 45,  14}, WidgetType::tableHeader, WindowColour::primary, STR_RANK_COLUMN    ),
        makeWidget({ 48,  46}, {160,  14}, WidgetType::tableHeader, WindowColour::primary, STR_PLAYER_NAME_COLUMN),
        makeWidget({208,  46}, {100,  14}, WidgetType::tableHeader, WindowColour::primary, STR_SCORE_COLUMN   ),
        makeWidget({  3,  60}, {344, 210}, WidgetType::scroll,      WindowColour::secondary, SCROLL_VERTICAL  )
    );
    // clang-format on

    class LeaderboardWindow final : public Window
    {
    public:
        void onOpen() override
        {
            setWidgets(_leaderboardWidgets);
            initScrollWidgets();
            WindowInitScrollWidgets(*this);

            colours[0] = Colour::grey;
            colours[1] = Colour::grey;
            colours[2] = Colour::grey;

            minWidth = kWindowSize.width;
            minHeight = kWindowSize.height;
            maxWidth = 500;
            maxHeight = 450;
        }

        void onUpdate() override
        {
            invalidate();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
            }
        }

        void onResize() override
        {
            WindowSetResize(*this, { kWindowSize.width, kWindowSize.height }, { maxWidth, maxHeight });
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            auto* ctx = GetContext();
            auto* competitionMgr = ctx->GetCompetitionManager();

            if (!competitionMgr || !competitionMgr->IsActive())
                return { 0, kRowHeight * 2 };

            auto leaderboard = competitionMgr->GetLeaderboard();
            int32_t contentHeight = static_cast<int32_t>(leaderboard.size()) * kRowHeight;
            return { 0, contentHeight };
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            auto* ctx = GetContext();
            auto* competitionMgr = ctx->GetCompetitionManager();

            ScreenCoordsXY screenCoords = { 0, 0 };

            if (!competitionMgr || !competitionMgr->IsActive())
            {
                DrawTextBasic(rt, screenCoords, STR_NO_ACTIVE_COMPETITION);
                return;
            }

            auto leaderboard = competitionMgr->GetLeaderboard();
            int32_t listIndex = 0;

            for (const auto& entry : leaderboard)
            {
                screenCoords.y = listIndex * kRowHeight;

                if (screenCoords.y > rt.y + rt.height)
                    break;

                if (screenCoords.y + kRowHeight >= rt.y)
                {
                    // Highlight selected row
                    if (listIndex == selectedListItem)
                    {
                        Rectangle::filter(
                            rt, { 0, screenCoords.y, width, screenCoords.y + kRowHeight - 1 },
                            FilterPaletteID::paletteDarken1);
                    }

                    // Alternate row background
                    if (listIndex % 2 == 0)
                    {
                        Rectangle::filter(
                            rt, { 0, screenCoords.y, width, screenCoords.y + kRowHeight - 1 },
                            FilterPaletteID::paletteDarken3);
                    }

                    // Draw rank
                    auto ft = Formatter();
                    ft.Add<uint32_t>(entry.Rank);
                    DrawTextBasic(rt, { 5, screenCoords.y }, STR_COMMA16, ft);

                    // Draw player name
                    ft = Formatter();
                    ft.Add<const char*>(entry.PlayerName.c_str());
                    DrawTextBasic(rt, { 50, screenCoords.y }, STR_STRING, ft);

                    // Draw score
                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(entry.Stats.Score));
                    DrawTextBasic(rt, { 210, screenCoords.y }, STR_COMMA16, ft);

                    // Add eliminated indicator if applicable
                    if (entry.IsEliminated)
                    {
                        DrawTextBasic(rt, { 300, screenCoords.y }, STR_PLAYER_ELIMINATED, {}, { Colour::brightRed });
                    }
                }
                listIndex++;
            }
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            int32_t index = screenCoords.y / kRowHeight;
            auto* ctx = GetContext();
            auto* competitionMgr = ctx->GetCompetitionManager();

            if (competitionMgr && competitionMgr->IsActive())
            {
                auto leaderboard = competitionMgr->GetLeaderboard();
                if (index >= 0 && static_cast<size_t>(index) < leaderboard.size())
                {
                    selectedListItem = index;
                    invalidate();
                }
            }
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);

            auto* ctx = GetContext();
            auto* competitionMgr = ctx->GetCompetitionManager();

            // Draw time remaining or status at bottom
            auto statusCoords = windowPos + ScreenCoordsXY{ 10, height - 20 };

            if (competitionMgr)
            {
                if (competitionMgr->IsActive())
                {
                    uint32_t timeRemaining = competitionMgr->GetTimeRemaining();
                    uint32_t minutes = timeRemaining / 60;
                    uint32_t seconds = timeRemaining % 60;

                    auto ft = Formatter();
                    ft.Add<uint32_t>(minutes);
                    ft.Add<uint32_t>(seconds);
                    DrawTextBasic(rt, statusCoords, STR_TIME_REMAINING_FORMAT, ft);

                    // Draw competition type
                    auto typeCoords = windowPos + ScreenCoordsXY{ width - 150, height - 20 };
                    StringId typeString = GetCompetitionTypeString(competitionMgr->GetType());
                    DrawTextBasic(rt, typeCoords, typeString);
                }
                else if (competitionMgr->IsFinished())
                {
                    DrawTextBasic(rt, statusCoords, STR_COMPETITION_ENDED);

                    // Show winner
                    auto winnerCoords = windowPos + ScreenCoordsXY{ width - 150, height - 20 };
                    uint8_t winnerId = competitionMgr->GetWinnerId();
                    auto* winnerData = competitionMgr->GetPlayerData(winnerId);
                    if (winnerData)
                    {
                        auto ft = Formatter();
                        ft.Add<const char*>(winnerData->PlayerName.c_str());
                        DrawTextBasic(rt, winnerCoords, STR_PLAYER_LEADING, ft, { Colour::brightGreen });
                    }
                }
                else
                {
                    DrawTextBasic(rt, statusCoords, STR_NO_ACTIVE_COMPETITION);
                }
            }
            else
            {
                DrawTextBasic(rt, statusCoords, STR_NO_ACTIVE_COMPETITION);
            }
        }

    private:
        StringId GetCompetitionTypeString(CompetitionType type) const
        {
            switch (type)
            {
                case CompetitionType::ParkValue:
                    return STR_COMPETITION_TYPE_PARK_VALUE;
                case CompetitionType::GuestCount:
                    return STR_COMPETITION_TYPE_GUEST_COUNT;
                case CompetitionType::RideIncome:
                    return STR_COMPETITION_TYPE_RIDE_INCOME;
                case CompetitionType::FastestGoal:
                    return STR_COMPETITION_TYPE_SPEED;
                case CompetitionType::Survival:
                    return STR_COMPETITION_TYPE_SURVIVAL;
                case CompetitionType::RideMaster:
                    return STR_COMPETITION_TYPE_RIDE_MASTER;
                default:
                    return STR_COMPETITIONS;
            }
        }
    };

    WindowBase* CompetitionLeaderboardOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::competitionLeaderboard);
        if (window == nullptr)
        {
            window = windowMgr->Create<LeaderboardWindow>(
                WindowClass::competitionLeaderboard, kWindowSize, { WindowFlag::autoPosition, WindowFlag::resizable });
        }
        return window;
    }

} // namespace OpenRCT2::Ui::Windows
