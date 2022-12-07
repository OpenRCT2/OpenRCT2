
#include "PaintSessionTest.h"

#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackPaint.h>
TEST_F(PaintSessionTest, CircusPaintTestAll)
{
    auto trackElement = TrackElement();
    auto paintFunction = GetTrackPaintFunctionCircus(TrackElemType::FlatTrack3x3);

    // go through all the sequences and directions
    std::vector<uint8_t> sequences = { 1, 3, 5, 6, 7, 8 };
    for (uint8_t direction = 0; direction < 4; direction++)
        for (auto sequence : sequences)
            paintFunction(_paintSession, _ride, sequence, direction, 0, trackElement);

    std::string filename = "CircusPaintTest";

    // use this to regenerate the table, it will appear in the outputs
    // you will need to manually copy the table to the testdata/paint_session folder
    // DumpJson(filename);
    CompareWith(filename);
}
