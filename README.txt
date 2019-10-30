

This is a Z80 Project Template 

started 2015-05-31
and it is work in progress.


Goal:

Simple Z80 system to serve as a project template.
As my Z80 emulation got fatter and fatter it was no longer suited for starting a new project.
So i removed everything optional and started anew.

This provides a working system and you can start adding and modifying it.
The Project is based on Qt, thus the GUI stuff is Qt, and it is maintained on Mac OSX.
Internally the project uses POSIX files, threads and more.



Z80 System

Z80 CPU
32k Ram         $0000 - $7FFF
32k Rom         $8000 - $FFFF
Timer interrupt
Mini SIO


Z80
configurable with #defines
  minimum configuration: 
  - 8 bit core bytes => no per-byte options
  - no cycle counting (and no wait states)
  - running full speed except for HALT 

  typical configuration:
  - 8 bit core bytes or 8+8 bit core bytes with options (to be defined)
  - cycle counting
  - no wait states
  - speed limited to an adjustable clock frequency
  
  
Memory
  $0000 - $7FFF   32k Ram         
  $8000 - $FFFF   32k Rom/NVRam (evtl. Flash Rom emulation)

  After reset the Rom is visible in the upper and in the lower bank.
  - The emulator just copies the first 3 bytes from upper Rom to Ram.
  In a real system there might be a flip flop which is set on reset 
  and cleared after a certain i/o or memory read or write condition.


Timer
Emulation of a 50 Hz system timer interrupt.
  

Mini SIO
  Emulation of a minimalistic 2 channel serial interface.
  It assumes HW handshake (e.g. can't overrun on the emulated serial line)
  and a preconfigured baud rate without any setup. 
  In the simplest setup probably no speed limiter is emulated.
  In a more complex setup later, 9600, 19200 (default) and 38400 are likely.

  I/O address:    $FF  
  3 registers:
    $FCFF  port A TXD and RXD
    $FDFF  port B TXD and RXD
    $FEFF  port C control port and status register
    
    read bits in status register:
    0 port A: 1 = out buffer empty (and interrupt pending)
    1 port B: 1 = out buffer empty (and interrupt pending)
    2 port A: 1 = input data available (and interrupt pending)
    3 port B: 1 = input data available (and interrupt pending)
    4-7 unused
  
    write bits in control port:
    0 port A: 1 = enable interrupt for "out buffer empty"    
    1 port B: 1 = enable interrupt for "out buffer empty"
    2 port A: 1 = enable interrupt for "input data available"    
    3 port B: 1 = enable interrupt for "input data available"
    4-7 unused

    reading RXD or writing TXD clears the interrupt, 
    or it can be disabled by writing '0' into it's control bit of port C.

  Port A is connected to a virtual character terminal (for output)
  and the keyboard (for input).

  Port B may be connected to a pipe or file.
  
  





