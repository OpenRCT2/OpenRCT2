#pragma once

#include "../Limits.h"
#include "../ride/TrackData.h"

#include <array>
#include <optional>
#include <tuple>
#include <vector>

struct Vehicle;

struct PaintStructSequenceMapping
{
    std::string Id;
    std::array<std::vector<uint32_t>, 4> Values;
};

struct PaintStructDescriptorKey
{
    std::optional<track_type_t> Element;
    std::optional<uint32_t> Direction;
    std::optional<uint32_t> TrackSequence;
    PaintStructSequenceMapping* TrackSequenceMapping = nullptr;
    std::optional<uint32_t> VehicleIndex;
    std::optional<uint32_t> VehicleSpriteDirection;
    std::optional<uint32_t> VehiclePitch;
    std::optional<uint32_t> VehicleNumPeeps;

    // helper method to check for the keys
    bool MatchWithKeys(track_type_t trackElement, uint32_t direction, uint32_t trackSequence, const Vehicle* vehicle) const;
};

template<class T> class PaintStructContainer
{
public:
    PaintStructContainer();

    void Add(const PaintStructDescriptorKey& key, const T& value);
    const T& Get(const PaintStructDescriptorKey& key) const;
private:
    // is it efficient? not too sure about that...
    using ElementMap = std::vector<T>;
    using DirectionMap = std::vector<ElementMap>;
    using TrackSequenceMap = std::vector<DirectionMap>;
    using VehicleIndexMap = std::vector<TrackSequenceMap>;
    using VehicleSpriteDirectionMap = std::vector<VehicleIndexMap>;
    using VehiclePitchMap = std::vector<VehicleSpriteDirectionMap>;
    using VehicleNumPeeps = std::vector<VehiclePitchMap>;
    VehicleNumPeeps _elements;

    void UpdateKeys(const PaintStructDescriptorKey& key);
    void GetNonNullKeys(
        const PaintStructDescriptorKey& key, size_t& vehicleNumPeeps, size_t& vehiclePitch, size_t& vehicleSpriteDirection,
        size_t& vehicleIndex, size_t& trackSequence, size_t& direction, size_t& elem) const;

    bool _elementKey;
    bool _directionKey;
    bool _trackSequenceKey;
    bool _vehicleIndexKey;
    bool _vehicleSpriteDirectionKey;
    bool _vehiclePitchKey;
    bool _vehicleNumPeepsKey;
};

template<class T>
PaintStructContainer<T>::PaintStructContainer()
    : _elementKey(false)
    , _directionKey(false)
    , _trackSequenceKey(false)
    , _vehicleIndexKey(false)
    , _vehicleSpriteDirectionKey(false)
    , _vehiclePitchKey(false)
    , _vehicleNumPeepsKey(false)
{
}

template<class T> void PaintStructContainer<T>::Add(const PaintStructDescriptorKey& key, const T& value)
{
    UpdateKeys(key);

    size_t vehicleNumPeeps = 0;
    size_t vehiclePitch = 0;
    size_t vehicleSpriteDirection = 0;
    size_t vehicleIndex = 0;
    size_t trackSequence = 0;
    size_t direction = 0;
    size_t elem = 0;
    GetNonNullKeys(key, vehicleNumPeeps, vehiclePitch, vehicleSpriteDirection, vehicleIndex, trackSequence, direction, elem);

    auto& vector0 = _elements;
    if (vector0.size() <= vehicleNumPeeps)
        vector0.resize(vehicleNumPeeps + 1);

    auto& vector1 = _elements[vehicleNumPeeps];
    if (vector1.size() <= vehiclePitch)
        vector1.resize(vehiclePitch + 1);

    auto& vector2 = _elements[vehicleNumPeeps][vehiclePitch];
    if (vector2.size() <= vehicleSpriteDirection)
        vector2.resize(vehicleSpriteDirection + 1);

    auto& vector3 = _elements[vehicleNumPeeps][vehiclePitch][vehicleSpriteDirection];
    if (vector3.size() <= vehicleIndex)
        vector3.resize(vehicleIndex + 1);

    auto& vector4 = _elements[vehicleNumPeeps][vehiclePitch][vehicleSpriteDirection][vehicleIndex];
    if (vector4.size() <= trackSequence)
        vector4.resize(trackSequence + 1);

    auto& vector5 = _elements[vehicleNumPeeps][vehiclePitch][vehicleSpriteDirection][vehicleIndex][trackSequence];
    if (vector5.size() <= direction)
        vector5.resize(direction + 1);

    auto& vector6 = _elements[vehicleNumPeeps][vehiclePitch][vehicleSpriteDirection][vehicleIndex][trackSequence][direction];
    if (vector6.size() <= elem)
        vector6.resize(elem + 1);

    // not too sure about multi indexing... marvelous piece of code
    _elements[vehicleNumPeeps][vehiclePitch][vehicleSpriteDirection][vehicleIndex][trackSequence][direction][elem] = value;
}

template<class T> const T& PaintStructContainer<T>::Get(const PaintStructDescriptorKey& key) const
{
    size_t vehicleNumPeeps = 0;
    size_t vehiclePitch = 0;
    size_t vehicleSpriteDirection = 0;
    size_t vehicleIndex = 0;
    size_t trackSequence = 0;
    size_t direction = 0;
    size_t elem = 0;
    GetNonNullKeys(key, vehicleNumPeeps, vehiclePitch, vehicleSpriteDirection, vehicleIndex, trackSequence, direction, elem);

    return _elements[vehicleNumPeeps][vehiclePitch][vehicleSpriteDirection][vehicleIndex][trackSequence][direction][elem];
}
template<class T>
void PaintStructContainer<T>::GetNonNullKeys(
    const PaintStructDescriptorKey& key, size_t& vehicleNumPeeps, size_t& vehiclePitch, size_t& vehicleSpriteDirection,
    size_t& vehicleIndex, size_t& trackSequence, size_t& direction, size_t& elem) const
{
    if (key.VehicleNumPeeps.has_value() && _vehicleNumPeepsKey)
        vehicleNumPeeps = key.VehicleNumPeeps.value();
    else
        vehicleNumPeeps = 0;

    if (key.VehiclePitch.has_value() && _vehiclePitchKey)
        vehiclePitch = key.VehiclePitch.value();
    else
        vehiclePitch = 0;

    if (key.VehicleSpriteDirection.has_value() && _vehicleSpriteDirectionKey)
        vehicleSpriteDirection = key.VehicleSpriteDirection.value();
    else
        vehicleSpriteDirection = 0;

    if (key.VehicleIndex.has_value() && _vehicleIndexKey)
        vehicleIndex = key.VehicleIndex.value();
    else
        vehicleIndex = 0;

    if (key.TrackSequence.has_value() && _trackSequenceKey)
        trackSequence = key.TrackSequence.value();
    else
        trackSequence = 0;

    if (key.Direction.has_value() && _directionKey)
        direction = key.Direction.value();
    else
        direction = 0;

    if (key.Element.has_value() && _elementKey)
        elem = key.Element.value();
    else
        elem = 0;
}

template<class T> void PaintStructContainer<T>::UpdateKeys(const PaintStructDescriptorKey& key)
{
    if (key.Direction.has_value())
        _directionKey = true;
    if (key.Element.has_value())
        _elementKey = true;
    if (key.TrackSequence.has_value())
        _trackSequenceKey = true;
    if (key.VehicleIndex.has_value())
        _vehicleIndexKey = true;
    if (key.VehicleNumPeeps.has_value())
        _vehicleNumPeepsKey = true;
    if (key.VehiclePitch.has_value())
        _vehiclePitchKey = true;
    if (key.VehicleSpriteDirection.has_value())
        _vehicleSpriteDirectionKey = true;
}

struct ImageIdOffset
{
    std::string Id;
    PaintStructContainer<std::vector<uint32_t>> Entries;
};

struct HeightSupportsTable
{
    std::string Id;
    uint32_t HeightOffset;
    std::map<uint8_t, uint32_t> Segments;
};

using PaintStructEdgesTable = std::vector<edge_t>;
struct PaintStructDescriptor
{
    enum class SupportsType
    {
        WoodenA
    };
    enum class FloorType
    {
        Cork
    };
    enum class FenceType
    {
        Ropes
    };
    enum class ImageIdBase
    {
        Car0
    };
    enum class PaintType
    {
        AddImageAsParent,
        AddImageAsChild,
        SetSegmentsSupportsHeight,
    };
    enum class Scheme
    {
        Misc,
        Track
    };

    enum class Colour
    {
        VehicleBody,
        VehicleTrim,
        PeepTShirt
    };

    // key fields
    PaintStructDescriptorKey Key;

    // output fields
    std::optional<SupportsType> Supports;
    std::optional<FloorType> Floor;
    PaintStructEdgesTable* Edges = nullptr;
    std::optional<FenceType> Fences;
    std::optional<PaintType> PaintType;
    std::optional<Scheme> ImageIdScheme;
    ImageIdBase ImageIdBase;
    Colour PrimaryColour;
    uint32_t PrimaryColourIndex;
    Colour SecondaryColour;
    uint32_t SecondaryColourIndex;

    ImageIdOffset* ImageIdOffset;
    uint32_t ImageIdOffsetIndex;

    CoordsXYZ Offset;
    BoundBoxXYZ BoundBox;
    HeightSupportsTable* HeightSupports;

    void Paint(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement) const;

    void ToJson(json_t& json) const;

    PaintStructDescriptor();
};
