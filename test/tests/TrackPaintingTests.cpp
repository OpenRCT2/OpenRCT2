/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TestData.h"
#include "helpers/GameInit.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/drawing/Drawing.Sprite.h>
#include <openrct2/drawing/ImageId.hpp>
#include <openrct2/drawing/PaletteIndex.h>
#include <openrct2/drawing/RenderTarget.h>
#include <openrct2/interface/ZoomLevel.h>
#include <openrct2/paint/Paint.h>
#include <openrct2/paint/track/Support.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/ride/TrackPaint.h>
#include <openrct2/ride/TrackStyle.h>
#include <openrct2/ride/ted/TrackElemType.h>
#include <openrct2/ride/ted/TrackElementDescriptor.h>
#include <openrct2/world/Location.hpp>
#include <openrct2/world/Map.h>
#include <openrct2/world/tile_element/TileElement.h>
#include <openrct2/world/tile_element/TrackElement.h>
#include <string>
#include <string_view>
#include <vector>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::TrackMetadata;

struct PaintSession;

namespace
{
    constexpr std::string_view kSweepParkName = "bpb.sv6";

    // Iterate every non-null TrackStyle
    constexpr auto kLastStyle = TrackStyle::woodenWildMouse;

    // Picked at the center of the map so the synthesized SpritePosition lies inside the park
    // and any in-pipeline tile lookups return real data.
    CoordsXY ParkCenter()
    {
        const auto& mapSize = getGameState().mapSize;
        return { mapSize.x * kCoordsXYStep / 2, mapSize.y * kCoordsXYStep / 2 };
    }

    bool IsDummy(TrackPaintFunction fn)
    {
        return &fn == &TrackPaintFunctionDummy;
    }
} // namespace

class PaintTestFixture : public ::testing::Test
{
protected:
    // The engine's `RenderTarget` itself does not own its `bits`, so tests use this to keep the buffer alive for the
    // duration of a render and free it automatically.
    struct OwnedRenderTarget
    {
        RenderTarget rt{};
        std::vector<PaletteIndex> buffer;
    };

    static void SetUpTestSuite()
    {
        // Initialise headless and with graphics disabled so LoadBaseGraphics is skipping any proprietary data.
        // Once the engine is up we load OpenRCT2's own sprite catalogs.
        // Derived suites flip the flag to false themselves once they're ready to run paint code, which is
        // the only place that actually needs lookups to not assert.
        gOpenRCT2Headless = true;
        gOpenRCT2NoGraphics = true;

        _context = CreateContext();
        ASSERT_TRUE(_context->Initialise());

        GfxLoadG2PalettesFontsTracks();
    }
    static void TearDownTestSuite()
    {
        // Derived suites leave the flag false at the end of their own SetUpTestSuite so context
        // teardown stays quiet; we just flip it back at the very end to leave globals as we found
        // them for subsequent tests.
        _context.reset();
        GfxUnloadG2PalettesFontsTracks();
        gOpenRCT2NoGraphics = true;
    }

    // Loads a park from `test/tests/testdata/parks/<name>` into the live GameState
    static void LoadParkFixture(const std::string_view name)
    {
        ASSERT_NE(_context.get(), nullptr) << "SetUpTestSuite must run before LoadParkFixture";
        TestHelpers::LoadSavedGameFromFile(*_context, TestData::GetParkPath(std::string(name)));
    }

    // Allocates an indexed-color buffer sized for the given viewport dimensions
    static OwnedRenderTarget MakeRenderTarget(const int32_t width, const int32_t height)
    {
        OwnedRenderTarget owned;
        owned.buffer.assign(static_cast<size_t>(width) * static_cast<size_t>(height), PaletteIndex::transparent);
        owned.rt.bits = owned.buffer.data();
        owned.rt.width = width;
        owned.rt.height = height;
        owned.rt.pitch = 0;
        owned.rt.zoom_level = ZoomLevel{ 0 };
        // x, y, culling{X,Y,Width,Height} default to 0; ViewportPaint sets per-column culling itself.
        return owned;
    }

    // Initializes the per-tile session fields a paint function may read when invoked outside the
    // normal viewport pipeline.
    static void ResetSessionForTrackPiece(
        PaintSession& session, const CoordsXY& mapPosition, TileElement* currentlyDrawnTileElement)
    {
        session.MapPosition = mapPosition;
        session.SpritePosition = mapPosition;
        session.LeftTunnels.clear();
        session.RightTunnels.clear();
        session.VerticalTunnelHeight = 0xFF;
        session.Flags = 0;
        session.WaterHeight = 0;
        session.CurrentlyDrawnTileElement = currentlyDrawnTileElement;
        session.CurrentlyDrawnEntity = nullptr;
        session.Surface = nullptr;
        session.PathElementOnSameHeight = nullptr;
        session.TrackElementOnSameHeight = nullptr;
        session.WoodenSupportsPrependTo = nullptr;
        for (auto& [height, slope, pad] : session.SupportSegments)
        {
            height = 0xFFFF;
            slope = 0xFF;
            pad = 0;
        }
        session.Support.height = 0;
        session.Support.slope = 0;
        session.Support.pad = 0;
        session.TrackColours = ImageId(0, Colour::brightRed);
        session.SupportColours = ImageId(0, Colour::lightBlue);
    }

    static inline std::unique_ptr<IContext> _context;
};

class PaintTrackDispatchSweepTests : public PaintTestFixture
{
public:
    static void SetUpTestSuite()
    {
        PaintTestFixture::SetUpTestSuite();

        // Load the park while gOpenRCT2NoGraphics is still true so object loading skips legacy
        // .POB file lookups.
        // Only the paint sweep needs graphics enabled so the GfxGetG1Element debug assert
        // doesn't fire, so flip the flag now that loading is done.
        LoadParkFixture(kSweepParkName);
        gOpenRCT2NoGraphics = false;

        _sampleTrackElement = FindFirstTrackElement();
        ASSERT_NE(_sampleTrackElement, nullptr) << kSweepParkName << " contains no track elements";
        _sampleRide = GetRide(_sampleTrackElement->GetRideIndex());
        ASSERT_NE(_sampleRide, nullptr) << "first track element has no owning ride";
    }

    static void TearDownTestSuite()
    {
        _sampleTrackElement = nullptr;
        _sampleRide = nullptr;
        PaintTestFixture::TearDownTestSuite();
    }

    // Walks every tile of the loaded park and returns a pointer to the first TrackElement
    // found, or nullptr if none was found.
    static TrackElement* FindFirstTrackElement()
    {
        const auto& mapSize = getGameState().mapSize;
        for (int32_t y = 0; y < mapSize.y; ++y)
        {
            for (int32_t x = 0; x < mapSize.x; ++x)
            {
                auto* element = MapGetFirstElementAt(TileCoordsXY{ x, y });
                if (element == nullptr)
                    continue;
                do
                {
                    if (element->getType() == TileElementType::Track)
                    {
                        if (auto* track = element->asTrack(); track != nullptr)
                            return track;
                    }
                } while (!(element++)->isLastForTile());
            }
        }
        return nullptr;
    }

    static inline TrackElement* _sampleTrackElement = nullptr;
    static inline Ride* _sampleRide = nullptr;
};

TEST_F(PaintTrackDispatchSweepTests, AllStylesTypesDirsAndSequencesNoCrash)
{
    ASSERT_NE(_sampleRide, nullptr);
    ASSERT_NE(_sampleTrackElement, nullptr);

    // The synthesized SpritePosition is at the center of the map; we don't care what gets drawn
    // only that `imageWithinRT` doesn't discard the paint structs on the way out.
    constexpr int32_t kRTSize = 1024;
    auto owned = MakeRenderTarget(kRTSize, kRTSize);
    auto& rt = owned.rt;
    constexpr int32_t kPermissiveCullExtent = 1'000'000;
    rt.x = -kPermissiveCullExtent / 2;
    rt.y = -kPermissiveCullExtent / 2;
    rt.cullingX = -kPermissiveCullExtent / 2;
    rt.cullingY = -kPermissiveCullExtent / 2;
    rt.cullingWidth = kPermissiveCullExtent;
    rt.cullingHeight = kPermissiveCullExtent;
    const auto centre = ParkCenter();

    // GetTrackPaintFunction is a pure dispatch-table lookup.
    // The ride and trackElement references only exist to satisfy the paint function signature.
    // The same sample pair is fed to every combo even though its real ride type and track type probably
    // differ from the loop's. Failures surface as a segfault.

    size_t totalInvocations = 0;
    size_t skippedZeroSequence = 0;

    // TileElement and TrackElement share TileElementBase's layout
    auto* asTileElement = reinterpret_cast<TileElement*>(_sampleTrackElement);

    for (uint16_t s = 0; s <= static_cast<uint8_t>(kLastStyle); ++s)
    {
        const auto style = static_cast<TrackStyle>(s);

        for (uint8_t direction = 0; direction < 4; ++direction)
        {
            auto* session = PaintSessionAlloc(rt, 0u, direction);
            ASSERT_NE(session, nullptr);

            for (uint16_t t = 0; t < static_cast<uint16_t>(TrackElemType::count); ++t)
            {
                const auto trackType = static_cast<TrackElemType>(t);
                TrackPaintFunction fn = GetTrackPaintFunction(style, trackType);
                if (IsDummy(fn))
                    continue;

                const auto& ted = GetTrackElementDescriptor(trackType);
                const uint8_t numSequences = ted.sequenceData.numSequences;
                if (numSequences == 0)
                {
                    ++skippedZeroSequence;
                    continue;
                }

                for (uint8_t seq = 0; seq < numSequences; ++seq)
                {
                    ResetSessionForTrackPiece(*session, centre, asTileElement);
                    SCOPED_TRACE(
                        "style=" + std::to_string(s) + " type=" + std::to_string(t) + " dir=" + std::to_string(direction)
                        + " seq=" + std::to_string(seq));
                    constexpr int32_t kSyntheticHeight = 64;
                    fn(*session, *_sampleRide, seq, direction, kSyntheticHeight, *_sampleTrackElement, SupportType{});
                    ++totalInvocations;
                }
            }

            PaintSessionFree(session);
        }
    }

    EXPECT_GT(totalInvocations, 0u)
        << "no non-dummy paint functions invoked across any style — the dispatch table may have changed, or all TED "
           "numSequences are zero.";
    if (skippedZeroSequence > 0)
    {
        // Not a failure: some track types legitimately have numSequences==0 for some styles.
        RecordProperty("skipped_zero_sequence", std::to_string(skippedZeroSequence));
    }
}
