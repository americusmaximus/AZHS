/*
Copyright (c) 2023 Americus Maximus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include "Native.Basic.hxx"
#include "SoundModule.Basic.hxx"

#if ORIGINAL_SOUND_MODULE_NAME_PATTERN
#define SOUND_MODULE_ACQUIRE_CAPABILITIES_NAME "_iSNDdirectcaps@4"
#define SOUND_MODULE_ACQUIRE_VERSION_NAME "_iSNDdllversion@0"
#define SOUND_MODULE_CREATE_SOUND_BUFFER_NAME "_iSNDdirectcreate3dbuf@28"
#define SOUND_MODULE_INITIALIZE_SOUND_BUFFER_NAME "_iSNDdirectfxinit@12"
#define SOUND_MODULE_LEVEL_SOUND_BUFFER_NAME "_iSNDdirectfxlevel@8"
#define SOUND_MODULE_MIX_LATENCY_NAME "_iSNDdirectmixlatency@0"
#define SOUND_MODULE_PACKET_RECORDING_NAME "_iSNDdirectrecordpacket@4"
#define SOUND_MODULE_PLAY_SOUND_BUFFER_NAME "_iSNDdirectplay3d@32"
#define SOUND_MODULE_POSITION_SOUND_BUFFER_NAME "_iSNDdirectpos3d@12"
#define SOUND_MODULE_RATE_SOUND_BUFFER_NAME "_iSNDdirectrate@8"
#define SOUND_MODULE_REMOVE_SOUND_BUFFER_NAME "_iSNDdirectremovebuf@4"
#define SOUND_MODULE_SELECT_LAMBDAS_NAME "_iSNDdirectsetfunctions@32"
#define SOUND_MODULE_SERVE_NAME "_iSNDdirectserve@0"
#define SOUND_MODULE_START_NAME "_iSNDdirectstart@8"
#define SOUND_MODULE_START_RECORDING_NAME "_iSNDdirectrecordstart@16"
#define SOUND_MODULE_STOP_SOUND_BUFFER_NAME "_iSNDdirectstopbuf@4"
#define SOUND_MODULE_STOP_NAME "_iSNDdirectstop@0"
#define SOUND_MODULE_STOP_RECORDING_NAME "_iSNDdirectrecordstop@0"
#define SOUND_MODULE_VOLUME_SOUND_BUFFER_NAME "_iSNDdirectvol@8"
#else
#define SOUND_MODULE_ACQUIRE_CAPABILITIES_NAME "_iSNDdirectcaps"
#define SOUND_MODULE_ACQUIRE_VERSION_NAME "_iSNDdllversion"
#define SOUND_MODULE_CREATE_SOUND_BUFFER_NAME "_iSNDdirectcreate3dbuf"
#define SOUND_MODULE_INITIALIZE_SOUND_BUFFER_NAME "_iSNDdirectfxinit"
#define SOUND_MODULE_LEVEL_SOUND_BUFFER_NAME "_iSNDdirectfxlevel"
#define SOUND_MODULE_MIX_LATENCY_NAME "_iSNDdirectmixlatency"
#define SOUND_MODULE_PACKET_RECORDING_NAME "_iSNDdirectrecordpacket"
#define SOUND_MODULE_PLAY_SOUND_BUFFER_NAME "_iSNDdirectplay3d"
#define SOUND_MODULE_POSITION_SOUND_BUFFER_NAME "_iSNDdirectpos3d"
#define SOUND_MODULE_RATE_SOUND_BUFFER_NAME "_iSNDdirectrate"
#define SOUND_MODULE_REMOVE_SOUND_BUFFER_NAME "_iSNDdirectremovebuf"
#define SOUND_MODULE_SELECT_LAMBDAS_NAME "_iSNDdirectsetfunctions"
#define SOUND_MODULE_SERVE_NAME "_iSNDdirectserve"
#define SOUND_MODULE_START_NAME "_iSNDdirectstart"
#define SOUND_MODULE_START_RECORDING_NAME "_iSNDdirectrecordstart"
#define SOUND_MODULE_STOP_SOUND_BUFFER_NAME "_iSNDdirectstopbuf"
#define SOUND_MODULE_STOP_NAME "_iSNDdirectstop"
#define SOUND_MODULE_STOP_RECORDING_NAME "_iSNDdirectrecordstop"
#define SOUND_MODULE_VOLUME_SOUND_BUFFER_NAME "_iSNDdirectvol"
#endif

namespace SoundModule
{
    typedef const s32(STDCALLAPI* SOUNDMODULEACQUIRECAPABILITIES) (const HWND hwnd);
    typedef const s32(STDCALLAPI* SOUNDMODULEACQUIREVERSION) (void);
    typedef const s32(STDCALLAPI* SOUNDMODULECREATESOUNDBUFFER) (const u32 p1, const u32 p2, const u32 frequency, const u32 p4, const u32 p5, const u32 p6, const u32 p7); // TODO
    typedef const s32(STDCALLAPI* SOUNDMODULEINITIALIZESOUNDBUFFER) (const u32 type, const s32 decay, const s32 damping);
    typedef const s32(STDCALLAPI* SOUNDMODULELEVELSOUNDBUFFER) (const u32 indx, const u32 value);
    typedef const s32(STDCALLAPI* SOUNDMODULEMIXLATENCY) (void);
    typedef const s32(STDCALLAPI* SOUNDMODULEPACKETRECORDING) (void*);
    typedef const s32(STDCALLAPI* SOUNDMODULEPLAYSOUNDBUFFER) (const u32 indx, const u32 slot, const BOOL looping, const u32 frequency, const u32 volume, const u32 level, const u32 p4, const u32 p5); // TODO
    typedef const s32(STDCALLAPI* SOUNDMODULEPOSITIONSOUNDBUFFER) (const u32 indx, const u32 p2, const u32 p3); // TODO
    typedef const s32(STDCALLAPI* SOUNDMODULERATESOUNDBUFFER) (const u32 indx, const u32 value);
    typedef const s32(STDCALLAPI* SOUNDMODULEREMOVESOUNDBUFFER) (const u32 indx);
    typedef const s32(STDCALLAPI* SOUNDMODULESELECTLAMBDAS) (const SOUNDMODULEACQUIREDATALAMBDA acquire, const SOUNDMODULESTOPSOUNDBUFFERLAMBDA stop, const SOUNDMODULELOGMESSAGELAMBDA log, const SOUNDMODULEUNKNOWN4LAMBDA p4, const SOUNDMODULEUNKNOWN5LAMBDA p5, const SOUNDMODULEUNKNOWN6LAMBDA p6, const SOUNDMODULEACQUIRESOUNDBUFFERPOSITIONLAMBDA position, const SOUNDMODULECONVERTSOUNDSAMPLELAMBDA convert); // TODO
    typedef const s32(STDCALLAPI* SOUNDMODULESERVE) (void);
    typedef const s32(STDCALLAPI* SOUNDMODULESTART) (const s32 options, const HWND hwnd);
    typedef const s32(STDCALLAPI* SOUNDMODULESTARTRECORDING) (void*, void*, void*, void*);
    typedef const s32(STDCALLAPI* SOUNDMODULESTOP) (void);
    typedef const s32(STDCALLAPI* SOUNDMODULESTOPSOUNDBUFFER) (const u32 indx);
    typedef const s32(STDCALLAPI* SOUNDMODULESTOPRECORDING) (void);
    typedef const s32(STDCALLAPI* SOUNDMODULEVOLUMESOUNDBUFFER) (const u32 indx, const u32 value);
}