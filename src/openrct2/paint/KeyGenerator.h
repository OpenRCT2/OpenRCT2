#pragma once

#include <array>
#include <optional>
#include <vector>

template<class KeyType> class KeyRange
{
    const std::vector<uint32_t>& Get(uint32_t location) const;
};

template<class KeyType> class KeyGenerator
{
public:
    KeyGenerator()
    {
        std::fill(_fieldPresent.begin(), _fieldPresent.end(), false);
    }
    void Initialize(const std::vector<KeyType>& keyDescs, const KeyRange<KeyType>& keyRange);

    std::vector<uint32_t> GetParams(const KeyType& key) const;
    void GenerateKeys(const KeyType& key, std::vector<KeyType>& newKeys) const;
private:
    using ElementsType = std::array<std::vector<uint32_t>, KeyType::NumArgs>;
    std::array<bool, KeyType::NumArgs> _fieldPresent;

    const KeyRange<KeyType>* _keysRange;
};

template<class KeyType>
void KeyGenerator<KeyType>::Initialize(const std::vector<KeyType>& keys, const KeyRange<KeyType>& keyRange)
{
    _keysRange = &keyRange;
    for (auto& key : keys)
    {
        for (uint32_t index = 0; index < _fieldPresent.size(); index++)
        {
            const std::optional<uint32_t> arg = key.Get(index);
            if (arg.has_value())
                _fieldPresent[index] = true;
        }
    }
}

template<class KeyType>
void KeyGenerator<KeyType>::GenerateKeys(const KeyType& keyDesc, std::vector<KeyType>& newKeys) const
{
    ElementsType elementValues;
    for (uint32_t index = 0; index < _fieldPresent.size(); index++)
    {
        auto& values = elementValues[index];
        const auto& fieldPresent = _fieldPresent[index];
        const auto& vector = _keysRange->Get(index);

        const std::optional<uint32_t> arg = keyDesc.Get(index);

        if (arg.has_value())
            values.push_back(arg.value());
        else if (!fieldPresent)
            values.push_back(0);
        else
            values = vector;
    }

    std::vector<KeyType> oldKeys;
    newKeys.push_back(keyDesc);

    for (uint32_t index = 0; index < _fieldPresent.size(); ++index)
    {
        oldKeys = newKeys;
        newKeys.clear();
        const auto& values = elementValues[index];

        for (const auto& oldKey : oldKeys)
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
}

template<class KeyType>
std::vector<uint32_t> KeyGenerator<KeyType>::GetParams(const KeyType& keyDesc) const
{
    std::vector<uint32_t> params;
    for (uint32_t index = 0; index < KeyType::NumArgs; index++)
    {
        if (_fieldPresent[index])
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
