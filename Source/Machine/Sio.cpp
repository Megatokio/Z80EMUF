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

#include "Sio.h"
#include "Z80/Z80opcodes.h"


Sio::Sio(Item* m, int32 cc_per_byte, uint addr, uint mask, uint creg_mask)
:
	Item(m, isa_Sio, addr,mask, addr,mask),
	ccpb(cc_per_byte),
	c_mask(creg_mask)
{
	int_ack_byte = RST30;
}

Sio::~Sio()
{}


void Sio::_reset(int32 cc)
{
	cc_next_update = NEVER;

	irpt = off;

	obu_interrupt_enabled = no;
	ibu_interrupt_enabled = no;

	oburp = obuwp = 0;
	iburp = ibuwp = 0;

	cc_obu_next = cc;
	cc_ibu_next = cc;
}

void Sio::init(/*cc=0*/)
{
	Item::init();
	_reset(0);
}


void Sio::reset(int32 cc)
{
	Item::reset(cc);
	_reset(cc);
}


void Sio::update_interrupt(int32 cc)
{
	// avoid race condition => read ibu_avail() and obu_free() only once:
	bool i_avail = ibu_avail();
	bool o_free  = obu_free();

	cc_next_update = NEVER;
	irpt = (i_avail && cc>=cc_ibu_next && ibu_interrupt_enabled) ||
		   (o_free  && cc>=cc_obu_next && obu_interrupt_enabled);
	if(irpt) return;

	bool i = ibu_interrupt_enabled && i_avail;
	bool o = obu_interrupt_enabled && o_free;

	cc_next_update = i&&o ? min(cc_ibu_next,cc_obu_next) :
					 i ? cc_ibu_next :
					 o ? cc_obu_next : NEVER;
}


void Sio::shift_cc(int32 cc, int32 dis)
{
	assert(cc>=dis);

	update_interrupt(cc);

	if(cc_obu_next > -ccpb) cc_obu_next -= dis;
	if(cc_ibu_next > -ccpb) cc_ibu_next -= dis;
	cc_next_update -= dis;
}

void Sio::update(int32 cc)
{
	assert(cc>=cc_next_update);

	update_interrupt(cc);
}

bool Sio::output(int32 cc, uint addr, uint8 byte)
{
	assert(matches_out(addr));

	if(addr&c_mask)	// control
	{
		obu_interrupt_enabled = byte & 1;
		ibu_interrupt_enabled = byte & 2;
		bool f = irpt;
		update_interrupt(cc);
		return f != irpt;
	}
	else			// data
	{
		if(obu_interrupt(cc))
		{
			obu[obuwp++ & bu_mask] = byte;
			cc_obu_next = max(cc_obu_next+ccpb, cc);
			update_interrupt(cc);
			return yes;
		}
	}
	return no;
}

bool Sio::input(int32 cc, uint addr, uint8& byte)
{
	assert(matches_in(addr));

	if(addr&c_mask)	// status
	{
		if(!ibu_interrupt(cc)) byte &= ~2;		// input idle
		if(!obu_interrupt(cc)) byte &= ~1;		// output busy
		return no;
	}
	else			// data
	{
		if(ibu_interrupt(cc))
		{
			byte &= ibu[iburp++ & bu_mask];
			cc_ibu_next = max(cc_ibu_next+ccpb, cc);
			update_interrupt(cc);
			return yes;
		}
	}
	byte &= oburp;	// random
	return no;
}





















