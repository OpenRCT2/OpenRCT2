#include "GameStateSnapshots.h"

#include "core/CircularBuffer.h"
#include "peep/Peep.h"
#include "world/Sprite.h"

static constexpr size_t MaximumGameStateSnapshots = 32;
static constexpr uint32_t InvalidTick = 0xFFFFFFFF;

struct GameStateSnapshot_t
{
    GameStateSnapshot_t& operator=(GameStateSnapshot_t&& mv)
    {
        tick = mv.tick;
        storedSprites = std::move(mv.storedSprites);
        return *this;
    }

    uint32_t tick = InvalidTick;

    MemoryStream storedSprites;

    void SerialiseSprites(rct_sprite* sprites, const size_t numSprites, bool saving)
    {
        const bool loading = !saving;

        storedSprites.SetPosition(0);
        DataSerialiser ds(saving, storedSprites);

        std::vector<uint32_t> indexTable;
        indexTable.reserve(numSprites);

        uint32_t numSavedSprites = 0;

        if (saving)
        {
            for (size_t i = 0; i < numSprites; i++)
            {
                if (sprites[i].generic.sprite_identifier == SPRITE_IDENTIFIER_NULL)
                    continue;
                indexTable.push_back((uint32_t)i);
            }
            numSavedSprites = (uint32_t)indexTable.size();
        }

        ds << numSavedSprites;

        if (loading)
        {
            indexTable.resize(numSavedSprites);
        }

        for (uint32_t i = 0; i < numSavedSprites; i++)
        {
            ds << indexTable[i];

            const uint32_t spriteIdx = indexTable[i];
            rct_sprite& sprite = sprites[spriteIdx];

            ds << sprite.generic.sprite_identifier;

            switch (sprite.generic.sprite_identifier)
            {
                case SPRITE_IDENTIFIER_VEHICLE:
                    ds << reinterpret_cast<uint8_t(&)[sizeof(rct_vehicle)]>(sprite.vehicle);
                    break;
                case SPRITE_IDENTIFIER_PEEP:
                    ds << reinterpret_cast<uint8_t(&)[sizeof(Peep)]>(sprite.peep);
                    break;
                case SPRITE_IDENTIFIER_LITTER:
                    ds << reinterpret_cast<uint8_t(&)[sizeof(rct_litter)]>(sprite.litter);
                    break;
            }
        }
    }
};

struct GameStateSnapshots : public IGameStateSnapshots
{
    virtual GameStateSnapshot_t& CreateSnapshot() override final
    {
        auto snapshot = std::make_unique<GameStateSnapshot_t>();
        _snapshots.push_back(std::move(snapshot));

        return *_snapshots.back();
    }

    virtual void LinkSnapshot(GameStateSnapshot_t& snapshot, uint32_t tick) override final
    {
        snapshot.tick = tick;
    }

    virtual void Capture(GameStateSnapshot_t& snapshot) override final
    {
        snapshot.SerialiseSprites(get_sprite(0), MAX_SPRITES, true);

        log_info("Snapshot size: %u bytes\n", (uint32_t)snapshot.storedSprites.GetLength());
    }

    virtual const GameStateSnapshot_t* GetLinkedSnapshot(uint32_t tick) const override final
    {
        for (size_t i = 0; i < _snapshots.size(); i++)
        {
            if (_snapshots[i]->tick == tick)
                return _snapshots[i].get();
        }
        return nullptr;
    }

    virtual void SerialiseSnapshot(GameStateSnapshot_t& snapshot, DataSerialiser& serialiser) const override final
    {
    }

    virtual GameStateCompareData_t Compare(
        const GameStateSnapshot_t& left, const GameStateSnapshot_t& right) const override final
    {
        return GameStateCompareData_t{};
    }

private:
    CircularBuffer<std::unique_ptr<GameStateSnapshot_t>, MaximumGameStateSnapshots> _snapshots;
};

std::unique_ptr<IGameStateSnapshots> CreateGameStateSnapshots()
{
    return std::make_unique<GameStateSnapshots>();
}
