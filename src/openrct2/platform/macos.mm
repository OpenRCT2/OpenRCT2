#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>
#include <mach-o/dyld.h>
#include <pwd.h>
#include "platform.h"
#include "../util/Util.h"
#include "../localisation/Language.h"
#include "../config/Config.h"

void macos_disallow_automatic_window_tabbing()
{
    @autoreleasepool {
        if ([NSWindow respondsToSelector:@selector(setAllowsAutomaticWindowTabbing:)]) {
            [NSWindow setAllowsAutomaticWindowTabbing:NO];
        }
    }
}

utf8* macos_str_decomp_to_precomp(utf8 *input)
{
    @autoreleasepool
    {
        if (input == NULL) {
            return NULL;
        }

        NSString *inputDecomp = [NSString stringWithUTF8String:input];
        return strdup([inputDecomp.precomposedStringWithCanonicalMapping cStringUsingEncoding:NSUTF8StringEncoding]);
    }
}

#ifndef NO_TTF
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
#endif // NO_TTF

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

void platform_get_changelog_path(utf8 *outPath, size_t outSize)
{
    platform_get_openrct_data_path(outPath, outSize);
    safe_strcat_path(outPath, "changelog.txt", outSize);
}

bool platform_get_steam_path(utf8 * outPath, size_t outSize)
{
    char steamPath[1024] = { 0 };
    const char * homeDir = getpwuid(getuid())->pw_dir;
    if (homeDir != NULL)
    {
        safe_strcpy(steamPath, homeDir, sizeof(steamPath));
        safe_strcat_path(steamPath, "Library/Application Support/Steam/steamapps/common", sizeof(steamPath));
        if (platform_directory_exists(steamPath))
        {
            safe_strcpy(outPath, steamPath, outSize);
            return true;
        }
    }
    return false;
}

#endif
