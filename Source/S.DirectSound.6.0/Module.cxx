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

#include "Mathematics.Basic.hxx"
#include "Module.hxx"

#include <stdio.h>

#define MAX_SOUND_MODULE_MESSAGE_LENGTH 512

using namespace Mathematics;

namespace SoundModule
{
    ModuleContainer State;

    GUID DSPROPSETID_VoiceManager = { 0x62a69bae, 0xdf9d, 0x11d1, 0x99, 0xa6, 0x00, 0xc0, 0x4f, 0xc9, 0x9d, 0x46 };
    GUID DSPROPSETID_EAX_ReverbProperties = { 0x4a4e6fc1, 0xc341, 0x11d1, 0xb7, 0x3a, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 };
    GUID DSPROPSETID_EAXBUFFER_ReverbProperties = { 0x4a4e6fc0, 0xc341, 0x11d1, 0xb7, 0x3a, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 };

    // 0x00401670
    // a.k.a. iSNDdirectcaps
    DLLAPI s32 STDCALLAPI AcquireCapabilities(const HWND hwnd)
    {
        if (INVALID_SOUND_MODULE_CAPABILITIES < (s32)State.Settings.Capabilities) { return State.Settings.Capabilities; }

        u32 result = SOUND_MODULE_CAPABILITIES_NONE;

        if (!State.State.IsInit)
        {
            const HRESULT code = DirectSoundCreate(NULL, &State.DX.Instance, NULL);

            if (code != DS_OK)
            {
                if (code == DSERR_ALLOCATED) { result = SOUND_MODULE_ALREADY_ALLOCATED_FAILURE; }
                else if (code == DSERR_NODRIVER) { result = SOUND_MODULE_MISSING_DRIVER_FAILURE; }
                else { result = SOUND_MODULE_FAILURE; }
            }
        }

        if (result == SOUND_MODULE_CAPABILITIES_NONE && State.DX.Instance != NULL)
        {
            DSCAPS caps;
            ZeroMemory(&caps, sizeof(DSCAPS));

            caps.dwSize = sizeof(DSCAPS);

            if (State.DX.Instance->GetCaps(&caps) == DS_OK)
            {
                State.Settings.IsEmulated = caps.dwFlags & DSCAPS_EMULDRIVER;

                WAVEFORMATEX format;
                ZeroMemory(&format, sizeof(WAVEFORMATEX));

                format.nChannels = SOUND_CHANNELS_1;
                format.wBitsPerSample = SOUND_BITS_8;

                if ((caps.dwFlags & DSCAPS_PRIMARY8BIT_MONO) == DSCAPS_PRIMARY8BIT_MONO) { result = result | SOUND_MODULE_CAPABILITIES_8BIT_MONO; }
                if ((caps.dwFlags & DSCAPS_PRIMARY8BIT_STEREO) == DSCAPS_PRIMARY8BIT_STEREO) { result = result | SOUND_MODULE_CAPABILITIES_8BIT_STEREO; }
                if ((caps.dwFlags & DSCAPS_PRIMARY16BIT_MONO) == DSCAPS_PRIMARY16BIT_MONO) { result = result | SOUND_MODULE_CAPABILITIES_16BIT_MONO; }

                if ((caps.dwFlags & DSCAPS_PRIMARY16BIT_STEREO) == DSCAPS_PRIMARY16BIT_STEREO)
                {
                    format.nChannels = SOUND_CHANNELS_2;
                    format.wBitsPerSample = SOUND_BITS_16;

                    result = result | SOUND_MODULE_CAPABILITIES_16BIT_STEREO;
                }

                if (caps.dwMaxHw3DAllBuffers != 0) { result = result | SOUND_MODULE_CAPABILITIES_3D_BUFFER_SUPPORTED; }

                if (State.DX.Instance->SetCooperativeLevel(hwnd, DSSCL_EXCLUSIVE) == DS_OK)
                {
                    DSBUFFERDESC desc;
                    ZeroMemory(&desc, sizeof(DSBUFFERDESC));

                    desc.dwSize = sizeof(DSBUFFERDESC);
                    desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

                    if (State.DX.Instance->CreateSoundBuffer(&desc, &State.DX.Buffers.Main, NULL) == DS_OK)
                    {
                        const u32 alignment = format.nChannels * (format.wBitsPerSample >> 3);

                        format.nAvgBytesPerSec = (alignment & 0xffff) * SOUND_FREQUENCY_11025;
                        format.nBlockAlign = (WORD)alignment;
                        format.nSamplesPerSec = SOUND_FREQUENCY_11025;
                        format.wFormatTag = WAVE_FORMAT_PCM;

                        if (State.DX.Buffers.Main->SetFormat(&format) == DS_OK)
                        {
                            result = result | SOUND_MODULE_CAPABILITIES_FREQUENCY_11025;
                        }

                        format.nSamplesPerSec = SOUND_FREQUENCY_16000;
                        format.nAvgBytesPerSec = format.nBlockAlign * SOUND_FREQUENCY_16000;

                        if (State.DX.Buffers.Main->SetFormat(&format) == DS_OK)
                        {
                            result = result | SOUND_MODULE_CAPABILITIES_FREQUENCY_16000;
                        }

                        format.nSamplesPerSec = SOUND_FREQUENCY_22050;
                        format.nAvgBytesPerSec = format.nBlockAlign * SOUND_FREQUENCY_22050;

                        if (State.DX.Buffers.Main->SetFormat(&format) == DS_OK)
                        {
                            result = result | SOUND_MODULE_CAPABILITIES_FREQUENCY_22050;
                        }

                        format.nSamplesPerSec = SOUND_FREQUENCY_32000;
                        format.nAvgBytesPerSec = format.nBlockAlign * SOUND_FREQUENCY_32000;

                        if (State.DX.Buffers.Main->SetFormat(&format) == DS_OK)
                        {
                            result = result | SOUND_MODULE_CAPABILITIES_FREQUENCY_32000;
                        }

                        format.nSamplesPerSec = SOUND_FREQUENCY_44100;
                        format.nAvgBytesPerSec = format.nBlockAlign * SOUND_FREQUENCY_44100;

                        if (State.DX.Buffers.Main->SetFormat(&format) == DS_OK)
                        {
                            result = result | SOUND_MODULE_CAPABILITIES_FREQUENCY_44100;
                        }

                        BOOL acquire = FALSE;

                        if ((result & SOUND_MODULE_CAPABILITIES_ANY_BITS_ANY) == 0
                            || (result & SOUND_MODULE_CAPABILITIES_FREQUENCY_ANY) == 0)
                        {
                            result = INVALID_SOUND_MODULE_CAPABILITIES;
                            acquire = TRUE;
                        }
                        else if (result == SOUND_MODULE_CAPABILITIES_NONE) { acquire = TRUE; }

                        if (acquire)
                        {
                            if (State.DX.Buffers.Main->GetFormat(&format, sizeof(WAVEFORMATEX), NULL) == DS_OK)
                            {
                                if (format.nChannels == SOUND_CHANNELS_1 && format.wBitsPerSample == SOUND_BITS_8) { result = SOUND_MODULE_CAPABILITIES_8BIT_MONO; }
                                else if (format.nChannels == SOUND_CHANNELS_2 && format.wBitsPerSample == SOUND_BITS_8) { result = SOUND_MODULE_CAPABILITIES_8BIT_STEREO; }
                                else if (format.nChannels == SOUND_CHANNELS_1 && format.wBitsPerSample == SOUND_BITS_16) { result = SOUND_MODULE_CAPABILITIES_16BIT_MONO; }
                                else if (format.nChannels == SOUND_CHANNELS_2 && format.wBitsPerSample == SOUND_BITS_16) { result = SOUND_MODULE_CAPABILITIES_16BIT_STEREO; }
                                else { result = INVALID_SOUND_MODULE_CAPABILITIES; }

                                if (result != INVALID_SOUND_MODULE_CAPABILITIES)
                                {
                                    if (format.nSamplesPerSec == SOUND_FREQUENCY_11025) { result = result | SOUND_MODULE_CAPABILITIES_FREQUENCY_11025; }
                                    else if (format.nSamplesPerSec == SOUND_FREQUENCY_16000) { result = result | SOUND_MODULE_CAPABILITIES_FREQUENCY_16000; }
                                    else if (format.nSamplesPerSec == SOUND_FREQUENCY_22050) { result = result | SOUND_MODULE_CAPABILITIES_FREQUENCY_22050; }
                                    else if (format.nSamplesPerSec == SOUND_FREQUENCY_32000) { result = result | SOUND_MODULE_CAPABILITIES_FREQUENCY_32000; }
                                    else if (format.nSamplesPerSec == SOUND_FREQUENCY_44100) { result = result | SOUND_MODULE_CAPABILITIES_FREQUENCY_44100; }
                                    else { result = INVALID_SOUND_MODULE_CAPABILITIES; }
                                }
                            }
                        }

                        {
                            State.Settings.Is3D = TRUE;

                            InitializeSoundExtensions();

                            if (State.EAX.State.IsReverb) { result = result | SOUND_MODULE_CAPABILITIES_REVERB_SUPPORTED; }

                            ReleaseSoundExtensions();

                            State.Settings.Is3D = FALSE;
                        }
                    }
                }
            }
        }
        else { result = SOUND_MODULE_FAILURE; }

        if (!State.State.IsInit)
        {
            if (State.DX.Instance != NULL)
            {
                State.DX.Instance->Release();
                State.DX.Instance = NULL;
            }
        }

        State.Settings.Capabilities = result == SOUND_MODULE_FAILURE ? INVALID_SOUND_MODULE_CAPABILITIES : result;

        return result;
    }

    // 0x00402fec
    // a.k.a. iSNDdirectcreate3dbuf
    DLLAPI s32 STDCALLAPI CreateSoundBuffer(const u32 volume, const u32 p2, const u32 frequency, const u32 p4, const u32 p5, const u32 p6, const u32 id)
    {
        if (!State.Settings.Is3D || !State.State.IsInit) { return INVALID_SOUND_SLOT_INDEX; }

        const s32 indx = AcquireAvailableSoundSlotIndex();

        if (indx == INVALID_SOUND_SLOT_INDEX) { return INVALID_SOUND_SLOT_INDEX; }

        State.Slots[indx].Unk10 = 0;
        State.Slots[indx].Channels = SOUND_CHANNELS_1;
        State.Slots[indx].Unk07 = (u8)p2;
        State.Slots[indx].Volume = (u8)volume;
        State.Slots[indx].Frequency = (u16)frequency;
        State.Slots[indx].Unk03 = p4;
        State.Slots[indx].Unk04 = p5;
        State.Slots[indx].Unk05 = p6;
        State.Slots[indx].ID = id;

        const u32 result = InitializeSoundSlot(indx);

        if (result != SOUND_MODULE_FAILURE) { return result; }

        State.Slots[indx].ID = INVALID_SOUND_SLOT_ID;

        return indx;
    }

    // 0x00401d98
    // a.k.a. iSNDdirectfxinit, SNDfxinitbus
    DLLAPI s32 STDCALLAPI InitializeSoundBuffer(const u32 type, const s32 decay, const s32 damping)
    {
        State.EAX.Settings.Type = type;
        State.EAX.Settings.Decay = decay;
        State.EAX.Settings.Damping = damping;

        if (!State.EAX.State.IsReverb) { return SOUND_MODULE_FAILURE; }

        u32 environment = EAX_ENVIRONMENT_ARENA;
        f32 volume = MAX_SOUND_VOLUME_VALUE;

        switch (type)
        {
        case SOUND_EFFECT_TYPE_NONE: { environment = EAX_ENVIRONMENT_PADDEDCELL; volume = MIN_SOUND_VOLUME_VALUE; break; }
        case SOUND_EFFECT_TYPE_CLASSIC: { environment = EAX_ENVIRONMENT_PADDEDCELL; break; }
        case SOUND_EFFECT_TYPE_SOFTER_CORRIDOR: { environment = EAX_ENVIRONMENT_CARPETEDHALLWAY; break; }
        case SOUND_EFFECT_TYPE_SOFTER_ROOM: { environment = EAX_ENVIRONMENT_LIVINGROOM; break; }
        case SOUND_EFFECT_TYPE_GENERIC_ROOM: { environment = EAX_ENVIRONMENT_ROOM; break; }
        case SOUND_EFFECT_TYPE_PLAIN: { environment = EAX_ENVIRONMENT_PLAIN; break; }
        case SOUND_EFFECT_TYPE_FORREST: { environment = EAX_ENVIRONMENT_FOREST; break; }
        case SOUND_EFFECT_TYPE_MOUNTAINS: { environment = EAX_ENVIRONMENT_MOUNTAINS; break; }
        case SOUND_EFFECT_TYPE_CITY: { environment = EAX_ENVIRONMENT_CITY; break; }
        case SOUND_EFFECT_TYPE_PARKING_LOT: { environment = EAX_ENVIRONMENT_PARKINGLOT; break; }
        case SOUND_EFFECT_TYPE_CORRIDOR: { environment = EAX_ENVIRONMENT_HALLWAY; break; }
        case SOUND_EFFECT_TYPE_ALLEY: { environment = EAX_ENVIRONMENT_ALLEY; break; }
        case SOUND_EFFECT_TYPE_GENERIC_ENVIRONMENT: { environment = EAX_ENVIRONMENT_GENERIC; break; }
        case SOUND_EFFECT_TYPE_HARDER_ROOM: { environment = EAX_ENVIRONMENT_STONEROOM; break; }
        case SOUND_EFFECT_TYPE_REVERB: { environment = EAX_ENVIRONMENT_BATHROOM; break; }
        case SOUND_EDDECT_TYPE_QUARRY: { environment = EAX_ENVIRONMENT_QUARRY; break; }
        case SOUND_EFFECT_TYPE_SEWER_PIPES: { environment = EAX_ENVIRONMENT_SEWERPIPE; break; }
        case SOUND_EFFECT_TYPE_CAVE: { environment = EAX_ENVIRONMENT_CAVE; break; }
        case SOUND_EFFECT_TYPE_HARDER_SOUND: { environment = EAX_ENVIRONMENT_STONECORRIDOR; break; }
        case SOUND_EFFECT_TYPE_AUDITORIUM: { environment = EAX_ENVIRONMENT_AUDITORIUM; break; }
        case SOUND_EFFECT_TYPE_HALL: { environment = EAX_ENVIRONMENT_CONCERTHALL; break; }
        case SOUND_EFFECT_TYPE_ARENA: { environment = EAX_ENVIRONMENT_ARENA; break;  }
        case SOUND_EFFECT_TYPE_HANGAR: { environment = EAX_ENVIRONMENT_HANGAR; break; }
        case SOUND_EFFECT_TYPE_UNDER_WATER: { environment = EAX_ENVIRONMENT_UNDERWATER; break; }
        case SOUND_EFFECT_TYPE_DIZZY: { environment = EAX_ENVIRONMENT_DIZZY; break; }
        case SOUND_EFFECT_TYPE_DRUGGED: { environment = EAX_ENVIRONMENT_DRUGGED; break; }
        case SOUND_EFFECT_TYPE_PSYCHOTIC: { environment = EAX_ENVIRONMENT_PSYCHOTIC; break; }
        default:
        {
            environment = EAX_ENVIRONMENT_HANGAR;

            if (type < SOUND_EFFECT_EXTRA_TYPE_HANGAR_FIRST || SOUND_EFFECT_EXTRA_TYPE_HANGAR_LAST < type)
            {
                Message("SNDfxinitbus - UNKNOWN EFFECT TYPE.\n");

                environment = EAX_ENVIRONMENT_ARENA;
            }
        }
        }

        if (State.EAX.Instance->Set(DSPROPSETID_EAX_ReverbProperties,
            DSPROPERTY_EAX_ENVIRONMENT, NULL, 0, &environment, sizeof(u32)) == DS_OK)
        {
            if (State.EAX.Instance->Set(DSPROPSETID_EAX_ReverbProperties,
                DSPROPERTY_EAX_VOLUME, NULL, 0, &volume, sizeof(f32)) == DS_OK)
            {
                if (decay != INVALID_SOUND_REVERB_PROPERTY_VALUE)
                {
                    if (decay < INVALID_SOUND_REVERB_PROPERTY_VALUE || MAX_SOUND_REVERB_PROPERTY_VALUE < decay) { Message("SNDfxinitbus - param1 OUT OF RANGE FOR EAX.\n"); }

                    f32 value = (f32)(decay * SOUND_REVERB_PROPERTY_MULTIPLIER);

                    if (State.EAX.Instance->Set(DSPROPSETID_EAX_ReverbProperties,
                        DSPROPERTY_EAX_DECAYTIME, NULL, 0, &value, sizeof(f32)) != DS_OK)
                    {
                        return SOUND_MODULE_FAILURE;
                    }
                }

                if (damping != INVALID_SOUND_REVERB_PROPERTY_VALUE)
                {
                    if (damping < INVALID_SOUND_REVERB_PROPERTY_VALUE || MAX_SOUND_REVERB_PROPERTY_VALUE < damping) { Message("SNDfxinitbus - param2 OUT OF RANGE FOR EAX.\n"); }

                    f32 value = (f32)(damping * SOUND_REVERB_PROPERTY_MULTIPLIER);

                    if (State.EAX.Instance->Set(DSPROPSETID_EAX_ReverbProperties,
                        DSPROPERTY_EAX_DAMPING, NULL, 0, &value, sizeof(f32)) != DS_OK)
                    {
                        return SOUND_MODULE_FAILURE;
                    }
                }

                return SOUND_MODULE_SUCCESS;
            }
        }

        return SOUND_MODULE_FAILURE;
    }

    // 0x00402e94
    // a.k.a. iSNDdirectfxlevel
    DLLAPI s32 STDCALLAPI LevelSoundBuffer(const u32 indx, const u32 value)
    {
        State.Buffers[indx].Settings.Level = (u8)value;

        if (!State.State.IsInit) { return SOUND_MODULE_SUCCESS; }

        if (State.Buffers[indx].Set == NULL) { return SOUND_MODULE_FAILURE; }

        EAXBUFFER_REVERBPROPERTIES data;
        ZeroMemory(&data, sizeof(EAXBUFFER_REVERBPROPERTIES));

        data.fMix = (f32)(value * SOUND_REVERBMIX_PROPERTY_MULTIPLIER);

        if (State.Buffers[indx].Set->Set(DSPROPSETID_EAXBUFFER_ReverbProperties,
            DSPROPERTY_EAXBUFFER_REVERBMIX, NULL, 0, &data, sizeof(EAXBUFFER_REVERBPROPERTIES)) == DS_OK)
        {
            return SOUND_MODULE_SUCCESS;
        }

        return SOUND_MODULE_FAILURE;
    }

    // 0x004028b0
    // a.k.a. iSNDdirectmixlatency
    // NOTE: Never being called by the application.
    DLLAPI s32 STDCALLAPI MixLatency(void)
    {
        if (!State.State.IsInit) { return SOUND_FREQUENCY_0; }

        DWORD play = 0;
        if (State.DX.Buffers.Active->GetCurrentPosition(&play, NULL) != DS_OK) { return SOUND_FREQUENCY_0; }

        s32 value = State.Settings.Unk05 - (play >> State.Settings.BlockAlignBytes);

        if (value < 0) { value = value + State.Settings.Unk16; }

        return (value * SOUND_FREQUENCY_MULTIPLIER) / State.Settings.Frequency;
    }

    // 0x004030c8
    // a.k.a. iSNDdirectplay3d
    DLLAPI s32 STDCALLAPI PlaySoundBuffer(const u32 indx, const u32 slot, const BOOL looping, const u32 frequency, const u32 volume, const u32 level, const u32 p4, const u32 p5)
    {
        if (!State.State.IsInit) { return SOUND_MODULE_FAILURE; }

        if (State.Settings.Is3D)
        {
            State.Buffers[indx].Settings.Slot = slot;
            State.Buffers[indx].Settings.IsLooping = looping;
            State.Buffers[indx].Buffers.Main = State.Slots[slot].Buffer;

            BOOL found = TRUE;
            BOOL created = FALSE;

            for (u32 x = 0; x < MAX_SOUND_BUFFER_COUNT; x++)
            {
                if (State.Slots[slot].Buffer == State.Buffers[x].Buffers.Main
                    && State.Slots[slot].Buffer == State.Buffers[x].Buffers.Active)
                {
                    if (State.DX.Instance->DuplicateSoundBuffer(State.Slots[slot].Buffer,
                        &State.Buffers[indx].Buffers.Active) != DS_OK)
                    {
                        found = FALSE; break;
                    }

                    created = TRUE; break;
                }
            }

            if (found)
            {
                if (!created) { State.Buffers[indx].Buffers.Active = State.Buffers[indx].Buffers.Main; }

                if (State.Buffers[indx].Buffers.Active->QueryInterface(IID_IDirectSound3DBuffer,
                    (void**)&State.Buffers[indx].Buffers.Volume) == DS_OK)
                {
                    State.Buffers[indx].Set = NULL;

                    if (State.EAX.State.IsReverb)
                    {
                        if (State.Buffers[indx].Buffers.Active->QueryInterface(IID_IKsPropertySet,
                            (void**)&State.Buffers[indx].Set) != DS_OK)
                        {
                            State.Buffers[indx].Set = NULL;
                        }
                    }

                    RateSoundBuffer(indx, frequency);
                    PositionSoundBuffer(indx, p4, p5);
                    VolumeSoundBuffer(indx, volume);
                    LevelSoundBuffer(indx, level);

                    State.Buffers[indx].Buffers.Active->SetCurrentPosition(0);

                    if (State.Buffers[indx].Buffers.Active->Play(0, 0, looping) == DS_OK)
                    {
                        DWORD status = DSBSTATUS_NONE;
                        State.Buffers[indx].Buffers.Active->GetStatus(&status);

                        if (status & DSBSTATUS_PLAYING) { return SOUND_MODULE_SUCCESS; }
                    }
                }
            }
        }

        if (State.Buffers[indx].Buffers.Active != NULL
            && State.Buffers[indx].Buffers.Active != State.Buffers[indx].Buffers.Main)
        {
            State.Buffers[indx].Buffers.Active->Release();
        }

        if (State.Buffers[indx].Buffers.Volume != NULL)
        {
            State.Buffers[indx].Buffers.Volume->Release();
        }

        State.Buffers[indx].Buffers.Main = NULL;
        State.Buffers[indx].Buffers.Active = NULL;
        State.Buffers[indx].Buffers.Volume = NULL;

        return SOUND_MODULE_SUCCESS;
    }

    // 0x00402f08
    //a.k.a. iSNDdirectpos3d
    DLLAPI s32 STDCALLAPI PositionSoundBuffer(const u32 indx, const u32 p2, const u32 p3)
    {
        f32x3 xyz;
        State.Lambdas.AcquireSoundBufferPosition(p2, p3, &xyz);

        State.Buffers[indx].Unk05 = p2;
        State.Buffers[indx].Unk06 = p3;

        if (!State.State.IsInit) { return SOUND_MODULE_SUCCESS; }

        const f32 x = (f32)(xyz.X * SOUND_BUFFER_POSITION_MULTIPLIER);
        const f32 y = (f32)(xyz.Y * SOUND_BUFFER_POSITION_MULTIPLIER);
        const f32 z = (f32)(xyz.Z * SOUND_BUFFER_POSITION_MULTIPLIER);

        if (State.Buffers[indx].Buffers.Volume->SetPosition(x, y, z, DS3D_IMMEDIATE) == DS_OK) { return SOUND_MODULE_SUCCESS; }

        return SOUND_MODULE_FAILURE;
    }

    // 0x00402fac
    // a.k.a. iSNDdirectrate
    DLLAPI s32 STDCALLAPI RateSoundBuffer(const u32 indx, const u32 value)
    {
        State.Buffers[indx].Settings.Frequency = (u16)value;

        if (!State.State.IsInit) { return SOUND_MODULE_SUCCESS; }

        if (State.Buffers[indx].Buffers.Active->SetFrequency(value) == DS_OK) { return SOUND_MODULE_SUCCESS; }

        return SOUND_MODULE_FAILURE;
    }

    // 0x00402934
    // a.k.a. iSNDdirectrecordpacket
    // NOTE: Never being called by the application.
    DLLAPI s32 STDCALLAPI PacketRecording(void*)
    {
        return SOUND_MODULE_SUCCESS;
    }

    // 0x0040292c
    // a.k.a. iSNDdirectrecordstart
    // NOTE: Never being called by the application.
    DLLAPI s32 STDCALLAPI StartRecording(void*, void*, void*, void*)
    {
        return SOUND_MODULE_FAILURE;
    }

    // 0x0040293c
    // a.k.a. iSNDdirectrecordstop
    // NOTE: Never being called by the application.
    DLLAPI s32 STDCALLAPI StopRecording(void)
    {
        return SOUND_MODULE_SUCCESS;
    }

    // 0x00403074
    // a.k.a. iSNDdirectremovebuf
    DLLAPI s32 STDCALLAPI RemoveSoundBuffer(const u32 indx)
    {
        if (!State.Settings.Is3D) { return SOUND_MODULE_FAILURE; }

        if (State.State.IsInit) { State.Slots[indx].Buffer->Release(); }

        State.Slots[indx].ID = INVALID_SOUND_SLOT_ID;

        return SOUND_MODULE_SUCCESS;
    }

    // 0x00402b54
    // a.k.a. iSNDdirectserve
    DLLAPI s32 STDCALLAPI Serve(void)
    {
        if (!State.State.IsLive) { return SOUND_MODULE_SUCCESS; }

        DWORD status = DSBSTATUS_NONE;
        State.DX.Buffers.Active->GetStatus(&status);

        if (status & DSBSTATUS_BUFFERLOST)
        {
            State.DX.Buffers.Active->Restore();

            return State.DX.Buffers.Active->Play(0, 0, DSBPLAY_LOOPING);
        }

        HRESULT result = State.DX.Buffers.Active->GetCurrentPosition(NULL, &State.Settings.CurrentWrite);

        if (result != DS_OK) { return result; }

        State.Settings.CurrentWrite = State.Settings.CurrentWrite >> State.Settings.BlockAlignBytes;
        State.Settings.CurrentWrite = State.Settings.CurrentWrite & 0xfffff0;

        {
            const u32 pos = State.Settings.CurrentWrite < State.Settings.MinimumWrite
                ? State.Settings.CurrentWrite + State.Settings.Unk16
                : State.Settings.CurrentWrite;

            State.Settings.Unk02 = State.Settings.Unk02 + pos - State.Settings.MinimumWrite;
        }

        if (State.Settings.CurrentWrite == State.Settings.MinimumWrite)
        {
            State.Counters.Unknown1 = State.Counters.Unknown1 + 1;

            if (500 < State.Counters.Unknown1) { return State.DX.Buffers.Active->Play(0, 0, DSBPLAY_LOOPING); } // TODO
        }
        else { State.Counters.Unknown1 = 0; }

        u32 todo1 = 0; // TODO
        u32 todo2 = 0; // TODO

        u32 position = 0;

        while (TRUE)
        {
            position = (State.Settings.CurrentWrite + State.Settings.MaximumPlay) & SOUND_BUFFER_POSITION_ALIGNMENT_MASK;

            if (State.Settings.Unk16 <= position)
            {
                position = position - State.Settings.Unk16;
            }

            if (position < State.Settings.Unk05)
            {
                todo1 = State.Settings.Unk16 - State.Settings.Unk05;
                todo2 = position;
            }
            else
            {
                todo1 = position - State.Settings.Unk05;
                todo2 = 0;
            }

            if ((todo1 + todo2) < (State.Settings.Unk16 - SOUND_PLAY_ITERATION_STEP)) { break; }

            State.Settings.MaximumPlay = State.Settings.MaximumPlay + SOUND_PLAY_ITERATION_STEP;
        }

        if (!State.Settings.IsEmulated) { AcquireSoundPosition(); }

        State.Settings.MinimumWrite = State.Settings.CurrentWrite;

        if (todo1 != 0)
        {
            State.Settings.Unk01 = State.Settings.Unk01 + todo1;

            {
                const u32 lo = State.Settings.Unk05 << State.Settings.BlockAlignBytes;
                const u32 ls = todo1 << State.Settings.BlockAlignBytes;

                result = State.DX.Buffers.Active->Lock(lo, ls, &State.Settings.Lock.Data, &State.Settings.Lock.Size, NULL, 0, DSBLOCK_NONE);

                if (result == DS_OK)
                {
                    AcquireSoundData(State.Settings.Lock.Data, todo1);

                    result = State.DX.Buffers.Active->Unlock(State.Settings.Lock.Data, ls, NULL, 0);
                }
            }

            if (todo2 != 0)
            {
                State.Settings.Unk01 = State.Settings.Unk01 + todo2;

                const u32 lo = todo2 << State.Settings.BlockAlignBytes;

                result = State.DX.Buffers.Active->Lock(0, lo, &State.Settings.Lock.Data, &State.Settings.Lock.Size, NULL, 0, DSBLOCK_NONE);

                if (result == DS_OK) { AcquireSoundData(State.Settings.Lock.Data, todo2); }

                result = State.DX.Buffers.Active->Unlock(State.Settings.Lock.Data, lo, NULL, 0);
            }
        }

        State.Settings.Unk05 = position;

        return SOUND_MODULE_SUCCESS;
    }

    // 0x00401620
    // a.k.a. iSNDdirectsetfunctions
    DLLAPI s32 STDCALLAPI SelectLambdas(const SOUNDMODULEACQUIREDATALAMBDA acquire, const SOUNDMODULESTOPSOUNDBUFFERLAMBDA stop, const SOUNDMODULELOGMESSAGELAMBDA log, const SOUNDMODULEUNKNOWNLAMBDA unknown, const SOUNDMODULEUNKNOWN5LAMBDA p5, const SOUNDMODULEUNKNOWN6LAMBDA p6, const SOUNDMODULEACQUIRESOUNDBUFFERPOSITIONLAMBDA position, const SOUNDMODULEUNKNOWN8LAMBDA p8)
    {
        State.Lambdas.AcquireData = acquire;
        State.Lambdas.StopBuffer = stop;
        State.Lambdas.LogMessage = log;
        State.Lambdas.Unknown = unknown;
        State.Lambdas.Lambda5 = p5;
        State.Lambdas.Lambda6 = p6;
        State.Lambdas.AcquireSoundBufferPosition = position;
        State.Lambdas.Lambda8 = p8;

        return SOUND_MODULE_SUCCESS;
    }

    // 0x004027dc
    // a.k.a. iSNDdirectstart
    DLLAPI s32 STDCALLAPI Start(const s32 options, const HWND hwnd)
    {
        if (State.State.IsLive) { Stop(); }

        if (!State.Settings.IsEmulated)
        {
            if (InitializeSounds(options, hwnd, SOUND_MODE_HARDWARE) == DS_OK) { return SOUND_MODULE_SUCCESS; }
        }

        return InitializeSounds(options, hwnd, SOUND_MODE_SOFTWARE);
    }

    // 0x0040282c
    // a.k.a. iSNDdirectstop
    DLLAPI s32 STDCALLAPI Stop(void)
    {
        ReleaseSoundExtensions();

        if (State.State.IsLive)
        {
            State.State.IsLive = FALSE;

            State.DX.Buffers.Active->Stop();
        }

        if (State.State.IsInit)
        {
            if (State.DX.Instance != NULL)
            {
                if (State.DX.Instance->Release() != DS_OK)
                {
                    Message("iSNDdirectstop - COULDN'T RELEASE DIRECTSOUND.\n");

                    return SOUND_MODULE_FAILURE;
                }
            }

            State.DX.Instance = NULL;

            State.State.IsActive = FALSE;
            State.State.IsInit = FALSE;
        }

        return SOUND_MODULE_SUCCESS;
    }

    // 0x00403284
    // a.k.a. iSNDdirectstopbuf
    DLLAPI s32 STDCALLAPI StopSoundBuffer(const u32 indx)
    {
        if (State.State.IsInit)
        {
            State.Buffers[indx].Buffers.Active->Stop();

            if (State.Buffers[indx].Buffers.Volume != NULL)
            {
                State.Buffers[indx].Buffers.Volume->Release();
            }

            if (State.Buffers[indx].Set != NULL)
            {
                State.Buffers[indx].Set->Release();
                State.Buffers[indx].Set = NULL;
            }

            if (State.Buffers[indx].Buffers.Active != State.Buffers[indx].Buffers.Main)
            {
                State.Buffers[indx].Buffers.Active->Release();
            }
        }

        State.Buffers[indx].Buffers.Main = NULL;
        State.Buffers[indx].Buffers.Active = NULL;
        State.Buffers[indx].Buffers.Volume = NULL;

        State.Lambdas.StopBuffer(indx);

        return SOUND_MODULE_SUCCESS;
    }

    // 0x00402e48
    // a.k.a. iSNDdirectvol
    DLLAPI s32 STDCALLAPI VolumeSoundBuffer(const u32 indx, const u32 value)
    {
        State.Buffers[indx].Settings.Volume = (u8)value;

        if (!State.State.IsInit) { return SOUND_MODULE_SUCCESS; }

        const s32 volume = (s32)((s16)SOUNDVOLUMEVALUEHIGH(Volume[value * SOUND_VOLUME_INDEX_MULTIPLIER]));

        if (State.Buffers[indx].Buffers.Active->SetVolume(volume) == DS_OK) { return SOUND_MODULE_SUCCESS; }

        return SOUND_MODULE_FAILURE;
    }

    // 0x0040160c
    // a.k.a. iSNDdllversion
    DLLAPI s32 STDCALLAPI AcquireVersion(void)
    {
        State.Settings.Capabilities = INVALID_SOUND_MODULE_CAPABILITIES;

        return SOUND_MODULE_VERSION_120005;
    }

    // 0x00401230
    void Message(const char* format, ...)
    {
        char buffer[MAX_SOUND_MODULE_MESSAGE_LENGTH];

        va_list args;
        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);

        if (State.Lambdas.LogMessage != NULL) { State.Lambdas.LogMessage(buffer); }
    }

    // 0x00401c54
    void InitializeSoundExtensions(void)
    {
        if (!State.Settings.Is3D) { return; }

        ReleaseSoundExtensions();

        WAVEFORMATEX format;
        ZeroMemory(&format, sizeof(WAVEFORMATEX));

        format.wFormatTag = WAVE_FORMAT_PCM;
        format.nChannels = SOUND_CHANNELS_1;
        format.nSamplesPerSec = SOUND_FREQUENCY_11025;
        format.nAvgBytesPerSec = SOUND_FREQUENCY_11025 * DEFAULT_SOUND_BUFFER_BLOCK_ALIGN;
        format.nBlockAlign = DEFAULT_SOUND_BUFFER_BLOCK_ALIGN;
        format.wBitsPerSample = SOUND_BITS_16;

        DSBUFFERDESC desc;
        ZeroMemory(&desc, sizeof(DSBUFFERDESC));

        desc.dwSize = sizeof(DSBUFFERDESC);
        desc.dwFlags = DSBCAPS_CTRL3D | DSBCAPS_LOCHARDWARE;
        desc.dwBufferBytes = DEFAULT_SOUND_BUFFER_BYTE_SIZE;
        desc.lpwfxFormat = &format;

        if (State.DX.Instance->CreateSoundBuffer(&desc, &State.EAX.Buffer, NULL) != DS_OK) { return; }

        if (State.EAX.Buffer->QueryInterface(IID_IKsPropertySet, (void**)&State.EAX.Instance) != DS_OK)
        {
            State.EAX.Buffer->Release();
            State.EAX.Buffer = NULL;

            return;
        }

        VmMode mode = DSPROPERTY_VMANAGER_MODE_DEFAULT;
        if (InitializeVoiceManager(DSPROPERTY_VMANAGER_MODE_REPORT, &mode) == SOUND_MODULE_SUCCESS)
        {
            State.EAX.State.Mode = mode;
            State.EAX.State.IsActive = TRUE;
        }

        ULONG support = KSPROPERTY_SUPPORT_NONE;
        if (State.EAX.Instance->QuerySupport(DSPROPSETID_EAX_ReverbProperties, DSPROPERTY_EAX_ALL, &support) == DS_OK)
        {
            if ((support & KSPROPERTY_SUPPORT_GET_SET_SUPPORTED) == KSPROPERTY_SUPPORT_GET_SET_SUPPORTED)
            {
                State.EAX.State.IsReverb = TRUE;
            }
        }
    }

    // 0x00401298
    s32 InitializeVoiceManager(const VmMode value, VmMode* out)
    {
        ULONG support = KSPROPERTY_SUPPORT_NONE;
        if (State.EAX.Instance->QuerySupport(DSPROPSETID_VoiceManager, DSPROPERTY_VMANAGER_MODE, &support) == DS_OK)
        {
            if ((support & KSPROPERTY_SUPPORT_GET_SET_SUPPORTED) == KSPROPERTY_SUPPORT_GET_SET_SUPPORTED)
            {
                if (out != NULL)
                {
                    ULONG count = 0;
                    State.EAX.Instance->Get(DSPROPSETID_VoiceManager, DSPROPERTY_VMANAGER_MODE,
                        NULL, 0, (void*)out, sizeof(VmMode), &count);
                }

                VmMode mode = value;

                if (State.EAX.Instance->Set(DSPROPSETID_VoiceManager, DSPROPERTY_VMANAGER_MODE,
                    NULL, 0, &mode, sizeof(VmMode)) == DS_OK) {
                    return SOUND_MODULE_SUCCESS;
                }
            }
        }

        return SOUND_MODULE_FAILURE;
    }

    // 0x00401be8
    void ReleaseSoundExtensions(void)
    {
        if (State.EAX.Instance != NULL)
        {
            InitializeVoiceManager(State.EAX.State.Mode, NULL);

            State.EAX.State.IsActive = FALSE;

            State.EAX.Instance->Release();
            State.EAX.Instance = NULL;

            if (State.EAX.Buffer != NULL)
            {
                State.EAX.Buffer->Release();
                State.EAX.Buffer = NULL;
            }

            State.EAX.State.IsReverb = FALSE;
        }
    }

    // 0x0040131c
    u32 AcquireAvailableSoundSlotIndex(void)
    {
        for (u32 x = MIN_VALID_SOUND_SLOT_COUNT; x < MAX_SOUND_SLOT_COUNT; x++)
        {
            if (State.Slots[x].ID == INVALID_SOUND_SLOT_ID) { return x; }
        }

        return INVALID_SOUND_SLOT_INDEX;
    }

    // 0x004020e0
    // a.k.a. iSNDdirectstart, iSNDstartbuffer
    s32 InitializeSounds(const u32 options, const HWND hwnd, const SOUND_MODE mode)
    {
        s32 result = SOUND_MODULE_FAILURE;

        State.Settings.Is3D = FALSE;

        u32 bits = SOUND_BITS_0;
        u32 channels = SOUND_CHANNELS_0;

        if (options & SOUND_MODULE_CAPABILITIES_3D_BUFFER_SUPPORTED)
        {
            if (mode != SOUND_MODE_SOFTWARE) { goto Release; }

            State.Settings.Is3D = TRUE;
        }

        if (options & SOUND_MODULE_CAPABILITIES_FREQUENCY_44100) { State.Settings.Frequency = SOUND_FREQUENCY_44100; }
        else if (options & SOUND_MODULE_CAPABILITIES_FREQUENCY_32000) { State.Settings.Frequency = SOUND_FREQUENCY_32000; }
        else if (options & SOUND_MODULE_CAPABILITIES_FREQUENCY_22050) { State.Settings.Frequency = SOUND_FREQUENCY_22050; }
        else if (options & SOUND_MODULE_CAPABILITIES_FREQUENCY_16000) { State.Settings.Frequency = SOUND_FREQUENCY_16000; }
        else if (options & SOUND_MODULE_CAPABILITIES_FREQUENCY_11025) { State.Settings.Frequency = SOUND_FREQUENCY_11025; }
        else { Message("iSNDdirectstart - NO VALID RATE SPECIFIED.\n"); goto Release; }

        if (options & SOUND_MODULE_CAPABILITIES_16BIT_STEREO) { bits = SOUND_BITS_16; channels = SOUND_CHANNELS_2; }
        else if (options & SOUND_MODULE_CAPABILITIES_8BIT_STEREO) { bits = SOUND_BITS_8; channels = SOUND_CHANNELS_2; }
        else if (options & SOUND_MODULE_CAPABILITIES_16BIT_MONO) { bits = SOUND_BITS_16; channels = SOUND_CHANNELS_1; }
        else if (options & SOUND_MODULE_CAPABILITIES_8BIT_MONO) { bits = SOUND_BITS_8; channels = SOUND_CHANNELS_1; }
        else { Message("iSNDdirectstart - NO VALID FORMAT SPECIFIED.\n"); goto Release; }

        if (!State.Settings.IsEmulated)
        {
            State.Settings.MaximumPlay = (State.Settings.Frequency * 2 * 10) / SOUND_FREQUENCY_MULTIPLIER; // TODO
        }
        else
        {
            State.Settings.MaximumPlay = bits == SOUND_BITS_8 ? 0x4000 : 0x2000; // TODO

            if (channels == SOUND_CHANNELS_1) { State.Settings.MaximumPlay = State.Settings.MaximumPlay * 2; } // TODO
        }

        State.Settings.MaximumPlay = State.Settings.MaximumPlay & SOUND_BUFFER_POSITION_ALIGNMENT_MASK;
        State.Settings.Unk05 = 0;
        State.Settings.CurrentWrite = 0;
        State.Settings.MinimumWrite = 0;
        State.Settings.Unk01 = 0;
        State.Settings.Unk10 = 0;
        State.Settings.Unk02 = (State.Settings.Frequency / 50) & SOUND_BUFFER_POSITION_ALIGNMENT_MASK; // TODO
        State.Settings.BlockAlignBits = (bits >> 3) * channels;
        State.Settings.BlockAlignBytes = State.Settings.BlockAlignBits / DEFAULT_SOUND_BUFFER_BLOCK_ALIGN;

        if (!State.State.IsInit)
        {
            const HRESULT code = DirectSoundCreate(NULL, &State.DX.Instance, NULL);

            if (code != DS_OK)
            {
                if (code == DSERR_ALLOCATED) { result = SOUND_MODULE_ALREADY_ALLOCATED_FAILURE; }
                else if (code == DSERR_NODRIVER) { result = SOUND_MODULE_MISSING_DRIVER_FAILURE; }
                else { result = SOUND_MODULE_FAILURE; }

                goto Release;
            }

            State.State.IsInit = TRUE;
        }

        if (State.DX.Instance->SetCooperativeLevel(hwnd, mode == SOUND_MODE_HARDWARE ? DSSCL_WRITEPRIMARY : DSSCL_EXCLUSIVE) != DS_OK)
        {
            Message("iSNDdirectstart - COULDN'T SET COOPERATION LEVEL.\n"); goto Release;
        }

        if (!State.State.IsActive)
        {
            DSBUFFERDESC desc;
            ZeroMemory(&desc, sizeof(DSBUFFERDESC));

            desc.dwSize = sizeof(DSBUFFERDESC);
            desc.dwFlags = (DWORD)(State.Settings.Is3D ? (DSBCAPS_CTRL3D | DSBCAPS_PRIMARYBUFFER) : DSBCAPS_PRIMARYBUFFER);

            if (State.DX.Instance->CreateSoundBuffer(&desc, &State.DX.Buffers.Main, NULL) != DS_OK)
            {
                Message("iSNDdirectstart - COULDN'T CREATE PRIMARY BUFFER.\n"); goto Release;
            }

            State.State.IsActive = TRUE;
        }

        WAVEFORMATEX format;
        ZeroMemory(&format, sizeof(WAVEFORMATEX));

        format.wFormatTag = WAVE_FORMAT_PCM;
        format.nChannels = (WORD)channels;
        format.nSamplesPerSec = State.Settings.Frequency;
        format.nAvgBytesPerSec = State.Settings.Frequency << State.Settings.BlockAlignBytes;
        format.nBlockAlign = (WORD)State.Settings.BlockAlignBits;
        format.wBitsPerSample = (WORD)bits;

        if (State.DX.Buffers.Main->SetFormat(&format) != DS_OK)
        {
            Message("iSNDdirectstart - COULDN'T SET PRIMARY BUFFER FORMAT.\n"); goto Release;
        }

        DSBCAPS caps;
        ZeroMemory(&caps, sizeof(DSBCAPS));

        caps.dwSize = sizeof(DSBCAPS);

        if (State.DX.Buffers.Main->GetCaps(&caps) != DS_OK)
        {
            Message("iSNDdirectstart - COULDN'T GET PRIMARY BUFFER CAPS.\n"); goto Release;
        }

        if (mode == SOUND_MODE_HARDWARE)
        {
            if ((caps.dwFlags & DSBCAPS_LOCHARDWARE) == DSBCAPS_NONE) { goto Release; }

            State.Settings.BufferSize = caps.dwBufferBytes;

            if (((caps.dwBufferBytes >> State.Settings.BlockAlignBytes) & SOUND_BUFFER_LENGTH_MASK) != 0) { goto Release; }
        }
        else
        {
            if (State.Settings.IsEmulated)
            {
                State.Settings.BufferSize = DEFAULT_SOFTWARE_SOUND_BUFFER_SIZE << State.Settings.BlockAlignBytes;
            }
            else
            {
                const u32 pos = ((State.Settings.Frequency * 400) / SOUND_FREQUENCY_MULTIPLIER) & SOUND_BUFFER_POSITION_ALIGNMENT_MASK; // TODO

                State.Settings.BufferSize = pos << State.Settings.BlockAlignBytes;
            }
        }

        State.Settings.Unk09 = State.Settings.BufferSize >> State.Settings.BlockAlignBytes;
        State.Settings.MinimumPlay = ((State.Settings.Unk09 << 3) / 10) & SOUND_BUFFER_POSITION_ALIGNMENT_MASK; // TODO
        State.Settings.Unk12 = 0;
        State.Settings.Unk13 = 0;
        State.Settings.Unk11 = 0;

        State.DX.Buffers.Active = State.DX.Buffers.Main;

        State.Settings.Unk16 = State.Settings.Unk09;

        if (mode == SOUND_MODE_SOFTWARE)
        {
            WAVEFORMATEX frmt;
            ZeroMemory(&frmt, sizeof(WAVEFORMATEX));

            frmt.wFormatTag = WAVE_FORMAT_PCM;
            frmt.nChannels = (WORD)channels;
            frmt.nSamplesPerSec = State.Settings.Frequency;
            frmt.nAvgBytesPerSec = State.Settings.Frequency << State.Settings.BlockAlignBytes;
            frmt.nBlockAlign = (WORD)State.Settings.BlockAlignBits;
            frmt.wBitsPerSample = (WORD)bits;

            DSBUFFERDESC desc;
            ZeroMemory(&desc, sizeof(DSBUFFERDESC));

            desc.dwSize = sizeof(DSBUFFERDESC);
            desc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_LOCSOFTWARE;
            desc.dwBufferBytes = State.Settings.BufferSize;
            desc.lpwfxFormat = &frmt;

            if (State.DX.Instance->CreateSoundBuffer(&desc, &State.DX.Buffers.Software, NULL) != DS_OK)
            {
                Message("iSNDdirectstart - COULDN'T CREATE SECONDARY BUFFER.\n"); goto Release;
            }

            State.DX.Buffers.Active = State.DX.Buffers.Software;
        }

        if (State.DX.Buffers.Active->Lock(0, State.Settings.BufferSize,
            &State.Settings.Lock.Data, &State.Settings.Lock.Size, NULL, 0, DSBLOCK_NONE) == DS_OK)
        {
            ZeroMemory(State.Settings.Lock.Data, State.Settings.BufferSize);

            State.DX.Buffers.Active->Unlock(State.Settings.Lock.Data, State.Settings.BufferSize, NULL, 0);

            if (State.DX.Buffers.Active->Play(0, 0, DSBPLAY_LOOPING) != DS_OK)
            {
                Message("iSNDstartbuffer - COULDN'T PLAY SECONDARY BUFFER.\n");  goto Release;
            }

            if (!State.Settings.Is3D)
            {
                State.State.IsLive = TRUE;

                return SOUND_MODULE_SUCCESS;
            }

            if (State.DX.Buffers.Main->QueryInterface(IID_IDirectSound3DListener, (void**)&State.EAX.Listener) == DS_OK)
            {
                InitializeSoundExtensions();

                InitializeSoundBuffer(State.EAX.Settings.Type, State.EAX.Settings.Decay, State.EAX.Settings.Damping);

                InitializeSoundSlots();
                InitializeSoundBuffers();

                State.State.IsLive = TRUE;

                return SOUND_MODULE_SUCCESS;
            }
        }

    Release:

        if (State.State.IsLive)
        {
            State.State.IsLive = FALSE;

            if (State.DX.Buffers.Active->Stop() != DS_OK) { Message("iSNDstartbuffer - COULDN'T STOP BUFFER.\n"); }
        }

        if (State.State.IsInit)
        {
            if (State.DX.Instance != NULL)
            {
                if (State.DX.Instance->Release() != DS_OK)
                {
                    Message("iSNDstartbuffer - COULDN'T RELEASE DIRECTSOUND.\n");

                    return SOUND_MODULE_FAILURE;
                }
            }

            State.DX.Instance = NULL;

            State.State.IsActive = FALSE;
            State.State.IsInit = FALSE;
        }

        return result;
    }

    // 0x00401584
    s32 InitializeSoundSlots(void)
    {
        for (u32 x = MIN_VALID_SOUND_SLOT_COUNT; x < MAX_SOUND_SLOT_COUNT; x++) { InitializeSoundSlot(x); }

        return SOUND_MODULE_SUCCESS;
    }

    // 0x00401350
    // a.k.a. iSNDcreateslot
    s32 InitializeSoundSlot(const u32 indx)
    {
        DSCAPS caps;
        ZeroMemory(&caps, sizeof(DSCAPS));

        caps.dwSize = sizeof(DSCAPS);

        if (State.DX.Instance->GetCaps(&caps) != DS_OK || caps.dwFreeHw3DStaticBuffers == 0) { return SOUND_MODULE_FAILURE; }

        if (State.Slots[indx].ID == INVALID_SOUND_SLOT_ID) { return SOUND_MODULE_FAILURE; }

        u32 bits = SOUND_BITS_16;

        if (State.Slots[indx].Unk10 != 0) // TODO
        {
            Message("iSNDcreateslot - dstsamplerep UNSUPPORTED.\n");

            bits = State.Slots[indx].Unk03;
        }

        if (0 < State.Slots[indx].Unk05) // TODO
        {
            bits = (State.Slots[indx].Unk05 - State.Slots[indx].Unk04) + 1; // TODO
        }

        const u32 bytes = (State.Slots[indx].Channels) * (bits >> 3);

        WAVEFORMATEX format;
        ZeroMemory(&format, sizeof(WAVEFORMATEX));

        format.wFormatTag = WAVE_FORMAT_PCM;
        format.nChannels = State.Slots[indx].Channels;
        format.nSamplesPerSec = State.Slots[indx].Frequency;
        format.nAvgBytesPerSec = (DWORD)(State.Slots[indx].Channels * State.Slots[indx].Frequency * (bits >> 3));
        format.nBlockAlign = (WORD)(State.Slots[indx].Channels * (bits >> 3));
        format.wBitsPerSample = (WORD)bits;

        DSBUFFERDESC desc;
        ZeroMemory(&desc, sizeof(DSBUFFERDESC));

        desc.dwSize = sizeof(DSBUFFERDESC);
        desc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRL3D | DSBCAPS_LOCHARDWARE;
        desc.dwBufferBytes = (DWORD)bytes;
        desc.lpwfxFormat = &format;

        if (State.DX.Instance->CreateSoundBuffer(&desc, &State.Slots[indx].Buffer, NULL) != DS_OK) { return SOUND_MODULE_FAILURE; }

        void* data = NULL;
        DWORD size = 0;

        if (State.Slots[indx].Buffer->Lock(0, bytes, &data, &size, NULL, 0, DSBLOCK_NONE) != DS_OK)
        {
            Message("iSNDcreateslot - COULDN'T LOCK 3D BUFFER.\n");

            return SOUND_MODULE_FAILURE;
        }

        State.Lambdas.Lambda8(State.Slots[indx].ID, State.Slots[indx].Unk07, State.Slots[indx].Volume,
            State.Slots[indx].Unk04, State.Slots[indx].Unk05, State.Slots[indx].Unk03, data,
            State.Slots[indx].Unk10, State.Slots[indx].Channels);

        if (State.Slots[indx].Buffer->Unlock(data, size, NULL, 0) != DS_OK)
        {
            Message("iSNDcreateslot - COULDN'T UNLOCK 3D BUFFER.\n");

            return SOUND_MODULE_FAILURE;
        }

        return indx;
    }

    // 0x004015a0
    s32 InitializeSoundBuffers(void)
    {
        for (u32 x = 0; x < MAX_SOUND_BUFFER_COUNT; x++)
        {
            if (State.Buffers[x].Buffers.Main == NULL) { continue; }

            State.Buffers[x].Buffers.Main = NULL;
            State.Buffers[x].Buffers.Active = NULL;
            State.Buffers[x].Buffers.Volume = NULL;

            const s32 result = PlaySoundBuffer(State.Buffers[x].Settings.Slot, x,
                State.Buffers[x].Settings.IsLooping, State.Buffers[x].Settings.Frequency,
                State.Buffers[x].Settings.Volume, State.Buffers[x].Settings.Level,
                State.Buffers[x].Unk05, State.Buffers[x].Unk06);

            if (result != SOUND_MODULE_SUCCESS) { State.Lambdas.StopBuffer(x); }

        }

        return SOUND_MODULE_SUCCESS;
    }

    // 0x00402a14
    u32 AcquireSoundPosition(void)
    {
        if (0x11000000 < State.Settings.Unk01) // TODO
        {
            State.Settings.Unk01 = State.Settings.Unk01 + 0xf0000000; // TODO
            State.Settings.Unk02 = State.Settings.Unk02 - 0x10000000; // TODO
        }

        s32 position = State.Settings.Unk01 - State.Settings.Unk02;

        if (State.Settings.Unk16 <= position)
        {
            State.Settings.Unk01 = State.Settings.Unk01 - State.Settings.Unk16;

            position = position - State.Settings.Unk16;
        }

        if (position < 0)
        {
            if (State.Settings.Unk01 != 0)
            {
                State.Settings.MaximumPlay = ((State.Settings.MaximumPlay - position) + SOUND_BUFFER_LENGTH_MASK) & SOUND_BUFFER_POSITION_ALIGNMENT_MASK;

                if (State.Settings.MinimumPlay < State.Settings.MaximumPlay)
                {
                    State.Settings.MaximumPlay = State.Settings.MinimumPlay;
                }

                State.Settings.Unk10 = 0;

                return 0;
            }
        }
        else
        {
            State.Settings.Unk10 = State.Settings.Unk10 + 1;

            if (position < State.Settings.Unk09) { State.Settings.Unk09 = position; }

            if (2999 < State.Settings.Unk10) // TODO
            {
                position = State.Settings.Unk10 / 10; // TODO

                if (State.Settings.Unk10 % 10 == 0) // TODO
                {
                    if (State.Settings.Unk09 < 0x11) // TODO
                    {
                        State.Settings.Unk09 = State.Settings.Unk16;

                        return State.Settings.Unk16;
                    }

                    State.Settings.MaximumPlay = (State.Settings.MaximumPlay - 1) & SOUND_BUFFER_POSITION_ALIGNMENT_MASK;
                    State.Settings.Unk09 = State.Settings.Unk16;

                    return State.Settings.Unk16;
                }
            }
        }

        return position;
    }

    // 0x00402940
    void AcquireSoundData(void* buffer, const s32 size)
    {
        u32 length = size;

        while (0 < length)
        {
            if (State.Settings.Unk11 < 1)
            {
                State.Settings.Unk12 = State.Settings.Unk12 + 1;

                State.Lambdas.Lambda5();

                State.Settings.Unk11 = ((State.Settings.Unk12 * State.Settings.Frequency & 0xffffffffU) / 100) - State.Settings.Unk13 & 0xffffff0; // TODO

                State.Settings.Unk13 = State.Settings.Unk13 + State.Settings.Unk11;

                if (30000 < State.Settings.Unk12) // TODO
                {
                    State.Settings.Unk12 = 0;
                    State.Settings.Unk13 = 0;
                }
            }

            const u32 len = Min(State.Settings.Unk11, length);

            State.Settings.Unk11 = State.Settings.Unk11 - len;

            State.Lambdas.AcquireData(buffer, len);

            length = length - len;

            buffer = (void*)((addr)buffer + (addr)(len << State.Settings.BlockAlignBytes));
        }

        if (State.Settings.Is3D) { StopSoundBuffers(); }
    }

    // 0x00402df8
    void StopSoundBuffers(void)
    {
        State.Counters.Unknown2 = State.Counters.Unknown2 + 1;

        if (State.Counters.Unknown2 & 3) { return; }

        for (u32 x = 0; x < MAX_SOUND_BUFFER_COUNT; x++)
        {
            if (State.Buffers[x].Buffers.Active != NULL)
            {
                DWORD status = DSBSTATUS_NONE;
                State.Buffers[x].Buffers.Active->GetStatus(&status);

                if ((status & DSBSTATUS_PLAYING) == 0) { StopSoundBuffer(x); }
            }
        }
    }

    // 0x0040bb1e
    // NOTE: each value contains a pairs of signed 16-bit values.
    // Direct Sound accepts values in the range [-10,000..0].
    // Moreover, it looks like there are two configurations available (A and B).
    const u32 Volume[MAX_SOUND_VOLUME_COUNT] =
    {
        0xd8f00000, // (-10000      0)
        0xf18cef98, // (-3700   -4200)
        0xf448f31c, // (-3000   -3300)
        0xf5c9f510, // (-2615   -2800)
        0xf6cff666, // (-2353   -2458)
        0xf781f72c, // (-2175   -2260)
        0xf815f7ce, // (-2027   -2098)
        0xf893f856, // (-1901   -1962)
        0xf902f8cd, // (-1790   -1843)
        0xf965f935, // (-1691   -1739)
        0xf9bef992, // (-1602   -1646)
        0xfa0ef9e7, // (-1522   -1561)
        0xfa58fa34, // (-1448   -1484)
        0xfa9cfa7b, // (-1380   -1413)
        0xfadcfabd, // (-1316   -1347)
        0xfb17fafa, // (-1257   -1286)
        0xfb4efb33, // (-1202   -1229)
        0xfb82fb69, // (-1150   -1175)
        0xfbb4fb9b, // (-1100   -1125)
        0xfbe2fbcb, // (-1054   -1077)
        0xfc0ffbf9, // (-1009   -1031)
        0xfc39fc24, // (-967     -988)
        0xfc61fc4d, // (-927     -947)
        0xfc88fc75, // (-888     -907)
        0xfcadfc9a, // (-851     -870)
        0xfcd0fcbe, // (-816     -834)
        0xfcf2fce1, // (-782     -799)
        0xfd13fd03, // (-749     -765)
        0xfd33fd23, // (-717     -733)
        0xfd51fd42, // (-687     -702)
        0xfd6ffd60, // (-657     -672)
        0xfd8bfd7d, // (-629     -643)
        0xfda7fd99, // (-601     -615)
        0xfdc2fdb4, // (-574     -588)
        0xfddcfdcf, // (-548     -561)
        0xfdf5fde9, // (-523     -535)
        0xfe0efe02, // (-498     -510)
        0xfe26fe1a, // (-474     -486)
        0xfe3dfe32, // (-451     -462)
        0xfe54fe49, // (-428     -439)
        0xfe6afe5f, // (-406     -417)
        0xfe80fe75, // (-384     -395)
        0xfe95fe8a, // (-363     -374)
        0xfea9fe9f, // (-343     -353)
        0xfebdfeb3, // (-323     -333)
        0xfed1fec7, // (-303     -313)
        0xfee4fedb, // (-284     -293)
        0xfef7feee, // (-265     -274)
        0xff0aff01, // (-246     -255)
        0xff1cff13, // (-228     -237)
        0xff2eff25, // (-210     -219)
        0xff3fff36, // (-193     -202)
        0xff50ff47, // (-176     -185)
        0xff61ff58, // (-159     -168)
        0xff71ff69, // (-143     -151)
        0xff81ff79, // (-127     -135)
        0xff91ff89, // (-111     -119)
        0xffa1ff99, // (-95      -103)
        0xffb0ffa8, // (-80       -88)
        0xffbfffb7, // (-65       -73)
        0xffceffc6, // (-50       -58)
        0xffdcffd5, // (-36       -43)
        0xffeaffe3, // (-22       -29)
        0xfff9fff2  // (-7        -14)
    };
}