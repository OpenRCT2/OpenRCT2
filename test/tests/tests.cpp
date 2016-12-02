// This serves as the entry point when building for MSVC which compiles gtest
// directly into the test binary.
#ifdef _MSC_VER

#include <gtest/gtest.h>

int main(int argc, char * * argv)
{ 
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif
