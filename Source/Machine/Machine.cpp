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

#include "Machine.h"
#include "Z80/Z80.h"
#include "unix/FD.h"
#include "SystemTimer.h"
#include "Sio.h"
#include "unix/pthreads.h"
#include "Mmu.h"


static void* start_z80 (void* machine)
{
	// run the Z80 on another thread:

	static_cast<Machine*>(machine)->run();
	return nullptr;
}

void Machine::run()
{
	double time = now();

	while (!terminate)
	{
		// run the machine for 1 frame:
		lock();
			cpu->run(ccpf);
			CpuCycle cc = cpu->cc;
			for (Item* item = this; item; item = item->next)
			{
				item->shift_cc(cc,ccpf);
			}
		unlock();

		// wait for 1 frame to to pass by in realtime:
		waitUntil(time += 1.0/fps);
	}
}

Machine::Machine (int32 ccps, int fps, cstr romfilepath) :
	Item(nullptr,isa_Machine),
	cpu(new Z80(this,isa_Z80)),
	mmu(new Mmu(cpu)),
	system_timer(new SystemTimer(cpu)),
	sio_A(new Sio(cpu,ccps/(SIO_A_BAUD/10), SIO_A, SIO_MASK, SIO_CTL)),
	sio_B(new Sio(cpu,ccps/(SIO_B_BAUD/10), SIO_B, SIO_MASK, SIO_CTL)),
	romfile(newcopy(romfilepath)),
	ccps(ccps),
	fps(fps),
	ccpf((ccps+fps/2)/fps),
	terminate(no)
{
	// Power-on the machine:
	init();

	// start Z80 thread:
	int e = pthread_create( &z80_thread, nullptr /*attr*/, start_z80, this );
	if(e) abort("pthread_create: %s", strerror(e));
}

Machine::~Machine()
{
	terminate = true;
	pthread_join(z80_thread,nullptr);

	while (next) delete next;	// delete items
	delete[] romfile;
}

void Machine::init()
{
	// Item interface:
	// initialize the machine
	// power-on reset

	// reload rom:
	// normally this is done only once in Machine c'tor,
	// but reloading also the rom here may speed up edit-compile-run cycles
	Byte bu[pagesize];
	FD fd(romfile);
	uint32 readsize = min(pagesize,uint(fd.file_size()));		// number of bytes to read
	fd.read_bytes(bu,readsize);									// read into buffer

	lock();
		// copy rom file data into rom
		// clear flag bytes in the upper half of the CoreBytes (though not used in this example system)
		// clear ram filling in some noise
		uint32 i=0;
		for (;i<readsize;i++) memory[i] = bu[i];				// copy uint8 buffer to uint16 memory
		for (;i<pagesize;i++) memory[i] = 0x00FF;				// clear rest of rom (if any)
		for (;i<pagesize*numpages; i++) memory[i] = i&0x00FF;	// clear ram filling in some noise

		// init all attached items:
		// the Mmu will map the memory into the Z80 address space.
		for (Item* item=next; item; item=item->next)
		{
			item->init();
		}
	unlock();
}

void Machine::reset (CpuCycle cc)
{
	// Item interface:
	// push reset

	lock();
		cc = max(cc,cpu->cc);
		for (Item* item=next; item; item=item->next)
		{
			item->reset(cc);
		}
	unlock();
}

void Machine::nmi()
{
	lock();
		cpu->setNmi();
		cpu->clearNmi();	// remains triggered!
	unlock();
}

void Machine::setSpeed (CpuCycle new_ccps)
{
	lock();
		ccps = new_ccps;
		ccpf = (ccps+fps/2)/fps;
		sio_A->set_ccpb(ccps/(SIO_A_BAUD/10));
		sio_B->set_ccpb(ccps/(SIO_B_BAUD/10));
	unlock();
}



































