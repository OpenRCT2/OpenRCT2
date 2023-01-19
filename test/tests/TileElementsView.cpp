/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TestData.h"

#include <gtest/gtest.h>
#include <memory>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/TileElementsView.h>

using namespace OpenRCT2;

class TileElementsViewTests : public testing::Test
{
protected:
    static void SetUpTestCase()
    {
        std::string parkPath = TestData::GetParkPath("bpb.sv6");
        gOpenRCT2Headless = true;
        gOpenRCT2NoGraphics = true;
        _context = CreateContext();
        bool initialised = _context->Initialise();
        ASSERT_TRUE(initialised);

        GetContext()->LoadParkFromFile(parkPath);
        GameLoadInit();

        // Changed in some tests. Store to restore its value
        _gScreenFlags = gScreenFlags;
        SUCCEED();
    }

    static void TearDownTestCase()
    {
        if (_context)
            _context.reset();

        gScreenFlags = _gScreenFlags;
    }

private:
    static std::shared_ptr<IContext> _context;
    static uint8_t _gScreenFlags;
};

std::shared_ptr<IContext> TileElementsViewTests::_context;
uint8_t TileElementsViewTests::_gScreenFlags;

template<typename T> std::vector<T*> BuildListManual(const CoordsXY& pos)
{
    std::vector<T*> res;

    TileElement* element = MapGetFirstElementAt(pos);
    if (element == nullptr)
        return res;

    do
    {
        if constexpr (!std::is_same_v<T, TileElement>)
        {
            auto* el = element->as<T>();
            if (el)
                res.push_back(el);
        }
        else
        {
            res.push_back(element);
        }

    } while (!(element++)->IsLastForTile());

    return res;
}

template<typename T> std::vector<T*> BuildListByView(const CoordsXY& pos)
{
    std::vector<T*> res;

    for (auto* element : TileElementsView<T>(pos))
    {
        res.push_back(element);
    }

    return res;
}

template<typename T> bool CompareLists(const CoordsXY& pos)
{
    auto listManual = BuildListManual<T>(pos);
    auto listView = BuildListByView<T>(pos);

    EXPECT_EQ(listManual.size(), listView.size());
    if (listManual.size() != listView.size())
        return false;

    for (size_t i = 0; i < listManual.size(); ++i)
    {
        EXPECT_EQ(listManual[i], listView[i]) << "[i] = " << i;

        if (listManual[i] != listView[i])
            return false;
    }

    return true;
}

template<typename T> void CheckMapTiles()
{
    for (int y = 0; y < MAXIMUM_MAP_SIZE_TECHNICAL; ++y)
    {
        for (int x = 0; x < MAXIMUM_MAP_SIZE_TECHNICAL; ++x)
        {
            auto pos = TileCoordsXY(x, y).ToCoordsXY();

            bool matches = CompareLists<T>(pos);
            EXPECT_TRUE(matches) << "x = " << x << ", y = " << y;

            if (!matches)
            {
                FAIL();
            }
        }
    }
    SUCCEED();
}

TEST_F(TileElementsViewTests, QueryTypeGeneric)
{
    CheckMapTiles<TileElement>();
}

TEST_F(TileElementsViewTests, QueryTypePathElements)
{
    CheckMapTiles<PathElement>();
}

TEST_F(TileElementsViewTests, QueryTypeSurfaceElements)
{
    CheckMapTiles<SurfaceElement>();
}

TEST_F(TileElementsViewTests, QueryTypeTrackElements)
{
    CheckMapTiles<TrackElement>();
}

TEST_F(TileElementsViewTests, QueryTypeSmallSceneryElements)
{
    CheckMapTiles<SmallSceneryElement>();
}

TEST_F(TileElementsViewTests, QueryTypeLargeSceneryElements)
{
    CheckMapTiles<LargeSceneryElement>();
}

TEST_F(TileElementsViewTests, QueryTypeWallElements)
{
    CheckMapTiles<WallElement>();
}

TEST_F(TileElementsViewTests, QueryTypeEntranceElements)
{
    CheckMapTiles<EntranceElement>();
}

TEST_F(TileElementsViewTests, QueryTypeBannerElements)
{
    CheckMapTiles<BannerElement>();
}
