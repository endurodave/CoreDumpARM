#include "Fault.h"
#include "CoreDump.h"
#include <stm32f10x.h>

// Don't use more stack within the exception ISR handlers
// until we have the stack pointer.
static unsigned int* _stackPointer;

register int LR __asm("lr");
register int MSP __asm("msp");
register int PSP __asm("psp");

//----------------------------------------------------------------------------
// HardFault_Handler
//----------------------------------------------------------------------------
void HardFault_Handler(void)
{
	// Determine if main stack or process stack is being used. Bit 2 of the 
	// LR (link register) indicates if MSP or PSP stack is used.
	if ((LR & 0x4) == 0)	
		_stackPointer = (unsigned int*)MSP;
	else
		_stackPointer = (unsigned int*)PSP;
	
	// Store a core dump
	CoreDumpStore(_stackPointer, __FILE__, __LINE__, 0);
	
	// Restart CPU
	NVIC_SystemReset();
}

//----------------------------------------------------------------------------
// FaultHandler
//----------------------------------------------------------------------------
void FaultHandler(const char* file, unsigned short line, int auxCode)
{
	// Store a core dump
	CoreDumpStore(0, file, line, auxCode);
	
	// Restart CPU
	NVIC_SystemReset();
}
