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
*/

#include "SystemTimer.h"
#include "Item.h"
#include "Z80/Z80opcodes.h"


/*	The system timer fires once per shift_cc()
*/


SystemTimer::SystemTimer(Item* m)
:
	Item(m,isa_SystemTimer)
{
	int_ack_byte = RST38;			// interrupt generating device: RST38 = 0xff = floating bus value
}


SystemTimer::~SystemTimer()
{}


void SystemTimer::init(/*cc=0*/)
{
	irpt = on;						// assert interrupt
	cc_next_update = 256;			// => irpt released in update()
}


/*	raise interrupt
*/
void SystemTimer::shift_cc(int32 /*cc*/, int32 /*dis*/)
{
	irpt = on;						// assert interrupt
	cc_next_update = 256;			// => irpt released in update()
}


/*	remove interrupt:
*/
void SystemTimer::update(int32 /*cc*/)
{
	irpt = off;
	cc_next_update += NEVER;		// => irpt raised again in shift_cc()
}























