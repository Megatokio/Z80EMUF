#pragma once
/*	Copyright  (c)	Günter Woigk 2015 - 2019
					mailto:kio@little-bat.de

	This file is free software.

	Permission to use, copy, modify, distribute, and sell this software
	and its documentation for any purpose is hereby granted without fee,
	provided that the above copyright notice appears in all copies and
	that both that copyright notice, this permission notice and the
	following disclaimer appear in supporting documentation.

	THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT ANY WARRANTY, NOT EVEN THE
	IMPLIED WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
	AND IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY DAMAGES
	ARISING FROM THE USE OF THIS SOFTWARE,
	TO THE EXTENT PERMITTED BY APPLICABLE LAW.

	Z80 cpu options
*/


/* #####################################################################

			   options and callback functions for Z80
			   this file contains the project-specific tweaks

   ##################################################################### */



#include "settings.h"
#include "kio/kio.h"
#define Z80_have_class_FD


// we don't use the emulated system's actual page size of 32 kB for the Z80's page size
// but slightly smaller pages to keep the dummy read and write pages small
// at the expense of some speed when the system is paging memory.
// The demo Rom doesn't use the paged memory anyway.
#define CPU_PAGEBITS	12		// 4 kB


// standard size for core bytes is 16 bit:
// 8 bit for the CPU plus 8 bit for options
typedef uint16 CoreByte;


// these are the default definitions as defined in Z80macros.h
// and could be left out here:
#define	OUTPUT(A,B)		{ INCR_CC(4); this->handle_output(cc-2,A,B); }
#define	INPUT(A,B)		{ INCR_CC(4); B = this->handle_input(cc-2,A); }
#define UPDATE()		{ cc_next_update = this->handle_update(cc, cc_exit); }







































