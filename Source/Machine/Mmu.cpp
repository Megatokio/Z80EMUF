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

#include "Mmu.h"
#include "Item.h"
#include "Z80/Z80.h"
#include "Machine.h"


// The emulated example Z80 system contains 4 pages of 32 kB:
//	32K rom and 3*32K ram. these are stored in Machine.memory[].
//
// The emulated system can page in memory in 2 banks:
//	bank 0 = 0x0000 - 0x7FFF and
//	bank 1 = 0x8000 - 0xFFFF
//
// For each bank one of the 4 possible pages can be selected for reading or writing.
//	this is configured in the out byte to address $FE:
//
#define	bank0_readpage_mask		0x03
#define	bank0_writepage_mask	0x0C
#define	bank1_readpage_mask		0x30
#define	bank1_writepage_mask	0xC0
//
// after reset the bank register is reset to 0x00 which maps rom for reading and writing in both banks.
// this has to be fixed by the system boot code immediately. ;-)
//
// Note that it is possible to map two different pages for reading and writing in a bank and
//	that it is possible to map a single page for reading and/or writing in both banks at the same time.


// MMU is selected by port address 0xFE:
#define o_addr	0x00FE
#define	o_mask	0x00FF




Mmu::Mmu(Item* prev)
:
	Item(prev, isa_Mmu, o_addr, o_mask),
	current_state(),
	cpu()
{}


Mmu::~Mmu()
{}


void Mmu::init(/*cc=0*/)
{
	// search the cpu: we may need it frequently.
	cpu = machine->cpu;

	// reset memory mapping.
	// unmapping write pages (as in reset()) is not required: the cpu already did it.
	current_state = 0x00;
	cpu->mapRom(0x0000, pagesize, machine->memory);
	cpu->mapRom(0x8000, pagesize, machine->memory);
}


void Mmu::reset(int32)
{
	// reset memory mapping.
	current_state = 0x00;
	cpu->unmapWom(0x0000,(uint16)0x10000);
	cpu->mapRom(0x0000, pagesize, machine->memory);
	cpu->mapRom(0x8000, pagesize, machine->memory);
}


bool Mmu::output(int32, uint addr, uint8 byte)
{
	assert(matches_out(addr));

	uint toggled = byte ^ current_state;
	if(!toggled) return no;
	current_state = byte;

	// map page for reading in bank 0:
	// rom and ram is readable, so no different handling:
	if(toggled & bank0_readpage_mask)
	{
		uint page = byte & bank0_readpage_mask;
		cpu->mapRom(0x0000,0x8000,machine->memory+(page<<15));
	}

	// map page for writing in bank 0:
	// rom is not readable so page is unmapped instead:
	if(toggled & bank0_writepage_mask)
	{
		uint page = byte & bank0_writepage_mask;
		if(page) cpu->mapWom(0x0000,0x8000,machine->memory+(page<<13));
		else cpu->unmapWom(0x0000,0x8000);
	}

	// map page for reading in bank 1:
	// rom and ram is readable, so no different handling:
	if(toggled & bank1_readpage_mask)
	{
		uint page = byte & bank1_readpage_mask;
		cpu->mapRom(0x8000,0x8000,machine->memory+(page<<11));
	}

	// map page for writing in bank 1:
	// rom is not readable so page is unmapped instead:
	if(toggled & bank1_writepage_mask)
	{
		uint page = byte & bank1_writepage_mask;
		if(page) cpu->mapWom(0x8000,0x8000,machine->memory+(page<<9));
		else cpu->unmapWom(0x8000,0x8000);
	}

	return no;
}








