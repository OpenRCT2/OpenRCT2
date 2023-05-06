/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

#    include "../../../Context.h"
#    include "../../../common.h"
#    include "../../../object/ObjectManager.h"
#    include "../../../object/RideObject.h"
#    include "../../../object/SmallSceneryObject.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"

#    include <memory>
#    include <optional>

namespace OpenRCT2::Scripting
{
    class ScObject
    {
    protected:
        ObjectType _type{};
        int32_t _index{};

    public:
        ScObject(ObjectType type, int32_t index)
            : _type(type)
            , _index(index)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScObject::type_get, nullptr, "type");
            dukglue_register_property(ctx, &ScObject::index_get, nullptr, "index");
            dukglue_register_property(ctx, &ScObject::identifier_get, nullptr, "identifier");
            dukglue_register_property(ctx, &ScObject::legacyIdentifier_get, nullptr, "legacyIdentifier");
            dukglue_register_property(ctx, &ScObject::name_get, nullptr, "name");
            dukglue_register_property(ctx, &ScObject::baseImageId_get, nullptr, "baseImageId");
            dukglue_register_property(ctx, &ScObject::numImages_get, nullptr, "numImages");
        }

        static std::optional<ObjectType> StringToObjectType(std::string_view type)
        {
            for (uint8_t i = 0; i < EnumValue(ObjectType::Count); i++)
            {
                auto s = ObjectTypeToString(i);
                if (s == type)
                {
                    return static_cast<ObjectType>(i);
                }
            }
            return ObjectType::None;
        }

        static std::string_view ObjectTypeToString(uint8_t type)
        {
            static constexpr std::string_view Types[] = {
                "ride",
                "small_scenery",
                "large_scenery",
                "wall",
                "banner",
                "footpath",
                "footpath_addition",
                "scenery_group",
                "park_entrance",
                "water",
                "stex",
                "terrain_surface",
                "terrain_edge",
                "station",
                "music",
                "footpath_surface",
                "footpath_railings",
            };
            if (type >= std::size(Types))
                return "unknown";
            return Types[type];
        }

    private:
        std::string type_get() const
        {
            return std::string(ObjectTypeToString(EnumValue(_type)));
        }

        int32_t index_get() const
        {
            return _index;
        }

        std::string identifier_get() const
        {
            auto obj = GetObject();
            if (obj != nullptr)
            {
                return std::string(obj->GetIdentifier());
            }
            return {};
        }

        std::string legacyIdentifier_get() const
        {
            auto obj = GetObject();
            if (obj != nullptr)
            {
                return std::string(obj->GetLegacyIdentifier());
            }
            return {};
        }

        std::string name_get() const
        {
            auto obj = GetObject();
            if (obj != nullptr)
            {
                return obj->GetName();
            }
            return {};
        }

        uint32_t baseImageId_get() const
        {
            auto obj = GetObject();
            if (obj != nullptr)
            {
                return obj->GetBaseImageId();
            }
            return 0;
        }

        uint32_t numImages_get() const
        {
            auto obj = GetObject();
            if (obj != nullptr)
            {
                return obj->GetNumImages();
            }
            return 0;
        }

    protected:
        Object* GetObject() const
        {
            auto& objManager = GetContext()->GetObjectManager();
            return objManager.GetLoadedObject(_type, _index);
        }
    };

    class ScRideObjectVehicle
    {
    private:
        ObjectType _objectType{};
        ObjectEntryIndex _objectIndex{};
        size_t _vehicleIndex{};

    public:
        ScRideObjectVehicle(ObjectType objectType, ObjectEntryIndex objectIndex, size_t vehicleIndex)
            : _objectType(objectType)
            , _objectIndex(objectIndex)
            , _vehicleIndex(vehicleIndex)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScRideObjectVehicle::rotationFrameMask_get, nullptr, "rotationFrameMask");
            dukglue_register_property(ctx, &ScRideObjectVehicle::spacing_get, nullptr, "spacing");
            dukglue_register_property(ctx, &ScRideObjectVehicle::carMass_get, nullptr, "carMass");
            dukglue_register_property(ctx, &ScRideObjectVehicle::tabHeight_get, nullptr, "tabHeight");
            dukglue_register_property(ctx, &ScRideObjectVehicle::numSeats_get, nullptr, "numSeats");
            dukglue_register_property(ctx, &ScRideObjectVehicle::spriteFlags_get, nullptr, "spriteFlags");
            dukglue_register_property(ctx, &ScRideObjectVehicle::spriteWidth_get, nullptr, "spriteWidth");
            dukglue_register_property(ctx, &ScRideObjectVehicle::spriteHeightNegative_get, nullptr, "spriteHeightNegative");
            dukglue_register_property(ctx, &ScRideObjectVehicle::spriteHeightPositive_get, nullptr, "spriteHeightPositive");
            dukglue_register_property(ctx, &ScRideObjectVehicle::animation_get, nullptr, "animation");
            dukglue_register_property(ctx, &ScRideObjectVehicle::flags_get, nullptr, "flags");
            dukglue_register_property(ctx, &ScRideObjectVehicle::baseNumFrames_get, nullptr, "baseNumFrames");
            dukglue_register_property(ctx, &ScRideObjectVehicle::baseImageId_get, nullptr, "baseImageId");
            dukglue_register_property(ctx, &ScRideObjectVehicle::spriteGroups_get, nullptr, "spriteGroups");
            dukglue_register_property(ctx, &ScRideObjectVehicle::noVehicleImages_get, nullptr, "noVehicleImages");
            dukglue_register_property(ctx, &ScRideObjectVehicle::noSeatingRows_get, nullptr, "noSeatingRows");
            dukglue_register_property(ctx, &ScRideObjectVehicle::spinningInertia_get, nullptr, "spinningInertia");
            dukglue_register_property(ctx, &ScRideObjectVehicle::spinningFriction_get, nullptr, "spinningFriction");
            dukglue_register_property(ctx, &ScRideObjectVehicle::frictionSoundId_get, nullptr, "frictionSoundId");
            dukglue_register_property(
                ctx, &ScRideObjectVehicle::logFlumeReverserVehicleType_get, nullptr, "logFlumeReverserVehicleType");
            dukglue_register_property(ctx, &ScRideObjectVehicle::soundRange_get, nullptr, "soundRange");
            dukglue_register_property(ctx, &ScRideObjectVehicle::doubleSoundFrequency_get, nullptr, "doubleSoundFrequency");
            dukglue_register_property(ctx, &ScRideObjectVehicle::poweredAcceleration_get, nullptr, "poweredAcceleration");
            dukglue_register_property(ctx, &ScRideObjectVehicle::poweredMaxSpeed_get, nullptr, "poweredMaxSpeed");
            dukglue_register_property(ctx, &ScRideObjectVehicle::carVisual_get, nullptr, "carVisual");
            dukglue_register_property(ctx, &ScRideObjectVehicle::effectVisual_get, nullptr, "effectVisual");
            dukglue_register_property(ctx, &ScRideObjectVehicle::drawOrder_get, nullptr, "drawOrder");
            dukglue_register_property(
                ctx, &ScRideObjectVehicle::numVerticalFramesOverride_get, nullptr, "numVerticalFramesOverride");
        }

    private:
        uint16_t rotationFrameMask_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->TabRotationMask;
            }
            return 0;
        }

        uint32_t spacing_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->spacing;
            }
            return 0;
        }

        uint16_t carMass_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->car_mass;
            }
            return 0;
        }

        int8_t tabHeight_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->tab_height;
            }
            return 0;
        }

        uint8_t numSeats_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->num_seats;
            }
            return 0;
        }

        uint16_t spriteFlags_get() const
        {
            return 0;
        }

        uint8_t spriteWidth_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->sprite_width;
            }
            return 0;
        }

        uint8_t spriteHeightNegative_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->sprite_height_negative;
            }
            return 0;
        }

        uint8_t spriteHeightPositive_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->sprite_height_positive;
            }
            return 0;
        }

        uint8_t animation_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return EnumValue(entry->animation);
            }
            return 0;
        }

        uint32_t flags_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->flags;
            }
            return 0;
        }

        uint16_t baseNumFrames_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->base_num_frames;
            }
            return 0;
        }

        uint32_t baseImageId_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->base_image_id;
            }
            return 0;
        }

        DukValue spriteGroups_get() const
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto* ctx = scriptEngine.GetContext();
            DukObject groups(ctx);
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                for (uint8_t g = 0; g < EnumValue<SpriteGroupType>(SpriteGroupType::Count); g++)
                {
                    auto group = entry->SpriteGroups[g];
                    if (group.Enabled())
                        groups.Set(SpriteGroupNames[g].c_str(), ToDuk<VehicleSpriteGroup>(ctx, group));
                }
            }
            return groups.Take();
        }

        uint32_t noVehicleImages_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->NumCarImages;
            }
            return 0;
        }

        uint8_t noSeatingRows_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->no_seating_rows;
            }
            return 0;
        }

        uint8_t spinningInertia_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->spinning_inertia;
            }
            return 0;
        }

        uint8_t spinningFriction_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->spinning_friction;
            }
            return 0;
        }

        int32_t frictionSoundId_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return static_cast<int32_t>(entry->friction_sound_id);
            }
            return 0;
        }

        uint8_t logFlumeReverserVehicleType_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->ReversedCarIndex;
            }
            return 0;
        }

        uint8_t soundRange_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->sound_range;
            }
            return 0;
        }

        uint8_t doubleSoundFrequency_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->double_sound_frequency;
            }
            return 0;
        }

        uint8_t poweredAcceleration_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->powered_acceleration;
            }
            return 0;
        }

        uint8_t poweredMaxSpeed_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->powered_max_speed;
            }
            return 0;
        }

        uint8_t carVisual_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->PaintStyle;
            }
            return 0;
        }

        uint8_t effectVisual_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->effect_visual;
            }
            return 0;
        }

        uint8_t drawOrder_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->draw_order;
            }
            return 0;
        }

        uint8_t numVerticalFramesOverride_get() const
        {
            auto entry = GetEntry();
            if (entry != nullptr)
            {
                return entry->num_vertical_frames_override;
            }
            return 0;
        }

        Object* GetObject() const
        {
            auto& objManager = GetContext()->GetObjectManager();
            return static_cast<RideObject*>(objManager.GetLoadedObject(_objectType, _objectIndex));
        }

        const CarEntry* GetEntry() const
        {
            auto obj = GetObject();
            if (obj != nullptr)
            {
                auto rideEntry = static_cast<RideObjectEntry*>(obj->GetLegacyData());
                if (rideEntry != nullptr && _vehicleIndex < std::size(rideEntry->Cars))
                {
                    return rideEntry->GetCar(_vehicleIndex);
                }
            }
            return nullptr;
        }
    };

    class ScRideObject : public ScObject
    {
    public:
        ScRideObject(ObjectType type, int32_t index)
            : ScObject(type, index)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScObject, ScRideObject>(ctx);
            dukglue_register_property(ctx, &ScRideObject::description_get, nullptr, "description");
            dukglue_register_property(ctx, &ScRideObject::capacity_get, nullptr, "capacity");
            dukglue_register_property(ctx, &ScRideObject::firstImageId_get, nullptr, "firstImageId");
            dukglue_register_property(ctx, &ScRideObject::flags_get, nullptr, "flags");
            dukglue_register_property(ctx, &ScRideObject::rideType_get, nullptr, "rideType");
            dukglue_register_property(ctx, &ScRideObject::minCarsInTrain_get, nullptr, "minCarsInTrain");
            dukglue_register_property(ctx, &ScRideObject::maxCarsInTrain_get, nullptr, "maxCarsInTrain");
            dukglue_register_property(ctx, &ScRideObject::carsPerFlatRide_get, nullptr, "carsPerFlatRide");
            dukglue_register_property(ctx, &ScRideObject::zeroCars_get, nullptr, "zeroCars");
            dukglue_register_property(ctx, &ScRideObject::tabVehicle_get, nullptr, "tabVehicle");
            dukglue_register_property(ctx, &ScRideObject::defaultVehicle_get, nullptr, "defaultVehicle");
            dukglue_register_property(ctx, &ScRideObject::frontVehicle_get, nullptr, "frontVehicle");
            dukglue_register_property(ctx, &ScRideObject::secondVehicle_get, nullptr, "secondVehicle");
            dukglue_register_property(ctx, &ScRideObject::rearVehicle_get, nullptr, "rearVehicle");
            dukglue_register_property(ctx, &ScRideObject::thirdVehicle_get, nullptr, "thirdVehicle");
            dukglue_register_property(ctx, &ScRideObject::vehicles_get, nullptr, "vehicles");
            dukglue_register_property(ctx, &ScRideObject::excitementMultiplier_get, nullptr, "excitementMultiplier");
            dukglue_register_property(ctx, &ScRideObject::intensityMultiplier_get, nullptr, "intensityMultiplier");
            dukglue_register_property(ctx, &ScRideObject::nauseaMultiplier_get, nullptr, "nauseaMultiplier");
            dukglue_register_property(ctx, &ScRideObject::maxHeight_get, nullptr, "maxHeight");
            dukglue_register_property(ctx, &ScRideObject::shopItem_get, nullptr, "shopItem");
            dukglue_register_property(ctx, &ScRideObject::shopItemSecondary_get, nullptr, "shopItemSecondary");
        }

    private:
        std::string description_get() const
        {
            auto obj = GetObject();
            if (obj != nullptr)
            {
                return obj->GetDescription();
            }
            return {};
        }

        std::string capacity_get() const
        {
            auto obj = GetObject();
            if (obj != nullptr)
            {
                return obj->GetCapacity();
            }
            return {};
        }

        uint32_t firstImageId_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->images_offset;
            }
            return 0;
        }

        uint32_t flags_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->flags;
            }
            return 0;
        }

        std::vector<uint8_t> rideType_get() const
        {
            std::vector<uint8_t> result;
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                for (auto rideType : entry->ride_type)
                {
                    result.push_back(rideType);
                }
            }
            return result;
        }

        uint8_t minCarsInTrain_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->min_cars_in_train;
            }
            return 0;
        }

        uint8_t maxCarsInTrain_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->max_cars_in_train;
            }
            return 0;
        }

        uint8_t carsPerFlatRide_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->cars_per_flat_ride;
            }
            return 0;
        }

        uint8_t zeroCars_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->zero_cars;
            }
            return 0;
        }

        uint8_t tabVehicle_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->TabCar;
            }
            return 0;
        }

        uint8_t defaultVehicle_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->DefaultCar;
            }
            return 0;
        }

        uint8_t frontVehicle_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->FrontCar;
            }
            return 0;
        }

        uint8_t secondVehicle_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->SecondCar;
            }
            return 0;
        }

        uint8_t rearVehicle_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->RearCar;
            }
            return 0;
        }

        uint8_t thirdVehicle_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->ThirdCar;
            }
            return 0;
        }

        std::vector<std::shared_ptr<ScRideObjectVehicle>> vehicles_get() const
        {
            std::vector<std::shared_ptr<ScRideObjectVehicle>> result;
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                for (size_t i = 0; i < std::size(entry->Cars); i++)
                {
                    result.push_back(std::make_shared<ScRideObjectVehicle>(static_cast<ObjectType>(_type), _index, i));
                }
            }
            return result;
        }

        int8_t excitementMultiplier_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->excitement_multiplier;
            }
            return 0;
        }

        int8_t intensityMultiplier_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->intensity_multiplier;
            }
            return 0;
        }

        int8_t nauseaMultiplier_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->nausea_multiplier;
            }
            return 0;
        }

        uint8_t maxHeight_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return entry->max_height;
            }
            return 0;
        }

        uint8_t shopItem_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return EnumValue(entry->shop_item[0]);
            }
            return 0;
        }

        uint8_t shopItemSecondary_get() const
        {
            auto entry = GetLegacyData();
            if (entry != nullptr)
            {
                return EnumValue(entry->shop_item[1]);
            }
            return 0;
        }

    protected:
        RideObject* GetObject() const
        {
            return static_cast<RideObject*>(ScObject::GetObject());
        }

        const RideObjectEntry* GetLegacyData() const
        {
            auto obj = GetObject();
            if (obj != nullptr)
            {
                return static_cast<RideObjectEntry*>(obj->GetLegacyData());
            }
            return nullptr;
        }
    };

    class ScSmallSceneryObject : public ScObject
    {
    public:
        ScSmallSceneryObject(ObjectType type, int32_t index)
            : ScObject(type, index)
        {
        }

        static void Register(duk_context* ctx)
        {
            dukglue_set_base_class<ScObject, ScSmallSceneryObject>(ctx);
            dukglue_register_property(ctx, &ScSmallSceneryObject::flags_get, nullptr, "flags");
            dukglue_register_property(ctx, &ScSmallSceneryObject::height_get, nullptr, "height");
            dukglue_register_property(ctx, &ScSmallSceneryObject::price_get, nullptr, "price");
            dukglue_register_property(ctx, &ScSmallSceneryObject::removalPrice_get, nullptr, "removalPrice");
        }

    private:
        uint32_t flags_get() const
        {
            auto sceneryEntry = GetLegacyData();
            if (sceneryEntry != nullptr)
            {
                return sceneryEntry->flags;
            }
            return 0;
        }

        uint8_t height_get() const
        {
            auto sceneryEntry = GetLegacyData();
            if (sceneryEntry != nullptr)
            {
                return sceneryEntry->height;
            }
            return 0;
        }

        uint8_t price_get() const
        {
            auto sceneryEntry = GetLegacyData();
            if (sceneryEntry != nullptr)
            {
                return sceneryEntry->price;
            }
            return 0;
        }

        uint8_t removalPrice_get() const
        {
            auto sceneryEntry = GetLegacyData();
            if (sceneryEntry != nullptr)
            {
                return sceneryEntry->removal_price;
            }
            return 0;
        }

    protected:
        SmallSceneryEntry* GetLegacyData() const
        {
            auto obj = GetObject();
            if (obj != nullptr)
            {
                return static_cast<SmallSceneryEntry*>(obj->GetLegacyData());
            }
            return nullptr;
        }

        SmallSceneryObject* GetObject() const
        {
            return static_cast<SmallSceneryObject*>(ScObject::GetObject());
        }
    };
} // namespace OpenRCT2::Scripting

#endif
