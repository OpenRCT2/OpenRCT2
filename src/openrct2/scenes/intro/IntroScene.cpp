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

#include <cstdint>

using OpenRCT2::Audio::SoundId;
using namespace OpenRCT2::Drawing;

namespace OpenRCT2
{
    static constexpr PaletteIndex kBackgroundColourDark = PaletteIndex::pi10;
    static constexpr PaletteIndex kBackgroundColourLogo = PaletteIndex::primaryRemap2;
    static constexpr PaletteIndex kBorderColourPublisher = PaletteIndex::pi129;

    static constexpr ImageIndex kSpriteChrisSawyerLogo00 = 23215;
    static constexpr ImageIndex kSpriteChrisSawyerLogo10 = kSpriteChrisSawyerLogo00 + 1;
    static constexpr ImageIndex kPaletteChrisSawyerLogo = 23217;

    static constexpr ImageIndex kSpriteRCT2Logo00 = 23218;
    static constexpr ImageIndex kSpriteRCT2Logo10 = kSpriteRCT2Logo00 + 1;
    static constexpr ImageIndex kSpriteRCT2Logo20 = kSpriteRCT2Logo00 + 2;
    static constexpr ImageIndex kSpriteRCT2Logo01 = kSpriteRCT2Logo00 + 3;
    static constexpr ImageIndex kSpriteRCT2Logo11 = kSpriteRCT2Logo00 + 4;
    static constexpr ImageIndex kSpriteRCT2Logo21 = kSpriteRCT2Logo00 + 5;
    static constexpr ImageIndex kPaletteRCT2Logo = 23224;

    static constexpr ImageIndex kSpriteInfogramesLogo00 = 23226;
    static constexpr ImageIndex kSpriteInfogramesLogo10 = kSpriteInfogramesLogo00 + 1;
    static constexpr ImageIndex kSpriteInfogramesLogo01 = kSpriteInfogramesLogo00 + 2;
    static constexpr ImageIndex kSpriteInfogramesLogo11 = kSpriteInfogramesLogo00 + 3;

    // Including the white box around it
    constexpr int32_t kInfogramesLogoWidth = 541;
    constexpr int32_t kInfogramesLogoHeight = 426;
    constexpr int32_t kInfogramesLogoPadding = 19;
    constexpr int32_t kInfogramesLogoOffscreen = kInfogramesLogoHeight + 154;

    static std::weak_ptr<IntroSceneImpl> introSceneImplementation;

    class IntroSceneImpl
    {
    public:
        bool IntroIsPlaying();
        void IntroDraw(Drawing::RenderTarget& rt);
        void Load();
        void Stop();
        void Tick(IContext& sceneContext);

    private:
        void ScreenIntroProcessMouseInput();
        void ScreenIntroProcessKeyboardInput();
        void ScreenIntroSkipPart();
        void ScreenIntroDrawLogo(RenderTarget& rt);

    private:
        enum class IntroState : uint8_t
        {
            None,
            PublisherBegin,
            PublisherScroll,
            DeveloperBegin,
            DeveloperScroll,
            LogoFadeIn,
            LogoWait,
            LogoFadeOut,
            Clear = 254,
            Finish = 255,
        };

        IntroState _introState;

        // Used mainly for timing but also for Y coordinate and fading.
        int32_t _introStateCounter;

        std::shared_ptr<Audio::IAudioChannel> _soundChannel = nullptr;
        bool _chainLiftFinished;
    };

    bool IntroIsPlaying()
    {
        if (auto impl = introSceneImplementation.lock())
        {
            return impl->IntroIsPlaying();
        }

        return false;
    }

    bool IntroSceneImpl::IntroIsPlaying()
    {
        return _introState != IntroState::None;
    }

    void IntroSceneImpl::Load()
    {
        _introState = IntroState::PublisherBegin;
    }

    void IntroSceneImpl::Stop()
    {
        _introState = IntroState::None;
        LoadPalette();
    }

    // rct2: 0x0068E966
    void IntroSceneImpl::Tick(IContext& sceneContext)
    {
        ScreenIntroProcessMouseInput();
        ScreenIntroProcessKeyboardInput();

        switch (_introState)
        {
            case IntroState::PublisherBegin:
                LoadPalette();

                // Set the Y for the Infogrames logo
                _introStateCounter = -kInfogramesLogoOffscreen;

                // Play the chain lift sound
                _soundChannel = Audio::CreateAudioChannel(SoundId::liftBM, true);
                _chainLiftFinished = false;
                _introState = IntroState::PublisherScroll;
                break;
            case IntroState::PublisherScroll:
                // Move the Infogrames logo down
                _introStateCounter += 5;

                // Check if logo is off the screen
                if (_introStateCounter > ContextGetHeight())
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
                sceneContext.SetActiveScene(sceneContext.GetTitleScene());
                break;
            }
            default:
                break;
        }
    }

    void IntroDraw(RenderTarget& rt)
    {
        if (auto impl = introSceneImplementation.lock())
        {
            impl->IntroDraw(rt);
        }
    }

    void IntroSceneImpl::IntroDraw(RenderTarget& rt)
    {
        int32_t screenWidth = ContextGetWidth();

        switch (_introState)
        {
            case IntroState::PublisherBegin:
                GfxClear(rt, kBackgroundColourDark);
                break;
            case IntroState::PublisherScroll:
            {
                GfxClear(rt, kBackgroundColourDark);

                // Draw a white rectangle for the logo background (gives a bit of white margin)
                const auto leftTop = ScreenCoordsXY((screenWidth - kInfogramesLogoWidth) / 2, _introStateCounter + 50);
                const auto rightBottom = leftTop + ScreenSize(kInfogramesLogoWidth - 1, kInfogramesLogoHeight - 1);
                Rectangle::fill(rt, { leftTop, rightBottom }, kBorderColourPublisher);

                // Draw Infogrames logo
                auto leftTopLogo = leftTop + ScreenCoordsXY(kInfogramesLogoPadding, kInfogramesLogoPadding);
                GfxDrawSprite(rt, ImageId(kSpriteInfogramesLogo00), leftTopLogo);
                GfxDrawSprite(rt, ImageId(kSpriteInfogramesLogo10), leftTopLogo + ScreenCoordsXY(250, 0));
                GfxDrawSprite(rt, ImageId(kSpriteInfogramesLogo01), leftTopLogo + ScreenCoordsXY(0, 250));
                GfxDrawSprite(rt, ImageId(kSpriteInfogramesLogo11), leftTopLogo + ScreenCoordsXY(250, 250));
                break;
            }
            case IntroState::DeveloperBegin:
                GfxClear(rt, kBackgroundColourDark);
                GfxTransposePalette(kPaletteChrisSawyerLogo, 255);
                break;
            case IntroState::DeveloperScroll:
                GfxClear(rt, kBackgroundColourDark);

                // Draw Chris Sawyer logo
                GfxDrawSprite(rt, ImageId(kSpriteChrisSawyerLogo00), { (screenWidth / 2) - 320 + 70, _introStateCounter });
                GfxDrawSprite(rt, ImageId(kSpriteChrisSawyerLogo10), { (screenWidth / 2) - 320 + 320, _introStateCounter });
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

    void IntroSceneImpl::ScreenIntroProcessMouseInput()
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
    void IntroSceneImpl::ScreenIntroProcessKeyboardInput()
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

    void IntroSceneImpl::ScreenIntroSkipPart()
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

    void IntroSceneImpl::ScreenIntroDrawLogo(RenderTarget& rt)
    {
        int32_t screenWidth = ContextGetWidth();
        int32_t imageWidth = 640;
        int32_t imageX = (screenWidth - imageWidth) / 2;

        DrawingEngineInvalidateImage(kSpriteRCT2Logo00);
        DrawingEngineInvalidateImage(kSpriteRCT2Logo10);
        DrawingEngineInvalidateImage(kSpriteRCT2Logo20);
        DrawingEngineInvalidateImage(kSpriteRCT2Logo01);
        DrawingEngineInvalidateImage(kSpriteRCT2Logo11);
        DrawingEngineInvalidateImage(kSpriteRCT2Logo21);

        GfxClear(rt, kBackgroundColourLogo);
        GfxDrawSprite(rt, ImageId(kSpriteRCT2Logo00), { imageX + 0, 0 });
        GfxDrawSprite(rt, ImageId(kSpriteRCT2Logo10), { imageX + 220, 0 });
        GfxDrawSprite(rt, ImageId(kSpriteRCT2Logo20), { imageX + 440, 0 });
        GfxDrawSprite(rt, ImageId(kSpriteRCT2Logo01), { imageX + 0, 240 });
        GfxDrawSprite(rt, ImageId(kSpriteRCT2Logo11), { imageX + 220, 240 });
        GfxDrawSprite(rt, ImageId(kSpriteRCT2Logo21), { imageX + 440, 240 });
    }

    IntroScene::IntroScene(IContext& context)
        : Scene(context)
        , _impl(std::make_shared<IntroSceneImpl>())
    {
        introSceneImplementation = _impl;
    }

    void IntroScene::Load()
    {
        _impl->Load();
    }

    void IntroScene::Stop()
    {
        _impl->Stop();
    }

    void IntroScene::Tick()
    {
        auto& sceneContext = GetContext();
        _impl->Tick(sceneContext);
    }
} // namespace OpenRCT2
