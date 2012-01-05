#pragma once


#ifdef __cplusplus
extern "C" {
#endif

void init_kernel_thunk();
void set_current_kernel_function_address( uint32_t addr );

extern void (*impl_KernelThunk[336])();

/* DbgPrint		0x008 */
void impl_DbgPrint();
/* KeTickCount	0x09C */
extern unsigned long impl_KeTickCount;

#ifdef __cplusplus
}
#endif