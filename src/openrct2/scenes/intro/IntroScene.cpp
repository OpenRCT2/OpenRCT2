/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "IntroScene.h"

#include "../../Context.h"
#include "../../Input.h"
#include "../../OpenRCT2.h"
#include "../../SpriteIds.h"
#include "../../audio/Audio.h"
#include "../../audio/AudioChannel.h"
#include "../../audio/AudioMixer.h"
#include "../../drawing/Drawing.h"
#include "../../drawing/Rectangle.h"

#include <memory>

using OpenRCT2::Audio::SoundId;
using namespace OpenRCT2::Drawing;

namespace OpenRCT2
{
    static constexpr PaletteIndex kBackgroundColourDark = PaletteIndex::pi10;
    static constexpr PaletteIndex kBackgroundColourLogo = PaletteIndex::primaryRemap2;
    static constexpr PaletteIndex kBorderColourPublisher = PaletteIndex::pi129;

    static constexpr ImageIndex kPaletteChrisSawyerLogo = 23217;
    static constexpr ImageIndex kPaletteRCT2Logo = 23224;

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
    static void ScreenIntroDrawLogo(RenderTarget& rt);

    // rct2: 0x0068E966
    void IntroScene::Tick()
    {
        ScreenIntroProcessMouseInput();
        ScreenIntroProcessKeyboardInput();

        switch (_introState)
        {
            case IntroState::PublisherBegin:
                LoadPalette();

                // Set the Y for the Infogrames logo
                _introStateCounter = -580;

                // Play the chain lift sound
                _soundChannel = Audio::CreateAudioChannel(SoundId::liftBM, true);
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
                        SoundId::trackFrictionBM, true, Audio::kMixerVolumeMax, 0.25f, 0.75);
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
                    _soundChannel = Audio::CreateAudioChannel(SoundId::scream1);

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

                if (gOpenRCT2Headless)
                {
                    // Move to next part
                    _introState = IntroState::Finish;
                    _introStateCounter = 0;
                }
                break;
            case IntroState::Finish:
            {
                auto& context = GetContext();
                context.SetActiveScene(context.GetTitleScene());
                break;
            }
            default:
                break;
        }
    }

    void IntroDraw(RenderTarget& rt)
    {
        int32_t screenWidth = ContextGetWidth();

        switch (_introState)
        {
            case IntroState::PublisherBegin:
                GfxClear(rt, kBackgroundColourDark);
                break;
            case IntroState::PublisherScroll:
                GfxClear(rt, kBackgroundColourDark);

                // Draw a white rectangle for the logo background (gives a bit of white margin)
                Rectangle::fill(
                    rt,
                    { { (screenWidth / 2) - 320 + 50, _introStateCounter + 50 },
                      { (screenWidth / 2) - 320 + 50 + 540, _introStateCounter + 50 + 425 } },
                    kBorderColourPublisher);

                // Draw Infogrames logo
                GfxDrawSprite(rt, ImageId(SPR_INTRO_INFOGRAMES_00), { (screenWidth / 2) - 320 + 69, _introStateCounter + 69 });
                GfxDrawSprite(rt, ImageId(SPR_INTRO_INFOGRAMES_10), { (screenWidth / 2) - 320 + 319, _introStateCounter + 69 });
                GfxDrawSprite(rt, ImageId(SPR_INTRO_INFOGRAMES_01), { (screenWidth / 2) - 320 + 69, _introStateCounter + 319 });
                GfxDrawSprite(
                    rt, ImageId(SPR_INTRO_INFOGRAMES_11), { (screenWidth / 2) - 320 + 319, _introStateCounter + 319 });
                break;
            case IntroState::DeveloperBegin:
                GfxClear(rt, kBackgroundColourDark);
                GfxTransposePalette(kPaletteChrisSawyerLogo, 255);
                break;
            case IntroState::DeveloperScroll:
                GfxClear(rt, kBackgroundColourDark);

                // Draw Chris Sawyer logo
                GfxDrawSprite(rt, ImageId(SPR_INTRO_CHRIS_SAWYER_00), { (screenWidth / 2) - 320 + 70, _introStateCounter });
                GfxDrawSprite(rt, ImageId(SPR_INTRO_CHRIS_SAWYER_10), { (screenWidth / 2) - 320 + 320, _introStateCounter });
                break;
            case IntroState::LogoFadeIn:
                if (_introStateCounter <= 0xFF00)
                {
                    GfxTransposePalette(kPaletteRCT2Logo, (_introStateCounter >> 8) & 0xFF);
                }
                else
                {
                    GfxTransposePalette(kPaletteRCT2Logo, 255);
                }
                ScreenIntroDrawLogo(rt);
                break;
            case IntroState::LogoWait:
                ScreenIntroDrawLogo(rt);
                break;
            case IntroState::LogoFadeOut:
                if (_introStateCounter >= 0)
                {
                    GfxTransposePalette(kPaletteRCT2Logo, (_introStateCounter >> 8) & 0xFF);
                }
                else
                {
                    GfxTransposePalette(kPaletteRCT2Logo, 0);
                }
                ScreenIntroDrawLogo(rt);
                break;
            case IntroState::Clear:
                GfxClear(rt, kBackgroundColourDark);

                // Move to next part. Has to be done here to ensure the screen is cleared.
                _introState = IntroState::Finish;
                _introStateCounter = 0;
                break;
            default:
                break;
        }
    }

    static void ScreenIntroProcessMouseInput()
    {
        if (ContextGetCursorState()->any & CURSOR_DOWN)
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
            default:
                _introState = IntroState::Clear;
                break;
        }
    }

    static void ScreenIntroDrawLogo(RenderTarget& rt)
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

        GfxClear(rt, kBackgroundColourLogo);
        GfxDrawSprite(rt, ImageId(SPR_INTRO_LOGO_00), { imageX + 0, 0 });
        GfxDrawSprite(rt, ImageId(SPR_INTRO_LOGO_10), { imageX + 220, 0 });
        GfxDrawSprite(rt, ImageId(SPR_INTRO_LOGO_20), { imageX + 440, 0 });
        GfxDrawSprite(rt, ImageId(SPR_INTRO_LOGO_01), { imageX + 0, 240 });
        GfxDrawSprite(rt, ImageId(SPR_INTRO_LOGO_11), { imageX + 220, 240 });
        GfxDrawSprite(rt, ImageId(SPR_INTRO_LOGO_21), { imageX + 440, 240 });
    }
} // namespace OpenRCT2
