#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

struct ObjectRepositoryItem;

namespace OpenRCT2
{
    // Current version that is saved.
    constexpr uint32_t PARK_FILE_CURRENT_VERSION = 22;

    // The minimum version that is forwards compatible with the current version.
    constexpr uint32_t PARK_FILE_MIN_VERSION = 22;

    // The minimum version that is backwards compatible with the current version.
    // If this is increased beyond 0, uncomment the checks in ParkFile.cpp and Context.cpp!
    constexpr uint32_t PARK_FILE_MIN_SUPPORTED_VERSION = 0x0;

    constexpr uint32_t PARK_FILE_MAGIC = 0x4B524150; // PARK

    struct IStream;
} // namespace OpenRCT2

class ParkFileExporter
{
public:
    std::vector<const ObjectRepositoryItem*> ExportObjectsList;

    void Export(std::string_view path);
    void Export(OpenRCT2::IStream& stream);
};
