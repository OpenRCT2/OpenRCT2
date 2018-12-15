#include "ParkFile.h"

#include "OpenRCT2.h"
#include "Version.h"
#include "core/Crypt.h"
#include "drawing/Drawing.h"
#include "interface/Viewport.h"
#include "interface/Window.h"
#include "localisation/Date.h"
#include "scenario/Scenario.h"
#include "world/Map.h"
#include "world/Park.h"

using namespace OpenRCT2;

constexpr uint32_t PARK_FILE_MAGIC = 0x4B524150; // PARK

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

    WriteAuthoringChunk();
    WriteGeneralChunk();

    // TODO avoid copying the buffer
    auto uncompressedData = _buffer.str();

    _header.NumChunks = (uint32_t)_chunks.size();
    _header.UncompressedSize = _buffer.tellp();
    _header.Sha1 = Crypt::SHA1(uncompressedData.data(), uncompressedData.size());

    std::ofstream fs(std::string(path).c_str(), std::ios::binary);
    WriteHeader(fs);
    fs.write(uncompressedData.data(), uncompressedData.size());
}

void ParkFile::WriteHeader(std::ostream& fs)
{
    fs.seekp(0);
    fs.write((const char*)&_header, sizeof(_header));
    for (const auto& chunk : _chunks)
    {
        fs.write((const char*)&chunk, sizeof(chunk));
    }
}

void ParkFile::BeginChunk(uint32_t id)
{
    _currentChunk.Id = id;
    _currentChunk.Offset = _buffer.tellp();
    _currentChunk.Length = 0;
}

void ParkFile::EndChunk()
{
    _currentChunk.Length = (uint64_t)_buffer.tellp() - _currentChunk.Offset;
    _chunks.push_back(_currentChunk);
}

void ParkFile::BeginArray(size_t count, size_t elementSize)
{
    WriteValue((uint32_t)count);
    WriteValue((uint32_t)elementSize);
}

void ParkFile::EndArray()
{
}

void ParkFile::WriteBuffer(const void* buffer, size_t len)
{
    _buffer.write((char*)buffer, len);
}

void ParkFile::WriteString(const std::string_view& s)
{
    char nullt = '\0';
    _buffer.write(s.data(), s.size());
    _buffer.write(&nullt, sizeof(nullt));
}

void ParkFile::WriteAuthoringChunk()
{
    BeginChunk(ParkFileChunkType::AUTHORING);
    WriteString(gVersionInfoFull);
    WriteString("Some notes...");
    EndChunk();
}

void ParkFile::WriteGeneralChunk()
{
    BeginChunk(ParkFileChunkType::GENERAL);
    WriteValue<uint64_t>(gScenarioTicks);
    WriteValue<uint32_t>(gDateMonthTicks);
    WriteValue(gDateMonthsElapsed);
    WriteValue(gScenarioSrand0);
    WriteValue(gScenarioSrand1);
    WriteValue(gInitialCash);
    WriteValue(gGuestInitialCash);
    WriteValue(gGuestInitialHunger);
    WriteValue(gGuestInitialThirst);

    auto numSpawns = (gPeepSpawns[0].isNull() ? 1 : 0) + (gPeepSpawns[1].isNull() ? 1 : 0);
    BeginArray(numSpawns, 13);
    for (const auto& spawn : gPeepSpawns)
    {
        if (!gPeepSpawns->isNull())
        {
            WriteValue(spawn.x);
            WriteValue(spawn.y);
            WriteValue(spawn.z);
            WriteValue(spawn.direction);
        }
    }
    EndArray();

    WriteValue(gLandPrice);
    WriteValue(gConstructionRightsPrice);
    EndChunk();
}

enum : uint32_t
{
    S6_SAVE_FLAG_EXPORT = 1 << 0,
    S6_SAVE_FLAG_SCENARIO = 1 << 1,
    S6_SAVE_FLAG_AUTOMATIC = 1u << 31,
};

int32_t scenario_save(const utf8* path, int32_t flags)
{
    if (flags & S6_SAVE_FLAG_SCENARIO)
    {
        log_verbose("saving scenario");
    }
    else
    {
        log_verbose("saving game");
    }

    if (!(flags & S6_SAVE_FLAG_AUTOMATIC))
    {
        window_close_construction_windows();
    }

    map_reorganise_elements();
    viewport_set_saved_view();

    bool result = false;
    auto parkFile = std::make_unique<ParkFile>();
    try
    {
        // if (flags & S6_SAVE_FLAG_EXPORT)
        // {
        //     auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
        //     s6exporter->ExportObjectsList = objManager.GetPackableObjects();
        // }
        // s6exporter->RemoveTracklessRides = true;
        // s6exporter->Export();
        if (flags & S6_SAVE_FLAG_SCENARIO)
        {
            // s6exporter->SaveScenario(path);
        }
        else
        {
            // s6exporter->SaveGame(path);
        }
        parkFile->Save(path);
        result = true;
    }
    catch (const std::exception&)
    {
    }

    gfx_invalidate_screen();

    if (result && !(flags & S6_SAVE_FLAG_AUTOMATIC))
    {
        gScreenAge = 0;
    }
    return result;
}
