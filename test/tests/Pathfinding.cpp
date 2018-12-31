#include "TestData.h"
#include "openrct2/core/StringReader.hpp"
#include "openrct2/peep/Peep.h"
#include "openrct2/scenario/Scenario.h"

#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/platform/platform.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Map.h>

using namespace OpenRCT2;

static std::ostream& operator<<(std::ostream& os, const TileCoordsXYZ& coords)
{
    return os << "(" << coords.x << ", " << coords.y << ", " << coords.z << ")";
}

class PathfindingTestBase : public testing::Test
{
public:
    static void SetUpTestCase()
    {
        core_init();

        gOpenRCT2Headless = true;
        gOpenRCT2NoGraphics = true;
        _context = CreateContext();
        const bool initialised = _context->Initialise();
        ASSERT_TRUE(initialised);

        std::string parkPath = TestData::GetParkPath("pathfinding-tests.sv6");
        load_from_sv6(parkPath.c_str());
        game_load_init();
    }

    void SetUp() override
    {
        // Use a consistent random seed in every test
        gScenarioSrand0 = 0x12345678;
        gScenarioSrand1 = 0x87654321;
    }

    static void TearDownTestCase()
    {
        _context = nullptr;
    }

protected:
    static bool FindPath(TileCoordsXYZ* pos, const TileCoordsXYZ& goal, int expectedSteps)
    {
        // Our start position is in tile coordinates, but we need to give the peep spawn
        // position in actual world coords (32 units per tile X/Y, 8 per Z level).
        // Add 16 so the peep spawns in the center of the tile.
        rct_peep* peep = peep_generate(pos->x * 32 + 16, pos->y * 32 + 16, pos->z * 8);

        // Peeps that are outside of the park use specialized pathfinding which we don't want to
        // use here
        peep->outside_of_park = 0;

        // Pick the direction the peep should initially move in, given the goal position.
        // This will also store the goal position and initialize pathfinding data for the peep.
        gPeepPathFindGoalPosition = goal;
        const int32_t moveDir = peep_pathfind_choose_direction(*pos, peep);
        if (moveDir < 0)
        {
            // Couldn't determine a direction to move off in
            return false;
        }

        // We have already set up the peep's overall pathfinding goal, but we also have to set their initial
        // 'destination' which is a close position that they will walk towards in a straight line - in this case, one
        // tile away. Stepping the peep will move them towards their destination, and once they reach it, a new
        // destination will be picked, to try and get the peep towards the overall pathfinding goal.
        peep->direction = moveDir;
        peep->destination_x = peep->x + CoordsDirectionDelta[moveDir].x;
        peep->destination_y = peep->y + CoordsDirectionDelta[moveDir].y;
        peep->destination_tolerance = 2;

        // Repeatedly step the peep, until they reach the target position or until the expected number of steps have
        // elapsed. Each step, check that the tile they are standing on is not marked as forbidden in the test data
        // (red neon ground type).
        int step = 0;
        while (!(*pos == goal) && step < expectedSteps)
        {
            uint8_t pathingResult = 0;
            peep->PerformNextAction(pathingResult);
            ++step;

            pos->x = peep->x / 32;
            pos->y = peep->y / 32;
            pos->z = peep->z / 8;

            EXPECT_PRED_FORMAT1(AssertIsNotForbiddenPosition, *pos);
        }

        // Clean up the peep, because we're reusing this loaded context for all tests.
        peep_sprite_remove(peep);

        // Require that the number of steps taken is exactly what we expected. The pathfinder is supposed to be
        // deterministic, and we reset the RNG seed for each test, everything should be entirely repeatable; as
        // such a change in the number of steps taken on one of these paths needs to be reviewed. For the negative
        // tests, we will not have reached the goal but we still expect the loop to have run for the total number
        // of steps requested before giving up.
        EXPECT_EQ(step, expectedSteps);

        return *pos == goal;
    }

    static ::testing::AssertionResult AssertIsStartPosition(const char*, const TileCoordsXYZ& location)
    {
        return AssertPositionIsSetUp("Start", 11u, location);
    }

    static ::testing::AssertionResult AssertIsGoalPosition(const char*, const TileCoordsXYZ& location)
    {
        return AssertPositionIsSetUp("Goal", 9u, location);
    }

    static ::testing::AssertionResult AssertIsNotForbiddenPosition(const char*, const TileCoordsXYZ& location)
    {
        const uint32_t forbiddenSurfaceStyle = 8u;

        const uint32_t style = map_get_surface_element_at(location.x, location.y)->AsSurface()->GetSurfaceStyle();

        if (style == forbiddenSurfaceStyle)
            return ::testing::AssertionFailure()
                << "Path traversed location " << location << ", but it is marked as a forbidden location (surface style "
                << forbiddenSurfaceStyle << "). Either the map is set up incorrectly, or the pathfinder went the wrong way.";

        return ::testing::AssertionSuccess();
    }

private:
    static ::testing::AssertionResult AssertPositionIsSetUp(
        const char* positionKind, uint32_t expectedSurfaceStyle, const TileCoordsXYZ& location)
    {
        const uint32_t style = map_get_surface_element_at(location.x, location.y)->AsSurface()->GetSurfaceStyle();

        if (style != expectedSurfaceStyle)
            return ::testing::AssertionFailure()
                << positionKind << " location " << location << " should have surface style " << expectedSurfaceStyle
                << " but actually has style " << style
                << ". Either the test map is not set up correctly, or you got the coordinates wrong.";

        return ::testing::AssertionSuccess();
    }

    static std::shared_ptr<IContext> _context;
};

std::shared_ptr<IContext> PathfindingTestBase::_context;

struct SimplePathfindingScenario
{
    const char* name;
    TileCoordsXYZ start;
    TileCoordsXYZ goal;
    uint32_t steps;

    SimplePathfindingScenario(const char* _name, const TileCoordsXYZ& _start, const TileCoordsXYZ& _goal, int _steps)
        : name(_name)
        , start(_start)
        , goal(_goal)
        , steps(_steps)
    {
    }

    friend std::ostream& operator<<(std::ostream& os, const SimplePathfindingScenario& scenario)
    {
        return os << scenario.start << " => " << scenario.goal;
    }

    static std::string ToName(const ::testing::TestParamInfo<SimplePathfindingScenario>& param_info)
    {
        return param_info.param.name;
    }
};

class SimplePathfindingTest : public PathfindingTestBase, public ::testing::WithParamInterface<SimplePathfindingScenario>
{
};

TEST_P(SimplePathfindingTest, CanFindPathFromStartToGoal)
{
    const SimplePathfindingScenario& scenario = GetParam();

    ASSERT_PRED_FORMAT1(AssertIsStartPosition, scenario.start);
    ASSERT_PRED_FORMAT1(AssertIsGoalPosition, scenario.goal);

    TileCoordsXYZ pos = scenario.start;

    const auto succeeded = FindPath(&pos, scenario.goal, scenario.steps) ? ::testing::AssertionSuccess()
                                                                         : ::testing::AssertionFailure()
            << "Failed to find path from " << scenario.start << " to " << scenario.goal << " in " << scenario.steps
            << " steps; reached " << pos << " before giving up.";

    EXPECT_TRUE(succeeded);
}

INSTANTIATE_TEST_CASE_P(
    ForScenario, SimplePathfindingTest,
    ::testing::Values(
        SimplePathfindingScenario("StraightFlat", { 2, 19, 14 }, { 4, 19, 14 }, 24),
        SimplePathfindingScenario("SBend", { 2, 17, 14 }, { 4, 16, 14 }, 39),
        SimplePathfindingScenario("UBend", { 2, 14, 14 }, { 2, 12, 14 }, 88),
        SimplePathfindingScenario("CBend", { 2, 10, 14 }, { 2, 7, 14 }, 133),
        SimplePathfindingScenario("TwoEqualRoutes", { 6, 18, 14 }, { 10, 18, 14 }, 819),
        SimplePathfindingScenario("TwoUnequalRoutes", { 6, 14, 14 }, { 10, 14, 14 }, 15643),
        SimplePathfindingScenario("StraightUpBridge", { 2, 4, 14 }, { 4, 4, 16 }, 24),
        SimplePathfindingScenario("StraightUpSlope", { 4, 1, 14 }, { 6, 1, 16 }, 24),
        SimplePathfindingScenario("SelfCrossingPath", { 6, 5, 14 }, { 8, 5, 14 }, 213)),
    SimplePathfindingScenario::ToName);

class ImpossiblePathfindingTest : public PathfindingTestBase, public ::testing::WithParamInterface<SimplePathfindingScenario>
{
};

TEST_P(ImpossiblePathfindingTest, CannotFindPathFromStartToGoal)
{
    const SimplePathfindingScenario& scenario = GetParam();
    TileCoordsXYZ pos = scenario.start;

    ASSERT_PRED_FORMAT1(AssertIsStartPosition, scenario.start);
    ASSERT_PRED_FORMAT1(AssertIsGoalPosition, scenario.goal);

    EXPECT_FALSE(FindPath(&pos, scenario.goal, 10000));
}

INSTANTIATE_TEST_CASE_P(
    ForScenario, ImpossiblePathfindingTest,
    ::testing::Values(
        SimplePathfindingScenario("PathWithGap", { 6, 9, 14 }, { 10, 9, 14 }, 10000),
        SimplePathfindingScenario("PathWithFences", { 6, 7, 14 }, { 10, 7, 14 }, 10000),
        SimplePathfindingScenario("PathWithCliff", { 10, 5, 14 }, { 12, 5, 14 }, 10000)),
    SimplePathfindingScenario::ToName);
