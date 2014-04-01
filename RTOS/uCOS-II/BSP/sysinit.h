/*
 * File:        sysinit.h
 * Purpose:     Kinetis L Family Configuration
 *              Initializes clock and UART to a default state
 *
 * Notes:
 *
 */

/********************************************************************/

#ifndef __SYSINIT_H__
#define __SYSINIT_H__

void sysinit (void);
unsigned int sys_get_core_freq_in_hz(void);

#endif
/********************************************************************/
