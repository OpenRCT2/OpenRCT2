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