#include "ParkFile.h"
#include "Version.h"

using namespace OpenRCT2;

constexpr uint32_t PARK_FILE_MAGIC = 0x4B526550; // PARK

// Current version that is saved.
constexpr uint32_t PARK_FILE_CURRENT_VERSION = 0x0;

// The minimum version that is forwards compatible with the current version.
constexpr uint32_t PARK_FILE_MIN_VERSION = 0x0;

constexpr uint32_t COMPRESSION_NONE = 0;
constexpr uint32_t COMPRESSION_GZIP = 1;

namespace ParkFileChunkType
{
    // clang-format off
    constexpr uint32_t RESERVED_0   = 0x00;
    constexpr uint32_t AUTHORING    = 0x01;
    constexpr uint32_t OBJECTS      = 0x02;
    constexpr uint32_t SCENARIO     = 0x03;
    constexpr uint32_t GENERAL      = 0x04;
    constexpr uint32_t INTERFACE    = 0x05;
    constexpr uint32_t CLIMATE      = 0x06;
    constexpr uint32_t PARK         = 0x07;
    constexpr uint32_t HISTORY      = 0x08;
    constexpr uint32_t INVENTIONS   = 0x09;
    constexpr uint32_t TILES        = 0x0A;
    constexpr uint32_t THINGS       = 0x0B;
    constexpr uint32_t RIDES        = 0x0C;
    constexpr uint32_t BANNERS      = 0x0D;
    constexpr uint32_t ANIMATIONS   = 0x0E;
    constexpr uint32_t STAFF        = 0x0F;
    constexpr uint32_t STRINGS      = 0x10;
    constexpr uint32_t EDITOR       = 0x11;
    constexpr uint32_t DERIVED      = 0x12;
    // clang-format on
}; // namespace ParkFileChunkType

void ParkFile::Save(const std::string_view& path)
{
    _header = {};
    _header.Magic = PARK_FILE_MAGIC;
    _header.TargetVersion = PARK_FILE_CURRENT_VERSION;
    _header.MinVersion = PARK_FILE_MIN_VERSION;
    _header.Compression = COMPRESSION_NONE;
}

void ParkFile::WriteHeader(std::ostream& stream, const Header& header, const std::vector<ChunkEntry>& chunks)
{
    stream.seekp(0);
    stream.write((const char*)&header, sizeof(header));
    for (const auto& chunk : chunks)
    {
        stream.write((const char*)&chunk, sizeof(chunk));
    }
}
