#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _VERSION_H_
#define _VERSION_H_

#include "common.h"

#define OPENRCT2_NAME				"OpenRCT2"
#define OPENRCT2_VERSION			"0.0.5"

#if defined(__LP64__) || defined(_WIN64)
	#define OPENRCT2_ARCHITECTURE		"x64"
#else
	#define OPENRCT2_ARCHITECTURE		"x86"
#endif

// Platform
#ifdef __WINDOWS__
	#define OPENRCT2_PLATFORM		"Windows"
#endif
#ifdef __LINUX__
	#define OPENRCT2_PLATFORM		"Linux"
#endif
#ifdef __MACOSX__
	#define OPENRCT2_PLATFORM		"macOS"
#endif
#ifndef OPENRCT2_PLATFORM
	#error Unknown platform!
#endif

#define OPENRCT2_TIMESTAMP			__DATE__ " " __TIME__

// The following constants are for automated build servers
extern const char *gBuildNumber;
extern const char *gBuildServer;
extern const char *gGitBranch;
extern const char *gCommitSha1;
extern const char *gCommitSha1Short;

#endif
