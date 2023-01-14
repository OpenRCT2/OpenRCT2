#pragma once

#include <array>
#include <optional>
#include <vector>

template<class KeyType> class KeyGenerator
{
public:
    KeyGenerator()
    {
    }
    void Initialize(const std::vector<KeyType>& keyDescs);

    std::vector<uint32_t> GetParams(const KeyType& key) const;
    std::vector<KeyType> GenerateKeys(const KeyType& key) const;
private:
    using ElementsType = std::array<std::vector<uint32_t>, KeyType::NumArgs>;
    ElementsType _elements;
};

template<class KeyType>
void KeyGenerator<KeyType>::Initialize(const std::vector<KeyType>& keys)
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

template<class KeyType>
std::vector<KeyType> KeyGenerator<KeyType>::GenerateKeys(const KeyType& keyDesc) const
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

    std::vector<KeyType> oldKeys, newKeys;
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
    return newKeys;
}

template<class KeyType>
std::vector<uint32_t> KeyGenerator<KeyType>::GetParams(const KeyType& keyDesc) const
{
    std::vector<uint32_t> params;
    for (uint32_t index = 0; index < KeyType::NumArgs; index++)
    {
        if (_elements[index].size() != 0)
        {
            if (keyDesc.Get(index).has_value())
            {
                uint32_t param = keyDesc.Get(index).value();
                params.push_back(param);
            }
            else
                params.push_back(0);
        }
    }
    return params;
}
