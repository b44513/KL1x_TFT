/******************************************************************************
* File:    vectors.h
*
* Purpose: Provide custom interrupt service routines for Kinetis L Family . 
*
* NOTE: This vector table is a superset table, so interrupt sources might be 
*       listed that are not available on the specific Kinetis L Family device you are 
*       using.
******************************************************************************/

#ifndef __VECTORS_H
#define __VECTORS_H     1

// function prototype for default_isr in vectors.c
void default_isr(void);
void spurious_isr(void);
void NMI_isr(void);

typedef void pointer(void);

extern void __startup(void);
extern unsigned long __BOOT_STACK_ADDRESS[];
extern void __iar_program_start(void);

#define VECTOR_PADDING  (pointer*)0xffffffff /*not used*/

/* Flash configuration field values below */
/* Please be careful when modifying any of
 * the values below as it can secure the 
* flash (possibly permanently): 0x400-0x409.
 */
#define CONFIG_1		(pointer*)0xffffffff 
#define CONFIG_2		(pointer*)0xffffffff 
#define CONFIG_3		(pointer*)0xffffffff
#define CONFIG_4		(pointer*)0xfffffffe //b5=1,b4=1,b0=1 div1 fast
//#define CONFIG_4	(pointer*)0xffffdffe //b5=0,b4=1,b0=1 div1 slow works
//#define CONFIG_4	(pointer*)0xffffcefe //b5=0,b4=0,b0=0;div8 slow
//#define CONFIG_4	(pointer*)0xffffeefe //b5=1,b4=0,b0=0 div8 fast 
//#define CONFIG_4	(pointer*)0xffffcffe //b5=0,b4=0,b0=1;div4 slow
//#define CONFIG_4	(pointer*)0xffffeffe //b5=1,b4=0,b0=1;div4 fast
//#define CONFIG_4	(pointer*)0xfffffefe //b5=1,b4=1,b0=0;div2 fast
//#define CONFIG_4	(pointer*)0xffffdefe //b5=0,b4=1,b0=0;div2 slow 

#endif /*__VECTORS_H*/

/* End of "vectors.h" */
