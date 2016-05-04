#ifndef _COMMON_H_
#define _COMMON_H_

#include "diagnostic.h"
#include "rct2.h"

#define SafeFree(x) do { free(x); (x) = NULL; } while (0)

#define SafeDelete(x) do { delete (x); (x) = nullptr; } while (0)
#define SafeDeleteArray(x) do { delete[] (x); (x) = nullptr; } while (0)

#ifndef interface
	#define interface struct
#endif
#define abstract = 0

#if defined(__i386__) || defined(_M_IX86)
#define PLATFORM_X86
#endif

#ifdef PLATFORM_X86
#ifdef __GNUC__
#define FASTCALL __attribute__((fastcall))
#elif _MSC_VER
#define FASTCALL __fastcall
#else
#pragma message "Not using fastcall calling convention, please check your compiler support"
#define FASTCALL
#endif
#else // PLATFORM_X86
#define FASTCALL
#endif // PLATFORM_X86

#endif
