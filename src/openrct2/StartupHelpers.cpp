/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "StartupHelpers.h"

#include "Context.h"
#include "FileClassifier.h"
#include "PlatformEnvironment.h"
#include "core/FileStream.h"
#include "core/Path.hpp"
#include "localisation/StringIds.h"
#include "ride/TrackDesign.h"
#include "ride/TrackDesignRepository.h"

namespace OpenRCT2
{
    StartupOpenResult loadStartupFile(IContext& context, IPlatformEnvironment& env, const u8string& originalPath)
    {
        auto stream = FileStream(originalPath, FileMode::open);
        ClassifiedFileInfo info;
        // May happen for .sea files. Autodetection needs to be fixed, but this will do for now.
        if (!TryClassifyFile(&stream, &info))
        {
            if (!context.LoadParkFromFile(originalPath, true))
                return { StartupOpenStatus::fileOpenFailure };

            return { StartupOpenStatus::loadedParkFile };
        }

        switch (info.Type)
        {
            case FileType::savedGame:
            case FileType::scenario:
            case FileType::park:
                if (!context.LoadParkFromStream(&stream, originalPath, true))
                    return { StartupOpenStatus::fileOpenFailure };

                return { StartupOpenStatus::loadedParkFile };

            case FileType::trackDesign:
            {
                // All the receiving functions currently expect a real file.
                // TODO: refactor these functions to take a stream.
                if (originalPath.empty())
                {
                    return { StartupOpenStatus::fileOpenFailure, STR_UNABLE_TO_LOAD_FILE };
                }

                auto trackDesign = TrackDesignImport(originalPath.c_str());
                if (trackDesign == nullptr)
                {
                    return { StartupOpenStatus::fileOpenFailure, STR_UNABLE_TO_LOAD_FILE };
                }

                auto destPath = env.GetDirectoryPath(DirBase::user, DirId::trackDesigns);
                auto trackName = GetNameFromTrackPath(originalPath);

                auto extension = trackDesignGetExtension(trackDesign->version);
                destPath = Path::Combine(destPath, trackName + extension);
                if (!Path::ResolveCasing(destPath).empty())
                {
                    return { StartupOpenStatus::installedContent, STR_TRACK_DESIGN_ALREADY_INSTALLED };
                }

                if (!TrackRepositoryInstall(originalPath, trackName))
                {
                    return { StartupOpenStatus::fileOpenFailure, STR_UNABLE_TO_INSTALL_THIS_TRACK_DESIGN };
                }

                return { StartupOpenStatus::installedContent, STR_TRACK_DESIGN_INSTALLED_SUCCESSFULLY };
            }
            case FileType::undefined:
            case FileType::object:
                return { StartupOpenStatus::fileOpenFailure };
        }

        return { StartupOpenStatus::fileOpenFailure };
    }

} // namespace OpenRCT2
