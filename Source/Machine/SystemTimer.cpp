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


// System Timer
// The system timer fires once per shift_cc()
// it is raised at cc = 0
// and cleared at cc = 256


SystemTimer::SystemTimer (Item* machine) :
	Item(machine,isa_SystemTimer)
{
	int_ack_byte = RST38;			// interrupt generating device: RST38 = 0xff = floating bus value
}

SystemTimer::~SystemTimer()
{}

void SystemTimer::init (/*cc=0*/)
{
	irpt = on;						// raise interrupt
	cc_next_update = 256;			// released in update()
}

void SystemTimer::shift_cc (CpuCycle /*cc*/, int32 /*dis*/)
{
	// shift clock cycle time base:
	// --> fire interrupt

	irpt = on;						// raise interrupt
	cc_next_update = 256;			// released in update()
}

void SystemTimer::update (CpuCycle /*cc*/)
{
	// callback:
	// --> reset interrupt

	irpt = off;
	cc_next_update += NEVER;		// => irpt raised again in shift_cc()
}























