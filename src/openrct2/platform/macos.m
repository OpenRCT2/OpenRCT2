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

#if defined(__APPLE__) && defined(__MACH__)

@import AppKit;
@import Foundation;
#include <mach-o/dyld.h>
#include "platform.h"
#include "../util/util.h"
#include "../localisation/date.h"
#include "../localisation/language.h"
#include "../config/Config.h"

void macos_disallow_automatic_window_tabbing()
{
	@autoreleasepool {
		if ([NSWindow respondsToSelector:@selector(setAllowsAutomaticWindowTabbing:)]) {
			[NSWindow setAllowsAutomaticWindowTabbing:NO];
		}
	}
}

bool platform_check_steam_overlay_attached() {
	STUB();
	return false;
}

void platform_get_exe_path(utf8 *outPath, size_t outSize)
{
	if (outSize == 0) return;
	char exePath[MAX_PATH];
	uint32_t size = MAX_PATH;
	int result = _NSGetExecutablePath(exePath, &size);
	if (result != 0) {
		log_fatal("failed to get path");
	}
	exePath[MAX_PATH - 1] = '\0';
	char *exeDelimiter = strrchr(exePath, *PATH_SEPARATOR);
	if (exeDelimiter == NULL)
	{
		log_error("should never happen here");
		outPath[0] = '\0';
		return;
	}
	*exeDelimiter = '\0';

	safe_strcpy(outPath, exePath, outSize);
}

/**
 * Default directory fallback is:
 *   - (command line argument)
 *   - ~/Library/Application Support/OpenRCT2
 */
void platform_posix_sub_user_data_path(char *buffer, size_t size, const char *homedir) {
	if (homedir == NULL)
	{
		log_fatal("Couldn't find user data directory");
		exit(-1);
		return;
	}

	safe_strcpy(buffer, homedir, size);
	safe_strcat_path(buffer, "Library", size);
	safe_strcat_path(buffer, "Application Support", size);
	safe_strcat_path(buffer, "OpenRCT2", size);
	path_end_with_separator(buffer, size);
}

/**
 * Default directory fallback is:
 *   - (command line argument)
 *   - <exePath>/data
 *   - <Resources Folder>
 */
void platform_posix_sub_resolve_openrct_data_path(utf8 *out, size_t size) {
	@autoreleasepool
	{
		NSBundle *bundle = [NSBundle mainBundle];
		if (bundle)
		{
			const utf8 *resources = bundle.resourcePath.UTF8String;
			if (platform_directory_exists(resources))
			{
				out[0] = '\0';
				safe_strcpy(out, resources, size);
				return;
			}
		}
	}
}

void platform_show_messagebox(const char * message)
{
	@autoreleasepool
	{
		NSAlert *alert = [[[NSAlert alloc] init] autorelease];
		[alert addButtonWithTitle:@"OK"];
		alert.messageText = [NSString stringWithUTF8String:message];
		[alert runModal];
	}
}

utf8 *platform_open_directory_browser(const utf8 *title)
{
	@autoreleasepool
	{
		NSOpenPanel *panel = [NSOpenPanel openPanel];
		panel.canChooseFiles = false;
		panel.canChooseDirectories = true;
		panel.allowsMultipleSelection = false;
		utf8 *url = NULL;
		if ([panel runModal] == NSFileHandlingPanelOKButton)
		{
			NSString *selectedPath = panel.URL.path;
			const char *path = selectedPath.UTF8String;
			url = _strdup(path);
		}
		return url;
	}
}

bool platform_open_common_file_dialog(utf8 *outFilename, file_dialog_desc *desc, size_t outSize) {
	@autoreleasepool
	{
		NSMutableArray *extensions = [NSMutableArray new];
		for (int i=0; i < countof(desc->filters); ++i) {
			if (desc->filters[i].pattern != NULL) {
				NSString *fp = [NSString stringWithUTF8String:desc->filters[i].pattern];
				fp = [fp stringByReplacingOccurrencesOfString:@"*." withString:@""];
				[extensions addObjectsFromArray:[fp componentsSeparatedByString:@";"]];
			}
		}

		NSString *directory;
		NSSavePanel *panel;
		if (desc->type == FD_SAVE)
		{
			NSString *filePath = [NSString stringWithUTF8String:desc->default_filename];
			directory = filePath.stringByDeletingLastPathComponent;
			NSString *basename = filePath.lastPathComponent;
			panel = [NSSavePanel savePanel];
			panel.nameFieldStringValue = [NSString stringWithFormat:@"%@.%@", basename, extensions.firstObject];
		}
		else if (desc->type == FD_OPEN)
		{
			directory = [NSString stringWithUTF8String:desc->initial_directory];
			NSOpenPanel *open = [NSOpenPanel openPanel];
			open.canChooseDirectories = false;
			open.canChooseFiles = true;
			open.allowsMultipleSelection = false;
			panel = open;
		} else {
			return false;
		}

		panel.title = [NSString stringWithUTF8String:desc->title];
		panel.allowedFileTypes = extensions;
		panel.directoryURL = [NSURL fileURLWithPath:directory];
		if ([panel runModal] == NSFileHandlingPanelCancelButton){
			SDL_RaiseWindow(gWindow);
			return false;
		} else {
			safe_strcpy(outFilename, panel.URL.path.UTF8String, outSize);
			SDL_RaiseWindow(gWindow);
			return true;
		}
	}
}

bool platform_get_font_path(TTFFontDescriptor *font, utf8 *buffer, size_t size)
{
	@autoreleasepool
	{
		CTFontDescriptorRef fontRef = CTFontDescriptorCreateWithNameAndSize((CFStringRef)[NSString stringWithUTF8String:font->font_name], 0.0);
		CFURLRef url = (CFURLRef)CTFontDescriptorCopyAttribute(fontRef, kCTFontURLAttribute);
		if (url) {
			NSString *fontPath = [NSString stringWithString:[(NSURL *)CFBridgingRelease(url) path]];
			safe_strcpy(buffer, fontPath.UTF8String, size);
			return true;
		} else {
			return false;
		}
	}
}

bool platform_has_matching_language(NSString *preferredLocale, uint16* languageIdentifier)
{
	@autoreleasepool
	{
		if ([preferredLocale isEqualToString:@"en"] || [preferredLocale isEqualToString:@"en-CA"]) {
			*languageIdentifier = LANGUAGE_ENGLISH_US;
			return YES;
		}

		if ([preferredLocale isEqualToString:@"zh-CN"]) {
			*languageIdentifier = LANGUAGE_CHINESE_SIMPLIFIED;
			return YES;
		}

		if ([preferredLocale isEqualToString:@"zh-TW"]) {
			*languageIdentifier = LANGUAGE_CHINESE_TRADITIONAL;
			return YES;
		}

		// Find an exact match (language and region)
		for (int i = 1; i < LANGUAGE_COUNT; i++) {
			if([preferredLocale isEqualToString:[NSString stringWithUTF8String:LanguagesDescriptors[i].locale]]) {
				*languageIdentifier = i;
				return YES;
			}
		}


		// Only check for a matching language
		NSString *languageCode = [[preferredLocale componentsSeparatedByString:@"-"] firstObject];
		for (int i = 1; i < LANGUAGE_COUNT; i++) {
			NSString *optionLanguageCode = [[[NSString stringWithUTF8String:LanguagesDescriptors[i].locale] componentsSeparatedByString:@"-"] firstObject];
			if([languageCode isEqualToString:optionLanguageCode]) {
				*languageIdentifier = i;
				return YES;
			}
		}

		return NO;
	}
}

uint16 platform_get_locale_language()
{
	@autoreleasepool
	{
		NSArray<NSString*> *preferredLanguages = [NSLocale preferredLanguages];
		for (NSString *preferredLanguage in preferredLanguages) {
			uint16 languageIdentifier;
			if (platform_has_matching_language(preferredLanguage, &languageIdentifier)) {
				return languageIdentifier;
			}
		}

		// Fallback
		return LANGUAGE_ENGLISH_UK;
	}
}

uint8 platform_get_locale_currency()
{
	@autoreleasepool
	{
		NSString *currencyCode = [[NSLocale currentLocale] objectForKey:NSLocaleCurrencyCode];
		return platform_get_currency_value(currencyCode.UTF8String);
	}
}

uint8 platform_get_locale_measurement_format()
{
	@autoreleasepool
	{
		NSNumber *metricSystem = [[NSLocale currentLocale] objectForKey:NSLocaleUsesMetricSystem];

		if (metricSystem.boolValue) {
			return MEASUREMENT_FORMAT_METRIC;
		}

		return MEASUREMENT_FORMAT_IMPERIAL;
	}
}

uint8 platform_get_locale_date_format()
{
	// TODO Stub implementation
	return DATE_FORMAT_DAY_MONTH_YEAR;
}

sint32 platform_get_non_window_flags()
{
	return SDL_WINDOW_MINIMIZED | SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP;
}

#endif
