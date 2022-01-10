/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../common.h"

// Despite the name, this file contains support for more OSs besides Linux, provided the necessary ifdefs remain small.
// Otherwise, they should be spun off into their own files.
#if defined(__unix__) && !defined(__ANDROID__) && !defined(__APPLE__)

#    ifdef __FreeBSD__
#        include <sys/sysctl.h>
#    endif

#    include <cstring>
#    include <ctype.h>
#    include <dlfcn.h>
#    include <errno.h>
#    ifndef NO_TTF
#        include <fontconfig/fontconfig.h>
#    endif // NO_TTF
#    include "../config/Config.h"
#    include "../core/File.h"
#    include "../core/Path.hpp"
#    include "../localisation/Language.h"
#    include "../localisation/StringIds.h"
#    include "../util/Util.h"
#    include "platform.h"

#    include <pwd.h>

#endif
