#ifdef DISABLE_TWITCH

	extern "C" {
		#include "twitch.h"
	}

	void twitch_update() { }

#else

// REQUIRES HTTP

#include <vector>
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

#define PULSE_TIME (2 * 60 * 1000)

const char *TwitchBaseUrl = "https://api.twitch.tv/kraken/";
const char *TwitchExtendedBaseUrl = "http://openrct.ursalabs.co/api/1/";

static int _twitchState = TWITCH_STATE_LEFT;
static bool _twitchIdle = true;
static uint32 _twitchLastPulseTick = 0;
static http_json_response *_twitchJsonResponse;

static void twitch_join();
static void twitch_leave();
static void twitch_get_followers();

static void twitch_parse_followers();

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
			twitch_parse_followers();
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
 * GET /channel/:channel/audience
 */
static void twitch_get_followers()
{
	char url[256];
	sprintf(url, "%schannel/%s/audience", TwitchExtendedBaseUrl, gConfigTwitch.channel);

	_twitchState = TWITCH_STATE_WAITING;
	_twitchIdle = false;
	http_request_json_async(url, [](http_json_response *jsonResponse) -> void {
		_twitchJsonResponse = jsonResponse;
		_twitchState = TWITCH_STATE_GET_FOLLOWERS;
		_twitchIdle = true;
	});
}

static void twitch_parse_followers()
{
	struct AudienceMember {
		const char *name;
		bool isFollower;
		bool isInChat;
		bool isMod;
		bool exists;
	};

	std::vector<AudienceMember> members;

	http_json_response *jsonResponse = _twitchJsonResponse;
	if (!json_is_array(jsonResponse->root))
		goto json_error;

	int audienceCount = json_array_size(jsonResponse->root);
	for (int i = 0; i < audienceCount; i++) {
		json_t *audienceMember = json_array_get(jsonResponse->root, i);
		if (!json_is_object(audienceMember))
			continue;

		json_t *name = json_object_get(audienceMember, "name");
		json_t *isFollower = json_object_get(audienceMember, "isFollower");
		json_t *isInChat = json_object_get(audienceMember, "inChat");
		json_t *isMod = json_object_get(audienceMember, "isMod");

		AudienceMember member;
		member.name = json_string_value(name);
		member.isFollower = json_boolean_value(isFollower);
		member.isInChat = json_boolean_value(isInChat);
		member.isMod = json_boolean_value(isMod);
		member.exists = false;

		if (member.name == NULL || member.name[0] == 0)
			continue;

		if (gConfigTwitch.enable_chat_peep_names && member.isInChat)
			members.push_back(member);
		else if (gConfigTwitch.enable_follower_peep_names && member.isFollower)
			members.push_back(member);
	}

	uint16 spriteIndex;
	rct_peep *peep;
	char buffer[256];

	// Check what followers are already in the park
	FOR_ALL_GUESTS(spriteIndex, peep) {
		if (!is_user_string_id(peep->name_string_idx)) {
			format_string(buffer, peep->name_string_idx, NULL);
		
			for (size_t i = 0; i < members.size(); i++) {
				if (_strcmpi(buffer, members[i].name) == 0) {
					members[i].exists = true;
					break;
				}
			}
		}
	}
		
	// Rename non-named peeps to followers that aren't currently in the park.
	if (members.size() > 0) {
		int memberIndex = -1;
		FOR_ALL_GUESTS(spriteIndex, peep) {
			int originalMemberIndex = memberIndex;
			for (size_t i = memberIndex + 1; i < members.size(); i++) {
				if (!members[i].exists) {
					memberIndex = i;
					break;
				}
			}
			if (originalMemberIndex == memberIndex)
				break;
		
			AudienceMember *member = &members[memberIndex];
			if (!is_user_string_id(peep->name_string_idx) && !(peep->flags & PEEP_FLAGS_LEAVING_PARK)) {
				rct_string_id newStringId = user_string_allocate(4, member->name);
				if (newStringId != 0) {
					peep->name_string_idx = newStringId;

					if (member->isInChat && gConfigTwitch.enable_chat_peep_tracking)
						peep->flags |= PEEP_FLAGS_TRACKING;
					else if (member->isFollower && gConfigTwitch.enable_follower_peep_tracking)
						peep->flags |= PEEP_FLAGS_TRACKING;
				}
			} else {
				memberIndex--;
			}
		}
	}

json_error:
	http_request_json_dispose(jsonResponse);
	_twitchState = TWITCH_STATE_JOINED;
}

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