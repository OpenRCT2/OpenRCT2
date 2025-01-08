/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Csg.h"

#include "../core/FileStream.h"
#include "../core/Path.hpp"
#include "../drawing/Drawing.h"
#include "../rct1/Limits.h"

using namespace OpenRCT2;

std::string FindCsg1datAtLocation(u8string_view path)
{
    auto checkPath1 = Path::Combine(path, u8"Data", u8"CSG1.DAT");
    auto checkPath2 = Path::Combine(path, u8"Data", u8"CSG1.1");

    // Since Linux is case sensitive (and macOS sometimes too), make sure we handle case properly.
    std::string path1result = Path::ResolveCasing(checkPath1);
    if (!path1result.empty())
    {
        return path1result;
    }

    std::string path2result = Path::ResolveCasing(checkPath2);
    return path2result;
}

bool Csg1datPresentAtLocation(u8string_view path)
{
    auto location = FindCsg1datAtLocation(path);
    return !location.empty();
}

u8string FindCsg1idatAtLocation(u8string_view path)
{
    auto result1 = Path::ResolveCasing(Path::Combine(path, u8"Data", u8"CSG1I.DAT"));
    if (!result1.empty())
    {
        return result1;
    }
    auto result2 = Path::ResolveCasing(Path::Combine(path, u8"RCTdeluxe_install", u8"Data", u8"CSG1I.DAT"));
    return result2;
}

bool Csg1idatPresentAtLocation(u8string_view path)
{
    std::string location = FindCsg1idatAtLocation(path);
    return !location.empty();
}

bool RCT1DataPresentAtLocation(u8string_view path)
{
    return Csg1datPresentAtLocation(path) && Csg1idatPresentAtLocation(path) && CsgAtLocationIsUsable(path);
}

bool CsgIsUsable(const Gx& csg)
{
    return csg.header.total_size == RCT1::Limits::kLLCsg1DatFileSize
        && csg.header.num_entries == RCT1::Limits::kNumLLCsgEntries;
}

bool CsgAtLocationIsUsable(u8string_view path)
{
    auto csg1HeaderPath = FindCsg1idatAtLocation(path);
    if (csg1HeaderPath.empty())
    {
        return false;
    }

    auto csg1DataPath = FindCsg1datAtLocation(path);
    if (csg1DataPath.empty())
    {
        return false;
    }

    auto fileHeader = FileStream(csg1HeaderPath, FILE_MODE_OPEN);
    auto fileData = FileStream(csg1DataPath, FILE_MODE_OPEN);
    size_t fileHeaderSize = fileHeader.GetLength();
    size_t fileDataSize = fileData.GetLength();

    Gx csg = {};
    csg.header.num_entries = static_cast<uint32_t>(fileHeaderSize / sizeof(RCTG1Element));
    csg.header.total_size = static_cast<uint32_t>(fileDataSize);
    return CsgIsUsable(csg);
}
