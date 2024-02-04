#ifndef _FAULT_H
#define _FAULT_H

#ifdef __cplusplus
extern "C" {
#endif

#define ASSERT() \
	FaultHandler(__FILE__, (unsigned short) __LINE__, 0)

#define ASSERT_TRUE(condition) \
	do {if (!(condition)) FaultHandler(__FILE__, (unsigned short) __LINE__, 0);} while (0)
	
#define ASSERT_TRUE_AUX(condition, auxCode) \
	do {if (!(condition)) FaultHandler(__FILE__, (unsigned short) __LINE__, (int)auxCode);} while (0)

/// Handles all software assertions in the system.
/// @param[in] file - the file name that the software assertion occurred on
/// @param[in] line - the line number that the software assertion occurred on
void FaultHandler(const char* file, unsigned short line, int auxCode);

#ifdef __cplusplus
}
#endif

#endif 
