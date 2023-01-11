#pragma once

#include <vector>
template<class KeyType, class ValueType, class KeyInserter> class Node
{
public:
    Node(){};
    void Insert(size_t index);
    template<class KeyType, class ValueType, class KeyInserter> friend class TreeContainer;

protected:
    std::vector<ValueType> _values;
    std::vector<Node> _children;
};

struct PaintStructDescriptorKey;
template<class KeyType, class ValueType, class KeyInserter> class TreeContainer : public Node<KeyType, ValueType, KeyInserter>
{
public:
    TreeContainer()
    {
    }
    void Add(const KeyType& location, const ValueType& value);
    const std::vector<ValueType>* Get(const KeyType& location) const;

private:
    const std::vector<ValueType>* Get(const std::vector<size_t>& location) const;
    void Add(const std::vector<size_t>& location, const ValueType& value);

    KeyInserter _keyInserter;
};

template<class KeyType, class ValueType, class KeyInserter> void Node<KeyType, ValueType, KeyInserter>::Insert(size_t index)
{
    if (_children.size() <= index)
        _children.resize(index + 1);
    _children[index] = Node<ValueType>();
}

template<class KeyType, class ValueType, class KeyInserter>
const std::vector<ValueType>* TreeContainer<KeyType, ValueType, KeyInserter>::Get(const std::vector<size_t>& location) const
{
    const Node<KeyType, ValueType, KeyInserter>* nextNode = this;
    for (const auto& loc : location)
    {
        if (loc < nextNode->_children.size())
            nextNode = &nextNode->_children[loc];
        else
            return nullptr;
    }
    return &nextNode->_values;
}

template<class KeyType, class ValueType, class KeyInserter>
void TreeContainer<KeyType, ValueType, KeyInserter>::Add(const std::vector<size_t>& location, const ValueType& value)
{
    Node<KeyType, ValueType, KeyInserter>* nextNode = this;
    for (const auto& loc : location)
    {
        if (nextNode->_children.size() <= loc)
            nextNode->_children.resize(loc + 1);
        nextNode = &nextNode->_children[loc];
    }
    nextNode->_values.push_back(value);
}

template<class KeyType, class ValueType, class KeyInserter>
void TreeContainer<KeyType, ValueType, KeyInserter>::Add(const KeyType& location, const ValueType& value)
{
    Add(_keyInserter(location), value);
}

template<class KeyType, class ValueType, class KeyInserter>
const std::vector<ValueType>* TreeContainer<KeyType, ValueType, KeyInserter>::Get(const KeyType& location) const
{
    return Get(_keyInserter(location));
}
