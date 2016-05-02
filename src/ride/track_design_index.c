#include "../common.h"
#include "../config.h"
#include "../game.h"
#include "../interface/window.h"
#include "../localisation/string_ids.h"
#include "../util/util.h"
#include "track.h"
#include "track_design.h"

typedef struct {
	uint8 ride_type;
	char ride_entry[9];
	utf8 path[MAX_PATH];
} td_index_item;

static bool track_design_index_read_header(SDL_RWops *file, uint32 *tdidxCount);
static void track_design_index_scan();
static int track_design_index_item_compare(const void *a, const void *b);
static void track_design_index_include(const utf8 *directory);
static void track_design_add_file(const utf8 *path);
static void track_design_add(const td_index_item *item);
static void track_design_index_dispose();
static void track_design_index_get_path(utf8 * buffer, size_t bufferLength);

static const uint32 TrackIndexMagicNumber = 0x58444954;
static const uint16 TrackIndexVersion = 0;

static td_index_item *_tdIndex = NULL;
static size_t _tdIndexSize = 0;
static size_t _tdIndexCapacity = 0;

void track_design_index_create()
{
	track_design_index_dispose();

	log_verbose("saving track design index (tracks.idx)");

	utf8 path[MAX_PATH];
	track_design_index_get_path(path, sizeof(path));

	SDL_RWops *file = SDL_RWFromFile(path, "wb");
	if (file != NULL) {
		track_design_index_scan();

		SDL_RWwrite(file, &TrackIndexMagicNumber, 4, 1);
		SDL_RWwrite(file, &TrackIndexVersion, 4, 1);
		SDL_RWwrite(file, &_tdIndexSize, 4, 1);
		SDL_RWwrite(file, _tdIndex, sizeof(td_index_item), _tdIndexSize);
		SDL_RWclose(file);
		track_design_index_dispose();
	}
}

size_t track_design_index_get_count_for_ride(uint8 rideType, const char *entry)
{
	log_verbose("reading track design index (tracks.idx)");

	utf8 path[MAX_PATH];
	track_design_index_get_path(path, sizeof(path));

	// Return list
	size_t refsCount = 0;
	SDL_RWops *file = SDL_RWFromFile(path, "rb");
	if (file != NULL) {
		uint32 tdidxCount;
		if (!track_design_index_read_header(file, &tdidxCount)) {
			SDL_RWclose(file);
			return 0;
		}

		for (uint32 i = 0; i < tdidxCount; i++) {
			td_index_item tdItem;
			SDL_RWread(file, &tdItem, sizeof(td_index_item), 1);

			if (tdItem.ride_type != rideType) continue;
			if (entry != NULL && _strcmpi(entry, tdItem.ride_entry) != 0) continue;
			refsCount++;
		}
	}

	return refsCount;
}

size_t track_design_index_get_for_ride(track_design_file_ref **tdRefs, uint8 rideType, const char *entry)
{
	log_verbose("reading track design index (tracks.idx)");

	utf8 path[MAX_PATH];
	track_design_index_get_path(path, sizeof(path));

	// Return list
	size_t refsCount = 0;
	size_t refsCapacity = 0;
	track_design_file_ref *refs = NULL;

	SDL_RWops *file = SDL_RWFromFile(path, "rb");
	if (file != NULL) {
		uint32 tdidxCount;
		if (!track_design_index_read_header(file, &tdidxCount)) {
			SDL_RWclose(file);
			return 0;
		}

		for (uint32 i = 0; i < tdidxCount; i++) {
			td_index_item tdItem;
			SDL_RWread(file, &tdItem, sizeof(td_index_item), 1);

			if (tdItem.ride_type != rideType) continue;
			if (entry != NULL && _strcmpi(entry, tdItem.ride_entry) != 0) continue;

			size_t nextIndex = refsCount;
			if (nextIndex >= refsCapacity) {
				refsCapacity = max(8, refsCapacity * 2);
				refs = realloc(refs, refsCapacity * sizeof(track_design_file_ref));
				if (refs == NULL) {
					log_fatal("Unable to allocate more memory.");
					exit(-1);
				}
			}
			refs[nextIndex].name = track_design_get_name_from_path(tdItem.path);
			refs[nextIndex].path = _strdup(tdItem.path);
			refsCount++;
		}
		SDL_RWclose(file);
	}

	*tdRefs = realloc(refs, refsCount * sizeof(track_design_file_ref));
	return refsCount;
}

utf8 *track_design_get_name_from_path(const utf8 *path)
{
	const char *filename = path_get_filename(path);
	const char *lastDot = strrchr(filename, '.');
	size_t nameLength;
	if (lastDot == NULL) {
		nameLength = strlen(filename);
	} else {
		nameLength = (size_t)(lastDot - filename);
	}
	return strndup(filename, nameLength);
}

/**
 *
 *  rct2: 0x006D3664
 */
bool track_design_index_rename(const utf8 *path, const utf8 *newName)
{
	if (str_is_null_or_empty(newName)) {
		gGameCommandErrorText = STR_CANT_RENAME_TRACK_DESIGN;
		return false;
	}

	if (!filename_valid_characters(newName)) {
		gGameCommandErrorText = STR_NEW_NAME_CONTAINS_INVALID_CHARACTERS;
		return false;
	}

	utf8 newPath[MAX_PATH];
	const char *lastPathSep = strrchr(path, platform_get_path_separator());
	if (lastPathSep == NULL) {
		gGameCommandErrorText = STR_CANT_RENAME_TRACK_DESIGN;
		return false;
	}
	size_t directoryLength = (size_t)(lastPathSep - path + 1);
	memcpy(newPath, path, directoryLength);
	strcpy(newPath + directoryLength, newName);
	strcat(newPath, ".td6");

	if (!platform_file_move(path, newPath)) {
		gGameCommandErrorText = STR_ANOTHER_FILE_EXISTS_WITH_NAME_OR_FILE_IS_WRITE_PROTECTED;
		return false;
	}

	track_design_index_create();

	rct_window *trackListWindow = window_find_by_class(WC_TRACK_DESIGN_LIST);
	if (trackListWindow != NULL) {
		trackListWindow->track_list.reload_track_designs = true;
	}
	return true;
}

/**
 *
 *  rct2: 0x006D3761
 */
bool track_design_index_delete(const utf8 *path)
{
	if (!platform_file_delete(path)) {
		gGameCommandErrorText = STR_FILE_IS_WRITE_PROTECTED_OR_LOCKED;
		return false;
	}

	track_design_index_create();

	rct_window *trackListWindow = window_find_by_class(WC_TRACK_DESIGN_LIST);
	if (trackListWindow != NULL) {
		trackListWindow->track_list.reload_track_designs = true;
	}
	return true;
}

/**
 *
 *  rct2: 0x006D40B2
 */
bool track_design_index_install(const utf8 *srcPath, const utf8 *destPath)
{
	if (!platform_file_copy(srcPath, destPath, false)) {
		return false;
	}

	track_design_index_create();

	rct_window *trackListWindow = window_find_by_class(WC_TRACK_DESIGN_LIST);
	if (trackListWindow != NULL) {
		trackListWindow->track_list.reload_track_designs = true;
	}
	return true;
}

static bool track_design_index_read_header(SDL_RWops *file, uint32 *tdidxCount)
{
	uint32 tdidxMagicNumber, tdidxVersion;
	SDL_RWread(file, &tdidxMagicNumber, 4, 1);
	SDL_RWread(file, &tdidxVersion, 4, 1);
	SDL_RWread(file, tdidxCount, 4, 1);
	if (tdidxMagicNumber != TrackIndexMagicNumber) {
		log_error("invalid track index file");
		return false;
	}
	if (tdidxVersion != TrackIndexVersion) {
		log_error("unsupported track index file version");
		return false;
	}
	return true;
}

static void track_design_index_scan()
{
	utf8 directory[MAX_PATH];

	// Get track directory from RCT2
	safe_strcpy(directory, gConfigGeneral.game_path, sizeof(directory));
	safe_strcat_path(directory, "Tracks", sizeof(directory));
	track_design_index_include(directory);

	// Get track directory from user directory
	platform_get_user_directory(directory, "tracks");
	track_design_index_include(directory);

	// Sort items by ride type then by filename
	qsort(_tdIndex, _tdIndexSize, sizeof(td_index_item), track_design_index_item_compare);
}

static int track_design_index_item_compare(const void *a, const void *b)
{
	const td_index_item *tdA = (const td_index_item*)a;
	const td_index_item *tdB = (const td_index_item*)b;

	if (tdA->ride_type != tdB->ride_type) {
		return tdA->ride_type - tdB->ride_type;
	}

	const utf8 *tdAName = path_get_filename(tdA->path);
	const utf8 *tdBName = path_get_filename(tdB->path);
	return _stricmp(tdAName, tdBName);
}

static void track_design_index_include(const utf8 *directory)
{
	int handle;
	file_info fileInfo;

	// Scenarios in this directory
	utf8 pattern[MAX_PATH];
	safe_strcpy(pattern, directory, sizeof(pattern));
	safe_strcat_path(pattern, "*.td6", sizeof(pattern));

	handle = platform_enumerate_files_begin(pattern);
	while (platform_enumerate_files_next(handle, &fileInfo)) {
		utf8 path[MAX_PATH];
		safe_strcpy(path, directory, sizeof(pattern));
		safe_strcat_path(path, fileInfo.path, sizeof(pattern));
		track_design_add_file(path);
	}
	platform_enumerate_files_end(handle);

	// Include sub-directories
	utf8 subDirectory[MAX_PATH];
	handle = platform_enumerate_directories_begin(directory);
	while (platform_enumerate_directories_next(handle, subDirectory)) {
		utf8 path[MAX_PATH];
		safe_strcpy(path, directory, sizeof(pattern));
		safe_strcat_path(path, subDirectory, sizeof(pattern));
		track_design_index_include(path);
	}
	platform_enumerate_directories_end(handle);
}

static void track_design_add_file(const utf8 *path)
{
	rct_track_td6 *td6 = track_design_open(path);
	if (td6 != NULL) {
		td_index_item tdIndexItem = { 0 };
		safe_strcpy(tdIndexItem.path, path, sizeof(tdIndexItem.path));
		memcpy(tdIndexItem.ride_entry, td6->vehicle_object.name, 8);
		tdIndexItem.ride_type = td6->type;
		track_design_add(&tdIndexItem);
		track_design_dispose(td6);
	}
}

static void track_design_add(const td_index_item *item)
{
	size_t nextIndex = _tdIndexSize;
	if (nextIndex >= _tdIndexCapacity) {
		_tdIndexCapacity = max(128, _tdIndexCapacity * 2);
		_tdIndex = realloc(_tdIndex, _tdIndexCapacity * sizeof(td_index_item));
		if (_tdIndex == NULL) {
			log_fatal("Unable to allocate more memory.");
			exit(-1);
		}
	}
	_tdIndex[nextIndex] = *item;
	_tdIndexSize++;
}

static void track_design_index_dispose()
{
	SafeFree(_tdIndex);
	_tdIndexSize = 0;
	_tdIndexCapacity = 0;
}

static void track_design_index_get_path(utf8 * buffer, size_t bufferLength)
{
	platform_get_user_directory(buffer, NULL);
	safe_strcat(buffer, "tracks2.idx", bufferLength);
}
