#pragma once

#include <array>
#include <optional>
#include <vector>

template<class KeyType, class KeyTypeDesc> class KeyGenerator
{
public:
    KeyGenerator()
    {
    }
    void Initialize(std::vector<KeyTypeDesc>& keyDescs);

    std::vector<std::vector<uint32_t>> GetParams(const KeyTypeDesc& key) const;
    std::vector<uint32_t> GetParams(const KeyType& key) const;

private:
    std::vector<KeyType> GenerateKeys(const KeyTypeDesc& key) const;

    using ElementsType = std::array<std::vector<uint32_t>, KeyTypeDesc::NumArgs>;
    ElementsType _elements;
};

template<class KeyType, class KeyTypeDesc>
void KeyGenerator<KeyType, KeyTypeDesc>::Initialize(std::vector<KeyTypeDesc>& keys)
{
    for (auto& vector : _elements)
        vector.clear();

    for (auto& key : keys)
    {
        for (uint32_t index = 0; index < _elements.size(); index++)
        {
            auto& vector = _elements[index];

            const std::optional<uint32_t> arg = key.Get(index);
            if (arg.has_value())
            {
                if (!vector.empty())
                {
                    auto it = std::find(vector.begin(), vector.end(), arg.value());
                    if (it == vector.end())
                        vector.push_back(arg.value());
                }
                else
                {
                    vector.push_back(arg.value());
                }
            }
        }
    }
}

template<class KeyType, class KeyTypeDesc>
std::vector<KeyType> KeyGenerator<KeyType, KeyTypeDesc>::GenerateKeys(const KeyTypeDesc& keyDesc) const
{
    ElementsType elementValues;
    for (uint32_t index = 0; index < _elements.size(); index++)
    {
        auto& values = elementValues[index];
        const auto& vector = _elements[index];

        const std::optional<uint32_t> arg = keyDesc.Get(index);

        if (arg.has_value())
            values.push_back(arg.value());
        else if (vector.empty())
            values.push_back(0);
        else
            values = vector;
    }

    std::vector<KeyTypeDesc> oldKeys, newKeys;
    newKeys.push_back(keyDesc);

    for (uint32_t index = 0; index < _elements.size(); index++)
    {
        oldKeys = newKeys;
        newKeys.clear();
        const auto& values = elementValues[index];

        for (auto& oldKey : oldKeys)
        {
            const std::optional<uint32_t> arg = oldKey.Get(index);

            if (arg.has_value())
            {
                auto newKey = oldKey;
                newKey.Set(index, arg.value());
                newKeys.push_back(newKey);
            }
            else
            {
                for (const auto& val : values)
                {
                    auto newKey = oldKey;
                    newKey.Set(index, val);
                    newKeys.push_back(newKey);
                }
            }
        }
    }

    std::vector<KeyType> result;
    for (auto& newKey : newKeys)
    {
        result.push_back(newKey.GetKey());
    }
    return result;
}

template<class KeyType, class KeyTypeDesc>
std::vector<uint32_t> KeyGenerator<KeyType, KeyTypeDesc>::GetParams(const KeyType& key) const
{
    std::vector<uint32_t> result;
    for (uint32_t index = 0; index < KeyTypeDesc::NumArgs; index++)
    {
        if (_elements[index].size() != 0)
        {
            uint32_t param = key.Get(index);
            result.push_back(param);
        }
    }
    return result;
}

template<class KeyType, class KeyTypeDesc>
std::vector<std::vector<uint32_t>> KeyGenerator<KeyType, KeyTypeDesc>::GetParams(const KeyTypeDesc& keyDesc) const
{
    std::vector<std::vector<uint32_t>> result;
    auto keys = GenerateKeys(keyDesc);

    for (const auto& key : keys)
        result.push_back(GetParams(key));
    return result;
}
