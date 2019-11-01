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

#include "kio/kio.h"
#include "Item.h"
#include "Z80options.h"
#include "unix/pthreads.h"


// the implemented example Mmu supports 128 kB of memory organized in 4 pages á 32 kB.
// the first page is used as the system's rom. (that is: never paged in for writing.)
#define pagesize	0x8000u
#define	numpages	4u


class Machine : public Item
{
	PLock	_lock;

public:
	class Z80*			cpu;		// first item
	class Mmu*			mmu;
	class SystemTimer*	system_timer;
	class Sio*			sio_A;
	class Sio*			sio_B;

	cstr	romfile;
	int32	ccps;			// cpu cycles per second
	int32	fps;			// frames per second (system timer frequency)
	int32	ccpf;			// cpu cycles per frame

	CoreByte memory[numpages*pagesize];

	pthread_t z80_thread;
	bool	terminate;


	Machine (int32 ccps, int fps, cstr romfilepath);
	~Machine();

	void run();
	void nmi();
	void setSpeed (int32 ccps);
	void reset();
	void poweron()		{ init(); }

	void lock()			{ _lock.lock(); }
	void unlock()		{ _lock.unlock(); }
	bool is_locked()	{ return _lock.trylock()==0; }		// only for error checking

// Item interface:
protected:
virtual void	init		(/*cc=0*/);
//virtual void	reset		(int32);
//virtual bool	input		(int32, uint, uint8&)		{return no;}
//virtual bool	output		(int32, uint, uint8)		{return no;}
//virtual void	update		(int32);
//virtual void	shift_cc	(int32, int32);
};

















