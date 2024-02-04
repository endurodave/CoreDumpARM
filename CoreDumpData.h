#ifndef _CORE_DUMP_DATA_H
#define _CORE_DUMP_DATA_H

#include <stdint.h>

#define CORE_DUMP_FILE_NAME_LEN 		100
#define CORE_DUMP_CALL_STACK_SIZE		10

#define KEY_CORE_DUMP_STORED 			0xBA5EBA11
#define SOFTWARE_VERSION				0x00010001

typedef enum { FAULT_EXCEPTION, SOFTWARE_ASSERTION } CoreDumpType;

typedef struct 
{
	uint32_t Key;
	uint32_t NotKey;
	CoreDumpType Type;
	uint32_t LineNumber;
	uint32_t AuxCode;
	uint32_t SoftwareVersion;
	uint32_t R0_register; 
	uint32_t R1_register; 
	uint32_t R2_register; 
	uint32_t R3_register; 
	uint32_t R12_register; 
	uint32_t LR_register;
	uint32_t PC_register;
	uint32_t XPSR_register;
	
	uint32_t CFSR_register;
	uint32_t HFSR_register;
	uint32_t MMFAR_register;
	uint32_t BFAR_register;
	uint32_t AFSR_register;
	char FileName[CORE_DUMP_FILE_NAME_LEN];
	
	uint32_t ActiveCallStack[CORE_DUMP_CALL_STACK_SIZE];
} CoreDumpData;

#endif 
