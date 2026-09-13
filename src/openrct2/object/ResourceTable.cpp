/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2
{
    Range<int32_t> ResourceTable::ParseRange(std::string_view s)
    {
        // Currently only supports [###] or [###..###]
        Range<int32_t> result = {};
        if (s.length() >= 3 && s[0] == '[' && s[s.length() - 1] == ']')
        {
            s = s.substr(1, s.length() - 2);
            auto parts = String::split(s, "..");
            if (parts.size() == 1)
            {
                result = Range<int32_t>(String::parse<int32_t>(parts[0]));
            }
            else
            {
                auto left = String::parse<int32_t>(parts[0]);
                auto right = String::parse<int32_t>(parts[1]);
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

        auto& env = GetContext()->GetPlatformEnvironment();
        if (String::startsWith(base, "$LGX:"))
        {
            info.Kind = SourceKind::gx;
            info.Path = fileName;
        }
        else if (String::startsWith(base, "$G1"))
        {
            auto dataPath = env.GetDirectoryPath(DirBase::rct2, DirId::data);
            info.Kind = SourceKind::g1;
            // info.Path = env->FindFile(DirBase::rct2, DirId::data, "g1.dat");
        }
        else if (String::startsWith(base, "$CSG"))
        {
            auto dataPath = env.GetDirectoryPath(DirBase::rct2, DirId::data);
            info.Kind = SourceKind::csg;
            // info.Path = env->FindFile(DirBase::rct2, DirId::data, "g1.dat");
        }
        else if (String::startsWith(base, "$RCT1:DATA/"))
        {
            info.Kind = SourceKind::data;
            info.Path = env.FindFile(DirBase::rct1, DirId::data, fileName);
        }
        else if (String::startsWith(base, "$RCT2:DATA/"))
        {
            info.Kind = SourceKind::data;
            info.Path = env.FindFile(DirBase::rct2, DirId::data, fileName);
        }
        else if (String::startsWith(base, "$RCT2:OBJDATA/"))
        {
            info.Kind = SourceKind::objData;
            info.Path = env.FindFile(DirBase::rct2, DirId::objects, fileName);
        }
        else if (!String::startsWith(base, "$"))
        {
            info.Path = base;
        }
        return info;
    }
} // namespace OpenRCT2
