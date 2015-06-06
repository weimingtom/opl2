<s><b>OPL2 will be an entry in the <a href='http://wololo.net/genesis/'>PSP Genesis competition</a>!</b> (If I can get the problems with the PSP version worked out...)</s>

Unfortunately, there were just too many issues to get it working in time. Congratulations to all the winners, though.

### Description ###
OPL2 aims to be a drop-in replacement for the original "Polygon Love 2" (PL2) game engine, to enable play on multiple platforms including Windows, Linux, and Play<b></b>Station Portable (PSP).

For maximum system portability, OPL2 is written in C using the SDL, OpenGL, and OpenAL libraries.

### Notice ###

This project shares data format information with the Hong<b></b>Fire-based project of the same name, but is otherwise unrelated.

The program and its workings in no way share or are derived from the source or binary code of the original executable. They are based solely on reverse-engineering the associated data files.

### Features ###
OPL2 currently supports:
  * Multiple screen resolutions, plus windowed mode
  * SSE acceleration (PC/Macintel?) (auto-detected)
  * VFPU acceleration (PSP only)
  * Loading resources from PL2 archives
  * 3D rendering and skeletal animation
  * Camera control and path following
  * Lua scripting
  * Dialog and menus
  * Sound: BGM, SFX, voice
Additional planned features include:
  * Load .PSD graphics
  * Freeplay (Unlimited H) mode

### Status ###
Tested successfully on Windows (using MinGW) and Ubuntu Linux. PSP version builds but does not run correctly.

Feedback is welcome about whether it builds and runs on your platform (and which platform that is).