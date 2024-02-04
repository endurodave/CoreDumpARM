#ifndef _CORE_DUMP_H
#define _CORE_DUMP_H

#include <stdint.h>
#include <stdbool.h>
#include "CoreDumpData.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Store a core dump data structure. 
/// @param[in] stackPointer - a pointer to the top of stack. This is either 
/// the main stack pointer (MSP) or process stack pointer (PSP). During an 
/// ISR, registers values are automatically pushed onto the stack by the CPU.
/// The stack values are saved as part of core dump. Set to 0 if not calling 
/// this function from an ISR handler.
/// @param[in] fileName - the software fault file name or 0 if no name.
/// @param[in] lineNumber - the line number within the file name that caused
/// the software fault or 0 if no line number. 	
/// @param[in] auxCode - a user-defined number to store inside the dump file.
void CoreDumpStore(uint32_t* stackPointer, const char* fileName, 
	uint32_t lineNumber, uint32_t auxCode);

/// Get the core dump data structure. 
/// @return The core dump data structure. 
CoreDumpData* CoreDumpGet(void);
	
/// Get the CPU startup state. 
/// @return True if the ARM rebooted and a core dump was stored. False if the  
/// ARM booted normally with no core dump. 
bool IsCoreDumpBoot(void);

/// Reset the core dump to allow saving new crash data.
void CoreDumpReset(void);

#ifdef __cplusplus
}
#endif
	
#endif 
