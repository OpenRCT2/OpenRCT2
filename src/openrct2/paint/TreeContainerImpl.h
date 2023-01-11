#pragma once

#include "TreeContainer.h"

template<class ValueType>
const std::vector<ValueType>* Get(
    const TreeContainer<PaintStructDescriptorKey, ValueType>& treeContainer, const PaintStructDescriptorKey& location)
{
    return treeContainer.Get(std::vector<size_t>{ location.Direction, location.Element, location.TrackSequence });
}

template<class ValueType>
void Add(
    TreeContainer<PaintStructDescriptorKey, ValueType>& treeContainer, const PaintStructDescriptorKey& location,
    const ValueType& value)
{
    treeContainer.Add(std::vector<size_t>{ location.Direction, location.Element, location.TrackSequence }, value);
}
