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

    #include <memory>
    #include <openrct2/Context.h>
    #include <openrct2/Game.h>
    #include <openrct2/GameState.h>
    #include <openrct2/OpenRCT2.h>
    #include <openrct2/ParkImporter.h>
    #include <openrct2/core/EnumMap.hpp>
    #include <openrct2/core/String.hpp>
    #include <openrct2/object/ObjectManager.h>
    #include <openrct2/scenario/Scenario.h>
    #include <openrct2/scenes/title/TitleScene.h>
    #include <openrct2/scenes/title/TitleSequence.h>
    #include <openrct2/scenes/title/TitleSequenceManager.h>
    #include <openrct2/scenes/title/TitleSequencePlayer.h>
    #include <openrct2/scripting/ScriptEngine.h>
    #include <type_traits>
    #include <variant>

namespace OpenRCT2::Scripting
{
    enum class TitleScript : uint8_t
    {
        Undefined = 0xFF,
        Wait = 0,
        Location,
        Rotate,
        Zoom,
        Follow,
        Restart,
        Load,
        End,
        Speed,
        Loop,
        EndLoop,
        LoadSc,
    };

    static const EnumMap<TitleScript> TitleScriptMap(
        {
            { Title::LoadParkCommand::ScriptingName, TitleScript::Load },
            { Title::SetLocationCommand::ScriptingName, TitleScript::Location },
            { Title::RotateViewCommand::ScriptingName, TitleScript::Rotate },
            { Title::SetZoomCommand::ScriptingName, TitleScript::Zoom },
            { Title::FollowEntityCommand::ScriptingName, TitleScript::Follow },
            { Title::SetSpeedCommand::ScriptingName, TitleScript::Speed },
            { Title::WaitCommand::ScriptingName, TitleScript::Wait },
            { Title::LoadScenarioCommand::ScriptingName, TitleScript::LoadSc },
            { Title::RestartCommand::ScriptingName, TitleScript::Restart },
            { Title::EndCommand::ScriptingName, TitleScript::End },
        });

    inline JSValue TitleScriptToJS(JSContext* ctx, TitleScript value)
    {
        return JSFromStdString(ctx, TitleScriptMap[value]);
    }

    inline JSValue TitleCommandToJS(JSContext* ctx, const Title::TitleCommand& value)
    {
        using namespace OpenRCT2::Title;
        JSValue obj = JS_NewObject(ctx);
        std::visit(
            [ctx, &obj](auto&& command) {
                using T = std::decay_t<decltype(command)>;
                JS_SetPropertyStr(ctx, obj, "type", JSFromStdString(ctx, T::ScriptingName));
                if constexpr (std::is_same_v<T, LoadParkCommand>)
                {
                    JS_SetPropertyStr(ctx, obj, "index", JS_NewInt32(ctx, command.SaveIndex));
                }
                else if constexpr (std::is_same_v<T, SetLocationCommand>)
                {
                    JS_SetPropertyStr(ctx, obj, "x", JS_NewInt32(ctx, command.Location.X));
                    JS_SetPropertyStr(ctx, obj, "y", JS_NewInt32(ctx, command.Location.Y));
                }
                else if constexpr (std::is_same_v<T, RotateViewCommand>)
                {
                    JS_SetPropertyStr(ctx, obj, "rotations", JS_NewInt32(ctx, command.Rotations));
                }
                else if constexpr (std::is_same_v<T, SetZoomCommand>)
                {
                    JS_SetPropertyStr(ctx, obj, "zoom", JS_NewInt32(ctx, command.Zoom));
                }
                else if constexpr (std::is_same_v<T, FollowEntityCommand>)
                {
                    if (command.Follow.SpriteIndex.IsNull())
                        JS_SetPropertyStr(ctx, obj, "id", JS_NULL);
                    else
                        JS_SetPropertyStr(ctx, obj, "id", JS_NewInt32(ctx, command.Follow.SpriteIndex.ToUnderlying()));
                }
                else if constexpr (std::is_same_v<T, SetSpeedCommand>)
                {
                    JS_SetPropertyStr(ctx, obj, "speed", JS_NewInt32(ctx, command.Speed));
                }
                else if constexpr (std::is_same_v<T, WaitCommand>)
                {
                    JS_SetPropertyStr(ctx, obj, "duration", JS_NewInt32(ctx, command.Milliseconds));
                }
                else if constexpr (std::is_same_v<T, LoadScenarioCommand>)
                {
                    JS_SetPropertyStr(ctx, obj, "scenario", JSFromStdString(ctx, command.Scenario));
                }
            },
            value);
        return obj;
    }

    inline std::optional<Title::TitleCommand> TitleCommandFromJS(JSContext* ctx, JSValue value)
    {
        using namespace OpenRCT2::Title;
        auto type = TitleScriptMap.TryGet(JSToStdString(ctx, value, "type"));
        if (!type.has_value())
            return std::nullopt;

        switch (type.value())
        {
            case TitleScript::Load:
            {
                auto index = JSToOptionalInt(ctx, value, "index");
                if (!index.has_value())
                    return std::nullopt;

                return LoadParkCommand{ static_cast<uint8_t>(index.value()) };
            }
            case TitleScript::Location:
            {
                auto x = JSToOptionalInt(ctx, value, "x");
                auto y = JSToOptionalInt(ctx, value, "y");
                if (!x.has_value() || !y.has_value())
                    return std::nullopt;

                return SetLocationCommand{
                    static_cast<uint8_t>(x.value()),
                    static_cast<uint8_t>(y.value()),
                };
            }
            case TitleScript::Rotate:
            {
                auto rotations = JSToOptionalInt(ctx, value, "rotations");
                if (!rotations.has_value())
                    return std::nullopt;

                return RotateViewCommand{ static_cast<uint8_t>(rotations.value()) };
            }
            case TitleScript::Zoom:
            {
                auto zoom = JSToOptionalInt(ctx, value, "zoom");
                if (!zoom.has_value())
                    return std::nullopt;

                return SetZoomCommand{ static_cast<uint8_t>(zoom.value()) };
            }
            case TitleScript::Follow:
                if (auto id = JSToOptionalInt(ctx, value, "id"); id.has_value())
                {
                    return FollowEntityCommand{ EntityId::FromUnderlying(id.value()) };
                }
                else
                {
                    return FollowEntityCommand{ EntityId::GetNull() };
                }
            case TitleScript::Speed:
            {
                auto speed = JSToOptionalInt(ctx, value, "speed");
                if (!speed.has_value())
                    return std::nullopt;

                return SetSpeedCommand{ static_cast<uint8_t>(speed.value()) };
            }
            case TitleScript::Wait:
            {
                auto duration = JSToOptionalInt(ctx, value, "duration");
                if (!duration.has_value())
                    return std::nullopt;

                return WaitCommand{ static_cast<uint16_t>(duration.value()) };
            }
            case TitleScript::LoadSc:
            {
                auto scenario = JSToOptionalStdString(ctx, value, "scenario");
                if (!scenario.has_value())
                    return std::nullopt;

                auto loadScenarioCommand = LoadScenarioCommand{};
                String::set(loadScenarioCommand.Scenario, sizeof(loadScenarioCommand.Scenario), scenario.value().c_str());
                return loadScenarioCommand;
            }
            case TitleScript::Restart:
                return RestartCommand{};
            case TitleScript::End:
                return EndCommand{};
            default:
                return std::nullopt;
        }
    }

    class ScTitleSequencePark;
    extern ScTitleSequencePark gScTitleSequencePark;
    class ScTitleSequencePark : public ScBase
    {
        struct OpaqueData
        {
            std::string _titleSequencePath{};
            std::string _fileName{};
        };

        static JSValue fileName_get(JSContext* ctx, JSValue thisVal)
        {
            OpaqueData* data = gScTitleSequencePark.GetOpaque<OpaqueData*>(thisVal);
            if (!data)
                return JS_UNDEFINED;
            return JSFromStdString(ctx, data->_fileName);
        }

        static JSValue fileName_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_STR(valueStr, ctx, value);
            OpaqueData* data = gScTitleSequencePark.GetOpaque<OpaqueData*>(thisVal);
            if (!data)
                return JS_UNDEFINED;

            if (valueStr == data->_fileName)
                return JS_UNDEFINED;

            auto seq = Title::LoadTitleSequence(data->_titleSequencePath);
            if (seq != nullptr)
            {
                // Check if name already in use
                auto index = GetIndex(*seq, valueStr);
                if (!index)
                {
                    index = GetIndex(*seq, data->_fileName);
                    if (index)
                    {
                        TitleSequenceRenamePark(*seq, *index, valueStr.c_str());
                        TitleSequenceSave(*seq);
                    }
                }
            }
            return JS_UNDEFINED;
        }

        static JSValue delete_(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            OpaqueData* data = gScTitleSequencePark.GetOpaque<OpaqueData*>(thisVal);
            if (!data)
                return JS_UNDEFINED;

            auto seq = Title::LoadTitleSequence(data->_titleSequencePath);
            if (seq != nullptr)
            {
                auto index = GetIndex(*seq, data->_fileName);
                if (index)
                {
                    OpenRCT2::Title::TitleSequenceRemovePark(*seq, *index);
                    OpenRCT2::Title::TitleSequenceSave(*seq);
                }
            }
            return JS_UNDEFINED;
        }

        static JSValue load(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            OpaqueData* data = gScTitleSequencePark.GetOpaque<OpaqueData*>(thisVal);
            if (!data)
                return JS_UNDEFINED;

            auto seq = Title::LoadTitleSequence(data->_titleSequencePath);
            if (seq != nullptr)
            {
                auto index = GetIndex(*seq, data->_fileName);
                if (index)
                {
                    auto handle = OpenRCT2::Title::TitleSequenceGetParkHandle(*seq, *index);
                    auto isScenario = ParkImporter::ExtensionIsScenario(handle->HintPath);
                    try
                    {
                        auto& objectMgr = GetContext()->GetObjectManager();
                        auto parkImporter = ParkImporter::Create(handle->HintPath);
                        auto result = parkImporter->LoadFromStream(handle->Stream.get(), isScenario);
                        objectMgr.LoadObjects(result.RequiredObjects);

                        // TODO: Have a separate GameState and exchange once loaded.
                        auto& gameState = getGameState();
                        parkImporter->Import(gameState);

                        auto old = gLoadKeepWindowsOpen;

                        // Unless we are already in the game, we have to re-create the windows
                        // so that the game toolbars are created.
                        if (gLegacyScene == LegacyScene::playing)
                        {
                            gLoadKeepWindowsOpen = true;
                        }

                        if (isScenario)
                            ScenarioBegin(gameState);
                        else
                            GameLoadInit();
                        gLoadKeepWindowsOpen = old;
                    }
                    catch (const std::exception&)
                    {
                        JS_ThrowPlainError(ctx, "Unable to load park.");
                        return JS_EXCEPTION;
                    }
                }
            }
            return JS_UNDEFINED;
        }

    public:
        JSValue New(JSContext* ctx, std::string_view path, std::string_view fileName)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("fileName", ScTitleSequencePark::fileName_get, ScTitleSequencePark::fileName_set),
                JS_CFUNC_DEF("delete", 0, ScTitleSequencePark::delete_),
                JS_CFUNC_DEF("load", 0, ScTitleSequencePark::load),
            };

            return MakeWithOpaque(ctx, funcs, new OpaqueData{ std::string(path), std::string(fileName) });
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "TitleSequencePark", Finalize);
        }

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal)
        {
            OpaqueData* data = gScTitleSequencePark.GetOpaque<OpaqueData*>(thisVal);
            if (data)
                delete data;
        }

        static std::optional<size_t> GetIndex(const Title::TitleSequence& seq, const std::string_view needle)
        {
            for (size_t i = 0; i < seq.Saves.size(); i++)
            {
                if (seq.Saves[i] == needle)
                {
                    return i;
                }
            }
            return std::nullopt;
        }
    };

    class ScTitleSequence;
    extern ScTitleSequence gScTitleSequence;
    class ScTitleSequence : public ScBase
    {
        struct OpaqueData
        {
            std::string _path;
        };

        static JSValue name_get(JSContext* ctx, JSValue thisVal)
        {
            const auto* item = GetItem(ctx, thisVal);
            if (item != nullptr)
            {
                return JSFromStdString(ctx, item->Name);
            }
            return JSFromStdString(ctx, {});
        }

        static JSValue name_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_STR(valueStr, ctx, value)
            OpaqueData* data = gScTitleSequence.GetOpaque<OpaqueData*>(thisVal);
            if (!data)
                return JS_UNDEFINED;

            auto index = GetManagerIndex(ctx, thisVal);
            if (index)
            {
                auto newIndex = TitleSequenceManager::RenameItem(*index, valueStr.c_str());

                // Update path to new value
                auto newItem = TitleSequenceManager::GetItem(newIndex);
                data->_path = newItem != nullptr ? newItem->Path : std::string();
            }
            return JS_UNDEFINED;
        }

        static JSValue path_get(JSContext* ctx, JSValue thisVal)
        {
            const auto* item = GetItem(ctx, thisVal);
            if (item != nullptr)
            {
                return JSFromStdString(ctx, item->Path);
            }
            return JSFromStdString(ctx, {});
        }

        static JSValue isDirectory_get(JSContext* ctx, JSValue thisVal)
        {
            const auto* item = GetItem(ctx, thisVal);
            if (item != nullptr)
            {
                return JS_NewBool(ctx, !item->IsZip);
            }
            return JS_NewBool(ctx, {});
        }

        static JSValue isReadOnly_get(JSContext* ctx, JSValue thisVal)
        {
            const auto* item = GetItem(ctx, thisVal);
            if (item != nullptr)
            {
                return JS_NewBool(ctx, item->PredefinedIndex != TitleSequenceManager::kPredefinedIndexCustom);
            }
            return JS_NewBool(ctx, {});
        }

        static JSValue parks_get(JSContext* ctx, JSValue thisVal)
        {
            JSValue result = JS_NewArray(ctx);
            OpaqueData* data = gScTitleSequence.GetOpaque<OpaqueData*>(thisVal);
            if (!data)
                return result;

            auto titleSeq = Title::LoadTitleSequence(data->_path);
            if (titleSeq != nullptr)
            {
                for (size_t i = 0; i < titleSeq->Saves.size(); i++)
                {
                    JS_SetPropertyInt64(ctx, result, i, gScTitleSequencePark.New(ctx, data->_path, titleSeq->Saves[i]));
                }
            }
            return result;
        }

        static JSValue commands_get(JSContext* ctx, JSValue thisVal)
        {
            JSValue result = JS_NewArray(ctx);
            OpaqueData* data = gScTitleSequence.GetOpaque<OpaqueData*>(thisVal);
            if (!data)
                return result;

            auto titleSeq = Title::LoadTitleSequence(data->_path);
            if (titleSeq != nullptr)
            {
                int64_t idx = 0;
                for (const auto& command : titleSeq->Commands)
                {
                    JS_SetPropertyInt64(ctx, result, idx++, TitleCommandToJS(ctx, command));
                }
            }
            return result;
        }

        static JSValue commands_set(JSContext* ctx, JSValue thisVal, JSValue value)
        {
            JS_UNPACK_ARRAY(array, ctx, value);
            OpaqueData* data = gScTitleSequence.GetOpaque<OpaqueData*>(thisVal);
            if (!data)
                return JS_UNDEFINED;

            int64_t length;
            if (JS_GetLength(ctx, array, &length) != 0)
                return JS_UNDEFINED;
            std::vector<Title::TitleCommand> commands;
            commands.reserve(length);

            for (int64_t i = 0; i < length; i++)
            {
                JSValue element = JS_GetPropertyInt64(ctx, array, i);
                auto command = TitleCommandFromJS(ctx, element);
                JS_FreeValue(ctx, element);
                if (!command.has_value())
                {
                    JS_ThrowPlainError(ctx, "Invalid command");
                    return JS_EXCEPTION;
                }
                commands.push_back(command.value());
            }

            auto titleSeq = Title::LoadTitleSequence(data->_path);
            titleSeq->Commands = commands;
            OpenRCT2::Title::TitleSequenceSave(*titleSeq);
            return JS_UNDEFINED;
        }

        static JSValue addPark(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(path, ctx, argv[0]);
            JS_UNPACK_STR(fileName, ctx, argv[1]);
            OpaqueData* data = gScTitleSequence.GetOpaque<OpaqueData*>(thisVal);
            if (!data)
                return JS_UNDEFINED;

            auto titleSeq = Title::LoadTitleSequence(data->_path);
            OpenRCT2::Title::TitleSequenceAddPark(*titleSeq, path.c_str(), fileName.c_str());
            OpenRCT2::Title::TitleSequenceSave(*titleSeq);
            return JS_UNDEFINED;
        }

        static JSValue clone(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(name, ctx, argv[0]);

            auto copyIndex = GetManagerIndex(ctx, thisVal);
            if (copyIndex)
            {
                auto index = TitleSequenceManager::DuplicateItem(*copyIndex, name.c_str());
                auto* item = TitleSequenceManager::GetItem(index);
                if (item != nullptr)
                {
                    return gScTitleSequence.New(ctx, item->Path);
                }
            }
            return JS_NULL;
        }

        static JSValue delete_(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            OpaqueData* data = gScTitleSequence.GetOpaque<OpaqueData*>(thisVal);
            if (!data)
                return JS_UNDEFINED;

            auto index = GetManagerIndex(ctx, thisVal);
            if (index)
            {
                TitleSequenceManager::DeleteItem(*index);
            }
            data->_path = {};
            return JS_UNDEFINED;
        }

        static bool IsPlaying(JSContext* ctx, JSValue thisVal)
        {
            auto index = GetManagerIndex(ctx, thisVal);
            return index && TitleIsPreviewingSequence() && *index == TitleGetCurrentSequence();
        }

        static JSValue isPlaying_get(JSContext* ctx, JSValue thisVal)
        {
            return JS_NewBool(ctx, IsPlaying(ctx, thisVal));
        }

        static JSValue position_get(JSContext* ctx, JSValue thisVal)
        {
            if (IsPlaying(ctx, thisVal))
            {
                auto* player = static_cast<ITitleSequencePlayer*>(TitleGetSequencePlayer());
                if (player != nullptr)
                {
                    return JS_NewInt32(ctx, player->GetCurrentPosition());
                }
            }
            return JS_NULL;
        }

        static JSValue play(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            auto index = GetManagerIndex(ctx, thisVal);
            if (index && (!TitleIsPreviewingSequence() || *index != TitleGetCurrentSequence()))
            {
                if (!TitlePreviewSequence(*index))
                {
                    JS_ThrowPlainError(ctx, "Failed to load title sequence");
                    return JS_EXCEPTION;
                }
                else if (gLegacyScene != LegacyScene::titleSequence)
                {
                    gPreviewingTitleSequenceInGame = true;
                }
            }
            return JS_UNDEFINED;
        }

        static JSValue seek(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_INT32(position, ctx, argv[0]);

            if (IsPlaying(ctx, thisVal))
            {
                auto* player = static_cast<ITitleSequencePlayer*>(TitleGetSequencePlayer());
                try
                {
                    player->Seek(position);
                    player->Update();
                }
                catch (...)
                {
                    JS_ThrowPlainError(ctx, "Failed to seek");
                    return JS_EXCEPTION;
                }
            }
            return JS_UNDEFINED;
        }

        static JSValue stop(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            if (IsPlaying(ctx, thisVal))
            {
                TitleStopPreviewingSequence();
            }
            return JS_UNDEFINED;
        }

    public:
        JSValue New(JSContext* ctx, std::string_view path)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("name", ScTitleSequence::name_get, ScTitleSequence::name_set),
                JS_CGETSET_DEF("path", ScTitleSequence::path_get, nullptr),
                JS_CGETSET_DEF("isDirectory", ScTitleSequence::isDirectory_get, nullptr),
                JS_CGETSET_DEF("isReadOnly", ScTitleSequence::isReadOnly_get, nullptr),
                JS_CGETSET_DEF("parks", ScTitleSequence::parks_get, nullptr),
                JS_CGETSET_DEF("commands", ScTitleSequence::commands_get, ScTitleSequence::commands_set),
                JS_CGETSET_DEF("isPlaying", ScTitleSequence::isPlaying_get, nullptr),
                JS_CGETSET_DEF("position", ScTitleSequence::position_get, nullptr),

                JS_CFUNC_DEF("addPark", 2, ScTitleSequence::addPark),
                JS_CFUNC_DEF("clone", 1, ScTitleSequence::clone),
                JS_CFUNC_DEF("delete", 0, ScTitleSequence::delete_),

                JS_CFUNC_DEF("play", 0, ScTitleSequence::play),
                JS_CFUNC_DEF("seek", 1, ScTitleSequence::seek),
                JS_CFUNC_DEF("stop", 0, ScTitleSequence::stop),
            };

            return MakeWithOpaque(ctx, funcs, new OpaqueData{ std::string(path) });
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "TitleSequence", Finalize);
        }

    private:
        static void Finalize(JSRuntime* rt, JSValue thisVal)
        {
            OpaqueData* data = gScTitleSequence.GetOpaque<OpaqueData*>(thisVal);
            if (data)
                delete data;
        }

        static std::optional<size_t> GetManagerIndex(JSContext* ctx, JSValue thisVal)
        {
            OpaqueData* data = gScTitleSequence.GetOpaque<OpaqueData*>(thisVal);
            if (!data)
                return std::nullopt;

            auto count = TitleSequenceManager::GetCount();
            for (size_t i = 0; i < count; i++)
            {
                auto item = TitleSequenceManager::GetItem(i);
                if (item != nullptr && item->Path == data->_path)
                {
                    return i;
                }
            }
            return std::nullopt;
        }

        static const TitleSequenceManager::Item* GetItem(JSContext* ctx, JSValue thisVal)
        {
            auto index = GetManagerIndex(ctx, thisVal);
            if (index)
            {
                return TitleSequenceManager::GetItem(*index);
            }
            return nullptr;
        }
    };

    class ScTitleSequenceManager;
    extern ScTitleSequenceManager gScTitleSequenceManager;
    class ScTitleSequenceManager : public ScBase
    {
        static JSValue titleSequences_get(JSContext* ctx, JSValue thisVal)
        {
            JSValue result = JS_NewArray(ctx);
            auto count = TitleSequenceManager::GetCount();
            for (size_t i = 0; i < count; i++)
            {
                const auto& path = TitleSequenceManager::GetItem(i)->Path;
                JS_SetPropertyInt64(ctx, result, i, gScTitleSequence.New(ctx, path));
            }
            return result;
        }

        static JSValue create(JSContext* ctx, JSValue thisVal, int argc, JSValue* argv)
        {
            JS_UNPACK_STR(name, ctx, argv[0])

            auto index = TitleSequenceManager::CreateItem(name.c_str());
            auto* item = TitleSequenceManager::GetItem(index);
            if (item != nullptr)
            {
                return gScTitleSequence.New(ctx, item->Path);
            }
            return JS_NULL;
        }

    public:
        JSValue New(JSContext* ctx)
        {
            static constexpr JSCFunctionListEntry funcs[] = {
                JS_CGETSET_DEF("titleSequences", ScTitleSequenceManager::titleSequences_get, nullptr),
                JS_CFUNC_DEF("create", 1, ScTitleSequenceManager::create),
            };
            return MakeWithOpaque(ctx, funcs, nullptr);
        }

        void Register(JSContext* ctx)
        {
            RegisterBaseStr(ctx, "TitleSequenceManager");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
