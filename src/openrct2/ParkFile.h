#pragma once

#include <string_view>
#include <vector>

struct ObjectRepositoryItem;

class ParkFileExporter
{
public:
    std::vector<const ObjectRepositoryItem*> ExportObjectsList;

    void Export(std::string_view path);
};
