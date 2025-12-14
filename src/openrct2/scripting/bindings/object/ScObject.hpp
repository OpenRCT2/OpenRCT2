/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../Context.h"
    #include "../../../object/LargeSceneryObject.h"
    #include "../../../object/ObjectManager.h"
    #include "../../../object/RideObject.h"
    #include "../../../object/SceneryGroupObject.h"
    #include "../../../object/SmallSceneryObject.h"
    #include "../../ScriptEngine.h"
    #include "ScInstalledObject.hpp"

    #include <memory>
    #include <optional>

namespace OpenRCT2::Scripting
{
    class ScObject;
    extern ScObject gScObject;

    class ScObject : public ScBase
    {
    protected:
        struct ObjectData
        {
            ObjectType _type{};
            int32_t _index{};
        };

    public:
        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "Object", Finalize);
        }

        JSValue NewInstance(JSContext* ctx, ObjectType type, int32_t index)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("installedObject", ScObject::installedObject_get, nullptr),
                JS_CGETSET_DEF("type", ScObject::type_get, nullptr),
                JS_CGETSET_DEF("index", ScObject::index_get, nullptr),
                JS_CGETSET_DEF("identifier", ScObject::identifier_get, nullptr),
                JS_CGETSET_DEF("legacyIdentifier", ScObject::legacyIdentifier_get, nullptr),
                JS_CGETSET_DEF("name", ScObject::name_get, nullptr),
                JS_CGETSET_DEF("baseImageId", ScObject::baseImageId_get, nullptr),
                JS_CGETSET_DEF("numImages", ScObject::numImages_get, nullptr),
            };
            return MakeWithOpaque(ctx, funcs, new ObjectData{ type, index });
        }

        static JSValue New(JSContext* ctx, ObjectType type, int32_t index)
        {
            return gScObject.NewInstance(ctx, type, index);
        }

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal)
        {
            ObjectData* data = GetObjectData(thisVal);
            if (data)
                delete data;
        }

        static JSValue installedObject_get(JSContext* ctx, JSValue thisVal)
        {
            auto obj = GetObject(thisVal);
            if (obj != nullptr)
            {
                auto& objectRepository = GetContext()->GetObjectRepository();
                auto installedObject = objectRepository.FindObject(obj->GetDescriptor());
                if (installedObject != nullptr)
                {
                    return gScInstalledObject.New(ctx, installedObject->Id);
                }
            }
            return JS_NULL;
        }

        static JSValue type_get(JSContext* ctx, JSValue thisVal)
        {
            auto* data = GetObjectData(thisVal);
            return JSFromStdString(ctx, std::string(objectTypeToString(data->_type)));
        }

        static JSValue index_get(JSContext* ctx, JSValue thisVal)
        {
            auto* data = GetObjectData(thisVal);
            return JS_NewInt32(ctx, data->_index);
        }

        static JSValue identifier_get(JSContext* ctx, JSValue thisVal)
        {
            auto obj = GetObject(thisVal);
            std::string str;
            if (obj != nullptr)
            {
                if (obj->GetGeneration() == ObjectGeneration::DAT)
                {
                    str = obj->GetDescriptor().ToString();
                }
                else
                {
                    str = obj->GetIdentifier();
                }
            }
            else
            {
                str = "";
            }
            return JSFromStdString(ctx, str);
        }

        static JSValue legacyIdentifier_get(JSContext* ctx, JSValue thisVal)
        {
            auto obj = GetObject(thisVal);
            if (obj != nullptr && obj->GetLegacyIdentifier().find('\0') == std::string::npos)
            {
                return JSFromStdString(ctx, obj->GetLegacyIdentifier());
            }
            return JSFromStdString(ctx, {});
        }

        static JSValue name_get(JSContext* ctx, JSValue thisVal)
        {
            auto obj = GetObject(thisVal);
            return JSFromStdString(ctx, obj != nullptr ? obj->GetName() : "");
        }

        static JSValue baseImageId_get(JSContext* ctx, JSValue thisVal)
        {
            auto obj = GetObject(thisVal);
            return JS_NewUint32(ctx, obj != nullptr ? obj->GetBaseImageId() : 0);
        }

        static JSValue numImages_get(JSContext* ctx, JSValue thisVal)
        {
            auto obj = GetObject(thisVal);
            return JS_NewUint32(ctx, obj != nullptr ? obj->GetNumImages() : 0);
        }

    protected:
        static ObjectData* GetObjectData(JSValue thisVal)
        {
            return gScObject.GetOpaque<ObjectData*>(thisVal);
        }

        static Object* GetObject(JSValue thisVal)
        {
            ObjectData* data = GetObjectData(thisVal);
            auto& objManager = GetContext()->GetObjectManager();
            return objManager.GetLoadedObject(data->_type, data->_index);
        }
    };

    class ScRideObjectVehicle;
    extern ScRideObjectVehicle gScRideObjectVehicle;

    class ScRideObjectVehicle final : public ScBase
    {
    private:
        struct VehicleData
        {
            ObjectEntryIndex _objectIndex{};
            size_t _vehicleIndex{};
        };

    public:
        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "RideObjectVehicle", Finalize);
        }

        JSValue New(JSContext* ctx, ObjectEntryIndex objectIndex, size_t vehicleIndex)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("rotationFrameMask", ScRideObjectVehicle::rotationFrameMask_get, nullptr),
                JS_CGETSET_DEF("spacing", ScRideObjectVehicle::spacing_get, nullptr),
                JS_CGETSET_DEF("carMass", ScRideObjectVehicle::carMass_get, nullptr),
                JS_CGETSET_DEF("tabHeight", ScRideObjectVehicle::tabHeight_get, nullptr),
                JS_CGETSET_DEF("numSeats", ScRideObjectVehicle::numSeats_get, nullptr),
                JS_CGETSET_DEF("spriteFlags", ScRideObjectVehicle::spriteFlags_get, nullptr),
                JS_CGETSET_DEF("spriteWidth", ScRideObjectVehicle::spriteWidth_get, nullptr),
                JS_CGETSET_DEF("spriteHeightNegative", ScRideObjectVehicle::spriteHeightNegative_get, nullptr),
                JS_CGETSET_DEF("spriteHeightPositive", ScRideObjectVehicle::spriteHeightPositive_get, nullptr),
                JS_CGETSET_DEF("animation", ScRideObjectVehicle::animation_get, nullptr),
                JS_CGETSET_DEF("flags", ScRideObjectVehicle::flags_get, nullptr),
                JS_CGETSET_DEF("baseNumFrames", ScRideObjectVehicle::baseNumFrames_get, nullptr),
                JS_CGETSET_DEF("baseImageId", ScRideObjectVehicle::baseImageId_get, nullptr),
                JS_CGETSET_DEF("spriteGroups", ScRideObjectVehicle::spriteGroups_get, nullptr),
                JS_CGETSET_DEF("noVehicleImages", ScRideObjectVehicle::noVehicleImages_get, nullptr),
                JS_CGETSET_DEF("noSeatingRows", ScRideObjectVehicle::noSeatingRows_get, nullptr),
                JS_CGETSET_DEF("spinningInertia", ScRideObjectVehicle::spinningInertia_get, nullptr),
                JS_CGETSET_DEF("spinningFriction", ScRideObjectVehicle::spinningFriction_get, nullptr),
                JS_CGETSET_DEF("frictionSoundId", ScRideObjectVehicle::frictionSoundId_get, nullptr),
                JS_CGETSET_DEF("logFlumeReverserVehicleType", ScRideObjectVehicle::logFlumeReverserVehicleType_get, nullptr),
                JS_CGETSET_DEF("soundRange", ScRideObjectVehicle::soundRange_get, nullptr),
                JS_CGETSET_DEF("doubleSoundFrequency", ScRideObjectVehicle::doubleSoundFrequency_get, nullptr),
                JS_CGETSET_DEF("poweredAcceleration", ScRideObjectVehicle::poweredAcceleration_get, nullptr),
                JS_CGETSET_DEF("poweredMaxSpeed", ScRideObjectVehicle::poweredMaxSpeed_get, nullptr),
                JS_CGETSET_DEF("carVisual", ScRideObjectVehicle::carVisual_get, nullptr),
                JS_CGETSET_DEF("effectVisual", ScRideObjectVehicle::effectVisual_get, nullptr),
                JS_CGETSET_DEF("drawOrder", ScRideObjectVehicle::drawOrder_get, nullptr),
                JS_CGETSET_DEF("numVerticalFramesOverride", ScRideObjectVehicle::numVerticalFramesOverride_get, nullptr),
            };
            return MakeWithOpaque(ctx, funcs, new VehicleData{ objectIndex, vehicleIndex });
        }

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal)
        {
            VehicleData* data = gScRideObjectVehicle.GetOpaque<VehicleData*>(thisVal);
            if (data)
                delete data;
        }

        static JSValue rotationFrameMask_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->TabRotationMask : 0);
        }

        static JSValue spacing_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->spacing : 0);
        }

        static JSValue carMass_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->car_mass : 0);
        }

        static JSValue tabHeight_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewInt32(ctx, carEntry != nullptr ? carEntry->tab_height : 0);
        }

        static JSValue numSeats_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->num_seats : 0);
        }

        static JSValue spriteFlags_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewUint32(ctx, 0);
        }

        static JSValue spriteWidth_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->spriteWidth : 0);
        }

        static JSValue spriteHeightNegative_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->spriteHeightNegative : 0);
        }

        static JSValue spriteHeightPositive_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->spriteHeightPositive : 0);
        }

        static JSValue animation_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? EnumValue(carEntry->animation) : 0);
        }

        static JSValue flags_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewInt64(ctx, carEntry != nullptr ? static_cast<int64_t>(carEntry->flags) : 0);
        }

        static JSValue baseNumFrames_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->base_num_frames : 0);
        }

        static JSValue baseImageId_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->base_image_id : 0);
        }

        static JSValue spriteGroups_get(JSContext* ctx, JSValue thisVal)
        {
            JSValue groups = JS_NewObject(ctx);
            auto carEntry = GetEntry(thisVal);
            if (carEntry != nullptr)
            {
                for (std::underlying_type_t<SpriteGroupType> g = 0; g < EnumValue<SpriteGroupType>(SpriteGroupType::Count); g++)
                {
                    auto group = carEntry->SpriteGroups[g];
                    if (group.Enabled())
                    {
                        JSValue groupObj = JS_NewObject(ctx);
                        JS_SetPropertyStr(ctx, groupObj, "imageId", JS_NewUint32(ctx, group.imageId));
                        JS_SetPropertyStr(
                            ctx, groupObj, "spriteNumImages",
                            JS_NewUint32(ctx, OpenRCT2::Entity::Yaw::NumSpritesPrecision(group.spritePrecision)));
                        JS_SetPropertyStr(ctx, groups, SpriteGroupNames[g], groupObj);
                    }
                }
            }
            return groups;
        }

        static JSValue noVehicleImages_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->NumCarImages : 0);
        }

        static JSValue noSeatingRows_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->no_seating_rows : 0);
        }

        static JSValue spinningInertia_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->spinning_inertia : 0);
        }

        static JSValue spinningFriction_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->spinning_friction : 0);
        }

        static JSValue frictionSoundId_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewInt32(ctx, carEntry != nullptr ? static_cast<int32_t>(carEntry->friction_sound_id) : 0);
        }

        static JSValue logFlumeReverserVehicleType_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->ReversedCarIndex : 0);
        }

        static JSValue soundRange_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? EnumValue(carEntry->soundRange) : 0);
        }

        static JSValue doubleSoundFrequency_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->double_sound_frequency : 0);
        }

        static JSValue poweredAcceleration_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->powered_acceleration : 0);
        }

        static JSValue poweredMaxSpeed_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->powered_max_speed : 0);
        }

        static JSValue carVisual_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->PaintStyle : 0);
        }

        static JSValue effectVisual_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->effect_visual : 0);
        }

        static JSValue drawOrder_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->draw_order : 0);
        }

        static JSValue numVerticalFramesOverride_get(JSContext* ctx, JSValue thisVal)
        {
            auto carEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, carEntry != nullptr ? carEntry->num_vertical_frames_override : 0);
        }

        static const RideObject* GetObject(JSValue thisVal)
        {
            VehicleData* data = gScRideObjectVehicle.GetOpaque<VehicleData*>(thisVal);
            auto& objManager = GetContext()->GetObjectManager();
            return objManager.GetLoadedObject<RideObject>(data->_objectIndex);
        }

        static const CarEntry* GetEntry(JSValue thisVal)
        {
            auto obj = GetObject(thisVal);
            if (obj != nullptr)
            {
                VehicleData* data = gScRideObjectVehicle.GetOpaque<VehicleData*>(thisVal);
                auto rideEntry = &obj->GetEntry();
                if (rideEntry != nullptr && data->_vehicleIndex < std::size(rideEntry->Cars))
                {
                    return rideEntry->GetCar(data->_vehicleIndex);
                }
            }
            return nullptr;
        }
    };

    class ScRideObject final : public ScObject
    {
    public:
        static JSValue New(JSContext* ctx, ObjectType type, int32_t index)
        {
            JSValue obj = ScObject::New(ctx, type, index);
            AddFuncs(ctx, obj);
            return obj;
        }

    private:
        static void AddFuncs(JSContext* ctx, JSValue obj)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("description", ScRideObject::description_get, nullptr),
                JS_CGETSET_DEF("capacity", ScRideObject::capacity_get, nullptr),
                JS_CGETSET_DEF("firstImageId", ScRideObject::firstImageId_get, nullptr),
                JS_CGETSET_DEF("flags", ScRideObject::flags_get, nullptr),
                JS_CGETSET_DEF("rideType", ScRideObject::rideType_get, nullptr),
                JS_CGETSET_DEF("minCarsInTrain", ScRideObject::minCarsInTrain_get, nullptr),
                JS_CGETSET_DEF("maxCarsInTrain", ScRideObject::maxCarsInTrain_get, nullptr),
                JS_CGETSET_DEF("carsPerFlatRide", ScRideObject::carsPerFlatRide_get, nullptr),
                JS_CGETSET_DEF("zeroCars", ScRideObject::zeroCars_get, nullptr),
                JS_CGETSET_DEF("tabVehicle", ScRideObject::tabVehicle_get, nullptr),
                JS_CGETSET_DEF("defaultVehicle", ScRideObject::defaultVehicle_get, nullptr),
                JS_CGETSET_DEF("frontVehicle", ScRideObject::frontVehicle_get, nullptr),
                JS_CGETSET_DEF("secondVehicle", ScRideObject::secondVehicle_get, nullptr),
                JS_CGETSET_DEF("rearVehicle", ScRideObject::rearVehicle_get, nullptr),
                JS_CGETSET_DEF("thirdVehicle", ScRideObject::thirdVehicle_get, nullptr),
                JS_CGETSET_DEF("vehicles", ScRideObject::vehicles_get, nullptr),
                JS_CGETSET_DEF("excitementMultiplier", ScRideObject::excitementMultiplier_get, nullptr),
                JS_CGETSET_DEF("intensityMultiplier", ScRideObject::intensityMultiplier_get, nullptr),
                JS_CGETSET_DEF("nauseaMultiplier", ScRideObject::nauseaMultiplier_get, nullptr),
                JS_CGETSET_DEF("maxHeight", ScRideObject::maxHeight_get, nullptr),
                JS_CGETSET_DEF("shopItem", ScRideObject::shopItem_get, nullptr),
                JS_CGETSET_DEF("shopItemSecondary", ScRideObject::shopItemSecondary_get, nullptr),
            };
            JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
        }

        static JSValue description_get(JSContext* ctx, JSValue thisVal)
        {
            auto obj = GetRideObject(thisVal);
            return JSFromStdString(ctx, obj != nullptr ? obj->GetDescription() : "");
        }

        static JSValue capacity_get(JSContext* ctx, JSValue thisVal)
        {
            auto obj = GetRideObject(thisVal);
            return JSFromStdString(ctx, obj != nullptr ? obj->GetCapacity() : "");
        }

        static JSValue firstImageId_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, rideEntry != nullptr ? rideEntry->images_offset : 0);
        }

        static JSValue flags_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, rideEntry != nullptr ? rideEntry->flags : 0);
        }

        static JSValue rideType_get(JSContext* ctx, JSValue thisVal)
        {
            JSValue result = JS_NewArray(ctx);
            auto rideEntry = GetEntry(thisVal);
            if (rideEntry != nullptr)
            {
                int64_t index = 0;
                for (auto rideType : rideEntry->ride_type)
                {
                    JS_SetPropertyInt64(ctx, result, index++, JS_NewUint32(ctx, rideType));
                }
            }
            return result;
        }

        static JSValue minCarsInTrain_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, rideEntry != nullptr ? rideEntry->min_cars_in_train : 0);
        }

        static JSValue maxCarsInTrain_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, rideEntry != nullptr ? rideEntry->max_cars_in_train : 0);
        }

        static JSValue carsPerFlatRide_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, rideEntry != nullptr ? rideEntry->cars_per_flat_ride : 0);
        }

        static JSValue zeroCars_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, rideEntry != nullptr ? rideEntry->zero_cars : 0);
        }

        static JSValue tabVehicle_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, rideEntry != nullptr ? rideEntry->TabCar : 0);
        }

        static JSValue defaultVehicle_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, rideEntry != nullptr ? rideEntry->DefaultCar : 0);
        }

        static JSValue frontVehicle_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, rideEntry != nullptr ? rideEntry->FrontCar : 0);
        }

        static JSValue secondVehicle_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, rideEntry != nullptr ? rideEntry->SecondCar : 0);
        }

        static JSValue rearVehicle_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, rideEntry != nullptr ? rideEntry->RearCar : 0);
        }

        static JSValue thirdVehicle_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, rideEntry != nullptr ? rideEntry->ThirdCar : 0);
        }

        static JSValue vehicles_get(JSContext* ctx, JSValue thisVal)
        {
            JSValue result = JS_NewArray(ctx);
            auto rideEntry = GetEntry(thisVal);
            if (rideEntry != nullptr)
            {
                ObjectData* data = GetObjectData(thisVal);
                int64_t index = 0;
                for (size_t i = 0; i < std::size(rideEntry->Cars); i++)
                {
                    JS_SetPropertyInt64(ctx, result, index++, gScRideObjectVehicle.New(ctx, data->_index, i));
                }
            }
            return result;
        }

        static JSValue excitementMultiplier_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewInt32(ctx, rideEntry != nullptr ? rideEntry->excitement_multiplier : 0);
        }

        static JSValue intensityMultiplier_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewInt32(ctx, rideEntry != nullptr ? rideEntry->intensity_multiplier : 0);
        }

        static JSValue nauseaMultiplier_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewInt32(ctx, rideEntry != nullptr ? rideEntry->nausea_multiplier : 0);
        }

        static JSValue maxHeight_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, rideEntry != nullptr ? rideEntry->maxHeight : 0);
        }

        static JSValue shopItem_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, rideEntry != nullptr ? EnumValue(rideEntry->shop_item[0]) : 0);
        }

        static JSValue shopItemSecondary_get(JSContext* ctx, JSValue thisVal)
        {
            auto rideEntry = GetEntry(thisVal);
            return JS_NewUint32(ctx, rideEntry != nullptr ? EnumValue(rideEntry->shop_item[1]) : 0);
        }

    protected:
        static RideObject* GetRideObject(JSValue thisVal)
        {
            return static_cast<RideObject*>(GetObject(thisVal));
        }

        static const RideObjectEntry* GetEntry(JSValue thisVal)
        {
            auto obj = GetRideObject(thisVal);
            if (obj != nullptr)
            {
                return &obj->GetEntry();
            }
            return nullptr;
        }
    };

    class ScSceneryObject : public ScObject
    {
    public:
        static JSValue New(JSContext* ctx, ObjectType type, int32_t index)
        {
            JSValue obj = ScObject::New(ctx, type, index);
            AddFuncs(ctx, obj);
            return obj;
        }

    private:
        static void AddFuncs(JSContext* ctx, JSValue obj)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("sceneryGroups", ScSceneryObject::sceneryGroups_get, nullptr),
            };
            JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
        }

        static JSValue sceneryGroups_get(JSContext* ctx, JSValue thisVal)
        {
            JSValue result = JS_NewArray(ctx);
            auto obj = GetSceneryObject(thisVal);
            if (obj != nullptr)
            {
                auto& scgDescriptor = obj->GetPrimarySceneryGroup();
                if (scgDescriptor.HasValue())
                {
                    JS_SetPropertyInt64(ctx, result, 0, JSFromStdString(ctx, scgDescriptor.ToString()));
                }
            }
            return result;
        }

    protected:
        static SceneryObject* GetSceneryObject(JSValue thisVal)
        {
            return static_cast<SceneryObject*>(GetObject(thisVal));
        }
    };

    class ScSmallSceneryObject final : public ScSceneryObject
    {
    public:
        static JSValue New(JSContext* ctx, ObjectType type, int32_t index)
        {
            JSValue obj = ScSceneryObject::New(ctx, type, index);
            AddFuncs(ctx, obj);
            return obj;
        }

    private:
        static void AddFuncs(JSContext* ctx, JSValue obj)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("flags", ScSmallSceneryObject::flags_get, nullptr),
                JS_CGETSET_DEF("height", ScSmallSceneryObject::height_get, nullptr),
                JS_CGETSET_DEF("price", ScSmallSceneryObject::price_get, nullptr),
                JS_CGETSET_DEF("removalPrice", ScSmallSceneryObject::removalPrice_get, nullptr),
            };
            JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
        }

        static JSValue flags_get(JSContext* ctx, JSValue thisVal)
        {
            auto sceneryEntry = GetLegacyData(thisVal);
            return JS_NewUint32(ctx, sceneryEntry != nullptr ? sceneryEntry->flags : 0);
        }

        static JSValue height_get(JSContext* ctx, JSValue thisVal)
        {
            auto sceneryEntry = GetLegacyData(thisVal);
            return JS_NewUint32(ctx, sceneryEntry != nullptr ? sceneryEntry->height : 0);
        }

        static JSValue price_get(JSContext* ctx, JSValue thisVal)
        {
            auto sceneryEntry = GetLegacyData(thisVal);
            return JS_NewInt64(ctx, sceneryEntry != nullptr ? sceneryEntry->price : 0);
        }

        static JSValue removalPrice_get(JSContext* ctx, JSValue thisVal)
        {
            auto sceneryEntry = GetLegacyData(thisVal);
            return JS_NewInt64(ctx, sceneryEntry != nullptr ? sceneryEntry->removal_price : 0);
        }

    protected:
        static SmallSceneryEntry* GetLegacyData(JSValue thisVal)
        {
            auto obj = GetSmallSceneryObject(thisVal);
            if (obj != nullptr)
            {
                return static_cast<SmallSceneryEntry*>(obj->GetLegacyData());
            }
            return nullptr;
        }

        static SmallSceneryObject* GetSmallSceneryObject(JSValue thisVal)
        {
            return static_cast<SmallSceneryObject*>(GetObject(thisVal));
        }
    };

    class ScLargeSceneryObjectTile;
    extern ScLargeSceneryObjectTile gScLargeSceneryObjectTile;

    class ScLargeSceneryObjectTile final : public ScBase
    {
    private:
        struct TileData
        {
            LargeSceneryTile _tile{};
        };

    public:
        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "LargeSceneryObjectTile", Finalize);
        }

        JSValue New(JSContext* ctx, const LargeSceneryTile& tile)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("offset", ScLargeSceneryObjectTile::offset_get, nullptr),
                JS_CGETSET_DEF("zClearance", ScLargeSceneryObjectTile::zClearance_get, nullptr),
                JS_CGETSET_DEF("hasSupports", ScLargeSceneryObjectTile::hasSupports_get, nullptr),
                JS_CGETSET_DEF("allowSupportsAbove", ScLargeSceneryObjectTile::allowSupportsAbove_get, nullptr),
                JS_CGETSET_DEF("corners", ScLargeSceneryObjectTile::corners_get, nullptr),
                JS_CGETSET_DEF("walls", ScLargeSceneryObjectTile::walls_get, nullptr),
            };
            return MakeWithOpaque(ctx, funcs, new TileData{ tile });
        }

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal)
        {
            TileData* data = GetObjectData(thisVal);
            if (data)
                delete data;
        }

        static JSValue offset_get(JSContext* ctx, JSValue thisVal)
        {
            TileData* data = GetObjectData(thisVal);
            return ToJSValue(ctx, data->_tile.offset);
        }

        static JSValue zClearance_get(JSContext* ctx, JSValue thisVal)
        {
            TileData* data = GetObjectData(thisVal);
            return JS_NewInt32(ctx, data->_tile.zClearance);
        }

        static JSValue hasSupports_get(JSContext* ctx, JSValue thisVal)
        {
            TileData* data = GetObjectData(thisVal);
            return JS_NewBool(ctx, data->_tile.hasSupports);
        }

        static JSValue allowSupportsAbove_get(JSContext* ctx, JSValue thisVal)
        {
            TileData* data = GetObjectData(thisVal);
            return JS_NewBool(ctx, data->_tile.allowSupportsAbove);
        }

        static JSValue corners_get(JSContext* ctx, JSValue thisVal)
        {
            TileData* data = GetObjectData(thisVal);
            return JS_NewUint32(ctx, data->_tile.corners);
        }

        static JSValue walls_get(JSContext* ctx, JSValue thisVal)
        {
            TileData* data = GetObjectData(thisVal);
            return JS_NewUint32(ctx, data->_tile.walls);
        }

        static TileData* GetObjectData(JSValue thisVal)
        {
            return gScLargeSceneryObjectTile.GetOpaque<TileData*>(thisVal);
        }
    };

    class ScLargeSceneryObject final : public ScSceneryObject
    {
    public:
        static JSValue New(JSContext* ctx, ObjectType type, int32_t index)
        {
            JSValue obj = ScSceneryObject::New(ctx, type, index);
            AddFuncs(ctx, obj);
            return obj;
        }

    private:
        static void AddFuncs(JSContext* ctx, JSValue obj)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("tiles", ScLargeSceneryObject::tiles_get, nullptr),
            };
            JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
        }

        static JSValue tiles_get(JSContext* ctx, JSValue thisVal)
        {
            JSValue result = JS_NewArray(ctx);
            auto entry = GetEntry(thisVal);
            if (entry != nullptr)
            {
                int64_t index = 0;
                for (auto& tile : entry->tiles)
                {
                    JS_SetPropertyInt64(ctx, result, index++, gScLargeSceneryObjectTile.New(ctx, tile));
                }
            }
            return result;
        }

        static LargeSceneryObject* GetLargeSceneryObject(JSValue thisVal)
        {
            return static_cast<LargeSceneryObject*>(GetObject(thisVal));
        }

        static const LargeSceneryEntry* GetEntry(JSValue thisVal)
        {
            auto obj = GetLargeSceneryObject(thisVal);
            if (obj != nullptr)
            {
                return static_cast<LargeSceneryEntry*>(obj->GetLegacyData());
            }
            return nullptr;
        }
    };

    class ScWallObject final : public ScSceneryObject
    {
    public:
        static JSValue New(JSContext* ctx, ObjectType type, int32_t index)
        {
            return ScSceneryObject::New(ctx, type, index);
        }
    };

    class ScFootpathAdditionObject final : public ScSceneryObject
    {
    public:
        static JSValue New(JSContext* ctx, ObjectType type, int32_t index)
        {
            return ScSceneryObject::New(ctx, type, index);
        }
    };

    class ScBannerObject final : public ScSceneryObject
    {
    public:
        static JSValue New(JSContext* ctx, ObjectType type, int32_t index)
        {
            return ScSceneryObject::New(ctx, type, index);
        }
    };

    class ScSceneryGroupObject final : public ScObject
    {
    public:
        static JSValue New(JSContext* ctx, ObjectType type, int32_t index)
        {
            JSValue obj = ScObject::New(ctx, type, index);
            AddFuncs(ctx, obj);
            return obj;
        }

    private:
        static void AddFuncs(JSContext* ctx, JSValue obj)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("items", ScSceneryGroupObject::items_get, nullptr),
            };
            JS_SetPropertyFunctionList(ctx, obj, funcs, std::size(funcs));
        }

        static JSValue items_get(JSContext* ctx, JSValue thisVal)
        {
            JSValue result = JS_NewArray(ctx);
            auto obj = GetSceneryGroupObject(thisVal);
            if (obj != nullptr)
            {
                auto& items = obj->GetItems();
                int64_t index = 0;
                for (const auto& item : items)
                {
                    JS_SetPropertyInt64(ctx, result, index++, JSFromStdString(ctx, item.ToString()));
                }
            }
            return result;
        }

        static SceneryGroupObject* GetSceneryGroupObject(JSValue thisVal)
        {
            return static_cast<SceneryGroupObject*>(GetObject(thisVal));
        }
    };
} // namespace OpenRCT2::Scripting

#endif
