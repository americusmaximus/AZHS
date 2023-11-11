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

#include "Basic.hxx"
#include "Native.Basic.hxx"

namespace SoundModule
{
    DLLAPI s32 STDCALLAPI AcquireCapabilities(const HWND hwnd);
    DLLAPI s32 STDCALLAPI AcquireVersion(void);
    DLLAPI s32 STDCALLAPI CreateSoundBuffer(const u32 p1, const u32 p2, const u32 frequency, const u32 p4, const u32 p5, const u32 p6, const u32 id); // TODO
    DLLAPI s32 STDCALLAPI InitializeSoundBuffer(const u32 type, const s32 decay, const s32 damping);
    DLLAPI s32 STDCALLAPI LevelSoundBuffer(const u32 indx, const u32 value);
    DLLAPI s32 STDCALLAPI MixLatency(void);
    DLLAPI s32 STDCALLAPI PacketRecording(void*);
    DLLAPI s32 STDCALLAPI PlaySoundBuffer(const u32 indx, const u32 slot, const BOOL looping, const u32 frequency, const u32 volume, const u32 level, const u32 p4, const u32 p5); // TODO
    DLLAPI s32 STDCALLAPI PositionSoundBuffer(const u32 indx, const u32 p2, const u32 p3); // TODO
    DLLAPI s32 STDCALLAPI RateSoundBuffer(const u32 indx, const u32 value);
    DLLAPI s32 STDCALLAPI RemoveSoundBuffer(const u32 indx);
    DLLAPI s32 STDCALLAPI SelectLambdas(const SOUNDMODULEACQUIREDATALAMBDA acquire, const SOUNDMODULESTOPSOUNDBUFFERLAMBDA stop, const SOUNDMODULELOGMESSAGELAMBDA log, const SOUNDMODULEUNKNOWNLAMBDA unknown, const SOUNDMODULEUNKNOWN5LAMBDA p5, const SOUNDMODULEUNKNOWN6LAMBDA p6, const SOUNDMODULEACQUIRESOUNDBUFFERPOSITIONLAMBDA position, const SOUNDMODULEUNKNOWN8LAMBDA p8); // TODO
    DLLAPI s32 STDCALLAPI Serve(void);
    DLLAPI s32 STDCALLAPI Start(const s32 options, const HWND hwnd);
    DLLAPI s32 STDCALLAPI StartRecording(void*, void*, void*, void*);
    DLLAPI s32 STDCALLAPI Stop(void);
    DLLAPI s32 STDCALLAPI StopSoundBuffer(const u32 indx);
    DLLAPI s32 STDCALLAPI StopRecording(void);
    DLLAPI s32 STDCALLAPI VolumeSoundBuffer(const u32 indx, const u32 value);
}