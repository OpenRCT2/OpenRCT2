#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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
#include "../core/Memory.hpp"
#include "../core/MemoryStream.h"
#include "../core/String.hpp"
#include "../core/Path.hpp"
#include "BannerObject.h"
#include "EntranceObject.h"
#include "FootpathItemObject.h"
#include "FootpathObject.h"
#include "LargeSceneryObject.h"
#include "Object.h"
#include "ObjectFactory.h"
#include "RideObject.h"
#include "SceneryGroupObject.h"
#include "SmallSceneryObject.h"
#include "StexObject.h"
#include "WallObject.h"
#include "WaterObject.h"

extern "C"
{
    #include "../object.h"
    #include "../util/sawyercoding.h"
}

class ReadObjectContext : public IReadObjectContext
{
private:
    utf8 *  _objectName;
    bool    _wasWarning = false;
    bool    _wasError = false;

public:
    bool WasWarning() const { return _wasWarning; }
    bool WasError() const { return _wasError; }

    ReadObjectContext(const utf8 * objectFileName)
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
            Console::Error::WriteFormat("[%s] Warning: %s", _objectName, text);
            Console::Error::WriteLine();
        }
    }

    void LogError(uint32 code, const utf8 * text) override
    {
        _wasError = true;

        if (!String::IsNullOrEmpty(text))
        {
            Console::Error::WriteFormat("[%s] Error: %s", _objectName, text);
            Console::Error::WriteLine();
        }
    }
};

namespace ObjectFactory
{
    Object * CreateObjectFromLegacyFile(const utf8 * path)
    {
        Object * result = nullptr;

        SDL_RWops * file = SDL_RWFromFile(path, "rb");
        if (file != nullptr)
        {
            rct_object_entry entry;
            if (SDL_RWread(file, &entry, sizeof(entry), 1) == 1)
            {
                result = CreateObject(entry);
                if (result != nullptr)
                {
                    size_t bufferSize = 0x600000;
                    void * buffer = Memory::Allocate<void>(bufferSize);
                    if (buffer == nullptr)
                    {
                        log_error("Unable to allocate data buffer.");
                        delete result;
                        result = nullptr;
                    }
                    else
                    {
                        utf8 objectName[9] = { 0 };
                        Memory::Copy(objectName, entry.name, 8);

                        auto readContext = ReadObjectContext(objectName);
                        try
                        {
                            bufferSize = sawyercoding_read_chunk_with_size(file, (uint8 *)buffer, bufferSize);
                            if (bufferSize == SIZE_MAX)
                            {
                                readContext.LogError(OBJECT_ERROR_BAD_ENCODING, "Unable to decode chunk.");
                            }
                            else
                            {
                                buffer = Memory::Reallocate(buffer, bufferSize);
                                auto ms = MemoryStream(buffer, bufferSize);
                                result->ReadLegacy(&readContext, &ms);
                            }
                        }
                        catch (IOException ex)
                        {
                            // TODO check that ex is really EOF and not some other error
                            readContext.LogError(OBJECT_ERROR_UNEXPECTED_EOF, "Unexpectedly reached end of file.");
                        }
                        catch (Exception ex)
                        {
                            readContext.LogError(OBJECT_ERROR_UNKNOWN, nullptr);
                        }
                        
                        Memory::Free(buffer);
                        if (readContext.WasError())
                        {
                            Console::Error::WriteFormat("Error reading object: '%s'", path);
                            Console::Error::WriteLine();

                            delete result;
                            result = nullptr;
                        }
                    }
                }
            }
            SDL_RWclose(file);
        }
        return result;
    }

    Object * CreateObject(const rct_object_entry &entry)
    {
        Object * result = nullptr;

        uint8 objectType = entry.flags & 0x0F;
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
        case OBJECT_TYPE_SCENERY_SETS:
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
        }

        return result;
    }
}
