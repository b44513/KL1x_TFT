/*
 * File:    startup.c
 * Purpose: Generic Kinetis startup code
 *
 * Notes:
 */

#include "common.h"


#pragma section = ".data"
#pragma section = ".data_init"
#pragma section = ".bss"
#pragma section = "CodeRelocate"
#pragma section = "CodeRelocateRam"

/********************************************************************/
void common_startup(void) {

	/* Declare a counter we'll use in all of the copy loops */
	uint32 n;


	/* Declare pointers for various data sections. These pointers
	 * are initialized using values pulled in from the linker file
	 */
	uint8 * data_ram, *data_rom, *data_rom_end;
	uint8 * bss_start, *bss_end;

	SCB_VTOR = (uint32) 0;

	/* Get the addresses for the .data section (initialized data section) */

	data_ram = __section_begin(".data");
	data_rom = __section_begin(".data_init");
	data_rom_end = __section_end(".data_init");
	n = data_rom_end - data_rom;

	/* Copy initialized data from ROM to RAM */
	while (n--)
		*data_ram++ = *data_rom++;

	/* Get the addresses for the .bss section (zero-initialized data) */

	bss_start = __section_begin(".bss");
	bss_end = __section_end(".bss");


	/* Clear the zero-initialized data section */
	n = bss_end - bss_start;
	while (n--)
		*bss_start++ = 0;


	/* Get addresses for any code sections that need to be copied from ROM to RAM.
	 * The IAR tools have a predefined keyword that can be used to mark individual
	 * functions for execution from RAM. Add "__ramfunc" before the return type in
	 * the function prototype for any routines you need to execute from RAM instead
	 * of ROM. ex: __ramfunc void foo(void);
	 */

	uint8* code_relocate_ram = __section_begin("CodeRelocateRam");
	uint8* code_relocate = __section_begin("CodeRelocate");
	uint8* code_relocate_end = __section_end("CodeRelocate");

	/* Copy functions from ROM to RAM */
	n = code_relocate_end - code_relocate;
	while (n--)
	*code_relocate_ram++ = *code_relocate++;
}
/********************************************************************/
