#include <stm32f10x.h>
#include "CoreDump.h"
#include "Fault.h"

// Defined to generate a hardware exception test. Undefined to 
// generate a software fault test. 
//#define HARD_FAULT_TEST	1

#if HARD_FAULT_TEST
static int val = 2, zero = 0, result;	
#endif

//----------------------------------------------------------------------------
// Call3
//----------------------------------------------------------------------------
int Call3()
{
	int stackArr3[5]= { 0x33333333, 0x33333333, 0x33333333, 0x33333333, 0x33333333 };	
	
#if HARD_FAULT_TEST
	// Cause a divide by 0 exception. HardFault_Handler will be called.
	result = val / zero;
#else
	// Cause a software fault. FaultHandler() will be called. 
	ASSERT();
#endif
	
	return stackArr3[0];
}

//----------------------------------------------------------------------------
// Call2
//----------------------------------------------------------------------------
int Call2()
{
	int stackArr2[5]= { 0x22222222, 0x22222222, 0x22222222, 0x22222222, 0x22222222 };
	Call3();
	return stackArr2[0];
}

//----------------------------------------------------------------------------
// Call1
//----------------------------------------------------------------------------
int Call1()
{
	int stackArr1[5]= { 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111 };
	Call2();
	return stackArr1[0];
}

//----------------------------------------------------------------------------
// main
//----------------------------------------------------------------------------
int main(void) 
{
	// Stack memory patterns to assist in viewing stack frame within debugger
	int stackArr0[5]= { 0xEFEFEFEF, 0xEFEFEFEF, 0xEFEFEFEF, 0xEFEFEFEF, 0xEFEFEFEF };

	// Enable divide by 0 hardware exception
	SCB->CCR |= 0x10;
	
	// Did a core dump get saved?
	if (IsCoreDumpBoot() == true)
	{
		// Get the core dump data structure
		CoreDumpData* coreDumpData = CoreDumpGet();
		
		// TODO - Save core dump to persistent storage or transmit 
		// the data to a remote device.
		
		// Reset core dump for next time. 
		CoreDumpReset();
	}	
	
	// Create call stack by calling a few functions
	Call1();
	
	return stackArr0[0];
}
