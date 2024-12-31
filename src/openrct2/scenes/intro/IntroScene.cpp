/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "IntroScene.h"

#include "../../Context.h"
#include "../../audio/AudioChannel.h"
#include "../../audio/AudioMixer.h"
#include "../../audio/audio.h"
#include "../../drawing/Drawing.h"
#include "../../sprites.h"

#include <memory>

using OpenRCT2::Audio::SoundId;

namespace OpenRCT2
{
    static constexpr PaletteIndex kBackgroundColourDark = PALETTE_INDEX_10;
    static constexpr PaletteIndex kBackgroundColourLogo = PALETTE_INDEX_245;
    static constexpr PaletteIndex kBorderColourPublisher = PALETTE_INDEX_129;

    constexpr int32_t PALETTE_G1_IDX_DEVELOPER = 23217;
    constexpr int32_t PALETTE_G1_IDX_LOGO = 23224;

    static IntroState _introState;

    // Used mainly for timing but also for Y coordinate and fading.
    static int32_t _introStateCounter;

    static std::shared_ptr<Audio::IAudioChannel> _soundChannel = nullptr;
    static bool _chainLiftFinished;

    bool IntroIsPlaying()
    {
        return _introState != IntroState::None;
    }

    void IntroScene::Load()
    {
        _introState = IntroState::PublisherBegin;
    }

    void IntroScene::Stop()
    {
        _introState = IntroState::None;
        LoadPalette();
    }

    static void ScreenIntroProcessMouseInput();
    static void ScreenIntroProcessKeyboardInput();
    static void ScreenIntroSkipPart();
    static void ScreenIntroDrawLogo(DrawPixelInfo& dpi);

    // rct2: 0x0068E966
    void IntroScene::Tick()
    {
        ScreenIntroProcessMouseInput();
        ScreenIntroProcessKeyboardInput();

        switch (_introState)
        {
            case IntroState::Disclaimer1:
            case IntroState::Disclaimer2:
                // Originally used for the disclaimer text
                _introState = IntroState::PublisherBegin;
                [[fallthrough]];
            case IntroState::PublisherBegin:
                LoadPalette();

                // Set the Y for the Infogrames logo
                _introStateCounter = -580;

                // Play the chain lift sound
                _soundChannel = Audio::CreateAudioChannel(SoundId::LiftBM, true);
                _chainLiftFinished = false;
                _introState = IntroState::PublisherScroll;
                break;
            case IntroState::PublisherScroll:
                // Move the Infogrames logo down
                _introStateCounter += 5;

                // Check if logo is off the screen...ish
                if (_introStateCounter > ContextGetHeight() - 120)
                {
                    _introStateCounter = -116;
                    _introState = IntroState::DeveloperBegin;
                }

                break;
            case IntroState::DeveloperBegin:
                // Set the Y for the Chris Sawyer logo
                _introStateCounter = -116;

                _introState = IntroState::DeveloperScroll;
                break;
            case IntroState::DeveloperScroll:
                _introStateCounter += 5;

                // Check if logo is almost scrolled to the bottom
                if (!_chainLiftFinished && _introStateCounter >= ContextGetHeight() + 40 - 421)
                {
                    _chainLiftFinished = true;

                    // Stop the chain lift sound
                    if (_soundChannel != nullptr)
                    {
                        _soundChannel->Stop();
                        _soundChannel = nullptr;
                    }

                    // Play the track friction sound
                    _soundChannel = Audio::CreateAudioChannel(
                        SoundId::TrackFrictionBM, true, Audio::kMixerVolumeMax, 0.25f, 0.75);
                }

                // Check if logo is off the screen...ish
                if (_introStateCounter >= ContextGetHeight() + 40)
                {
                    // Stop the track friction sound
                    if (_soundChannel != nullptr)
                    {
                        _soundChannel->Stop();
                        _soundChannel = nullptr;
                    }

                    // Play long peep scream sound
                    _soundChannel = Audio::CreateAudioChannel(SoundId::Scream1);

                    _introState = IntroState::LogoFadeIn;
                    _introStateCounter = 0;
                }
                break;
            case IntroState::LogoFadeIn:
                // Fade in, add 4 / 256 to fading
                _introStateCounter += 0x400;
                if (_introStateCounter > 0xFF00)
                {
                    _introState = IntroState::LogoWait;
                    _introStateCounter = 0;
                }
                break;
            case IntroState::LogoWait:
                // Wait 80 game ticks
                _introStateCounter++;
                if (_introStateCounter >= 80)
                {
                    // Set fading to 256
                    _introStateCounter = 0xFF00;

                    _introState = IntroState::LogoFadeOut;
                }
                break;
            case IntroState::LogoFadeOut:
                // Fade out, subtract 4 / 256 from fading
                _introStateCounter -= 0x400;
                if (_introStateCounter < 0)
                {
                    _introState = IntroState::Clear;
                }
                break;
            case IntroState::Clear:
                // Stop any playing sound
                if (_soundChannel != nullptr)
                {
                    _soundChannel->Stop();
                    _soundChannel = nullptr;
                }

                // Move to next part
                _introState = IntroState::Finish;
                _introStateCounter = 0;
                break;
            case IntroState::Finish:
            {
                auto& context = GetContext();
                context.SetActiveScene(context.GetTitleScene());
            }
            default:
                break;
        }
    }

    void IntroDraw(DrawPixelInfo& dpi)
    {
        int32_t screenWidth = ContextGetWidth();

        switch (_introState)
        {
            case IntroState::Disclaimer1:
            case IntroState::Disclaimer2:
                break;
            case IntroState::PublisherBegin:
                GfxClear(dpi, kBackgroundColourDark);
                break;
            case IntroState::PublisherScroll:
                GfxClear(dpi, kBackgroundColourDark);

                // Draw a white rectangle for the logo background (gives a bit of white margin)
                GfxFillRect(
                    dpi,
                    { { (screenWidth / 2) - 320 + 50, _introStateCounter + 50 },
                      { (screenWidth / 2) - 320 + 50 + 540, _introStateCounter + 50 + 425 } },
                    kBorderColourPublisher);

                // Draw Infogrames logo
                GfxDrawSprite(dpi, ImageId(SPR_INTRO_INFOGRAMES_00), { (screenWidth / 2) - 320 + 69, _introStateCounter + 69 });
                GfxDrawSprite(
                    dpi, ImageId(SPR_INTRO_INFOGRAMES_10), { (screenWidth / 2) - 320 + 319, _introStateCounter + 69 });
                GfxDrawSprite(
                    dpi, ImageId(SPR_INTRO_INFOGRAMES_01), { (screenWidth / 2) - 320 + 69, _introStateCounter + 319 });
                GfxDrawSprite(
                    dpi, ImageId(SPR_INTRO_INFOGRAMES_11), { (screenWidth / 2) - 320 + 319, _introStateCounter + 319 });
                break;
            case IntroState::DeveloperBegin:
                GfxClear(dpi, kBackgroundColourDark);
                GfxTransposePalette(PALETTE_G1_IDX_DEVELOPER, 255);
                break;
            case IntroState::DeveloperScroll:
                GfxClear(dpi, kBackgroundColourDark);

                // Draw Chris Sawyer logo
                GfxDrawSprite(dpi, ImageId(SPR_INTRO_CHRIS_SAWYER_00), { (screenWidth / 2) - 320 + 70, _introStateCounter });
                GfxDrawSprite(dpi, ImageId(SPR_INTRO_CHRIS_SAWYER_10), { (screenWidth / 2) - 320 + 320, _introStateCounter });
                break;
            case IntroState::LogoFadeIn:
                if (_introStateCounter <= 0xFF00)
                {
                    GfxTransposePalette(PALETTE_G1_IDX_LOGO, (_introStateCounter >> 8) & 0xFF);
                }
                else
                {
                    GfxTransposePalette(PALETTE_G1_IDX_LOGO, 255);
                }
                ScreenIntroDrawLogo(dpi);
                break;
            case IntroState::LogoWait:
                ScreenIntroDrawLogo(dpi);
                break;
            case IntroState::LogoFadeOut:
                if (_introStateCounter >= 0)
                {
                    GfxTransposePalette(PALETTE_G1_IDX_LOGO, (_introStateCounter >> 8) & 0xFF);
                }
                else
                {
                    GfxTransposePalette(PALETTE_G1_IDX_LOGO, 0);
                }
                ScreenIntroDrawLogo(dpi);
                break;
            case IntroState::Clear:
                GfxClear(dpi, kBackgroundColourDark);
                break;
            default:
                break;
        }
    }

    static void ScreenIntroProcessMouseInput()
    {
        if (ContextGetCursorState()->any == CURSOR_PRESSED)
        {
            ScreenIntroSkipPart();
        }
    }

    /**
     *
     *  rct2: 0x006E3AEC
     */
    static void ScreenIntroProcessKeyboardInput()
    {
        const uint8_t* keys = ContextGetKeysState();
        for (int i = 0; i < 256; i++)
        {
            if (keys[i] != 0)
            {
                ScreenIntroSkipPart();
                break;
            }
        }
    }

    static void ScreenIntroSkipPart()
    {
        switch (_introState)
        {
            case IntroState::None:
                break;
            case IntroState::Disclaimer2:
                _introState = IntroState::PublisherBegin;
                break;
            default:
                _introState = IntroState::Clear;
                break;
        }
    }

    static void ScreenIntroDrawLogo(DrawPixelInfo& dpi)
    {
        int32_t screenWidth = ContextGetWidth();
        int32_t imageWidth = 640;
        int32_t imageX = (screenWidth - imageWidth) / 2;

        DrawingEngineInvalidateImage(SPR_INTRO_LOGO_00);
        DrawingEngineInvalidateImage(SPR_INTRO_LOGO_10);
        DrawingEngineInvalidateImage(SPR_INTRO_LOGO_20);
        DrawingEngineInvalidateImage(SPR_INTRO_LOGO_01);
        DrawingEngineInvalidateImage(SPR_INTRO_LOGO_11);
        DrawingEngineInvalidateImage(SPR_INTRO_LOGO_21);

        GfxClear(dpi, kBackgroundColourLogo);
        GfxDrawSprite(dpi, ImageId(SPR_INTRO_LOGO_00), { imageX + 0, 0 });
        GfxDrawSprite(dpi, ImageId(SPR_INTRO_LOGO_10), { imageX + 220, 0 });
        GfxDrawSprite(dpi, ImageId(SPR_INTRO_LOGO_20), { imageX + 440, 0 });
        GfxDrawSprite(dpi, ImageId(SPR_INTRO_LOGO_01), { imageX + 0, 240 });
        GfxDrawSprite(dpi, ImageId(SPR_INTRO_LOGO_11), { imageX + 220, 240 });
        GfxDrawSprite(dpi, ImageId(SPR_INTRO_LOGO_21), { imageX + 440, 240 });
    }
} // namespace OpenRCT2
