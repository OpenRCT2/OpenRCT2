/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/JsonFwd.hpp"
#include "../core/StringTypes.h"
#include "../drawing/ImageIndexType.h"
#include "ImageTable.h"
#include "ObjectAsset.h"
#include "ObjectTypes.h"
#include "StringTable.h"

#include <memory>
#include <optional>
#include <string_view>
#include <vector>

struct ObjectRepositoryItem;
using ride_type_t = uint16_t;

constexpr size_t VersionNumFields = 3;
using ObjectVersion = std::tuple<uint16_t, uint16_t, uint16_t>;
static_assert(std::tuple_size<ObjectVersion>{} == VersionNumFields);

namespace OpenRCT2::ObjectSelectionFlags
{
    constexpr uint8_t Selected = (1 << 0);
    constexpr uint8_t InUse = (1 << 2);
    // constexpr uint8_t Required = (1 << 3);               // Unused feature
    constexpr uint8_t AlwaysRequired = (1 << 4);
    constexpr uint8_t Flag6 = (1 << 5);
    constexpr uint8_t AllFlags = 0xFF;
}; // namespace OpenRCT2::ObjectSelectionFlags

enum class ObjectSourceGame : uint8_t
{
    Custom,
    WackyWorlds,
    TimeTwister,
    OpenRCT2Official,
    RCT1,
    AddedAttractions,
    LoopyLandscapes,
    RCT2 = 8
};

#pragma pack(push, 1)
/**
 * Object entry structure.
 * size: 0x10
 */
struct RCTObjectEntry
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

    void SetName(std::string_view value);

    ObjectType GetType() const
    {
        return static_cast<ObjectType>(flags & 0x0F);
    }

    void SetType(ObjectType newType)
    {
        flags &= ~0x0F;
        flags |= (static_cast<uint8_t>(newType) & 0x0F);
    }

    ObjectSourceGame GetSourceGame() const
    {
        return static_cast<ObjectSourceGame>((flags & 0xF0) >> 4);
    }

    bool IsEmpty() const;
    bool operator==(const RCTObjectEntry& rhs) const;
    bool operator!=(const RCTObjectEntry& rhs) const;
};
static_assert(sizeof(RCTObjectEntry) == 0x10);

#pragma pack(pop)

struct RideFilters
{
    uint8_t category[2];
    ride_type_t ride_type;
};

enum class ObjectGeneration : uint8_t
{
    DAT,
    JSON,
};

struct ObjectEntryDescriptor
{
    ObjectGeneration Generation = ObjectGeneration::JSON;

    // DAT
    RCTObjectEntry Entry{};

    // JSON
    ObjectType Type{};
    std::string Identifier;
    ObjectVersion Version;

    ObjectEntryDescriptor() = default;
    explicit ObjectEntryDescriptor(const RCTObjectEntry& newEntry);
    explicit ObjectEntryDescriptor(std::string_view newIdentifier);
    explicit ObjectEntryDescriptor(ObjectType type, std::string_view newIdentifier);
    explicit ObjectEntryDescriptor(const ObjectRepositoryItem& ori);
    bool HasValue() const;
    ObjectType GetType() const;
    std::string_view GetName() const;
    std::string ToString() const;

    bool operator==(const ObjectEntryDescriptor& rhs) const;
    bool operator!=(const ObjectEntryDescriptor& rhs) const;

    static ObjectEntryDescriptor Parse(std::string_view identifier);
};

struct IObjectRepository;
namespace OpenRCT2
{
    struct IStream;
}
struct ObjectRepositoryItem;
struct DrawPixelInfo;

enum class ObjectError : uint32_t
{
    Ok,
    Unknown,
    BadEncoding,
    InvalidProperty,
    BadStringTable,
    BadImageTable,
    UnexpectedEOF,
};

struct IReadObjectContext
{
    virtual ~IReadObjectContext() = default;

    virtual std::string_view GetObjectIdentifier() = 0;
    virtual IObjectRepository& GetObjectRepository() = 0;
    virtual bool ShouldLoadImages() = 0;
    virtual std::vector<uint8_t> GetData(std::string_view path) = 0;
    virtual ObjectAsset GetAsset(std::string_view path) = 0;

    virtual void LogVerbose(ObjectError code, const utf8* text) = 0;
    virtual void LogWarning(ObjectError code, const utf8* text) = 0;
    virtual void LogError(ObjectError code, const utf8* text) = 0;
};

#ifdef __WARN_SUGGEST_FINAL_TYPES__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsuggest-final-types"
    #pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#endif
class Object
{
private:
    std::string _identifier;
    ObjectVersion _version;
    ObjectEntryDescriptor _descriptor{};
    StringTable _stringTable;
    ImageTable _imageTable;
    std::vector<ObjectSourceGame> _sourceGames;
    std::vector<std::string> _authors;
    ObjectGeneration _generation{};
    bool _usesFallbackImages{};
    bool _isCompatibilityObject{};
    ImageIndex _baseImageId{ ImageIndexUndefined };

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

    /**
     * Populates the image and string tables from a JSON object
     * @param context
     * @param root JSON node of type object containing image and string info
     * @note root is deliberately left non-const: json_t behaviour changes when const
     */
    void PopulateTablesFromJson(IReadObjectContext* context, json_t& root);

    std::string GetString(ObjectStringID index) const;
    std::string GetString(int32_t language, ObjectStringID index) const;

public:
    virtual ~Object() = default;

    std::string_view GetIdentifier() const
    {
        return _identifier;
    }
    void SetIdentifier(std::string_view identifier)
    {
        _identifier = identifier;
    }

    void MarkAsJsonObject()
    {
        _generation = ObjectGeneration::JSON;
    }

    ObjectGeneration GetGeneration() const
    {
        return _generation;
    };

    ObjectType GetObjectType() const
    {
        return _descriptor.GetType();
    }

    ObjectEntryDescriptor GetDescriptor() const
    {
        return _descriptor;
    }
    void SetDescriptor(const ObjectEntryDescriptor& value)
    {
        _descriptor = value;
    }

    constexpr bool UsesFallbackImages() const
    {
        return _usesFallbackImages;
    }

    // DONOT USE THIS CAN LEAD TO OBJECT COLLISIONS
    std::string_view GetLegacyIdentifier() const
    {
        return _descriptor.GetName();
    }

    // TODO remove this, we should no longer assume objects have a legacy object entry
    const RCTObjectEntry& GetObjectEntry() const
    {
        return _descriptor.Entry;
    }
    virtual void* GetLegacyData();

    /**
     * @note root is deliberately left non-const: json_t behaviour changes when const
     */
    virtual void ReadJson(IReadObjectContext* /*context*/, json_t& /*root*/)
    {
    }
    virtual void ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream);
    virtual void Load() = 0;
    virtual void Unload() = 0;

    virtual void DrawPreview(DrawPixelInfo& /*dpi*/, int32_t /*width*/, int32_t /*height*/) const
    {
    }

    virtual std::string GetName() const;
    virtual std::string GetName(int32_t language) const;

    virtual void SetRepositoryItem(ObjectRepositoryItem* /*item*/) const
    {
    }
    std::vector<ObjectSourceGame> GetSourceGames();
    void SetSourceGames(const std::vector<ObjectSourceGame>& sourceGames);

    const std::vector<std::string>& GetAuthors() const;
    void SetAuthors(std::vector<std::string>&& authors);
    const ObjectVersion& GetVersion() const
    {
        return _version;
    }
    void SetVersion(const ObjectVersion& version)
    {
        _version = version;
    }

    bool IsCompatibilityObject() const;
    void SetIsCompatibilityObject(const bool on);

    const ImageTable& GetImageTable() const
    {
        return _imageTable;
    }

    ObjectEntryDescriptor GetScgPathXHeader() const;
    RCTObjectEntry CreateHeader(const char name[9], uint32_t flags, uint32_t checksum);

    uint32_t GetNumImages() const
    {
        return GetImageTable().GetCount();
    }

    ImageIndex GetBaseImageId() const
    {
        return _baseImageId;
    }

    uint32_t LoadImages();
    void UnloadImages();
};
#ifdef __WARN_SUGGEST_FINAL_TYPES__
    #pragma GCC diagnostic pop
#endif

int32_t ObjectCalculateChecksum(const RCTObjectEntry* entry, const void* data, size_t dataLength);
void ObjectCreateIdentifierName(char* string_buffer, size_t size, const RCTObjectEntry* object);

void ObjectEntryGetNameFixed(utf8* buffer, size_t bufferSize, const RCTObjectEntry* entry);

void* ObjectEntryGetChunk(ObjectType objectType, ObjectEntryIndex index);
const Object* ObjectEntryGetObject(ObjectType objectType, ObjectEntryIndex index);

constexpr bool IsIntransientObjectType(ObjectType type)
{
    return type == ObjectType::Audio;
}

u8string VersionString(const ObjectVersion& version);
ObjectVersion VersionTuple(std::string_view version);
