/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cassert>
#include <cstdint>
#include <limits>

using colour_t = uint8_t;
enum class FilterPaletteID : int32_t;
typedef uint32_t ImageIndex;

static constexpr ImageIndex ImageIndexUndefined = std::numeric_limits<ImageIndex>::max();

enum class ImageCatalogue
{
    UNKNOWN,
    G1,
    G2,
    CSG,
    OBJECT,
    TEMPORARY,
};

FilterPaletteID GetGlassPaletteId(colour_t);

/**
 * Represents a specific image from a catalogue such as G1, G2, CSG etc. with remap
 * colours and flags.
 *
 * This is currently all stored as a single 32-bit integer, but will allow easy
 * extension to 64-bits or higher so that more images can be used.
 */
struct ImageId
{
private:
    // clang-format off
    static constexpr uint32_t MASK_INDEX       = 0b00000000000001111111111111111111;
    static constexpr uint32_t MASK_REMAP       = 0b00000111111110000000000000000000;
    static constexpr uint32_t MASK_PRIMARY     = 0b00000000111110000000000000000000;
    static constexpr uint32_t MASK_SECONDARY   = 0b00011111000000000000000000000000;
    static constexpr uint32_t FLAG_PRIMARY     = 0b00100000000000000000000000000000;
    static constexpr uint32_t FLAG_BLEND       = 0b01000000000000000000000000000000;
    static constexpr uint32_t FLAG_SECONDARY   = 0b10000000000000000000000000000000;
    static constexpr uint32_t SHIFT_REMAP      = 19;
    static constexpr uint32_t SHIFT_PRIMARY    = 19;
    static constexpr uint32_t SHIFT_SECONDARY  = 24;
    static constexpr uint32_t INDEX_UNDEFINED  = 0b00000000000001111111111111111111;
    static constexpr uint32_t VALUE_UNDEFINED  = INDEX_UNDEFINED;

    static constexpr uint8_t  NEW_FLAG_PRIMARY      = 1;
    static constexpr uint8_t  NEW_FLAG_BLEND        = 2;
    static constexpr uint8_t  NEW_FLAG_SECONDARY    = 4;
    // clang-format on

    // NONE = No remap
    // BLENDED = No source copy, remap destination only (glass)
    // PRIMARY | BLENDED = Destination is blended with source (water)
    // PRIMARY = Remap with palette id (first 32 are colour palettes)
    // PRIMARY | SECONDARY = Remap with primary and secondary colours
    // SECONDARY = Remap with primary, secondary and tertiary colours

    ImageIndex _index = ImageIndexUndefined;
    uint8_t _primary = 0;
    uint8_t _secondary = 0;
    uint8_t _tertiary = 0;
    uint8_t _flags = 0;

public:
    [[nodiscard]] static ImageId FromUInt32(uint32_t value)
    {
        ImageId result;
        result._index = value & MASK_INDEX;
        if (value & FLAG_SECONDARY)
            result._primary = (value & MASK_PRIMARY) >> SHIFT_PRIMARY;
        else
            result._primary = (value & MASK_REMAP) >> SHIFT_REMAP;
        if (value & FLAG_SECONDARY)
            result._secondary = (value & MASK_SECONDARY) >> SHIFT_SECONDARY;
        if (value & FLAG_PRIMARY)
            result._flags |= NEW_FLAG_PRIMARY;
        if (value & FLAG_BLEND)
            result._flags |= NEW_FLAG_BLEND;
        if (value & FLAG_SECONDARY)
            result._flags |= NEW_FLAG_SECONDARY;
        assert(result.ToUInt32() == value);
        return result;
    }

    ImageId() = default;

    explicit constexpr ImageId(ImageIndex index)
        : _index(index == INDEX_UNDEFINED ? ImageIndexUndefined : index)
    {
    }

    constexpr ImageId(uint32_t index, FilterPaletteID palette)
        : ImageId(ImageId(index).WithRemap(palette))
    {
    }

    constexpr ImageId(uint32_t index, colour_t primaryColour)
        : ImageId(ImageId(index).WithPrimary(primaryColour))
    {
    }

    constexpr ImageId(uint32_t index, colour_t primaryColour, colour_t secondaryColour)
        : ImageId(ImageId(index).WithPrimary(primaryColour).WithSecondary(secondaryColour))
    {
    }

    constexpr ImageId(uint32_t index, colour_t primaryColour, colour_t secondaryColour, colour_t tertiaryColour)
        : ImageId(ImageId(index).WithPrimary(primaryColour).WithSecondary(secondaryColour).WithTertiary(tertiaryColour))
    {
    }

    [[nodiscard]] uint32_t ToUInt32() const
    {
        auto result = (_index & MASK_INDEX);
        result |= (_primary << SHIFT_REMAP) & MASK_REMAP;
        if (_flags & NEW_FLAG_PRIMARY)
            result |= FLAG_PRIMARY;
        if (_flags & NEW_FLAG_SECONDARY)
        {
            result |= (_secondary << SHIFT_SECONDARY) & MASK_SECONDARY;
            result |= FLAG_SECONDARY;
        }
        if (_flags & NEW_FLAG_BLEND)
            result |= FLAG_BLEND;
        return result;
    }

    bool HasValue() const
    {
        return GetIndex() != ImageIndexUndefined;
    }

    bool HasPrimary() const
    {
        return (_flags & NEW_FLAG_PRIMARY) || (_flags & NEW_FLAG_SECONDARY);
    }

    bool HasSecondary() const
    {
        return _flags & NEW_FLAG_SECONDARY;
    }

    bool HasTertiary() const
    {
        return !(_flags & NEW_FLAG_PRIMARY) && (_flags & NEW_FLAG_SECONDARY);
    }

    bool IsRemap() const
    {
        return (_flags & NEW_FLAG_PRIMARY) && !(_flags & NEW_FLAG_SECONDARY);
    }

    bool IsBlended() const
    {
        return _flags & NEW_FLAG_BLEND;
    }

    ImageIndex GetIndex() const
    {
        return _index;
    }

    uint8_t GetRemap() const
    {
        return _primary;
    }

    colour_t GetPrimary() const
    {
        return _primary;
    }

    colour_t GetSecondary() const
    {
        return _secondary;
    }

    colour_t GetTertiary() const
    {
        return _tertiary;
    }

    ImageCatalogue GetCatalogue() const;

    [[nodiscard]] constexpr ImageId WithIndex(ImageIndex index) const
    {
        ImageId result = *this;
        result._index = index;
        return result;
    }

    [[nodiscard]] constexpr ImageId WithIndexOffset(ImageIndex offset) const
    {
        ImageId result = *this;
        result._index += offset;
        return result;
    }

    [[nodiscard]] constexpr ImageId WithRemap(FilterPaletteID paletteId) const
    {
        return WithRemap(static_cast<uint8_t>(paletteId));
    }

    [[nodiscard]] constexpr ImageId WithRemap(uint8_t paletteId) const
    {
        ImageId result = *this;
        result._primary = paletteId;
        result._secondary = 0;
        result._tertiary = 0;
        result._flags |= NEW_FLAG_PRIMARY;
        result._flags &= ~NEW_FLAG_SECONDARY;
        return result;
    }

    [[nodiscard]] constexpr ImageId WithPrimary(colour_t colour) const
    {
        ImageId result = *this;
        result._primary = colour & 31;
        result._flags |= NEW_FLAG_PRIMARY;
        return result;
    }

    [[nodiscard]] constexpr ImageId WithSecondary(colour_t colour) const
    {
        ImageId result = *this;
        result._secondary = colour & 31;
        result._flags |= NEW_FLAG_SECONDARY;
        return result;
    }

    [[nodiscard]] constexpr ImageId WithoutSecondary() const
    {
        ImageId result = *this;
        result._secondary = 0;
        result._flags &= ~NEW_FLAG_SECONDARY;
        return result;
    }

    [[nodiscard]] constexpr ImageId WithTertiary(colour_t tertiary) const
    {
        ImageId result = *this;
        result._tertiary = tertiary & 31;
        result._flags &= ~NEW_FLAG_PRIMARY;
        result._flags |= NEW_FLAG_SECONDARY;
        if (!(_flags & NEW_FLAG_SECONDARY))
        {
            // Tertiary implies primary and secondary, so if colour was remap (8-bit primary) then
            // we need to zero the secondary colour.
            result._secondary = 0;
        }
        return result;
    }

    [[nodiscard]] ImageId WithTransparency(colour_t colour) const
    {
        return WithTransparency(GetGlassPaletteId(colour & 31));
    }

    [[nodiscard]] ImageId WithTransparency(FilterPaletteID palette) const
    {
        ImageId result = *this;
        result._primary = static_cast<uint8_t>(palette);
        result._secondary = 0;
        result._tertiary = 0;
        result._flags = NEW_FLAG_BLEND;
        return result;
    }

    [[nodiscard]] constexpr ImageId WithBlended(bool value) const
    {
        ImageId result = *this;
        if (value)
            result._flags |= NEW_FLAG_BLEND;
        else
            result._flags &= ~NEW_FLAG_BLEND;
        return result;
    }
};
static_assert(sizeof(ImageId) == 8, "The size of this struct is expected to fit in 64 bits for perfomance reasons. See #18555");
