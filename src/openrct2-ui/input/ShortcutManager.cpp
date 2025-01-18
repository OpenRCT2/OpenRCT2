/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include <openrct2/localisation/Language.h>
#include <openrct2/ui/WindowManager.h>

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
        shortcut.OrderIndex = Shortcuts.size();
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

            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseByClass(WindowClass::ChangeKeyboardShortcut);
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
    constexpr uint16_t kNullBinding = 0xFFFF;
    constexpr uint16_t kShift = 0x100;
    constexpr uint16_t kCtrl = 0x200;
    constexpr uint16_t kAlt = 0x400;
    constexpr uint16_t kCmd = 0x800;

    if (binding == kNullBinding)
    {
        return std::nullopt;
    }

    ShortcutInput result;
    result.Kind = InputDeviceKind::Keyboard;
    if (binding & kShift)
        result.Modifiers |= KMOD_SHIFT;
    if (binding & kCtrl)
        result.Modifiers |= KMOD_CTRL;
    if (binding & kAlt)
        result.Modifiers |= KMOD_ALT;
    if (binding & kCmd)
        result.Modifiers |= KMOD_GUI;
    result.Button = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(binding & 0xFF));
    return result;
}

void ShortcutManager::LoadLegacyBindings(const fs::path& path)
{
    constexpr int32_t kSupportedFileVersion = 1;
    constexpr int32_t kMaxLegacyShortcuts = 85;

    auto fs = FileStream(path, FILE_MODE_OPEN);
    auto version = fs.ReadValue<uint16_t>();
    if (version == kSupportedFileVersion)
    {
        for (size_t i = 0; i < kMaxLegacyShortcuts; i++)
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
        ShortcutId::kInterfaceCloseTop,
        ShortcutId::kInterfaceCloseAll,
        ShortcutId::kInterfaceCancelConstruction,
        ShortcutId::kInterfacePause,
        ShortcutId::kViewGeneralZoomOut,
        ShortcutId::kViewGeneralZoomIn,
        ShortcutId::kViewGeneralRotateClockwise,
        ShortcutId::kViewGeneralRotateAnticlockwise,
        ShortcutId::kInterfaceRotateConstruction,
        ShortcutId::kViewToggleUnderground,
        ShortcutId::kViewToggleBaseLand,
        ShortcutId::kViewToggleVerticalLand,
        ShortcutId::kViewToggleRides,
        ShortcutId::kViewToggleScenery,
        ShortcutId::kViewToggleSupports,
        ShortcutId::kViewToggleGuests,
        ShortcutId::kViewToggleLandHeightMarkers,
        ShortcutId::kViewToggleTrackHeightMarkers,
        ShortcutId::kViewToggleFootpathHeightMarkers,
        ShortcutId::kInterfaceOpenLand,
        ShortcutId::kInterfaceOpenWater,
        ShortcutId::kInterfaceOpenScenery,
        ShortcutId::kInterfaceOpenFootpaths,
        ShortcutId::kInterfaceOpenNewRide,
        ShortcutId::kInterfaceOpenFinances,
        ShortcutId::kInterfaceOpenResearch,
        ShortcutId::kInterfaceOpenRides,
        ShortcutId::kInterfaceOpenPark,
        ShortcutId::kInterfaceOpenGuests,
        ShortcutId::kInterfaceOpenStaff,
        ShortcutId::kInterfaceOpenMessages,
        ShortcutId::kInterfaceOpenMap,
        ShortcutId::kInterfaceScreenshot,
        ShortcutId::kInterfaceDecreaseSpeed,
        ShortcutId::kInterfaceIncreaseSpeed,
        ShortcutId::kInterfaceOpenCheats,
        ShortcutId::kInterfaceToggleToolbars,
        ShortcutId::kViewScrollUp,
        ShortcutId::kViewScrollLeft,
        ShortcutId::kViewScrollDown,
        ShortcutId::kViewScrollRight,
        ShortcutId::kInterfaceMultiplayerChat,
        ShortcutId::kInterfaceSaveGame,
        ShortcutId::kInterfaceShowOptions,
        ShortcutId::kInterfaceMute,
        ShortcutId::kInterfaceScaleToggleWindowMode,
        ShortcutId::kInterfaceMultiplayerShow,
        std::string_view(),
        ShortcutId::kDebugTogglePaintDebugWindow,
        ShortcutId::kViewToggleFootpaths,
        ShortcutId::kWindowRideConstructionTurnLeft,
        ShortcutId::kWindowRideConstructionTurnRight,
        ShortcutId::kWindowRideConstructionDefault,
        ShortcutId::kWindowRideConstructionSlopeDown,
        ShortcutId::kWindowRideConstructionSlopeUp,
        ShortcutId::kWindowRideConstructionChainLift,
        ShortcutId::kWindowRideConstructionBankLeft,
        ShortcutId::kWindowRideConstructionBankRight,
        ShortcutId::kWindowRideConstructionPrevious,
        ShortcutId::kWindowRideConstructionNext,
        ShortcutId::kWindowRideConstructionBuild,
        ShortcutId::kWindowRideConstructionDemolish,
        ShortcutId::kInterfaceLoadGame,
        ShortcutId::kInterfaceClearScenery,
        ShortcutId::kViewToggleGridlines,
        ShortcutId::kViewToggleCutAway,
        ShortcutId::kViewToggleFootpathIssues,
        ShortcutId::kInterfaceOpenTileInspector,
        ShortcutId::kDebugAdvanceTick,
        ShortcutId::kInterfaceSceneryPicker,
        ShortcutId::kInterfaceScaleIncrease,
        ShortcutId::kInterfaceScaleDecrease,
        ShortcutId::kWindowTileInspectorToggleInvisibility,
        ShortcutId::kWindowTileInspectorCopy,
        ShortcutId::kWindowTileInspectorPaste,
        ShortcutId::kWindowTileInspectorRemove,
        ShortcutId::kWindowTileInspectorMoveUp,
        ShortcutId::kWindowTileInspectorMoveDown,
        ShortcutId::kWindowTileInspectorIncreaseX,
        ShortcutId::kWindowTileInspectorDecreaseX,
        ShortcutId::kWindowTileInspectorIncreaseY,
        ShortcutId::kWindowTileInspectorDecreaseY,
        ShortcutId::kWindowTileInspectorIncreaseHeight,
        ShortcutId::kWindowTileInspectorDecreaseHeight,
        ShortcutId::kInterfaceDisableClearance,
    };
    return index < std::size(_legacyMap) ? _legacyMap[index] : std::string_view();
}
