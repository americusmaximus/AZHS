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
#include "Direct.Sound.EAX.hxx"
#include "Direct.Sound.VoiceManager.hxx"
#include "Sound.Basic.hxx"
#include "SoundModule.Basic.hxx"
#include "SoundModule.Export.hxx"

#define SOUND_BITS_0 0
#define SOUND_BITS_8 8
#define SOUND_BITS_16 16

#define SOUND_CHANNELS_0 0
#define SOUND_CHANNELS_1 1
#define SOUND_CHANNELS_2 2

#define SOUND_FREQUENCY_0 0

#define SOUND_FREQUENCY_11025 11025
#define SOUND_FREQUENCY_16000 16000
#define SOUND_FREQUENCY_22050 22050
#define SOUND_FREQUENCY_32000 32000
#define SOUND_FREQUENCY_44100 44100

#define INVALID_SOUND_SLOT_INDEX 0
#define INVALID_SOUND_SLOT_ID 0

#define MAX_SOUND_BUFFER_COUNT 32

#define MIN_VALID_SOUND_SLOT_COUNT 1
#define MAX_SOUND_SLOT_COUNT 128

#define SOUND_REVERBMIX_PROPERTY_MULTIPLIER (1.0 / 127.0)

#define INVALID_SOUND_REVERB_PROPERTY_VALUE (-1)
#define MAX_SOUND_REVERB_PROPERTY_VALUE (16777215)
#define SOUND_REVERB_PROPERTY_MULTIPLIER (1.0 / 16777215.0)

#define MIN_SOUND_VOLUME_VALUE (0.0f)
#define MAX_SOUND_VOLUME_VALUE (1.0f)

#define SOUNDVOLUMEVALUELOW(value) ((value << 16) >> 16)

#define SOUNDVOLUMEVALUEHIGH(value) (value >> 16)

#define MAX_SOUND_VOLUME_COUNT 64

#define SOUND_VOLUME_INDEX_MULTIPLIER 2

#define KSPROPERTY_SUPPORT_NONE 0
#define KSPROPERTY_SUPPORT_GET_SET_SUPPORTED (KSPROPERTY_SUPPORT_GET | KSPROPERTY_SUPPORT_SET)

#define DEFAULT_SOUND_BUFFER_BLOCK_ALIGN 2
#define DEFAULT_SOUND_BUFFER_BYTE_SIZE 128

#define SOUND_FREQUENCY_MULTIPLIER 1000

#define SOUND_PLAY_ITERATION_STEP 16

#define DEFAULT_SOFTWARE_SOUND_BUFFER_SIZE 40960

#define SOUND_BUFFER_LENGTH_MASK 0xf

#define SOUND_BUFFER_POSITION_MULTIPLIER (1.0 / 65536.0)

#define SOUND_BUFFER_POSITION_ALIGNMENT_MASK 0xffff0

namespace SoundModule
{
    extern GUID DSPROPSETID_VoiceManager;
    extern GUID DSPROPSETID_EAX_ReverbProperties;
    extern GUID DSPROPSETID_EAXBUFFER_ReverbProperties;

    typedef enum SOUND_MODE
    {
        SOUND_MODE_SOFTWARE = 0,
        SOUND_MODE_HARDWARE = 1
    } SOUND_MODE;

    struct SoundSlot
    {
        u32 ID;

        IDirectSoundBuffer* Buffer;

        s32 Unk03; // TODO
        s32 Unk04; // TODO
        s32 Unk05; // TODO

        u16 Frequency;
        u8 Unk07;
        u8 Volume;
        u8 Unk10;
        u8 Channels;
        u8 Unk12;
        u8 Unk13;
    };

    struct SoundBuffer
    {
        struct
        {
            IDirectSoundBuffer* Active;
            IDirectSoundBuffer* Main;
            IDirectSound3DBuffer* Volume;
        } Buffers;

        IKsPropertySet* Set;

        u16 Unk05; // TODO
        u16 Unk06; // TODO

        struct
        {
            u16 Frequency;
            bool IsLooping;
            u8 Volume;
            u8 Level;
            u8 Slot;
            u8 Unk11; // TODO
            u8 Unk12; // TODO
        } Settings;
    };

    struct ModuleContainer
    {
        struct
        {
            IDirectSound* Instance; // 0x0040a8dc

            struct
            {
                IDirectSoundBuffer* Main; // 0x0040a8e0
                IDirectSoundBuffer* Software; // 0x0040a8e4
                IDirectSoundBuffer* Active; // 0x0040a8e8
            } Buffers;
        } DX;

        struct
        {
            bool IsInit; // 0x0040a8ec
            bool IsActive; // 0x0040a8ed
            bool IsLive; // 0x0040a8ee
        } State;

        struct
        {
            u8 BlockAlignBits; // 0x0040a8ef
            u8 BlockAlignBytes; // 0x0040a8f0
            bool IsEmulated; // 0x0040a8f1

            bool Is3D; // 0x0040a8f3


            u32 Unk01; // 0x0040a8f4
            u32 Unk02; // 0x0040a8f8
            DWORD CurrentWrite; // 0x0040a8fc
            DWORD MinimumWrite; // 0x0040a900
            u32 Unk05; // 0x0040a904
            s32 MaximumPlay; // 0x0040a908
            s32 MinimumPlay; // 0x0040a90c

            u32 BufferSize; // 0x0040a914
            u32 Unk09; // 0x0040a918
            u32 Unk10; // 0x0040a91c
            u32 Unk11; // 0x0040a920
            u32 Unk12; // 0x0040a924
            u32 Unk13; // 0x0040a928

            struct
            {
                void* Data; // 0x0040a92c
                DWORD Size; // 0x0040a930
            } Lock;

            s32 Unk16; // 0x0040a934
            u32 Capabilities; // 0x0040a938
            u32 Frequency; // 0x0040a93c
        } Settings;

        struct
        {
            SOUNDMODULEUNKNOWN1LAMBDA Lambda1; // 0x0040a940
            SOUNDMODULESTOPSOUNDBUFFERLAMBDA StopBuffer; // 0x0040a944
            SOUNDMODULELOGMESSAGELAMBDA LogMessage; // 0x0040a948
            SOUNDMODULEUNKNOWNLAMBDA Unknown; // 0x0040a94c
            SOUNDMODULEUNKNOWN5LAMBDA Lambda5; // 0x0040a950
            SOUNDMODULEUNKNOWN6LAMBDA Lambda6; // 0x0040a954
            SOUNDMODULEACQUIRESOUNDBUFFERPOSITIONLAMBDA AcquireSoundBufferPosition; // 0x0040a958
            SOUNDMODULEUNKNOWN8LAMBDA Lambda8; // 0x0040a95c
        } Lambdas;

        SoundBuffer Buffers[MAX_SOUND_BUFFER_COUNT]; // 0x0040a960
        SoundSlot Slots[MAX_SOUND_SLOT_COUNT]; // 0x0040ace0

        struct
        {
            IDirectSound3DListener* Listener; // 0x0040bb00
            IKsPropertySet* Instance; // 0x0040bb04
            IDirectSoundBuffer* Buffer; // 0x0040bb08

            struct
            {
                u32 Type; // 0x0040bb0c // TODO enum
                s32 Decay; // 0x0040bb10
                s32 Damping; // 0x0040bb14
            } Settings;

            struct
            {
                VmMode Mode; // 0040bb18
                bool IsActive; // 0x0040bb1c
                bool IsReverb; // 0x0040bb1d
            } State;
        } EAX;

        struct
        {
            u32 Unknown1; // 0x0040bc20
            u32 Unknown2; // 0x0040bc24
        } Counters;
    };

    extern ModuleContainer  State;
    extern const u32 Volume[MAX_SOUND_VOLUME_COUNT];

    void Message(const char* format, ...);

    void InitializeSoundExtensions(void);
    s32 InitializeVoiceManager(const VmMode value, VmMode* out);
    void ReleaseSoundExtensions(void);

    s32 InitializeSounds(const u32 options, const HWND hwnd, const SOUND_MODE mode);
    s32 InitializeSoundSlots(void);
    s32 InitializeSoundSlot(const u32 indx);
    s32 InitializeSoundBuffers(void);
    u32 AcquireAvailableSoundSlotIndex(void);

    u32 AcquireSoundPosition(void);
    void AcquireSoundData(void* data, const s32 size);
    void StopSoundBuffers(void);
}