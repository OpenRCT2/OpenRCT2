#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#pragma once

#include <SDL_endian.h>
#include <SDL_platform.h>

#ifndef DISABLE_NETWORK
#ifdef __WINDOWS__
    // winsock2 must be included before windows.h
    #include <winsock2.h>
    #include <ws2tcpip.h>

    #define LAST_SOCKET_ERROR() WSAGetLastError()
    #undef EWOULDBLOCK
    #define EWOULDBLOCK WSAEWOULDBLOCK
    #ifndef SHUT_RD
        #define SHUT_RD SD_RECEIVE
    #endif
    #ifndef SHUT_RDWR
        #define SHUT_RDWR SD_BOTH
    #endif
    #define FLAG_NO_PIPE 0
#else
    #include <errno.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/tcp.h>
    #include <sys/socket.h>
    #include <fcntl.h>
    typedef int SOCKET;
    #define SOCKET_ERROR -1
    #define INVALID_SOCKET -1
    #define LAST_SOCKET_ERROR() errno
    #define closesocket close
    #define ioctlsocket ioctl
    #if defined(__LINUX__)
        #define FLAG_NO_PIPE MSG_NOSIGNAL
    #else
        #define FLAG_NO_PIPE 0
    #endif // defined(__LINUX__)
#endif // __WINDOWS__

#include "../common.h"
#endif

enum NETWORK_READPACKET
{
    NETWORK_READPACKET_SUCCESS,
    NETWORK_READPACKET_NO_DATA,
    NETWORK_READPACKET_MORE_DATA,
    NETWORK_READPACKET_DISCONNECTED
};

enum NETWORK_AUTH
{
    NETWORK_AUTH_NONE,
    NETWORK_AUTH_REQUESTED,
    NETWORK_AUTH_OK,
    NETWORK_AUTH_BADVERSION,
    NETWORK_AUTH_BADNAME,
    NETWORK_AUTH_BADPASSWORD,
    NETWORK_AUTH_VERIFICATIONFAILURE,
    NETWORK_AUTH_FULL,
    NETWORK_AUTH_REQUIREPASSWORD,
    NETWORK_AUTH_VERIFIED,
    NETWORK_AUTH_UNKNOWN_KEY_DISALLOWED,
};

enum NETWORK_COMMAND
{
    NETWORK_COMMAND_AUTH,
    NETWORK_COMMAND_MAP,
    NETWORK_COMMAND_CHAT,
    NETWORK_COMMAND_GAMECMD,
    NETWORK_COMMAND_TICK,
    NETWORK_COMMAND_PLAYERLIST,
    NETWORK_COMMAND_PING,
    NETWORK_COMMAND_PINGLIST,
    NETWORK_COMMAND_SETDISCONNECTMSG,
    NETWORK_COMMAND_GAMEINFO,
    NETWORK_COMMAND_SHOWERROR,
    NETWORK_COMMAND_GROUPLIST,
    NETWORK_COMMAND_EVENT,
    NETWORK_COMMAND_TOKEN,
    NETWORK_COMMAND_MAX,
    NETWORK_COMMAND_INVALID = -1
};

#ifdef __cplusplus

template <size_t size>
struct ByteSwapT { };

template <>
struct ByteSwapT<1>
{
    static uint8 SwapBE(uint8 value)
    {
        return value;
    }
};

template <>
struct ByteSwapT<2>
{
    static uint16 SwapBE(uint16 value)
    {
        return SDL_SwapBE16(value);
    }
};

template <>
struct ByteSwapT<4>
{
    static uint32 SwapBE(uint32 value)
    {
        return SDL_SwapBE32(value);
    }
};

template <typename T>
T ByteSwapBE(const T& value)
{
    return ByteSwapT<sizeof(T)>::SwapBE(value);
}

#endif
