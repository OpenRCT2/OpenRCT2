#pragma once

#include <vector>
template<class T> class Node
{
public:
    Node(const T& value);
    Node(){};
    void Insert(size_t index);
    template<class T> friend class TreeContainer;

protected:
    std::vector<T> _values;
    std::vector<Node> _children;
};

template<class T> class TreeContainer : public Node<T>
{
public:
    TreeContainer();

    template<class KeyType> std::vector<T>* Get(const KeyType& location);
    template<class KeyType> void Add(const KeyType& location, const T& value);
private:
    std::vector<T>* Get(const std::vector<size_t>& location);
    void Add(const std::vector<size_t>& location, const T& value);
};

template<class T> TreeContainer<T>::TreeContainer()
{
}

template<class T> void Node<T>::Insert(size_t index)
{
    if (_children.size() <= index)
        _children.resize(index + 1);
    _children[index] = Node<T>();
}

template<class T> std::vector<T>* TreeContainer<T>::Get(const std::vector<size_t>& location)
{
    Node<T>* nextNode = this;
    for (const auto& loc : location)
    {
        if (loc < nextNode->_children.size())
            nextNode = &nextNode->_children[loc];
        else
            return nullptr;
    }
    return &nextNode->_values;
}

template<class T> void TreeContainer<T>::Add(const std::vector<size_t>& location, const T& value)
{
    Node<T>* nextNode = this;
    for (const auto& loc : location)
    {
        if (nextNode->_children.size() <= loc)
            nextNode->_children.resize(loc + 1);
        nextNode = &nextNode->_children[loc];
    }
    nextNode->_values.push_back(value);
}
