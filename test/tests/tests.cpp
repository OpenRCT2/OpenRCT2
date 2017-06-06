// This serves as the entry point when building for MSVC which compiles gtest
// directly into the test binary.
#ifdef _MSC_VER

#include <gtest/gtest.h>
#include <openrct2/core/Guard.hpp>

int main(int argc, char * * argv)
{
    // Abort on an assertions so the tests do not hang
    Guard::SetAssertBehaviour(ASSERT_BEHAVIOUR::ABORT);

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#endif
