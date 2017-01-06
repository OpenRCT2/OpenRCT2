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

#include "Console.hpp"
#include "File.h"
#include "FileStream.hpp"
#include "String.hpp"

extern "C"
{
    #include "../platform/platform.h"
}

namespace File
{
    bool Copy(const std::string &srcPath, const std::string &dstPath, bool overwrite)
    {
        return platform_file_copy(srcPath.c_str(), dstPath.c_str(), overwrite);
    }

    bool Delete(const std::string &path)
    {
        return platform_file_delete(path.c_str());
    }

    bool Move(const std::string &srcPath, const std::string &dstPath)
    {
        return platform_file_move(srcPath.c_str(), dstPath.c_str());
    }

    void * ReadAllBytes(const std::string &path, size_t * length)
    {
        void * result = nullptr;

        FileStream fs = FileStream(path, FILE_MODE_OPEN);
        uint64 fsize = fs.GetLength();
        if (fsize > SIZE_MAX)
        {
            std::string message = String::StdFormat("'%s' exceeds maximum length of %lld bytes.", SIZE_MAX);
            throw IOException(message);
        }
        else
        {
            result = fs.ReadArray<uint8>((size_t)fsize);
        }
        *length = (size_t)fsize;
        return result;
    }
}
