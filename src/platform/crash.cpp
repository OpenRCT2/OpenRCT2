#include "crash.h"
#include <SDL_platform.h>

#ifdef USE_BREAKPAD
#include <stdio.h>

#ifdef __WINDOWS__
#include <breakpad/client/windows/handler/exception_handler.h>
#include <string>
#include <ShlObj.h>
#define BREAKPAD_PATH "."
#else
#error Breakpad support not implemented yet for this platform
#endif __WINDOWS__

#ifdef __LINUX__
#include <breakpad/client/linux/handler/exception_handler.h>
#define BREAKPAD_PATH "/tmp"
#endif __LINUX__

extern "C" {
#include "platform.h"
#include "../localisation/language.h"
#include "../scenario.h"
}


static bool dumpCallback(const wchar_t *dump_path, const wchar_t *minidump_id, void* context, EXCEPTION_POINTERS* exinfo, MDRawAssertionInfo* assertion, bool succeeded) {
	if (!succeeded) {
		const char *msg = "Failed to create the dump. Nothing left to do. Please file an issue with OpenRCT2 on Github and provide latest save.";
		log_fatal(msg);
		MessageBoxA(NULL, msg, "OpenRCT2", MB_OK);
		return succeeded;
	}
	char *buffer_path = widechar_to_utf8(dump_path);
	char *buffer_minidump = widechar_to_utf8(minidump_id);
	char dump_file_path[MAX_PATH];
	char save_file_path[MAX_PATH];
	sprintf(dump_file_path, "%s%s.dmp", buffer_path, buffer_minidump);
	sprintf(save_file_path, "%s%s.sv6", buffer_path, buffer_minidump);
	log_fatal("Dump path: %s", buffer_path);
	log_fatal("minidump id: %s", buffer_minidump);
	log_fatal("Version: %s", OPENRCT2_VERSION);
	log_fatal("Commit: %s", OPENRCT2_COMMIT_SHA1_SHORT);
	SDL_RWops* rw = SDL_RWFromFile(save_file_path, "wb+");
	bool saved = false;
	if (rw != NULL) {
		scenario_save(rw, 0x80000000);
		saved = true;
		SDL_RWclose(rw);
	}
	char message[MAX_PATH * 2];
	sprintf(message, "A crash has occurred and dump was created at %s. Please create an issue with OpenRCT2 on Github and provide the dump and %s. Version: %s, Commit: %s",
		dump_file_path, saved ? save_file_path : "latest save", OPENRCT2_VERSION, OPENRCT2_COMMIT_SHA1_SHORT);
	// Cannot use platform_show_messagebox here, it tries to set parent window already dead.
	MessageBoxA(NULL, message, "OpenRCT2", MB_OK);
	HRESULT coinit_result = CoInitialize(NULL);
	if (coinit_result == S_OK) {
		//ShellExecute(NULL, "explore", message, NULL, NULL, 5);
		ITEMIDLIST *pidl = ILCreateFromPath(buffer_path);
		ITEMIDLIST *files[2];
		files[0] = ILCreateFromPath(dump_file_path);
		int files_count = 1;
		if (saved) {
			files[1] = ILCreateFromPath(save_file_path);
			files_count = 2;
		}
		if (pidl) {
			HRESULT res = SHOpenFolderAndSelectItems(pidl, files_count, (LPCITEMIDLIST *)files, 0);
			ILFree(pidl);
			ILFree(files[0]);
			if (saved) {
				ILFree(files[1]);
			}
		}
		CoUninitialize();
	}
	free(buffer_path);
	free(buffer_minidump);
	return succeeded;
}
#endif // USE_BREAKPAD

extern "C" CExceptionHandler newCExceptionHandlerSimple(void)
{
#ifdef USE_BREAKPAD
	char path[MAX_PATH];
	platform_get_user_directory(path, NULL);
	wchar_t *wpath_buffer = utf8_to_widechar(path);
	std::wstring wpath(wpath_buffer);
	free(wpath_buffer);
	// Path must exist and be RW!
	return reinterpret_cast<void*>(new google_breakpad::ExceptionHandler(
	                                   wpath,
	                                   0,
	                                   dumpCallback,
	                                   0,
	                                   google_breakpad::ExceptionHandler::HANDLER_ALL,
	                                   MiniDumpNormal,
	                                   L"openrct2-bpad", // using non-null pipe name here lets breakpad try setting OOP crash handling
	                                   0 ));
#else // USE_BREAKPAD
	return nullptr;
#endif // USE_BREAKPAD
}
