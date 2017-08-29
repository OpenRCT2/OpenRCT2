#pragma region Copyright (c) 2017 OpenRCT2 Developers
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

#include <string>
#include "../common.h"

template<typename TItem>
class FileIndex
{
private:
    struct DirectoryStats
    {
        uint32 TotalFiles = 0;
        uint64 TotalFileSize = 0;
        uint32 FileDateModifiedChecksum = 0;
        uint32 PathChecksum = 0;
    }

    struct ScanResult
    {
        DirectoryStats const Stats;
        std::vector<std::string> const Files;

        ScanResult(DirectoryStats stats, std::vector<std::string> files)
            : Stats(stats),
              Files(files)
        {
        }
    };

    struct FileIndexHeader
    {
        uint32          MagicNumber = 0;
        uint8           VersionA = 0;
        uint8           VersionB = 0;
        uint16          LanguageId = 0;
        DirectoryStats  DirectoryStats;
        uint32          NumItems = 0;
    };

    constexpr uint8 FILE_INDEX_VERSION = 4;

    uint32 _magicNumber;
    uint8 _version;
    std::string _indexPath;
    std::string _pattern;
    std::vector<std::string> _paths;

public:
    FileIndex(uint32 magicNumber,
              uint8 version,
              std::string indexPath,
              std::string pattern,
              std::vector<std::string> paths) :
        _magicNumber(magicNumber),
        _version(version),
        _indexPath(indexPath),
        _pattern(pattern),
        _paths(paths)
    {
    }

    /**
     * Queries and directories and loads the index header. If the index is up to date,
     * the items are loaded from the index and returned, otherwise the index is rebuilt.
     */
    std::vector<TItem> LoadOrBuild()
    {
        std::vector<TItem> items;
        auto scanResult = Scan();
        auto readIndexResult = ReadIndexFile(scanResult.Stats);
        if (std::get<0>(readIndexResult))
        {
            // Index was loaded
            items = std::get<1>(readIndexResult);
        }
        else
        {
            // Index was not loaded
            for (auto filePath : scanResult.Files)
            {
                auto item = Create(filePath);
                items.push_back(item);
            }
            WriteIndexFile(items);
        }
        return items;
    }

    /**
     * Loads the given file and creates the item representing the data to store in the index.
     */
    virtual TItem Create(const std::string &path) abstract;

    /**
     * Serialises an index item to the given stream.
     */
    virtual void Serialise(IStream * stream, const TItem item);

    /**
     * Deserialises an index item from the given stream.
     */
    virtual TItem Deserialise(IStream * stream) abstract;

private:
    ScanResult Scan()
    {
        ScanResult scanResult;
        for (const auto directory : _paths)
        {
            auto pattern = Path::Combine(directory, _pattern);
            auto scanner = Path::ScanDirectory(pattern, true);
            while (scanner->Next())
            {
                auto fileInfo = scanner->GetFileInfo();
                auto path = std::string(scanner->GetPath());

                scanResult.Files.push(path);

                scanResult.TotalFiles++;
                scanResult.TotalFileSize += fileInfo->Size;
                scanResult.FileDateModifiedChecksum ^=
                    (uint32)(fileInfo->LastModified >> 32) ^
                    (uint32)(fileInfo->LastModified & 0xFFFFFFFF);
                scanResult.FileDateModifiedChecksum = ror32(result->FileDateModifiedChecksum, 5);
                scanResult.PathChecksum += GetPathChecksum(path);
            }
            delete scanner;
        }
    }

    std::tuple<bool, std::vector<TItem>> ReadIndexFile(const DirectoryStats &stats)
    {
        bool loadedItems = false;
        std::vector<TItem> items;
        try
        {
            auto fs = FileStream(path, FILE_MODE_OPEN);

            // Read header, check if we need to re-scan
            auto header = fs.ReadValue<FileIndexHeader>();
            if (header.MagicNumber == _magicNumber &&
                header.VersionA == FILE_INDEX_VERSION &&
                header.VersionB == _version &&
                header.TotalFiles == scanResult.TotalFiles &&
                header.TotalFileSize == scanResult.TotalFileSize &&
                header.FileDateModifiedChecksum == scanResult.FileDateModifiedChecksum &&
                header.PathChecksum == scanResult.PathChecksum)
            {
                // Directory is the same, just read the saved items
                for (uint32 i = 0; i < header.NumItems; i++)
                {
                    auto item = Deserialise(fs);
                    items.push_back(item);
                }
                loadedItems = true;
            }
        }
        catch (const Exception &)
        {
            Console::Error::WriteLine("Unable to load index.");
        }
        return std::make_tuple(loadedItems, items);
    }

    void WriteIndexFile(const DirectoryStats &stats, const std::vector<TItem> &items)
    {
        try
        {
            auto fs = FileStream(_indexPath, FILE_MODE_WRITE);
    
            // Write header
            FileIndexHeader header = { 0 };
            header.MagicNumber = _magicNumber;
            header.Version = SCENARIO_REPOSITORY_VERSION;
            header.LanguageId = gCurrentLanguage;
            header.DirectoryStats = stats;
            header.NumItems = items.size();
            fs.WriteValue(header);
    
            // Write items
            for (const auto item : items)
            {
                Serialise(fs, item);
            }
            return true;
        }
        catch (const Exception &)
        {
            Console::Error::WriteLine("Unable to save index.");
            return false;
        }
    }
};
