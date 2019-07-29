; ================================================================
;	Example rom with c code for a Z80 system
;	Copyright  (c)	Günter Woigk 1994 - 2015
;					mailto:kio@little-bat.de
; ================================================================

#target rom

_rom_start::		equ	0x0000
_rom_end::			equ	0x4000
_ram_start::		equ	0x8000
_ram_end::			equ	0x10000
_min_heap_size::	equ	0x1000


; the system has 4 pages à 32K:
mmu_addr::	equ		0x00FE
; bit offsets in 0xFE data byte:
bank0rd::	equ		0	
bank0wr::	equ		2
bank1rd::	equ		4
bank1wr::	equ		6


; we have a dual Acia, it's of a yet-to-invent type:
sioA_data		equ	$00ff		; port address
sioB_data		equ	$01ff		; port address
sioA_control	equ	$02ff		; port address
sioB_control	equ	$03ff		; port address
; bit in control/status register:
sio_out_mask 	equ $01 		; in: query TxD interrupt state;  out: enable/disable out interrupt
sio_in_mask		equ	$02			; in: query RxD interrupt state;  out: enable/disable out interrupt


; ================================================================
; Define ordering of code segments in ram:
; these segments produce code in the output file!
; ================================================================

#code 	_HEADER,_rom_start
#code 	_GSINIT				; init code: the compiler adds some code here and there as required
#code 	_HOME				; code that must not be put in a bank switched part of memory.
#code 	_CODE				; most code and const data go here
#code 	_CABS,*,0			; referenced but never (?) actually used by sdcc
#code 	_GSFINAL,*,0		; referenced but never (?) actually used by sdcc
#code 	_INITIALIZER		; initializer for initialized data in ram
#code	_ROM_PADDING		; pad rom file up to rom end
		defs  _rom_end-$$


; ================================================================
; Define variables in ram:
; note: data segments do not produce actual code
; ================================================================

#data 	_DATA, _ram_start	; uninitialized data
#data 	_INITIALIZED		; data initialized from _INITIALIZER
#data	_DABS,*,0			; referenced but never (?) actually used by sdcc
#data	_RSEG,*,0			; referenced but never (?) actually used by kcc

#data 	_HEAP				; heap:
__sdcc_heap_start:	 		; --> sdcc _malloc.c
		ds	_min_heap_size	; minimum required size
		ds	_ram_end-$-1	; add all unused memory to the heap
__sdcc_heap_end: 			; --> sdcc _malloc.c
		ds 	1


; ================================================================
; 	_HEADER segment:
; 	starts at 0x0000
; ================================================================

;	reset vector
;	RST vectors
;	INT vector (IM 1)
;	NMI vector

#CODE _HEADER

; reset vector
RST0::	di
		ld		a,0<<bank0rd + 0<<bank0wr + 1<<bank1rd + 1<<bank1wr
		out		(mmu_addr),a
		jp		init
		defs	0x08-$

RST1::	reti
		defs	0x10-$

RST2::	reti
		defs	0x18-$

RST3::	reti
		defs	0x20-$

RST4::	reti
		defs	0x28-$

RST5::	reti
		defs	0x30-$

; Sio puts opcode RST6 on bus:
RST6::	jp		sio_irpt
		defs	0x38-$

; SystemTimer puts opcode RST7 on bus:
#data _DATA
_system_time	defs	4
#code _GSINIT
		ld		hl,0
		ld		(_system_time),hl
		ld		(_system_time+2),hl
#code _HEADER
RST7::	push	af		
		push	hl
		ld  	hl,_system_time
1$:		ld  	a,(hl)
		inc 	a
		ld  	(hl),a
		inc 	hl
		jr		z,1$
		pop 	hl	
		pop 	af
		ei
		ret


; init:
; globals and statics initialization
; starts with copying the fixed data:

init:	ld		sp,_ram_end
		ld		bc,_INITIALIZER_len		; length of segment _INITIALIZER
		ld		de,_INITIALIZED			; start of segment _INITIALIZED
		ld		hl,_INITIALIZER			; start of segment _INITIALIZER
		ld		a,b
		or		c
		jr		z,$+4
		ldir

		call    _GSINIT					; Initialise global variables
		call	_main					; execute main()

; shut down:
; if main() returns then something went wrong.
; call debugger and on exit restart system.

_exit::	di
		call 	NMI
		rst		0

; non maskable interrupt:
; e.g. call debugger and on exit resume.

		defs   	0x66-$
NMI::	RETN						; add NMI handler here



; sio A or B interrupt fired:
; we enable them in putchar() and getchar() to wait for i/o to become available
; once fired we can disable the interrupts again
sio_irpt:
		push	af
		push	bc
		xor		a
		ld		bc,sioA_control
		out		(bc),a			; switch it off
		ld		bc,sioB_control
		out		(bc),a			; switch it off
		pop		bc
		pop		af
		ei
		ret

sioA_out_wait:
		ld		a,sio_out_mask
		di
		out		(bc),a
		ei
		halt
		pop		af
		
sioA_putchar::
		push	af				; temp store the char
		ld		bc,sioA_control
		in		a,(bc)
		and		sio_out_mask
		jr		z,sioA_out_wait	; wait for transmitter ready
		pop		af
		ld		bc,sioA_data
		out		(bc),a			; send the char
		ret

sioA_in_wait:
		ld		a,sio_in_mask
		di
		out		(bc),a
		ei
		halt
		
sioA_getchar::
		ld		bc,sioA_control
		in		a,(bc)
		and		sio_in_mask
		jr		z,sioA_in_wait	; wait for receiver ready
		ld		bc,sioA_data
		in		a,(bc)			; receive the char
		ret

		

; ----------------------------------------------------------------
; extern void putchar(char);
;
; note:
;
; void putchar(char c) { outchar = c; }
; -->	push  	ix
;		ld	  	ix,#0
;		add	  	ix,sp
;		ld	  	a, 4 (ix)
;		ld	  	(#_outchar),a
;		pop	  	ix
;		ret

_putchar::
		ld	  	hl,2
		add	  	hl,sp
		ld	  	a,(hl)			; a = char
		jr		sioA_putchar
		

; ----------------------------------------------------------------
; extern char getchar(void);
;
; note:
;
; char getchar(void) { return inchar; }
; -->	ld		hl,#_inchar
;		ld		l,(hl)
;		ret

_getchar::
		call	sioA_getchar
		ld		l,a
		ret



; ================================================================
; the payload:
; ================================================================

;#CFLAGS  $CFLAGS --nostdinc -I../sdcc/include	; add some flags for sdcc
#INCLUDE "main.c"								; compile & include file "main.c"
;#INCLUDE LIBRARY "../sdcc/lib/"					; resolve missing global labels
#include standard library

; ================================================================
; calculate some last labels:
; ================================================================

#CODE _GSINIT

		ret							; final ret from initialization code


#CODE _INITIALIZER

_INITIALIZER_len = $ - _INITIALIZER	; calc size of initializer data for ldir in init code














