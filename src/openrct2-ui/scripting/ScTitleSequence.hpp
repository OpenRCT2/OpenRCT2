/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
    #include <openrct2/core/String.hpp>
    #include <openrct2/entity/EntityRegistry.h>
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

    static const DukEnumMap<TitleScript> TitleScriptMap({
        { OpenRCT2::Title::LoadParkCommand::ScriptingName, TitleScript::Load },
        { OpenRCT2::Title::SetLocationCommand::ScriptingName, TitleScript::Location },
        { OpenRCT2::Title::RotateViewCommand::ScriptingName, TitleScript::Rotate },
        { OpenRCT2::Title::SetZoomCommand::ScriptingName, TitleScript::Zoom },
        { OpenRCT2::Title::FollowEntityCommand::ScriptingName, TitleScript::Follow },
        { OpenRCT2::Title::SetSpeedCommand::ScriptingName, TitleScript::Speed },
        { OpenRCT2::Title::WaitCommand::ScriptingName, TitleScript::Wait },
        { OpenRCT2::Title::LoadScenarioCommand::ScriptingName, TitleScript::LoadSc },
        { OpenRCT2::Title::RestartCommand::ScriptingName, TitleScript::Restart },
        { OpenRCT2::Title::EndCommand::ScriptingName, TitleScript::End },
    });

    template<>
    DukValue ToDuk(duk_context* ctx, const TitleScript& value)
    {
        return ToDuk(ctx, TitleScriptMap[value]);
    }

    template<>
    DukValue ToDuk(duk_context* ctx, const OpenRCT2::Title::TitleCommand& value)
    {
        using namespace OpenRCT2::Title;
        DukObject obj(ctx);
        std::visit(
            [&obj](auto&& command) {
                using T = std::decay_t<decltype(command)>;
                obj.Set("type", T::ScriptingName);
                if constexpr (std::is_same_v<T, LoadParkCommand>)
                {
                    obj.Set("index", command.SaveIndex);
                }
                else if constexpr (std::is_same_v<T, SetLocationCommand>)
                {
                    obj.Set("x", command.Location.X);
                    obj.Set("y", command.Location.Y);
                }
                else if constexpr (std::is_same_v<T, RotateViewCommand>)
                {
                    obj.Set("rotations", command.Rotations);
                }
                else if constexpr (std::is_same_v<T, SetZoomCommand>)
                {
                    obj.Set("zoom", command.Zoom);
                }
                else if constexpr (std::is_same_v<T, FollowEntityCommand>)
                {
                    if (command.Follow.SpriteIndex.IsNull())
                        obj.Set("id", nullptr);
                    else
                        obj.Set("id", command.Follow.SpriteIndex.ToUnderlying());
                }
                else if constexpr (std::is_same_v<T, SetSpeedCommand>)
                {
                    obj.Set("speed", command.Speed);
                }
                else if constexpr (std::is_same_v<T, WaitCommand>)
                {
                    obj.Set("duration", command.Milliseconds);
                }
                else if constexpr (std::is_same_v<T, LoadScenarioCommand>)
                {
                    obj.Set("scenario", String::toStringView(command.Scenario, sizeof(command.Scenario)));
                }
            },
            value);
        return obj.Take();
    }

    template<>
    TitleScript FromDuk(const DukValue& value)
    {
        if (value.type() == DukValue::Type::STRING)
            return TitleScriptMap[value.as_string()];
        throw DukException() << "Invalid title command id";
    }

    template<>
    OpenRCT2::Title::TitleCommand FromDuk(const DukValue& value)
    {
        using namespace OpenRCT2::Title;
        auto type = FromDuk<TitleScript>(value["type"]);
        TitleCommand command{};
        switch (type)
        {
            case TitleScript::Load:
                command = LoadParkCommand{ static_cast<uint8_t>(value["index"].as_uint()) };
                break;
            case TitleScript::Location:
                command = SetLocationCommand{
                    static_cast<uint8_t>(value["x"].as_uint()),
                    static_cast<uint8_t>(value["y"].as_uint()),
                };
                break;
            case TitleScript::Rotate:
                command = RotateViewCommand{ static_cast<uint8_t>(value["rotations"].as_uint()) };
                break;
            case TitleScript::Zoom:
                command = SetZoomCommand{ static_cast<uint8_t>(value["zoom"].as_uint()) };
                break;
            case TitleScript::Follow:
            {
                auto dukId = value["id"];
                if (dukId.type() == DukValue::Type::NUMBER)
                {
                    command = FollowEntityCommand{ EntityId::FromUnderlying(dukId.as_uint()) };
                }
                else
                {
                    command = FollowEntityCommand{ EntityId::GetNull() };
                }
                break;
            }
            case TitleScript::Speed:
                command = SetSpeedCommand{ static_cast<uint8_t>(value["speed"].as_uint()) };
                break;
            case TitleScript::Wait:
                command = WaitCommand{ static_cast<uint16_t>(value["duration"].as_uint()) };
                break;
            case TitleScript::LoadSc:
            {
                auto loadScenarioCommand = LoadScenarioCommand{};
                String::set(
                    loadScenarioCommand.Scenario, sizeof(loadScenarioCommand.Scenario), value["scenario"].as_c_string());
                command = loadScenarioCommand;
                break;
            }
            case TitleScript::Restart:
                command = RestartCommand{};
                break;
            case TitleScript::End:
                command = EndCommand{};
                break;
            default:
                break;
        }
        return command;
    }

    class ScTitleSequencePark
    {
    private:
        std::string _titleSequencePath;
        std::string _fileName;

    public:
        ScTitleSequencePark(std::string_view path, std::string_view fileName)
            : _titleSequencePath(path)
            , _fileName(fileName)
        {
        }

    private:
        std::string fileName_get() const
        {
            return _fileName;
        }

        void fileName_set(const std::string& value)
        {
            if (value == _fileName)
                return;

            auto seq = OpenRCT2::Title::LoadTitleSequence(_titleSequencePath);
            if (seq != nullptr)
            {
                // Check if name already in use
                auto index = GetIndex(*seq, value);
                if (!index)
                {
                    index = GetIndex(*seq, _fileName);
                    if (index)
                    {
                        TitleSequenceRenamePark(*seq, *index, value.c_str());
                        TitleSequenceSave(*seq);
                    }
                }
            }
        }

        void delete_()
        {
            auto seq = OpenRCT2::Title::LoadTitleSequence(_titleSequencePath);
            if (seq != nullptr)
            {
                auto index = GetIndex(*seq, _fileName);
                if (index)
                {
                    OpenRCT2::Title::TitleSequenceRemovePark(*seq, *index);
                    OpenRCT2::Title::TitleSequenceSave(*seq);
                }
            }
        }

        void load()
        {
            auto seq = OpenRCT2::Title::LoadTitleSequence(_titleSequencePath);
            if (seq != nullptr)
            {
                auto index = GetIndex(*seq, _fileName);
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
                        auto& gameState = GetGameState();
                        parkImporter->Import(gameState);

                        auto old = gLoadKeepWindowsOpen;

                        // Unless we are already in the game, we have to re-create the windows
                        // so that the game toolbars are created.
                        if (gScreenFlags == SCREEN_FLAGS_PLAYING)
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
                        auto ctx = GetContext()->GetScriptEngine().GetContext();
                        duk_error(ctx, DUK_ERR_ERROR, "Unable to load park.");
                    }
                }
            }
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScTitleSequencePark::fileName_get, &ScTitleSequencePark::fileName_set, "fileName");
            dukglue_register_method(ctx, &ScTitleSequencePark::delete_, "delete");
            dukglue_register_method(ctx, &ScTitleSequencePark::load, "load");
        }

    private:
        static std::optional<size_t> GetIndex(const OpenRCT2::Title::TitleSequence& seq, const std::string_view needle)
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

    class ScTitleSequence
    {
    private:
        std::string _path;

    public:
        ScTitleSequence(const std::string& path)
        {
            _path = path;
        }

    private:
        std::string name_get() const
        {
            const auto* item = GetItem();
            if (item != nullptr)
            {
                return item->Name;
            }
            return {};
        }

        void name_set(const std::string& value)
        {
            auto index = GetManagerIndex();
            if (index)
            {
                auto newIndex = TitleSequenceManager::RenameItem(*index, value.c_str());

                // Update path to new value
                auto newItem = TitleSequenceManager::GetItem(newIndex);
                _path = newItem != nullptr ? newItem->Path : std::string();
            }
        }

        std::string path_get() const
        {
            const auto* item = GetItem();
            if (item != nullptr)
            {
                return item->Path;
            }
            return {};
        }

        bool isDirectory_get() const
        {
            const auto* item = GetItem();
            if (item != nullptr)
            {
                return !item->IsZip;
            }
            return {};
        }

        bool isReadOnly_get() const
        {
            const auto* item = GetItem();
            if (item != nullptr)
            {
                return item->PredefinedIndex != TitleSequenceManager::kPredefinedIndexCustom;
            }
            return {};
        }

        std::vector<std::shared_ptr<ScTitleSequencePark>> parks_get() const
        {
            std::vector<std::shared_ptr<ScTitleSequencePark>> result;
            auto titleSeq = OpenRCT2::Title::LoadTitleSequence(_path);
            if (titleSeq != nullptr)
            {
                for (size_t i = 0; i < titleSeq->Saves.size(); i++)
                {
                    result.push_back(std::make_shared<ScTitleSequencePark>(_path, titleSeq->Saves[i]));
                }
            }
            return result;
        }

        std::vector<DukValue> commands_get() const
        {
            auto& scriptEngine = GetContext()->GetScriptEngine();
            auto ctx = scriptEngine.GetContext();

            std::vector<DukValue> result;
            auto titleSeq = OpenRCT2::Title::LoadTitleSequence(_path);
            if (titleSeq != nullptr)
            {
                for (const auto& command : titleSeq->Commands)
                {
                    result.push_back(ToDuk(ctx, command));
                }
            }
            return result;
        }

        void commands_set(const std::vector<DukValue>& value)
        {
            std::vector<OpenRCT2::Title::TitleCommand> commands;
            for (const auto& v : value)
            {
                auto command = FromDuk<OpenRCT2::Title::TitleCommand>(v);
                commands.push_back(std::move(command));
            }

            auto titleSeq = OpenRCT2::Title::LoadTitleSequence(_path);
            titleSeq->Commands = commands;
            OpenRCT2::Title::TitleSequenceSave(*titleSeq);
        }

        void addPark(const std::string& path, const std::string& fileName)
        {
            auto titleSeq = OpenRCT2::Title::LoadTitleSequence(_path);
            OpenRCT2::Title::TitleSequenceAddPark(*titleSeq, path.c_str(), fileName.c_str());
            OpenRCT2::Title::TitleSequenceSave(*titleSeq);
        }

        std::shared_ptr<ScTitleSequence> clone(const std::string& name) const
        {
            auto copyIndex = GetManagerIndex();
            if (copyIndex)
            {
                auto index = TitleSequenceManager::DuplicateItem(*copyIndex, name.c_str());
                auto* item = TitleSequenceManager::GetItem(index);
                if (item != nullptr)
                {
                    return std::make_shared<ScTitleSequence>(item->Path);
                }
            }
            return nullptr;
        }

        void delete_()
        {
            auto index = GetManagerIndex();
            if (index)
            {
                TitleSequenceManager::DeleteItem(*index);
            }
            _path = {};
        }

        bool isPlaying_get() const
        {
            auto index = GetManagerIndex();
            return index && TitleIsPreviewingSequence() && *index == TitleGetCurrentSequence();
        }

        DukValue position_get() const
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            if (isPlaying_get())
            {
                auto* player = static_cast<ITitleSequencePlayer*>(TitleGetSequencePlayer());
                if (player != nullptr)
                {
                    return ToDuk(ctx, player->GetCurrentPosition());
                }
            }
            return ToDuk(ctx, nullptr);
        }

        void play()
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            auto index = GetManagerIndex();
            if (index && (!TitleIsPreviewingSequence() || *index != TitleGetCurrentSequence()))
            {
                if (!TitlePreviewSequence(*index))
                {
                    duk_error(ctx, DUK_ERR_ERROR, "Failed to load title sequence");
                }
                else if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO))
                {
                    gPreviewingTitleSequenceInGame = true;
                }
            }
        }

        void seek(int32_t position)
        {
            auto ctx = GetContext()->GetScriptEngine().GetContext();
            if (isPlaying_get())
            {
                auto* player = static_cast<ITitleSequencePlayer*>(TitleGetSequencePlayer());
                try
                {
                    player->Seek(position);
                    player->Update();
                }
                catch (...)
                {
                    duk_error(ctx, DUK_ERR_ERROR, "Failed to seek");
                }
            }
        }

        void stop()
        {
            if (isPlaying_get())
            {
                TitleStopPreviewingSequence();
            }
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScTitleSequence::name_get, &ScTitleSequence::name_set, "name");
            dukglue_register_property(ctx, &ScTitleSequence::path_get, nullptr, "path");
            dukglue_register_property(ctx, &ScTitleSequence::isDirectory_get, nullptr, "isDirectory");
            dukglue_register_property(ctx, &ScTitleSequence::isReadOnly_get, nullptr, "isReadOnly");
            dukglue_register_property(ctx, &ScTitleSequence::parks_get, nullptr, "parks");
            dukglue_register_property(ctx, &ScTitleSequence::commands_get, &ScTitleSequence::commands_set, "commands");
            dukglue_register_property(ctx, &ScTitleSequence::isPlaying_get, nullptr, "isPlaying");
            dukglue_register_property(ctx, &ScTitleSequence::position_get, nullptr, "position");
            dukglue_register_method(ctx, &ScTitleSequence::addPark, "addPark");
            dukglue_register_method(ctx, &ScTitleSequence::clone, "clone");
            dukglue_register_method(ctx, &ScTitleSequence::delete_, "delete");

            dukglue_register_method(ctx, &ScTitleSequence::play, "play");
            dukglue_register_method(ctx, &ScTitleSequence::seek, "seek");
            dukglue_register_method(ctx, &ScTitleSequence::stop, "stop");
        }

    private:
        std::optional<size_t> GetManagerIndex() const
        {
            auto count = TitleSequenceManager::GetCount();
            for (size_t i = 0; i < count; i++)
            {
                auto item = TitleSequenceManager::GetItem(i);
                if (item != nullptr && item->Path == _path)
                {
                    return i;
                }
            }
            return std::nullopt;
        }

        const TitleSequenceManager::Item* GetItem() const
        {
            auto index = GetManagerIndex();
            if (index)
            {
                return TitleSequenceManager::GetItem(*index);
            }
            return nullptr;
        }
    };

    class ScTitleSequenceManager
    {
    private:
        std::vector<std::shared_ptr<ScTitleSequence>> titleSequences_get() const
        {
            std::vector<std::shared_ptr<ScTitleSequence>> result;
            auto count = TitleSequenceManager::GetCount();
            for (size_t i = 0; i < count; i++)
            {
                const auto& path = TitleSequenceManager::GetItem(i)->Path;
                result.push_back(std::make_shared<ScTitleSequence>(path));
            }
            return result;
        }

        std::shared_ptr<ScTitleSequence> create(const std::string& name)
        {
            auto index = TitleSequenceManager::CreateItem(name.c_str());
            auto* item = TitleSequenceManager::GetItem(index);
            if (item != nullptr)
            {
                return std::make_shared<ScTitleSequence>(item->Path);
            }
            return nullptr;
        }

    public:
        static void Register(duk_context* ctx)
        {
            dukglue_register_property(ctx, &ScTitleSequenceManager::titleSequences_get, nullptr, "titleSequences");
            dukglue_register_method(ctx, &ScTitleSequenceManager::create, "create");
        }
    };
} // namespace OpenRCT2::Scripting

#endif
