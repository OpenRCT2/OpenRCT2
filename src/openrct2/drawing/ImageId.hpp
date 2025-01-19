/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "ImageIndexType.h"

#include <cassert>
#include <cstdint>
#include <limits>

using colour_t = uint8_t;
enum class FilterPaletteID : int32_t;

static constexpr ImageIndex kImageIndexUndefined = std::numeric_limits<ImageIndex>::max();

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
    static constexpr uint8_t kFlagPrimary = 1;
    static constexpr uint8_t kFlagBlend = 2;
    static constexpr uint8_t kFlagSecondary = 4;

    // NONE = No remap
    // BLENDED = No source copy, remap destination only (glass)
    // PRIMARY | BLENDED = Destination is blended with source (water)
    // PRIMARY = Remap with palette id (first 32 are colour palettes)
    // PRIMARY | SECONDARY = Remap with primary and secondary colours
    // SECONDARY = Remap with primary, secondary and tertiary colours

    ImageIndex _index = kImageIndexUndefined;
    uint8_t _primary = 0;
    uint8_t _secondary = 0;
    uint8_t _tertiary = 0;
    uint8_t _flags = 0;

public:
    ImageId() = default;

    explicit constexpr ImageId(ImageIndex index)
        : _index(index)
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

    bool HasValue() const
    {
        return GetIndex() != kImageIndexUndefined;
    }

    bool HasPrimary() const
    {
        return (_flags & kFlagPrimary) || (_flags & kFlagSecondary);
    }

    bool HasSecondary() const
    {
        return _flags & kFlagSecondary;
    }

    bool HasTertiary() const
    {
        return !(_flags & kFlagPrimary) && (_flags & kFlagSecondary);
    }

    bool IsRemap() const
    {
        return (_flags & kFlagPrimary) && !(_flags & kFlagSecondary);
    }

    bool IsBlended() const
    {
        return _flags & kFlagBlend;
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
        result._flags |= kFlagPrimary;
        result._flags &= ~kFlagSecondary;
        return result;
    }

    [[nodiscard]] constexpr ImageId WithPrimary(colour_t colour) const
    {
        ImageId result = *this;
        result._primary = colour;
        result._flags |= kFlagPrimary;
        return result;
    }

    [[nodiscard]] constexpr ImageId WithSecondary(colour_t colour) const
    {
        ImageId result = *this;
        result._secondary = colour;
        result._flags |= kFlagSecondary;
        return result;
    }

    [[nodiscard]] constexpr ImageId WithoutSecondary() const
    {
        ImageId result = *this;
        result._secondary = 0;
        result._flags &= ~kFlagSecondary;
        return result;
    }

    [[nodiscard]] constexpr ImageId WithTertiary(colour_t tertiary) const
    {
        ImageId result = *this;
        result._tertiary = tertiary;
        result._flags &= ~kFlagPrimary;
        result._flags |= kFlagSecondary;
        if (!(_flags & kFlagSecondary))
        {
            // Tertiary implies primary and secondary, so if colour was remap (8-bit primary) then
            // we need to zero the secondary colour.
            result._secondary = 0;
        }
        return result;
    }

    [[nodiscard]] ImageId WithTransparency(colour_t colour) const
    {
        return WithTransparency(GetGlassPaletteId(colour));
    }

    [[nodiscard]] ImageId WithTransparency(FilterPaletteID palette) const
    {
        ImageId result = *this;
        result._primary = static_cast<uint8_t>(palette);
        result._secondary = 0;
        result._tertiary = 0;
        result._flags = kFlagBlend;
        return result;
    }

    [[nodiscard]] constexpr ImageId WithBlended(bool value) const
    {
        ImageId result = *this;
        if (value)
            result._flags |= kFlagBlend;
        else
            result._flags &= ~kFlagBlend;
        return result;
    }

    constexpr bool operator==(const ImageId& rhs) const
    {
        return _index == rhs._index && _primary == rhs._primary && _secondary == rhs._secondary && _tertiary == rhs._tertiary
            && _flags == rhs._flags;
    }

    constexpr bool operator!=(const ImageId& rhs) const
    {
        return !(*this == rhs);
    }
};
static_assert(sizeof(ImageId) == 8, "The size of this struct is expected to fit in 64 bits for perfomance reasons. See #18555");
