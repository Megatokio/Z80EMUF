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
*/

#include "Item.h"


class Sio : public Item
{
	CpuCycle  ccpb;			// clock cycles per byte, e.g. 3500000 / 960.0 = 3646
	Address c_mask;			// the bit in address which selects control vs. data register

	// buffers can be filled and read asynchronously without locking!

	static const int bu_mask = 63, bu_size = 64;

	Byte obu[bu_size];		// transmit buffer
	int	 oburp;				// volatile for Z80 thread
	int	 obuwp;				// volatile for Gui thread

	Byte ibu[bu_size];		// receive buffer
	int	 iburp;				// volatile for Gui thread
	int	 ibuwp;				// volatile for Z80 thread

	// for emulation of the baud rate:
	CpuCycle cc_obu_next;		// cc last byte sent + cc per byte
	CpuCycle cc_ibu_next;		// cc last byte read + cc per byte

	// control register:
	//	bit 0: 1 = enable interrupt for "out buffer empty"
	//	bit 1: 1 = enable interrupt for "input data available"
	bool obu_interrupt_enabled;
	bool ibu_interrupt_enabled;

	// status register:
	//	bit 0: 1 = out buffer empty (and interrupt pending)
	//	bit 1: 1 = input data available (and interrupt pending)
	bool obu_interrupt (CpuCycle cc)	{ return obu_free() && cc>=cc_obu_next; }
	bool ibu_interrupt (CpuCycle cc)	{ return ibu_avail() && cc>=cc_ibu_next; }

	// helper:
	void update_interrupt (CpuCycle);
	void do_reset (CpuCycle);

protected:
// Item interface:
	virtual void init	() override;
	virtual void reset	(CpuCycle) override;
	virtual bool input	(CpuCycle, Address, Byte&) override;
	virtual bool output	(CpuCycle, Address, Byte) override;
	virtual void shift_cc (CpuCycle, int32 dis) override;
	virtual void update	(CpuCycle) override;

public:
	Sio (Item*, CpuCycle cc_per_byte, Address, Address mask, Address c_mask);
	~Sio() override;

	int obu_avail() const noexcept		{ return obuwp-oburp; }
	int obu_free() const noexcept		{ return bu_size - (obuwp-oburp); }

	int ibu_avail() const noexcept		{ return ibuwp-iburp; }
	int ibu_free() const noexcept		{ return bu_size - (ibuwp-iburp); }

	void store_byte (Byte c) noexcept	{ ibu[ibuwp++ & bu_mask] = c; }
	Byte get_byte() noexcept			{ return obu[oburp++ & bu_mask]; }

	void set_ccpb (int n) noexcept		{ ccpb = n; }
};



















