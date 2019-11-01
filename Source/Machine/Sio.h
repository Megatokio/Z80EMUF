#pragma once
/*	Copyright  (c)	Günter Woigk 2015 - 2019
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


#include "Item.h"
class Machine;




class Sio : public Item
{
protected:
// Item interface:
virtual void	init			();
virtual void	reset			(int32 cc);
virtual bool	input			(int32 cc, uint addr, uint8& byte);
virtual bool	output			(int32 cc, uint addr, uint8 byte);
virtual void	shift_cc		(int32 cc, int32 dis);
virtual void	update			(int32 cc);


private:
	void		update_interrupt(int32);
	void		_reset(int32);

public:
	Sio(Item *, int32 cc_per_byte=9600, uint addr=0x00ff, uint mask=0x00ff, uint c_mask=0x0100);
	~Sio();

	uint		obu_avail()			{ return obuwp-oburp; }
	uint		obu_free()			{ return bu_size - (obuwp-oburp); }

	uint		ibu_avail()			{ return ibuwp-iburp; }
	uint		ibu_free()			{ return bu_size - (ibuwp-iburp); }

	void		store_byte(uint8 c)	{ ibu[ibuwp++ & bu_mask] = c; }
	uint8		get_byte()			{ return obu[oburp++ & bu_mask]; }



	int32		ccpb;				// cc per byte, e.g. 3500000 / 960.0 = 3646
	uint		c_mask;

	// buffers may be filled and read asynchronously without locking!

	enum { bu_mask = 63, bu_size = 64 };

	uint8		obu[bu_size];		// transmit buffer
	int			oburp;				// volatile for Z80 thread
	int			obuwp;				// volatile for Gui thread

	uint8		ibu[bu_size];		// receive buffer
	int			iburp;				// volatile for Gui thread
	int			ibuwp;				// volatile for Z80 thread

	// for emulation of the baud rate:
	int32		cc_obu_next;		// cc last byte sent + cc per byte
	int32		cc_ibu_next;		// cc last byte read + cc per byte

	// control register:
	//	bit 0: 1 = enable interrupt for "out buffer empty"
	//	bit 1: 1 = enable interrupt for "input data available"
	bool		obu_interrupt_enabled;
	bool		ibu_interrupt_enabled;

	// status register:
	//	bit 0: 1 = out buffer empty (and interrupt pending)
	//	bit 1: 1 = input data available (and interrupt pending)
	bool		obu_interrupt(int32 cc)		{ return obu_free() && cc>=cc_obu_next; }
	bool		ibu_interrupt(int32 cc)		{ return ibu_avail() && cc>=cc_ibu_next; }
};



















