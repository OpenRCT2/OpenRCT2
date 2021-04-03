#pragma once

#include <string_view>
#include <vector>

struct ObjectRepositoryItem;

namespace OpenRCT2
{
    struct IStream;
}

class ParkFileExporter
{
public:
    std::vector<const ObjectRepositoryItem*> ExportObjectsList;

    void Export(std::string_view path);
    void Export(OpenRCT2::IStream& stream);
};
