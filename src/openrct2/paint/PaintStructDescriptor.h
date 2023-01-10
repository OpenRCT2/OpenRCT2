#pragma once

#include "../Limits.h"
#include "../ride/TrackData.h"

#include <array>
#include <optional>
#include <tuple>
#include <vector>

struct Vehicle;
using PaintStructSequenceMapping = std::array<std::vector<uint32_t>, 4>;

struct PaintStructDescriptorKey
{
    PaintStructDescriptorKey()
        : Element(0)
        , Direction(0)
        , TrackSequence(0)
        , VehicleIndex(0)
        , VehicleSpriteDirection(0)
        , VehiclePitch(0)
    {
    }

    track_type_t Element;
    uint32_t Direction;
    uint32_t TrackSequence;
    PaintStructSequenceMapping* TrackSequenceMapping = nullptr;
    uint32_t VehicleIndex;
    uint32_t VehicleSpriteDirection;
    uint32_t VehiclePitch;
    std::optional<uint32_t> VehicleNumPeeps;

    // helper method to check for the keys
    bool MatchWithKeys(track_type_t trackElement, uint32_t direction, uint32_t trackSequence, const Vehicle* vehicle) const;
};

template<class T> class PaintStructContainer
{
public:
    PaintStructContainer();

    void Add(const PaintStructDescriptorKey& key, const T& value);
    const std::vector<T>* Get(const PaintStructDescriptorKey& key) const;
    const T* Get(const PaintStructDescriptorKey& key, size_t index) const;
private:
    // is it efficient? not too sure about that...
    using FinalStage = std::vector<T>;
    using ElementMap = std::vector<FinalStage>;
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
    bool IndexValid(
        const size_t& vehicleNumPeeps, const size_t& vehiclePitch, const size_t& vehicleSpriteDirection, const size_t& vehicleIndex, const size_t& trackSequence, const size_t& direction, const size_t& elem,
        const size_t& index) const;

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
    _elements[vehicleNumPeeps][vehiclePitch][vehicleSpriteDirection][vehicleIndex][trackSequence][direction][elem].push_back(value);
}

template<class T> const std::vector<T>* PaintStructContainer<T>::Get(const PaintStructDescriptorKey& key) const
{
    size_t vehicleNumPeeps = 0;
    size_t vehiclePitch = 0;
    size_t vehicleSpriteDirection = 0;
    size_t vehicleIndex = 0;
    size_t trackSequence = 0;
    size_t direction = 0;
    size_t elem = 0;
    GetNonNullKeys(key, vehicleNumPeeps, vehiclePitch, vehicleSpriteDirection, vehicleIndex, trackSequence, direction, elem);

    if (!IndexValid(vehicleNumPeeps, vehiclePitch, vehicleSpriteDirection, vehicleIndex, trackSequence, direction, elem, 0))
        return nullptr;

    return &_elements[vehicleNumPeeps][vehiclePitch][vehicleSpriteDirection][vehicleIndex][trackSequence][direction][elem];
}

template<class T> const T* PaintStructContainer<T>::Get(const PaintStructDescriptorKey& key, size_t index) const
{
    size_t vehicleNumPeeps = 0;
    size_t vehiclePitch = 0;
    size_t vehicleSpriteDirection = 0;
    size_t vehicleIndex = 0;
    size_t trackSequence = 0;
    size_t direction = 0;
    size_t elem = 0;
    GetNonNullKeys(key, vehicleNumPeeps, vehiclePitch, vehicleSpriteDirection, vehicleIndex, trackSequence, direction, elem);

    if (!IndexValid(vehicleNumPeeps, vehiclePitch, vehicleSpriteDirection, vehicleIndex, trackSequence, direction, elem, index))
        return nullptr;

    return &_elements[vehicleNumPeeps][vehiclePitch][vehicleSpriteDirection][vehicleIndex][trackSequence][direction][elem][index];
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

    if (_vehiclePitchKey)
        vehiclePitch = key.VehiclePitch;
    else
        vehiclePitch = 0;

    if (_vehicleSpriteDirectionKey)
        vehicleSpriteDirection = key.VehicleSpriteDirection;
    else
        vehicleSpriteDirection = 0;

    if (_vehicleIndexKey)
        vehicleIndex = key.VehicleIndex;
    else
        vehicleIndex = 0;

    if (_trackSequenceKey)
        trackSequence = key.TrackSequence;
    else
        trackSequence = 0;

    if (_directionKey)
        direction = key.Direction;
    else
        direction = 0;

    if (_elementKey)
        elem = key.Element;
    else
        elem = 0;
}

template<class T> void PaintStructContainer<T>::UpdateKeys(const PaintStructDescriptorKey& key)
{
    _directionKey = true;
    _elementKey = true;
    _trackSequenceKey = true;
    _vehicleIndexKey = true;
    if (key.VehicleNumPeeps.has_value())
        _vehicleNumPeepsKey = true;
    _vehiclePitchKey = true;
    _vehicleSpriteDirectionKey = true;
}

template<class T>
bool PaintStructContainer<T>::IndexValid(
    const size_t& vehicleNumPeeps, const size_t& vehiclePitch, const size_t& vehicleSpriteDirection, const size_t& vehicleIndex,
    const size_t& trackSequence, const size_t& direction, const size_t& elem, const size_t& index) const
{
    const auto& vec0 = _elements;
    if (vec0.size() <= vehicleNumPeeps)
        return false;

    const auto& vec1 = vec0[vehicleNumPeeps];
    if (vec1.size() <= vehiclePitch)
        return false;

    const auto& vec2 = vec1[vehiclePitch];
    if (vec2.size() <= vehicleSpriteDirection)
        return false;

    const auto& vec3 = vec2[vehicleSpriteDirection];
    if (vec3.size() <= vehicleIndex)
        return false;

    const auto& vec4 = vec3[vehicleIndex];
    if (vec4.size() <= trackSequence)
        return false;

    const auto& vec5 = vec4[trackSequence];
    if (vec5.size() <= direction)
        return false;

    const auto& vec6 = vec5[direction];
    if (vec6.size() <= elem)
        return false;

    const auto& vec7 = vec6[elem];
    if (vec7.size() <= index)
        return false;

    return true;
}

struct ImageIdOffset
{
    std::string Id;
    PaintStructContainer<uint32_t> Entries;
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

    ImageIdOffset* ImageIdOffset = nullptr;
    uint32_t ImageIdOffsetIndex;

    CoordsXYZ Offset;
    BoundBoxXYZ BoundBox;
    HeightSupportsTable* HeightSupports = nullptr;

    void Paint(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement) const;

    void ToJson(json_t& json) const;

    PaintStructDescriptor();
};
