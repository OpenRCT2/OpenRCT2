/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ShortcutManager.h"

#include "ShortcutIds.h"

#include <SDL.h>
#include <openrct2-ui/UiStringIds.h>
#include <openrct2/Context.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/core/Console.hpp>
#include <openrct2/core/DataSerialiser.h>
#include <openrct2/core/FileStream.h>
#include <openrct2/core/FileSystem.hpp>
#include <openrct2/core/Json.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/ui/WindowManager.h>

using namespace OpenRCT2::Ui;

std::string_view RegisteredShortcut::getTopLevelGroup() const
{
    auto fullstopIndex = id.find('.');
    if (fullstopIndex != std::string::npos)
    {
        return std::string_view(id.c_str(), fullstopIndex);
    }
    return {};
}

std::string_view RegisteredShortcut::getGroup() const
{
    auto fullstopIndex = id.find_last_of('.');
    if (fullstopIndex != std::string::npos)
    {
        return std::string_view(id.c_str(), fullstopIndex);
    }
    return {};
}

bool RegisteredShortcut::matches(const InputEvent& e) const
{
    if (isSuitableInputEvent(e))
    {
        auto result = std::find_if(
            current.begin(), current.end(), [e](const ShortcutInput& _action) { return _action.matches(e); });
        return result != current.end();
    }
    return false;
}

bool RegisteredShortcut::isSuitableInputEvent(const InputEvent& e) const
{
    // Do not intercept button releases
    if (e.state == InputEventState::release)
    {
        return false;
    }

    if (e.deviceKind == InputDeviceKind::mouse)
    {
        // Do not allow LMB or RMB to be shortcut
        if (e.button == 0 || e.button == 1)
        {
            return false;
        }
    }
    else if (e.deviceKind == InputDeviceKind::keyboard)
    {
        // Do not allow modifier keys alone
        switch (e.button)
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

std::string RegisteredShortcut::getDisplayString() const
{
    std::string result;
    auto numChords = current.size();
    for (size_t i = 0; i < numChords; i++)
    {
        const auto& kc = current[i];
        result += kc.toLocalisedString();
        if (i < numChords - 1)
        {
            result += " ";
            result += LanguageGetString(STR_OR);
            result += " ";
        }
    }
    return result;
}

ShortcutManager::ShortcutManager(IPlatformEnvironment& env)
    : _env(env)
{
    registerDefaultShortcuts();
}

void ShortcutManager::registerShortcut(RegisteredShortcut&& shortcut)
{
    if (!shortcut.id.empty() && getShortcut(shortcut.id) == nullptr)
    {
        auto id = std::make_unique<std::string>(shortcut.id);
        auto idView = std::string_view(*id);
        _ids.push_back(std::move(id));
        shortcut.orderIndex = shortcuts.size();
        shortcuts[idView] = shortcut;
    }
}

RegisteredShortcut* ShortcutManager::getShortcut(std::string_view id)
{
    auto result = shortcuts.find(id);
    return result == shortcuts.end() ? nullptr : &result->second;
}

void ShortcutManager::removeShortcut(std::string_view id)
{
    shortcuts.erase(id);
    _ids.erase(
        std::remove_if(_ids.begin(), _ids.end(), [id](const std::unique_ptr<std::string>& x) { return *x == id; }), _ids.end());
}

bool ShortcutManager::isPendingShortcutChange() const
{
    return !_pendingShortcutChange.empty();
}

void ShortcutManager::setPendingShortcutChange(std::string_view id)
{
    _pendingShortcutChange = id;
}

void ShortcutManager::processEvent(const InputEvent& e)
{
    if (!isPendingShortcutChange())
    {
        for (const auto& shortcut : shortcuts)
        {
            if (shortcut.second.matches(e))
            {
                shortcut.second.action();
            }
        }
    }
    else
    {
        auto shortcut = getShortcut(_pendingShortcutChange);
        if (shortcut != nullptr && shortcut->isSuitableInputEvent(e))
        {
            auto shortcutInput = ShortcutInput::fromInputEvent(e);
            if (shortcutInput.has_value())
            {
                shortcut->current.clear();
                shortcut->current.push_back(std::move(shortcutInput.value()));
            }
            _pendingShortcutChange.clear();

            auto* windowMgr = GetWindowManager();
            windowMgr->CloseByClass(WindowClass::changeKeyboardShortcut);
            saveUserBindings();
        }
    }
}

bool ShortcutManager::processEventForSpecificShortcut(const InputEvent& e, std::string_view id)
{
    auto shortcut = getShortcut(id);
    if (shortcut != nullptr && shortcut->matches(e))
    {
        shortcut->action();
        return true;
    }
    return false;
}

void ShortcutManager::loadUserBindings()
{
    try
    {
        auto path = fs::u8path(_env.GetFilePath(PathId::configShortcuts));
        if (fs::exists(path))
        {
            loadUserBindings(path);
        }
        else
        {
            try
            {
                Console::WriteLine("Importing legacy shortcuts...");
                auto legacyPath = fs::u8path(_env.GetFilePath(PathId::configShortcutsLegacy));
                if (fs::exists(legacyPath))
                {
                    loadLegacyBindings(legacyPath);
                    saveUserBindings();
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

std::optional<ShortcutInput> ShortcutManager::convertLegacyBinding(uint16_t binding)
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
    result.kind = InputDeviceKind::keyboard;
    if (binding & kShift)
        result.modifiers |= KMOD_SHIFT;
    if (binding & kCtrl)
        result.modifiers |= KMOD_CTRL;
    if (binding & kAlt)
        result.modifiers |= KMOD_ALT;
    if (binding & kCmd)
        result.modifiers |= KMOD_GUI;
    result.button = SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(binding & 0xFF));
    return result;
}

void ShortcutManager::loadLegacyBindings(const fs::path& path)
{
    constexpr int32_t kSupportedFileVersion = 1;
    constexpr int32_t kMaxLegacyShortcuts = 85;

    auto fs = FileStream(path, FileMode::open);
    auto version = fs.ReadValue<uint16_t>();
    if (version == kSupportedFileVersion)
    {
        for (size_t i = 0; i < kMaxLegacyShortcuts; i++)
        {
            auto value = fs.ReadValue<uint16_t>();
            auto shortcutId = getLegacyShortcutId(i);
            if (!shortcutId.empty())
            {
                auto shortcut = getShortcut(shortcutId);
                if (shortcut != nullptr)
                {
                    shortcut->current.clear();
                    auto input = convertLegacyBinding(value);
                    if (input.has_value())
                    {
                        shortcut->current.push_back(std::move(input.value()));
                    }
                }
            }
        }
    }
}

void ShortcutManager::loadUserBindings(const fs::path& path)
{
    auto root = Json::ReadFromFile(path.u8string());
    if (root.is_object())
    {
        for (auto it = root.begin(); it != root.end(); ++it)
        {
            const auto& key = it.key();
            const auto& value = it.value();

            const auto& shortcut = getShortcut(key);
            if (shortcut != nullptr)
            {
                shortcut->current.clear();
                if (value.is_string())
                {
                    shortcut->current.emplace_back(value.get<std::string>());
                }
                else if (value.is_array())
                {
                    for (auto& subValue : value)
                    {
                        shortcut->current.emplace_back(subValue.get<std::string>());
                    }
                }
            }
        }
    }
}

void ShortcutManager::saveUserBindings()
{
    try
    {
        auto path = fs::u8path(_env.GetFilePath(PathId::configShortcuts));
        saveUserBindings(path);
    }
    catch (const std::exception& e)
    {
        Console::Error::WriteLine("Unable to save shortcut bindings: %s", e.what());
    }
}

void ShortcutManager::saveUserBindings(const fs::path& path)
{
    json_t root;
    if (fs::exists(path))
    {
        root = Json::ReadFromFile(path.u8string());
    }

    for (const auto& shortcut : shortcuts)
    {
        auto& jShortcut = root[shortcut.second.id];
        if (shortcut.second.current.size() == 1)
        {
            jShortcut = shortcut.second.current[0].toString();
        }
        else
        {
            jShortcut = nlohmann::json::array();
            for (const auto& binding : shortcut.second.current)
            {
                jShortcut.push_back(binding.toString());
            }
        }
    }

    Json::WriteToFile(path.u8string(), root);
}

std::string_view ShortcutManager::getLegacyShortcutId(size_t index)
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
