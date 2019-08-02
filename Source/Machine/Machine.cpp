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

#include "Machine.h"
#include "Z80/Z80.h"
#include "unix/FD.h"
#include "SystemTimer.h"
#include "Sio.h"
#include "unix/pthreads.h"
#include "Mmu.h"
#include <unistd.h>

/*	run the Z80 on another thread:
*/
void* start_z80(void* m)
{
	((Machine*)m)->run();
	return NULL;
}

void Machine::run()
{
	double time, delay;

	for(time = now(); ; time += 1.0/fps)
	{
		// wait for 1 frame to to pass by in realtime:
		while((delay = time-now()) > 0)
		{
			usleep(delay*1000000);
		}

		// run the machine for 1 frame:
		lock();
			cpu->run(ccpf);
			int32 cc = cpu->cc;
			for(Item* item=this; item; item=item->next)
			{
				item->shift_cc(cc,ccpf);
			}
		unlock();
	}
}


Machine::Machine(uint32 ccps, uint fps, cstr romfilepath)
:
	Item(NULL,isa_Machine),
	cpu(new Z80(this,isa_Z80)),
	mmu(new Mmu(cpu)),
	system_timer(new SystemTimer(cpu)),
	sio_A(new Sio(cpu,ccps/960,0x00ff,0x01ff,0x0200)),
	sio_B(new Sio(cpu,ccps/960,0x01ff,0x01ff,0x0200)),
	romfile(newcopy(romfilepath)),
	ccps(ccps),
	fps(fps),
	ccpf((ccps+fps/2)/fps)
{
//	uint8 bu[romsize];
//	FD fd(romfilepath);
//	fd.read_bytes(bu,min(romsize,fd.file_size()));
//	Z80::b2c(bu,rom,romsize);

	// Power-on the machine:
	init();

	// start Z80 thread:
	int e = pthread_create( &z80_thread, NULL /*attr*/, start_z80, this );
	if(e) abort("executeAt: %s", strerror(e));
}


Machine::~Machine()
{
	lock();
		pthread_cancel(z80_thread);
		while(next) delete next;
	unlock();
	delete[] romfile;
}


/*	initialize the machine
	kind of power-on reset
*/
void Machine::init()
{
	// reload rom:
	// normally this is done only once in Machine c'tor,
	// but reloading also the rom here may speed up edit-compile-run cycles
	uint8 bu[pagesize];
	FD fd(romfile);
	uint32 readsize = min(pagesize,(int)fd.file_size());		// number of bytes to read
	fd.read_bytes(bu,readsize);									// read into buffer

	lock();
		// copy rom file data into rom
		// clear flag bytes in the upper half of the CoreBytes (though not used in this example system)
		// clear ram filling in some noise
		uint32 i=0;
		for(;i<readsize;i++) memory[i] = bu[i];					// copy uint8 buffer to uint16 memory
		for(;i<pagesize;i++) memory[i] = 0x00FF;				// clear rest of rom (if any)
		for(;i<pagesize*numpages; i++) memory[i] = i&0x00FF;	// clear ram filling in some noise

		// init all attached items:
		// the Mmu will map the memory into the Z80 address space.
		for(Item* item=next; item; item=item->next)
		{
			item->init();
		}
	unlock();
}


void Machine::reset(int32 cc)
{
	lock();
		for(Item* item=next; item; item=item->next)
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


void Machine::setSpeed(uint32 new_ccps)
{
	ccps = new_ccps;
	ccpf = (ccps+fps/2)/fps;
	sio_A->ccpb = ccps/960;
	sio_B->ccpb = ccps/960;
}



































