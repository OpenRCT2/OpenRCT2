/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#include "Particle.h"

#include "../audio/audio.h"
#include "../core/DataSerialiser.h"
#include "../paint/Paint.h"
#include "../profiling/Profiling.h"
#include "../ride/VehicleColour.h"
#include "../scenario/Scenario.h"
#include "EntityRegistry.h"

#include <iterator>

// TODO: Create constants in sprites.h
static constexpr uint32_t _VehicleCrashParticleSprites[] = {
    22577, 22589, 22601, 22613, 22625,
};

template<> bool EntityBase::Is<SteamParticle>() const
{
    return Type == EntityType::SteamParticle;
}

template<> bool EntityBase::Is<ExplosionFlare>() const
{
    return Type == EntityType::ExplosionFlare;
}

template<> bool EntityBase::Is<ExplosionCloud>() const
{
    return Type == EntityType::ExplosionCloud;
}

template<> bool EntityBase::Is<VehicleCrashParticle>() const
{
    return Type == EntityType::CrashedVehicleParticle;
}

template<> bool EntityBase::Is<CrashSplashParticle>() const
{
    return Type == EntityType::CrashSplash;
}
/**
 *
 *  rct2: 0x006735A1
 */
void VehicleCrashParticle::Create(VehicleColour& colours, const CoordsXYZ& vehiclePos)
{
    VehicleCrashParticle* sprite = CreateEntity<VehicleCrashParticle>();
    if (sprite != nullptr)
    {
        sprite->colour[0] = colours.Body;
        sprite->colour[1] = colours.Trim;
        sprite->SpriteData.sprite_width = 8;
        sprite->SpriteData.sprite_height_negative = 8;
        sprite->SpriteData.sprite_height_positive = 8;
        sprite->MoveTo(vehiclePos);

        sprite->frame = (ScenarioRand() & 0xFF) * 12;
        sprite->time_to_live = (ScenarioRand() & 0x7F) + 140;
        sprite->crashed_sprite_base = ScenarioRandMax(static_cast<uint32_t>(std::size(_VehicleCrashParticleSprites)));
        sprite->acceleration_x = (static_cast<int16_t>(ScenarioRand() & 0xFFFF)) * 4;
        sprite->acceleration_y = (static_cast<int16_t>(ScenarioRand() & 0xFFFF)) * 4;
        sprite->acceleration_z = (ScenarioRand() & 0xFFFF) * 4 + 0x10000;
        sprite->velocity_x = 0;
        sprite->velocity_y = 0;
        sprite->velocity_z = 0;
    }
}

/**
 *
 *  rct2: 0x00673298
 */
void VehicleCrashParticle::Update()
{
    Invalidate();
    time_to_live--;
    if (time_to_live == 0)
    {
        EntityRemove(this);
        return;
    }

    // Apply gravity
    acceleration_z -= 5041;

    // Apply air resistance
    acceleration_x -= (acceleration_x / 256);
    acceleration_y -= (acceleration_y / 256);
    acceleration_z -= (acceleration_z / 256);

    // Update velocity and position
    int32_t vx = velocity_x + acceleration_x;
    int32_t vy = velocity_y + acceleration_y;
    int32_t vz = velocity_z + acceleration_z;

    CoordsXYZ newLoc = { x + (vx >> 16), y + (vy >> 16), z + (vz >> 16) };

    velocity_x = vx & 0xFFFF;
    velocity_y = vy & 0xFFFF;
    velocity_z = vz & 0xFFFF;

    // Check collision with land / water
    int16_t landZ = TileElementHeight(newLoc);
    int16_t waterZ = TileElementWaterHeight(newLoc);

    if (waterZ != 0 && z >= waterZ && newLoc.z <= waterZ)
    {
        // Splash
        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::Water2, { x, y, waterZ });
        CrashSplashParticle::Create({ x, y, waterZ });
        EntityRemove(this);
        return;
    }

    if (z >= landZ && newLoc.z <= landZ)
    {
        // Bounce
        acceleration_z *= -1;
        newLoc.z = landZ;
    }
    MoveTo(newLoc);

    frame += 85;
    if (frame >= 3072)
    {
        frame = 0;
    }
}

void VehicleCrashParticle::Serialise(DataSerialiser& stream)
{
    EntityBase::Serialise(stream);
    stream << frame;
    stream << time_to_live;
    stream << colour;
    stream << crashed_sprite_base;
    stream << velocity_x;
    stream << velocity_y;
    stream << velocity_z;
    stream << acceleration_x;
    stream << acceleration_y;
    stream << acceleration_z;
}

void VehicleCrashParticle::Paint(PaintSession& session, int32_t imageDirection) const
{
    PROFILED_FUNCTION();

    DrawPixelInfo& dpi = session.DPI;
    if (dpi.zoom_level > ZoomLevel{ 0 })
    {
        return;
    }

    uint32_t imageId = _VehicleCrashParticleSprites[crashed_sprite_base] + frame / 256;
    auto image = ImageId(imageId, colour[0], colour[1]);
    PaintAddImageAsParent(session, image, { 0, 0, z }, { 1, 1, 0 });
}

/**
 *
 *  rct2: 0x00673699
 */
void CrashSplashParticle::Create(const CoordsXYZ& splashPos)
{
    auto* sprite = CreateEntity<CrashSplashParticle>();
    if (sprite != nullptr)
    {
        sprite->SpriteData.sprite_width = 33;
        sprite->SpriteData.sprite_height_negative = 51;
        sprite->SpriteData.sprite_height_positive = 16;
        sprite->MoveTo(splashPos + CoordsXYZ{ 0, 0, 3 });
        sprite->frame = 0;
    }
}

/**
 *
 *  rct2: 0x0067339D
 */
void CrashSplashParticle::Update()
{
    Invalidate();
    frame += 85;
    if (frame >= 7168)
    {
        EntityRemove(this);
    }
}

void CrashSplashParticle::Serialise(DataSerialiser& stream)
{
    EntityBase::Serialise(stream);
    stream << frame;
}

void CrashSplashParticle::Paint(PaintSession& session, int32_t imageDirection) const
{
    PROFILED_FUNCTION();

    // TODO: Create constant in sprites.h
    uint32_t imageId = 22927 + (frame / 256);
    PaintAddImageAsParent(session, ImageId(imageId), { 0, 0, z }, { 1, 1, 0 });
}

/**
 *
 *  rct2: 0x006734B2
 */
void SteamParticle::Create(const CoordsXYZ& coords)
{
    auto surfaceElement = MapGetSurfaceElementAt(coords);
    if (surfaceElement != nullptr && coords.z > surfaceElement->GetBaseZ())
    {
        SteamParticle* steam = CreateEntity<SteamParticle>();
        if (steam == nullptr)
            return;

        steam->SpriteData.sprite_width = 20;
        steam->SpriteData.sprite_height_negative = 18;
        steam->SpriteData.sprite_height_positive = 16;
        steam->frame = 256;
        steam->time_to_move = 0;
        steam->MoveTo(coords);
    }
}

/**
 *
 *  rct2: 0x00673200
 */
void SteamParticle::Update()
{
    // Move up 1 z every 3 ticks (Starts after 4 ticks)
    Invalidate();
    time_to_move++;
    if (time_to_move >= 4)
    {
        time_to_move = 1;
        MoveTo({ x, y, z + 1 });
    }
    frame += 64;
    if (frame >= (56 * 64))
    {
        EntityRemove(this);
    }
}

void SteamParticle::Serialise(DataSerialiser& stream)
{
    EntityBase::Serialise(stream);
    stream << frame;
    stream << time_to_move;
}

void SteamParticle::Paint(PaintSession& session, int32_t imageDirection) const
{
    PROFILED_FUNCTION();

    // TODO: Create constant in sprites.h
    uint32_t imageId = 22637 + (frame / 256);
    PaintAddImageAsParent(session, ImageId(imageId), { 0, 0, z }, { 1, 1, 0 });
}

/**
 *
 *  rct2: 0x0067363D
 */
void ExplosionCloud::Create(const CoordsXYZ& cloudPos)
{
    auto* entity = CreateEntity<ExplosionCloud>();
    if (entity != nullptr)
    {
        entity->SpriteData.sprite_width = 44;
        entity->SpriteData.sprite_height_negative = 32;
        entity->SpriteData.sprite_height_positive = 34;
        entity->MoveTo(cloudPos + CoordsXYZ{ 0, 0, 4 });
        entity->frame = 0;
    }
}

/**
 *
 *  rct2: 0x00673385
 */
void ExplosionCloud::Update()
{
    Invalidate();
    frame += 128;
    if (frame >= (36 * 128))
    {
        EntityRemove(this);
    }
}

void ExplosionCloud::Serialise(DataSerialiser& stream)
{
    EntityBase::Serialise(stream);
    stream << frame;
}

void ExplosionCloud::Paint(PaintSession& session, int32_t imageDirection) const
{
    PROFILED_FUNCTION();

    uint32_t imageId = 22878 + (frame / 256);
    PaintAddImageAsParent(session, ImageId(imageId), { 0, 0, z }, { 1, 1, 0 });
}

/**
 *
 *  rct2: 0x0067366B
 */
void ExplosionFlare::Create(const CoordsXYZ& flarePos)
{
    auto* entity = CreateEntity<ExplosionFlare>();
    if (entity != nullptr)
    {
        entity->SpriteData.sprite_width = 25;
        entity->SpriteData.sprite_height_negative = 85;
        entity->SpriteData.sprite_height_positive = 8;
        entity->MoveTo(flarePos + CoordsXYZ{ 0, 0, 4 });
        entity->frame = 0;
    }
}

/**
 *
 *  rct2: 0x006733B4
 */
void ExplosionFlare::Update()
{
    Invalidate();
    frame += 64;
    if (frame >= (124 * 64))
    {
        EntityRemove(this);
    }
}

void ExplosionFlare::Serialise(DataSerialiser& stream)
{
    EntityBase::Serialise(stream);
    stream << frame;
}

void ExplosionFlare::Paint(PaintSession& session, int32_t imageDirection) const
{
    PROFILED_FUNCTION();

    // TODO: Create constant in sprites.h
    uint32_t imageId = 22896 + (frame / 256);
    PaintAddImageAsParent(session, ImageId(imageId), { 0, 0, z }, { 1, 1, 0 });
}
