#ifdef DISABLE_TWITCH

	extern "C" {
		#include "twitch.h"
	}

	void twitch_update() { }

#else

// REQUIRES HTTP

#include <SDL.h>

extern "C" {

	#include "../addresses.h"
	#include "../config.h"
	#include "../interface/console.h"
	#include "../localisation/localisation.h"
	#include "../management/news_item.h"
	#include "../peep/peep.h"
	#include "../world/sprite.h"
	#include "http.h"
	#include "twitch.h"

}

enum {
	TWITCH_STATE_JOINING,
	TWITCH_STATE_JOINED,
	TWITCH_STATE_WAITING,
	TWITCH_STATE_GET_FOLLOWERS,
	TWITCH_STATE_LEAVING,
	TWITCH_STATE_LEFT
};

#define PULSE_TIME (5 * 60 * 1000)

const char *TwitchBaseUrl = "https://api.twitch.tv/kraken/";
const char *TwitchExtendedBaseUrl = "http://openrct.ursalabs.co/api/1/";

static int _twitchState = TWITCH_STATE_LEFT;
static bool _twitchIdle = true;
static uint32 _twitchLastPulseTick = 0;

static void twitch_join();
static void twitch_leave();
static void twitch_get_followers();

void twitch_update()
{
	if (!_twitchIdle)
		return;

	if (!(RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & (~SCREEN_FLAGS_PLAYING))) {
		if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) != 0)
			return;

		if (gConfigTwitch.channel == NULL || gConfigTwitch.channel[0] == 0)
			return;

		switch (_twitchState) {
		case TWITCH_STATE_LEFT:
			twitch_join();
			break;
		case TWITCH_STATE_JOINED:
		{
			uint32 currentTime = SDL_GetTicks();
			uint32 timeSinceLastPulse = _twitchLastPulseTick - currentTime;
			if (_twitchLastPulseTick == 0 || timeSinceLastPulse > PULSE_TIME) {
				_twitchLastPulseTick = currentTime;
				twitch_get_followers();
			}
			break;
		}
		case TWITCH_STATE_GET_FOLLOWERS:
			break;
		}
	} else {
		if (_twitchState != TWITCH_STATE_LEFT)
			twitch_leave();
	}
}

/**
 * GET /leave/:join
 */
static void twitch_join()
{
	char url[256];
	sprintf(url, "%sjoin/%s", TwitchExtendedBaseUrl, gConfigTwitch.channel);

	_twitchState = TWITCH_STATE_JOINING;
	_twitchIdle = false;
	http_request_json_async(url, [](http_json_response *jsonResponse) -> void {
		if (jsonResponse == NULL)
			_twitchState = TWITCH_STATE_LEFT;

		json_t *jsonStatus = json_object_get(jsonResponse->root, "status");
		if (json_is_number(jsonStatus) && json_integer_value(jsonStatus) == 200)
			_twitchState = TWITCH_STATE_JOINED;
		else
			_twitchState = TWITCH_STATE_LEFT;
	
		http_request_json_dispose(jsonResponse);

		_twitchIdle = true;
		console_writeline("Connected to twitch channel.");
	});
}

/**
 * GET /leave/:channel
 */
static void twitch_leave()
{
	char url[256];
	sprintf(url, "%sleave/%s", TwitchExtendedBaseUrl, gConfigTwitch.channel);

	_twitchState = TWITCH_STATE_LEAVING;
	_twitchIdle = false;
	http_request_json_async(url, [](http_json_response *jsonResponse) -> void {
		http_request_json_dispose(jsonResponse);
		_twitchState = TWITCH_STATE_LEFT;
		_twitchIdle = true;

		console_writeline("Left twitch channel.");
	});
}

/**
 * GET /follows/:channel
 */
static void twitch_get_followers()
{

}









///**
// * https://api.twitch.tv/kraken/channels/:channel/follows
// */
//bool twitch_get_followers(const twitch_info *twitchInfo, int *outNumFollowers, char ***outFollowerNames)
//{
//	char url[256];
//	http_json_response *jsonResponse;
//
//	sprintf(url, "%schannels/%s/follows", TwitchBaseUrl, twitchInfo->channel);
//	jsonResponse = http_request_json(url);
//	if (jsonResponse == NULL)
//		return false;
//
//	if (jsonResponse->status_code != 200)
//		goto json_error;
//	
//	json_t *jsonFollowsArray = json_object_get(jsonResponse->root, "follows");
//	if (jsonFollowsArray == NULL || !json_is_array(jsonFollowsArray))
//		goto json_error;
//
//	int numFollowers = json_array_size(jsonFollowsArray);
//	char **followerNames = (char**)malloc(numFollowers * sizeof(char*));
//	for (int i = 0; i < numFollowers; i++) {
//		json_t *jsonFollowObject = json_array_get(jsonFollowsArray, i);
//		if (!json_is_object(jsonFollowObject))
//			goto json_error;
//
//		json_t *jsonUserObject = json_object_get(jsonFollowObject, "user");
//		if (jsonUserObject == NULL)
//			goto json_error;
//
//		json_t *jsonDisplayName = json_object_get(jsonUserObject, "display_name");
//		if (jsonDisplayName == NULL)
//			goto json_error;
//
//		followerNames[i] = _strdup(json_string_value(jsonDisplayName));
//	}
//
//	http_request_json_dispose(jsonResponse);
//
//	*outNumFollowers = numFollowers;
//	*outFollowerNames = followerNames;
//	return true;
//
//json_error:
//	http_request_json_dispose(jsonResponse);
//	return false;
//}
//
//void twitch_get_followers_async(const twitch_info *twitchInfo, void (*callback)(int numFollowers, char **followerNames))
//{
//	struct TempThreadArgs {
//		const twitch_info *twitchInfo;
//		void (*callback)(int numFollowers, char **followerNames);
//	};
//
//	TempThreadArgs *args = (TempThreadArgs*)malloc(sizeof(TempThreadArgs));
//	args->twitchInfo = twitchInfo;
//	args->callback = callback;
//
//	SDL_Thread *thread = SDL_CreateThread([](void *ptr) -> int {
//		TempThreadArgs *args = (TempThreadArgs*)ptr;
//
//		int numFollowers;
//		char **followers;
//		bool result = twitch_get_followers(args->twitchInfo, &numFollowers, &followers);
//		free(args);
//
//		if (result)
//			args->callback(numFollowers, followers);
//
//		return 0;
//	}, NULL, args);
//
//	if (thread == NULL) {
//		log_error("Unable to create thread!");
//	}
//}
//
//void twitch_update_peeps()
//{
//	twitch_info twitchInfo;
//
//	twitchInfo.channel = gConfigTwitch.channel;
//	if (twitchInfo.channel == NULL)
//		return;
//
//	twitch_get_followers_async(&twitchInfo, [](int numFollowers, char **followers) -> void {
//		char buffer[256];
//
//		bool *followerExists = (bool*)calloc(numFollowers, sizeof(bool));
//		int peepsToName = numFollowers;
//
//		uint16 spriteIndex;
//		rct_peep *peep;
//
//		// Check what followers are already in the park
//		FOR_ALL_PEEPS(spriteIndex, peep) {
//			if (peep->name_string_idx >= 0x8000 && peep->name_string_idx < 0x9000) {
//				format_string(buffer, peep->name_string_idx, NULL);
//
//				for (int i = 0; i < numFollowers; i++) {
//					if (_strcmpi(buffer, followers[i]) == 0) {
//						followerExists[i] = true;
//						peepsToName--;
//						break;
//					}
//				}
//			}
//		}
//
//		// Rename non-named peeps to followers that aren't currently in the park.
//		if (peepsToName > 0) {
//			int followerIndex = -1;
//			FOR_ALL_PEEPS(spriteIndex, peep) {
//				for (int i = followerIndex + 1; i < numFollowers; i++) {
//					if (!followerExists[i]) {
//						followerIndex = i;
//						break;
//					}
//				}
//
//				if (peep->name_string_idx < 0x8000 || peep->name_string_idx >= 0x9000) {
//					rct_string_id newStringId = user_string_allocate(4, followers[followerIndex]);
//					if (newStringId != STR_NONE) {
//						peep->name_string_idx = newStringId;
//						peep->flags |= PEEP_FLAGS_TRACKING;
//
//						peepsToName--;
//						if (peepsToName == 0)
//							break;
//					}
//				}
//			}
//		}
//
//		free(followers);
//		free(followerExists);
//	});
//}

/**
 * Like strchr but allows searching for one of many characters.
 */
char *strchrm(const char *str, const char *find)
{
	const char *result = NULL;
	do {
		const char *fch = find;
		while (*fch != 0) {
			if (*str == *fch)
				return (char*)str;

			fch++;
		}
	} while (*str++ != 0);
}

char *strskipwhitespace(const char *str)
{
	while (*str == ' ' || *str == '\t')
		str++;

	return (char*)str;
}

void twitch_parse_chat_message(const char *message)
{
	char buffer[256], *ch;

	message = strskipwhitespace(message);
	if (message[0] != '!')
		return;

	message++;
	ch = strchrm(message, " \t");
	strncpy(buffer, message, ch - message);

	if (_strcmpi(buffer, "news") == 0) {
		ch = strskipwhitespace(ch);
		strncpy(buffer, ch, sizeof(buffer) - 1);

		news_item_add_to_queue_raw(NEWS_ITEM_BLANK, buffer, 0);
	}
}

#endif