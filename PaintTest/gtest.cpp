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

#ifdef USE_GTEST
#include <algorithm>
#include <string>
#include <vector>
#include "gtest/gtest.h"

extern "C" {
#include "main.h"
#include "../src/rct2.h"
}

int main(int argc, char *argv[]) {
    initHooks();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


class RidePaintTest : public testing::TestWithParam<uint8> {
};

TEST_P(RidePaintTest, TestRidePainting) {
    bool success = testRide(GetParam());

    ASSERT_TRUE(success);
}

std::vector<uint8> ReadTestCasesFromDisk() {
    std::vector<uint8> rides;

    for (int i = 0; i < 91; i++) {
        if (!rideIsImplemented(i)) {
            continue;
        }
        rides.push_back(i);
    }

    return rides;
}

inline std::string CustomParamNameFunction(const ::testing::TestParamInfo<uint8>& info) {
    return std::to_string(info.param);
}

INSTANTIATE_TEST_CASE_P(
        ParameterizedTest,  // Instantiation name can be chosen arbitrarily.
        RidePaintTest,
        testing::ValuesIn(ReadTestCasesFromDisk()),
		CustomParamNameFunction
);

#endif // USE_GTEST
