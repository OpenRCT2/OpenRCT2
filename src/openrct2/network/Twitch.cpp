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

#ifdef DISABLE_TWITCH

    extern "C"
    {
        #include "twitch.h"
    }

    void twitch_update() { }

#else

#ifdef DISABLE_HTTP
    #error HTTP must be enabled to use the TWITCH functionality.
#endif

#include <vector>
#include "../core/Math.hpp"
#include "../core/String.hpp"
#include "../OpenRCT2.h"

extern "C"
{
    #include "../config/Config.h"
    #include "../drawing/drawing.h"
    #include "../game.h"
    #include "../interface/console.h"
    #include "../localisation/localisation.h"
    #include "../management/news_item.h"
    #include "../peep/peep.h"
    #include "../platform/platform.h"
    #include "../util/util.h"
    #include "../world/sprite.h"
    #include "http.h"
    #include "twitch.h"
}

bool gTwitchEnable = false;

namespace Twitch
{
    enum
    {
        TWITCH_STATE_JOINING,
        TWITCH_STATE_JOINED,
        TWITCH_STATE_WAITING,
        TWITCH_STATE_GET_FOLLOWERS,
        TWITCH_STATE_GET_MESSAGES,
        TWITCH_STATE_LEAVING,
        TWITCH_STATE_LEFT
    };

    enum
    {
        TWITCH_STATUS_OK = 200
    };

    struct AudienceMember
    {
        const char * Name;
        bool         IsFollower;
        bool         IsInChat;
        bool         IsMod;
        bool         Exists;
        bool         ShouldTrack;

        static AudienceMember FromJson(json_t * json)
        {
            AudienceMember member = { 0 };

            if (!json_is_object(json)) return member;
            json_t * name = json_object_get(json, "name");
            json_t * isFollower = json_object_get(json, "isFollower");
            json_t * isInChat = json_object_get(json, "inChat");
            json_t * isMod = json_object_get(json, "isMod");

            member.Name = json_string_value(name);
            member.IsFollower = json_is_true(isFollower);
            member.IsInChat = json_is_true(isInChat);
            member.IsMod = json_is_true(isMod);
            member.Exists = false;
            member.ShouldTrack = false;
            return member;
        }
    };

    /**
     * The time between HTTP requests.
     * TODO Ideally, the chat message pulse should be more frequent than the followers / chat members so that news messages etc.
     * have a lower latency.
     */
    constexpr uint32 PulseTime = 10 * 1000;
    constexpr const char * TwitchExtendedBaseUrl = "http://openrct.ursalabs.co/api/1/";

    static sint32               _twitchState = TWITCH_STATE_LEFT;
    static bool              _twitchIdle = true;
    static uint32            _twitchLastPulseTick = 0;
    static sint32               _twitchLastPulseOperation = 1;
    static http_response_t * _twitchJsonResponse;

    static void Join();
    static void Leave();
    static void GetFollowers();
    static void GetMessages();
    static void ParseFollowers();
    static void ParseMessages();
    static bool ShouldTrackMember(const AudienceMember * member);
    static bool ShouldMemberBeGuest(const AudienceMember * member);
    static void ManageGuestNames(std::vector<AudienceMember> &members);
    static void ParseChatMessage(const char * message);
    static void DoChatMessageNews(const char * message);

    static bool IsTwitchEnabled()
    {
        if (!gTwitchEnable) return false;
        if (gScreenFlags & (~SCREEN_FLAGS_PLAYING)) return false;
        if (String::IsNullOrEmpty(gConfigTwitch.channel)) return false;
        return true;
    }

    static void Update()
    {
        if (!_twitchIdle) return;
        if (IsTwitchEnabled()) {
            if (game_is_paused()) return;

            switch (_twitchState) {
            case TWITCH_STATE_LEFT:
            {
                uint32 currentTime = platform_get_ticks();
                uint32 timeSinceLastPulse = currentTime - _twitchLastPulseTick;
                if (_twitchLastPulseTick == 0 || timeSinceLastPulse > PulseTime)
                {
                    _twitchLastPulseTick = currentTime;
                    Join();
                }
                break;
            }
            case TWITCH_STATE_JOINED:
            {
                uint32 currentTime = platform_get_ticks();
                uint32 timeSinceLastPulse = currentTime - _twitchLastPulseTick;
                if (_twitchLastPulseTick == 0 || timeSinceLastPulse > PulseTime) {
                    _twitchLastPulseTick = currentTime;
                    _twitchLastPulseOperation = (_twitchLastPulseOperation + 1) % 2;
                    switch (_twitchLastPulseOperation + TWITCH_STATE_GET_FOLLOWERS) {
                    case TWITCH_STATE_GET_FOLLOWERS:
                        GetFollowers();
                        break;
                    case TWITCH_STATE_GET_MESSAGES:
                        if (gConfigTwitch.enable_news)
                        {
                            GetMessages();
                        }
                        break;
                    }
                }
                break;
            }
            case TWITCH_STATE_GET_FOLLOWERS:
                ParseFollowers();
                break;
            case TWITCH_STATE_GET_MESSAGES:
                ParseMessages();
                break;
            }
        }
        else
        {
            if (_twitchState != TWITCH_STATE_LEFT)
            {
                Leave();
            }
        }
    }

    /**
     * GET /leave/:join
     */
    static void Join()
    {
        char url[256];
        snprintf(url, sizeof(url), "%sjoin/%s", TwitchExtendedBaseUrl, gConfigTwitch.channel);

        _twitchState = TWITCH_STATE_JOINING;
        _twitchIdle = false;

        http_request_t request = {};
        request.url = url;
        request.method = HTTP_METHOD_GET;
        request.body = nullptr;
        request.type = HTTP_DATA_JSON;
        http_request_async(&request, [](http_response_t *jsonResponse) -> void
        {
            if (jsonResponse == nullptr)
            {
                _twitchState = TWITCH_STATE_LEFT;
                console_writeline("Unable to connect to twitch channel.");
            }
            else
            {
                json_t * jsonStatus = json_object_get(jsonResponse->root, "status");
                if (json_is_number(jsonStatus) && json_integer_value(jsonStatus) == TWITCH_STATUS_OK)
                {
                    _twitchState = TWITCH_STATE_JOINED;
                }
                else
                {
                    _twitchState = TWITCH_STATE_LEFT;
                }

                http_request_dispose(jsonResponse);

                _twitchLastPulseTick = 0;
                console_writeline("Connected to twitch channel.");
            }
            _twitchIdle = true;
        });
    }

    /**
     * GET /leave/:channel
     */
    static void Leave()
    {
        if (_twitchJsonResponse != nullptr)
        {
            http_request_dispose(_twitchJsonResponse);
            _twitchJsonResponse = nullptr;
        }

        console_writeline("Left twitch channel.");
        _twitchState = TWITCH_STATE_LEFT;
        _twitchLastPulseTick = 0;
        gTwitchEnable = false;

        // TODO reset all peeps with twitch flag

        // HTTP request no longer used as it could be abused
        // char url[256];
        // snprintf(url, sizeof(url), "%sleave/%s", TwitchExtendedBaseUrl, gConfigTwitch.channel);
        // _twitchState = TWITCH_STATE_LEAVING;
        // _twitchIdle = false;
        // http_request_json_async(url, [](http_json_response * jsonResponse) -> void
        // {
        //     http_request_json_dispose(jsonResponse);
        //     _twitchState = TWITCH_STATE_LEFT;
        //     _twitchIdle = true;
        //
        //     console_writeline("Left twitch channel.");
        // });
    }

    /**
     * GET /channel/:channel/audience
     */
    static void GetFollowers()
    {
        char url[256];
        snprintf(url, sizeof(url), "%schannel/%s/audience", TwitchExtendedBaseUrl, gConfigTwitch.channel);

        _twitchState = TWITCH_STATE_WAITING;
        _twitchIdle = false;

        http_request_t request = {};
        request.url = url;
        request.method = HTTP_METHOD_GET;
        request.body = nullptr;
        request.type = HTTP_DATA_JSON;
        http_request_async(&request, [](http_response_t * jsonResponse) -> void
        {
            if (jsonResponse == nullptr)
            {
                _twitchState = TWITCH_STATE_JOINED;
            }
            else
            {
                _twitchJsonResponse = jsonResponse;
                _twitchState = TWITCH_STATE_GET_FOLLOWERS;
            }
            _twitchIdle = true;
        });
    }

    /**
     * GET /channel/:channel/messages
     */
    static void GetMessages()
    {
        char url[256];
        snprintf(url, sizeof(url), "%schannel/%s/messages", TwitchExtendedBaseUrl, gConfigTwitch.channel);

        _twitchState = TWITCH_STATE_WAITING;
        _twitchIdle = false;

        http_request_t request = {};
        request.url = url;
        request.method = HTTP_METHOD_GET;
        request.body = nullptr;
        request.type = HTTP_DATA_JSON;
        http_request_async(&request, [](http_response_t * jsonResponse) -> void
        {
            if (jsonResponse == nullptr)
            {
                _twitchState = TWITCH_STATE_JOINED;
            }
            else
            {
                _twitchJsonResponse = jsonResponse;
                _twitchState = TWITCH_STATE_GET_MESSAGES;
            }
            _twitchIdle = true;
        });
    }

    static void ParseFollowers()
    {
        http_response_t *jsonResponse = _twitchJsonResponse;
        if (json_is_array(jsonResponse->root))
        {
            std::vector<AudienceMember> members;

            size_t audienceCount = json_array_size(jsonResponse->root);
            for (size_t i = 0; i < audienceCount; i++)
            {
                json_t * jsonAudienceMember = json_array_get(jsonResponse->root, i);
                auto member = AudienceMember::FromJson(jsonAudienceMember);
                if (!String::IsNullOrEmpty(member.Name))
                {
                    member.ShouldTrack = ShouldTrackMember(&member);
                    if (ShouldMemberBeGuest(&member))
                    {
                        members.push_back(member);
                    }
                }
            }

            ManageGuestNames(members);
        }

        http_request_dispose(_twitchJsonResponse);
        _twitchJsonResponse = nullptr;
        _twitchState = TWITCH_STATE_JOINED;

        gfx_invalidate_screen();
    }

    static void ParseMessages()
    {
        http_response_t * jsonResponse = _twitchJsonResponse;
        if (json_is_array(jsonResponse->root))
        {
            size_t messageCount = json_array_size(jsonResponse->root);
            for (size_t i = 0; i < messageCount; i++) {
                json_t * jsonMessage = json_array_get(jsonResponse->root, i);
                if (!json_is_object(jsonMessage))
                {
                    continue;
                }

                json_t * jsonText = json_object_get(jsonMessage, "message");
                const char * text = json_string_value(jsonText);
                ParseChatMessage(text);
            }
        }

        http_request_dispose(_twitchJsonResponse);
        _twitchJsonResponse = nullptr;
        _twitchState = TWITCH_STATE_JOINED;
    }

    static bool ShouldTrackMember(const AudienceMember * member)
    {
        if (member->IsInChat && gConfigTwitch.enable_chat_peep_tracking)
        {
            return true;
        }
        else if (member->IsFollower && gConfigTwitch.enable_follower_peep_tracking)
        {
            return true;
        }
        return false;
    }

    static bool ShouldMemberBeGuest(const AudienceMember * member)
    {
        if (gConfigTwitch.enable_chat_peep_names && member->IsInChat)
        {
            return true;
        }
        else if (gConfigTwitch.enable_follower_peep_names && member->IsFollower)
        {
            return true;
        }
        return false;
    }

    static void ManageGuestNames(std::vector<AudienceMember> &members)
    {
        // Check what followers are already in the park
        uint16 spriteIndex;
        rct_peep *peep;
        FOR_ALL_GUESTS(spriteIndex, peep)
        {
            if (is_user_string_id(peep->name_string_idx))
            {
                utf8 buffer[256];
                format_string(buffer, 256, peep->name_string_idx, nullptr);

                AudienceMember * member = nullptr;
                for (AudienceMember &m : members)
                {
                    if (String::Equals(buffer, m.Name, true))
                    {
                        member = &m;
                        m.Exists = true;
                        break;
                    }
                }

                if (peep->peep_flags & PEEP_FLAGS_TWITCH)
                {
                    if (member == nullptr)
                    {
                        // Member no longer peep name worthy
                        peep->peep_flags &= ~(PEEP_FLAGS_TRACKING | PEEP_FLAGS_TWITCH);

                        // TODO set peep name back to number / real name
                    }
                    else
                    {
                        if (member->ShouldTrack)
                        {
                            peep->peep_flags |= (PEEP_FLAGS_TRACKING);
                        }
                        else if (!member->ShouldTrack)
                        {
                            peep->peep_flags &= ~(PEEP_FLAGS_TRACKING);
                        }
                    }
                }
                else if (member != nullptr && !(peep->peep_flags & PEEP_FLAGS_LEAVING_PARK))
                {
                    // Peep with same name already exists but not twitch
                    peep->peep_flags |= PEEP_FLAGS_TWITCH;
                    if (member->ShouldTrack)
                    {
                        peep->peep_flags |= PEEP_FLAGS_TRACKING;
                    }
                }
            }
        }

        // Rename non-named peeps to followers that aren't currently in the park.
        if (members.size() > 0)
        {
            size_t memberIndex = SIZE_MAX;
            FOR_ALL_GUESTS(spriteIndex, peep)
            {
                size_t originalMemberIndex = memberIndex;
                for (size_t i = memberIndex + 1; i < members.size(); i++)
                {
                    if (!members[i].Exists)
                    {
                        memberIndex = i;
                        break;
                    }
                }
                if (originalMemberIndex == memberIndex)
                {
                    break;
                }

                AudienceMember * member = &members[memberIndex];
                if (!is_user_string_id(peep->name_string_idx) && !(peep->peep_flags & PEEP_FLAGS_LEAVING_PARK))
                {
                    // Rename peep and add flags
                    rct_string_id newStringId = user_string_allocate(USER_STRING_HIGH_ID_NUMBER, member->Name);
                    if (newStringId != 0)
                    {
                        peep->name_string_idx = newStringId;
                        peep->peep_flags |= PEEP_FLAGS_TWITCH;
                        if (member->ShouldTrack)
                        {
                            peep->peep_flags |= PEEP_FLAGS_TRACKING;
                        }
                    }
                }
                else
                {
                    // Peep still yet to be found for member
                    memberIndex--;
                }
            }
        }
    }

    static char * strskipwhitespace(const char * str)
    {
        while (*str == ' ' || *str == '\t')
        {
            str++;
        }
        return (char *)str;
    }

    static void ParseChatMessage(const char * message)
    {
        message = strskipwhitespace(message);
        if (!String::StartsWith(message, "!"))
        {
            return;
        }

        // Skip '!'
        message++;

        // Check that command is "news"
        const char *ch, *cmd;
        for (ch = message, cmd = "news"; *cmd != '\0'; ++ch, ++cmd) {
            if (*ch != *cmd) return;
        }

        if (!isspace(*ch)) return;

        ch = strskipwhitespace(ch);
        DoChatMessageNews(ch);
    }

    static void DoChatMessageNews(const char * message)
    {
        if (gConfigTwitch.enable_news)
        {
            utf8 buffer[256];
            buffer[0] = (utf8)(uint8)FORMAT_TOPAZ;
            safe_strcpy(buffer + 1, message, sizeof(buffer) - 1);

            // Remove unsupported characters
            // TODO allow when OpenRCT2 gains unicode support
            char * ch = buffer + 1;
            while (ch[0] != '\0')
            {
                if ((uint8)ch[0] < 32 || (uint8)ch[0] > 122)
                {
                    ch[0] = ' ';
                }
                ch++;
            }

            // TODO Create a new news item type for twitch which has twitch icon
            news_item_add_to_queue_raw(NEWS_ITEM_BLANK, buffer, 0);
        }
    }
}

void twitch_update()
{
    Twitch::Update();
}

#endif
