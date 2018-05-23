#include <string>
#include <openrct2/core/Hash.h>
#include <gtest/gtest.h>

class HashTests : public testing::Test
{
public:
    template<typename T>
    void AssertHash(std::string expected, T hash)
    {
        auto actual = StringToHex(hash);
        ASSERT_EQ(expected, actual);
    }

    template<typename T>
    std::string StringToHex(T input)
    {
        std::string result;
        result.reserve(input.size() * 2);
        for (auto b : input)
        {
            static_assert(sizeof(b) == 1);

            char buf[3];
            snprintf(buf, 3, "%02x", b);
            result.append(buf);
        }
        return result;
    }
};

TEST_F(HashTests, SHA1_Basic)
{
    std::string input = "The quick brown fox jumped over the lazy dog.";
    auto result = Hash::SHA1(input.data(), input.size());
    AssertHash("c0854fb9fb03c41cce3802cb0d220529e6eef94e", result);
}

TEST_F(HashTests, SHA1_Multiple)
{
    std::string input[] = {
        "Merry-go-round 2 looks too intense for me",
        "This park is really clean and tidy",
        "This balloon from Balloon Stall 1 is really good value"
    };

    auto alg = Hash::CreateSHA1();
    for (auto s : input)
    {
        alg->Update(s.data(), s.size());
    }
    auto result = alg->Finish();

    AssertHash("758a238d9a4748f80cc81f12be3885d5e45d34c2", result);
}
