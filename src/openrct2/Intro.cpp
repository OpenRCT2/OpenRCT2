/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Intro.h"

#include "Context.h"
#include "audio/AudioChannel.h"
#include "audio/AudioMixer.h"
#include "audio/audio.h"
#include "drawing/Drawing.h"
#include "sprites.h"

#include <memory>

using namespace OpenRCT2::Audio;

#define BACKROUND_COLOUR_DARK PALETTE_INDEX_10
#define BACKROUND_COLOUR_LOGO PALETTE_INDEX_245
#define BORDER_COLOUR_PUBLISHER PALETTE_INDEX_129

constexpr int32_t PALETTE_G1_IDX_DEVELOPER = 23217;
constexpr int32_t PALETTE_G1_IDX_LOGO = 23224;

IntroState gIntroState;

// Used mainly for timing but also for Y coordinate and fading.
static int32_t _introStateCounter;

static std::shared_ptr<IAudioChannel> _soundChannel = nullptr;
static bool _chainLiftFinished;

static void screen_intro_process_mouse_input();
static void screen_intro_process_keyboard_input();
static void screen_intro_skip_part();
static void screen_intro_draw_logo(rct_drawpixelinfo* dpi);

// rct2: 0x0068E966
void intro_update()
{
    screen_intro_process_mouse_input();
    screen_intro_process_keyboard_input();

    switch (gIntroState)
    {
        case IntroState::Disclaimer1:
        case IntroState::Disclaimer2:
            // Originally used for the disclaimer text
            gIntroState = IntroState::PublisherBegin;
            [[fallthrough]];
        case IntroState::PublisherBegin:
            LoadPalette();

            // Set the Y for the Infogrames logo
            _introStateCounter = -580;

            // Play the chain lift sound
            _soundChannel = CreateAudioChannel(SoundId::LiftBM, true);
            _chainLiftFinished = false;
            gIntroState = IntroState::PublisherScroll;
            break;
        case IntroState::PublisherScroll:
            // Move the Infogrames logo down
            _introStateCounter += 5;

            // Check if logo is off the screen...ish
            if (_introStateCounter > ContextGetHeight() - 120)
            {
                _introStateCounter = -116;
                gIntroState = IntroState::DeveloperBegin;
            }

            break;
        case IntroState::DeveloperBegin:
            // Set the Y for the Chris Sawyer logo
            _introStateCounter = -116;

            gIntroState = IntroState::DeveloperScroll;
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
                _soundChannel = CreateAudioChannel(SoundId::TrackFrictionBM, true, MIXER_VOLUME_MAX, 0.25f, 0.75);
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
                _soundChannel = CreateAudioChannel(SoundId::Scream1);

                gIntroState = IntroState::LogoFadeIn;
                _introStateCounter = 0;
            }
            break;
        case IntroState::LogoFadeIn:
            // Fade in, add 4 / 256 to fading
            _introStateCounter += 0x400;
            if (_introStateCounter > 0xFF00)
            {
                gIntroState = IntroState::LogoWait;
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

                gIntroState = IntroState::LogoFadeOut;
            }
            break;
        case IntroState::LogoFadeOut:
            // Fade out, subtract 4 / 256 from fading
            _introStateCounter -= 0x400;
            if (_introStateCounter < 0)
            {
                gIntroState = IntroState::Clear;
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
            gIntroState = IntroState::Finish;
            _introStateCounter = 0;
            break;
        case IntroState::Finish:
            gIntroState = IntroState::None;
            LoadPalette();
            OpenRCT2::Audio::PlayTitleMusic();
            break;
        default:
            break;
    }
}

void intro_draw(rct_drawpixelinfo* dpi)
{
    int32_t screenWidth = ContextGetWidth();

    switch (gIntroState)
    {
        case IntroState::Disclaimer1:
        case IntroState::Disclaimer2:
            break;
        case IntroState::PublisherBegin:
            GfxClear(dpi, BACKROUND_COLOUR_DARK);
            break;
        case IntroState::PublisherScroll:
            GfxClear(dpi, BACKROUND_COLOUR_DARK);

            // Draw a white rectangle for the logo background (gives a bit of white margin)
            GfxFillRect(
                dpi,
                { { (screenWidth / 2) - 320 + 50, _introStateCounter + 50 },
                  { (screenWidth / 2) - 320 + 50 + 540, _introStateCounter + 50 + 425 } },
                BORDER_COLOUR_PUBLISHER);

            // Draw Infogrames logo
            gfx_draw_sprite(dpi, ImageId(SPR_INTRO_INFOGRAMES_00), { (screenWidth / 2) - 320 + 69, _introStateCounter + 69 });
            gfx_draw_sprite(dpi, ImageId(SPR_INTRO_INFOGRAMES_10), { (screenWidth / 2) - 320 + 319, _introStateCounter + 69 });
            gfx_draw_sprite(dpi, ImageId(SPR_INTRO_INFOGRAMES_01), { (screenWidth / 2) - 320 + 69, _introStateCounter + 319 });
            gfx_draw_sprite(dpi, ImageId(SPR_INTRO_INFOGRAMES_11), { (screenWidth / 2) - 320 + 319, _introStateCounter + 319 });
            break;
        case IntroState::DeveloperBegin:
            GfxClear(dpi, BACKROUND_COLOUR_DARK);
            GfxTransposePalette(PALETTE_G1_IDX_DEVELOPER, 255);
            break;
        case IntroState::DeveloperScroll:
            GfxClear(dpi, BACKROUND_COLOUR_DARK);

            // Draw Chris Sawyer logo
            gfx_draw_sprite(dpi, ImageId(SPR_INTRO_CHRIS_SAWYER_00), { (screenWidth / 2) - 320 + 70, _introStateCounter });
            gfx_draw_sprite(dpi, ImageId(SPR_INTRO_CHRIS_SAWYER_10), { (screenWidth / 2) - 320 + 320, _introStateCounter });
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
            screen_intro_draw_logo(dpi);
            break;
        case IntroState::LogoWait:
            screen_intro_draw_logo(dpi);
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
            screen_intro_draw_logo(dpi);
            break;
        case IntroState::Clear:
            GfxClear(dpi, BACKROUND_COLOUR_DARK);
            break;
        default:
            break;
    }
}

static void screen_intro_process_mouse_input()
{
    if (ContextGetCursorState()->any == CURSOR_PRESSED)
    {
        screen_intro_skip_part();
    }
}

/**
 *
 *  rct2: 0x006E3AEC
 */
static void screen_intro_process_keyboard_input()
{
    const uint8_t* keys = ContextGetKeysState();
    for (int i = 0; i < 256; i++)
    {
        if (keys[i] != 0)
        {
            screen_intro_skip_part();
            break;
        }
    }
}

static void screen_intro_skip_part()
{
    switch (gIntroState)
    {
        case IntroState::None:
            break;
        case IntroState::Disclaimer2:
            gIntroState = IntroState::PublisherBegin;
            break;
        default:
            gIntroState = IntroState::Clear;
            break;
    }
}

static void screen_intro_draw_logo(rct_drawpixelinfo* dpi)
{
    int32_t screenWidth = ContextGetWidth();
    int32_t imageWidth = 640;
    int32_t imageX = (screenWidth - imageWidth) / 2;

    drawing_engine_invalidate_image(SPR_INTRO_LOGO_00);
    drawing_engine_invalidate_image(SPR_INTRO_LOGO_10);
    drawing_engine_invalidate_image(SPR_INTRO_LOGO_20);
    drawing_engine_invalidate_image(SPR_INTRO_LOGO_01);
    drawing_engine_invalidate_image(SPR_INTRO_LOGO_11);
    drawing_engine_invalidate_image(SPR_INTRO_LOGO_21);

    GfxClear(dpi, BACKROUND_COLOUR_LOGO);
    gfx_draw_sprite(dpi, ImageId(SPR_INTRO_LOGO_00), { imageX + 0, 0 });
    gfx_draw_sprite(dpi, ImageId(SPR_INTRO_LOGO_10), { imageX + 220, 0 });
    gfx_draw_sprite(dpi, ImageId(SPR_INTRO_LOGO_20), { imageX + 440, 0 });
    gfx_draw_sprite(dpi, ImageId(SPR_INTRO_LOGO_01), { imageX + 0, 240 });
    gfx_draw_sprite(dpi, ImageId(SPR_INTRO_LOGO_11), { imageX + 220, 240 });
    gfx_draw_sprite(dpi, ImageId(SPR_INTRO_LOGO_21), { imageX + 440, 240 });
}
