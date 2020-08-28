/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "ImageTable.h"
#include "StringTable.h"

#include <algorithm>
#include <optional>
#include <string_view>
#include <vector>

using ObjectEntryIndex = uint16_t;
constexpr const ObjectEntryIndex OBJECT_ENTRY_INDEX_NULL = std::numeric_limits<ObjectEntryIndex>::max();

// First 0xF of rct_object_entry->flags
enum OBJECT_TYPE
{
    OBJECT_TYPE_RIDE,
    OBJECT_TYPE_SMALL_SCENERY,
    OBJECT_TYPE_LARGE_SCENERY,
    OBJECT_TYPE_WALLS,
    OBJECT_TYPE_BANNERS,
    OBJECT_TYPE_PATHS,
    OBJECT_TYPE_PATH_BITS,
    OBJECT_TYPE_SCENERY_GROUP,
    OBJECT_TYPE_PARK_ENTRANCE,
    OBJECT_TYPE_WATER,
    OBJECT_TYPE_SCENARIO_TEXT,
    OBJECT_TYPE_TERRAIN_SURFACE,
    OBJECT_TYPE_TERRAIN_EDGE,
    OBJECT_TYPE_STATION,
    OBJECT_TYPE_MUSIC,

    OBJECT_TYPE_COUNT
};

enum OBJECT_SELECTION_FLAGS
{
    OBJECT_SELECTION_FLAG_SELECTED = (1 << 0),
    OBJECT_SELECTION_FLAG_2 = (1 << 1),
    OBJECT_SELECTION_FLAG_IN_USE = (1 << 2),
    // OBJECT_SELECTION_FLAG_REQUIRED = (1 << 3),               // Unused feature
    OBJECT_SELECTION_FLAG_ALWAYS_REQUIRED = (1 << 4),
    OBJECT_SELECTION_FLAG_6 = (1 << 5),
    OBJECT_SELECTION_FLAG_7 = (1 << 6),
    OBJECT_SELECTION_FLAG_8 = (1 << 7),
    OBJECT_SELECTION_FLAG_ALL = 0xFF,
};

#define OBJECT_SELECTION_NOT_SELECTED_OR_REQUIRED 0

enum OBJECT_SOURCE_GAME
{
    OBJECT_SOURCE_CUSTOM,
    OBJECT_SOURCE_WACKY_WORLDS,
    OBJECT_SOURCE_TIME_TWISTER,
    OBJECT_SOURCE_OPENRCT2_OFFICIAL,
    OBJECT_SOURCE_RCT1,
    OBJECT_SOURCE_ADDED_ATTRACTIONS,
    OBJECT_SOURCE_LOOPY_LANDSCAPES,
    OBJECT_SOURCE_RCT2 = 8
};

#pragma pack(push, 1)
/**
 * Object entry structure.
 * size: 0x10
 */
struct rct_object_entry
{
    union
    {
        uint8_t end_flag; // needed not to read past allocated buffer.
        uint32_t flags;
    };
    union
    {
        char nameWOC[12];
        struct
        {
            char name[8];
            uint32_t checksum;
        };
    };

    std::string_view GetName() const
    {
        return std::string_view(name, std::size(name));
    }

    void SetName(const std::string_view& value);

    uint8_t GetType() const
    {
        return flags & 0x0F;
    }

    std::optional<uint8_t> GetSceneryType() const;
};
assert_struct_size(rct_object_entry, 0x10);

struct rct_object_entry_group
{
    void** chunks;
    rct_object_entry* entries;
};
#ifdef PLATFORM_32BIT
assert_struct_size(rct_object_entry_group, 8);
#endif

struct rct_ride_filters
{
    uint8_t category[2];
    uint8_t ride_type;
};
assert_struct_size(rct_ride_filters, 3);

struct rct_object_filters
{
    union
    {
        rct_ride_filters ride;
    };
};
assert_struct_size(rct_object_filters, 3);
#pragma pack(pop)

struct IObjectRepository;
namespace OpenRCT2
{
    struct IStream;
}
struct ObjectRepositoryItem;
struct rct_drawpixelinfo;
struct json_t;

struct IReadObjectContext
{
    virtual ~IReadObjectContext() = default;

    virtual std::string_view GetObjectIdentifier() abstract;
    virtual IObjectRepository& GetObjectRepository() abstract;
    virtual bool ShouldLoadImages() abstract;
    virtual std::vector<uint8_t> GetData(const std::string_view& path) abstract;

    virtual void LogWarning(uint32_t code, const utf8* text) abstract;
    virtual void LogError(uint32_t code, const utf8* text) abstract;
};

#ifdef __WARN_SUGGEST_FINAL_TYPES__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsuggest-final-types"
#    pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#endif
class Object
{
private:
    std::string _identifier;
    rct_object_entry _objectEntry{};
    StringTable _stringTable;
    ImageTable _imageTable;
    std::vector<uint8_t> _sourceGames;
    std::vector<std::string> _authors;
    bool _isJsonObject{};

protected:
    StringTable& GetStringTable()
    {
        return _stringTable;
    }
    const StringTable& GetStringTable() const
    {
        return _stringTable;
    }
    ImageTable& GetImageTable()
    {
        return _imageTable;
    }

    std::string GetOverrideString(uint8_t index) const;
    std::string GetString(ObjectStringID index) const;
    std::string GetString(int32_t language, ObjectStringID index) const;

public:
    explicit Object(const rct_object_entry& entry);
    virtual ~Object() = default;

    std::string_view GetIdentifier() const
    {
        return _identifier;
    }
    void SetIdentifier(const std::string_view& identifier)
    {
        _identifier = identifier;
    }

    void MarkAsJsonObject()
    {
        _isJsonObject = true;
    }

    bool IsJsonObject() const
    {
        return _isJsonObject;
    };

    // Legacy data structures
    std::string_view GetLegacyIdentifier() const
    {
        return _objectEntry.GetName();
    }
    const rct_object_entry* GetObjectEntry() const
    {
        return &_objectEntry;
    }
    virtual void* GetLegacyData();

    virtual void ReadJson(IReadObjectContext* /*context*/, const json_t* /*root*/)
    {
    }
    virtual void ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream);
    virtual void Load() abstract;
    virtual void Unload() abstract;

    virtual void DrawPreview(rct_drawpixelinfo* /*dpi*/, int32_t /*width*/, int32_t /*height*/) const
    {
    }

    virtual uint8_t GetObjectType() const final
    {
        return _objectEntry.GetType();
    }
    virtual std::string GetName() const;
    virtual std::string GetName(int32_t language) const;

    virtual void SetRepositoryItem(ObjectRepositoryItem* /*item*/) const
    {
    }
    std::vector<uint8_t> GetSourceGames();
    void SetSourceGames(const std::vector<uint8_t>& sourceGames);

    const std::vector<std::string>& GetAuthors() const;
    void SetAuthors(const std::vector<std::string>&& authors);

    const ImageTable& GetImageTable() const
    {
        return _imageTable;
    }

    rct_object_entry GetScgWallsHeader();
    rct_object_entry GetScgPathXHeader();
    rct_object_entry CreateHeader(const char name[9], uint32_t flags, uint32_t checksum);

    uint32_t GetNumImages() const
    {
        return GetImageTable().GetCount();
    }
};
#ifdef __WARN_SUGGEST_FINAL_TYPES__
#    pragma GCC diagnostic pop
#endif

enum OBJECT_ERROR : uint32_t
{
    OBJECT_ERROR_OK,
    OBJECT_ERROR_UNKNOWN,
    OBJECT_ERROR_BAD_ENCODING,
    OBJECT_ERROR_INVALID_PROPERTY,
    OBJECT_ERROR_BAD_STRING_TABLE,
    OBJECT_ERROR_BAD_IMAGE_TABLE,
    OBJECT_ERROR_UNEXPECTED_EOF,
};

extern int32_t object_entry_group_counts[];
extern int32_t object_entry_group_encoding[];

bool object_entry_is_empty(const rct_object_entry* entry);
bool object_entry_compare(const rct_object_entry* a, const rct_object_entry* b);
int32_t object_calculate_checksum(const rct_object_entry* entry, const void* data, size_t dataLength);
bool find_object_in_entry_group(const rct_object_entry* entry, uint8_t* entry_type, ObjectEntryIndex* entryIndex);
void object_create_identifier_name(char* string_buffer, size_t size, const rct_object_entry* object);

const rct_object_entry* object_list_find(rct_object_entry* entry);

void object_entry_get_name_fixed(utf8* buffer, size_t bufferSize, const rct_object_entry* entry);

void* object_entry_get_chunk(int32_t objectType, ObjectEntryIndex index);
const rct_object_entry* object_entry_get_entry(int32_t objectType, ObjectEntryIndex index);
