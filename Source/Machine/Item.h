#pragma once
/*	Copyright  (c)	Günter Woigk 2004 - 2019
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

#include "IsaObject.h"


typedef int32  CpuCycle;	// clock cycle
typedef uint16 Address;	// i/o address
typedef uint8  Byte;
typedef uint16 Word;


/*
	Base class for all "items" in the computer

	The Item interface supports IN and OUT handling at precise cpu cycles
	and running state machines with cpu cycle precision.
*/

class Item : public IsaObject
{
public:			// public access to internal data
				// in general FOR READING ONLY!

	Machine* machine;
	Item*  next;
	Item*  prev;
	Address in_mask,  in_bits;		// address and mask for decoded bits for input
	Address out_mask, out_bits;		// address and mask for decoded bits for output

	Byte  int_ack_byte = 0;			// byte read in int ack cycle (0 = no interupts)
	Address int0_call_address = 0;	// mostly unused
	CpuCycle  cc_next_update = NEVER; // next time to call update()
	bool   irpt = off;				// interrupt asserted by this item

	static const CpuCycle NEVER = 0x7fffffff;

private:
	Item (const Item&) = delete;			// prohibit
	Item& operator= (const Item&) = delete;	// prohibit

protected:
	void linkBehind	(Item*) noexcept;
	void unlink () noexcept;

	/*	Item constructor:
		allocate all required data but do not yet initialize.
		init() will be called after all items have been constructed and attached to the machine.
	*/
	Item (Item*, IsaID,							// link behind or parent, type
		  Address o_addr = 0xffff, Address o_mask = 0,	// no output
		  Address i_addr = 0xffff, Address i_mask = 0);	// no input


// ---------------- P U B L I C -------------------

public:

	virtual ~Item () override;

	// helper:
	bool matches_in	(Address addr) volatile const noexcept	{ return (addr & in_mask)  == in_bits;  }
	bool matches_out (Address addr) volatile const noexcept	{ return (addr & out_mask) == out_bits; }


// ---------------- Item Interface -------------------

	virtual void init (/*cc=0*/) {}
		/*	Initialize item at cpu cycle cc = 0:
			called after all items have been created to power-up the machine
			and whenever the whole machine shall be reset and re-initialized.
			--> update pointers etc.
			--> Do everything the item does when it is powered up. */

	virtual void reset (CpuCycle/*cc*/) {}
		/*	Reset item at clock cycle cc:
			--> run state machine (if any) up to clock cycle cc.
			--> then do whatever the item does when the reset line is activated. */

	virtual bool input	(CpuCycle, Address, Byte&) { return no; }
		/*	Called by the CPU to handle an IN instruction at clock cycle cc.
			The cpu only calls actually selected item(s) for which matches_in() is TRUE.
			--> run state machine (if any) up to clock cycle cc.
			--> then do whatever the item does when there is an input cycle with IOaddr.
			--> store the byte placed on the data bus in Byte.
			If you need to support multiple items reacting to the same address,
				then you must combine the returned byted value with the value already present on the bus
				(the current value in Byte) using operator '&' ('0' bits normally override '1' bits).
			--> return TRUE if cc_next_update was decremented because the state machine (if any)
				now needs an earlier callback to update(). */

	virtual bool output	(CpuCycle, Address, Byte)  { return no; }
		/*	Called by the CPU to handle an OUT instruction at clock cycle cc.
			The cpu only calls actually selected item(s) for which matches_in() is TRUE.
			--> run state machine (if any) up to clock cycle cc.
			--> then do whatever the item does when there is an output cycle with IOaddr and Byte.
			--> return TRUE if cc_next_update was decremented because the state machine (if any)
				now needs an earlier callback to update(). */

	virtual void update	(CpuCycle) { cc_next_update = NEVER; }
		/*	Called by the CPU when the clock cycle reaches this->cc_next_update.
			May be called too early or erronously.
			--> run state machine (if any) up to clock cycle cc.
			--> Do whatever the item does when clock cycle cc is reached.
			--> update this->cc_next_update.
			Note: Items which need very frequent callbacks, most notably a video controller,
				may need special tests and callbacks whenever the CPU writes a byte to memory,
				e.g. to allow precise emulation of when bytes written to the video ram become
				visible. This should be implemented in macro POKE in Z80macros.h. */

	virtual void shift_cc (CpuCycle /*cc*/, int32 /*dis*/) { cc_next_update = NEVER; }
		/*	Called by the CPU to rewind the clock cycles time base.
			--> run state machine (if any) up to clock cycle cc.
			--> then rewind all your internal time stamps and cc_next_update by dis clock cycles.
			Clock cycle counting starts at the major regular timing event in the system,
			preferably the start of a TV video frame, else the main system timer. */
};
































































