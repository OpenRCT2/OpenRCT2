/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ResourceTable.h"

#include "../Context.h"
#include "../PlatformEnvironment.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"

using namespace OpenRCT2;

Range<int32_t> ResourceTable::ParseRange(std::string_view s)
{
    // Currently only supports [###] or [###..###]
    Range<int32_t> result = {};
    if (s.length() >= 3 && s[0] == '[' && s[s.length() - 1] == ']')
    {
        s = s.substr(1, s.length() - 2);
        auto parts = String::Split(s, "..");
        if (parts.size() == 1)
        {
            result = Range<int32_t>(std::stoi(parts[0]));
        }
        else
        {
            auto left = std::stoi(parts[0]);
            auto right = std::stoi(parts[1]);
            if (left <= right)
            {
                result = Range<int32_t>(left, right);
            }
            else
            {
                result = Range<int32_t>(right, left);
            }
        }
    }
    return result;
}

ResourceTable::SourceInfo ResourceTable::ParseSource(std::string_view source)
{
    SourceInfo info;
    auto base = source;
    auto rangeStart = source.find('[');
    if (rangeStart != std::string::npos)
    {
        base = source.substr(0, rangeStart);
        info.SourceRange = ParseRange(source.substr(rangeStart));
    }

    auto fileName = base;
    auto fileNameStart = base.find('/');
    if (fileNameStart != std::string::npos)
    {
        fileName = base.substr(fileNameStart + 1);
    }
    else
    {
        fileNameStart = base.find(':');
        if (fileNameStart != std::string::npos)
        {
            fileName = base.substr(fileNameStart + 1);
        }
    }

    if (String::StartsWith(base, "$LGX:"))
    {
        info.Kind = SourceKind::Gx;
        info.Path = fileName;
    }
    else if (String::StartsWith(base, "$G1"))
    {
        auto env = GetContext()->GetPlatformEnvironment();
        auto dataPath = env->GetDirectoryPath(DIRBASE::RCT2, DIRID::DATA);
        info.Kind = SourceKind::G1;
        // info.Path = env->FindFile(DIRBASE::RCT2, DIRID::DATA, "g1.dat");
    }
    else if (String::StartsWith(base, "$CSG"))
    {
        auto env = GetContext()->GetPlatformEnvironment();
        auto dataPath = env->GetDirectoryPath(DIRBASE::RCT2, DIRID::DATA);
        info.Kind = SourceKind::Csg;
        // info.Path = env->FindFile(DIRBASE::RCT2, DIRID::DATA, "g1.dat");
    }
    else if (String::StartsWith(base, "$RCT1:DATA/"))
    {
        auto env = GetContext()->GetPlatformEnvironment();
        info.Kind = SourceKind::Data;
        info.Path = env->FindFile(DIRBASE::RCT1, DIRID::DATA, fileName);
    }
    else if (String::StartsWith(base, "$RCT2:DATA/"))
    {
        auto env = GetContext()->GetPlatformEnvironment();
        info.Kind = SourceKind::Data;
        info.Path = env->FindFile(DIRBASE::RCT2, DIRID::DATA, fileName);
    }
    else if (String::StartsWith(base, "$RCT2:OBJDATA/"))
    {
        auto env = GetContext()->GetPlatformEnvironment();
        info.Kind = SourceKind::ObjData;
        info.Path = env->FindFile(DIRBASE::RCT2, DIRID::OBJECT, fileName);
    }
    else if (!String::StartsWith(base, "$"))
    {
        info.Path = base;
    }
    return info;
}
