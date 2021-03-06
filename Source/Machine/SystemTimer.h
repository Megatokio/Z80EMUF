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


class SystemTimer : public Item
{
public:
	explicit SystemTimer (Item* machine);
	~SystemTimer() override;

// Item interface:
virtual void init (/*cc=0*/) override;
//virtual void reset (Cycle cc) override;
//virtual bool input (Cycle cc, uint addr, uint8& byte) override;
//virtual bool output (Cycle cc, uint addr, uint8  byte) override;
virtual void update (CpuCycle cc) override;
virtual void shift_cc (CpuCycle cc, int32 dis) override;
};

























