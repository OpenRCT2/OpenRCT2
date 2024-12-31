/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../Diagnostic.h"
#include "Console.hpp"
#include "DataSerialiser.h"
#include "File.h"
#include "FileScanner.h"
#include "FileStream.h"
#include "JobPool.h"
#include "Numerics.hpp"
#include "Path.hpp"

#include <chrono>
#include <list>
#include <string>
#include <tuple>
#include <vector>

template<typename TItem>
class FileIndex
{
private:
    struct DirectoryStats
    {
        uint32_t TotalFiles = 0;
        uint64_t TotalFileSize = 0;
        uint32_t FileDateModifiedChecksum = 0;
        uint32_t PathChecksum = 0;
    };

    struct ScanResult
    {
        DirectoryStats const Stats;
        std::vector<std::string> const Files;

        ScanResult(DirectoryStats stats, std::vector<std::string>&& files) noexcept
            : Stats(stats)
            , Files(std::move(files))
        {
        }
    };

    struct FileIndexHeader
    {
        uint32_t HeaderSize = sizeof(FileIndexHeader);
        uint32_t MagicNumber = 0;
        uint8_t VersionA = 0;
        uint8_t VersionB = 0;
        uint16_t LanguageId = 0;
        DirectoryStats Stats;
        uint32_t NumItems = 0;
    };

    // Index file format version which when incremented forces a rebuild
    static constexpr uint8_t FILE_INDEX_VERSION = 4;

    std::string const _name;
    uint32_t const _magicNumber;
    uint8_t const _version;
    std::string const _indexPath;
    std::string const _pattern;

public:
    std::vector<std::string> const SearchPaths;

public:
    /**
     * Creates a new FileIndex.
     * @param name Name of the index (used for logging).
     * @param magicNumber Magic number for the index (to distinguish between different index files).
     * @param version Version of the specialised index, increment this to force a rebuild.
     * @param indexPath Full path to read and write the index file to.
     * @param pattern The search pattern for indexing files.
     * @param paths A list of search directories.
     */
    FileIndex(
        std::string&& name, uint32_t magicNumber, uint8_t version, std::string&& indexPath, std::string&& pattern,
        std::vector<std::string>&& paths) noexcept
        : _name(std::move(name))
        , _magicNumber(magicNumber)
        , _version(version)
        , _indexPath(std::move(indexPath))
        , _pattern(std::move(pattern))
        , SearchPaths(std::move(paths))
    {
    }

    virtual ~FileIndex() = default;

    /**
     * Queries and directories and loads the index header. If the index is up to date,
     * the items are loaded from the index and returned, otherwise the index is rebuilt.
     */
    std::vector<TItem> LoadOrBuild(int32_t language) const
    {
        std::vector<TItem> items;
        auto scanResult = Scan();
        auto readIndexResult = ReadIndexFile(language, scanResult.Stats);
        if (std::get<0>(readIndexResult))
        {
            // Index was loaded
            items = std::get<1>(readIndexResult);
        }
        else
        {
            // Index was not loaded
            items = Build(language, scanResult);
        }
        return items;
    }

    std::vector<TItem> Rebuild(int32_t language) const
    {
        auto scanResult = Scan();
        auto items = Build(language, scanResult);
        return items;
    }

protected:
    /**
     * Loads the given file and creates the item representing the data to store in the index.
     */
    virtual std::optional<TItem> Create(int32_t language, const std::string& path) const = 0;

    /**
     * Serialises/DeSerialises an index item to/from the given stream.
     */
    virtual void Serialise(DataSerialiser& ds, const TItem& item) const = 0;

private:
    ScanResult Scan() const
    {
        DirectoryStats stats{};
        std::vector<std::string> files;
        for (const auto& directory : SearchPaths)
        {
            auto absoluteDirectory = OpenRCT2::Path::GetAbsolute(directory);
            LOG_VERBOSE("FileIndex:Scanning for %s in '%s'", _pattern.c_str(), absoluteDirectory.c_str());

            auto pattern = OpenRCT2::Path::Combine(absoluteDirectory, _pattern);
            auto scanner = OpenRCT2::Path::ScanDirectory(pattern, true);
            while (scanner->Next())
            {
                const auto& fileInfo = scanner->GetFileInfo();
                auto path = scanner->GetPath();

                stats.TotalFiles++;
                stats.TotalFileSize += fileInfo.Size;
                stats.FileDateModifiedChecksum ^= static_cast<uint32_t>(fileInfo.LastModified >> 32)
                    ^ static_cast<uint32_t>(fileInfo.LastModified & 0xFFFFFFFF);
                stats.FileDateModifiedChecksum = OpenRCT2::Numerics::ror32(stats.FileDateModifiedChecksum, 5);
                stats.PathChecksum += GetPathChecksum(path);

                files.push_back(std::move(path));
            }
        }
        return ScanResult(stats, std::move(files));
    }

    std::vector<TItem> Build(int32_t language, const ScanResult& scanResult) const
    {
        std::vector<TItem> allItems;
        OpenRCT2::Console::WriteLine("Building %s (%zu items)", _name.c_str(), scanResult.Files.size());

        auto startTime = std::chrono::high_resolution_clock::now();

        const size_t totalCount = scanResult.Files.size();
        if (totalCount > 0)
        {
            JobPool jobPool;
            std::mutex mtx;
            std::atomic<size_t> processed{ 0 };

            for (size_t i = 0; i < totalCount; i++)
            {
                jobPool.AddTask([&, index = i]() {
                    const auto& filePath = scanResult.Files.at(index);

                    if (auto item = Create(language, filePath); item.has_value())
                    {
                        std::lock_guard lock(mtx);
                        allItems.push_back(std::move(item.value()));
                    }

                    processed++;
                });
            }

            jobPool.Join([&]() {
                OpenRCT2::GetContext()->SetProgress(static_cast<uint32_t>(processed.load()), static_cast<uint32_t>(totalCount));
            });
        }

        WriteIndexFile(language, scanResult.Stats, allItems);

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<float>(endTime - startTime);
        OpenRCT2::Console::WriteLine("Finished building %s in %.2f seconds.", _name.c_str(), duration.count());

        return allItems;
    }

    std::tuple<bool, std::vector<TItem>> ReadIndexFile(int32_t language, const DirectoryStats& stats) const
    {
        bool loadedItems = false;
        std::vector<TItem> items;
        if (OpenRCT2::File::Exists(_indexPath))
        {
            try
            {
                LOG_VERBOSE("FileIndex:Loading index: '%s'", _indexPath.c_str());
                auto fs = OpenRCT2::FileStream(_indexPath, OpenRCT2::FILE_MODE_OPEN);

                // Read header, check if we need to re-scan
                auto header = fs.ReadValue<FileIndexHeader>();
                if (header.HeaderSize == sizeof(FileIndexHeader) && header.MagicNumber == _magicNumber
                    && header.VersionA == FILE_INDEX_VERSION && header.VersionB == _version && header.LanguageId == language
                    && header.Stats.TotalFiles == stats.TotalFiles && header.Stats.TotalFileSize == stats.TotalFileSize
                    && header.Stats.FileDateModifiedChecksum == stats.FileDateModifiedChecksum
                    && header.Stats.PathChecksum == stats.PathChecksum)
                {
                    items.reserve(header.NumItems);
                    DataSerialiser ds(false, fs);
                    // Directory is the same, just read the saved items
                    for (uint32_t i = 0; i < header.NumItems; i++)
                    {
                        TItem item;
                        Serialise(ds, item);
                        items.emplace_back(std::move(item));
                    }
                    loadedItems = true;
                }
                else
                {
                    OpenRCT2::Console::WriteLine("%s out of date", _name.c_str());
                }
            }
            catch (const std::exception& e)
            {
                OpenRCT2::Console::Error::WriteLine("Unable to load index: '%s'.", _indexPath.c_str());
                OpenRCT2::Console::Error::WriteLine("%s", e.what());
            }
        }
        return std::make_tuple(loadedItems, std::move(items));
    }

    void WriteIndexFile(int32_t language, const DirectoryStats& stats, const std::vector<TItem>& items) const
    {
        try
        {
            LOG_VERBOSE("FileIndex:Writing index: '%s'", _indexPath.c_str());
            OpenRCT2::Path::CreateDirectory(OpenRCT2::Path::GetDirectory(_indexPath));
            auto fs = OpenRCT2::FileStream(_indexPath, OpenRCT2::FILE_MODE_WRITE);

            // Write header
            FileIndexHeader header;
            header.MagicNumber = _magicNumber;
            header.VersionA = FILE_INDEX_VERSION;
            header.VersionB = _version;
            header.LanguageId = language;
            header.Stats = stats;
            header.NumItems = static_cast<uint32_t>(items.size());
            fs.WriteValue(header);

            DataSerialiser ds(true, fs);
            // Write items
            for (const auto& item : items)
            {
                Serialise(ds, item);
            }
        }
        catch (const std::exception& e)
        {
            OpenRCT2::Console::Error::WriteLine("Unable to save index: '%s'.", _indexPath.c_str());
            OpenRCT2::Console::Error::WriteLine("%s", e.what());
        }
    }

    static uint32_t GetPathChecksum(const std::string& path)
    {
        uint32_t hash = 0xD8430DED;
        for (const utf8* ch = path.c_str(); *ch != '\0'; ch++)
        {
            hash += (*ch);
            hash += (hash << 10);
            hash ^= (hash >> 6);
        }
        hash += (hash << 3);
        hash ^= (hash >> 11);
        hash += (hash << 15);
        return hash;
    }
};
