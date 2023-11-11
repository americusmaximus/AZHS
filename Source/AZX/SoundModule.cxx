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

#include "SoundModule.hxx"

namespace SoundModule
{
    SoundModuleContainer SoundModuleState;

    // 0x00548c04
    void STDCALLAPI AcquireDataLambda(void* buffer, const u32 size)
    {
        FUN_00565934a(buffer, size);
    }

    // 0x00548be0
    void STDCALLAPI StopBufferLambda(const u32 indx)
    {
        FUN_00564e04(indx);
    }

    // 0x00548bec
    void STDCALLAPI AcquireBufferPosition(const u32 p1, const u32 p2, f32x3* position)
    {
        FUN_00564f70(p1, p2, position);
    }

    // 0x00548e2c
    s32 InitializeSoundModule(void)
    {
        if (*SoundModuleState.Module._Handle != NULL) { return SOUND_MODULE_SUCCESS; }

        *SoundModuleState.Module._Handle = LoadLibraryA(SOUND_MODULE_NAME);

        if (*SoundModuleState.Module._Handle == NULL) { return SOUND_MODULE_FAILURE; }

        {
            const SOUNDMODULEACQUIREVERSION version = (SOUNDMODULEACQUIREVERSION)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_ACQUIRE_VERSION_NAME);

            if (version == NULL) { return SOUND_MODULE_FAILURE; }

            if (version() != SOUND_MODULE_VERSION_120005) { return SOUND_MODULE_INVALID_VERSION_FAILURE; }
        }

        {
            const SOUNDMODULESELECTLAMBDAS lambdas = (SOUNDMODULESELECTLAMBDAS)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_SELECT_LAMBDAS_NAME);

            lambdas((SOUNDMODULEACQUIREDATALAMBDA)AcquireDataLambda,
                (SOUNDMODULESTOPSOUNDBUFFERLAMBDA)StopBufferLambda,
                NULL, NULL,
                (SOUNDMODULEUNKNOWN5LAMBDA)FUN_00548c50,
                (SOUNDMODULEUNKNOWN6LAMBDA)FUN_00548c14,
                (SOUNDMODULEACQUIRESOUNDBUFFERPOSITIONLAMBDA)AcquireBufferPosition,
                (SOUNDMODULEUNKNOWN8LAMBDA)FUN_00548c58);
        }

        *SoundModuleState._AcquireCapabilities = (SOUNDMODULEACQUIRECAPABILITIES)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_ACQUIRE_CAPABILITIES_NAME);
        *SoundModuleState._Start = (SOUNDMODULESTART)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_START_NAME);
        *SoundModuleState._Stop = (SOUNDMODULESTOP)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_STOP_NAME);
        *SoundModuleState._MixLatency = (SOUNDMODULEMIXLATENCY)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_MIX_LATENCY_NAME);
        *SoundModuleState._InitializeBuffer = (SOUNDMODULEINITIALIZESOUNDBUFFER)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_INITIALIZE_SOUND_BUFFER_NAME);
        *SoundModuleState._StartRecording = (SOUNDMODULESTARTRECORDING)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_START_RECORDING_NAME);
        *SoundModuleState._PacketRecording = (SOUNDMODULEPACKETRECORDING)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_PACKET_RECORDING_NAME);
        *SoundModuleState._StopRecording = (SOUNDMODULESTOPRECORDING)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_STOP_RECORDING_NAME);
        *SoundModuleState._Serve = (SOUNDMODULESERVE)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_SERVE_NAME);
        *SoundModuleState._CreateBuffer = (SOUNDMODULECREATESOUNDBUFFER)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_CREATE_SOUND_BUFFER_NAME);
        *SoundModuleState._RemoveBuffer = (SOUNDMODULEREMOVESOUNDBUFFER)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_REMOVE_SOUND_BUFFER_NAME);
        *SoundModuleState._PlayBuffer = (SOUNDMODULEPLAYSOUNDBUFFER)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_PLAY_SOUND_BUFFER_NAME);
        *SoundModuleState._PositionBuffer = (SOUNDMODULEPOSITIONSOUNDBUFFER)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_POSITION_SOUND_BUFFER_NAME);
        *SoundModuleState._VolumeBuffer = (SOUNDMODULEVOLUMESOUNDBUFFER)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_VOLUME_SOUND_BUFFER_NAME);
        *SoundModuleState._LevelBuffer = (SOUNDMODULELEVELSOUNDBUFFER)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_LEVEL_SOUND_BUFFER_NAME);
        *SoundModuleState._RateBuffer = (SOUNDMODULERATESOUNDBUFFER)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_RATE_SOUND_BUFFER_NAME);
        *SoundModuleState._StopBuffer = (SOUNDMODULESTOPSOUNDBUFFER)GetProcAddress(*SoundModuleState.Module._Handle, SOUND_MODULE_STOP_SOUND_BUFFER_NAME);

        return SOUND_MODULE_SUCCESS;
    }
}