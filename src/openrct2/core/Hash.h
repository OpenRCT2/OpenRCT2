#pragma region Copyright (c) 2018 OpenRCT2 Developers
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

#include <array>
#include <memory>

template<size_t TLength>
class HashAlgorithm
{
public:
    virtual ~HashAlgorithm() = default;
    virtual void Clear() = 0;
    virtual void Update(const void * data, size_t dataLen) = 0;
    virtual std::array<uint8_t, TLength> Finish() = 0;
};

namespace Hash
{
    std::unique_ptr<HashAlgorithm<20>> CreateSHA1();
    std::array<uint8_t, 20> SHA1(const void * data, size_t dataLen);
}
