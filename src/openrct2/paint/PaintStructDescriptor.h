#pragma once

#include "../Limits.h"
#include "../core/Json.hpp"
#include "../ride/TrackData.h"
#include "KeyGenerator.h"
#include "TreeContainer.h"

#include <array>
#include <optional>
#include <tuple>
#include <vector>

struct Vehicle;
using PaintStructSequenceMapping = std::array<std::vector<uint32_t>, 4>;

struct VehicleKey
{
    uint32_t SpriteDirection;
    uint32_t Pitch;
    uint32_t NumPeeps;

    VehicleKey()
        : SpriteDirection(0)
        , Pitch(0)
        , NumPeeps(0)
    {
    }
};

bool operator==(const VehicleKey& lhs, const VehicleKey& rhs);

bool operator==(const PaintStructDescriptorKey& lhs, const PaintStructDescriptorKey& rhs);

struct PaintStructKey
{
    PaintStructKey()
    {
        SetFields();
    }

    PaintStructKey(const PaintStructKey& other)
        : Element(other.Element)
        , Direction(other.Direction)
        , TrackSequence(other.TrackSequence)
        , VehicleSpriteDirection(other.VehicleSpriteDirection)
        , VehiclePitch(other.VehiclePitch)
        , VehicleNumPeeps(other.VehicleNumPeeps)
        , SessionCurrentRotation(other.SessionCurrentRotation)
        , VehicleRestraintsPosition(other.VehicleRestraintsPosition)
    {
        SetFields();
    }

    using VehicleParam = std::array<std::optional<uint32_t>, OpenRCT2::Limits::MaxTrainsPerRide + 1>;
    std::optional<uint32_t> Element;
    std::optional<uint32_t> Direction;
    std::optional<uint32_t> TrackSequence;
    std::optional<uint32_t> SessionCurrentRotation;
    VehicleParam VehicleSpriteDirection;
    VehicleParam VehiclePitch;
    VehicleParam VehicleNumPeeps;
    VehicleParam VehicleRestraintsPosition;
    void FromJson(const json_t& element);

    const std::optional<uint32_t>& Get(uint32_t location) const;
    void Set(uint32_t location, uint32_t value);
    PaintStructDescriptorKey GetKey() const;

    static constexpr const size_t NumArgs = 8;

private:
    std::array<std::optional<uint32_t>*, NumArgs> _fields;
    void SetFields()
    {
        _fields[0] = &Element;
        _fields[1] = &Direction;
        _fields[2] = &TrackSequence;
        _fields[3] = &VehicleNumPeeps[0];
        _fields[4] = &VehiclePitch[0];
        _fields[5] = &VehicleSpriteDirection[0];
        _fields[6] = &SessionCurrentRotation;
        _fields[7] = &VehicleRestraintsPosition[0];
    }
};

struct ImageIdOffsetJson
{
    std::string Id;
    std::vector<PaintStructKey> Keys;
    std::vector<std::vector<uint32_t>> Values;

    void FromJson(const json_t& element);
};

struct HeightSupportsTable
{
    std::string Id;
    uint32_t HeightOffset;
    std::map<uint8_t, uint32_t> Segments;
};

struct BoundBoxEntryValue
{
    CoordsXYZ Coords;
    BoundBoxXYZ Boundbox;
};

using BoundBoxTree = TreeContainer<PaintStructKey, std::shared_ptr<BoundBoxEntryValue>>;
struct BoundBoxEntry
{
    std::string Id;
    std::shared_ptr<BoundBoxTree> Values;
};

using ImageIdTree = TreeContainer<PaintStructKey, uint32_t>;
struct ImageIdOffset
{
    std::string Id;
    std::shared_ptr<ImageIdTree> Entries;

    ImageIdOffset()
    {
    }
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

    std::optional<SupportsType> Supports;
    std::optional<FloorType> Floor;
    std::optional<FenceType> Fences;
    std::optional<PaintType> PaintCode;
    std::optional<Scheme> ImageIdScheme;
    std::optional<ImageIdBase> ImageId;
    Colour PrimaryColour;
    uint32_t PrimaryColourIndex;
    std::optional<Colour> SecondaryColour;
    uint32_t SecondaryColourIndex;

    const ImageIdOffset* ImageIdOffset;
    uint32_t ImageIdOffsetIndex;

    const BoundBoxEntry* BoundBoxTable;
    const HeightSupportsTable* HeightSupports;

    // to-do : in the future, send a list of vehicle pointers so we can get the peep tshirt colours from every vehicle...
    void Paint(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, const PaintStructKey& key, const Vehicle* vehicle) const;

    PaintStructDescriptor();

    static track_type_t TrackElemTypeFromString(const std::string& elem);

private:
    constexpr const uint8_t GetEdges(track_type_t element, uint8_t trackSequence) const;
};

class PaintObject;
struct PaintStructJson
{
    PaintStructJson(const PaintObject& object)
        : _object(object)
        , PrimaryColour(PaintStructDescriptor::Colour::VehicleBody)
        , PrimaryColourIndex(0)
        , SecondaryColour(PaintStructDescriptor::Colour::VehicleTrim)
        , SecondaryColourIndex(0)
        , ImageIdOffsetIndex(0)
    {
    }
    PaintStructKey Key;
    std::optional<PaintStructDescriptor::SupportsType> Supports;
    std::optional<PaintStructDescriptor::FloorType> Floor;
    std::string EdgesId;
    std::optional<PaintStructDescriptor::FenceType> Fences;
    std::optional<PaintStructDescriptor::PaintType> PaintType;
    std::optional<PaintStructDescriptor::Scheme> ImageIdScheme;
    std::optional<PaintStructDescriptor::ImageIdBase> ImageIdBase;
    PaintStructDescriptor::Colour PrimaryColour;
    uint32_t PrimaryColourIndex;
    std::optional<PaintStructDescriptor::Colour> SecondaryColour;
    uint32_t SecondaryColourIndex;

    std::string ImageIdOffsetId;
    uint32_t ImageIdOffsetIndex;

    std::string BoundBoxId;
    std::string HeightSupportsId;
    void FromJson(const json_t& paintStruct);

    PaintStructDescriptor Value() const;

private:
    const PaintObject& _object;
};

template<> class KeyRange<PaintStructKey>
{
public:
    KeyRange()
    {
        SetFields();
    }

    KeyRange(const KeyRange& other)
        : Elements(other.Elements)
        , Directions(other.Directions)
        , TrackSequences(other.TrackSequences)
        , VehicleNumPeepes(other.VehicleNumPeepes)
        , VehiclePitches(other.VehiclePitches)
        , SessionCurrentRotations(other.SessionCurrentRotations)
        , VehicleRestraintsPositions(other.VehicleRestraintsPositions)
    {
        SetFields();
    }

    const std::vector<uint32_t>& Get(uint32_t location) const
    {
        return *_fieldsRanges[location];
    }

    std::vector<uint32_t> Elements;
    std::vector<uint32_t> Directions;
    std::vector<uint32_t> TrackSequences;
    std::vector<uint32_t> VehicleNumPeepes;
    std::vector<uint32_t> VehiclePitches;
    std::vector<uint32_t> VehicleSpriteDirections;
    std::vector<uint32_t> VehicleRestraintsPositions;
    std::vector<uint32_t> SessionCurrentRotations;

    void FromJson(const json_t& json);

private:
    std::array<std::vector<uint32_t>*, PaintStructKey::NumArgs> _fieldsRanges;

    void SetFields()
    {
        _fieldsRanges[0] = &Elements;
        _fieldsRanges[1] = &Directions;
        _fieldsRanges[2] = &TrackSequences;
        _fieldsRanges[3] = &VehicleNumPeepes;
        _fieldsRanges[4] = &VehiclePitches;
        _fieldsRanges[5] = &VehicleSpriteDirections;
        _fieldsRanges[6] = &SessionCurrentRotations;
        _fieldsRanges[7] = &VehicleRestraintsPositions;
    }
};

inline void KeyRange<PaintStructKey>::FromJson(const json_t& json)
{
    if (json.contains("elements"))
    {
        const auto& elementsJson = json["elements"];
        if (elementsJson.is_array())
        {
            for (const auto& elementJson : elementsJson)
            {
                auto trackType = PaintStructDescriptor::TrackElemTypeFromString(elementJson);
                Elements.push_back(trackType);
            }
        }
    }

    if (json.contains("directions"))
    {
        const auto& directionsJson = json["directions"];
        if (directionsJson.is_array())
        {
            for (const auto& directionJson : directionsJson)
                Directions.push_back(directionJson);
        }
    }

    if (json.contains("trackSequences"))
    {
        const auto& trackSequencesJson = json["trackSequences"];
        if (trackSequencesJson.is_array())
        {
            for (const auto& trackSequenceJson : trackSequencesJson)
                TrackSequences.push_back(trackSequenceJson);
        }
    }

    if (json.contains("vehicleNumPeeps"))
    {
        const auto& vehicleNumPeepesJson = json["vehicleNumPeeps"];
        if (vehicleNumPeepesJson.is_array())
        {
            for (const auto& vehicleNumPeepsJson : vehicleNumPeepesJson)
                VehicleNumPeepes.push_back(vehicleNumPeepsJson);
        }
    }

    if (json.contains("vehiclePitches"))
    {
        const auto& vehiclePitchesJson = json["vehiclePitches"];
        if (vehiclePitchesJson.is_array())
        {
            for (const auto& vehiclePitchJson : vehiclePitchesJson)
                VehiclePitches.push_back(vehiclePitchJson);
        }
    }

    if (json.contains("vehicleSpriteDirections"))
    {
        const auto& vehicleSpriteDirectionsJson = json["vehicleSpriteDirections"];
        if (vehicleSpriteDirectionsJson.is_array())
        {
            for (const auto& vehicleSpriteDirectionJson : vehicleSpriteDirectionsJson)
                VehicleSpriteDirections.push_back(vehicleSpriteDirectionJson);
        }
    }

    if (json.contains("sessionCurrentRotations"))
    {
        const auto& sessionCurrentRotationsJson = json["sessionCurrentRotations"];
        if (sessionCurrentRotationsJson.is_array())
        {
            for (const auto& sessionCurrentRotationJson : sessionCurrentRotationsJson)
                SessionCurrentRotations.push_back(sessionCurrentRotationJson);
        }
    }

    if (json.contains("vehicleRestraintsPositions"))
    {
        const auto& vehicleRestraintsPositionsJson = json["vehicleRestraintsPositions"];
        if (vehicleRestraintsPositionsJson.is_array())
        {
            for (const auto& vehicleRestraintsPositionJson : vehicleRestraintsPositionsJson)
                VehicleRestraintsPositions.push_back(vehicleRestraintsPositionJson);
        }
    }
}
