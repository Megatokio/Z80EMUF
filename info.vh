/*
	this file is included by index.cgi
*/


var TITLE = «Z80 EMUF - Z80 Emulation Core and Example System»

var DESCRIPTION =
«Z80 EMUF - Z80 Emulation Core and Example System
This page provides a versatile Z80 emulation and a simple example system which uses it.
The source of the Z80 emulation, the example system emulation and the rom source of the system rom can be downloaded here.»

var KEYWORDS = «emuf z80 example system source download»

var ROBOTS = «index,follow»


var MAIN =
«
h1	Z80 EMUF – Z80 Emulation Core and Example System

p	<img src="404.png">

p	This page describes my Z80 emulation soft core and a simple emulated Z80 system which uses it.
	Source of the Z80 emulation, the Z80 system and the system's Rom is available for download at the end of this page.

p	The emulation handles most aspects of the Z80 CPU exactly:
ul
li	All opcodes are implemented.
li	All illegal opcodes are implemented.
	Note: some illegal opcodes behave differently on some brands of Z80 CPUs, most notably on CMOS versions. I may add support if there is actual request. (<a href='mailto:kio@little-bat.de'>kio</a>)
li	The core can be restricted to emulate a 8080 CPU instead.
li	Exact cpu clock cycle counting. Wait states may be added in modified macro definitions.
li	Exact R register emulation.
li	Flag register: some undefined flag bit modifications are not handled perfectly. Actually no emulation does this. Maybe some other emulations are more precise, but in essence more effort only results in more exactly crashing programs.
li	Support for all 3 interrupt modi. In mode 0 the CPU can read a RST opcode or a CALL opcode with 2 subsequent address bytes. If this ever was used in a real system.
li	No interruption immediately after EI.
li	Exact emulation of HALT opcode.
li	Exact emulation of LDIR and similar opcodes, e.g. when it is overwriting itself.
li	Exact emulation of NMI behaviour.

p	The emulation speed is approx. 1Hz/12Hz (250 MHz on a 3 GHz Intel 64 bit CPU). But will degrade with every complication added. <b>:-)</b> You can add a lot of complications until you hit the clock speed of a real system, though.

p	The Z80 emulation is written in C++. It can be heavily customized by replacing the provided standard macros with own definitions. The core is plain C and could be back-ported to C easily.

p	The example system is written in C++ and uses the Qt framework and POSIX calls. It was developed on Mac OSX and should compile with only tiny tweaks on Linux as well. Windows people have to rewrite the POSIX files and threads (and maybe other parts) if they want to start with the example project. Maybe Cygwin can help.

p	The program in the emulated system's Rom is written in Z80 assembler and C. I used <a href="../zasm-4.0/Distributions/">zasm</a> to assemble it. zasm can include C source with the help of the C compiler sdcc. This program is just to show "yee, it works" and does nothing useful.

p	I reworked the old source of my Z80 emulation to make it usable for starting projects again. Over time it became too fat, because i use it in <a href="../zxsp-osx/Distributions/">zxsp</a>, my ZX Spectrum emulator, and every requirement for zxsp made it's way into the Z80 emulation.


// -----------------------------------------------
h2	Files and Folders in this Directory

pre	build-*			build directory, contains the executable
	config.h		main config file: settings for compilation
	Libraries/		some of my library files from here are included
	Qt/				Qt specific souce files: mostly Gui
	Rom/			Source of target system's Rom
	Uni/			universal sources: mostly Machine and Items
	Z80/			the Z80 emulation (an Item as well)
	Z80EMUF.pro		the Qt project (setup for OSX)
	Z80EMUF 2015-06-05.zip		all files zipped up for download


// -----------------------------------------------
h2	The Z80 Emulation

h3	How it fit's in the sytem

p	There is a small Type hierarchy:

pre	IsaObject
		Item
			Machine
			Z80
			SystemTimer
			Sio
			Mmu

p	<b>IsaObject</b> provides the base for objects which can expose their type. Just a kind of private RTTI. I have reduced this type to the minimum as used in the example system.

p	<b>Item</b> is the base class for anything which provides a functional unit to the computer. They are linked in a chain, so that every item can find any other by walking the chain and comparing the isa_id.

p	<b>Machine</b> is the class for the object which contains all the emulated system's parts. It contains the system memory and instantiates all required Items. <i>Machine</i> itself is based on <i>Item</i>, which is actually not required. I just happened to do it this way in the example project. <i>Machine</i> does not contain any Gui stuff. (Model - View paradigma)

p	<b>Z80</b> The Z80 CPU of course is an <i>Item</i> as well. The current implementation expects that it is the first Item in the linked list of all Items (at least of those which require update(), input() or output() to be called, this excludes the machine :-) )

p	<b>SystemTime</b> In the current implementation of the Z80 emulation all interrupts must be generated by Items, because the Z80 searches the list of all Items for an active interrupt. Therefore the system timer interrupt must be generated by an Item and i implemented it as a distinct Item in this project.

p	<b>Sio</b> The example project provides an emulation of a serial interface. It is not simply an intercepted illegal instruction, to show how things work. It is no real device either, because these are already very complex and hard to understand, making them inappropriate as an example. This Sio just has a data port and a status register where the target code can test for pending interrupts for input and output and a control register where it can enable or disable these interrupts. The emulation provides a real-time emulation of the serial speed. The serial link may be viewed as having hardware handshake without the need to configure it, it can't overflow in any direction. The handling of the Sios in the example Rom is near the bare minimum.

p	<b>Mmu</b> As an example for support of paged memory i have included a MMU (memory management unit) Item. The Machine contains 128 kB of memory, thereof the first 32 kB are handled as Rom (read only) and the rest as Ram. The Mmu can page in any of these four 32K pages for reading and/or writing for the lower and upper address half. If Rom is paged in for writing, then all writes go to a dummy write page. The example project makes no use of the paged memory except that must setup the MMU after reset, it is just provided as an example.

h3	Configurable options

h5	Z80::run(int32 cc_exit, uint options)
p	The emulation itself is run by calling Z80::run(int32 cc_exit, uint options).
p	This runs the Z80 from the current cpu cycle (Z80::cc) to at least <i>cc_exit</i>. Small overshoots are normal, because Z80 instructions are 4 to 23 cpu cycles and may be extended to an unlimited duration by prepending illegal prefixes…

p	During Z80::run() some data is held in local variables, e.g. registers A and F, the cpu cycle counter CC and others. You have to take care for this when you want to peek and poke the registers, e.g. either only do that while the CPU is not currently run() or if you do this from a macro, e.g. for INPUT, then you must extend the macro to provide access to the local variable, e.g. if you want to add wait cycles to the cpu cycle counter CC.
p	The <i>options</i> are not used by the current implementation of the CPU, but will be handy when you implement additional features. E.g. the standard CoreByte is 16 bit, though the Z80 uses 8 bit wide bytes. The additional bits can be used to associate flags with memory locations, e.g. for a debugger or to flag video memory.
p	In case of video memory you may wish to call a CRTC (cathode ray tube controller) in parallel to the CPU so that it exactly reads what the real device would have read. This requires to call the CRTC before each write to video ram, so that it can read up to the current cpu cycle and may – or may not read the old value at the overwritten address. This is time consuming, so you will do this only for video ram, and there only during the critical time when the video ram is actually read, not in the upper or lower border.
p	This is where all the bits come together: The bytes in the video ram are flagged with a dedicated bit, the CPU is run with <i>options</i> which indicate whether this run() is within the screen section, and the macro POKE(A,R) is replaced by an own macro which calls the CRTC if and only if the <i>options</i> and the flag bit at the memory address are set, to update the display up to the current cpu cycle. In total you have to take care for a lot of things, but you can start with and modify a running system, which makes it a little bit easier.

p	File "Z80/macros.h" contains a bunch of macro definitions which hook in for any macro which is not defined. That's your chance! :-)

p	File "Z80/options.h" can be used to define any customizations which do not ultimately require modifications to the Z80 implementation itself.

h5	#define CPU_PAGEBITS 12
p	Defines the page size for the CPU. Even if you don't use paged memory it must be defined.
	If you use paged memory set it to the smallest page size your system supports.
	The class Z80 contains some helper methods to page in and out memory. Supplied address and size must always be a multiple of the page size.

h5	typedef uint16 CoreByte
p	Defines the size of data used per byte in the Z80 system. Normally this is 16 bit, so you have 8 bits for flags. If you need more set it to uint32. If you really don't need any set it to uint8, but you have to remove some methods in Z80 as well, which now become redundant. The compiler will tell you which…

h5	#define Z80_ISA_8080
p	That's new! <b>:-)</b> The emulation can be limited to a 8080 CPU which did not have the $ED, $BC and index register prefix opcodes. No real loss, but it also did not have the relative jumps, which was a pity. But this is nice to test your code for CP/M compatibility. <b>;-)</b>

h5	#define INCR_CC(N)		{ cc += (N); }
	#define INCR_R()		{ r += 1; }
	#define INCR_IC()		/* nop */
p	Increment the cpu cycle counter, R register or a (not existing) instruction counter. There is almost never any need to change these macros.

h5	#define	PEEK(R,A)		{ INCR_CC(3); R=peek(A); }
p	This macro is used for every standard memory read which is not an M1 cycle and takes 3 cpu clock cycles if no wait states are added.

h5	#define	POKE(A,R)		{ INCR_CC(3); poke(A,R); }
p	This macro is used for every standard memory write which takes 3 cpu clock cycles if no wait states are added.

h5	#define	GET_INSTR(R)	{ INCR_CC(4); INCR_R(); INCR_IC(); R=peek(pc++); }
p	Get the first byte of the next instruction. This is a M1 cycle which normally takes 4 cpu clock cycles.
	This is an ideal location to intercept for a debugger.

h5	#define	GET_CB_OP(R)	{ INCR_CC(4); INCR_R(); R=peek(pc++); }
	#define	GET_ED_OP(R)	{ INCR_CC(4); INCR_R(); R=peek(pc++); }
	#define	GET_XY_OP(R)	{ INCR_CC(4); INCR_R(); R=peek(pc++); }
p	Read the second byte after a prefix opcode. These are M1 cycles as well.

h5	#define	GET_XYCB_OP(R)	{ INCR_CC(5); R=peek(pc++); }
p	Read the 4th byte in a IX+CB or IY+CB instruction. I'm not sure whether this couldn't be replaced with a plain GET_N() macro.

h5	#define	GET_N(R)		{ INCR_CC(3); R=peek(pc++); }
	#define	SKIP_N()		{ INCR_CC(3); peek(pc++); }
p	Read or skip one byte fetched at current PC position. This is a specialization of PEEK(R,A).

h5	#define SKIP_5X1CC(RR)	{ INCR_CC(5); }
	#define SKIP_1CC(RR)	{ INCR_CC(1); }
	#define SKIP_2X1CC(RR)	{ INCR_CC(2); }
	#define SKIP_4X1CC(RR)	{ INCR_CC(4); }
	#define SKIP_7X1CC(RR)	{ INCR_CC(7); }
p	Various variants for adding some cpu cycles used in special instructions, see source. These macros are expected to do a wait test in every cycle. This is required for exact emulation of the ZX Spectrum family, the implementation for other systems may vary. E.g. these wait tests are made while the Z80 does not actually access the bus, which is a little bit stupid. The default implementation just adds the requested number of cpu cycles.

h5	#define	OUTPUT(A,B)		{ INCR_CC(4); this->handle_output(A,B); }
p	Handle an OUT instruction. The default implementation walks the linked list of all attached Items to find the requested Item and calls Item::output(cc,a,b). You may add code to add wait states or handle OUT to multiple Items.

h5	#define	INPUT(A,B)		{ INCR_CC(4); B = this->handle_input(A); }
p	Handle an IN instruction. The default implementation walks the linked list of all attached Items to find the requested Item and calls Item::input(cc,a,b&). You may add code to add wait states or handle IN from multiple Items. The target byte, which is passed to Item::input(cc,a,b&), should be preset with $FF so that the Items can "merge" their byte as would happen in a real system. Typical code to do so is:
pre	byte &= my_value;
p	(low bits always won due to TTL driving power and threshold level.)

h5	INFO Callbacks
p	The following macros provide easy hooks for callbacks on certain special opcodes and occasions:

pre	#define	Z80_INFO_CTOR			/* nop */
	#define	Z80_INFO_DTOR			/* nop */
	#define Z80_INFO_IRPT()			/* nop */
	#define	Z80_INFO_NMI()			/* nop */
	#define	Z80_INFO_RETI			/* nop */
	#define	Z80_INFO_RETN			/* nop */
	#define	Z80_INFO_HALT			/* nop */
	#define Z80_INFO_ILLEGAL(CC,PC)	/* nop */
	#define Z80_INFO_POP			/* nop */
	#define Z80_INFO_RET			/* nop */
	#define Z80_INFO_EX_HL_xSP		/* nop */
	#define Z80_INFO_RST00			/* nop */
	#define Z80_INFO_RST08			/* nop */
	#define Z80_INFO_RST10			/* nop */
	#define Z80_INFO_RST18			/* nop */
	#define Z80_INFO_RST20			/* nop */
	#define Z80_INFO_RST28			/* nop */
	#define Z80_INFO_RST30			/* nop */
	#define Z80_INFO_RST38			/* nop */
	#define Z80_INFO_EI				/* nop */
	#define Z80_INFO_LD_R_A			/* nop */
	#define Z80_INFO_LD_I_A			/* nop */


h2	class Item : public IsaObject

p	The class Item defines an interface:

pre	// Item interface:
	virtual void	init		(/*cc=0*/)					{}
	virtual void	reset		(int32)						{}
	virtual bool	input		(int32, uint, uint8&)		{return no;}
	virtual bool	output		(int32, uint, uint8)		{return no;}
	virtual void	update		(int32)						{}
	virtual void	shift_cc	(int32, int32)				{}

p	<b>init()</b> Initialize the system as after Power-On.
	The Machine first creates all Items which must do all their minimal and one-time setup.
	Then all Items a are called their init() method to "power-up". When init() is called they can trust that all other Items exist and that they are linked in an eventually expected order. In the example system the "guaranteed order" is machine -> z80 -> others. When init() is called, the system time (measured in cpu clock cycles 'cc') is 0.

p	<b>reset(int32 cc)</b> may be called any time and therefore provides the current system time as it's argument. reset() is called for all Items in order and all items should do exactly what the real device would do if the reset button of the system was pressed. E.g. there may be items which do not react to a reset.

p	<b>input(int32 cc, uint addr, uint8& byte)</b> Items which react to a Z80 IN instruction must register their input address and the bit mask of the decoded bits when calling the Item's constructor in their constructor. Then the Z80 finds them when it searches for the target of an IN instruction and calls it's input() method. The provided default implementation expects that each address is decoded by at most one Item, systems which (may) contain multiple Items which react to the same address must replace the default macro.

p	The Z80 calls the found Item's input() method with the current cpu cycle, the exact address and a reference where the Item can store it's byte. input() returns a boolean flag which tells the Z80, whether this input() caused changes to it's schedule list: whether this caused the interrupt state of this item to change or whether the Item updated the timestamp for the next update().

p	The byte provided to this call is preset with $FF. This allows the Item to AND in it's bytes which is required if more than one Item reacts to an address, e.g. if the low 5 bits come from the keyboard and bit 7 from the EAR socket, or whatever.

p	The provided implementation does not much magic beyond searching the i/o target and then calling it's input() method.
	A more sophisticated implementation will replace the INPUT macro to add wait cycles, update video ram in a cycle-precise manner and so on.

p	Items may do a time-based job while they live. When their input() method is called those Items will have to catch-up to the provided time stamp before they can provide their byte. See the Sio of the example project.

p	<b>output(int32 cc, uint addr, uint8 byte)</b> Basically the same as for input(), but to handle the OUT instruction.

p	<b>update(int32 cc)</b> Items may do a job in the background and may require to be called at a certain time, e.g. to assert or remove their interrupt request at a foreseen time. See the Sio as an example. If Items just need a regular callback they should use shift_cc() instead.

p	<b>shift_cc(int32 cc, int32 dis)</b> The cpu cycle based system time will overflow approx. every 20 minutes at 3.5 MHz. Basically this can be handled by rewriting all compares from
pre	if(A > B)
p	to
pre	if(A - B > 0)
p	but it's more than likely that you forget this, and debugging won't be easy, because it starts to happen not before running for 20 minutes…
p	Therefore the time base of the system is shifted regularly. It is recommended to synchronize this with either the system timer interrupt or the video frame flyback. (which ideally happen to be the same.) Then the current system time, as passed in every callback to the Item interface, is a fixed point within the video frame which simplifies precise emulation of video ram reading.
p	The example system (which has no video circuitry) calls shift_cc() for all Items once per system timer interrupt and the implementation of the SystemTimer makes use of this.

p	class Item has a protected creconstruconstructorctorator to be called by all derived classes, e.g. by the Z80 constructor:

pre	Item ( Item* behind, isa_id,				// link behind or parent, type
			 uint o_addr=~0, uint o_mask=0,		// no output
			 uint i_addr=~0, uint i_mask=0 );	// no input
p	<i>Item* behind</i> defines the Item behind which this Item is to be linked. The first item is the Machine itself which links behind NULL, then the Z80 behind the Machine and then all other Items behind the Z80.
p	<i>o_addr</i> and <i>o_mask</i> define the address and the decoded address bits for this item, if it reacts to OUT instructions. The default arguments can be used for items without output.
p	<i>i_addr</i> and <i>i_mask</i> define the address and the decoded address bits for this item, if it reacts to IN instructions. The default arguments can be used for items without input.
p	The Z80 uses these informations in it's IN and OUT instruction implementations. See Z80::handle_input() and Z80::handle_output() which are called by the INPUT and OUTPUT macro.



h2	The Z80 EMUF

p.center	<i>EMUF = Einplatinen-Microcomputer für Universelle Festanwendungen – yeah, that's German…</i> :-)
			(Single-board microcomputer for universal but fixed applications)

p	The example Z80 computer comprises of a PCB (let's call it "Machine"), a Z80 CPU, memory (Rom and Ram) which can be paged in by the help of a MMU, a system timer, a Dual ACIA (or two single ACIAs…) an invisible keyboard and a 404 dot matrix display which i happen to actually own.

p	The <b>Machine</b> is represented by an item of class Machine. It contains all the other items, except those related to the Gui which aren't Items.

p	The <b>CPU</b> is implemented by an instance of class Z80. It is used 'as is' without any tweaks. You may soon rewrite some of the macros to supply weird unusual features if you use it. The CPU is run on a separate thread by the machine, which is not required and makes things more complicated (you have to lock the machine sometimes so that you can access Item internals, e.g. to call reset() on them)) but this way it can run on a different core so that i can see more reliably how fast it actually is.

p	The <b>memory</b> consists of 32 kB Rom and 96 kB Ram, provided by a CoreByte array in the Machine.
p	The <b>MMU</b> (memory management unit) is implemented in class Mmu. It can be programmed by writing to port $FE. 4 groups of 2 bits in the i/o byte configure the two 32K banks of the system for reading and writing. 2 bits can just select one of the 4 pages.
pre	#define	bank0_readpage_mask		0x03
	#define	bank0_writepage_mask	0x0C
	#define	bank1_readpage_mask		0x30
	#define	bank1_writepage_mask	0xC0

p	Two <b>ACIAs</b> are implemented by class Sio. These don't represent real devices, because they would be too complicated. But they emulate the real timing of a serial interface and provide an input and output interrupt. This allows to show an example implementation for some kind of real-time Item. The Sios are set up as follows:

pre	sioA_data		equ	$00ff		; port address: RxD/TxD data
	sioB_data		equ	$01ff		; port address
	sioA_control	equ	$02ff		; port address: control / status
	sioB_control	equ	$03ff		; port address
	 
	; bit in control/status register:
	; in:  query TxD or RxD interrupt state
	: out: enable/disable RxD/TxD interrupt
	sio_out_mask 	equ $01 		; TxD interrupt
	sio_in_mask		equ	$02			; RxD interrupt

p	Baud rate is set to 9600 bits/second in the Machine's constructor.
p	Sio A is connected to the keyboard (that is: the MainWindow get's the key press events and calls machine->sio_A->store_byte()) and the 404 LCD display (that is: the MainWindow regularly reads available transmit data from machine->sio_A and sends it to the Display404), Sio B is not used in this example.

p	The <b>SystemTimer</b> is set up to fire once per shift_cc() callback. This is organized in the run loop of the Machine: It wait's for 1/100 sec to pass by and then calls the Z80 to run the corresponding number of cpu cycles. Then it calls shift_cc() to shift the system time base and the SystemTimer uses this opportunity to fire an interrupt.

h2	The Target System's Rom

p	This is a minimalistic rom to show "yee, it works!". It prints some text to the 404 display, reads one keypress back and then displays the system time counter. This shows that all parts in the emulated system work, basically.

p	I opted to include C code, so i can more easily add this or that. The source is assembled with <a href="../zasm-4.0/Distributions/">zasm</a>, which can include C source if the C compiler sdcc (or something very similar) is installed.

p	The Sio is handled very simple (wait and read or write) but already uses the sio interrupts to speed up things.
p	The MMU of the system is not used, but needs to be initialized in the boot code.

p	The Rom and it's source can be found in the folder "Rom" (sic!).


h2	Archive
»


