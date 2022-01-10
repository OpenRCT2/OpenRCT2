/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__APPLE__) && defined(__MACH__)

#    include "../config/Config.h"
#    include "../core/Path.hpp"
#    include "../localisation/Language.h"
#    include "../util/Util.h"
#    include "platform.h"

// undefine `interface` and `abstract`, because it's causing conflicts with Objective-C's keywords
#    undef interface
#    undef abstract

#    import <AppKit/AppKit.h>
#    import <Foundation/Foundation.h>
#    include <mach-o/dyld.h>
#    include <pwd.h>

#endif
