#pragma once

#include <unordered_map>
#include <vector>

template<class KeyType, class KeyTypeDesc, class ValueType, class KeyInserter> class Node
{
public:
    Node(){};
    void Insert(uint32_t index);
    template<class KeyType, class KeyTypeDesc, class ValueType, class KeyInserter> friend class TreeContainer;

protected:
    std::vector<ValueType> _values;
    std::unordered_map<uint32_t, Node> _children;
};

struct PaintStructDescriptorKey;
template<class KeyType, class KeyTypeDesc, class ValueType, class KeyInserter>
class TreeContainer : public Node<KeyType, KeyTypeDesc, ValueType, KeyInserter>
{
public:
    TreeContainer(const KeyInserter& keyInserter)
        : _keyInserter(keyInserter)
    {
    }

    void Add(const KeyTypeDesc& location, const ValueType& value);

    const std::vector<ValueType>* Get(const KeyType& location) const;

private:
    const std::vector<ValueType>* Get(const std::vector<uint32_t>& location) const;
    void Add(const std::vector<uint32_t>& location, const ValueType& value);

    const KeyInserter& _keyInserter;
};

template<class KeyType, class KeyTypeDesc, class ValueType, class KeyInserter>
void Node<KeyType, KeyTypeDesc, ValueType, KeyInserter>::Insert(uint32_t index)
{
    _children[index] = Node<KeyType, KeyTypeDesc, ValueType, KeyInserter>();
}

template<class KeyType, class KeyTypeDesc, class ValueType, class KeyInserter>
const std::vector<ValueType>* TreeContainer<KeyType, KeyTypeDesc, ValueType, KeyInserter>::Get(
    const std::vector<uint32_t>& location) const
{
    const Node<KeyType, KeyTypeDesc, ValueType, KeyInserter>* nextNode = this;
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

template<class KeyType, class KeyTypeDesc, class ValueType, class KeyInserter>
void TreeContainer<KeyType, KeyTypeDesc, ValueType, KeyInserter>::Add(const std::vector<uint32_t>& location, const ValueType& value)
{
    Node<KeyType, KeyTypeDesc, ValueType, KeyInserter>* nextNode = this;
    for (const auto& loc : location)
    {
        auto it = nextNode->_children.find(loc);
        if (it == nextNode->_children.end())
            nextNode->Insert(loc);
        nextNode = &nextNode->_children[loc];
    }
    nextNode->_values.push_back(value);
}

template<class KeyType, class KeyTypeDesc, class ValueType, class KeyInserter>
void TreeContainer<KeyType, KeyTypeDesc, ValueType, KeyInserter>::Add(const KeyTypeDesc& location, const ValueType& value)
{
    auto keys = _keyInserter.GetParams(location);
    for (const auto& key : keys)
        Add(key, value);
}

template<class KeyType, class KeyTypeDesc, class ValueType, class KeyInserter>
const std::vector<ValueType>* TreeContainer<KeyType, KeyTypeDesc, ValueType, KeyInserter>::Get(const KeyType& location) const
{
    return Get(_keyInserter.GetParams(location));
}
