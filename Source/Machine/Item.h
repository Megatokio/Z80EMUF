#pragma once
/*	Copyright  (c)	Günter Woigk 2004 - 2019
					mailto:kio@little-bat.de

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

	Permission to use, copy, modify, distribute, and sell this software and
	its documentation for any purpose is hereby granted without fee, provided
	that the above copyright notice appear in all copies and that both that
	copyright notice and this permission notice appear in supporting
	documentation, and that the name of the copyright holder not be used
	in advertising or publicity pertaining to distribution of the software
	without specific, written prior permission.  The copyright holder makes no
	representations about the suitability of this software for any purpose.
	It is provided "as is" without express or implied warranty.

	THE COPYRIGHT HOLDER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
	INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
	EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE FOR ANY SPECIAL, INDIRECT OR
	CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
	DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
	TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
	PERFORMANCE OF THIS SOFTWARE.
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
































































