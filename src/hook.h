#ifndef _HOOK_H_
#define _HOOK_H_

enum REGISTER_ARGS {
	EAX = 1 << 0,
	EBX = 1 << 1,
	ECX = 1 << 2,
	EDX = 1 << 3,
	ESI = 1 << 4,
	EDI = 1 << 5,
	EBP = 1 << 6,
	END = 0
};

void addhook(int address, int newaddress, int stacksize, int registerargs[], int registersreturned, int eaxDestinationRegister);

#endif
