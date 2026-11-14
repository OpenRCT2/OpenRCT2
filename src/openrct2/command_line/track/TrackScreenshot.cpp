/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../Context.h"
#include "../../GameState.h"
#include "../../OpenRCT2.h"
#include "../../core/Console.hpp"
#include "../../core/Imaging.h"
#include "../../drawing/NewDrawing.h"
#include "../../drawing/Palette.h"
#include "../../ride/TrackDesign.h"
#include "../CommandLine.hpp"
#include "TrackCommands.h"

namespace OpenRCT2::CommandLine::Track
{
    ExitCode screenshot(CommandLineArgEnumerator* argEnumerator)
    {
        bool missingArguments = false;

        const char* inputPath;
        if (!argEnumerator->TryPopString(&inputPath))
        {
            Console::Error::WriteLine("Expected a track design file path.");
            missingArguments = true;
        }

        const char* outputPath;
        if (!argEnumerator->TryPopString(&outputPath))
        {
            Console::Error::WriteLine("Expected an output image path.");
            missingArguments = true;
        }

        if (missingArguments)
            return ExitCode::fail;

        gOpenRCT2Headless = true;
        auto context = CreateContext();
        if (!context->Initialise())
        {
            throw std::runtime_error("Failed to initialize context.");
        }

        DrawingEngineInit();

        gLegacyScene = LegacyScene::playing;
        getGameState().cheats.ignoreResearchStatus = true;
        Drawing::LoadPalette();

        std::unique_ptr<TrackDesign> trackDesign = TrackDesignImport(inputPath);

        TrackDesignPreviewBuffer trackDesignPreviewPixels{};
        TrackDesignDrawPreview(*trackDesign, trackDesignPreviewPixels, true, true);

        // The Track List window defaults to this rotation
        Direction rotation = kTrackPreviewDefaultDirection;
        auto const pixels8 = reinterpret_cast<uint8_t*>(trackDesignPreviewPixels.data() + (rotation * kTrackPreviewImageSize));

        Image image;
        image.Width = kTrackPreviewWidth;
        image.Height = kTrackPreviewHeight;
        image.Depth = 8;
        image.Stride = kTrackPreviewWidth * 1;
        image.Palette = Drawing::gPalette;
        image.Pixels = std::vector<uint8_t>(pixels8, pixels8 + kTrackPreviewImageSize);
        Imaging::WriteToFile(outputPath, image, ImageFormat::png);

        return ExitCode::ok;
    }
} // namespace OpenRCT2::CommandLine::Track
