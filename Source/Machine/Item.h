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



class Item : public IsaObject
{
public:			// public access to internal data
				// in general FOR READING ONLY!

	Machine		*machine;
	Item		*next, *prev;
	uint		in_mask, in_bits;		// address and decoded bits mask for input
	uint		out_mask, out_bits;		// address and decoded bits mask for output
	uint8		int_ack_byte;			// byte read in int ack cycle (0 = no interupts)
	uint16		int0_call_address;		// mostly unused

	int32		cc_next_update;			// next time to call update()
	bool		irpt;					// interrupt asserted

private:
				Item(const Item&);		// prohibit
	Item&		operator=(const Item&);	// prohibit

protected:
	void		linkBehind	(Item*);
	void		unlink		();

				Item		(Item*, isa_id,					 // link behind or parent, type
							 uint o_addr=~0u, uint o_mask=0, // no output
							 uint i_addr=~0u, uint i_mask=0);// no input


// ---------------- P U B L I C -------------------

public:

virtual			~Item		();

	bool		matches_in	(uint addr) volatile const	{ return (addr & in_mask)  == in_bits;  }
	bool		matches_out	(uint addr) volatile const	{ return (addr & out_mask) == out_bits; }

// Item interface:
virtual void	init		(/*cc=0*/)					{}
virtual void	reset		(int32)						{}
virtual bool	input		(int32, uint, uint8&)		{return no;}
virtual bool	output		(int32, uint, uint8)		{return no;}
virtual void	update		(int32)						{}
virtual void	shift_cc	(int32, int32)				{}

	enum { NEVER=0x7fffffff };
};
































































