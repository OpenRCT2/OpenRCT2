#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include "../common.h"
#include "../core/Json.hpp"
#include "ImageTable.h"
#include "StringTable.h"

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

#define OBJECT_ENTRY_COUNT 721

#pragma pack(push, 1)
/**
 * Object entry structure.
 * size: 0x10
 */
struct rct_object_entry {
    union {
        uint8 end_flag; // needed not to read past allocated buffer.
        uint32 flags;
    };
    union {
        char nameWOC[12];
        struct {
            char name[8];
            uint32 checksum;
        };
    };

};
assert_struct_size(rct_object_entry, 0x10);

struct rct_object_entry_group {
    void **chunks;
    rct_object_entry *entries;
};
#ifdef PLATFORM_32BIT
assert_struct_size(rct_object_entry_group, 8);
#endif

struct rct_ride_filters {
    uint8 category[2];
    uint8 ride_type;
};
assert_struct_size(rct_ride_filters, 3);

struct rct_object_filters {
    union {
        rct_ride_filters ride;
    };
};
assert_struct_size(rct_object_filters, 3);
#pragma pack(pop)

enum OBJ_STRING_ID
{
    OBJ_STRING_ID_NAME,
    OBJ_STRING_ID_DESCRIPTION,
    OBJ_STRING_ID_SCENARIO_NAME = 0,
    OBJ_STRING_ID_PARK_NAME = 1,
    OBJ_STRING_ID_SCENARIO_DETAILS = 2,
    OBJ_STRING_ID_CAPACITY = 2,
};

interface IStream;
struct    ObjectRepositoryItem;
struct    rct_drawpixelinfo;

interface IReadObjectContext
{
    virtual ~IReadObjectContext() = default;

    virtual void LogWarning(uint32 code, const utf8 * text) abstract;
    virtual void LogError(uint32 code, const utf8 * text) abstract;
};

#ifdef __WARN_SUGGEST_FINAL_TYPES__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsuggest-final-types"
    #pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#endif
class Object
{
private:
    char *              _identifier;
    rct_object_entry    _objectEntry;
    StringTable         _stringTable;
    ImageTable          _imageTable;

protected:
    StringTable *       GetStringTable() { return &_stringTable; }
    const StringTable * GetStringTable() const { return &_stringTable; }
    ImageTable  *       GetImageTable() { return &_imageTable; }

    std::string         GetOverrideString(uint8 index) const;
    std::string         GetString(uint8 index) const;

    void                SetSourceGame(const uint8 sourceGame);
    bool                IsRCT1Object();
    bool                IsAAObject();
    bool                IsLLObject();
    bool                IsOpenRCT2OfficialObject();

public:
    explicit Object(const rct_object_entry &entry);
    virtual ~Object();

    // Legacy data structures
    const char *                GetIdentifier() const { return _identifier; }
    const rct_object_entry *    GetObjectEntry() const { return &_objectEntry; }
    virtual void *              GetLegacyData() abstract;

    virtual void ReadJson(IReadObjectContext * context, const json_t * root) { }
    virtual void ReadLegacy(IReadObjectContext * context, IStream * stream) abstract;
    virtual void Load() abstract;
    virtual void Unload() abstract;

    virtual void DrawPreview(rct_drawpixelinfo * dpi, sint32 width, sint32 height) const { }

    virtual uint8           GetObjectType() const final { return _objectEntry.flags & 0x0F; }
    virtual std::string     GetName() const;

    virtual void SetRepositoryItem(ObjectRepositoryItem * item) const { }

    const ImageTable * GetImageTable() const { return &_imageTable; }

    rct_object_entry GetScgWallsHeader();
    rct_object_entry GetScgPathXHeader();
    rct_object_entry CreateHeader(const char name[9], uint32 flags, uint32 checksum);

};
#ifdef __WARN_SUGGEST_FINAL_TYPES__
    #pragma GCC diagnostic pop
#endif

enum OBJECT_ERROR : uint32
{
    OBJECT_ERROR_OK,
    OBJECT_ERROR_UNKNOWN,
    OBJECT_ERROR_BAD_ENCODING,
    OBJECT_ERROR_INVALID_PROPERTY,
    OBJECT_ERROR_BAD_STRING_TABLE,
    OBJECT_ERROR_BAD_IMAGE_TABLE,
    OBJECT_ERROR_UNEXPECTED_EOF,
};

extern sint32 object_entry_group_counts[];
extern sint32 object_entry_group_encoding[];

void object_list_load();

bool object_entry_is_empty(const rct_object_entry *entry);
bool object_entry_compare(const rct_object_entry *a, const rct_object_entry *b);
sint32 object_calculate_checksum(const rct_object_entry * entry, const void * data, size_t dataLength);
bool find_object_in_entry_group(const rct_object_entry* entry, uint8* entry_type, uint8* entry_index);
void object_create_identifier_name(char* string_buffer, size_t size, const rct_object_entry* object);

const rct_object_entry * object_list_find_by_name(const char *name);
const rct_object_entry * object_list_find(rct_object_entry *entry);

void object_entry_get_name_fixed(utf8 * buffer, size_t bufferSize, const rct_object_entry * entry);

void * object_entry_get_chunk(sint32 objectType, size_t index);
const rct_object_entry * object_entry_get_entry(sint32 objectType, size_t index);
