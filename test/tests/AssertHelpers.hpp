#pragma once

#include <initializer_list>
#include <vector>
#include <gtest/gtest.h>

template<typename T, typename TExpected>
void AssertVector(std::vector<T> actual, TExpected expected)
{
    ASSERT_EQ(actual.size(), expected.size()) <<
        "Expected vector of size " << expected.size() << ", but was " << actual.size();
    size_t i = 0;
    for (auto item : expected)
    {
        EXPECT_EQ(actual[i], item) <<
            "Element at index " << i << " did not match";
        i++;
    }
}

template<typename T>
void AssertVector(std::vector<T> actual, std::initializer_list<T> expected)
{
    AssertVector<T, std::initializer_list<T>>(actual, expected);
}
