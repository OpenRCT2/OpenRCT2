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

#include <stdio.h>

#include "../common.h"

#if defined(__WINDOWS__)
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
#endif

#if defined(__unix__) || defined(__MACOSX__)
	#include <sys/mman.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <unistd.h>
#endif // defined(__unix__) || defined(__MACOSX__)

#include "../OpenRCT2.h"
#include "../util/sawyercoding.h"
#include "../util/util.h"
#include "addresses.h"
#include "hook.h"
#include "interop.h"

#if defined(USE_MMAP) && (defined(__unix__) || defined(__MACOSX__)) && !defined(NO_RCT2)
	static sint32 fdData = -1;
#endif
#if !defined(NO_RCT2)
	static char * segments = (char *)(GOOD_PLACE_FOR_DATA_SEGMENT);
#endif

static void rct2_interop_get_segment_data_path(char * buffer, size_t bufferSize)
{
	platform_get_exe_path(buffer, bufferSize);
	safe_strcat_path(buffer, "openrct2_data", bufferSize);
}

/**
 * Loads RCT2's data model and remaps the addresses.
 * @returns true if the data integrity check succeeded, otherwise false.
 */
bool rct2_interop_setup_segment()
{
	// OpenRCT2 on Linux and macOS is wired to have the original Windows PE sections loaded
	// necessary. Windows does not need to do this as OpenRCT2 runs as a DLL loaded from the Windows PE.
	sint32 len = 0x01429000 - 0x8a4000; // 0xB85000, 12079104 bytes or around 11.5MB
	sint32 err = 0;
	// in some configurations err and len may be unused
	UNUSED(err);
	UNUSED(len);
#if !defined(NO_RCT2)
	UNUSED(segments);
#endif
#if defined(USE_MMAP) && (defined(__unix__) || defined(__MACOSX__)) && !defined(NO_RCT2)
	#define RDATA_OFFSET 0x004A4000
	#define DATASEG_OFFSET 0x005E2000

	// Using PE-bear I was able to figure out all the needed addresses to be filled.
	// There are three sections to be loaded: .rdata, .data and .text, plus another
	// one to be mapped: DATASEG.
	// Out of the three, two can simply be mmapped into memory, while the third one,
	// .data has a virtual size which is much completely different to its file size
	// (even when taking page-alignment into consideration)
	//
	// The sections are as follows (dump from gdb)
	// [0]     0x401000->0x6f7000 at 0x00001000: .text ALLOC LOAD READONLY CODE HAS_CONTENTS
	// [1]     0x6f7000->0x8a325d at 0x002f7000: CODESEG ALLOC LOAD READONLY CODE HAS_CONTENTS
	// [2]     0x8a4000->0x9a5894 at 0x004a4000: .rdata ALLOC LOAD DATA HAS_CONTENTS
	// [3]     0x9a6000->0x9e2000 at 0x005a6000: .data ALLOC LOAD DATA HAS_CONTENTS
	// [4]     0x1428000->0x14282bc at 0x005e2000: DATASEG ALLOC LOAD DATA HAS_CONTENTS
	// [5]     0x1429000->0x1452000 at 0x005e3000: .cms_t ALLOC LOAD READONLY CODE HAS_CONTENTS
	// [6]     0x1452000->0x14aaf3e at 0x0060c000: .cms_d ALLOC LOAD DATA HAS_CONTENTS
	// [7]     0x14ab000->0x14ac58a at 0x00665000: .idata ALLOC LOAD READONLY DATA HAS_CONTENTS
	// [8]     0x14ad000->0x14b512f at 0x00667000: .rsrc ALLOC LOAD DATA HAS_CONTENTS
	//
	// .data section, however, has virtual size of 0xA81C3C, and so
	// 0x9a6000 + 0xA81C3C = 0x1427C3C, which after alignment to page size becomes
	// 0x1428000, which can be seen as next section, DATASEG
	//
	// The data is now loaded into memory with a linker script, which proves to
	// be more reliable, as mallocs that happen before we reach segment setup
	// could have already taken the space we need.

	// TODO: UGLY, UGLY HACK!
	//off_t file_size = 6750208;

	utf8 segmentDataPath[MAX_PATH];
	rct2_interop_get_segment_data_path(segmentDataPath, sizeof(segmentDataPath));
	fdData = open(segmentDataPath, O_RDONLY);
	if (fdData < 0)
	{
		log_fatal("failed to load openrct2_data");
		exit(1);
	}
	log_warning("%p", GOOD_PLACE_FOR_DATA_SEGMENT);
	segments = mmap((void *)(GOOD_PLACE_FOR_DATA_SEGMENT), len, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE, fdData, 0);
	log_warning("%p", segments);
	if ((uintptr_t)segments != GOOD_PLACE_FOR_DATA_SEGMENT) {
		perror("mmap");
		return false;
	}
#endif // defined(USE_MMAP) && (defined(__unix__) || defined(__MACOSX__))

#if defined(__unix__) && !defined(NO_RCT2)
	sint32 pageSize = getpagesize();
	sint32 numPages = (len + pageSize - 1) / pageSize;
	uint8 *dummy = malloc(numPages);

	err = mincore((void *)segments, len, dummy);
	bool pagesMissing = false;
	if (err != 0)
	{
		err = errno;
#ifdef __LINUX__
		// On Linux ENOMEM means all requested range is unmapped
		if (err != ENOMEM)
		{
			pagesMissing = true;
			perror("mincore");
		}
#else
		pagesMissing = true;
		perror("mincore");
#endif // __LINUX__
	} else {
		for (sint32 i = 0; i < numPages; i++)
		{
			if (dummy[i] != 1)
			{
				pagesMissing = true;
				void *start = (void *)segments + i * pageSize;
				void *end = (void *)segments + (i + 1) * pageSize - 1;
				log_warning("required page %p - %p is not in memory!", start, end);
			}
		}
	}
	free(dummy);
	if (pagesMissing)
	{
		log_error("At least one of required pages was not found in memory. This can cause segfaults later on.");
	}
#if !defined(USE_MMAP)
	// section: text
	err = mprotect((void *)0x401000, 0x8a4000 - 0x401000, PROT_READ | PROT_EXEC | PROT_WRITE);
	if (err != 0)
	{
		perror("mprotect");
	}
#endif // !defined(USE_MMAP)
	// section: rw data
	err = mprotect((void *)segments, 0x01429000 - 0x8a4000, PROT_READ | PROT_WRITE);
	if (err != 0)
	{
		perror("mprotect");
	}
#endif // defined(__unix__)

#if defined(USE_MMAP) && defined(__WINDOWS__)
	segments = VirtualAlloc((void *)(GOOD_PLACE_FOR_DATA_SEGMENT), len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if ((uintptr_t)segments != GOOD_PLACE_FOR_DATA_SEGMENT) {
		log_error("VirtualAlloc, segments = %p, GetLastError = 0x%x", segments, GetLastError());
		return false;
	}

	utf8 segmentDataPath[MAX_PATH];
	rct2_interop_get_segment_data_path(segmentDataPath, sizeof(segmentDataPath));

	// Warning: for Windows this will fail if given a non-ASCII path,
	//          but given this code is temporary - its not worth resolving.
	FILE * file = fopen(segmentDataPath, "rb");
	if (file == NULL)
	{
		log_error("failed to load file");
		return false;
	}
	if (fread(segments, len, 1, file) != 1) {
		log_error("Unable to read chunk header!");
		return false;
	}
	fclose(file);
#endif // defined(USE_MMAP) && defined(__WINDOWS__)

#if !defined(NO_RCT2) && defined(USE_MMAP)
	// Check that the expected data is at various addresses.
	// Start at 0x9a6000, which is start of .data, to skip the region containing addresses to DLL
	// calls, which can be changed by windows/wine loader.
	const uint32 c1 = sawyercoding_calculate_checksum((const uint8*)(segments + (uintptr_t)(0x009A6000 - 0x8a4000)), 0x009E0000 - 0x009A6000);
	const uint32 c2 = sawyercoding_calculate_checksum((const uint8*)(segments + (uintptr_t)(0x01428000 - 0x8a4000)), 0x014282BC - 0x01428000);
	const uint32 exp_c1 = 10114815;
	const uint32 exp_c2 = 23564;
	if (c1 != exp_c1 || c2 != exp_c2) {
		log_warning("c1 = %u, expected %u, match %d", c1, exp_c1, c1 == exp_c1);
		log_warning("c2 = %u, expected %u, match %d", c2, exp_c2, c2 == exp_c2);
		return false;
	}
#endif
	return true;
}

/**
 * Setup hooks to allow RCT2 to call OpenRCT2 functions instead.
 */
void rct2_interop_setup_hooks()
{
	// None for now
}

void rct2_interop_dispose()
{
#if defined(USE_MMAP) && (defined(__unix__) || defined(__MACOSX__)) && !defined(NO_RCT2)
	munmap(segments, 12079104);
	close(fdData);
#endif
}
