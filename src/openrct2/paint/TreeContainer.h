#pragma once

#include <vector>
template<class KeyType, class ValueType> class Node
{
public:
    Node(){};
    void Insert(size_t index);
    template<class KeyType, class ValueType> friend class TreeContainer;

protected:
    std::vector<ValueType> _values;
    std::vector<Node> _children;
};

struct PaintStructDescriptorKey;
template<class KeyType, class ValueType> class TreeContainer : public Node<KeyType, ValueType>
{
public:
    TreeContainer();

    template<class ValueType>
    friend void Add(
        TreeContainer<PaintStructDescriptorKey, ValueType>& treeContainer, const PaintStructDescriptorKey& location,
        const ValueType& value);

    template<class ValueType>
    friend const std::vector<ValueType>* Get(
        const TreeContainer<PaintStructDescriptorKey, ValueType>& treeContainer, const PaintStructDescriptorKey& location);

private:
    const std::vector<ValueType>* Get(const std::vector<size_t>& location) const;
    void Add(const std::vector<size_t>& location, const ValueType& value);
};

template<class KeyType, class ValueType>
const std::vector<ValueType>* Get(const TreeContainer<KeyType, ValueType>& treeContainer, const KeyType& location);

template<class KeyType, class ValueType>
void Add(TreeContainer<KeyType, ValueType>& treeContainer, const KeyType& location, const ValueType& value);

template<class KeyType, class ValueType> TreeContainer<KeyType, ValueType>::TreeContainer()
{
}

template<class KeyType, class ValueType> void Node<KeyType, ValueType>::Insert(size_t index)
{
    if (_children.size() <= index)
        _children.resize(index + 1);
    _children[index] = Node<ValueType>();
}

template<class KeyType, class ValueType>
const std::vector<ValueType>* TreeContainer<KeyType, ValueType>::Get(const std::vector<size_t>& location) const
{
    const Node<KeyType, ValueType>* nextNode = this;
    for (const auto& loc : location)
    {
        if (loc < nextNode->_children.size())
            nextNode = &nextNode->_children[loc];
        else
            return nullptr;
    }
    return &nextNode->_values;
}

template<class KeyType, class ValueType>
void TreeContainer<KeyType, ValueType>::Add(const std::vector<size_t>& location, const ValueType& value)
{
    Node<KeyType, ValueType>* nextNode = this;
    for (const auto& loc : location)
    {
        if (nextNode->_children.size() <= loc)
            nextNode->_children.resize(loc + 1);
        nextNode = &nextNode->_children[loc];
    }
    nextNode->_values.push_back(value);
}
