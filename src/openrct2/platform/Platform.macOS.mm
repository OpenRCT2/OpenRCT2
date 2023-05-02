/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__APPLE__) && defined(__MACH__)

#    include "Platform.h"

#    include "../Date.h"
#    include "../OpenRCT2.h"
#    include "../core/Path.hpp"
#    include "../core/String.hpp"
#    include "../localisation/Language.h"

// undefine `interface` and `abstract`, because it's causing conflicts with Objective-C's keywords
#    undef interface
#    undef abstract

#    include <AvailabilityMacros.h>
#    include <CoreText/CoreText.h>
#    include <Foundation/Foundation.h>
#    include <mach-o/dyld.h>
#    include <mach/mach_time.h>
#    include <pwd.h>

namespace Platform
{
    std::string GetFolderPath(SPECIAL_FOLDER folder)
    {
        // macOS stores everything in ~/Library/Application Support/OpenRCT2
        switch (folder)
        {
            case SPECIAL_FOLDER::USER_CACHE:
            case SPECIAL_FOLDER::USER_CONFIG:
            case SPECIAL_FOLDER::USER_DATA:
            {
                auto home = GetFolderPath(SPECIAL_FOLDER::USER_HOME);
                return Path::Combine(home, "Library/Application Support");
            }
            case SPECIAL_FOLDER::USER_HOME:
                return GetHomePath();
            default:
                return std::string();
        }
    }

    static std::string GetBundlePath()
    {
        @autoreleasepool
        {
            NSBundle* bundle = [NSBundle mainBundle];
            if (bundle)
            {
                // This method may return a valid bundle object even for unbundled apps.
                // See https://developer.apple.com/documentation/foundation/nsbundle/1410786-mainbundle?language=objc
                // Therefore, double check this is a valid bundle (has an ID string)
                auto bundleId = bundle.bundleIdentifier.UTF8String;
                if (bundleId)
                {
                    auto resources = bundle.resourcePath.UTF8String;
                    if (Path::DirectoryExists(resources))
                    {
                        return resources;
                    }
                }
            }
            return std::string();
        }
    }

    std::string GetDocsPath()
    {
        return GetBundlePath();
    }

    std::string GetInstallPath()
    {
        auto path = std::string(gCustomOpenRCT2DataPath);
        if (!path.empty())
        {
            path = Path::GetAbsolute(path);
            return path;
        }
        else
        {
            // check if this is an app bundle
            path = GetBundlePath();
            if (!path.empty())
            {
                return path;
            }
            else
            {
                // this is not in an app bundle
                auto exePath = GetCurrentExecutablePath();
                auto exeDirectory = Path::GetDirectory(exePath);

                // check build and install paths
                NSArray *dataSearchLocations = @[@"data", @"../share/openrct2"];

                for (NSString *searchLocation in dataSearchLocations) {
                    path = Path::Combine(exeDirectory, [searchLocation UTF8String]);
                    NSString* nsPath = [NSString stringWithUTF8String:path.c_str()];
                    if ([[NSFileManager defaultManager] fileExistsAtPath:nsPath])
                    {
                        return path;
                    }
                }
            }
        }
        return "/";
    }

    std::string GetCurrentExecutablePath()
    {
        char exePath[MAX_PATH];
        uint32_t size = MAX_PATH;
        int result = _NSGetExecutablePath(exePath, &size);
        if (result == 0)
        {
            return exePath;
        }
        else
        {
            return std::string();
        }
    }

    u8string StrDecompToPrecomp(u8string_view input)
    {
        @autoreleasepool
        {
            auto cppString = u8string(input);
            NSString* inputDecomp = [NSString stringWithUTF8String:cppString.c_str()];
            return u8string([inputDecomp.precomposedStringWithCanonicalMapping cStringUsingEncoding:NSUTF8StringEncoding]);
        }
    }

    bool HandleSpecialCommandLineArgument(const char* argument)
    {
        if (String::Equals(argument, "-NSDocumentRevisionsDebugMode"))
        {
            return true;
        }
        if (String::StartsWith(argument, "-psn_"))
        {
            return true;
        }
        return false;
    }

    bool HasMatchingLanguage(NSString* preferredLocale, uint16_t* languageIdentifier)
    {
        @autoreleasepool
        {
            if ([preferredLocale isEqualToString:@"en"] || [preferredLocale isEqualToString:@"en-CA"])
            {
                *languageIdentifier = LANGUAGE_ENGLISH_US;
                return YES;
            }

            // Find an exact match (language and region)
            for (int i = 1; i < LANGUAGE_COUNT; i++)
            {
                if ([preferredLocale isEqualToString:[NSString stringWithUTF8String:LanguagesDescriptors[i].locale]])
                {
                    *languageIdentifier = i;
                    return YES;
                }
            }

            // Only check for a matching language
            NSString* languageCode = [[preferredLocale componentsSeparatedByString:@"-"] firstObject];
            for (int i = 1; i < LANGUAGE_COUNT; i++)
            {
                NSString* optionLanguageCode = [[[NSString stringWithUTF8String:LanguagesDescriptors[i].locale]
                    componentsSeparatedByString:@"-"] firstObject];
                if ([languageCode isEqualToString:optionLanguageCode])
                {
                    *languageIdentifier = i;
                    return YES;
                }
            }

            return NO;
        }
    }

    uint16_t GetLocaleLanguage()
    {
        @autoreleasepool
        {
            NSArray<NSString*>* preferredLanguages = [NSLocale preferredLanguages];
            for (NSString* preferredLanguage in preferredLanguages)
            {
                uint16_t languageIdentifier;
                if (HasMatchingLanguage(preferredLanguage, &languageIdentifier))
                {
                    return languageIdentifier;
                }
            }

            // Fallback
            return LANGUAGE_ENGLISH_UK;
        }
    }

    CurrencyType GetLocaleCurrency()
    {
        @autoreleasepool
        {
            NSString* currencyCode = [[NSLocale currentLocale] objectForKey:NSLocaleCurrencyCode];
            return Platform::GetCurrencyValue(currencyCode.UTF8String);
        }
    }

    MeasurementFormat GetLocaleMeasurementFormat()
    {
        @autoreleasepool
        {
            NSNumber* metricSystem = [[NSLocale currentLocale] objectForKey:NSLocaleUsesMetricSystem];

            if (metricSystem.boolValue)
            {
                return MeasurementFormat::Metric;
            }

            return MeasurementFormat::Imperial;
        }
    }

    std::string GetSteamPath()
    {
        const char* homeDir = getpwuid(getuid())->pw_dir;
        if (homeDir == nullptr)
        {
            return {};
        }

        auto steamPath = Path::Combine(
            homeDir, "Library/Application Support/Steam/Steam.AppBundle/Steam/Contents/MacOS/steamapps");
        if (Path::DirectoryExists(steamPath))
        {
            return steamPath;
        }

        return {};
    }

    std::string GetFontPath(const TTFFontDescriptor& font)
    {
        @autoreleasepool
        {
            CTFontDescriptorRef fontRef = CTFontDescriptorCreateWithNameAndSize(
                static_cast<CFStringRef>([NSString stringWithUTF8String:font.font_name]), 0.0);
            CFURLRef url = static_cast<CFURLRef>(CTFontDescriptorCopyAttribute(fontRef, kCTFontURLAttribute));
            if (url)
            {
                NSString* fontPath = [NSString stringWithString:[static_cast<NSURL*>(CFBridgingRelease(url)) path]];
                return fontPath.UTF8String;
            }
            else
            {
                return {};
            }
        }
    }
}

#endif
