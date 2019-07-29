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

#include <QWidget>
#include <QEvent>
#include "Item.h"
#include "Machine.h"
#include "Z80/Z80.h"



void Item::unlink()
{
	if(prev) prev->next=next;
	if(next) next->prev=prev;
	prev=next=0;
}


void Item::linkBehind( Item* p )
{
	prev = p;
	next = p ? p->next : NULL;
	if(prev) prev->next = this;
	if(next) next->prev = this;
}


Item::Item(Item *behind, isa_id id, uint o_addr, uint o_mask, uint i_addr , uint i_mask)
:
	IsaObject(id),
	machine(behind?behind->machine:id==isa_Machine?(Machine*)this:NULL),
	in_mask(i_mask),		// dflt: no i/o
	in_bits(i_addr),
	out_mask(o_mask),
	out_bits(o_addr),
	int_ack_byte(0),		// 0 => no interrupts
	cc_next_update(NEVER),	// never
	irpt(off)				// no irpt asserted
{
	linkBehind(behind);
}


Item::~Item ( )
{
	unlink();
}

















