#ifndef _VERSION_H_
#define _VERSION_H_

#define OPENRCT2_NAME				"OpenRCT2"
#define OPENRCT2_VERSION			"0.0.4"
#define OPENRCT2_ARCHITECTURE		"x86"

// Platform
#ifdef _WIN32
	#define OPENRCT2_PLATFORM		"Windows"
#endif // _WIN32
#ifdef __linux__
	#define OPENRCT2_PLATFORM		"Linux"
#endif
#if defined(__APPLE__) && defined(__MACH__)
	#define OPENRCT2_PLATFORM		"OS X"
#endif
#ifndef OPENRCT2_PLATFORM
	#error Unknown platform!
#endif

#define OPENRCT2_TIMESTAMP			__DATE__ " " __TIME__

// The following constants are for automated build servers
#ifndef OPENRCT2_BUILD_NUMBER
	#define OPENRCT2_BUILD_NUMBER			""
#endif
#ifndef OPENRCT2_BUILD_SERVER
	#define OPENRCT2_BUILD_SERVER			""
#endif
#ifndef OPENRCT2_BRANCH
	#define OPENRCT2_BRANCH					"develop"
#endif
#ifndef OPENRCT2_COMMIT_SHA1
	#define OPENRCT2_COMMIT_SHA1			""
#endif
#ifndef OPENRCT2_COMMIT_SHA1_SHORT
	#define OPENRCT2_COMMIT_SHA1_SHORT		""
#endif

#endif
