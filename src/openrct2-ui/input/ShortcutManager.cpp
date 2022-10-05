/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ShortcutManager.h"

#include "ShortcutIds.h"

#include <SDL.h>
#include <openrct2/Context.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/core/Console.hpp>
#include <openrct2/core/DataSerialiser.h>
#include <openrct2/core/FileStream.h>
#include <openrct2/core/FileSystem.hpp>
#include <openrct2/core/Json.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/interface/Window.h>
#include <openrct2/localisation/Language.h>

using namespace OpenRCT2::Ui;

std::string_view RegisteredShortcut::GetTopLevelGroup() const
{
    auto fullstopIndex = Id.find('.');
    if (fullstopIndex != std::string::npos)
    {
        return std::string_view(Id.c_str(), fullstopIndex);
    }
    return {};
}

std::string_view RegisteredShortcut::GetGroup() const
{
    auto fullstopIndex = Id.find_last_of('.');
    if (fullstopIndex != std::string::npos)
    {
        return std::string_view(Id.c_str(), fullstopIndex);
    }
    return {};
}

bool RegisteredShortcut::Matches(const InputEvent& e) const
{
    if (IsSuitableInputEvent(e))
    {
        auto result = std::find_if(
            Current.begin(), Current.end(), [e](const ShortcutInput& action) { return action.Matches(e); });
        return result != Current.end();
    }
    return false;
}

bool RegisteredShortcut::IsSuitableInputEvent(const InputEvent& e) const
{
    // Do not intercept button releases
    if (e.State == InputEventState::Release)
    {
        return false;
    }

    if (e.DeviceKind == InputDeviceKind::Mouse)
    {
        // Do not allow LMB or RMB to be shortcut
        if (e.Button == 0 || e.Button == 1)
        {
            return false;
        }
    }
    else if (e.DeviceKind == InputDeviceKind::Keyboard)
    {
        // Do not allow modifier keys alone
        switch (e.Button)
        {
            case SDLK_LCTRL:
            case SDLK_RCTRL:
            case SDLK_LSHIFT:
            case SDLK_RSHIFT:
            case SDLK_LALT:
            case SDLK_RALT:
            case SDLK_LGUI:
            case SDLK_RGUI:
                return false;
        }
    }

    return true;
}

std::string RegisteredShortcut::GetDisplayString() const
{
    std::string result;
    auto numChords = Current.size();
    for (size_t i = 0; i < numChords; i++)
    {
        const auto& kc = Current[i];
        result += kc.ToLocalisedString();
        if (i < numChords - 1)
        {
            // TODO localise...
            result += " or ";
        }
    }
    return result;
}

ShortcutManager::ShortcutManager(const std::shared_ptr<IPlatformEnvironment>& env)
    : _env(env)
{
    RegisterDefaultShortcuts();
}

void ShortcutManager::RegisterShortcut(RegisteredShortcut&& shortcut)
{
    if (!shortcut.Id.empty() && GetShortcut(shortcut.Id) == nullptr)
    {
        auto id = std::make_unique<std::string>(shortcut.Id);
        auto idView = std::string_view(*id);
        _ids.push_back(std::move(id));
        Shortcuts[idView] = shortcut;
    }
}

RegisteredShortcut* ShortcutManager::GetShortcut(std::string_view id)
{
    auto result = Shortcuts.find(id);
    return result == Shortcuts.end() ? nullptr : &result->second;
}

void ShortcutManager::RemoveShortcut(std::string_view id)
{
    Shortcuts.erase(id);
    _ids.erase(
        std::remove_if(_ids.begin(), _ids.end(), [id](const std::unique_ptr<std::string>& x) { return *x == id; }), _ids.end());
}

bool ShortcutManager::IsPendingShortcutChange() const
{
    return !_pendingShortcutChange.empty();
}

void ShortcutManager::SetPendingShortcutChange(std::string_view id)
{
    _pendingShortcutChange = id;
}

void ShortcutManager::ProcessEvent(const InputEvent& e)
{
    if (!IsPendingShortcutChange())
    {
        for (const auto& shortcut : Shortcuts)
        {
            if (shortcut.second.Matches(e))
            {
                shortcut.second.Action();
            }
        }
    }
    else
    {
        auto shortcut = GetShortcut(_pendingShortcutChange);
        if (shortcut != nullptr && shortcut->IsSuitableInputEvent(e))
        {
            auto shortcutInput = ShortcutInput::FromInputEvent(e);
            if (shortcutInput.has_value())
            {
                shortcut->Current.clear();
                shortcut->Current.push_back(std::move(shortcutInput.value()));
            }
            _pendingShortcutChange.clear();
            window_close_by_class(WindowClass::ChangeKeyboardShortcut);
            SaveUserBindings();
        }
    }
}

bool ShortcutManager::ProcessEventForSpecificShortcut(const InputEvent& e, std::string_view id)
{
    auto shortcut = GetShortcut(id);
    if (shortcut != nullptr && shortcut->Matches(e))
    {
        shortcut->Action();
        return true;
    }
    return false;
}

void ShortcutManager::LoadUserBindings()
{
    try
    {
        auto path = fs::u8path(_env->GetFilePath(PATHID::CONFIG_SHORTCUTS));
        if (fs::exists(path))
        {
            LoadUserBindings(path);
        }
        else
        {
            try
            {
                Console::WriteLine("Importing legacy shortcuts...");
                auto legacyPath = fs::u8path(_env->GetFilePath(PATHID::CONFIG_SHORTCUTS_LEGACY));
                if (fs::exists(legacyPath))
                {
                    LoadLegacyBindings(legacyPath);
                    SaveUserBindings();
                    Console::WriteLine("Legacy shortcuts imported");
                }
            }
            catch (const std::exception& e)
            {
                Console::Error::WriteLine("Unable to import legacy shortcut bindings: %s", e.what());
            }
        }
    }
    catch (const std::exception& e)
    {
        Console::Error::WriteLine("Unable to load shortcut bindings: %s", e.what());
    }
}

std::optional<ShortcutInput> ShortcutManager::ConvertLegacyBinding(uint16_t binding)
{
    constexpr uint16_t nullBinding = 0xFFFF;
    constexpr uint16_t shift = 0x100;
    constexpr uint16_t ctrl = 0x200;
    constexpr uint16_t alt = 0x400;
    constexpr uint16_t cmd = 0x800;

    if (binding == nullBinding)
    {
        return std::nullopt;
    }

    ShortcutInput result;
    result.Kind = InputDeviceKind::Keyboard;
    if (binding & shift)
        result.Modifiers |= KMOD_SHIFT;
    if (binding & ctrl)
        result.Modifiers |= KMOD_CTRL;
    if (binding & alt)
        result.Modifiers |= KMOD_ALT;
    if (binding & cmd)
        result.Modifiers |= KMOD_GUI;
    result.Button = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(binding & 0xFF));
    return result;
}

void ShortcutManager::LoadLegacyBindings(const fs::path& path)
{
    constexpr int32_t SUPPORTED_FILE_VERSION = 1;
    constexpr int32_t MAX_LEGACY_SHORTCUTS = 85;

    auto fs = FileStream(path, FILE_MODE_OPEN);
    auto version = fs.ReadValue<uint16_t>();
    if (version == SUPPORTED_FILE_VERSION)
    {
        for (size_t i = 0; i < MAX_LEGACY_SHORTCUTS; i++)
        {
            auto value = fs.ReadValue<uint16_t>();
            auto shortcutId = GetLegacyShortcutId(i);
            if (!shortcutId.empty())
            {
                auto shortcut = GetShortcut(shortcutId);
                if (shortcut != nullptr)
                {
                    shortcut->Current.clear();
                    auto input = ConvertLegacyBinding(value);
                    if (input.has_value())
                    {
                        shortcut->Current.push_back(std::move(input.value()));
                    }
                }
            }
        }
    }
}

void ShortcutManager::LoadUserBindings(const fs::path& path)
{
    auto root = Json::ReadFromFile(path.u8string());
    if (root.is_object())
    {
        for (auto it = root.begin(); it != root.end(); ++it)
        {
            const auto& key = it.key();
            const auto& value = it.value();

            const auto& shortcut = GetShortcut(key);
            if (shortcut != nullptr)
            {
                shortcut->Current.clear();
                if (value.is_string())
                {
                    shortcut->Current.emplace_back(value.get<std::string>());
                }
                else if (value.is_array())
                {
                    for (auto& subValue : value)
                    {
                        shortcut->Current.emplace_back(subValue.get<std::string>());
                    }
                }
            }
        }
    }
}

void ShortcutManager::SaveUserBindings()
{
    try
    {
        auto path = fs::u8path(_env->GetFilePath(PATHID::CONFIG_SHORTCUTS));
        SaveUserBindings(path);
    }
    catch (const std::exception& e)
    {
        Console::Error::WriteLine("Unable to save shortcut bindings: %s", e.what());
    }
}

void ShortcutManager::SaveUserBindings(const fs::path& path)
{
    json_t root;
    if (fs::exists(path))
    {
        root = Json::ReadFromFile(path.u8string());
    }

    for (const auto& shortcut : Shortcuts)
    {
        auto& jShortcut = root[shortcut.second.Id];
        if (shortcut.second.Current.size() == 1)
        {
            jShortcut = shortcut.second.Current[0].ToString();
        }
        else
        {
            jShortcut = nlohmann::json::array();
            for (const auto& binding : shortcut.second.Current)
            {
                jShortcut.push_back(binding.ToString());
            }
        }
    }

    Json::WriteToFile(path.u8string(), root);
}

std::string_view ShortcutManager::GetLegacyShortcutId(size_t index)
{
    static constexpr std::string_view _legacyMap[] = {
        ShortcutId::InterfaceCloseTop,
        ShortcutId::InterfaceCloseAll,
        ShortcutId::InterfaceCancelConstruction,
        ShortcutId::InterfacePause,
        ShortcutId::ViewGeneralZoomOut,
        ShortcutId::ViewGeneralZoomIn,
        ShortcutId::ViewGeneralRotateClockwise,
        ShortcutId::ViewGeneralRotateAnticlockwise,
        ShortcutId::InterfaceRotateConstruction,
        ShortcutId::ViewToggleUnderground,
        ShortcutId::ViewToggleBaseLand,
        ShortcutId::ViewToggleVerticalLand,
        ShortcutId::ViewToggleRides,
        ShortcutId::ViewToggleScenery,
        ShortcutId::ViewToggleSupports,
        ShortcutId::ViewToggleGuests,
        ShortcutId::ViewToggleLandHeightMarkers,
        ShortcutId::ViewToggleTrackHeightMarkers,
        ShortcutId::ViewToggleFootpathHeightMarkers,
        ShortcutId::InterfaceOpenLand,
        ShortcutId::InterfaceOpenWater,
        ShortcutId::InterfaceOpenScenery,
        ShortcutId::InterfaceOpenFootpaths,
        ShortcutId::InterfaceOpenNewRide,
        ShortcutId::InterfaceOpenFinances,
        ShortcutId::InterfaceOpenResearch,
        ShortcutId::InterfaceOpenRides,
        ShortcutId::InterfaceOpenPark,
        ShortcutId::InterfaceOpenGuests,
        ShortcutId::InterfaceOpenStaff,
        ShortcutId::InterfaceOpenMessages,
        ShortcutId::InterfaceOpenMap,
        ShortcutId::InterfaceScreenshot,
        ShortcutId::InterfaceDecreaseSpeed,
        ShortcutId::InterfaceIncreaseSpeed,
        ShortcutId::InterfaceOpenCheats,
        ShortcutId::InterfaceToggleToolbars,
        ShortcutId::ViewScrollUp,
        ShortcutId::ViewScrollLeft,
        ShortcutId::ViewScrollDown,
        ShortcutId::ViewScrollRight,
        ShortcutId::MultiplayerChat,
        ShortcutId::InterfaceSaveGame,
        ShortcutId::InterfaceShowOptions,
        ShortcutId::InterfaceMute,
        ShortcutId::ScaleToggleWindowMode,
        ShortcutId::MultiplayerShow,
        std::string_view(),
        ShortcutId::DebugTogglePaintDebugWindow,
        ShortcutId::ViewToggleFootpaths,
        ShortcutId::WindowRideConstructionTurnLeft,
        ShortcutId::WindowRideConstructionTurnRight,
        ShortcutId::WindowRideConstructionDefault,
        ShortcutId::WindowRideConstructionSlopeDown,
        ShortcutId::WindowRideConstructionSlopeUp,
        ShortcutId::WindowRideConstructionChainLift,
        ShortcutId::WindowRideConstructionBankLeft,
        ShortcutId::WindowRideConstructionBankRight,
        ShortcutId::WindowRideConstructionPrevious,
        ShortcutId::WindowRideConstructionNext,
        ShortcutId::WindowRideConstructionBuild,
        ShortcutId::WindowRideConstructionDemolish,
        ShortcutId::InterfaceLoadGame,
        ShortcutId::InterfaceClearScenery,
        ShortcutId::ViewToggleGridlines,
        ShortcutId::ViewToggleCutAway,
        ShortcutId::ViewToogleFootpathIssues,
        ShortcutId::InterfaceOpenTileInspector,
        ShortcutId::DebugAdvanceTick,
        ShortcutId::InterfaceSceneryPicker,
        ShortcutId::InterfaceScaleIncrease,
        ShortcutId::InterfaceScaleDecrease,
        ShortcutId::WindowTileInspectorToggleInvisibility,
        ShortcutId::WindowTileInspectorCopy,
        ShortcutId::WindowTileInspectorPaste,
        ShortcutId::WindowTileInspectorRemove,
        ShortcutId::WindowTileInspectorMoveUp,
        ShortcutId::WindowTileInspectorMoveDown,
        ShortcutId::WindowTileInspectorIncreaseX,
        ShortcutId::WindowTileInspectorDecreaseX,
        ShortcutId::WindowTileInspectorIncreaseY,
        ShortcutId::WindowTileInspectorDecreaseY,
        ShortcutId::WindowTileInspectorIncreaseHeight,
        ShortcutId::WindowTileInspectorDecreaseHeight,
        ShortcutId::InterfaceDisableClearance,
    };
    return index < std::size(_legacyMap) ? _legacyMap[index] : std::string_view();
}
