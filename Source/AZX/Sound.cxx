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

#include "AppWindow.hxx"
#include "Sound.hxx"
#include "SoundModule.hxx"

using namespace AppWindow;
using namespace SoundModule;

namespace Sound
{
    SoundContainer SoundState;

    // 0x0041f3c0
    BOOL Is3DSoundAvailable(void)
    {
        return (AcquireSoundCapabilities() & SOUND_MODULE_CAPABILITIES_3D_BUFFER_SUPPORTED) ? TRUE : FALSE;
    }

    // 0x0054909c
    // a.k.a. SNDcaps
    u32 AcquireSoundCapabilities(void)
    {
        if (*SoundState.Lambdas._Unknown001 == NULL) { *SoundState.Lambdas._Unknown001 = FUN_0054b760; }

        if (SOUND_MODULE_FAILURE < InitializeSoundModule())
        {
            u32 caps = (*SoundModuleState._AcquireCapabilities)(AcquireWindow());

            if (SOUND_MODULE_CAPABILITIES_NONE < (s32)caps) { caps = caps | SOUND_MODULE_CAPABILITIES_UNKNOWN; }

            return caps;
        }

        return INVALID_SOUND_MODULE_CAPABILITIES;
    }
}