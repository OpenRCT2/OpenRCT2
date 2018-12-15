#include "ParkFile.h"

#include "Context.h"
#include "GameState.h"
#include "OpenRCT2.h"
#include "ParkImporter.h"
#include "Version.h"
#include "core/Crypt.h"
#include "drawing/Drawing.h"
#include "interface/Viewport.h"
#include "interface/Window.h"
#include "localisation/Date.h"
#include "object/ObjectManager.h"
#include "object/ObjectRepository.h"
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

    _buffer = std::stringstream(std::ios::out | std::ios::binary);

    WriteAuthoringChunk();
    WriteObjectsChunk();
    WriteGeneralChunk();
    WriteTilesChunk();

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

void ParkFile::BeginArray()
{
    _currentArrayCount = 0;
    _currentArrayElementSize = 0;
    _currentArrayStartPos = _buffer.tellp();
    WriteValue<uint32_t>(0);
    WriteValue<uint32_t>(0);
    _currentArrayLastPos = _buffer.tellp();
}

void ParkFile::NextArrayElement()
{
    auto lastElSize = (size_t)_buffer.tellp() - _currentArrayLastPos;
    if (_currentArrayCount == 0)
    {
        // Set array element size based on first element size
        _currentArrayElementSize = lastElSize;
    }
    else if (_currentArrayElementSize != lastElSize)
    {
        // Array element size was different from first element so reset it
        // to dynamic
        _currentArrayElementSize = 0;
    }
    _currentArrayCount++;
    _currentArrayLastPos = _buffer.tellp();
}

void ParkFile::EndArray()
{
    auto backupPos = _buffer.tellp();
    if ((size_t)backupPos != (size_t)_currentArrayStartPos + 8 && _currentArrayCount == 0)
    {
        throw std::runtime_error("Array data was written but no elements were added.");
    }
    _buffer.seekp(_currentArrayStartPos);
    WriteValue((uint32_t)_currentArrayCount);
    WriteValue((uint32_t)_currentArrayElementSize);
    _buffer.seekp(backupPos);
}

void ParkFile::WriteBuffer(const void* buffer, size_t len)
{
    _buffer.write((char*)buffer, len);
}

void ParkFile::WriteString(const std::string_view& s)
{
    char nullt = '\0';
    auto len = s.find('\0');
    if (len == std::string_view::npos)
    {
        len = s.size();
    }
    _buffer.write(s.data(), len);
    _buffer.write(&nullt, sizeof(nullt));
}

void ParkFile::WriteAuthoringChunk()
{
    BeginChunk(ParkFileChunkType::AUTHORING);
    WriteString(gVersionInfoFull);
    WriteString("Some notes...");
    EndChunk();
}

void ParkFile::WriteObjectsChunk()
{
    BeginChunk(ParkFileChunkType::OBJECTS);
    BeginArray();
    // TODO do not hard code object count
    auto& objManager = GetContext()->GetObjectManager();
    for (size_t i = 0; i < OBJECT_ENTRY_COUNT; i++)
    {
        auto obj = objManager.GetLoadedObject(i);
        if (obj != nullptr)
        {
            auto entry = obj->GetObjectEntry();
            auto type = (uint16_t)(entry->flags & 0x0F);
            type |= 0x8000; // Make as legacy object
            WriteValue<uint16_t>(type);
            WriteString(std::string_view(entry->name, 8));
            WriteString("");
        }
        else
        {
            WriteValue<uint16_t>(0);
            WriteString("");
            WriteString("");
        }
        NextArrayElement();
    }
    EndArray();
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

    BeginArray();
    for (const auto& spawn : gPeepSpawns)
    {
        if (!gPeepSpawns->isNull())
        {
            WriteValue(spawn.x);
            WriteValue(spawn.y);
            WriteValue(spawn.z);
            WriteValue(spawn.direction);
        }
        NextArrayElement();
    }
    EndArray();

    WriteValue(gLandPrice);
    WriteValue(gConstructionRightsPrice);
    EndChunk();
}

void ParkFile::WriteTilesChunk()
{
    BeginChunk(ParkFileChunkType::TILES);
    WriteValue<uint32_t>(gMapSize);
    WriteValue<uint32_t>(gMapSize);
    BeginArray();
    auto numTiles = std::size(gTileElements);
    for (size_t i = 0; i < numTiles; i++)
    {
        WriteBuffer(&gTileElements[i], sizeof(gTileElements[i]));
        NextArrayElement();
    }
    EndArray();
    EndChunk();
}

void ParkFile::Load(const std::string_view& path)
{
    std::ifstream fs(std::string(path).c_str(), std::ios::binary);

    _header = ReadHeader(fs);

    _chunks.clear();
    for (uint32_t i = 0; i < _header.NumChunks; i++)
    {
        ChunkEntry entry;
        fs.read((char*)&entry, sizeof(entry));
        _chunks.push_back(entry);
    }

    _buffer = std::stringstream(std::ios::in | std::ios::out | std::ios::binary);
    _buffer.clear();

    char temp[2048];
    size_t read = 0;
    do
    {
        fs.read(temp, sizeof(temp));
        read = fs.gcount();
        _buffer.write(temp, read);
    } while (read != 0);

    RequiredObjects.clear();
    if (SeekChunk(ParkFileChunkType::OBJECTS))
    {
        auto len = ReadArray();
        for (size_t i = 0; i < len; i++)
        {
            auto type = ReadValue<uint16_t>();
            auto id = ReadString();
            auto version = ReadString();

            rct_object_entry entry{};
            entry.flags = type & 0x7FFF;
            strncpy(entry.name, id.c_str(), 8);
            RequiredObjects.push_back(entry);

            ReadNextArrayElement();
        }
    }
}

void ParkFile::Import()
{
    ReadTilesChunk();
}

ParkFile::Header ParkFile::ReadHeader(std::istream& fs)
{
    Header header;
    fs.read((char*)&header, sizeof(header));
    return header;
}

bool ParkFile::SeekChunk(uint32_t id)
{
    auto result = std::find_if(_chunks.begin(), _chunks.end(), [id](const ChunkEntry& e) { return e.Id == id; });
    if (result != _chunks.end())
    {
        auto offset = result->Offset;
        _buffer.seekg(offset);
        return true;
    }
    return false;
}

size_t ParkFile::ReadArray()
{
    _currentArrayCount = ReadValue<uint32_t>();
    _currentArrayElementSize = ReadValue<uint32_t>();
    _currentArrayLastPos = _buffer.tellg();
    return _currentArrayCount;
}

bool ParkFile::ReadNextArrayElement()
{
    if (_currentArrayCount == 0)
    {
        return false;
    }
    if (_currentArrayElementSize != 0)
    {
        _buffer.seekg((size_t)_currentArrayLastPos + _currentArrayElementSize);
    }
    _currentArrayCount--;
    return _currentArrayCount == 0;
}

void ParkFile::ReadBuffer(void* dst, size_t len)
{
    _buffer.read((char*)dst, len);
}

std::string ParkFile::ReadString()
{
    std::string buffer;
    buffer.reserve(64);
    char c;
    while ((c = ReadValue<char>()) != 0)
    {
        buffer.push_back(c);
    }
    buffer.shrink_to_fit();
    return buffer;
}

void ParkFile::ReadTilesChunk()
{
    if (SeekChunk(ParkFileChunkType::TILES))
    {
        auto mapWidth = ReadValue<uint32_t>();
        [[maybe_unused]] auto mapHeight = ReadValue<uint32_t>();

        OpenRCT2::GetContext()->GetGameState()->InitAll(mapWidth);

        auto numElements = ReadArray();
        ReadBuffer(gTileElements, numElements * sizeof(TileElement));

        map_update_tile_pointers();
    }
    else
    {
        throw std::runtime_error("No tiles chunk found.");
    }
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

class ParkFileImporter : public IParkImporter
{
private:
    const IObjectRepository& _objectRepository;
    std::unique_ptr<ParkFile> _parkFile;

public:
    ParkFileImporter(IObjectRepository& objectRepository)
        : _objectRepository(objectRepository)
    {
    }

    ParkLoadResult Load(const utf8* path) override
    {
        _parkFile = std::make_unique<ParkFile>();
        _parkFile->Load(path);
        return ParkLoadResult(std::move(_parkFile->RequiredObjects));
    }

    ParkLoadResult LoadSavedGame(const utf8* path, bool skipObjectCheck = false) override
    {
        return Load(path);
    }

    ParkLoadResult LoadScenario(const utf8* path, bool skipObjectCheck = false) override
    {
        return Load(path);
    }

    ParkLoadResult LoadFromStream(
        IStream* stream, bool isScenario, bool skipObjectCheck = false, const utf8* path = String::Empty) override
    {
        return Load(path);
    }

    void Import() override
    {
        _parkFile->Import();
    }

    bool GetDetails(scenario_index_entry* dst) override
    {
        return false;
    }
};

std::unique_ptr<IParkImporter> ParkImporter::CreateParkFile(IObjectRepository& objectRepository)
{
    return std::make_unique<ParkFileImporter>(objectRepository);
}
