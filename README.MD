# AutoZone High Stakes

## Project
AZHS is an open source implementation of Electronic Arts' [Need For Speed: High Stakes](https://en.wikipedia.org/wiki/Need_for_Speed:_High_Stakes) version 4.50 from 1999.

## Goals
There are multiple goals this project tries to achieve:
1. Learn how games were made in an era when things had to be done in the code, and not through fancy game engines like today.
2. Have a fully playable game implemented end-to-end, including resource management, audio, and video rendering, as well as support of large screen resolutions.
3. Eventually, to support 64-bit compilation for modern systems, and provide an avenue for development of modern renderer implementations such as DirectX or Vulkan.
4. Also, eventually, to support modern input devices that do not comply with DirectInput, and even port to Linux or Mac.

## Requirements & Dependencies
1. [Microsoft Visual Studio](https://visualstudio.microsoft.com/downloads/)

## Notes
The original game was compiled by Watcom C/C++ compiler, it contains the following sting: `WATCOM C/C++32 Run-Time system. (c) Copyright by WATCOM International Corp. 1988-1995. All rights reserved.`.

Examination of the game files, indicates that three calling conventions were used.
1. The game was compiled with Watcom compiler, and uses watcom calling convention internally and stdcall to call renderer and sound modules.
2. The sound module was compiled with Watcom compiler, `eacsnd.dll`, uses watcom calling concention internally and exports methods with stdcall convention.
3. The renderers are a mixed bag:
	1. DirectX 6.0 renderer module was compiled with Visual C++ compiler, `d3da.dll`, uses cdecl calling convention internally and exports methods with stdcall convention.
	2. DirectX 7.0 renderer module was compiled with Visual C++ compiler, `d3da.dll`, uses cdecl calling convention internally and exports methods with stdcall convention.
	3. Software renderer module was compiled with Watcom compiler, `softtria.dll`, uses watcom calling convention internally and exports methods with stdcall convention.

### Compatibility, Code Style & Standards
In order to be able to run the code not only in the modern operating systems, but also in legacy environments, such as Windows XP, the code base has to abstain from usage of modern C++ language features.

Modern Visual Studio makes the binary dependent on modern runtime libraries that are not available in the legacy systems, therefore the game won't work. In order to make the code work on legacy systems please see [detailed instructions on compatibility](COMPATIBILITY.MD)

### Sound Module
The sound system module, `eacsnd.dll`, is a module that is used by the game to manage sound and sound effects playback. The module is only responsible for playback, and has no insight into anything else. The module calls functions back from the main game to acquire data, signal events, or log messages.

Based on the structure sizes, the original module uses DirectSound version prior to 7.0, since no other inputs or hints were given, I selected version 6.0 for the project, and it aligns well with the release dates.

The original game requires the sound system to be present, no graceful error handling is happening if the file is missing. Moreover, the game mostly expects the module to be valid as well, it does not perform checks whether all required methods were imported correctly, and just assumes they were.

One more thing regarding the callbacks, some are required to be called during the `Serve` call, without it the game goes into endless loop when player attempts to start a race.

### Renderer Modules
#### DirectX 6
The game has multiple DirectX 6 renderers available with slightly different design and capabilities. I added all of them, with appropriate patches for proper operation these days. The patches mostly include fixes to crashes caused by writing out of array bounds in a few places, and some tweaks to settings values, as well as configuration file support. Here are the DirectX 6 renderers:

1. DirectX 6 3DS is a renderer based on the orignal file `d3da.dll` version: 1999.01.15.1121 (6553.1.21.4385) that came with the original game in 1999 and was located in `3dSetup` folder. It looks like it was intended to use with the game, but is totally capable to render the game.
2. DirectX 6 A is a renderer based on the orignal file `d3da.dll` version: 1999.05.10.1318 (6553.5.16.4888) that came with the original game in 1999.
3. DirectX 6 M is a a renderer based on the DirectX 6 renderer `dx6a.dll` from the modern patch. Since the patch is a binarry patch to the `DirectX 6 A`, I untangled it as well, for completeness.

### Other Similar Projects
#### Predecessors
1. [AutoZone Hot Pursuit](https://github.com/americusmaximus/AZHP)
#### Successors
1. [AutoZone Motor City Online](https://github.com/americusmaximus/AZMCO)

## Thanks
1. [My Abandonware](https://www.myabandonware.com/) for providing [Need For Speed: High Stakes](https://www.myabandonware.com/game/need-for-speed-high-stakes-bgf) and the patch 4.50.
3. [VEG.BY](https://veg.by/en/projects/nfs4/) for providing modern patch for the game.

## Legal
1. This is not a complete game. Please purchase software you like!
2. The source code in this repository is mostly produced by reverse engineering the original binaries. There are a couple of exceptions for reverse engineering under DMCA -- documentation, interoperability, fair use. See goals section for the interoperability and fair use cases. The documentation is needed to support those. Also please see an article about [software preservation](https://en.wikipedia.org/wiki/Digital_preservation).
3. Need For Speed, Need For Speed: High Stakes, DirectX, OpenGL, Vulkan, and others are trademarks of their respective owners.

