#include "CoreDump.h"
#include <string.h>
#include <stm32f10x.h>

#define FLASH_END 	(FLASH_BASE + 0xFFFFF)
#define RAM_BEGIN 	SRAM_BASE
#define RAM_END		(RAM_BEGIN + 0x1FFFF)

static const int MAX_STACK_DEPTH_SEARCH = 100;

// Any data in this section is not initialized to 0's. Requires that the linker
// scatter file (.sct) has this section defined. 
#pragma arm section zidata = "NoInit"

// Core dump data structure to store registers and data related to a fault. 
// This structure needs to reside in a section that is not initialized to 0's at
// startup. This allows a fault to occur, store the data in this structure, 
// reboot the CPU and the data still persists. 
static CoreDumpData _coreDumpData;

#pragma arm section zidata // back to default (.bss section)

register int R0 __asm("r0");
register int R1 __asm("r1");
register int R2 __asm("r2");
register int R3 __asm("r3");
register int R12 __asm("r12");
register int SP __asm("sp");
register int LR __asm("lr");
register int PC __asm("pc");
register int XPSR __asm("xpsr");
register int MSP __asm("msp");
register int PSP __asm("psp");
register int CONTROL __asm("control");

//----------------------------------------------------------------------------
// StoreCallStack
//----------------------------------------------------------------------------
static void StoreCallStack(uint32_t* stackPointer, uint32_t* stackStoreArr, int stackStoreArrLen)
{
	int stackDepth = 0;
	int depth = 0;
	
	// Clear the core dump call stack storage
	memset(stackStoreArr, 0, sizeof(uint32_t) * stackStoreArrLen);
	
	// Ensure the stack pointer is within RAM address range
	if (((uint32_t)stackPointer < RAM_BEGIN || (uint32_t)stackPointer > RAM_END))
		return;
		
	// Search the stack for address values within the flash address range. 
	// We're looking for stored LR (link register) values pushed onto the stack.
	// During post-mortem crash analysis, an address-to-line tool converts 
	// each address to a source file and line number. 
	for (depth = 0; depth<MAX_STACK_DEPTH_SEARCH; depth++)
	{
		// Get a 32-bit value from the stack
		uint32_t stackData = *(stackPointer+depth);
		
		// Have we reached the start of the stack?
		if (stackData == 0xEFEFEFEF && *(stackPointer+depth+1) == 0xEFEFEFEF)
			break;
			
		// Is the stack value within the flash address range?
		if (stackData >= FLASH_BASE && stackData <= FLASH_END)
		{
			// Save the function address within the core dump structure
			stackStoreArr[stackDepth++] = stackData;
		}
		
		// Have we reached the maximum stack depth to store?
		if (stackDepth >= stackStoreArrLen)
			break;
	}
}

//----------------------------------------------------------------------------
// CoreDumpStore
//----------------------------------------------------------------------------
void CoreDumpStore(uint32_t* stackPointer, const char* fileName, 
	uint32_t lineNumber, uint32_t auxCode)
{
	//uint32_t stackTop = 0;
	
	// Is a core dump already stored? Then don't overwrite. The first  
	// core dump is what is needed, not any subsequent crashes detected
	// after the first one.
	if (_coreDumpData.Key == KEY_CORE_DUMP_STORED &&
		_coreDumpData.NotKey == ~KEY_CORE_DUMP_STORED) 
		return;
	
	// Set the key indicating a core dump is stored 
	_coreDumpData.Key = KEY_CORE_DUMP_STORED;
	_coreDumpData.NotKey = ~KEY_CORE_DUMP_STORED;
		
	_coreDumpData.SoftwareVersion = SOFTWARE_VERSION;
	_coreDumpData.AuxCode = auxCode;
	
	// If the caller provides a stack pointer it means that this
	// function is being called from an ISR handler and register 
	// values are pushed onto the stack by the CPU. 
	if (stackPointer != 0)
	{
		_coreDumpData.Type = FAULT_EXCEPTION;
		
		// Store all the registers located on the stack
		_coreDumpData.R0_register = *stackPointer;
		_coreDumpData.R1_register = *(stackPointer+1);
		_coreDumpData.R2_register = *(stackPointer+2);
		_coreDumpData.R3_register = *(stackPointer+3);
		_coreDumpData.R12_register = *(stackPointer+4);
		_coreDumpData.LR_register = *(stackPointer+5);
		_coreDumpData.PC_register = *(stackPointer+6);
		_coreDumpData.XPSR_register = *(stackPointer+7);
		
		// Store other registers to assist with exception decoding
		_coreDumpData.CFSR_register = SCB->CFSR;
		_coreDumpData.HFSR_register= SCB->HFSR;
		_coreDumpData.MMFAR_register = SCB->MMFAR;
		_coreDumpData.BFAR_register = SCB->BFAR;
		_coreDumpData.AFSR_register = SCB->AFSR;
	}
	else
	{
		_coreDumpData.Type = SOFTWARE_ASSERTION;		
	}
	
	// Save file name and line number
	_coreDumpData.LineNumber = lineNumber;	
	if (fileName != NULL)
	{
		strncpy(_coreDumpData.FileName, fileName, CORE_DUMP_FILE_NAME_LEN);
		_coreDumpData.FileName[CORE_DUMP_FILE_NAME_LEN-1] = 0;
	}
	
	// Get the stack pointer if none passed in
	if (stackPointer == 0)	
	{
		// Get current stack location using SP register
		stackPointer = (uint32_t*)SP;
	
		// Alternate means of getting active stack pointer without a register
		//stackPointer = (uint32_t*)(&stackTop);		
	}
	
	// Save the current call stack
	StoreCallStack(stackPointer, &_coreDumpData.ActiveCallStack[0], CORE_DUMP_CALL_STACK_SIZE);
}

//----------------------------------------------------------------------------
// CoreDumpGet
//----------------------------------------------------------------------------
CoreDumpData* CoreDumpGet(void)
{
	return &_coreDumpData;
}

//----------------------------------------------------------------------------
// IsCoreDumpBoot
//----------------------------------------------------------------------------
bool IsCoreDumpBoot(void)
{
	// If keys are set then data within the core dump is valid
	if (_coreDumpData.Key == KEY_CORE_DUMP_STORED &&
		_coreDumpData.NotKey == ~KEY_CORE_DUMP_STORED) 
		return true;
	else
		return false;
}

//----------------------------------------------------------------------------
// CoreDumpReset
//----------------------------------------------------------------------------
void CoreDumpReset(void)
{
	_coreDumpData.Key = 0;	
}

