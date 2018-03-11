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

#include "core/Console.hpp"
#include "core/FileStream.hpp"
#include "core/Path.hpp"
#include "FileClassifier.h"
#include "rct12/SawyerChunkReader.h"

#include "scenario/Scenario.h"
#include "util/SawyerCoding.h"

static bool TryClassifyAsS6(IStream * stream, ClassifiedFileInfo * result);
static bool TryClassifyAsS4(IStream * stream, ClassifiedFileInfo * result);
static bool TryClassifyAsTD4_TD6(IStream * stream, ClassifiedFileInfo * result);

bool TryClassifyFile(const std::string &path, ClassifiedFileInfo * result)
{
    try
    {
        auto fs = FileStream(path, FILE_MODE_OPEN);
        return TryClassifyFile(&fs, result);
    }
    catch (const std::exception &)
    {
        return false;
    }
}

bool TryClassifyFile(IStream * stream, ClassifiedFileInfo * result)
{
    // TODO Currently track designs get classified as SC4s because they use the
    //      same checksum algorithm. The only way after to tell the difference
    //      between them is to decode it. Decoding however is currently not protected
    //      against invalid compression data for that decoding algorithm and will crash.

    // S6 detection
    if (TryClassifyAsS6(stream, result))
    {
        return true;
    }

    // S4 detection
    if (TryClassifyAsS4(stream, result))
    {
        return true;
    }

    // TD6 detection
    if (TryClassifyAsTD4_TD6(stream, result))
    {
        return true;
    }

    return false;
}

static bool TryClassifyAsS6(IStream * stream, ClassifiedFileInfo * result)
{
    bool success = false;
    uint64 originalPosition = stream->GetPosition();
    try
    {
        auto chunkReader = SawyerChunkReader(stream);
        auto s6Header = chunkReader.ReadChunkAs<rct_s6_header>();
        if (s6Header.type == S6_TYPE_SAVEDGAME)
        {
            result->Type = FILE_TYPE::SAVED_GAME;
        }
        else if (s6Header.type == S6_TYPE_SCENARIO)
        {
            result->Type = FILE_TYPE::SCENARIO;
        }
        result->Version = s6Header.version;
        success = true;
    }
    catch (const std::exception &e)
    {
        // Exceptions are likely to occur if file is not S6 format
        log_verbose(e.what());
    }
    stream->SetPosition(originalPosition);
    return success;
}

static bool TryClassifyAsS4(IStream * stream, ClassifiedFileInfo * result)
{
    bool success = false;
    uint64 originalPosition = stream->GetPosition();
    try
    {
        size_t dataLength = (size_t)stream->GetLength();
        auto deleter_lambda = [dataLength](uint8 * ptr) { Memory::FreeArray(ptr, dataLength); };
        std::unique_ptr<uint8, decltype(deleter_lambda)> data(stream->ReadArray<uint8>(dataLength), deleter_lambda);
        stream->SetPosition(originalPosition);
        sint32 fileTypeVersion = sawyercoding_detect_file_type(data.get(), dataLength);

        sint32 type = fileTypeVersion & FILE_TYPE_MASK;
        sint32 version = fileTypeVersion & FILE_VERSION_MASK;

        if (type == FILE_TYPE_SV4)
        {
            result->Type = FILE_TYPE::SAVED_GAME;
            result->Version = version;
            success = true;
        }
        else if (type == FILE_TYPE_SC4)
        {
            result->Type = FILE_TYPE::SCENARIO;
            result->Version = version;
            success = true;
        }
    }
    catch (const std::exception &e)
    {
        Console::Error::WriteLine(e.what());
    }

    stream->SetPosition(originalPosition);
    return success;
}

static bool TryClassifyAsTD4_TD6(IStream * stream, ClassifiedFileInfo * result)
{
    bool success = false;
    uint64 originalPosition = stream->GetPosition();
    try
    {
        size_t dataLength = (size_t)stream->GetLength();
        auto deleter_lambda = [dataLength](uint8 * ptr) { Memory::FreeArray(ptr, dataLength); };
        std::unique_ptr<uint8, decltype(deleter_lambda)> data(stream->ReadArray<uint8>(dataLength), deleter_lambda);
        stream->SetPosition(originalPosition);

        if (sawyercoding_validate_track_checksum(data.get(), dataLength))
        {
            std::unique_ptr<uint8, decltype(&Memory::Free<uint8>)> td6data(Memory::Allocate<uint8>(0x10000), &Memory::Free<uint8>);
            size_t td6len = sawyercoding_decode_td6(data.get(), td6data.get(), dataLength);
            if (td6data != nullptr && td6len >= 8)
            {
                uint8 version = (td6data.get()[7] >> 2) & 3;
                if (version <= 2)
                {
                    result->Type = FILE_TYPE::TRACK_DESIGN;
                    result->Version = version;
                    success = true;
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        Console::Error::WriteLine(e.what());
    }

    return success;
}

uint32 get_file_extension_type(const utf8 * path)
{
    auto extension = Path::GetExtension(path);
    if (String::Equals(extension, ".dat", true)) return FILE_EXTENSION_DAT;
    if (String::Equals(extension, ".sc4", true)) return FILE_EXTENSION_SC4;
    if (String::Equals(extension, ".sv4", true)) return FILE_EXTENSION_SV4;
    if (String::Equals(extension, ".td4", true)) return FILE_EXTENSION_TD4;
    if (String::Equals(extension, ".sc6", true)) return FILE_EXTENSION_SC6;
    if (String::Equals(extension, ".sv6", true)) return FILE_EXTENSION_SV6;
    if (String::Equals(extension, ".td6", true)) return FILE_EXTENSION_TD6;
    return FILE_EXTENSION_UNKNOWN;
}

