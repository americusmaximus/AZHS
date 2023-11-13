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

#include "SoundModule.Import.hxx"

#define SOUND_MODULE_NAME "eacsnd.dll"

namespace SoundModule
{
    struct SoundModuleContainer
    {
        struct
        {
            HMODULE* _Handle; // 0x009e6fc0
        } Module;

        SOUNDMODULEACQUIRECAPABILITIES* _AcquireCapabilities; // 0x009e6d78
        SOUNDMODULESTART* _Start; // 0x009e6d7c
        SOUNDMODULESTOP* _Stop; // 0x009e6d80
        SOUNDMODULEMIXLATENCY* _MixLatency; // 0x009e6d84
        SOUNDMODULEINITIALIZESOUNDBUFFER* _InitializeBuffer; // 0x009e6d94
        SOUNDMODULESTARTRECORDING* _StartRecording; // 0x009e6d88
        SOUNDMODULEPACKETRECORDING* _PacketRecording; // 0x009e6d8c
        SOUNDMODULESTOPRECORDING* _StopRecording; // 0x009e6d90
        SOUNDMODULESERVE* _Serve; // 0x009e6d98
        SOUNDMODULECREATESOUNDBUFFER* _CreateBuffer; // 0x009e6d9c
        SOUNDMODULEREMOVESOUNDBUFFER* _RemoveBuffer; // 0x009e6da0
        SOUNDMODULEPLAYSOUNDBUFFER* _PlayBuffer; // 0x009e6da4
        SOUNDMODULEPOSITIONSOUNDBUFFER* _PositionBuffer; // 0x009e6da8
        SOUNDMODULEVOLUMESOUNDBUFFER* _VolumeBuffer; // 0x009e6dac
        SOUNDMODULELEVELSOUNDBUFFER* _LevelBuffer; // 0x009e6db0
        SOUNDMODULERATESOUNDBUFFER* _RateBuffer; // 0x009e6db4
        SOUNDMODULESTOPSOUNDBUFFER* _StopBuffer; // 0x009e6db8
    };

    extern SoundModuleContainer SoundModuleState;

    void STDCALLAPI AcquireSoundDataLambda(void* buffer, const u32 size);
    void STDCALLAPI StopSoundBufferLambda(const u32 indx);
    void STDCALLAPI AcquireSoundBufferPositionLambda(const u32 p1, const u32 p2, f32x3* position);

    s32 InitializeSoundModule(void);

    typedef const void (*FUN_00565934) (void* data, const u32 offset); // TODO
    static const FUN_00565934 FUN_00565934a = (FUN_00565934)0x00565934; // TODO

    typedef const void(*FUN_00564E04)(const u32 indx); // TODO
    static const FUN_00564E04 FUN_00564e04 = (FUN_00564E04)0x00564e04; // TODO

    typedef const void (STDCALLAPI* FUN_00548C50) (void); // TODO
    static const FUN_00548C50 FUN_00548c50 = (FUN_00548C50)0x00548c50; // TODO

    typedef const void (STDCALLAPI* FUN_00548C14) (void); // TODO
    static const FUN_00548C14 FUN_00548c14 = (FUN_00548C14)0x00548c14; // TODO

    typedef const void(*FUN_00564F70)(const u32 p1, const u32 p2, f32x3* position); // TODO
    static const FUN_00564F70 FUN_00564f70 = (FUN_00564F70)0x00564f70; // TODO

    typedef const void (STDCALLAPI* FUN_00548C58) (const u32 p1, const u32 p2, const u32 p3, const u32 p4, const u32 p5, const u32 p6, void* p7, const u32 p8, const u32 p9); // TODO
    static const FUN_00548C58 FUN_00548c58 = (FUN_00548C58)0x00548c58; // TODO
}