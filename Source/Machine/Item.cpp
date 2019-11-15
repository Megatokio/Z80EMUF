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

#include <QWidget>
#include <QEvent>
#include "Item.h"
#include "Machine.h"
#include "Z80/Z80.h"


void Item::unlink() noexcept
{
	if (prev) prev->next=next;
	if (next) next->prev=prev;
	prev = next = nullptr;
}

void Item::linkBehind (Item* p) noexcept
{
	prev = p;
	next = p ? p->next : nullptr;
	if (prev) prev->next = this;
	if (next) next->prev = this;
}

Item::Item (Item *behind, IsaID id, Address o_addr, Address o_mask, Address i_addr , Address i_mask) :
	IsaObject(id),
	machine(behind?behind->machine:id==isa_Machine?(Machine*)this:nullptr),
	in_mask(i_mask),
	in_bits(i_addr),
	out_mask(o_mask),
	out_bits(o_addr)
{
	linkBehind(behind);
}

Item::~Item ()
{
	unlink();
}

















