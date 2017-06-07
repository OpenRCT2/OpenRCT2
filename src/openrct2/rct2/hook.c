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

#include "../common.h"

#ifndef NO_RCT2

#ifdef __WINDOWS__
    #include <windows.h>
#else
    #include <sys/mman.h>
#endif // __WINDOWS__

#include "../platform/platform.h"
#include "hook.h"

void* _hookTableAddress = 0;
sint32 _hookTableOffset = 0;
sint32 _maxHooks = 1000;
#define HOOK_BYTE_COUNT (140)

registers gHookRegisters = {0};

// This macro writes a little-endian 4-byte long value into *data
// It is used to avoid type punning.
#define write_address_strictalias(data, addr) \
    *(data + 0) = ((addr) & 0x000000ff) >> 0; \
    *(data + 1) = ((addr) & 0x0000ff00) >> 8; \
    *(data + 2) = ((addr) & 0x00ff0000) >> 16; \
    *(data + 3) = ((addr) & 0xff000000) >> 24;

static void hookfunc(uintptr_t address, uintptr_t hookAddress, sint32 stacksize)
{
    sint32 i = 0;
    uint8 data[HOOK_BYTE_COUNT] = {0};

    uintptr_t registerAddress = (uintptr_t) &gHookRegisters;

    data[i++] = 0x89; // mov [gHookRegisters], eax
    data[i++] = (0b000 << 3) | 0b101;
    write_address_strictalias(&data[i], registerAddress);
    i += 4;

    data[i++] = 0x89; // mov [gHookRegisters + 4], ebx
    data[i++] = (0b011 << 3) | 0b101;
    write_address_strictalias(&data[i], registerAddress + 4);
    i += 4;

    data[i++] = 0x89; // mov [gHookRegisters + 8], ecx
    data[i++] = (0b001 << 3) | 0b101;
    write_address_strictalias(&data[i], registerAddress + 8);
    i += 4;

    data[i++] = 0x89; // mov [gHookRegisters + 12], edx
    data[i++] = (0b010 << 3) | 0b101;
    write_address_strictalias(&data[i], registerAddress + 12);
    i += 4;

    data[i++] = 0x89; // mov [gHookRegisters + 16], esi
    data[i++] = (0b110 << 3) | 0b101;
    write_address_strictalias(&data[i], registerAddress + 16);
    i += 4;

    data[i++] = 0x89; // mov [gHookRegisters + 20], edi
    data[i++] = (0b111 << 3) | 0b101;
    write_address_strictalias(&data[i], registerAddress + 20);
    i += 4;

    data[i++] = 0x89; // mov [gHookRegisters + 24], ebp
    data[i++] = (0b101 << 3) | 0b101;
    write_address_strictalias(&data[i], registerAddress + 24);
    i += 4;

    // work out distance to nearest 0xC
    // (esp - numargs * 4) & 0xC
    // move to align - 4
    // save that amount

    // push the registers to be on the stack to access as arguments
    data[i++] = 0x68; // push gHookRegisters
    write_address_strictalias(&data[i], registerAddress);
    i += 4;

    data[i++] = 0xE8; // call

    write_address_strictalias(&data[i], hookAddress - address - i - 4);
    i += 4;


    data[i++] = 0x83; // add esp, 4
    data[i++] = 0xC4;
    data[i++] = 0x04;

    data[i++] = 0x25; // and eax,0xff
    data[i++] = 0xff;
    data[i++] = 0x00;
    data[i++] = 0x00;
    data[i++] = 0x00;
    data[i++] = 0xc1; // shl eax, 8
    data[i++] = 0xe0;
    data[i++] = 0x08;
    data[i++] = 0x9e; // sahf
    data[i++] = 0x9c; // pushf

    data[i++] = 0x8B; // mov eax, [gHookRegisters]
    data[i++] = (0b000 << 3) | 0b101;
    write_address_strictalias(&data[i], registerAddress);
    i += 4;

    data[i++] = 0x8B; // mov ebx, [gHookRegisters + 4]
    data[i++] = (0b011 << 3) | 0b101;
    write_address_strictalias(&data[i], registerAddress + 4);
    i += 4;

    data[i++] = 0x8B; // mov ecx, [gHookRegisters + 8]
    data[i++] = (0b001 << 3) | 0b101;
    write_address_strictalias(&data[i], registerAddress + 8);
    i += 4;

    data[i++] = 0x8B; // mov edx, [gHookRegisters + 12]
    data[i++] = (0b010 << 3) | 0b101;
    write_address_strictalias(&data[i], registerAddress + 12);
    i += 4;

    data[i++] = 0x8B; // mov esi, [gHookRegisters + 16]
    data[i++] = (0b110 << 3) | 0b101;
    write_address_strictalias(&data[i], registerAddress + 16);
    i += 4;

    data[i++] = 0x8B; // mov edi, [gHookRegisters + 20]
    data[i++] = (0b111 << 3) | 0b101;
    write_address_strictalias(&data[i], registerAddress + 20);
    i += 4;

    data[i++] = 0x8B; // mov ebp, [gHookRegisters + 24]
    data[i++] = (0b101 << 3) | 0b101;
    write_address_strictalias(&data[i], registerAddress + 24);
    i += 4;

    data[i++] = 0x9d; // popf

    data[i++] = 0xC3; // retn

#ifdef __WINDOWS__
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, data, i, 0);
#else
    // We own the pages with PROT_WRITE | PROT_EXEC, we can simply just memcpy the data
    memcpy((void *)address, data, i);
#endif // __WINDOWS__
}

void addhook(uintptr_t address, hook_function *function)
{
    if (!_hookTableAddress) {
        size_t size = _maxHooks * HOOK_BYTE_COUNT;
#ifdef __WINDOWS__
        _hookTableAddress = VirtualAllocEx(GetCurrentProcess(), NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#else
        _hookTableAddress = mmap(NULL, size, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (_hookTableAddress == MAP_FAILED)
        {
            perror("mmap");
            exit(1);
        }
#endif // __WINDOWS__
    }
    if (_hookTableOffset > _maxHooks) {
        return;
    }
    uint32 hookaddress = (uint32)_hookTableAddress + (_hookTableOffset * HOOK_BYTE_COUNT);
    uint8 data[9];
    sint32 i = 0;
    data[i++] = 0xE9; // jmp

    write_address_strictalias(&data[i], hookaddress - address - i - 4);
    i += 4;

    data[i++] = 0xC3; // retn
#ifdef __WINDOWS__
    WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, data, i, 0);
#else
    // We own the pages with PROT_WRITE | PROT_EXEC, we can simply just memcpy the data
    sint32 err = mprotect((void *)0x401000, 0x8a4000 - 0x401000, PROT_READ | PROT_WRITE);
    if (err != 0)
    {
        perror("mprotect");
    }

    memcpy((void *)address, data, i);

    err = mprotect((void *)0x401000, 0x8a4000 - 0x401000, PROT_READ | PROT_EXEC);
    if (err != 0)
    {
        perror("mprotect");
    }
#endif // __WINDOWS__
    hookfunc(hookaddress, (uintptr_t)function, 0);
    _hookTableOffset++;
}

#endif
