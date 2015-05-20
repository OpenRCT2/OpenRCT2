#ifndef _TWITCH_H_
#define _TWITCH_H_

#include "../common.h"

typedef struct {
	char *channel;
} twitch_info;

bool twitch_get_followers(const twitch_info *twitchInfo, int *outNumFollowers, char ***outFollowerNames);

void twitch_update_peeps();

#endif