#include "../addresses.h"
#include "../config.h"
#include "../localisation/localisation.h"
#include "../peep/peep.h"
#include "../world/sprite.h"
#include "http.h"
#include "twitch.h"

const char *TwitchBaseUrl = "https://api.twitch.tv/kraken/";

/**
 * https://api.twitch.tv/kraken/channels/:channel/follows
 */
bool twitch_get_followers(const twitch_info *twitchInfo, int *outNumFollowers, char ***outFollowerNames)
{
	char url[256];
	http_json_response *jsonResponse;

	sprintf(url, "%schannels/%s/follows", TwitchBaseUrl, twitchInfo->channel);
	jsonResponse = http_request_json(url);
	if (jsonResponse == NULL)
		return false;

	if (jsonResponse->status_code != 200)
		goto json_error;
	
	json_t *jsonFollowsArray = json_object_get(jsonResponse->root, "follows");
	if (jsonFollowsArray == NULL || !json_is_array(jsonFollowsArray))
		goto json_error;

	int numFollowers = json_array_size(jsonFollowsArray);
	char **followerNames = malloc(numFollowers * sizeof(char*));
	for (int i = 0; i < numFollowers; i++) {
		json_t *jsonFollowObject = json_array_get(jsonFollowsArray, i);
		if (!json_is_object(jsonFollowObject))
			goto json_error;

		json_t *jsonUserObject = json_object_get(jsonFollowObject, "user");
		if (jsonUserObject == NULL)
			goto json_error;

		json_t *jsonDisplayName = json_object_get(jsonUserObject, "display_name");
		if (jsonDisplayName == NULL)
			goto json_error;

		followerNames[i] = _strdup(json_string_value(jsonDisplayName));
	}

	http_request_json_dispose(jsonResponse);

	*outNumFollowers = numFollowers;
	*outFollowerNames = followerNames;
	return true;

json_error:
	http_request_json_dispose(jsonResponse);
	return false;
}

void twitch_update_peeps()
{
	char buffer[256];

	twitch_info twitchInfo;
	twitchInfo.channel = gConfigTwitch.channel;

	if (twitchInfo.channel == NULL)
		return;

	int numFollowers;
	char **followers;

	if (!twitch_get_followers(&twitchInfo, &numFollowers, &followers)) {
		return;
	}

	bool *followerExists = calloc(numFollowers, sizeof(bool));
	int peepsToName = numFollowers;

	uint16 spriteIndex;
	rct_peep *peep;

	// Check what followers are already in the park
	FOR_ALL_PEEPS(spriteIndex, peep) {
		if (peep->name_string_idx >= 0x8000 && peep->name_string_idx < 0x9000) {
			format_string(buffer, peep->name_string_idx, NULL);

			for (int i = 0; i < numFollowers; i++) {
				if (_strcmpi(buffer, followers[i]) == 0) {
					followerExists[i] = true;
					peepsToName--;
					break;
				}
			}
		}
	}

	// Rename non-named peeps to followers that aren't currently in the park.
	if (peepsToName > 0) {
		int followerIndex = -1;
		FOR_ALL_PEEPS(spriteIndex, peep) {
			for (int i = followerIndex + 1; i < numFollowers; i++) {
				if (!followerExists[i]) {
					followerIndex = i;
					break;
				}
			}

			if (peep->name_string_idx < 0x8000 || peep->name_string_idx >= 0x9000) {
				rct_string_id newStringId = user_string_allocate(4, followers[followerIndex]);
				if (newStringId != STR_NONE) {
					peep->name_string_idx = newStringId;
					peep->flags |= PEEP_FLAGS_TRACKING;

					peepsToName--;
					if (peepsToName == 0)
						break;
				}
			}
		}
	}

	free(followers);
	free(followerExists);
}