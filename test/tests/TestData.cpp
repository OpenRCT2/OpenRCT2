#include <openrct2/core/Path.hpp>
#include "TestData.h"

namespace TestData
{
    std::string GetBasePath()
    {
        return "testdata";
    }

    std::string GetParkPath(std::string name)
    {
        std::string path = Path::Combine(GetBasePath(), "parks", name);
        return path;
    }
};
