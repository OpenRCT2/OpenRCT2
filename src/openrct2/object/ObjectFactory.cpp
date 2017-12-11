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

#include "../core/Console.hpp"
#include "../core/FileStream.hpp"
#include "../core/Json.hpp"
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../core/String.hpp"
#include "../rct12/SawyerChunkReader.h"
#include "BannerObject.h"
#include "EntranceObject.h"
#include "FootpathItemObject.h"
#include "FootpathObject.h"
#include "LargeSceneryObject.h"
#include "Object.h"
#include "ObjectLimits.h"
#include "ObjectList.h"
#include "ObjectFactory.h"
#include "RideObject.h"
#include "SceneryGroupObject.h"
#include "SmallSceneryObject.h"
#include "StexObject.h"
#include "WallObject.h"
#include "WaterObject.h"

class ReadObjectContext : public IReadObjectContext
{
private:
    utf8 *  _objectName;
    bool    _wasWarning = false;
    bool    _wasError = false;

public:
    bool WasWarning() const { return _wasWarning; }
    bool WasError() const { return _wasError; }

    explicit ReadObjectContext(const utf8 * objectFileName)
    {
        _objectName = String::Duplicate(objectFileName);
    }

    ~ReadObjectContext() override
    {
        Memory::Free(_objectName);
        _objectName = nullptr;
    }

    void LogWarning(uint32 code, const utf8 * text) override
    {
        _wasWarning = true;

        if (!String::IsNullOrEmpty(text))
        {
            log_verbose("[%s] Warning: %s", _objectName, text);
        }
    }

    void LogError(uint32 code, const utf8 * text) override
    {
        _wasError = true;

        if (!String::IsNullOrEmpty(text))
        {
            Console::Error::WriteLine("[%s] Error: %s", _objectName, text);
        }
    }
};

namespace ObjectFactory
{
    static void ReadObjectLegacy(Object * object, IReadObjectContext * context, IStream * stream)
    {
        try
        {
            object->ReadLegacy(context, stream);
        }
        catch (const IOException &)
        {
            // TODO check that ex is really EOF and not some other error
            context->LogError(OBJECT_ERROR_UNEXPECTED_EOF, "Unexpectedly reached end of file.");
        }
        catch (const std::exception &)
        {
            context->LogError(OBJECT_ERROR_UNKNOWN, nullptr);
        }
    }

    Object * CreateObjectFromLegacyFile(const utf8 * path)
    {
        log_verbose("CreateObjectFromLegacyFile(\"%s\")", path);

        Object * result = nullptr;
        try
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);
            auto chunkReader = SawyerChunkReader(&fs);

            rct_object_entry entry = fs.ReadValue<rct_object_entry>();
            result = CreateObject(entry);

            utf8 objectName[DAT_NAME_LENGTH + 1] = { 0 };
            object_entry_get_name_fixed(objectName, sizeof(objectName), &entry);
            log_verbose("  entry: { 0x%08X, \"%s\", 0x%08X }", entry.flags, objectName, entry.checksum);

            auto chunk = chunkReader.ReadChunk();
            log_verbose("  size: %zu", chunk->GetLength());

            auto chunkStream = MemoryStream(chunk->GetData(), chunk->GetLength());
            auto readContext = ReadObjectContext(objectName);
            ReadObjectLegacy(result, &readContext, &chunkStream);
            if (readContext.WasError())
            {
                throw std::runtime_error("Object has errors");
            }
        }
        catch (const std::exception &)
        {
            Console::Error::WriteLine("Unable to open or read '%s'", path);

            delete result;
            result = nullptr;
        }
        return result;
    }

    Object * CreateObjectFromLegacyData(const rct_object_entry * entry, const void * data, size_t dataSize)
    {
        Guard::ArgumentNotNull(entry, GUARD_LINE);
        Guard::ArgumentNotNull(data, GUARD_LINE);

        Object * result = CreateObject(*entry);
        if (result != nullptr)
        {
            utf8 objectName[DAT_NAME_LENGTH + 1];
            object_entry_get_name_fixed(objectName, sizeof(objectName), entry);

            auto readContext = ReadObjectContext(objectName);
            auto chunkStream = MemoryStream(data, dataSize);
            ReadObjectLegacy(result, &readContext, &chunkStream);

            if (readContext.WasError())
            {
                delete result;
                result = nullptr;
            }
        }
        return result;
    }

    Object * CreateObject(const rct_object_entry &entry)
    {
        Object * result;
        uint8 objectType = object_entry_get_type(&entry);
        switch (objectType) {
        case OBJECT_TYPE_RIDE:
            result = new RideObject(entry);
            break;
        case OBJECT_TYPE_SMALL_SCENERY:
            result = new SmallSceneryObject(entry);
            break;
        case OBJECT_TYPE_LARGE_SCENERY:
            result = new LargeSceneryObject(entry);
            break;
        case OBJECT_TYPE_WALLS:
            result = new WallObject(entry);
            break;
        case OBJECT_TYPE_BANNERS:
            result = new BannerObject(entry);
            break;
        case OBJECT_TYPE_PATHS:
            result = new FootpathObject(entry);
            break;
        case OBJECT_TYPE_PATH_BITS:
            result = new FootpathItemObject(entry);
            break;
        case OBJECT_TYPE_SCENERY_GROUP:
            result = new SceneryGroupObject(entry);
            break;
        case OBJECT_TYPE_PARK_ENTRANCE:
            result = new EntranceObject(entry);
            break;
        case OBJECT_TYPE_WATER:
            result = new WaterObject(entry);
            break;
        case OBJECT_TYPE_SCENARIO_TEXT:
            result = new StexObject(entry);
            break;
        default:
            throw std::runtime_error("Invalid object type");
        }
        return result;
    }

    static uint8 ParseObjectType(const std::string &s)
    {
        if (s == "ride") return OBJECT_TYPE_RIDE;
        if (s == "footpath") return OBJECT_TYPE_PATHS;
        if (s == "footpath_banner") return OBJECT_TYPE_BANNERS;
        if (s == "footpath_item") return OBJECT_TYPE_PATH_BITS;
        if (s == "park_entrance") return OBJECT_TYPE_PARK_ENTRANCE;
        return 0xFF;
    }

    Object * CreateObjectFromJsonFile(const std::string &path)
    {
        log_verbose("CreateObjectFromJsonFile(\"%s\")", path.c_str());

        Object * result = nullptr;
        try
        {
            auto jRoot = Json::ReadFromFile(path.c_str());
            auto jObjectType = json_object_get(jRoot, "objectType");
            if (json_is_string(jObjectType))
            {
                auto objectType = ParseObjectType(json_string_value(jObjectType));
                if (objectType != 0xFF)
                {
                    auto id = json_string_value(json_object_get(jRoot, "id"));

                    rct_object_entry entry = { 0 };
                    auto originalId = String::ToStd(json_string_value(json_object_get(jRoot, "originalId")));
                    auto originalName = originalId;
                    if (originalId.length() == 8 + 1 + 8 + 1 + 8)
                    {
                        entry.flags = std::stoul(originalId.substr(0, 8), 0, 16);
                        originalName = originalId.substr(9, 8);
                        entry.checksum = std::stoul(originalId.substr(18, 8), 0, 16);
                    }
                    auto minLength = std::min<size_t>(8, originalName.length());
                    memcpy(entry.name, originalName.c_str(), minLength);

                    result = CreateObject(entry);
                    auto readContext = ReadObjectContext(id);
                    result->ReadJson(&readContext, jRoot);
                    if (readContext.WasError())
                    {
                        throw Exception("Object has errors");
                    }
                }
            }
            json_decref(jRoot);
        }
        catch (Exception)
        {
            Console::Error::WriteLine("Unable to open or read '%s'", path.c_str());

            delete result;
            result = nullptr;
        }
        return result;
    }
}
