#pragma once

#include <unordered_map>
#include <vector>
#include "KeyGenerator.h"

template<class KeyType, class ValueType> class Node
{
public:
    Node(){};
    void Insert(uint32_t index);
    template<class KeyType, class ValueType> friend class TreeContainer;

protected:
    std::vector<ValueType> _values;
    std::unordered_map<uint32_t, Node> _children;
};

struct PaintStructDescriptorKey;
template<class KeyType, class ValueType>
class TreeContainer : public Node<KeyType, ValueType>
{
public:
    TreeContainer()
    {
    }
    void Initialize(const KeyRange<KeyType>& keyRange);
    const std::vector<ValueType>* Get(const KeyType& location) const;

    void Add(const KeyType& location, const ValueType& value);
    void ReadKey(const KeyType& location);
private:

    const std::vector<ValueType>* Get(const std::vector<uint32_t>& location) const;
    void Add(const std::vector<uint32_t>& location, const ValueType& value);
    KeyGenerator<KeyType> _keyGenerator;

    const KeyRange<KeyType>* _keyRange;
};

template<class KeyType, class ValueType>
void Node<KeyType, ValueType>::Insert(uint32_t index)
{
    _children[index] = Node<KeyType, ValueType>();
}

template<class KeyType, class ValueType>
const std::vector<ValueType>* TreeContainer<KeyType, ValueType>::Get(
    const std::vector<uint32_t>& location) const
{
    const Node<KeyType, ValueType>* nextNode = this;
    for (const auto& loc : location)
    {
        const auto it = nextNode->_children.find(loc);
        if (it != nextNode->_children.end())
            nextNode = &(it->second);
        else
            return nullptr;
    }
    return &nextNode->_values;
}

template<class KeyType, class ValueType>
void TreeContainer<KeyType, ValueType>::Add(const std::vector<uint32_t>& location, const ValueType& value)
{
    Node<KeyType, ValueType>* nextNode = this;
    for (const auto& loc : location)
    {
        auto it = nextNode->_children.find(loc);
        if (it == nextNode->_children.end())
            nextNode->Insert(loc);
        nextNode = &nextNode->_children[loc];
    }
    nextNode->_values.push_back(value);
}

template<class KeyType, class ValueType>
void TreeContainer<KeyType, ValueType>::Add(const KeyType& key, const ValueType& value)
{
    auto keysGenerated = _keyGenerator.GenerateKeys(key);
    for (const auto& aKey : keysGenerated)
    {
        auto params = _keyGenerator.GetParams(aKey);
        Add(params, value);
    }
}

template<class KeyType, class ValueType>
const std::vector<ValueType>* TreeContainer<KeyType, ValueType>::Get(
    const KeyType& location) const
{
    return Get(_keyGenerator.GetParams(location));
}

template<class KeyType, class ValueType> void TreeContainer<KeyType, ValueType>::Initialize(const KeyRange<KeyType>& keyRange)
{
    _keyRange = &keyRange;
    _keyGenerator.Initialize(keyRange);
}

template<class KeyType, class ValueType> void TreeContainer<KeyType, ValueType>::ReadKey(const KeyType& location)
{
    _keyGenerator.ReadKey(location);
}
