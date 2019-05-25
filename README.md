
# NanoRTOS
This is a small, single processor, multithreading, cooperative operating system.  It relies on DOS for basic Disk access.  


## History

- Originally IPC (Intelligent Peripheral Controller)
- Originally NIPC (New Intelligent Peripheral Controller)
- Renamed at some point to PNTHROS  (Panther OS.)

While writing this document about directory structure; I found this HEADER
emitted in loader.c.
I updated it subsequently.

```
  print("Intelligent Peripheral Controller\r\n");
  print("Copyright (c) 1992,1993 Logical Data Corporation\r\n");
  print("All Rights Reserved.\r\n");
  print("Copyright (c) 2019 Freedom Collective\r\n");
  print("No Rights Reserved.\r\n");
  print("Released Open Source 2019;WTFPL");
  print("If you make it better, maybe share that back?\r\n");
```

LDC May still exist.  This code is from before 1995.  LDC Did continue development on their own sysems, even migrating to QNX for the host operating system instead of DOS.  


## What is This?

A Micro Operating system which runs in X86 DOS.

 - [OS Docs](COMMON.IPC/OS.DOC)
 - [Other Notes](COMMON.IPC/PANTHER.DOC)
 - [Loader Docs](COMMON.IPC/LOADER.DOC)
 - [More Docs](SOFTWARE.DOC)

VGA Text Graphics output.

Dynamic loader support; functions can request functions from the core, and get fixed up to patch over the request code.

Built with Turbo C (2.1?)  Probably Borland C 3.1...

ASM is in TASM (Turbo Assembly from Borland) assembly format.

( should track down those)

---

Howto build

tools/bcc.7z is a copy of borland C 3.1 (1992, 27 years old?)

the `MAKEFILE`s are probably bcc make.
the `BATCH.IPC` folder has batch files to do compile, assemble and link.

` `BATCH.IPC`
  - `ML.BAT` - model Large compile ( far pointer default, multiple code and data segments)
  - `QMC.BAT` - modified compact model?
  - `MC.BAT` - model compact.  (seprate code, data, one segment each)
  - `ASM.BAT` - compile some assembly
  - `BACKIPC.BAT` - obsolete backup of sources
  - `RESEXE.FIL` - linker script used in ML
  - `MEDRES.FIL` - linker script for MC
  
- CMDWND - simple toy project that emulates command.com (supports `dir`, and `copy`)
- COMM - COM port driver (IRQ3/4...)
- COMMON.IPC - this would be like 'include'
   - (STRUCT.OS)[COMMON.IPC/STRUCT.OS] - documentation on OS Structures
   - `OS.H` - task structure
   - `LOADER.DOC` - earlier documention before OS.DOC. (deprecated)
   - (OS.DOC)[COMMON.IPC/OS.DOC] - documentation OS fork() and perish(), and Dynamic linking.
   - `MODHEAD.ASM` - module deadstart header - this is the entry point for modules.
   - `MOD.H` - C header for interface to module header.
   - `MMODHEAD.ASM` - C header for interface to medium module header.
   - `BITS.H` - bit math utility macros
   - most of the others are kind of empty, or are actually build products, or reference something else.
   - `NPCA.H` - 
 - DISK - Obsolete SCSI controller.
 - ETHER - Obsolete western digital ether driver + TCP Stack (could be salvaged).
 - PCA - Obsolete proprietary hardware interface to other computers
 - HANDLERS - ??? New Versions of Older things?  Dev things before other things?
 - HOSTESS - This was a driver for a smart COM port card that had offboard CPU to control the com ports, allowing protocols to be offloaded to external card, and sending full packet buffers to CPU.  16/32 port RS-232 for Dialup/Terminal Sharing services.
 - KEYBRD - Keyboard/Mouse Driver; take keyboard scan codes, and make numbers and letters out of them.  Uses a text CONFIG.KBD that gets compiled to a CONFIG.I file.  PROC.C is the processor. UNPROC.C is a de-coimpiler of the configuration.
 - LISP - Hobby project to introduce a command-line LISP processor.  It parses text a little bit (gets expressions).  Doesn't do much
 - __*LOADER*__ - This is the core OS.  
    - EXEC.ASM - Executive Services ( general interface like int 21/syscall) looks like a wrapper for some other fucntions to Regsiter and Request functions, fork, relinquish, ...
    - HUGEHEAD.ASM - entry point code for main to setup stack.
    - LOADER.C - Entry point for OS.  Loads other modules; Core OS.
    - MEMORY.ASM - memory allocation system,  uses a 16 byte header between allocated blocks (span of 1 segment). Alloc(), Free().
    - MISCROU.ASM - C Runtime libraries.  The Borland library required a lot of space, and I didn't use that much of the CRT.
    - OSDISK.ASM - Disk (open/close/read/write/seek) interface to DOS Int 0x21 services.
    - OSLIB.ASM - this would be like the IMPORT library header.  Modules link to this, which requests the address of the CRT routines from the OS core.
    - SWAPPER.ASM - This handles stack(thread) creation, and swapping.  Tasks are given stacks themselves, but are tracked separately so we can get per-module CPU usage.  Also handles interrupt context switching.  Allowing interrupts to use system services and return.
    - MAKEFILE - the script to make this module.
  - PNTHROS - copy of above. (newer?older?)
  - MULTHAND - Not sure... 
  - PARALLEL - port for parallel port on classic PC.
  - PCA - LDC proprietary hardware- PC Adapter (makes PC slave of another system)
  - PCM - PC Mynah was another LDC product which was a terminal emulator, which I wanted to port to this OS, but apparently didn't get very far.
  - POINTER - updates a cursor on the video display based on keyboard/mouse input.
  - TESTS - Probably some tests... very obsolete.
  - UPS - Uninteruptable POwer Supply Monitor that will popup a message / BEEP when there's power issue. (few dozen lines of code)
  - UTILS - These are some extra utility modules that popup in their own GUI displays.
     - MEMSIZE - how much memory is used/available.
     - NODEDUMP - oustanding IO request logger
     - TASKLIST - shows the list of tasks and the numbr of times they have been dispatched to.
     - WNDW_MGR - Maybe a list of windows?
  - VIDEO - Video display
    - NULLVID - export all the routines that do nothing.
    - TEXT - Text UI display manager; exports an interface library( works cross-module)  Provides widndows for other tasks to output into.  Handles things like windows occluding other windows.  Any task is free to update to the display at any point it is running.
    - GRAPHICS - Attempt at a genuine graphical GUI?
