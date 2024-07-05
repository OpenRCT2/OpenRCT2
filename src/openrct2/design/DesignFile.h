#pragma once

//#include "../core/IStream.hpp"
#include "../core/String.hpp"
//#include "../ride/TrackDesign.h"

#include <cstdint>
#include <string_view>
#include <vector>

struct TrackDesign;

namespace OpenRCT2
{
    struct IStream;

    class DesignFileExporter
    {
    public:
        void Export(TrackDesign& trackDesign, u8string_view path);
        void Export(TrackDesign& trackDesign, OpenRCT2::IStream& stream);
    };
} // namespace OpenRCT2
