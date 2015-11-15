#include "addresses.h"

#if defined(__GNUC__)
#define DISABLE_OPT __attribute__((noinline,optimize("O0")))
#else
#define DISABLE_OPT
#endif // defined(__GNUC__)

int DISABLE_OPT RCT2_CALLPROC_X(int address, int _eax, int _ebx, int _ecx, int _edx, int _esi, int _edi, int _ebp)
{
	int result;
	#ifdef _MSC_VER
	__asm {
		push ebp
		push address
		mov eax, _eax
		mov ebx, _ebx
		mov ecx, _ecx
		mov edx, _edx
		mov esi, _esi
		mov edi, _edi
		mov ebp, _ebp
		call [esp]
		lahf
		pop ebp
		pop ebp
		/* Load result with flags */
		mov result, eax
	}
	#else
	__asm__ volatile ( "\
		\n\
		push %%ebx \n\
		push %%ebp \n\
		push %[address] 	\n\
		mov %[eax], %%eax 	\n\
		mov %[ebx], %%ebx 	\n\
		mov %[ecx], %%ecx 	\n\
		mov %[edx], %%edx 	\n\
		mov %[esi], %%esi 	\n\
		mov %[edi], %%edi 	\n\
		mov %[ebp], %%ebp 	\n\
		call *(%%esp) 	\n\
		lahf \n\
		add $4, %%esp 	\n\
		pop %%ebp \n\
		pop %%ebx \n\
		/* Load result with flags */ \n\
		mov %%eax, %[result] \n\
		" : [address] "+m" (address), [eax] "+m" (_eax), [ebx] "+m" (_ebx), [ecx] "+m" (_ecx), [edx] "+m" (_edx), [esi] "+m" (_esi), [edi] "+m" (_edi), [ebp] "+m" (_ebp), [result] "+m" (result)
		:
		: "eax","ecx","edx","esi","edi","memory"
	);
	#endif
	// lahf only modifies ah, zero out the rest
	return result & 0xFF00;
}

int DISABLE_OPT RCT2_CALLFUNC_X(int address, int *_eax, int *_ebx, int *_ecx, int *_edx, int *_esi, int *_edi, int *_ebp)
{
	int result;
	#ifdef _MSC_VER
	__asm {
		// Store C's base pointer
		push ebp
		push ebx
		// Store address to call
		push address

		// Set all registers to the input values
		mov eax, [_eax]
		mov eax, [eax]
		mov ebx, [_ebx]
		mov ebx, [ebx]
		mov ecx, [_ecx]
		mov ecx, [ecx]
		mov edx, [_edx]
		mov edx, [edx]
		mov esi, [_esi]
		mov esi, [esi]
		mov edi, [_edi]
		mov edi, [edi]
		mov ebp, [_ebp]
		mov ebp, [ebp]

		// Call function
		call [esp]

		// Store output eax
		push eax
		push ebp
		push ebx
		mov ebp, [esp + 20]
		mov ebx, [esp + 16]

		// Get resulting ecx, edx, esi, edi registers

		mov eax, [_edi]
		mov [eax], edi
		mov eax, [_esi]
		mov [eax], esi
		mov eax, [_edx]
		mov [eax], edx
		mov eax, [_ecx]
		mov [eax], ecx

		// Pop ebx reg into ecx
		pop ecx
		mov eax, [_ebx]
		mov[eax], ecx

		// Pop ebp reg into ecx
		pop ecx
		mov eax, [_ebp]
		mov[eax], ecx

		pop eax
		// Get resulting eax register
		mov ecx, [_eax]
		mov [ecx], eax

		// Save flags as return in eax
		lahf
		// Pop address
		pop ebp

		pop ebx
		pop ebp
		/* Load result with flags */
		mov result, eax
	}
	#else
	__asm__ volatile ( "\
		\n\
		/* Store C's base pointer*/     \n\
		push %%ebp        \n\
		push %%ebx        \n\
		\n\
		/* Store %[address] to call*/   \n\
		push %[address]         \n\
		\n\
		/* Set all registers to the input values*/      \n\
		mov %[_eax], %%eax      \n\
		mov (%%eax), %%eax  \n\
		mov %[_ebx], %%ebx      \n\
		mov (%%ebx), %%ebx  \n\
		mov %[_ecx], %%ecx      \n\
		mov (%%ecx), %%ecx  \n\
		mov %[_edx], %%edx      \n\
		mov (%%edx), %%edx  \n\
		mov %[_esi], %%esi      \n\
		mov (%%esi), %%esi  \n\
		mov %[_edi], %%edi      \n\
		mov (%%edi), %%edi  \n\
		mov %[_ebp], %%ebp      \n\
		mov (%%ebp), %%ebp  \n\
		\n\
		/* Call function*/      \n\
		call *(%%esp)      \n\
		\n\
		/* Store output eax */ \n\
		push %%eax \n\
		push %%ebp \n\
		push %%ebx \n\
		mov 20(%%esp), %%ebp \n\
		mov 16(%%esp), %%ebx \n\
		/* Get resulting ecx, edx, esi, edi registers*/       \n\
		mov %[_edi], %%eax      \n\
		mov %%edi, (%%eax)  \n\
		mov %[_esi], %%eax      \n\
		mov %%esi, (%%eax)  \n\
		mov %[_edx], %%eax      \n\
		mov %%edx, (%%eax)  \n\
		mov %[_ecx], %%eax      \n\
		mov %%ecx, (%%eax)  \n\
		/* Pop ebx reg into ecx*/ \n\
		pop %%ecx		\n\
		mov %[_ebx], %%eax \n\
		mov %%ecx, (%%eax) \n\
		\n\
		/* Pop ebp reg into ecx */\n\
		pop %%ecx \n\
		mov %[_ebp], %%eax \n\
		mov %%ecx, (%%eax) \n\
		\n\
		pop %%eax \n\
		/* Get resulting eax register*/ \n\
		mov %[_eax], %%ecx \n\
		mov %%eax, (%%ecx) \n\
		\n\
		/* Save flags as return in eax*/  \n\
		lahf \n\
		/* Pop address*/ \n\
		pop %%ebp \n\
		\n\
		pop %%ebx \n\
		pop %%ebp \n\
		/* Load result with flags */ \n\
		mov %%eax, %[result] \n\
		" : [address] "+m" (address), [_eax] "+m" (_eax), [_ebx] "+m" (_ebx), [_ecx] "+m" (_ecx), [_edx] "+m" (_edx), [_esi] "+m" (_esi), [_edi] "+m" (_edi), [_ebp] "+m" (_ebp), [result] "+m" (result)

		:
		: "eax","ecx","edx","esi","edi","memory"
	);
	#endif
	// lahf only modifies ah, zero out the rest
	return result & 0xFF00;
}
