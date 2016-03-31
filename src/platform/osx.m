/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * This file is part of OpenRCT2.
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#if defined(__APPLE__) && defined(__MACH__)

@import AppKit;
@import Foundation;
#include <mach-o/dyld.h>
#include "platform.h"
#include "../util/util.h"

bool platform_check_steam_overlay_attached() {
	STUB();
	return false;
}

void platform_get_exe_path(utf8 *outPath)
{
	char exePath[MAX_PATH];
	uint32_t size = MAX_PATH;
	int result = _NSGetExecutablePath(exePath, &size);
	if (result != 0) {
		log_fatal("failed to get path");
	}
	exePath[MAX_PATH - 1] = '\0';
	char *exeDelimiter = strrchr(exePath, platform_get_path_separator());
	if (exeDelimiter == NULL)
	{
		log_error("should never happen here");
		outPath[0] = '\0';
		return;
	}
	int exeDelimiterIndex = (int)(exeDelimiter - exePath);

	safe_strcpy(outPath, exePath, exeDelimiterIndex + 1);
	outPath[exeDelimiterIndex] = '\0';
}

/**
 * Default directory fallback is:
 *   - (command line argument)
 *   - ~/Library/Application Support/OpenRCT2
 */
void platform_posix_sub_user_data_path(char *buffer, const char *homedir, const char *separator) {
	if (homedir == NULL)
	{
		log_fatal("Couldn't find user data directory");
		exit(-1);
		return;
	}
	
	strncat(buffer, homedir, MAX_PATH - 1);
	strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, "Library", MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, "Application Support", MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, "OpenRCT2", MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
	strncat(buffer, separator, MAX_PATH - strnlen(buffer, MAX_PATH) - 1);
}

/**
 * Default directory fallback is:
 *   - (command line argument)
 *   - <exePath>/data
 *   - <Resources Folder>
 */
void platform_posix_sub_resolve_openrct_data_path(utf8 *out) {
	@autoreleasepool
	{
		NSBundle *bundle = [NSBundle mainBundle];
		if (bundle)
		{
			const utf8 *resources = bundle.resourcePath.UTF8String;
			if (platform_directory_exists(resources))
			{
				out[0] = '\0';
				safe_strcpy(out, resources, MAX_PATH);
				return;
			}
		}
	}
}

void platform_show_messagebox(char *message)
{
	@autoreleasepool
	{
		NSAlert *alert = [[[NSAlert alloc] init] autorelease];
		[alert addButtonWithTitle:@"OK"];
		alert.messageText = [NSString stringWithUTF8String:message];
		alert.alertStyle = NSWarningAlertStyle;
		[alert runModal];
	}
}

utf8 *platform_open_directory_browser(utf8 *title)
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
			url = (utf8*)malloc(strlen(path) + 1);
			strcpy(url,path);
		}
		return url;
	}
}

bool platform_open_common_file_dialog(utf8 *outFilename, file_dialog_desc *desc) {
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
		
		NSString *filePath = [NSString stringWithUTF8String:desc->default_filename];
		NSString *directory = filePath.stringByDeletingLastPathComponent;
		NSString *basename = filePath.lastPathComponent;
		
		NSSavePanel *panel;
		if (desc->type == FD_SAVE)
		{
			panel = [NSSavePanel savePanel];
			panel.nameFieldStringValue = [NSString stringWithFormat:@"%@.%@", basename, extensions.firstObject];
		}
		else if (desc->type == FD_OPEN)
		{
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
			strcpy(outFilename, panel.URL.path.UTF8String);
			SDL_RaiseWindow(gWindow);
			return true;
		}
	}
}

bool platform_get_font_path(TTFFontDescriptor *font, utf8 *buffer)
{
	@autoreleasepool
	{
		CTFontDescriptorRef fontRef = CTFontDescriptorCreateWithNameAndSize((CFStringRef)[NSString stringWithUTF8String:font->font_name], 0.0);
		CFURLRef url = (CFURLRef)CTFontDescriptorCopyAttribute(fontRef, kCTFontURLAttribute);
		if (url) {
			NSString *fontPath = [NSString stringWithString:[(NSURL *)CFBridgingRelease(url) path]];
			strcpy(buffer, fontPath.UTF8String);
			return true;
		} else {
			return false;
		}
	}
}

#endif
