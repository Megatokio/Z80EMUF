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

#include "kio/kio.h"


typedef double Time;
class FD;
class Machine;
class Item;
class Z80;
class Sio;

enum isa_id
{
	isa_none=0,
	isa_unknown,
	isa_Screen,
	isa_Machine,
	isa_Z80,
	isa_Sio,
	isa_SystemTimer,
	isa_Keyboard,
	isa_Joy,
	isa_Mmu
};


//===================================================================


class IsaObject
{
public:

	isa_id		id;

				IsaObject   (isa_id id)					:id(id){}
				IsaObject   (const IsaObject& q)		:id(q.id){}

	IsaObject&  operator=   (const IsaObject& q)		{id = q.id; return *this;}
	virtual     ~IsaObject  ()							{id = isa_none;}

	bool		isA			(isa_id i) volatile const	{return id==i;}
	isa_id		isaId       () volatile const			{return id;}
};














