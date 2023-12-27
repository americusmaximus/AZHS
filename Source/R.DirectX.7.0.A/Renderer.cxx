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

#include "Graphics.Basic.hxx"
#include "Renderer.hxx"
#include "RendererValues.hxx"
#include "Settings.hxx"

#include <malloc.h>
#include <math.h>
#include <stdio.h>

#define MAX_MESSAGE_BUFFER_LENGTH 512

#define MAX_RENDERER_MESSAGE_BUFFER_LENGTH 80
#define MAX_RENDERER_MESSAGE_DESCRIPTION_BUFFER_LENGTH 64

#define MAX_SETTINGS_BUFFER_LENGTH 80

#define MAX_DEVICE_FORMAT_SCORE 10000

using namespace Renderer;
using namespace RendererModuleValues;
using namespace Settings;

namespace RendererModule
{
    RendererModuleState State;

    u32 DAT_60018850; // TODO
    u32 DAT_60058df4; // TODO
    u32 DAT_60058df8; // TODO
    u32 DAT_6005ab50; // TODO
    u32 DAT_6005ab5c; // TODO

    // 0x60009250
    s32 AcquireSettingsValue(const s32 value, const char* section, const char* name)
    {
        char buffer[MAX_SETTINGS_BUFFER_LENGTH];

        sprintf(buffer, "%s_%s", section, name);

        const char* tv = getenv(buffer);

        if (tv == NULL)
        {
            sprintf(buffer, "THRASH_%s", name);

            const char* ttv = getenv(buffer);

            return ttv == NULL ? value : atoi(ttv);
        }

        return atoi(tv);
    }

    // 0x6000b9e0
    // 0x6000ba10
    // NOTE: Originally there are 3 different methods for error, warning, and info (which is never being called).
    void Message(const u32 severity, const char* format, ...)
    {
        char buffer[MAX_MESSAGE_BUFFER_LENGTH];

        va_list args;
        va_start(args, format);
        vsnprintf_s(buffer, MAX_MESSAGE_BUFFER_LENGTH, format, args);
        va_end(args);

        if (severity == RENDERER_MODULE_MESSAGE_SEVERITY_ERROR)
        {
            if (State.Lambdas.Log != NULL) { State.Lambdas.Log(severity, buffer); }
            else { MessageBoxA(NULL, buffer, "Abort Message", MB_SETFOREGROUND | MB_SYSTEMMODAL | MB_ICONERROR | MB_OKCANCEL); }
        }
    }

    // 0x600036b0
    void ReleaseRendererModule(void)
    {
        RestoreGameWindow();
    }

    // 0x6000b970
    const char* AcquireRendererMessageDescription(const HRESULT code)
    {
        static char buffer[MAX_RENDERER_MESSAGE_DESCRIPTION_BUFFER_LENGTH]; // 0x60058db0

        sprintf(buffer, "Direct draw error code (0x%lx, %d)", code, code & 0xffff);

        return buffer;
    }

    // 0x60003a20
    const char* AcquireRendererMessage(const HRESULT code)
    {
        static char buffer[MAX_RENDERER_MESSAGE_BUFFER_LENGTH]; // 0x600186f0

        sprintf(buffer, "DX7 Error Code: %s (%8x)", AcquireRendererMessageDescription(code), code);

        return buffer;
    }

    // 0x60001f40
    u32 AcquireRendererDeviceCount(void)
    {
        State.Devices.Count = 0;
        State.Device.Identifier = NULL;

        GUID* uids[MAX_ENUMERATE_DEVICE_COUNT];

        State.Devices.Count = AcquireDirectDrawDeviceCount(uids, NULL, ENVIRONMENT_SECTION_NAME);

        u32 indx = 0;

        for (u32 x = 0; x < State.Devices.Count; x++)
        {
            if (AcquireRendererDeviceAccelerationState(x))
            {
                State.Devices.Indexes[indx] = uids[x];

                IDirectDraw* instance = NULL;
                DirectDrawCreate(uids[x], &instance, NULL);

                IDirectDraw7* dd = NULL;
                if (instance->QueryInterface(IID_IDirectDraw7, (void**)&dd) != DD_OK) { instance->Release(); continue; }

                DDDEVICEIDENTIFIER2 identifier;
                ZeroMemory(&identifier, sizeof(DDDEVICEIDENTIFIER2));

                dd->GetDeviceIdentifier(&identifier, DDGDI_GETHOSTIDENTIFIER);

                strncpy(State.Devices.Enumeration.Names[indx], identifier.szDescription, MAX_ENUMERATE_DEVICE_NAME_LENGTH);

                if (dd != NULL) { dd->Release(); dd = NULL; }
                if (instance != NULL) { instance->Release(); instance = NULL; }

                indx = indx + 1;
            }
        }

        State.Device.Identifier = State.Devices.Indexes[0];
        State.Devices.Count = indx;

        return State.Devices.Count;
    }

    // 0x6000bb10
    u32 AcquireDirectDrawDeviceCount(GUID** uids, HMONITOR** monitors, const char* section)
    {
        State.Devices.Enumeration.Count = 0;
        State.Devices.Enumeration.IsAvailable = FALSE;

        {
            const char* value = getenv(RENDERER_MODULE_DISPLAY_ENVIRONMENT_PROPERTY_NAME);

            if (value == NULL || atoi(value) != 0)
            {
                if (!AcquireState(RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_STATE))
                {
                    DirectDrawEnumerateExA(EnumerateDirectDrawDevices, NULL, DDENUM_ATTACHEDSECONDARYDEVICES);
                    DirectDrawEnumerateExA(EnumerateDirectDrawDevices, NULL, DDENUM_NONDISPLAYDEVICES);
                }
                else
                {
                    State.Devices.Enumeration.Count = 1;
                    State.Devices.Enumeration.IsAvailable = TRUE;
                }
            }
            else
            {
                State.Devices.Enumeration.Count = 1;
                State.Devices.Enumeration.IsAvailable = TRUE;
            }
        }

        if (uids != NULL)
        {
            for (u32 x = 0; x < State.Devices.Enumeration.Count; x++)
            {
                uids[x] = State.Devices.Enumeration.Identifiers.Indexes[x];
            }
        }

        if (monitors != NULL)
        {
            for (u32 x = 0; x < State.Devices.Enumeration.Count; x++)
            {
                monitors[x] = State.Devices.Enumeration.Monitors.Indexes[x];
            }
        }

        State.Devices.Enumeration.Count = AcquireSettingsValue(State.Devices.Enumeration.Count, section, "displays");

        return State.Devices.Enumeration.Count;
    }

    // 0x6000bbd0
    BOOL CALLBACK EnumerateDirectDrawDevices(GUID* uid, LPSTR name, LPSTR description, LPVOID context, HMONITOR monitor)
    {
        IDirectDraw* instance = NULL;
        if (DirectDrawCreate(uid, &instance, NULL) != DD_OK) { return FALSE; }

        IDirectDraw4* dd = NULL;
        if (instance->QueryInterface(IID_IDirectDraw4, (void**)&dd) != DD_OK) { return FALSE; }

        instance->Release();

        DDDEVICEIDENTIFIER idn;
        ZeroMemory(&idn, sizeof(DDDEVICEIDENTIFIER));

        dd->GetDeviceIdentifier(&idn, DDGDI_NONE);

        DDDEVICEIDENTIFIER idh;
        ZeroMemory(&idh, sizeof(DDDEVICEIDENTIFIER));

        dd->GetDeviceIdentifier(&idh, DDGDI_GETHOSTIDENTIFIER);

        BOOL skip = FALSE;

        if (State.Devices.Enumeration.Count == 0)
        {
            CopyMemory(&State.Devices.Enumeration.Identifier, &idh, sizeof(DDDEVICEIDENTIFIER));
        }
        else
        {
            const BOOL same = strcmp(State.Devices.Enumeration.Identifier.szDescription, idh.szDescription) == 0;

            if (!same && (!State.Devices.Enumeration.IsAvailable || uid == NULL))
            {
                skip = TRUE;

                if (uid != NULL) { State.Devices.Enumeration.IsAvailable = TRUE; }
            }
        }

        if (dd != NULL) { dd->Release(); }

        if (!skip && State.Devices.Enumeration.Count < MAX_ENUMERATE_DEVICE_COUNT)
        {
            if (uid != NULL)
            {
                State.Devices.Enumeration.Identifiers.Identifiers[State.Devices.Enumeration.Count] = *uid;
                State.Devices.Enumeration.Monitors.Monitors[State.Devices.Enumeration.Count] = monitor;
            }

            State.Devices.Enumeration.Identifiers.Indexes[State.Devices.Enumeration.Count] = &State.Devices.Enumeration.Identifiers.Identifiers[State.Devices.Enumeration.Count];
            State.Devices.Enumeration.Monitors.Indexes[State.Devices.Enumeration.Count] = &State.Devices.Enumeration.Monitors.Monitors[State.Devices.Enumeration.Count];

            State.Devices.Enumeration.Count = State.Devices.Enumeration.Count + 1;
        }

        return TRUE;
    }

    // 0x6000bd60
    BOOL AcquireRendererDeviceAccelerationState(const u32 indx)
    {
        IDirectDraw* instance = NULL;
        if (DirectDrawCreate(State.Devices.Enumeration.Identifiers.Indexes[indx], &instance, NULL) != DD_OK) { return FALSE; }

        IDirectDraw4* dd = NULL;
        if (instance->QueryInterface(IID_IDirectDraw4, (void**)&dd) != DD_OK) { return FALSE; }

        instance->Release();

        DDCAPS hal;
        ZeroMemory(&hal, sizeof(DDCAPS));

        hal.dwSize = sizeof(DDCAPS);

        dd->GetCaps(&hal, NULL);

        dd->Release();

        return hal.dwCaps & DDCAPS_3D;
    }

    // 0x600034c0
    u32 InitializeRendererDeviceLambdas(void)
    {
        if (State.Mutex == NULL) { State.Mutex = CreateEventA(NULL, FALSE, FALSE, NULL); }

        State.Window.HWND = State.Lambdas.Lambdas.AcquireWindow();

        if (State.Window.HWND != NULL)
        {
            State.Settings.CooperativeLevel = (State.Settings.CooperativeLevel & ~(DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN))
                | (DDSCL_MULTITHREADED | DDSCL_NORMAL);

            AcquireWindowModeCapabilities();

            State.Settings.CooperativeLevel = State.Settings.IsWindowMode
                ? State.Settings.CooperativeLevel | DDSCL_MULTITHREADED | DDSCL_NORMAL
                : State.Settings.CooperativeLevel | DDSCL_MULTITHREADED | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN;

            SetForegroundWindow(State.Window.HWND);
            PostMessageA(State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE, 0, 0);
            WaitForSingleObject(State.Mutex, 10000);

            return State.DX.Code;
        }

        State.DX.Code = RENDERER_MODULE_INITIALIZE_DEVICE_SUCCESS;

        return RENDERER_MODULE_INITIALIZE_DEVICE_SUCCESS;
    }

    // 0x600030b0
    u32 InitializeRendererDevice(void)
    {
        if (State.Window.HWND != NULL) { return RENDERER_MODULE_FAILURE; }

        State.Settings.CooperativeLevel = (State.Settings.CooperativeLevel & ~(DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN))
            | (DDSCL_MULTITHREADED | DDSCL_NORMAL);

        AcquireWindowModeCapabilities();

        State.Settings.CooperativeLevel = State.Settings.IsWindowMode
            ? State.Settings.CooperativeLevel | (DDSCL_MULTITHREADED | DDSCL_NORMAL)
            : State.Settings.CooperativeLevel | (DDSCL_MULTITHREADED | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);

        IDirectDraw* instance = NULL;
        State.DX.Code = DirectDrawCreate(State.Device.Identifier, &instance, NULL);

        if (State.DX.Code != DD_OK) { return RENDERER_MODULE_FAILURE; }

        State.DX.Code = DirectDrawCreateEx(State.Device.Identifier, (void**)&State.DX.Instance, IID_IDirectDraw7, NULL);

        instance->Release();

        if (State.DX.Code != DD_OK) { return RENDERER_MODULE_FAILURE; }

        State.DX.Code = State.DX.Instance->SetCooperativeLevel(State.Window.HWND, State.Settings.CooperativeLevel);

        if (State.DX.Code == DD_OK) { return RENDERER_MODULE_FAILURE; }

        u32 pitch = 0;
        u32 height = 0;

        {
            DDSURFACEDESC2 desc;
            ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

            desc.dwSize = sizeof(DDSURFACEDESC2);
            desc.dwFlags = DDSD_CAPS;
            desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

            IDirectDrawSurface7* surface = NULL;
            State.DX.Code = State.DX.Instance->CreateSurface(&desc, &surface, NULL);

            if (State.DX.Code == DD_OK)
            {
                surface->GetSurfaceDesc(&desc);

                pitch = desc.lPitch;
                height = desc.dwHeight;
            }
            else
            {
                LOGWARNING("*** FAILURE in creating primary surface (error code: %8x)***\n", State.DX.Code);
            }

            if (surface != NULL) { surface->Release(); }
        }

        {
            DWORD free = 0;
            DWORD total = 0;

            {
                DDSCAPS2 caps;
                ZeroMemory(&caps, sizeof(DDSCAPS2));

                caps.dwCaps = DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;

                if (State.DX.Instance->GetAvailableVidMem(&caps, &total, &free) == DD_OK)
                {
                    State.Settings.MaxAvailableMemory = total;

                    ModuleDescriptor.MemorySize = total;
                    ModuleDescriptor.MemoryType = 3; // TODO
                }
                else
                {
                    State.Settings.MaxAvailableMemory = MIN_DEVICE_AVAIABLE_VIDEO_MEMORY;

                    ModuleDescriptor.MemorySize = 0;
                    ModuleDescriptor.MemoryType = 0; // TODO
                }
            }

            {
                DDSCAPS2 caps;
                ZeroMemory(&caps, sizeof(DDSCAPS2));

                caps.dwCaps = DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;

                const HRESULT result = State.DX.Instance->GetAvailableVidMem(&caps, &total, &free);

                State.Settings.MaxAvailableMemory = result == DD_OK
                    ? height * pitch + total
                    : MIN_DEVICE_AVAIABLE_VIDEO_MEMORY;
            }

            {
                DDSCAPS2 caps;
                ZeroMemory(&caps, sizeof(DDSCAPS2));

                caps.dwCaps = DDSCAPS_NONLOCALVIDMEM | DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;

                if (State.DX.Instance->GetAvailableVidMem(&caps, &total, &free) == DD_OK)
                {
                    if (total != 0) { ModuleDescriptor.MemoryType = ModuleDescriptor.MemoryType | 0x8000; } // TODO
                }
            }
        }

        {
            DDCAPS hal;
            ZeroMemory(&hal, sizeof(DDCAPS));

            hal.dwSize = sizeof(DDCAPS);

            DDCAPS hel;
            ZeroMemory(&hel, sizeof(DDCAPS));

            hel.dwSize = sizeof(DDCAPS);

            if (State.DX.Instance->GetCaps(&hal, &hel) == DD_OK)
            {
                State.Device.Capabilities.IsAccelerated = hal.dwCaps & DDCAPS_3D;
            }
        }

        ZeroMemory(ModuleDescriptor.Capabilities.Capabilities,
            MAX_DEVICE_CAPABILITIES_COUNT * sizeof(RendererModuleDescriptorDeviceCapabilities));

        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Width = GRAPHICS_RESOLUTION_640;
        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Height = GRAPHICS_RESOLUTION_480;
        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Bits = GRAPHICS_BITS_PER_PIXEL_16;
        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Format = RENDERER_PIXEL_FORMAT_R5G6B5;
        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Unk03 = 1;
        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Unk04 = 1;
        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].IsActive = TRUE;

        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Width = GRAPHICS_RESOLUTION_800;
        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Height = GRAPHICS_RESOLUTION_600;
        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Bits = GRAPHICS_BITS_PER_PIXEL_16;
        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Format = RENDERER_PIXEL_FORMAT_R5G6B5;
        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Unk03 = 1;
        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Unk04 = 1;
        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].IsActive = TRUE;

        ModuleDescriptor.Capabilities.Count = 5;

        State.DX.Instance->EnumDisplayModes(DDEDM_NONE, NULL,
            &ModuleDescriptor.Capabilities.Count, EnumerateRendererDeviceModes);

        AssignRendererDeviceResolutionMode(AcquireMinimumRendererDeviceResolutionModeIndex(GRAPHICS_RESOLUTION_640, GRAPHICS_RESOLUTION_480, GRAPHICS_BITS_PER_PIXEL_16), RENDERER_RESOLUTION_MODE_640_480_16);
        AssignRendererDeviceResolutionMode(AcquireMinimumRendererDeviceResolutionModeIndex(GRAPHICS_RESOLUTION_800, GRAPHICS_RESOLUTION_600, GRAPHICS_BITS_PER_PIXEL_16), RENDERER_RESOLUTION_MODE_800_600_16);
        AssignRendererDeviceResolutionMode(AcquireMinimumRendererDeviceResolutionModeIndex(GRAPHICS_RESOLUTION_1024, GRAPHICS_RESOLUTION_768, GRAPHICS_BITS_PER_PIXEL_16), RENDERER_RESOLUTION_MODE_1024_768_16);
        AssignRendererDeviceResolutionMode(AcquireMinimumRendererDeviceResolutionModeIndex(GRAPHICS_RESOLUTION_1280, GRAPHICS_RESOLUTION_1024, GRAPHICS_BITS_PER_PIXEL_16), RENDERER_RESOLUTION_MODE_1280_1024_16);
        AssignRendererDeviceResolutionMode(AcquireMinimumRendererDeviceResolutionModeIndex(GRAPHICS_RESOLUTION_1600, GRAPHICS_RESOLUTION_1200, GRAPHICS_BITS_PER_PIXEL_16), RENDERER_RESOLUTION_MODE_1600_1200_16);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60002eb0
    void AcquireWindowModeCapabilities(void)
    {
        IDirectDraw* instance = NULL;
        State.DX.Code = DirectDrawCreate(State.Device.Identifier, &instance, NULL);

        State.DX.Code = instance->SetCooperativeLevel(State.Window.HWND, State.Settings.CooperativeLevel);

        IDirectDraw7* dd = NULL;
        State.DX.Code = instance->QueryInterface(IID_IDirectDraw7, (void**)&dd);

        DDCAPS hal;
        ZeroMemory(&hal, sizeof(DDCAPS));

        hal.dwSize = sizeof(DDCAPS);

        DDCAPS hel;
        ZeroMemory(&hel, sizeof(DDCAPS));

        hel.dwSize = sizeof(DDCAPS);

        dd->GetCaps(&hal, &hel);

        if ((hal.dwCaps2 & DDCAPS2_CANRENDERWINDOWED) == 0) { State.Settings.IsWindowMode = FALSE; }

        const HWND hwnd = GetDesktopWindow();
        const HDC hdc = GetWindowDC(hwnd);

        const u32 bits = GetDeviceCaps(hdc, BITSPIXEL);

        if (bits < (GRAPHICS_BITS_PER_PIXEL_8 + 1) || bits == GRAPHICS_BITS_PER_PIXEL_24) { State.Settings.IsWindowMode = FALSE; }
        else if (bits == GRAPHICS_BITS_PER_PIXEL_32)
        {
            IDirect3D3* dx = NULL;

            {
                const HRESULT result = instance->QueryInterface(IID_IDirect3D3, (void**)&dx);

                if (result != DD_OK) { LOGERROR("SetDesktopMode Test failed! %s\n", AcquireRendererMessage(result)); }
            }

            {
                const HRESULT result = dx->EnumZBufferFormats(IID_IDirect3DHALDevice, EnumerateRendererDeviceDepthPixelFormats, NULL);

                if (result != DD_OK) { LOGERROR("DX7_SetDesktopModeIfNotWin: %s\n", AcquireRendererMessage(result)); }
            }

            BOOL found = FALSE;

            for (u32 x = 0; x < MAX_TEXTURE_DEPTH_FORMAT_COUNT; x++)
            {
                if (State.Textures.Formats.Depth.Formats[x] == bits) { found = TRUE; }
            }

            if (!found) { State.Settings.IsWindowMode = FALSE; }

            if (dx != NULL) { dx->Release(); }
        }

        dd->Release();
    }

    // 0x60003080
    HRESULT CALLBACK EnumerateRendererDeviceDepthPixelFormats(LPDDPIXELFORMAT format, LPVOID ctx)
    {
        State.Textures.Formats.Depth.Formats[State.Textures.Formats.Depth.Count] = format->dwRGBBitCount;
        State.Textures.Formats.Depth.Count = State.Textures.Formats.Depth.Count + 1;

        // NOTE: The original does not have this check,
        // thus it is prone the array overflow that can cause crash in some cases.
        if (MAX_TEXTURE_DEPTH_FORMAT_COUNT <= (State.Textures.Formats.Depth.Count + 1)) { return D3DENUMRET_CANCEL; }

        return D3DENUMRET_OK;
    }

    // 0x60002710
    HRESULT CALLBACK EnumerateRendererDeviceModes(LPDDSURFACEDESC2 desc, LPVOID context)
    {
        const u32 format = AcquirePixelFormat(&desc->ddpfPixelFormat);

        const u32 bits = desc->ddpfPixelFormat.dwRGBBitCount;

        if (format != RENDERER_PIXEL_FORMAT_NONE
            && bits != GRAPHICS_BITS_PER_PIXEL_8 && bits != GRAPHICS_BITS_PER_PIXEL_24
            && (GRAPHICS_RESOLUTION_512 - 1) < desc->dwWidth)
        {
            const u32 bytes = bits == (GRAPHICS_BITS_PER_PIXEL_16 - 1) ? 2 : (bits >> 3);

            const u32 width = desc->dwWidth;
            const u32 height = desc->dwHeight;
            const u32 count = State.Settings.MaxAvailableMemory / (height * width * bytes);

            u32 indx = 0;

            if (width == GRAPHICS_RESOLUTION_640 && height == GRAPHICS_RESOLUTION_480 && bits == GRAPHICS_BITS_PER_PIXEL_16) { indx = 1; }
            else if (width == GRAPHICS_RESOLUTION_800 && height == GRAPHICS_RESOLUTION_600 && bits == GRAPHICS_BITS_PER_PIXEL_16) { indx = 2; }
            else
            {
                indx = *(u32*)context;

                if ((MAX_DEVICE_CAPABILITIES_COUNT - 1) < indx) { return DDENUMRET_CANCEL; }

                *(u32*)context = indx + 1;
            }

            ModuleDescriptor.Capabilities.Capabilities[indx].Width = width;
            ModuleDescriptor.Capabilities.Capabilities[indx].Height = height;
            ModuleDescriptor.Capabilities.Capabilities[indx].Bits =
                format == RENDERER_PIXEL_FORMAT_R5G5B5 ? (GRAPHICS_BITS_PER_PIXEL_16 - 1) : bits;

            ModuleDescriptor.Capabilities.Capabilities[indx].Format = format;

            if (count < 4) // TODO
            {
                ModuleDescriptor.Capabilities.Capabilities[indx].Unk03 = count;
                ModuleDescriptor.Capabilities.Capabilities[indx].Unk04 = count - 1;
            }
            else
            {
                ModuleDescriptor.Capabilities.Capabilities[indx].Unk03 = 3;
                ModuleDescriptor.Capabilities.Capabilities[indx].Unk04 = 3;
            }

            ModuleDescriptor.Capabilities.Capabilities[indx].IsActive = TRUE;
        }

        return DDENUMRET_OK;
    }

    // 0x60003960
    u32 AcquirePixelFormat(const DDPIXELFORMAT* format)
    {
        const u32 bits = format->dwRGBBitCount;

        const u32 red = format->dwRBitMask;
        const u32 green = format->dwGBitMask;
        const u32 blue = format->dwBBitMask;

        if (bits == GRAPHICS_BITS_PER_PIXEL_16)
        {
            if (red == 0x7c00 && green == 0x3e0 && blue == 0x1f) { return RENDERER_PIXEL_FORMAT_R5G5B5; }
            else if (red == 0xf800 && green == 0x7e0 && blue == 0x1f) { return RENDERER_PIXEL_FORMAT_R5G6B5; }
            else if (red == 0xf00 && green == 0xf0 && blue == 0xf && format->dwRGBAlphaBitMask == 0xf000) { return RENDERER_PIXEL_FORMAT_R4G4B4; }
        }
        else if (red == 0xff0000 && green == 0xff00 && blue == 0xff)
        {
            if (bits == GRAPHICS_BITS_PER_PIXEL_24) { return RENDERER_PIXEL_FORMAT_R8G8B8; }
            else if (bits == GRAPHICS_BITS_PER_PIXEL_32) { return RENDERER_PIXEL_FORMAT_A8R8G8B8; }
        }

        return RENDERER_PIXEL_FORMAT_NONE;
    }

    // 0x60002e20
    s32 AcquireMinimumRendererDeviceResolutionModeIndex(const u32 width, const u32 height, const u32 bpp)
    {
        s32 result = RENDERER_RESOLUTION_MODE_NONE;
        s32 min = MAX_DEVICE_FORMAT_SCORE;

        for (u32 x = 1; x < ModuleDescriptor.Capabilities.Count; x++)
        {
            const RendererModuleDescriptorDeviceCapabilities* caps = &ModuleDescriptor.Capabilities.Capabilities[x];
            const s32 score = AcquireRendererDeviceResulutionModeScore(caps, width, height, bpp);

            if (score < min)
            {
                result = x;
                min = score;

                if (score == RENDERER_RESOLUTION_MODE_NONE) { return x; }
            }
        }

        if (min == RENDERER_RESOLUTION_MODE_NONE || min == RENDERER_RESOLUTION_MODE_INVALID) { return result; }

        return RENDERER_RESOLUTION_MODE_INVALID;
    }

    // 0x60002dc0
    s32 AcquireRendererDeviceResulutionModeScore(const RendererModuleDescriptorDeviceCapabilities* caps, const u32 width, const u32 height, const u32 bpp)
    {
        s32 result = MAX_DEVICE_FORMAT_SCORE;

        if (caps->Width == width && caps->Height == height && caps->IsActive)
        {
            result = caps->Bits - bpp;

            if (result < 0 && result != MAX_DEVICE_FORMAT_SCORE) { result = -2 * result; } // TODO constant
        }

        if (caps->Bits < (GRAPHICS_BITS_PER_PIXEL_8 + 1))
        {
            if (GRAPHICS_BITS_PER_PIXEL_8 < bpp) { return MAX_DEVICE_FORMAT_SCORE; }

            if (caps->Bits < (GRAPHICS_BITS_PER_PIXEL_8 + 1)) { return result; }
        }

        if (bpp < (GRAPHICS_BITS_PER_PIXEL_8 + 1)) { result = result * 4; } // TODO constant

        return result;
    }

    // 0x60002d40
    void AssignRendererDeviceResolutionMode(const s32 src, const u32 dst)
    {
        RendererModuleDescriptorDeviceCapabilities* dm = NULL;
        RendererModuleDescriptorDeviceCapabilities* sm = NULL;

        if (src == RENDERER_RESOLUTION_MODE_INVALID)
        {
            sm = &ModuleDescriptor.Capabilities.Capabilities[dst];
        }
        else
        {
            sm = &ModuleDescriptor.Capabilities.Capabilities[src];
            dm = &ModuleDescriptor.Capabilities.Capabilities[dst];

            dm->Width = sm->Width;
            dm->Height = sm->Height;
            dm->Bits = sm->Bits;
            dm->Format = sm->Format;
            dm->Unk03 = sm->Unk03;
            dm->Unk04 = sm->Unk04;
            dm->IsActive = sm->IsActive;

            if (src == dst) { return; }
        }

        sm->Width = 0;
        sm->Height = 0;
        sm->Bits = 0;
        sm->Format = RENDERER_PIXEL_FORMAT_NONE;
        sm->Unk03 = 0;
        sm->Unk04 = 0;
        sm->IsActive = FALSE;
    }

    // 0x600021f0
    u32 STDCALLAPI InitializeRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        IDirectDraw* instance = NULL;
        State.DX.Code = DirectDrawCreate(State.Device.Identifier, &instance, NULL);
        State.DX.Code = DirectDrawCreateEx(State.Device.Identifier, (void**)&State.DX.Instance, IID_IDirectDraw7, NULL);

        if (State.DX.Code == DD_OK)
        {
            State.Lambdas.Lambdas.SelectInstance(instance);

            instance->Release();

            State.DX.Code = State.DX.Instance->SetCooperativeLevel(State.Window.HWND, State.Settings.CooperativeLevel);

            if (State.DX.Code == DD_OK)
            {
                u32 pitch = 0;
                u32 height = 0;

                {
                    DDSURFACEDESC2 desc;
                    ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

                    desc.dwSize = sizeof(DDSURFACEDESC2);
                    desc.dwFlags = DDSD_CAPS;
                    desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

                    IDirectDrawSurface7* surface = NULL;
                    State.DX.Code = State.DX.Instance->CreateSurface(&desc, &surface, NULL);

                    if (State.DX.Code == DD_OK)
                    {
                        surface->GetSurfaceDesc(&desc);

                        pitch = desc.lPitch;
                        height = desc.dwHeight;
                    }
                    else
                    {
                        LOGWARNING("*** FAILURE in creating primary surface (error code: %8x)***\n", State.DX.Code);
                    }

                    if (surface != NULL) { surface->Release(); }
                }

                {
                    DWORD free = 0;
                    DWORD total = 0;

                    {
                        DDSCAPS2 caps;
                        ZeroMemory(&caps, sizeof(DDSCAPS2));

                        caps.dwCaps = DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;

                        if (State.DX.Instance->GetAvailableVidMem(&caps, &total, &free) == DD_OK)
                        {
                            State.Settings.MaxAvailableMemory = total;

                            ModuleDescriptor.MemorySize = total;
                            ModuleDescriptor.MemoryType = 3; // TODO
                        }
                        else
                        {
                            State.Settings.MaxAvailableMemory = MIN_DEVICE_AVAIABLE_VIDEO_MEMORY;

                            ModuleDescriptor.MemorySize = 0;
                            ModuleDescriptor.MemoryType = 0; // TODO
                        }
                    }

                    {
                        DDSCAPS2 caps;
                        ZeroMemory(&caps, sizeof(DDSCAPS2));

                        caps.dwCaps = DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;

                        const HRESULT result = State.DX.Instance->GetAvailableVidMem(&caps, &total, &free);

                        State.Settings.MaxAvailableMemory = result == DD_OK
                            ? height * pitch + total
                            : MIN_DEVICE_AVAIABLE_VIDEO_MEMORY;

                        ModuleDescriptor.MemorySize = State.Settings.MaxAvailableMemory;
                    }

                    {
                        DDSCAPS2 caps;
                        ZeroMemory(&caps, sizeof(DDSCAPS2));

                        caps.dwCaps = DDSCAPS_NONLOCALVIDMEM | DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;

                        if (State.DX.Instance->GetAvailableVidMem(&caps, &total, &free) == DD_OK)
                        {
                            if (total != 0) { ModuleDescriptor.MemoryType = ModuleDescriptor.MemoryType | 0x8000; } // TODO
                        }
                    }
                }

                {
                    DDCAPS hal;
                    ZeroMemory(&hal, sizeof(DDCAPS));

                    hal.dwSize = sizeof(DDCAPS);

                    DDCAPS hel;
                    ZeroMemory(&hel, sizeof(DDCAPS));

                    hel.dwSize = sizeof(DDCAPS);

                    if (State.DX.Instance->GetCaps(&hal, &hel) == DD_OK)
                    {
                        State.Device.Capabilities.IsAccelerated = hal.dwCaps & DDCAPS_3D;
                    }
                }

                ZeroMemory(ModuleDescriptor.Capabilities.Capabilities,
                    MAX_DEVICE_CAPABILITIES_COUNT * sizeof(RendererModuleDescriptorDeviceCapabilities));

                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Width = GRAPHICS_RESOLUTION_640;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Height = GRAPHICS_RESOLUTION_480;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Bits = GRAPHICS_BITS_PER_PIXEL_16;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Format = RENDERER_PIXEL_FORMAT_R5G6B5;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Unk03 = 1;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Unk04 = 1;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].IsActive = TRUE;

                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Width = GRAPHICS_RESOLUTION_800;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Height = GRAPHICS_RESOLUTION_600;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Bits = GRAPHICS_BITS_PER_PIXEL_16;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Format = RENDERER_PIXEL_FORMAT_R5G6B5;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Unk03 = 2;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Unk04 = 1;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].IsActive = TRUE;

                ModuleDescriptor.Capabilities.Count = 5;

                State.DX.Instance->EnumDisplayModes(DDEDM_NONE, NULL,
                    &ModuleDescriptor.Capabilities.Count, EnumerateRendererDeviceModes);

                AssignRendererDeviceResolutionMode(AcquireMinimumRendererDeviceResolutionModeIndex(GRAPHICS_RESOLUTION_640, GRAPHICS_RESOLUTION_480, GRAPHICS_BITS_PER_PIXEL_16), RENDERER_RESOLUTION_MODE_640_480_16);
                AssignRendererDeviceResolutionMode(AcquireMinimumRendererDeviceResolutionModeIndex(GRAPHICS_RESOLUTION_800, GRAPHICS_RESOLUTION_600, GRAPHICS_BITS_PER_PIXEL_16), RENDERER_RESOLUTION_MODE_800_600_16);
                AssignRendererDeviceResolutionMode(AcquireMinimumRendererDeviceResolutionModeIndex(GRAPHICS_RESOLUTION_1024, GRAPHICS_RESOLUTION_768, GRAPHICS_BITS_PER_PIXEL_16), RENDERER_RESOLUTION_MODE_1024_768_16);
                AssignRendererDeviceResolutionMode(AcquireMinimumRendererDeviceResolutionModeIndex(GRAPHICS_RESOLUTION_1280, GRAPHICS_RESOLUTION_1024, GRAPHICS_BITS_PER_PIXEL_16), RENDERER_RESOLUTION_MODE_1280_1024_16);
                AssignRendererDeviceResolutionMode(AcquireMinimumRendererDeviceResolutionModeIndex(GRAPHICS_RESOLUTION_1600, GRAPHICS_RESOLUTION_1200, GRAPHICS_BITS_PER_PIXEL_16), RENDERER_RESOLUTION_MODE_1600_1200_16);

                IDirect3D7* dx = NULL;
                State.DX.Instance->QueryInterface(IID_IDirect3D7, (void**)&dx);

                dx->EnumDevices(EnumerateDirectDrawAcceleratedDevices, NULL);

                for (u32 x = 0; x < ModuleDescriptor.Capabilities.Count; x++)
                {
                    const u32 bits = ModuleDescriptor.Capabilities.Capabilities[x].Bits;

                    BOOL invalid = FALSE;

                    if (bits == GRAPHICS_BITS_PER_PIXEL_16) { invalid = (State.Device.Capabilities.RendererDeviceDepthBits & DEPTH_BIT_MASK_16_BIT) == 0; }
                    else if (bits == GRAPHICS_BITS_PER_PIXEL_32) { invalid = (State.Device.Capabilities.RendererDeviceDepthBits & DEPTH_BIT_MASK_32_BIT) == 0; }

                    if (invalid)
                    {
                        ModuleDescriptor.Capabilities.Capabilities[x].Width = 0;
                        ModuleDescriptor.Capabilities.Capabilities[x].Height = 0;
                        ModuleDescriptor.Capabilities.Capabilities[x].Bits = 0;
                        ModuleDescriptor.Capabilities.Capabilities[x].Format = RENDERER_PIXEL_FORMAT_NONE;
                        ModuleDescriptor.Capabilities.Capabilities[x].Unk03 = 0;
                        ModuleDescriptor.Capabilities.Capabilities[x].Unk04 = 0;
                        ModuleDescriptor.Capabilities.Capabilities[x].IsActive = FALSE;
                    }
                }

                dx->Release();
            }
        }

        SetEvent(State.Mutex);

        *result = State.DX.Code;

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60002e90
    HRESULT CALLBACK EnumerateDirectDrawAcceleratedDevices(LPSTR description, LPSTR name, LPD3DDEVICEDESC7 desc, LPVOID context)
    {
        const u32 result = (desc->dwDevCaps & D3DDEVCAPS_HWRASTERIZATION) == 0;

        if (!result) { State.Device.Capabilities.RendererDeviceDepthBits = desc->dwDeviceRenderBitDepth; }

        return result;
    }

    // 0x60002d00
    u32 STDCALLAPI ReleaseRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        ReleaseRendererDeviceSurfaces();

        State.Lambdas.Lambdas.SelectInstance(NULL);

        if (State.DX.Instance != NULL)
        {
            State.DX.Instance->Release();
            State.DX.Instance = NULL;
        }

        SetEvent(State.Mutex);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60003610
    void ReleaseRendererDeviceSurfaces(void)
    {
        for (s32 x = (MAX_ACTIVE_SURFACE_COUNT - 1); x >= 0; x--)
        {
            if (State.DX.Surfaces.Active[x] != NULL)
            {
                State.DX.Surfaces.Active[x]->Release();
                State.DX.Surfaces.Active[x] = NULL;
            }
        }

        if (State.DX.Surfaces.Back != NULL)
        {
            State.DX.Surfaces.Back->Release();
            State.DX.Surfaces.Back = NULL;
        }

        if (State.DX.Surfaces.Main != NULL)
        {
            State.DX.Surfaces.Main->Release();
            State.DX.Surfaces.Main = NULL;
        }
    }

    // 0x60002850
    u32 STDCALLAPI InitializeRendererDeviceSurfacesExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        ReleaseRendererDevice();
        ReleaseRendererDeviceSurfaces();

        if (!State.Settings.IsWindowMode)
        {
            State.Window.Width = ModuleDescriptor.Capabilities.Capabilities[wp].Width;
            State.Window.Height = ModuleDescriptor.Capabilities.Capabilities[wp].Height;
        }
        else
        {
            if (!State.Settings.IsWindowModeActive)
            {
                RECT rect;
                GetClientRect(State.Window.HWND, &rect);

                State.Window.Width = rect.right - rect.left;
                State.Window.Height = rect.bottom - rect.top;
            }
            else
            {
                HDC hdc = GetDC(hwnd);

                State.Window.Width = GetDeviceCaps(hdc, HORZRES);
                State.Window.Height = GetDeviceCaps(hdc, VERTRES);

                ReleaseDC(hwnd, hdc);
            }

            if (State.Window.Width == 0 && State.Window.Height == 0)
            {
                State.Window.Width = ModuleDescriptor.Capabilities.Capabilities[wp].Width;
                State.Window.Height = ModuleDescriptor.Capabilities.Capabilities[wp].Height;
            }
        }

        const u32 bits = ModuleDescriptor.Capabilities.Capabilities[wp].Bits == (GRAPHICS_BITS_PER_PIXEL_16 - 1)
            ? GRAPHICS_BITS_PER_PIXEL_16 : ModuleDescriptor.Capabilities.Capabilities[wp].Bits;

        if (!State.Settings.IsWindowMode)
        {
            State.DX.Code = State.DX.Instance->SetDisplayMode(ModuleDescriptor.Capabilities.Capabilities[wp].Width,
                ModuleDescriptor.Capabilities.Capabilities[wp].Height, bits, 0, DDSDM_NONE);

            if (State.DX.Code == DD_OK)
            {
                DDSURFACEDESC2 desc;
                ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

                desc.dwSize = sizeof(DDSURFACEDESC2);

                if (lp - 1 == 0)
                {
                    State.DX.Active.Surfaces.Active.Back = NULL;
                    State.DX.Active.Surfaces.Back = NULL;
                    State.DX.Surfaces.Active[2] = NULL;
                }
                else
                {
                    desc.dwFlags = DDSD_BACKBUFFERCOUNT | DDSD_CAPS;
                    desc.dwBackBufferCount = lp - 1;
                }

                const u32 options = (lp - 1 == 0)
                    ? DDSCAPS_3DDEVICE | DDSCAPS_PRIMARYSURFACE
                    : DDSCAPS_3DDEVICE | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

                desc.ddsCaps.dwCaps = State.Device.Capabilities.IsAccelerated
                    ? options | DDSCAPS_VIDEOMEMORY
                    : options | DDSCAPS_SYSTEMMEMORY;

                State.DX.Code = State.DX.Instance->CreateSurface(&desc, &State.DX.Surfaces.Main, NULL);

                while (State.DX.Code == DDERR_OUTOFVIDEOMEMORY && 1 < desc.dwBackBufferCount)
                {
                    desc.dwBackBufferCount = desc.dwBackBufferCount - 1;

                    State.DX.Code = State.DX.Instance->CreateSurface(&desc, &State.DX.Surfaces.Main, NULL);
                }

                if (State.DX.Surfaces.Main != NULL)
                {
                    if (State.DX.Surfaces.Back == NULL && desc.dwBackBufferCount != 0)
                    {
                        DDSCAPS2 caps;
                        ZeroMemory(&caps, sizeof(DDSCAPS2));

                        caps.dwCaps = DDSCAPS_BACKBUFFER;

                        State.DX.Code = State.DX.Surfaces.Main->GetAttachedSurface(&caps, &State.DX.Surfaces.Back);
                    }
                }

                if (State.DX.Surfaces.Main != NULL)
                {
                    State.DX.Surfaces.Main->QueryInterface(IID_IDirectDrawSurface7, (void**)&State.DX.Surfaces.Active[1]); // TODO
                }

                if (State.DX.Surfaces.Back != NULL)
                {
                    State.DX.Surfaces.Back->QueryInterface(IID_IDirectDrawSurface7, (void**)&State.DX.Surfaces.Active[2]); // TODO

                    {
                        ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

                        desc.dwSize = sizeof(DDSURFACEDESC2);

                        State.DX.Code = State.DX.Surfaces.Active[2]->GetSurfaceDesc(&desc);
                        State.Device.Capabilities.IsVideoMemoryCapable = (desc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;
                    }
                }
            }
        }
        else
        {
            DDSURFACEDESC2 desc;
            ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

            desc.dwSize = sizeof(DDSURFACEDESC2);
            desc.dwFlags = DDSD_CAPS;
            desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

            State.DX.Code = State.DX.Instance->CreateSurface(&desc, &State.DX.Surfaces.Main, NULL);

            if (State.DX.Code == DD_OK)
            {
                if (State.Settings.IsWindowMode)
                {
                    State.DX.Code = State.DX.Instance->CreateClipper(0, &State.DX.Clipper, NULL);
                    State.DX.Code = State.DX.Surfaces.Main->SetClipper(State.DX.Clipper);
                    State.DX.Code = State.DX.Clipper->SetHWnd(0, State.Window.HWND);
                }

                if (State.DX.Surfaces.Main != NULL)
                {
                    State.DX.Surfaces.Main->QueryInterface(IID_IDirectDrawSurface7, (void**)&State.DX.Surfaces.Active[1]); // TODO
                }

                desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
                desc.dwHeight = State.Window.Height;
                desc.dwWidth = State.Window.Width;
                desc.ddsCaps.dwCaps = RendererDeviceType == RENDERER_MODULE_DEVICE_TYPE_1_ACCELERATED
                    ? DDSCAPS_VIDEOMEMORY | DDSCAPS_3DDEVICE | DDSCAPS_OFFSCREENPLAIN
                    : DDSCAPS_3DDEVICE | DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;

                State.DX.Code = State.DX.Instance->CreateSurface(&desc, &State.DX.Surfaces.Back, NULL);

                if (State.DX.Code == DD_OK)
                {
                    State.DX.Surfaces.Back->QueryInterface(IID_IDirectDrawSurface7, (void**)&State.DX.Surfaces.Active[2]); // TODO

                    {
                        ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

                        desc.dwSize = sizeof(DDSURFACEDESC2);

                        State.DX.Code = State.DX.Surfaces.Active[2]->GetSurfaceDesc(&desc);
                        State.Device.Capabilities.IsVideoMemoryCapable = (desc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;
                    }
                }
                else if (State.DX.Code == DDERR_OUTOFMEMORY || State.DX.Code == DDERR_OUTOFVIDEOMEMORY) { LOGWARNING("There was not enough video memory to create the rendering surface.\nTo run this program in a window of this size, please adjust your display settings for a smaller desktop area or a lower palette size and restart the program.\n"); }
                else { LOGWARNING("CreateSurface for window back buffer failed %s.\n", AcquireRendererMessage(State.DX.Code)); }
            }
            else if (State.DX.Code == DDERR_OUTOFMEMORY || State.DX.Code == DDERR_OUTOFVIDEOMEMORY) { LOGWARNING("There was not enough video memory to create the rendering surface.\nTo run this program in a window of this size, please adjust your display settings for a smaller desktop area or a lower palette size and restart the program.\n"); }
            else { LOGWARNING("CreateSurface for window front buffer failed %s.\n", AcquireRendererMessage(State.DX.Code)); }
        }

        InitializeRendererDeviceAcceleration();

        if (State.DX.GammaControl != NULL)
        {
            if (State.DX.GammaControl->GetGammaRamp(0, &State.Settings.GammaControl) != DD_OK)
            {
                State.DX.GammaControl->Release();
                State.DX.GammaControl = NULL;
            }
        }

        if (State.Lambdas.Lambdas.AcquireWindow != NULL)
        {
            SetEvent(State.Mutex);

            *result = State.DX.Code;
        }

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60009680
    // a.k.a. createD3D
    u32 InitializeRendererDeviceAcceleration(void)
    {
        State.DX.Active.Instance = State.DX.Instance;

        State.DX.Active.Surfaces.Main = State.DX.Surfaces.Main;
        State.DX.Active.Surfaces.Back = State.DX.Surfaces.Back;

        State.DX.Active.Surfaces.Active.Main = State.DX.Surfaces.Active[1]; // TODO
        State.DX.Active.Surfaces.Active.Back = State.DX.Surfaces.Active[2]; // TODO

        State.DX.Surfaces.Main->QueryInterface(IID_IDirectDrawGammaControl, (void**)&State.DX.GammaControl);

        {
            const HRESULT result = State.DX.Instance->QueryInterface(IID_IDirect3D7, (void**)&State.DX.DirectX);

            if (result != DD_OK)
            {
                LOGERROR("Creation of IDirect3D7 failed.\nCheck DX7 installed.\n (%s)\n", AcquireRendererMessage(result));
            }
        }

        InitializeConcreteRendererDevice();

        D3DDEVICEDESC7 caps;
        ZeroMemory(&caps, sizeof(D3DDEVICEDESC7));

        if (State.DX.Device->GetCaps(&caps) != DD_OK) { LOGERROR("GetCaps of IDirect3DDevice7 Failed\n"); } // ORIGINAL: IDirect3D3

        State.Device.Capabilities.IsAccelerated = ((caps.dwDevCaps & D3DDEVCAPS_HWRASTERIZATION) != 0);

        if (caps.dwDevCaps & D3DDEVCAPS_SEPARATETEXTUREMEMORIES)
        {
            ModuleDescriptor.MemorySize = ModuleDescriptor.MemorySize / caps.wMaxSimultaneousTextures;
        }

        State.Device.Capabilities.IsTransformLightBufferSystemMemoryAvailable = (caps.dwDevCaps & D3DDEVCAPS_TLVERTEXSYSTEMMEMORY) != 0;
        State.Device.Capabilities.IsTransformLightBufferVideoMemoryAvailable = (caps.dwDevCaps & D3DDEVCAPS_TLVERTEXVIDEOMEMORY) != 0;
        State.Device.Capabilities.MaxActiveLights = caps.dwMaxActiveLights;
        State.Device.Capabilities.MaxVertexBlendMatrices = caps.wMaxVertexBlendMatrices;
        State.Device.Capabilities.MaxUserClipPlanes = caps.wMaxUserClipPlanes;

        State.Device.Capabilities.MaxTextureRepeat = (f32)caps.dwMaxTextureRepeat;
        if (isnan(State.Device.Capabilities.MaxTextureRepeat) != (State.Device.Capabilities.MaxTextureRepeat == 0.0f))
        {
            State.Device.Capabilities.MaxTextureRepeat = 65535.0f;
        }

        State.Device.Capabilities.MaxTextureWidth = caps.dwMaxTextureWidth;
        State.Device.Capabilities.MinTextureWidth = caps.dwMinTextureWidth;

        State.Device.Capabilities.MaxTextureHeight = caps.dwMaxTextureHeight;
        State.Device.Capabilities.MinTextureHeight = caps.dwMinTextureHeight;

        State.Device.Capabilities.IsSquareOnlyTextures = (caps.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY) != 0;

        State.Device.Capabilities.MultipleTextureWidth = 1;
        State.Device.Capabilities.MultipleTextureHeight = 1;

        State.Device.Capabilities.IsPowerOfTwoTexturesHeight = (caps.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2) != 0;
        State.Device.Capabilities.IsPowerOfTwoTexturesWidth = (caps.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2) != 0;

        State.Device.Capabilities.IsTextureIndependentUVs = (caps.dpcTriCaps.dwTextureAddressCaps & D3DPTADDRESSCAPS_INDEPENDENTUV) != 0;

        State.Device.Capabilities.Capabilities.Unk01 = (caps.dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_NEVER) != 0;
        State.Device.Capabilities.Capabilities.Unk02 = (caps.dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_LESS) != 0;
        State.Device.Capabilities.Capabilities.Unk03 = (caps.dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_EQUAL) != 0;
        State.Device.Capabilities.Capabilities.Unk04 = (caps.dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_LESSEQUAL) != 0;
        State.Device.Capabilities.Capabilities.Unk05 = (caps.dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_GREATER) != 0;
        State.Device.Capabilities.Capabilities.Unk06 = (caps.dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_NOTEQUAL) != 0;
        State.Device.Capabilities.Capabilities.Unk07 = (caps.dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_GREATEREQUAL) != 0;
        State.Device.Capabilities.Capabilities.Unk08 = (caps.dpcTriCaps.dwZCmpCaps & D3DPCMPCAPS_ALWAYS) != 0;

        State.Device.Capabilities.Capabilities.Unk09 = (caps.dpcTriCaps.dwAlphaCmpCaps & D3DPCMPCAPS_NEVER) != 0;
        State.Device.Capabilities.Capabilities.Unk10 = (caps.dpcTriCaps.dwAlphaCmpCaps & D3DPCMPCAPS_LESS) != 0;
        State.Device.Capabilities.Capabilities.Unk11 = (caps.dpcTriCaps.dwAlphaCmpCaps & D3DPCMPCAPS_EQUAL) != 0;
        State.Device.Capabilities.Capabilities.Unk12 = (caps.dpcTriCaps.dwAlphaCmpCaps & D3DPCMPCAPS_LESSEQUAL) != 0;
        State.Device.Capabilities.Capabilities.Unk13 = (caps.dpcTriCaps.dwAlphaCmpCaps & D3DPCMPCAPS_GREATER) != 0;
        State.Device.Capabilities.Capabilities.Unk14 = (caps.dpcTriCaps.dwAlphaCmpCaps & D3DPCMPCAPS_NOTEQUAL) != 0;
        State.Device.Capabilities.Capabilities.Unk15 = (caps.dpcTriCaps.dwAlphaCmpCaps & D3DPCMPCAPS_GREATEREQUAL) != 0;
        State.Device.Capabilities.Capabilities.Unk16 = (caps.dpcTriCaps.dwAlphaCmpCaps & D3DPCMPCAPS_ALWAYS) != 0;

        State.Device.Capabilities.IsAlphaComparisonAvailable =
            (caps.dpcTriCaps.dwAlphaCmpCaps == D3DPCMPCAPS_ALWAYS || caps.dpcTriCaps.dwAlphaCmpCaps == D3DPCMPCAPS_NEVER) ? FALSE : TRUE;

        DDPIXELFORMAT format;
        ZeroMemory(&format, sizeof(DDPIXELFORMAT));

        format.dwSize = sizeof(DDPIXELFORMAT);

        State.DX.Active.Surfaces.Active.Main->GetPixelFormat(&format);

        State.Device.Capabilities.IsGreenAllowSixBits = format.dwGBitMask == 0x7e0;
        State.Device.Capabilities.RendererBits = caps.dwDeviceRenderBitDepth;
        State.Device.Capabilities.RendererDepthBits = caps.dwDeviceZBufferBitDepth;

        State.Device.Capabilities.AntiAliasing = (caps.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT) != 0;

        if (caps.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT)
        {
            State.Device.Capabilities.AntiAliasing = State.Device.Capabilities.AntiAliasing | RENDERER_MODULE_ANTIALIASING_SORT_INDEPENDENT;
        }

        State.Device.Capabilities.IsAntiAliasEdges = (caps.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANTIALIASEDGES) != 0;

        State.Device.Capabilities.IsAnisotropyAvailable = (caps.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANISOTROPY) != 0;

        if (State.Device.Capabilities.IsAnisotropyAvailable)
        {
            State.Device.Capabilities.MaxAnisotropy = caps.dwMaxAnisotropy;
        }

        State.Device.Capabilities.IsMipMapBiasAvailable = (caps.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS) != 0;
        State.Device.Capabilities.IsWBufferAvailable = (caps.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_WBUFFER) != 0;
        State.Device.Capabilities.IsWFogAvailable = (caps.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_WFOG) != 0;
        State.Device.Capabilities.IsDitherAvailable = (caps.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_DITHER) != 0;
        State.Device.Capabilities.IsAlphaTextures = (caps.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_ALPHA) != 0;
        State.Device.Capabilities.IsPerspectiveTextures = (caps.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_PERSPECTIVE) != 0;

        {
            const u32 phong = caps.dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_ALPHAPHONGBLEND;
            const u32 gouraud = caps.dpcTriCaps.dwShadeCaps & (D3DPSHADECAPS_ALPHAGOURAUDBLEND | D3DPSHADECAPS_ALPHAFLATBLEND);

            State.Device.Capabilities.IsAlphaFlatBlending = (phong || gouraud) ? TRUE : FALSE;
        }

        State.Device.Capabilities.IsSpecularGouraudBlending = (caps.dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_SPECULARGOURAUDRGB) != 0;

        {
            const u32 phong = caps.dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_ALPHAPHONGBLEND;
            const u32 gouraud = caps.dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND;

            State.Device.Capabilities.IsAlphaProperBlending = (phong || gouraud) ? TRUE : FALSE;
        }

        State.Device.Capabilities.IsModulateBlending = (caps.dpcTriCaps.dwTextureBlendCaps & D3DPTBLENDCAPS_MODULATEALPHA) != 0;

        if ((caps.dpcTriCaps.dwSrcBlendCaps & D3DPBLENDCAPS_SRCALPHA) == 0)
        {
            State.Device.Capabilities.IsSourceAlphaBlending = TRUE;

            if ((caps.dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_ONE) == 0)
            {
                State.Device.Capabilities.IsSourceAlphaBlending = FALSE;
            }
        }

        State.Device.Capabilities.IsColorBlending = (caps.dpcTriCaps.dwShadeCaps & (D3DPSHADECAPS_COLORPHONGRGB | D3DPSHADECAPS_COLORGOURAUDRGB)) != 0;
        State.Device.Capabilities.IsSpecularBlending = (caps.dpcTriCaps.dwShadeCaps & (D3DPSHADECAPS_SPECULARPHONGRGB | D3DPSHADECAPS_SPECULARGOURAUDRGB)) != 0;

        if (!State.Device.Capabilities.IsColorBlending) { State.Device.Capabilities.IsSourceAlphaBlending = FALSE; }

        if (isnan(caps.dvGuardBandLeft) == (caps.dvGuardBandLeft == 0.0))
        {
            State.Device.Capabilities.GuardBandLeft = caps.dvGuardBandLeft;
            State.Device.Capabilities.GuardBandRight = caps.dvGuardBandRight;
            State.Device.Capabilities.GuardBandTop = caps.dvGuardBandTop;
            State.Device.Capabilities.GuardBandBottom = caps.dvGuardBandBottom;
        }
        else
        {
            State.Device.Capabilities.GuardBandBottom = 0.0f;
            State.Device.Capabilities.GuardBandTop = 0.0f;
            State.Device.Capabilities.GuardBandRight = 0.0f;
            State.Device.Capabilities.GuardBandLeft = 0.0f;
        }

        if (State.Window.Bits == GRAPHICS_BITS_PER_PIXEL_32
            && (caps.dwDeviceZBufferBitDepth & (DEPTH_BIT_MASK_24_BIT | DEPTH_BIT_MASK_32_BIT)) == 0)
        {
            State.Window.Bits = GRAPHICS_BITS_PER_PIXEL_16;
        }

        for (u32 x = 0; x < ModuleDescriptor.Capabilities.Count; x++)
        {
            BOOL found = FALSE;
            u32 value = 0;

            switch (ModuleDescriptor.Capabilities.Capabilities[x].Bits)
            {
            case GRAPHICS_BITS_PER_PIXEL_8: { value = caps.dwDeviceRenderBitDepth & DEPTH_BIT_MASK_8_BIT; found = TRUE; break; }
            case GRAPHICS_BITS_PER_PIXEL_16: { value = caps.dwDeviceRenderBitDepth & DEPTH_BIT_MASK_16_BIT; found = TRUE; break; }
            case GRAPHICS_BITS_PER_PIXEL_24: { value = caps.dwDeviceRenderBitDepth & DEPTH_BIT_MASK_24_BIT; found = TRUE; break; }
            case GRAPHICS_BITS_PER_PIXEL_32: { value = caps.dwDeviceRenderBitDepth & DEPTH_BIT_MASK_32_BIT; found = TRUE; break; }
            }

            if (found && value == 0)
            {
                ModuleDescriptor.Capabilities.Capabilities[x].Width = 0;
                ModuleDescriptor.Capabilities.Capabilities[x].Height = 0;
                ModuleDescriptor.Capabilities.Capabilities[x].Bits = 0;
                ModuleDescriptor.Capabilities.Capabilities[x].Format = RENDERER_PIXEL_FORMAT_NONE;
                ModuleDescriptor.Capabilities.Capabilities[x].IsActive = FALSE;
                ModuleDescriptor.Capabilities.Capabilities[x].Unk03 = 0;
                ModuleDescriptor.Capabilities.Capabilities[x].Unk04 = 0;
            }
        }

        State.Device.Capabilities.MaximumSimultaneousTextures = caps.wMaxSimultaneousTextures;

        DAT_6005ab50 = 0; // TODO
        DAT_6005ab5c = 1; // TODO

        ModuleDescriptor.SubType = 1; // TODO

        {
            DDCAPS hal;
            ZeroMemory(&hal, sizeof(DDCAPS));

            hal.dwSize = sizeof(DDCAPS);

            DDCAPS hel;
            ZeroMemory(&hel, sizeof(DDCAPS));

            hel.dwSize = sizeof(DDCAPS);

            const HRESULT result = State.DX.Active.Instance->GetCaps(&hal, &hel);

            if (result != DD_OK) { LOGERROR("GetCaps of IDirectDraw7 Failed %s\n", AcquireRendererMessage(result)); }

            State.Device.Capabilities.IsWindowMode = (hal.dwCaps2 & DDCAPS2_CANRENDERWINDOWED) != 0;

            State.Device.Capabilities.IsNoVerticalSyncAvailable = (hal.dwCaps2 & DDCAPS2_FLIPNOVSYNC) != 0;

            State.Device.Capabilities.IsGammaAvailable = (hal.dwCaps2 & DDCAPS2_PRIMARYGAMMA) != 0;
        }

        State.Device.Capabilities.IsTrilinearInterpolationAvailable = AcquireRendererDeviceTrilinearInterpolationCapabilities();

        State.Device.Capabilities.IsDepthBufferRemovalAvailable = AcquireRendererDeviceDepthBufferRemovalCapabilities();

        if ((State.Device.Capabilities.RendererDeviceDepthBits & DEPTH_BIT_MASK_32_BIT) == 0)
        {
            if ((State.Device.Capabilities.RendererDepthBits & DEPTH_BIT_MASK_24_BIT) == 0)
            {
                if ((State.Device.Capabilities.RendererDepthBits & DEPTH_BIT_MASK_16_BIT) == 0)
                {
                    State.Device.Capabilities.RendererDepthBits = (State.Device.Capabilities.RendererDepthBits >> 8) & 8;
                }
                else
                {
                    State.Device.Capabilities.RendererDepthBits = GRAPHICS_BITS_PER_PIXEL_16;
                }
            }
            else
            {
                State.Device.Capabilities.RendererDepthBits = GRAPHICS_BITS_PER_PIXEL_24;
            }
        }
        else
        {
            State.Device.Capabilities.RendererDepthBits = GRAPHICS_BITS_PER_PIXEL_32;
        }

        State.Device.Capabilities.IsDepthAvailable = FALSE;

        if (State.Window.Bits != 0 && InitializeRendererDeviceDepthSurfaces(State.Window.Width, State.Window.Height, NULL, NULL))
        {
            State.Device.Capabilities.IsDepthAvailable = TRUE;
        }

        State.DX.Device->Release();

        InitializeConcreteRendererDevice();

        AcquireRendererDeviceTextureFormats();

        D3DVIEWPORT7 vp;
        ZeroMemory(&vp, sizeof(D3DVIEWPORT7));

        vp.dwHeight = State.Window.Height;
        vp.dwWidth = State.Window.Width;

        vp.dvMinZ = 0.0f;
        vp.dvMaxZ = 0.9999847f;

        State.DX.Device->SetViewport(&vp);

        D3DMATERIAL7 material;
        ZeroMemory(&material, sizeof(D3DMATERIAL7));

        State.DX.Device->SetMaterial(&material);

        InitializeRendererState();

        if (State.Device.Capabilities.MaximumSimultaneousTextures != 0)
        {
            const BOOL op1 = (caps.dwTextureOpCaps & (D3DTEXOPCAPS_MODULATE2X | D3DTEXOPCAPS_SELECTARG2 | D3DTEXOPCAPS_SELECTARG1)) != 0;
            const BOOL op2 = (caps.dwTextureOpCaps & (D3DTEXOPCAPS_MODULATE2X | D3DTEXOPCAPS_SELECTARG2 | D3DTEXOPCAPS_SELECTARG1 | D3DTEXOPCAPS_DISABLE)) != 0;

            for (u32 x = 0; x < State.Device.Capabilities.MaximumSimultaneousTextures; x++)
            {
                State.Textures.Stages[x].Unk11 = op1; // TODO
                State.Textures.Stages[x].Unk12 = op2; // TODO
            }
        }

        InitializeViewPort();

        State.DX.Active.IsInit = TRUE;

        return TRUE;
    }

    // 0x60009f50
    void InitializeConcreteRendererDevice(void)
    {
        HRESULT result = DD_OK;

        State.DX.Active.IsActive = FALSE;

        {
            const char* value = getenv(RENDERER_MODULE_DEVICE_TYPE_ENVIRONMENT_PROPERTY_NAME);

            if (value != NULL) { RendererDeviceType = atoi(value); }
        }

        if (SettingsState.Accelerate)
        {
            const HRESULT res = State.DX.DirectX->CreateDevice(State.Settings.Acceleration == RENDERER_MODULE_ACCELERATION_NORMAL ? IID_IDirect3DHALDevice : IID_IDirect3DTnLHalDevice,
                State.DX.Active.Surfaces.Back == NULL ? State.DX.Active.Surfaces.Main : State.DX.Active.Surfaces.Back, &State.DX.Device);

            if (res == DD_OK) { State.DX.Active.IsActive = TRUE; return; }
        }

        switch (RendererDeviceType)
        {
        case RENDERER_MODULE_DEVICE_TYPE_1_RGB:
        {
            State.DX.Active.IsSoft = TRUE;

            result = State.DX.DirectX->CreateDevice(IID_IDirect3DRGBDevice, State.DX.Active.Surfaces.Back, &State.DX.Device);

            break;
        }
        case RENDERER_MODULE_DEVICE_TYPE_1_REF:
        {
            State.DX.Active.IsSoft = TRUE;

            result = State.DX.DirectX->CreateDevice(IID_IDirect3DRefDevice, State.DX.Active.Surfaces.Back, &State.DX.Device);

            break;
        }
        case RENDERER_MODULE_DEVICE_TYPE_1_RAMP:
        {
            State.DX.Active.IsSoft = TRUE;

            result = State.DX.DirectX->CreateDevice(IID_IDirect3DRampDevice, State.DX.Active.Surfaces.Back, &State.DX.Device);

            break;
        }
        case RENDERER_MODULE_DEVICE_TYPE_1_MMX:
        {
            State.DX.Active.IsSoft = TRUE;

            result = State.DX.DirectX->CreateDevice(IID_IDirect3DMMXDevice, State.DX.Active.Surfaces.Back, &State.DX.Device);

            break;
        }
        case RENDERER_MODULE_DEVICE_TYPE_1_INVALID:
        case RENDERER_MODULE_DEVICE_TYPE_1_ACCELERATED:
        {
            State.DX.Active.IsSoft = FALSE;

            result = State.DX.DirectX->CreateDevice(State.Settings.Acceleration == RENDERER_MODULE_ACCELERATION_NORMAL ? IID_IDirect3DHALDevice : IID_IDirect3DTnLHalDevice,
                State.DX.Active.Surfaces.Back == NULL ? State.DX.Active.Surfaces.Main : State.DX.Active.Surfaces.Back, &State.DX.Device);

            break;
        }
        default: { LOGERROR("D3D device type not recognised\n"); return; }
        }

        if (result == DD_OK) { State.DX.Active.IsActive = TRUE; }
        else { LOGERROR(AcquireRendererMessage(result)); }
    }

    // 0x6000aa60
    BOOL AcquireRendererDeviceTrilinearInterpolationCapabilities(void)
    {
        D3DDEVICEDESC7 caps;
        ZeroMemory(&caps, sizeof(D3DDEVICEDESC7));

        State.DX.Device->GetCaps(&caps);

        return (caps.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEARMIPLINEAR) != 0;
    }

    // 0x6000aab0
    BOOL AcquireRendererDeviceDepthBufferRemovalCapabilities(void)
    {
        D3DDEVICEDESC7 caps;
        ZeroMemory(&caps, sizeof(D3DDEVICEDESC7));

        State.DX.Device->GetCaps(&caps);

        return (caps.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ZBUFFERLESSHSR) != 0;
    }

    // 0x6000a490
    void InitializeRendererState(void)
    {
        State.DX.Device->BeginScene();

        SelectRendererTransforms(1.0, 65535.0);

        State.DX.Device->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATER);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_ALPHAREF, 0);

        if (State.Device.Capabilities.IsDepthAvailable)
        {
            State.DX.Device->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
            State.DX.Device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
            State.DX.Device->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
        }
        else
        {
            State.DX.Device->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
            State.DX.Device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);
        }

        State.DX.Device->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE);

        State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_MAGFILTER, D3DTFN_LINEAR);
        State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_MINFILTER, D3DTFN_LINEAR);

        if (!State.Device.Capabilities.IsModulateBlending)
        {
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        }
        else
        {
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        }

        State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);

        State.DX.Device->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_CLIPPING, 0);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_LIGHTING, 0);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_EXTENTS, 0);

        {
            const char* value = getenv(RENDERER_MODULE_WIRE_FRAME_DX7_ENVIRONMENT_PROPERTY_NAME);

            State.DX.Device->SetRenderState(D3DRENDERSTATE_FILLMODE,
                (value == NULL || atoi(value) == 0) ? D3DFILL_SOLID : D3DFILL_WIREFRAME);
        }

        State.DX.Device->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, FALSE);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_NONE);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_FOGCOLOR, 0xff0000);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_NONE);

        {
            const f32 value = 0.0f;
            State.DX.Device->SetRenderState(D3DRENDERSTATE_FOGSTART, *(DWORD*)(&value));
        }

        {
            const f32 value = 1.0f; // ORIGINAL: 1.4013e-45f
            State.DX.Device->SetRenderState(D3DRENDERSTATE_FOGEND, *(DWORD*)(&value));
        }

        State.DX.Device->SetRenderState(D3DRENDERSTATE_TEXTUREFACTOR, GRAPCHICS_COLOR_WHITE);

        ZeroMemory(State.Textures.Stages, MAX_TEXTURE_STAGE_COUNT * sizeof(TextureStage));

        State.Scene.IsActive = TRUE;

        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_FILTER_STATE, (void*)RENDERER_MODULE_TEXTURE_FILTER_LENEAR);
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_MIP_FILTER_STATE, (void*)RENDERER_MODULE_TEXTURE_MIP_FILTER_LINEAR);

        for (u32 x = 0; x < State.Device.Capabilities.MaximumSimultaneousTextures; x++)
        {
            const u32 stage = MAKETEXTURESTAGEMASK(x); // NOTE: Originally a switch statement.

            const u32 message = stage | RENDERER_MODULE_STATE_SELECT_TEXTURE_STAGE_BLEND_STATE;

            {
                SelectState(message, (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_TEXTURE_ALPHA);

                DWORD value = 0;
                State.Textures.Stages[x].Unk09 = State.DX.Device->ValidateDevice(&value) == DD_OK;
            }

            {
                SelectState(message, (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_ADD);

                DWORD value = 0;
                State.Textures.Stages[x].Unk01 = State.DX.Device->ValidateDevice(&value) == DD_OK;
            }

            {
                SelectState(message, (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_ADD_SIGNED);

                DWORD value = 0;
                State.Textures.Stages[x].Unk08 = State.DX.Device->ValidateDevice(&value) == DD_OK;
            }

            {
                SelectState(message, (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_MODULATE);

                DWORD value = 0;
                State.Textures.Stages[x].Unk03 = State.DX.Device->ValidateDevice(&value) == DD_OK;
            }

            {
                SelectState(message, (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_SUBTRACT);

                DWORD value = 0;
                State.Textures.Stages[x].Unk02 = State.DX.Device->ValidateDevice(&value) == DD_OK;
            }

            {
                SelectState(message, (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_MODULATE_2X);

                DWORD value = 0;
                State.Textures.Stages[x].Unk04 = State.DX.Device->ValidateDevice(&value) == DD_OK;
            }

            {
                SelectState(message, (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_MODULATE_4X);

                DWORD value = 0;
                State.Textures.Stages[x].Unk05 = State.DX.Device->ValidateDevice(&value) == DD_OK;
            }

            {
                SelectState(message, (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_ADD_SIGNED);

                DWORD value = 0;
                State.Textures.Stages[x].Unk08 = State.DX.Device->ValidateDevice(&value) == DD_OK;
            }

            {
                SelectState(message, (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_DOT_PRODUCT_3);

                DWORD value = 0;
                State.Textures.Stages[x].Unk10 = State.DX.Device->ValidateDevice(&value) == DD_OK;
            }

            SelectState(message, (stage == RENDERER_TEXTURE_STAGE_0 || stage == RENDERER_TEXTURE_STAGE_1)
                ? (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_NORMAL : (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_DISABLE);
        }

        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_MIP_FILTER_STATE, (void**)RENDERER_MODULE_TEXTURE_MIP_FILTER_POINT);

        State.DX.Device->EndScene();

        State.Scene.IsActive = FALSE;
    }

    // 0x60001460
    void InitializeViewPort(void)
    {
        State.ViewPort.X0 = 0;
        State.ViewPort.Y0 = 0;
        State.ViewPort.X1 = 0;
        State.ViewPort.Y1 = 0;
    }
    
    // 0x60006f10
    void AttemptRenderScene(void)
    {
        if (State.Data.Vertexes.Count != 0) { RendererRenderScene(); }
    }

    // 0x60006f20
    void RendererRenderScene(void)
    {
        if (State.Data.Vertexes.Count != 0 && State.Data.Indexes.Count != 0)
        {
            if (!State.Scene.IsActive) { BeginRendererScene(); }

            State.DX.Device->DrawIndexedPrimitive(RendererPrimitiveType, RendererVertexType,
                State.Data.Vertexes.Vertexes, State.Data.Vertexes.Count,
                State.Data.Indexes.Indexes, State.Data.Indexes.Count, 0);

            State.Data.Vertexes.Count = 0;
            State.Data.Indexes.Count = 0;
        }
    }

    // 0x60008810
    BOOL BeginRendererScene(void)
    {
        if (State.Lock.IsActive) { UnlockGameWindow(NULL); }

        State.Scene.IsActive = State.DX.Device->BeginScene() == DD_OK;

        return State.Scene.IsActive;
    }

    // 0x60008860
    BOOL EndRendererScene(void)
    {
        if (State.Data.Vertexes.Count != 0) { RendererRenderScene(); }

        if (State.Scene.IsActive)
        {
            RendererDepthBias = 0.0f;

            const HRESULT result = State.DX.Device->EndScene();

            State.Scene.IsActive = FALSE;

            return result == DD_OK;
        }

        return TRUE;
    }

    // 0x6000ab00
    u32 SelectRendererTransforms(const f32 zNear, const f32 zFar)
    {
        if ((zFar <= zNear) != (zFar == zNear)) { return TRUE; }

        D3DMATRIX world;

        world._11 = 1.0f;
        world._12 = 0.0f;
        world._13 = 0.0f;
        world._14 = 0.0f;

        world._21 = 0.0f;
        world._22 = 1.0f;
        world._23 = 0.0f;
        world._24 = 0.0f;

        world._31 = 0.0f;
        world._32 = 0.0f;
        world._33 = 1.0f;
        world._34 = 0.0f;

        world._41 = 0.0f;
        world._42 = 0.0f;
        world._43 = 0.0f;
        world._44 = 1.0f;

        D3DMATRIX view;

        view._11 = 1.0f;
        view._12 = 0.0f;
        view._13 = 0.0f;
        view._14 = 0.0f;

        view._21 = 0.0f;
        view._22 = 1.0f;
        view._23 = 0.0f;
        view._24 = 0.0f;

        view._31 = 0.0f;
        view._32 = 0.0f;
        view._33 = 1.0f;
        view._34 = 0.0f;

        view._41 = 0.0f;
        view._42 = 0.0f;
        view._43 = 0.0f;
        view._44 = 1.0f;

        D3DMATRIX projection;

        projection._11 = 1.0f;
        projection._12 = 0.0f;
        projection._13 = 0.0f;
        projection._14 = 0.0f;

        projection._21 = 0.0f;
        projection._22 = 1.0f;
        projection._23 = 0.0f;
        projection._24 = 0.0f;

        projection._31 = 0.0f;
        projection._32 = 0.0f;
        projection._33 = 1.0f;
        projection._34 = 0.0f;

        projection._41 = 0.0f;
        projection._42 = 0.0f;
        projection._43 = 0.0f;
        projection._44 = 1.0f;

        HRESULT result = State.DX.Device->SetTransform(D3DTRANSFORMSTATE_WORLD, &world);

        if (result == DD_OK)
        {
            result = State.DX.Device->SetTransform(D3DTRANSFORMSTATE_VIEW, &view);

            if (result == DD_OK)
            {
                projection._44 = zNear;
                projection._34 = 1.0f;
                projection._33 = zNear / (zFar - zNear) + 1.0f;
                projection._43 = 0.0f;

                result = State.DX.Device->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &projection);
            }
        }

        return result;
    }

    // 0x6000c110
    void AcquireRendererDeviceTextureFormats(void)
    {
        State.Textures.Formats.Count = 0;

        s32 count = INVALID_TEXTURE_FORMAT_COUNT;
        State.DX.Device->EnumTextureFormats(EnumerateRendererDeviceTextureFormats, &count);

        State.Textures.Formats.Indexes[0] = INVALID_TEXTURE_FORMAT_INDEX;
        State.Textures.Formats.Indexes[1] = AcquireRendererDeviceTextureFormatIndex(4, 0, 0, 0, 0, 0, 0);
        State.Textures.Formats.Indexes[2] = AcquireRendererDeviceTextureFormatIndex(8, 0, 0, 0, 0, 0, 0);
        State.Textures.Formats.Indexes[3] = AcquireRendererDeviceTextureFormatIndex(0, 1, 5, 5, 5, 0, 0);
        State.Textures.Formats.Indexes[4] = AcquireRendererDeviceTextureFormatIndex(0, 0, 5, 6, 5, 0, 0);
        State.Textures.Formats.Indexes[5] = INVALID_TEXTURE_FORMAT_INDEX;
        State.Textures.Formats.Indexes[6] = AcquireRendererDeviceTextureFormatIndex(0, 8, 8, 8, 8, 0, 0);
        State.Textures.Formats.Indexes[7] = AcquireRendererDeviceTextureFormatIndex(0, 4, 4, 4, 4, 0, 0);
        State.Textures.Formats.Indexes[12] = AcquireRendererDeviceTextureFormatIndex(0, 0, 0, 0, 0, 1, 0);
        State.Textures.Formats.Indexes[13] = AcquireRendererDeviceTextureFormatIndex(0, 0, 0, 0, 0, 3, 0);
        State.Textures.Formats.Indexes[14] = INVALID_TEXTURE_FORMAT_INDEX;
        State.Textures.Formats.Indexes[15] = INVALID_TEXTURE_FORMAT_INDEX;
        State.Textures.Formats.Indexes[16] = INVALID_TEXTURE_FORMAT_INDEX;
        State.Textures.Formats.Indexes[17] = INVALID_TEXTURE_FORMAT_INDEX;
        State.Textures.Formats.Indexes[18] = AcquireRendererDeviceTextureFormatIndex(0, 0, 8, 8, 0, 0, 1);
        State.Textures.Formats.Indexes[19] = AcquireRendererDeviceTextureFormatIndex(0, 0, 8, 8, 8, 0, 2);
        State.Textures.Formats.Indexes[20] = AcquireRendererDeviceTextureFormatIndex(0, 0, 5, 5, 6, 0, 3);

        RendererTextureFormatStates[0] = 0; // TODO
        RendererTextureFormatStates[1] = State.Textures.Formats.Indexes[1] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[2] = State.Textures.Formats.Indexes[2] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[4] = State.Textures.Formats.Indexes[4] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[5] = State.Textures.Formats.Indexes[5] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[6] = State.Textures.Formats.Indexes[6] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[7] = State.Textures.Formats.Indexes[7] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[12] = State.Textures.Formats.Indexes[12] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[13] = State.Textures.Formats.Indexes[13] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[14] = State.Textures.Formats.Indexes[14] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[15] = State.Textures.Formats.Indexes[15] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[16] = State.Textures.Formats.Indexes[16] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[17] = State.Textures.Formats.Indexes[17] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[18] = State.Textures.Formats.Indexes[18] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[19] = State.Textures.Formats.Indexes[19] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[20] = State.Textures.Formats.Indexes[20] != INVALID_TEXTURE_FORMAT_INDEX;

        if (State.Textures.Formats.Indexes[3] == INVALID_TEXTURE_FORMAT_INDEX)
        {
            State.Textures.Formats.Indexes[3] = AcquireRendererDeviceTextureFormatIndex(0, 0, 5, 5, 5, 0, 0);
            RendererTextureFormatStates[3] = (State.Textures.Formats.Indexes[3] != INVALID_TEXTURE_FORMAT_INDEX) ? 5 : 0; // TODO
        }
        else
        {
            RendererTextureFormatStates[3] = 1;
        }
    }

    // 0x6000c450
    HRESULT CALLBACK EnumerateRendererDeviceTextureFormats(LPDDPIXELFORMAT format, LPVOID context)
    {
        ZeroMemory(&State.Textures.Formats.Formats[State.Textures.Formats.Count], sizeof(TextureFormat));

        CopyMemory(&State.Textures.Formats.Formats[State.Textures.Formats.Count].Descriptor.ddpfPixelFormat, format, sizeof(DDPIXELFORMAT));

        State.Textures.Formats.Formats[State.Textures.Formats.Count].DXTF = DXT_FORMAT_NONE;

        if (format->dwFlags & DDPF_FOURCC)
        {
            if (format->dwFourCC == FOURCC_DXT1)
            {
                State.Textures.Formats.Formats[State.Textures.Formats.Count].DXTF = DXT_FORMAT_DXT1;
                State.Textures.Formats.Formats[State.Textures.Formats.Count].DXTN = FOURCC_DXT1;
            }
            else if (format->dwFourCC == FOURCC_DXT2)
            {
                State.Textures.Formats.Formats[State.Textures.Formats.Count].DXTF = DXT_FORMAT_DXT2;
                State.Textures.Formats.Formats[State.Textures.Formats.Count].DXTN = FOURCC_DXT2;
            }
            else if (format->dwFourCC == FOURCC_DXT3)
            {
                State.Textures.Formats.Formats[State.Textures.Formats.Count].DXTF = DXT_FORMAT_DXT3;
                State.Textures.Formats.Formats[State.Textures.Formats.Count].DXTN = FOURCC_DXT3;
            }
        }

        if (format->dwFlags & DDPF_PALETTEINDEXED8)
        {
            State.Textures.Formats.Formats[State.Textures.Formats.Count].IsPalette = TRUE;
            State.Textures.Formats.Formats[State.Textures.Formats.Count].PaletteColorBits = 8;
            State.Textures.Formats.Formats[State.Textures.Formats.Count].DXTT = 0; // TODO
        }
        else
        {
            if (format->dwFlags & DDPF_PALETTEINDEXED4)
            {
                State.Textures.Formats.Formats[State.Textures.Formats.Count].IsPalette = TRUE;
                State.Textures.Formats.Formats[State.Textures.Formats.Count].PaletteColorBits = 4;
                State.Textures.Formats.Formats[State.Textures.Formats.Count].DXTT = 0; // TODO
            }
            else
            {
                if (format->dwFlags & DDPF_RGB)
                {
                    State.Textures.Formats.Formats[State.Textures.Formats.Count].IsPalette = FALSE;
                    State.Textures.Formats.Formats[State.Textures.Formats.Count].PaletteColorBits = 0;

                    u32 red = 0;

                    {
                        u32 value = format->dwRBitMask;

                        while ((value & 1) == 0) { value = value >> 1; }

                        while ((value & 1) != 0) { red = red + 1; value = value >> 1; }
                    }

                    u32 green = 0;

                    {
                        u32 value = format->dwGBitMask;

                        while ((value & 1) == 0) { value = value >> 1; }

                        while ((value & 1) != 0) { green = green + 1; value = value >> 1; }
                    }

                    u32 blue = 0;

                    {
                        u32 value = format->dwBBitMask;

                        while ((value & 1) == 0) { value = value >> 1; }

                        while ((value & 1) != 0) { blue = blue + 1; value = value >> 1; }
                    }

                    State.Textures.Formats.Formats[State.Textures.Formats.Count].RedBitCount = red;
                    State.Textures.Formats.Formats[State.Textures.Formats.Count].GreenBitCount = green;
                    State.Textures.Formats.Formats[State.Textures.Formats.Count].BlueBitCount = blue;

                    if (format->dwFlags & DDPF_ALPHAPIXELS)
                    {
                        u32 alpha = 0;

                        {
                            u32 value = format->dwRGBAlphaBitMask;

                            while ((value & 1) == 0) { value = value >> 1; }

                            while ((value & 1) != 0) { alpha = alpha + 1; value = value >> 1; }
                        }

                        State.Textures.Formats.Formats[State.Textures.Formats.Count].AlphaBitCount = alpha;
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].PaletteColorBits = 0;
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].DXTT = 0; // TODO
                    }
                    else
                    {
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].AlphaBitCount = 0;
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].PaletteColorBits = 0;
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].DXTT = 0; // TODO
                    }
                }
                else
                {
                    if (format->dwFlags & DDPF_BUMPLUMINANCE)
                    {
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].IsPalette = FALSE;
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].PaletteColorBits = 0;

                        u32 red = 0;

                        {
                            u32 value = format->dwRBitMask;

                            while ((value & 1) == 0) { value = value >> 1; }

                            while ((value & 1) != 0) { red = red + 1; value = value >> 1; }
                        }

                        u32 green = 0;

                        {
                            u32 value = format->dwGBitMask;

                            while ((value & 1) == 0) { value = value >> 1; }

                            while ((value & 1) != 0) { green = green + 1; value = value >> 1; }
                        }

                        u32 blue = 0;

                        {
                            u32 value = format->dwBBitMask;

                            while ((value & 1) == 0) { value = value >> 1; }

                            while ((value & 1) != 0) { blue = blue + 1; value = value >> 1; }
                        }

                        State.Textures.Formats.Formats[State.Textures.Formats.Count].RedBitCount = red;
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].GreenBitCount = green;
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].BlueBitCount = blue;

                        State.Textures.Formats.Formats[State.Textures.Formats.Count].DXTT = (red == 5 && green == 5 && blue == 6) ? 3 : 2; // TODO
                    }
                    else if (format->dwFlags & DDPF_BUMPDUDV)
                    {
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].IsPalette = FALSE;
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].PaletteColorBits = 0;

                        u32 red = 0;

                        {
                            u32 value = format->dwRBitMask;

                            while ((value & 1) == 0) { value = value >> 1; }

                            while ((value & 1) != 0) { red = red + 1; value = value >> 1; }
                        }

                        u32 green = 0;

                        {
                            u32 value = format->dwGBitMask;

                            while ((value & 1) == 0) { value = value >> 1; }

                            while ((value & 1) != 0) { green = green + 1; value = value >> 1; }
                        }

                        State.Textures.Formats.Formats[State.Textures.Formats.Count].RedBitCount = red;
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].GreenBitCount = green;
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].BlueBitCount = 0;
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].DXTT = 1; // TODO
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].Descriptor.ddpfPixelFormat.dwFlags = DDPF_BUMPDUDV;
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].Descriptor.ddpfPixelFormat.dwRGBBitCount = GRAPHICS_BITS_PER_PIXEL_16;
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].Descriptor.ddpfPixelFormat.dwRBitMask = 0xff;
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].Descriptor.ddpfPixelFormat.dwGBitMask = 0xff00;
                        State.Textures.Formats.Formats[State.Textures.Formats.Count].Descriptor.ddpfPixelFormat.dwBBitMask = 0;
                    }
                }
            }
        }

        if (*(s32*)context == INVALID_TEXTURE_FORMAT_COUNT)
        {
            *(s32*)context = State.Textures.Formats.Count;
        }

        // NOTE: Added check to avoid writing outside the array boundaries.
        if (MAX_TEXTURE_FORMAT_COUNT <= State.Textures.Formats.Count + 1) { return DDENUMRET_CANCEL; }

        State.Textures.Formats.Count = State.Textures.Formats.Count + 1;

        return DDENUMRET_OK;
    }

    // 0x6000c3c0
    s32 AcquireRendererDeviceTextureFormatIndex(const u32 palette, const u32 alpha, const u32 red, const u32 green, const u32 blue, const u32 dxtf, const u32 dxtt)
    {
        for (u32 x = 0; x < State.Textures.Formats.Count; x++)
        {
            const TextureFormat* format = &State.Textures.Formats.Formats[x];

            if (format->RedBitCount == red && format->GreenBitCount == green && format->BlueBitCount == blue)
            {
                if (format->PaletteColorBits == palette)
                {
                    if (format->AlphaBitCount == alpha && format->DXTF == dxtf && format->DXTT == dxtt) { return x; }
                    else if (format->PaletteColorBits == palette && palette != 0) { return x; }
                }
            }
            else if (format->PaletteColorBits == palette && palette != 0) { return x; }
        }

        return INVALID_TEXTURE_FORMAT_INDEX;
    }

    // 0x6000a130
    // a.k.a. createzbuffer
    BOOL InitializeRendererDeviceDepthSurfaces(const u32 width, const u32 height, IDirectDrawSurface7* depth, IDirectDrawSurface7* surf)
    {
        if (State.Device.Capabilities.IsDepthBufferRemovalAvailable) { return TRUE; }

        IDirectDrawSurface7* ds = surf == NULL ? State.DX.Surfaces.Depth : depth;

        DDPIXELFORMAT format;
        ZeroMemory(&format, sizeof(DDPIXELFORMAT));

        {
            const HRESULT result = State.DX.DirectX->EnumZBufferFormats(IID_IDirect3DHALDevice, EnumerateRendererDevicePixelFormats, &format);

            if (result != DD_OK) { LOGERROR("EnumZBufferFormats failure! %8x\n", result); }
        }

        if (format.dwSize != sizeof(DDPIXELFORMAT)) { return FALSE; }

        DDSURFACEDESC2 desc;
        ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

        desc.dwSize = sizeof(DDSURFACEDESC2);
        desc.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
        desc.dwHeight = height;
        desc.dwWidth = width;

        CopyMemory(&desc.ddpfPixelFormat, &format, sizeof(DDPIXELFORMAT));

        State.Device.Capabilities.IsStencilBufferAvailable = (format.dwFlags & DDPF_STENCILBUFFER) != 0;

        desc.ddsCaps.dwCaps = State.Device.Capabilities.IsAccelerated
            ? DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY
            : DDSCAPS_ZBUFFER | DDSCAPS_SYSTEMMEMORY;

        {
            const HRESULT result = State.DX.Active.Instance->CreateSurface(&desc, &ds, NULL);

            if (result != DD_OK)
            {
                LOGERROR("CreateSurface failure! %s\n", AcquireRendererMessage(result));

                if (result != DDERR_OUTOFMEMORY && result != DDERR_OUTOFVIDEOMEMORY)
                {
                    LOGWARNING("CreateSurface for Z-buffer failed %s.\n", AcquireRendererMessage(result));

                    return FALSE;
                }

                LOGWARNING("There was not enough video memory to create the Z-buffer surface.\nPlease restart the program and try another fullscreen mode with less resolution or lower bit depth.\n");

                return FALSE;
            }
        }

        {
            IDirectDrawSurface7* s = surf != NULL ? surf
                : (State.DX.Active.Surfaces.Back == NULL ? State.DX.Active.Surfaces.Main : State.DX.Active.Surfaces.Back);

            HRESULT result = s->AddAttachedSurface(ds);

            if (result == DD_OK)
            {
                DDSURFACEDESC2 desc;
                ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

                desc.dwSize = sizeof(DDSURFACEDESC2);

                result = ds->GetSurfaceDesc(&desc);

                if (result == DD_OK)
                {
                    State.Device.Capabilities.IsDepthVideoMemoryCapable = (desc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) != 0;

                    if (!State.Device.Capabilities.IsAccelerated || State.Device.Capabilities.IsDepthVideoMemoryCapable)
                    {
                        if (ds != NULL) { ds->Release(); }

                        return TRUE;
                    }

                    LOGWARNING("Could not fit the Z-buffer in video memory for this hardware device.\n");
                }
                else { LOGWARNING("Failed to get surface description of Z buffer %d.\n", result); }
            }
            else { LOGWARNING("AddAttachedBuffer failed for Z-Buffer %d.\n", result); }
        }

        State.DX.Device->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);

        if (ds != NULL) { ds->Release(); }

        return FALSE;
    }

    // 0x6000a450
    HRESULT CALLBACK EnumerateRendererDevicePixelFormats(LPDDPIXELFORMAT format, LPVOID context)
    {
        if ((format->dwFlags & DDPF_ZBUFFER) && format->dwRGBBitCount == State.Window.Bits)
        {
            CopyMemory(context, format, sizeof(DDPIXELFORMAT));

            return D3DENUMRET_CANCEL;
        }

        return D3DENUMRET_OK;
    }
    
    // 0x60002060
    void SelectRendererDevice(void)
    {
        if (RendererDeviceIndex < DEFAULT_DEVICE_INDEX)
        {
            State.Device.Capabilities.MinTextureWidth = 1;

            if (State.Lambdas.Lambdas.AcquireWindow != NULL || State.Window.HWND != NULL)
            {
                const char* value = getenv(RENDERER_MODULE_DISPLAY_ENVIRONMENT_PROPERTY_NAME);

                SelectDevice(value == NULL ? DEFAULT_DEVICE_INDEX : atoi(value));
            }
        }
    }

    // 0x600092c0
    void AcquireRendererModuleDescriptor(RendererModuleDescriptor* desc, const char* section)
    {
        desc->Signature = AcquireSettingsValue(desc->Signature, section, "signature");
        desc->Version = AcquireSettingsValue(desc->Version, section, "version");

        {
            const u32 caps = desc->Caps;

            const u32 lw = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_LINE_WIDTH) >> 0, section, "linewidth");
            const u32 ts = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_TEXTURE_SQUARE) >> 1, section, "texturesquare");
            const u32 twp2 = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_TEXTURE_WIDTH_POW2) >> 2, section, "texturewidthpowerof2");
            const u32 thp2 = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_TEXTURE_HEIGHT_POW2) >> 3, section, "textureheightpowerof2");
            const u32 soft = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_SOFTWARE) >> 4, section, "software");
            const u32 win = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_WINDOWED) >> 5, section, "windowed");
            const u32 gc = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_GLOBAL_CUT) >> 6, section, "globalclut");
            const u32 tn2p = AcquireSettingsValue((caps & RENDERER_MODULE_CAPS_TRILINEAR_PASS) >> 7, section, "trilinear2pass");

            desc->Caps = ((lw & 1) << 0) | ((ts & 1) << 1) | ((twp2 & 1) << 2) | ((thp2 & 1) << 3)
                | ((soft & 1) << 4) | ((win & 1) << 5) | ((gc & 1) << 6) | ((tn2p & 1) << 7);
        }

        desc->MinimumTextureWidth = AcquireSettingsValue(desc->MinimumTextureWidth, section, "texturewidthmin");
        desc->MaximumTextureWidth = AcquireSettingsValue(desc->MaximumTextureWidth, section, "texturewidthmax");
        desc->MultipleTextureWidth = AcquireSettingsValue(desc->MultipleTextureWidth, section, "texturewidthmultiple");
        desc->MinimumTextureHeight = AcquireSettingsValue(desc->MinimumTextureHeight, section, "textureheightmin");
        desc->MaximumTextureHeight = AcquireSettingsValue(desc->MaximumTextureHeight, section, "textureheightmax");
        desc->MultipleTextureHeight = AcquireSettingsValue(desc->MultipleTextureHeight, section, "textureheightmultiple");
        desc->ClipAlign = AcquireSettingsValue(desc->ClipAlign, section, "clipalign");
        desc->MaximumSimultaneousTextures = AcquireSettingsValue(desc->MaximumSimultaneousTextures, section, "numstages");
        desc->SubType = AcquireSettingsValue(desc->SubType, section, "subtype");
        desc->MemorySize = AcquireSettingsValue(desc->MemorySize, section, "textureramsize");
        desc->MemoryType = AcquireSettingsValue(desc->MemoryType, section, "textureramtype");
        desc->DXV = AcquireSettingsValue(desc->DXV, section, "dxversion");
    }

    // 0x60008dc0
    u32 ClearRendererViewPort(const u32 x0, const u32 y0, const u32 x1, const u32 y1, const BOOL window)
    {
        AttemptRenderScene();

        D3DRECT rect;

        DWORD options = (window == FALSE); // D3DCLEAR_TARGET
        
        if (State.Device.Capabilities.IsDepthAvailable && State.Window.Bits != 0) { options = options | D3DCLEAR_ZBUFFER; }

        if (State.Device.Capabilities.IsStencilBufferAvailable) { options = options | D3DCLEAR_STENCIL; }

        if (x1 == 0)
        {
            rect.x1 = 0;
            rect.y1 = 0;
            rect.x2 = State.Window.Width;
            rect.y2 = State.Window.Height;
        }
        else
        {
            rect.x1 = x0;
            rect.x2 = x1;
            rect.y1 = y0;
            rect.y2 = y1;
        }

        return State.DX.Device->Clear(1, &rect, options, RendererClearColor, RendererClearDepth, 0) == DD_OK;
    }

    // 0x60008a50
    // a.k.a. showbackbuffer
    u32 ToggleRenderer(void)
    {
        if (State.Lock.IsActive) { UnlockGameWindow(NULL); }

        if (!State.Settings.IsWindowMode)
        {
            if (State.DX.Active.Surfaces.Back != NULL)
            {
                HRESULT result = State.DX.Active.Surfaces.Main->Flip(NULL, RendererToggleOptions);

                if (result == DDERR_SURFACELOST) { return RENDERER_MODULE_FAILURE; }

                const u32 state = AcquireState(RENDERER_MODULE_STATE_SELECT_DISPLAY_STATE);

                if (!State.Settings.IsToggleAllowed) { return RENDERER_MODULE_FAILURE; }

                while (TRUE)
                {
                    while (result = State.DX.Active.Instance->TestCooperativeLevel(), result == DD_OK)
                    {
                        ResetTextures();

                        if (result = State.DX.Active.Instance->RestoreAllSurfaces(), result == DD_OK) { return RENDERER_MODULE_SUCCESS; }

                        Sleep(1);
                    }

                    if (result == DDERR_WRONGMODE) { break; }

                    if (result == DDERR_EXCLUSIVEMODEALREADYSET || result == DDERR_NOEXCLUSIVEMODE) { Sleep(1); }

                    Sleep(1);
                }

                SelectVideoMode(state, 2, 1); // TODO

                return RENDERER_MODULE_SUCCESS;
            }
        }
        else
        {
            for (u32 x = 0; x < 1000000; x++)
            {
                const HRESULT result = State.DX.Active.Surfaces.Main->GetBltStatus(DDGBS_ISBLTDONE);

                if (result != DDERR_WASSTILLDRAWING && result != DDERR_SURFACEBUSY) { break; }
            }

            RECT dst;
            RECT src;

            if (!State.Settings.IsWindowMode)
            {
                GetClientRect(State.Window.HWND, &dst);

                POINT point;
                ZeroMemory(&point, sizeof(POINT));

                ClientToScreen(State.Window.HWND, &point);
                OffsetRect(&dst, point.x, point.y);
                SetRect(&src, 0, 0, State.Window.Width, State.Window.Height);
            }
            else
            {
                POINT point;
                ZeroMemory(&point, sizeof(POINT));

                ClientToScreen(State.Window.HWND, &point);
                SetRect(&dst, point.x, point.y, State.Window.Width + point.x, State.Window.Height + point.y);
                SetRect(&src, 0, 0, State.Window.Width, State.Window.Height);
            }

            const HRESULT result = State.DX.Active.Surfaces.Active.Main->Blt(&dst,
                State.DX.Active.Surfaces.Active.Back, &src, RendererBlitOptions, NULL);

            if (result != DD_OK) { return RENDERER_MODULE_FAILURE; }
        }

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60008a10
    BOOL SelectRendererState(const D3DRENDERSTATETYPE type, const DWORD value)
    {
        if (!State.Scene.IsActive) { BeginRendererScene(); }

        if (State.Data.Vertexes.Count != 0) { RendererRenderScene(); }

        return State.DX.Device->SetRenderState(type, value) == DD_OK;
    }

    // 0x600089d0
    BOOL SelectRendererTextureStage(const u32 stage, const D3DTEXTURESTAGESTATETYPE type, const DWORD value)
    {
        if (!State.Scene.IsActive) { BeginRendererScene(); }

        if (State.Data.Vertexes.Count != 0) { RendererRenderScene(); }

        return State.DX.Device->SetTextureStageState(stage, type, value) == DD_OK;
    }

    // 0x60008d00
    void SelectRendererMaterial(const u32 color)
    {
        D3DMATERIAL7 material;
        ZeroMemory(&material, sizeof(D3DMATERIAL7));

        RendererClearColor = color;

        const f32 red = RGBA_GETRED(color) / 255.0f;
        const f32 green = RGBA_GETGREEN(color) / 255.0f;
        const f32 blue = RGBA_GETBLUE(color) / 255.0f;

        material.diffuse.r = red;
        material.diffuse.g = green;
        material.diffuse.b = blue;

        material.ambient.r = red;
        material.ambient.g = green;
        material.ambient.b = blue;

        State.DX.Device->SetMaterial(&material);
    }

    // 0x60006e50
    void SelectRendererFogAlphas(const u8* input, u8* output)
    {
        if (input == NULL) { return; }

        for (u32 x = 0; x < MAX_OUTPUT_FOG_ALPHA_COUNT; x++)
        {
            const f32 value = roundf(x / 255.0f) * 63.0f;
            const u32 indx = (u32)value;

            const f32 diff = value - indx;

            if (0.0f < diff)
            {
                const u8 result = (u8)roundf(input[indx] + (input[indx + 1] - input[indx]) * diff);
                output[x] = (u8)(MAX_OUTPUT_FOG_ALPHA_VALUE - result);
            }
            else
            {
                output[x] = (u8)(MAX_OUTPUT_FOG_ALPHA_VALUE - input[indx]);
            }
        }
    }

    // 0x60008c80
    BOOL RestoreRendererSurfaces(void)
    {
        for (u32 x = 0; x < 5000; x++)
        {
            if (State.DX.Active.Instance->TestCooperativeLevel() == DD_OK)
            {
                return State.DX.Active.Instance->RestoreAllSurfaces() == DD_OK;
            }

            Sleep(100);
        }

        LOGERROR("WARNING! STATE_RESTORESURFACES failed. Focus/Mode not regained before timeout.\n");

        return FALSE;
    }

    // 0x60006fa0
    void SelectRendererVertexCount(void)
    {
        AttemptRenderScene();

        MaximumRendererVertexCount = MAX_VERTEX_COUNT / RendererVertexSize;
    }

    // 0x600094f0
    void SelectRendererDeviceType(const u32 type)
    {
        RendererDeviceType = type;
    }

    // 0x6000b2c0
    u32 SelectBasicRendererState(const u32 state, void* value)
    {
        switch (state)
        {
        case RENDERER_MODULE_STATE_SELECT_CULL_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_CULL_NONE:
            {
                State.Settings.Cull = 1; // TODO

                SelectRendererStateValue(state, (void*)RENDERER_MODULE_CULL_NONE);

                return RENDERER_MODULE_SUCCESS;
            }
            case RENDERER_MODULE_CULL_COUNTER_CLOCK_WISE:
            {
                State.Settings.Cull = 0x80000000; // TODO

                SelectRendererStateValue(state, (void*)RENDERER_MODULE_CULL_COUNTER_CLOCK_WISE);

                return RENDERER_MODULE_FAILURE;
            }
            case RENDERER_MODULE_CULL_CLOCK_WISE:
            {
                State.Settings.Cull = 0; // TODO

                SelectRendererStateValue(state, (void*)RENDERER_MODULE_CULL_CLOCK_WISE);

                return RENDERER_MODULE_FAILURE;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            break;
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_STATE:
        case RENDERER_MODULE_STATE_SELECT_EXECUTE_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_LOCK_WINDOW_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_DISPLAY_STATE:
        case RENDERER_MODULE_STATE_SELECT_GAME_WINDOW_INDEX:
        case RENDERER_MODULE_STATE_SELECT_SHAMELESS_PLUG_STATE:
        case RENDERER_MODULE_STATE_SELECT_LOG_STATE: { break; }
        case RENDERER_MODULE_STATE_SELECT_LAMBDAS:
        {
            const RendererModuleLambdaContainer* lambdas = (RendererModuleLambdaContainer*)value;

            State.Lambdas.Log = lambdas == NULL ? NULL : lambdas->Log;

            break;
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW: { State.Window.Parent.HWND = (HWND)value; break; }
        case RENDERER_MODULE_STATE_SELECT_LOG_LAMBDA: { State.Lambdas.Log = (RENDERERMODULELOGLAMBDA)value; break; }
        case RENDERER_MODULE_STATE_SELECT_LINE_VERTEX_SIZE: { RendererLineVertexSize = (u32)value; break; }
        case RENDERER_MODULE_STATE_SELECT_VERSION:
        {
            const u32 version = (u32)value;

            RendererVersion = version;

            if (version < RENDERER_MODULE_VERSION_104 || version < RENDERER_MODULE_VERSION_107)
            {
                RendererVersion = RENDERER_MODULE_VERSION_107;
            }

            break;
        }
        case RENDERER_MODULE_STATE_SELECT_MEMORY_ALLOCATE_LAMBDA: { State.Lambdas.AllocateMemory = (RENDERERMODULEALLOCATEMEMORYLAMBDA)value; break; }
        case RENDERER_MODULE_STATE_SELECT_MEMORY_RELEASE_LAMBDA: { State.Lambdas.ReleaseMemory = (RENDERERMODULERELEASEMEMORYLAMBDA)value; break; }
        case RENDERER_MODULE_STATE_SELECT_SELECT_STATE_LAMBDA: { State.Lambdas.SelectState = (RENDERERMODULESELECTSTATELAMBDA)value; break; }
        case RENDERER_MODULE_STATE_55: { DAT_60058df4 = (u32)value; break; }
        case RENDERER_MODULE_STATE_62: { DAT_60058df8 = (u32)value; break; }
        case RENDERER_MODULE_STATE_INDEX_SIZE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_INDEX_SIZE_1: { RendererIndexSize = RENDERER_MODULE_INDEX_SIZE_1; break; }
            case RENDERER_MODULE_INDEX_SIZE_2: { RendererIndexSize = RENDERER_MODULE_INDEX_SIZE_2; break; }
            case RENDERER_MODULE_INDEX_SIZE_4: { RendererIndexSize = RENDERER_MODULE_INDEX_SIZE_4; break; }
            case RENDERER_MODULE_INDEX_SIZE_8: { RendererIndexSize = RENDERER_MODULE_INDEX_SIZE_8; break; }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            break;
        }
        default: { return RENDERER_MODULE_FAILURE; }
        }

        SelectRendererStateValue(state, value);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60001e70
    void SelectRendererStateValue(const u32 state, void* value)
    {
        const u32 actual = state & RENDERER_MODULE_SELECT_STATE_MASK;
        const u32 stage = MAKETEXTURESTAGEVALUE(state);

        const s32 indx = AcquireTextureStateStageIndex(actual);

        if (indx >= 0) { State.Textures.StageStates[indx].Values[stage] = (s32)value; }

        if (State.Lambdas.SelectState != NULL) { State.Lambdas.SelectState(actual, value); }
    }

    // 0x60001e20
    s32 AcquireTextureStateStageIndex(const u32 state)
    {
        s32 sum = 0;
        u32 indx = 0;

        while (state < RendererModuleValues::MinMax[indx].Min || RendererModuleValues::MinMax[indx].Max < state)
        {
            sum = sum + (RendererModuleValues::MinMax[indx].Max - RendererModuleValues::MinMax[indx].Min);

            indx = indx + 1;

            if (5 < indx) { return -1; } // TODO
        }

        return (sum - RendererModuleValues::MinMax[indx].Min) + state;
    }

    // 0x60001f20
    void InitializeTextureStateStates(void)
    {
        ZeroMemory(State.Textures.StageStates, MAX_TEXTURE_STATE_STATE_COUNT * sizeof(TextureStageState));
    }

    // 0x6000b590
    void InitializeRendererModuleState(const u32 mode, const u32 pending, const u32 depth, const char* section)
    {
        SelectState(RENDERER_MODULE_STATE_SELECT_HINT_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_HINT_INACTIVE, section, "HINT"));
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE, NULL);
        SelectState(RENDERER_MODULE_STATE_SELECT_CULL_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_CULL_COUNTER_CLOCK_WISE, section, "CULL"));
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_FILTER_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_TEXTURE_FILTER_LENEAR, section, "FILTER"));
        SelectState(RENDERER_MODULE_STATE_SELECT_SHADE_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_SHADE_GOURAUD, section, "SHADE"));
        SelectState(RENDERER_MODULE_STATE_SELECT_ALPHA_BLEND_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_ALPHA_BLEND_ACTIVE, section, "TRANSPARENCY"));
        SelectState(RENDERER_MODULE_STATE_SELECT_ALPHA_TEST_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_ALPHA_TEST_16, section, "ALPHATEST"));
        SelectState(RENDERER_MODULE_STATE_SELCT_ALPHA_FUNCTION,
            (void*)AcquireSettingsValue(RENDERER_MODULE_ALPHA_FUNCTION_GREATER, section, "ALPHACMP"));
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_MIP_FILTER_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_TEXTURE_MIP_FILTER_POINT, section, "MIPMAP"));
        SelectState(RENDERER_MODULE_STATE_SELECT_MATERIAL,
            (void*)AcquireSettingsValue(0x00000000, section, "BACKGROUNDCOLOUR"));
        SelectState(RENDERER_MODULE_STATE_SELECT_CHROMATIC_COLOR,
            (void*)AcquireSettingsValue(0x00000000, section, "CHROMACOLOUR"));
        SelectState(RENDERER_MODULE_STATE_SELECT_DITHER_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_DITHER_INACTIVE, section, "DITHER")); // ORIGINAL: ACTIVE
        SelectState(RENDERER_MODULE_STATE_SELECT_FOG_ALPHAS, NULL);
        SelectState(RENDERER_MODULE_STATE_SELECT_FOG_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_FOG_INACTIVE, section, "FOGMODE"));

        {
            const f32 value = 0.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_FOG_DENSITY,
                (void*)AcquireSettingsValue((s32)(*(s32*)&value), section, "FOGDENSITY"));
        }

        {
            const f32 value = 0.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_FOG_START,
                (void*)AcquireSettingsValue((s32)(*(s32*)&value), section, "STATE_FOGZNEAR"));
        }

        {
            const f32 value = 1.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_FOG_END,
                (void*)AcquireSettingsValue((s32)(*(s32*)&value), section, "FOGZFAR"));
        }

        SelectState(RENDERER_MODULE_STATE_SELECT_FOG_COLOR,
            (void*)AcquireSettingsValue(GRAPCHICS_COLOR_WHITE, section, "FOGCOLOUR"));

        SelectState(RENDERER_MODULE_STATE_INDEX_SIZE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_INDEX_SIZE_4, section, "INDEXSIZE"));

        SelectState(RENDERER_MODULE_STATE_SELECT_BLEND_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_ALPHA_BLEND_SOURCE_INVERSE_SOURCE, section, "ALPHA"));
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_ADDRESS_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_TEXTURE_ADDRESS_CLAMP, section, "TEXTURECLAMP"));

        {
            const f32 value = 1.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_GAMMA_CONTROL_STATE, (void*)(u32)(*(u32*)&value));
        }

        {
            const f32 value = 0.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_DEPTH_BIAS_STATE,
                (void*)AcquireSettingsValue((u32)(*(u32*)&value), section, "DEPTHBIAS"));
        }

        {
            const f32 value = 0.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_MIP_MAP_LOD_BIAS_STATE, (void*)(u32)(*(u32*)&value));
        }

        SelectState(RENDERER_MODULE_STATE_SELECT_VERTEX_TYPE, (void*)RENDERER_MODULE_VERTEX_TYPE_RTLVX);
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_STAGE_BLEND_STATE, (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_NORMAL);
        SelectState(MAKETEXTURESTAGEMASK(RENDERER_TEXTURE_STAGE_1) | RENDERER_MODULE_STATE_SELECT_TEXTURE_STAGE_BLEND_STATE, (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_DISABLE);
        SelectState(RENDERER_MODULE_STATE_MAX_PENDING_STATE,
            (void*)AcquireSettingsValue(pending - 2U & ((s32)(pending - 2U) < 0) - 1, section, "MAXPENDING")); // TODO
        SelectState(RENDERER_MODULE_STATE_SELECT_BACK_BUFFER_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_BACK_BUFFER_ACTIVE, section, "BACKBUFFERTYPE"));
        SelectState(RENDERER_MODULE_STATE_SELECT_FLAT_FANS_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_STATE_FLAT_FANS_ACTIVE, section, "FLATFANS"));
        SelectState(RENDERER_MODULE_STATE_SELECT_LINE_WIDTH, (void*)AcquireSettingsValue(1, section, "LINEWIDTH"));
        SelectState(RENDERER_MODULE_STATE_SELECT_STENCIL_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_STENCIL_INACTIVE, section, "STENCILBUFFER"));
        SelectState(RENDERER_MODULE_STATE_SELECT_DISPLAY_STATE, (void*)AcquireSettingsValue(mode, section, "DISPLAYMODE"));
        SelectState(RENDERER_MODULE_STATE_SELECT_LINE_DOUBLE_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_LINE_DOUBLE_INACTIVE, section, "LINEDOUBLE"));
        SelectState(RENDERER_MODULE_STATE_SELECT_GAME_WINDOW_INDEX, (void*)0); // TODO

        {
            const f32 value = 1.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_CLEAR_DEPTH_STATE, (void*)(u32)(*(u32*)&value));
        }

        SelectState(RENDERER_MODULE_STATE_SELECT_TOGGLE_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_TOGGLE_VERTICAL_SYNC, section, "FLIPRATE"));
        SelectState(RENDERER_MODULE_STATE_SELECT_SHAMELESS_PLUG_STATE,
            (void*)AcquireSettingsValue(0, section, "SHAMELESSPLUG")); // TODO

        {
            const u32 value = AcquireSettingsValue(((depth < 1) - 1) & 2, section, "DEPTHBUFFER");
            const u32 result = SelectState(RENDERER_MODULE_STATE_SELECT_DEPTH_STATE, (void*)value);

            if (result == RENDERER_MODULE_FAILURE && value == RENDERER_MODULE_DEPTH_ACTIVE_W)
            {
                SelectState(RENDERER_MODULE_STATE_SELECT_DEPTH_STATE, (void*)RENDERER_MODULE_DEPTH_ACTIVE);
            }
        }

        SelectState(RENDERER_MODULE_STATE_SELCT_DEPTH_FUNCTION,
            (void*)AcquireSettingsValue(RENDERER_MODULE_DEPTH_FUNCTION_LESS_EQUAL, section, "DEPTHCMP"));
        SelectState(RENDERER_MODULE_STATE_SELECT_LOG_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_LOG_ACTIVE, section, "LOG"));
    }

    // 0x600088b0
    // NOTE: Looks like the engineers made an interesting decision to extend the API in an unconventional way, such that
    // a stage index is passed into the function, and not a valid pointer, to reset current texture.
    BOOL SelectRendererTexture(RendererTexture* tex)
    {
        if (!State.Scene.IsActive) { BeginRendererScene(); }

        if (State.Data.Vertexes.Count != 0) { RendererRenderScene(); }

        if ((u32)tex < 16) // TODO
        {
            if ((u32)tex == 0) // TODO
            {
                HRESULT result = DD_OK;

                for (u32 x = 0; x < State.Device.Capabilities.MaximumSimultaneousTextures; x++)
                {
                    result = State.DX.Device->SetTexture(x, NULL);
                }

                return result == DD_OK;
            }

            if ((u32)tex == 1) { return State.DX.Device->SetTexture(RENDERER_TEXTURE_STAGE_0, NULL) == DD_OK; } // TODO

            if ((u32)tex == 2) { return State.DX.Device->SetTexture(RENDERER_TEXTURE_STAGE_1, NULL) == DD_OK; } // TODO

            return TRUE;
        }

        return State.DX.Device->SetTexture(tex->Stage, tex->Texture) == DD_OK;
    }

    // 0x60003570
    u32 ReleaseRendererWindow(void)
    {
        if (State.DX.Instance != NULL)
        {
            SetForegroundWindow(State.Window.HWND);
            PostMessageA(State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_RELEASE_DEVICE, 0, 0);
            WaitForSingleObject(State.Mutex, 10000);
            CloseHandle(State.Mutex);

            State.Mutex = NULL;
            State.Window.HWND = NULL;

            return State.DX.Code;
        }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x600035e0
    u32 ReleaseRendererDeviceInstance(void)
    {
        ReleaseRendererDeviceSurfaces();

        if (State.DX.Instance != NULL)
        {
            State.DX.Instance->Release();
            State.DX.Instance = NULL;
        }

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60009080
    RendererTexture* AllocateRendererTexture(const u32 width, const u32 height, const u32 format, const u32 options, const u32 state, const BOOL destination)
    {
        if (State.DX.Active.Instance == NULL) { return NULL; }
        if (State.Textures.Illegal) { return NULL; }

        RendererTexture* tex = InitializeRendererTexture();

        tex->Width = width;
        tex->Height = height;
        tex->FormatIndex = State.Textures.Formats.Indexes[format];
        tex->UnknownFormatIndexValue = UnknownFormatValues[format];
        tex->Stage = MAKETEXTURESTAGEVALUE(state);
        tex->FormatIndexValue = format & 0xff;

        tex->MipMapCount = MAKETEXTUREMIPMAPVALUE(state) != 0 ? (MAKETEXTUREMIPMAPVALUE(state) + 1) : 0;

        tex->Is16Bit = (format == RENDERER_PIXEL_FORMAT_R5G5B5 || format == RENDERER_PIXEL_FORMAT_R4G4B4);

        tex->Options = options;
        tex->MemoryType = RENDERER_MODULE_TEXTURE_LOCATION_SYSTEM_MEMORY;

        tex->Surface = NULL;
        tex->Texture = NULL;
        tex->Palette = NULL;

        tex->Colors = 0;

        const s32 result = InitializeRendererTextureDetails(tex, destination);

        if (result < 1) // TODO
        {
            ReleaseRendererTexture(tex);

            if (result != -1) { State.Textures.Illegal = TRUE; } // TODO

            return NULL;
        }

        tex->Previous = State.Textures.Current;
        State.Textures.Current = tex;

        State.Textures.Count = State.Textures.Count + 1;

        return tex;
    }

    // 0x60009210
    RendererTexture* InitializeRendererTexture(void)
    {
        RendererTexture* result = (RendererTexture*)AllocateRendererTexture(sizeof(RendererTexture));

        if (result == NULL) { LOGERROR("D3D texture allocation ran out of memory\n"); }

        return result;
    }

    // 0x6000b270
    RendererTexture* AllocateRendererTexture(const u32 size)
    {
        if (State.Lambdas.AllocateMemory != NULL) { return (RendererTexture*)State.Lambdas.AllocateMemory(size); }

        return (RendererTexture*)malloc(size);
    }

    // 0x6000c790
    s32 InitializeRendererTextureDetails(RendererTexture* tex, const BOOL destination) // TODO returns -1, 0, 1, where 1 is success, -1 is total failure and no further allocations allowed
    {
        if (tex->Texture != NULL)
        {
            tex->Texture->Release();
            tex->Texture = NULL;
        }

        DDSURFACEDESC2 desc;
        CopyMemory(&desc, &State.Textures.Formats.Formats[tex->FormatIndex].Descriptor, sizeof(DDSURFACEDESC2));

        desc.dwSize = sizeof(DDSURFACEDESC2);

        // Attempt to create texture in RAM.
        {
            if (tex->MipMapCount == 0)
            {
                desc.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
                desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
            }
            else
            {
                desc.dwFlags = DDSD_MIPMAPCOUNT | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
                desc.ddsCaps.dwCaps = DDSCAPS_MIPMAP | DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY | DDSCAPS_COMPLEX;
                desc.dwMipMapCount = tex->MipMapCount;
            }

            if (RendererTextureHint == RENDERER_MODULE_TEXTURE_HINT_DYNAMIC) { desc.ddsCaps.dwCaps2 = DDSCAPS2_HINTDYNAMIC; }
            else if (RendererTextureHint == RENDERER_MODULE_TEXTURE_HINT_STATIC) { desc.ddsCaps.dwCaps2 = DDSCAPS2_HINTSTATIC; }

            desc.dwHeight = tex->Height;
            desc.dwWidth = tex->Width;

            if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1 || tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT3)
            {
                ZeroMemory(&desc.ddpfPixelFormat, sizeof(DDPIXELFORMAT));

                desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
                desc.ddpfPixelFormat.dwFlags = DDPF_FOURCC;

                if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1)
                {
                    desc.ddpfPixelFormat.dwFourCC = FOURCC_DXT1;
                }
                else if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT3)
                {
                    desc.ddpfPixelFormat.dwFourCC = FOURCC_DXT3;
                }
            }
            else if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_BUMPDUDV_1)
            {
                ZeroMemory(&desc.ddpfPixelFormat, sizeof(DDPIXELFORMAT));

                desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
                desc.ddpfPixelFormat.dwFlags = DDPF_BUMPDUDV;
                desc.ddpfPixelFormat.dwRGBBitCount = GRAPHICS_BITS_PER_PIXEL_16;
                desc.ddpfPixelFormat.dwRBitMask = 0xff;
                desc.ddpfPixelFormat.dwGBitMask = 0xff00;
                desc.ddpfPixelFormat.dwBBitMask = 0x00;
            }
            else if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_BUMPDUDV_2)
            {
                ZeroMemory(&desc.ddpfPixelFormat, sizeof(DDPIXELFORMAT));

                desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
                desc.ddpfPixelFormat.dwFlags = DDPF_BUMPDUDV;
                desc.ddpfPixelFormat.dwRGBBitCount = GRAPHICS_BITS_PER_PIXEL_24;
                desc.ddpfPixelFormat.dwRBitMask = 0xff;
                desc.ddpfPixelFormat.dwGBitMask = 0xff00;
                desc.ddpfPixelFormat.dwBBitMask = 0xff0000;
            }
            else if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_BUMPDUDV_3)
            {
                ZeroMemory(&desc.ddpfPixelFormat, sizeof(DDPIXELFORMAT));

                desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
                desc.ddpfPixelFormat.dwFlags = DDPF_BUMPDUDV;
                desc.ddpfPixelFormat.dwRGBBitCount = GRAPHICS_BITS_PER_PIXEL_16;
                desc.ddpfPixelFormat.dwRBitMask = 0x1f;
                desc.ddpfPixelFormat.dwGBitMask = 0x3e0;
                desc.ddpfPixelFormat.dwBBitMask = 0xfc00;
            }

            IDirectDrawSurface7* surface = NULL;

            {
                const HRESULT result = State.DX.Active.Instance->CreateSurface(&desc, &surface, NULL);

                if (result != DD_OK) { return (result != DDERR_INVALIDPIXELFORMAT) - 1; } // TODO
            }

            tex->Surface = surface;

            ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

            desc.dwSize = sizeof(DDSURFACEDESC2);

            if (surface->GetSurfaceDesc(&desc) != DD_OK)
            {
                surface->Release();

                return NULL;
            }
        }

        CopyMemory(&tex->Descriptor, &desc, sizeof(DDSURFACEDESC2));

        // Attempt to create texture in Video Memory or RAM, depending on the device capabilities.
        {
            if (tex->MipMapCount == 0)
            {
                desc.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
                desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
            }
            else
            {
                desc.dwFlags = DDSD_MIPMAPCOUNT | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
                desc.ddsCaps.dwCaps = DDSCAPS_MIPMAP | DDSCAPS_TEXTURE | DDSCAPS_COMPLEX;
                desc.dwMipMapCount = tex->MipMapCount;
            }

            if (RendererTextureHint == RENDERER_MODULE_TEXTURE_HINT_DYNAMIC) { desc.ddsCaps.dwCaps2 = DDSCAPS2_HINTDYNAMIC; }
            else if (RendererTextureHint == RENDERER_MODULE_TEXTURE_HINT_STATIC) { desc.ddsCaps.dwCaps2 = DDSCAPS2_HINTSTATIC; }

            if (destination) { desc.ddsCaps.dwCaps = desc.ddsCaps.dwCaps | DDSCAPS_3DDEVICE; }

            if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1 || tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT3)
            {
                ZeroMemory(&desc.ddpfPixelFormat, sizeof(DDPIXELFORMAT));

                desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
                desc.ddpfPixelFormat.dwFlags = DDPF_FOURCC;

                if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1)
                {
                    desc.ddpfPixelFormat.dwFourCC = FOURCC_DXT1;
                }
                else if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT3)
                {
                    desc.ddpfPixelFormat.dwFourCC = FOURCC_DXT3;
                }
            }
            else if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_BUMPDUDV_1)
            {
                ZeroMemory(&desc.ddpfPixelFormat, sizeof(DDPIXELFORMAT));

                desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
                desc.ddpfPixelFormat.dwFlags = DDPF_BUMPDUDV;
                desc.ddpfPixelFormat.dwRGBBitCount = GRAPHICS_BITS_PER_PIXEL_16;
                desc.ddpfPixelFormat.dwRBitMask = 0xff;
                desc.ddpfPixelFormat.dwGBitMask = 0xff00;
                desc.ddpfPixelFormat.dwBBitMask = 0x00;
            }
            else if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_BUMPDUDV_2)
            {
                ZeroMemory(&desc.ddpfPixelFormat, sizeof(DDPIXELFORMAT));

                desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
                desc.ddpfPixelFormat.dwFlags = DDPF_BUMPDUDV;
                desc.ddpfPixelFormat.dwRGBBitCount = GRAPHICS_BITS_PER_PIXEL_24;
                desc.ddpfPixelFormat.dwRBitMask = 0xff;
                desc.ddpfPixelFormat.dwGBitMask = 0xff00;
                desc.ddpfPixelFormat.dwBBitMask = 0xff0000;
            }
            else if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_BUMPDUDV_3)
            {
                ZeroMemory(&desc.ddpfPixelFormat, sizeof(DDPIXELFORMAT));

                desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
                desc.ddpfPixelFormat.dwFlags = DDPF_BUMPDUDV;
                desc.ddpfPixelFormat.dwRGBBitCount = GRAPHICS_BITS_PER_PIXEL_16;
                desc.ddpfPixelFormat.dwRBitMask = 0x1f;
                desc.ddpfPixelFormat.dwGBitMask = 0x3e0;
                desc.ddpfPixelFormat.dwBBitMask = 0xfc00;
            }

            desc.ddsCaps.dwCaps = State.Device.Capabilities.IsAccelerated
                ? desc.ddsCaps.dwCaps | DDSCAPS_VIDEOMEMORY
                : desc.ddsCaps.dwCaps | DDSCAPS_SYSTEMMEMORY;

            IDirectDrawSurface7* surface = NULL;

            {
                const HRESULT result = State.DX.Active.Instance->CreateSurface(&desc, &surface, NULL);

                if (result != DD_OK)
                {
                    if (tex->Surface != NULL) { tex->Surface->Release(); }

                    return (result != DDERR_INVALIDPIXELFORMAT) - 1; // TODO
                }
            }

            IDirectDrawPalette* palette = NULL;

            if (desc.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
            {
                tex->Colors = 256;

                PALETTEENTRY entries[MAX_TEXTURE_PALETTE_COLOR_COUNT];

                for (u32 x = 0; x < MAX_TEXTURE_PALETTE_COLOR_COUNT; x++)
                {
                    entries[x].peRed = 0;
                    entries[x].peGreen = 0xff;
                    entries[x].peBlue = 0;
                    entries[x].peFlags = 0;
                }

                if (State.DX.Active.Instance->CreatePalette(DDPCAPS_ALLOW256 | DDPCAPS_8BIT, entries, &palette, NULL) != DD_OK)
                {
                    if (surface != NULL) { surface->Release(); }

                    if (tex->Surface != NULL) { tex->Surface->Release(); }

                    return 0; // TODO
                }

                if (surface->SetPalette(palette) != DD_OK)
                {
                    if (surface != NULL) { surface->Release(); }

                    if (tex->Surface != NULL) { tex->Surface->Release(); }

                    if (palette != NULL) { palette->Release(); }

                    return 0; // TODO
                }

                tex->Palette = palette;
            }
            else if (desc.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
            {
                tex->Colors = 256;

                PALETTEENTRY entries[MAX_TEXTURE_PALETTE_COLOR_COUNT];

                for (u32 x = 0; x < MAX_TEXTURE_PALETTE_COLOR_COUNT; x++)
                {
                    entries[x].peRed = 0;
                    entries[x].peGreen = 0xff;
                    entries[x].peBlue = 0;
                    entries[x].peFlags = 0;
                }

                if (State.DX.Active.Instance->CreatePalette(DDPCAPS_4BIT, entries, &palette, NULL) != DD_OK)
                {
                    if (surface != NULL) { surface->Release(); }

                    if (tex->Surface != NULL) { tex->Surface->Release(); }

                    return 0; // TODO
                }

                if (surface->SetPalette(palette) != DD_OK)
                {
                    if (surface != NULL) { surface->Release(); }

                    if (tex->Surface != NULL) { tex->Surface->Release(); }

                    if (palette != NULL) { palette->Release(); }

                    return 0; // TODO
                }

                tex->Palette = palette;
            }
            else
            {
                tex->Colors = 0;
                tex->Palette = NULL;
            }

            if (surface->Blt(NULL, tex->Surface, NULL, DDBLT_WAIT, NULL) != DD_OK)
            {
                if (surface != NULL) { surface->Release(); }

                if (tex->Surface != NULL) { tex->Surface->Release(); }

                if (palette != NULL) { palette->Release(); }

                return 0; // TODO
            }

            ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

            desc.dwSize = sizeof(DDSURFACEDESC2);

            if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1 || tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT3) { desc.dwFlags = DDSD_LINEARSIZE; }

            if (surface->GetSurfaceDesc(&desc) != DD_OK)
            {
                if (surface != NULL) { surface->Release(); }

                if (tex->Surface != NULL) { tex->Surface->Release(); }

                if (palette != NULL) { palette->Release(); }
            }

            tex->MemoryType = desc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY
                ? (desc.ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM ? RENDERER_MODULE_TEXTURE_LOCATION_NON_LOCAL_VIDEO_MEMORY : RENDERER_MODULE_TEXTURE_LOCATION_LOCAL_VIDEO_MEMORY)
                : (desc.ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY ? RENDERER_MODULE_TEXTURE_LOCATION_NON_LOCAL_VIDEO_MEMORY : RENDERER_MODULE_TEXTURE_LOCATION_SYSTEM_MEMORY);

            tex->Texture = surface;

            return 1; // TODO
        }

        return 0; // TODO
    }

    // 0x60009070
    void ReleaseRendererTexture(RendererTexture* tex)
    {
        if (tex != NULL) { DisposeRendererTexture(tex); }
    }

    // 0x6000b290
    u32 DisposeRendererTexture(RendererTexture* tex)
    {
        if (State.Lambdas.ReleaseMemory != NULL) { return State.Lambdas.ReleaseMemory(tex); }

        free(tex);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60001650
    RendererModuleWindowLock* RendererLock(const u32 mode)
    {
        if (State.DX.Surfaces.Window != NULL)
        {
            if (State.Lock.IsActive) { LOGERROR("D3D lock called while locked\n"); }

            if (State.Scene.IsActive) { EndRendererScene(); }

            if (State.Lambdas.Lambdas.LockWindow != NULL) { State.Lambdas.Lambdas.LockWindow(TRUE); }

            DDSURFACEDESC2 desc;
            ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

            desc.dwSize = sizeof(DDSURFACEDESC2);

            u32 options = DDLOCK_SURFACEMEMORYPTR;

            switch (mode)
            {
            case LOCK_NONE: { options = DDLOCK_SURFACEMEMORYPTR; break; }
            case LOCK_READ: { options = DDLOCK_READONLY; break; }
            case LOCK_WRITE: { options = DDLOCK_WRITEONLY; break; }
            }

            State.Lock.Surface = State.DX.Surfaces.Window;

            State.DX.Code = State.DX.Surfaces.Window->Lock(NULL, &desc, options | DDLOCK_NOSYSLOCK | DDLOCK_WAIT, NULL);

            if (State.DX.Code != DD_OK)
            {
                if (State.Lambdas.Lambdas.LockWindow != NULL) { State.Lambdas.Lambdas.LockWindow(FALSE); }

                return NULL;
            }

            if (desc.ddpfPixelFormat.dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_16)
            {
                State.Lock.State.Format = (desc.ddpfPixelFormat.dwGBitMask == 0x7e0)
                    ? RENDERER_PIXEL_FORMAT_R5G6B5
                    : RENDERER_PIXEL_FORMAT_A1R5G5B5;
            }
            else if (desc.ddpfPixelFormat.dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_32)
            {
                State.Lock.State.Format = RENDERER_PIXEL_FORMAT_A8R8G8B8;
            }
            else if (desc.ddpfPixelFormat.dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_24)
            {
                State.Lock.State.Format = RENDERER_PIXEL_FORMAT_R8G8B8;
            }

            if (State.Settings.IsWindowMode)
            {
                RECT rect;
                GetClientRect(State.Window.HWND, &rect);

                POINT point;
                ZeroMemory(&point, sizeof(POINT));

                ClientToScreen(State.Window.HWND, &point);
                OffsetRect(&rect, point.x, point.y);

                if (State.DX.Surfaces.Window == State.DX.Surfaces.Active[1]) // TODO
                {
                    desc.lpSurface = (void*)((addr)desc.lpSurface
                        + (addr)((desc.ddpfPixelFormat.dwRGBBitCount >> 3) * rect.left)
                        + (addr)(desc.lPitch * rect.top));
                }
            }

            State.Lock.State.Data = desc.lpSurface;
            State.Lock.State.Stride = desc.lPitch;
            State.Lock.State.Width = State.Window.Width;
            State.Lock.State.Height = State.Window.Height;

            State.Lock.IsActive = TRUE;
        }

        return &State.Lock.State;
    }

    // 0x6000bab0
    void InitializeRenderState55(void)
    {
        SelectBasicRendererState(RENDERER_MODULE_STATE_55, (void*)(DAT_60058df4 + 1));
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE, NULL);
    }

    // 0x60009670
    BOOL AcquireRendererDeviceState(void)
    {
        return State.DX.Active.IsInit;
    }
    
    // 0x60009500
    void ReleaseRendererDevice(void)
    {
        if (AcquireRendererDeviceState() && State.Scene.IsActive)
        {
            FlushGameWindow();
            SyncGameWindow(0);
            Idle();

            State.Scene.IsActive = FALSE;
        }

        if (State.DX.Clipper != NULL)
        {
            State.DX.Clipper->Release();
            State.DX.Clipper = NULL;
        }

        if (State.DX.GammaControl != NULL)
        {
            {
                const f32 value = 1.0f;

                SelectState(RENDERER_MODULE_STATE_SELECT_GAMMA_CONTROL_STATE, (void*)(u32)(*(u32*)&value));
            }

            State.DX.GammaControl->Release();
            State.DX.GammaControl = NULL;
        }

        ResetTextures();

        State.DX.Active.Instance = NULL;

        State.DX.Active.Surfaces.Main = NULL;
        State.DX.Active.Surfaces.Back = NULL;
        State.DX.Active.Surfaces.Active.Main = NULL;
        State.DX.Active.Surfaces.Active.Back = NULL;

        State.DX.Surfaces.Window = NULL;

        State.Window.Index = 0;

        if (State.DX.Surfaces.Depth != NULL)
        {
            State.DX.Device->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE);
            State.DX.Device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);

            State.DX.Surfaces.Depth->Release();
            State.DX.Surfaces.Depth = NULL;
        }

        if (State.DX.Device != NULL)
        {
            State.DX.Device->Release();
            State.DX.Device = NULL;
        }

        if (State.DX.DirectX != NULL)
        {
            State.DX.DirectX->Release();
            State.DX.DirectX = NULL;
        }

        State.DX.Active.IsInit = FALSE;

        State.Scene.IsActive = FALSE;

        ReleaseRendererWindows();
    }

    // 0x60001980
    void ReleaseRendererWindows(void)
    {
        for (u32 x = 0; x < State.Window.Count + WINDOW_OFFSET; x++)
        {
            if (State.Windows[x + WINDOW_OFFSET].Surface != NULL)
            {
                State.Windows[x + WINDOW_OFFSET].Surface->Release();
                State.Windows[x + WINDOW_OFFSET].Surface = NULL;
            }
        }

        State.Window.Count = 0;
    }

    // 0x6000cf50
    BOOL UpdateRendererTexture(RendererTexture* tex, const u32* pixels, const u32* palette)
    {
        if (pixels == NULL && palette == NULL) { return FALSE; }

        if (pixels != NULL)
        {
            tex->Descriptor.dwFlags = DDSD_LPSURFACE;

            tex->Descriptor.lpSurface =
                (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1 || tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT3)
                ? (void*)((addr)pixels + (addr)8) : (void*)pixels;

            if (tex->Surface->SetSurfaceDesc(&tex->Descriptor, 0) != DD_OK) { return FALSE; }

            if (State.Scene.IsActive)
            {
                AttemptRenderScene();
                EndRendererScene();
            }

            if (tex->Texture->Blt(NULL, tex->Surface, NULL, DDBLT_WAIT, NULL) != DD_OK) { return FALSE; }
        }

        if (palette != NULL && tex->Options != 0)
        {
            PALETTEENTRY entries[MAX_TEXTURE_PALETTE_COLOR_COUNT];

            for (u32 x = 0; x < MAX_TEXTURE_PALETTE_COLOR_COUNT; x++)
            {
                entries[x].peRed = (u8)RGBA_GETRED(palette[x]);
                entries[x].peGreen = (u8)RGBA_GETGREEN(palette[x]);
                entries[x].peBlue = (u8)RGBA_GETBLUE(palette[x]);
                entries[x].peFlags = 0;
            }

            if (tex->Palette->SetEntries(0, 0, tex->Colors, entries) != DD_OK) { return FALSE; }

            if (tex->Texture->SetPalette(tex->Palette) != DD_OK) { return FALSE; }
        }

        if (tex->MipMapCount != 0) { UpdateRendererTexture(tex, pixels); }

        return TRUE;
    }

    // 0x6000d0c0
    BOOL UpdateRendererTexture(RendererTexture* tex, const u32* pixels)
    {
        if (tex->MipMapCount == 0 || tex->MipMapCount == 1) { return TRUE; }

        IDirectDrawSurface7* s1 = tex->Surface;
        IDirectDrawSurface7* s2 = tex->Texture;

        s1->AddRef();
        s2->AddRef();

        DDSCAPS2 caps;
        ZeroMemory(&caps, sizeof(DDSCAPS2));

        DDSURFACEDESC2 desc;
        ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

        desc.dwSize = sizeof(DDSURFACEDESC2);

        u32 offset = (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1
            || tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT3) ? 8 : 0; // TODO

        void* allocated = NULL;
        u32* data = NULL;

        for (u32 x = 0; x < tex->MipMapCount; x++)
        {
            if (pixels != NULL)
            {
                s1->GetSurfaceDesc(&desc);

                if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1
                    || tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT3)
                {
                    desc.lPitch = desc.lPitch / desc.dwHeight;
                }

                offset = offset + desc.dwHeight * desc.lPitch;

                caps.dwCaps = DDSCAPS_MIPMAP | DDSCAPS_TEXTURE;

                {
                    IDirectDrawSurface7* attached = NULL;
                    s1->GetAttachedSurface(&caps, &attached);
                    s1->Release();

                    s1 = attached;
                }

                {
                    IDirectDrawSurface7* attached = NULL;
                    s2->GetAttachedSurface(&caps, &attached);
                    s2->Release();

                    s2 = attached;
                }

                s1->GetSurfaceDesc(&tex->Descriptor);

                tex->Descriptor.dwFlags = DDSD_LPSURFACE;

                const u32 pitch = (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1
                    || tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT3)
                    ? tex->Descriptor.lPitch
                    : (tex->Descriptor.ddpfPixelFormat.dwRGBBitCount * ((tex->Descriptor.dwWidth + 7) >> 3));

                if (pitch == tex->Descriptor.lPitch)
                {
                    tex->Descriptor.lpSurface = (void*)((addr)pixels + (addr)offset);
                }
                else
                {
                    if (allocated == NULL)
                    {
                        allocated = _alloca((desc.lPitch * desc.dwHeight + 3) & 0xfffffffc);
                        memset(allocated, 0xff, (desc.lPitch * desc.dwHeight + 3) & 0xfffffffc);
                    }

                    if (data == NULL) { data = (u32*)((addr)pixels + (addr)offset); }

                    tex->Descriptor.lpSurface = allocated;

                    for (u32 xx = 0; xx < tex->Descriptor.dwHeight; xx++)
                    {
                        CopyMemory(allocated, &data[xx * pitch], pitch);
                    }
                }

                s1->SetSurfaceDesc(&tex->Descriptor, 0);
                s2->Blt(NULL, s1, NULL, DDBLT_WAIT, NULL);
            }

            s1->Release();
            s2->Release();
        }

        return TRUE;
    }

    // 0x6000cd50
    BOOL UpdateRendererTexture(RendererTexture* tex, const u32* pixels, const u32* palette, const u32 x, const u32 y, const u32 width, const u32 height, const u32 size)
    {
        if (pixels != NULL)
        {
            RECT source;

            source.left = x;
            source.right = x + width;
            source.top = y;
            source.bottom = y + height;

            RECT destination;

            destination.left = 0;
            destination.right = width;
            destination.top = 0;
            destination.bottom = height;

            tex->Descriptor.dwFlags = DDSD_LPSURFACE;

            tex->Descriptor.lpSurface =
                (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1 || tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT3)
                ? (void*)((addr)pixels + (addr)8) : (void*)pixels;

            if (tex->Descriptor.lPitch != size)
            {
                void* allocated = _alloca((tex->Descriptor.lPitch * height + 3) & 0xfffffffc);
                memset(allocated, 0xff, (tex->Descriptor.lPitch * height + 3) & 0xfffffffc);

                tex->Descriptor.lpSurface = (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1
                    || tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT3)
                    ? (void*)((addr)allocated + (addr)8) : allocated;

                for (u32 xx = 0; xx < height; xx++)
                {
                    CopyMemory(allocated, &pixels[xx * size], size);
                }
            }

            if (tex->Surface->SetSurfaceDesc(&tex->Descriptor, 0) != DD_OK) { return FALSE; }

            if (State.Scene.IsActive)
            {
                AttemptRenderScene();
                EndRendererScene();
            }

            if (tex->Texture->Blt(&source, tex->Surface, &destination, DDBLT_WAIT, NULL) != DD_OK) { return FALSE; }
        }

        if (palette != NULL && tex->Options != 0)
        {
            PALETTEENTRY entries[MAX_TEXTURE_PALETTE_COLOR_COUNT];

            for (u32 x = 0; x < MAX_TEXTURE_PALETTE_COLOR_COUNT; x++)
            {
                entries[x].peRed = (u8)RGBA_GETRED(palette[x]);
                entries[x].peGreen = (u8)RGBA_GETGREEN(palette[x]);
                entries[x].peBlue = (u8)RGBA_GETBLUE(palette[x]);
                entries[x].peFlags = 0;
            }

            if (tex->Palette->SetEntries(0, 0, tex->Colors, entries) != DD_OK) { return FALSE; }

            if (tex->Texture->SetPalette(tex->Palette) != DD_OK) { return FALSE; }
        }

        if (tex->MipMapCount != 0) { UpdateRendererTexture(tex, pixels); }

        return TRUE;
    }

    // 0x600082f0
    void RenderLine(RVX* a, RVX* b)
    {
        if (RendererPrimitiveType != D3DPT_LINELIST) { RendererRenderScene(); }

        RendererPrimitiveType = D3DPT_LINELIST;

        if (MaximumRendererVertexCount - 2 < State.Data.Vertexes.Count) { RendererRenderScene(); }

        // A
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 0)));

            CopyMemory(v, a, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_60018850 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        // B
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 1)));

            CopyMemory(v, b, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_60018850 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 0] = State.Data.Vertexes.Count + 0;
        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 1] = State.Data.Vertexes.Count + 1;

        State.Data.Indexes.Count = State.Data.Indexes.Count + 2;
        State.Data.Vertexes.Count = State.Data.Vertexes.Count + 2;
    }

    // 0x600084e0
    void RenderLineMesh(RVX* vertexes, const u32* indexes, const u32 count)
    {
        if (RendererPrimitiveType != D3DPT_LINELIST) { RendererRenderScene(); }

        RendererPrimitiveType = D3DPT_LINELIST;

        for (u32 x = 0; x < count; x++)
        {
            if (MaximumRendererVertexCount - 2 < State.Data.Vertexes.Count) { RendererRenderScene(); }

            // A
            {
                void* v = (void*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 0)));

                const u16 indx = *(u16*)((addr)indexes + (addr)(RendererIndexSize * (x * 2 + 0)));

                void* a = (void*)((addr)vertexes + (addr)(RendererVertexSize * indx));

                CopyMemory(v, a, RendererVertexSize);

                {
                    RTLVX* vertex = (RTLVX*)v;

                    if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                    if (State.Settings.IsFogActive && DAT_60018850 == 16) // TODO
                    {
                        vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                    }

                    vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
                }
            }

            // B
            {
                void* v = (void*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 1)));
                
                const u16 indx = *(u16*)((addr)indexes + (addr)(RendererIndexSize * (x * 2 + 1)));
                
                void* b = (void*)((addr)vertexes + (addr)(RendererVertexSize * indx));

                CopyMemory(v, b, RendererVertexSize);

                {
                    RTLVX* vertex = (RTLVX*)v;

                    if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                    if (State.Settings.IsFogActive && DAT_60018850 == 16) // TODO
                    {
                        vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                    }

                    vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
                }
            }

            State.Data.Indexes.Indexes[State.Data.Indexes.Count + 0] = State.Data.Vertexes.Count + 0;
            State.Data.Indexes.Indexes[State.Data.Indexes.Count + 1] = State.Data.Vertexes.Count + 1;

            State.Data.Indexes.Count = State.Data.Indexes.Count + 2;
            State.Data.Vertexes.Count = State.Data.Vertexes.Count + 2;
        }
    }

    // 0x60008730
    BOOL RenderPoints(RVX* vertexes, const u32 count)
    {
        if (!State.Scene.IsActive) { BeginRendererScene(); }

        AttemptRenderScene();

        for (u32 x = 0; x < count; x++)
        {
            RTLVX* vertex = (RTLVX*)((addr)vertexes + (addr)(RendererVertexSize * x));

            if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

            if (State.Settings.IsFogActive && DAT_60018850 == 16) // TODO
            {
                vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
            }

            vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
        }

        return State.DX.Device->DrawPrimitive(D3DPT_POINTLIST, RendererVertexType, vertexes, count, D3DDP_NONE) == DD_OK;
    }

    // 0x60007af0
    void RenderQuad(RVX* a, RVX* b, RVX* c, RVX* d)
    {
        if (RendererPrimitiveType != D3DPT_TRIANGLELIST) { RendererRenderScene(); }

        RendererPrimitiveType = D3DPT_TRIANGLELIST;

        if (MaximumRendererVertexCount - 4 < State.Data.Vertexes.Count) { RendererRenderScene(); }

        // A
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 0)));

            CopyMemory(v, a, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_60018850 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        // B
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 1)));

            CopyMemory(v, b, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_60018850 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        // C
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 2)));

            CopyMemory(v, c, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_60018850 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        // D
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 3)));

            CopyMemory(v, d, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_60018850 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 0] = State.Data.Vertexes.Count + 0;
        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 1] = State.Data.Vertexes.Count + 1;
        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 2] = State.Data.Vertexes.Count + 2;
        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 3] = State.Data.Vertexes.Count + 0;
        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 4] = State.Data.Vertexes.Count + 2;
        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 5] = State.Data.Vertexes.Count + 3;

        State.Data.Indexes.Count = State.Data.Indexes.Count + 6;
        State.Data.Vertexes.Count = State.Data.Vertexes.Count + 4;
    }

    // 0x60007650
    void RenderQuadMesh(RVX* vertexes, const u32* indexes, const u32 count)
    {
        if (RendererPrimitiveType != D3DPT_TRIANGLELIST) { RendererRenderScene(); }

        RendererPrimitiveType = D3DPT_TRIANGLELIST;

        for (u32 x = 0; x < count; x++)
        {
            if (MaximumRendererVertexCount - 4 < State.Data.Vertexes.Count) { RendererRenderScene(); }

            const u16 ia = *(u16*)((addr)indexes + (addr)(RendererIndexSize * (x * 4 + 0)));
            const u16 ib = *(u16*)((addr)indexes + (addr)(RendererIndexSize * (x * 4 + 1)));
            const u16 ic = *(u16*)((addr)indexes + (addr)(RendererIndexSize * (x * 4 + 2)));
            const u16 id = *(u16*)((addr)indexes + (addr)(RendererIndexSize * (x * 4 + 3)));

            RVX* a = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * ia));
            RVX* b = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * ib));
            RVX* c = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * ic));
            RVX* d = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * id));

            if (State.Settings.Cull == 1 || ((u32)AcquireNormal((f32x3*)a, (f32x3*)b, (f32x3*)c) & 0x80000000) != State.Settings.Cull) { RenderQuad(a, b, c, d); } // TODO
        }
    }

    // 0x60006fe0
    void RenderTriangle(RVX* a, RVX* b, RVX* c)
    {
        if (RendererPrimitiveType != D3DPT_TRIANGLELIST) { RendererRenderScene(); }

        RendererPrimitiveType = D3DPT_TRIANGLELIST;

        if (MaximumRendererVertexCount - 3 < State.Data.Vertexes.Count) { RendererRenderScene(); }

        // A
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 0)));

            CopyMemory(v, a, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_60018850 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        // B
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 1)));

            CopyMemory(v, b, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_60018850 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        // C
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 2)));

            CopyMemory(v, c, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_60018850 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 0] = State.Data.Vertexes.Count + 0;
        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 1] = State.Data.Vertexes.Count + 1;
        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 2] = State.Data.Vertexes.Count + 2;

        State.Data.Indexes.Count = State.Data.Indexes.Count + 3;
        State.Data.Vertexes.Count = State.Data.Vertexes.Count + 3;
    }

    // 0x60008270
    BOOL RenderTriangleFans(RVX* vertexes, const u32 vertexCount, const u32 indexCount, const u32* indexes)
    {
        u32 max = 0;

        for (u32 x = 0; x < indexCount + 2; x++)
        {
            State.Data.Indexes.Large[x] = indexes[x];

            if (max < indexes[x]) { max = indexes[x]; }
        }

        if (!State.Scene.IsActive) { BeginRendererScene(); }

        return State.DX.Device->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, RendererVertexType,
            vertexes, max + 1, State.Data.Indexes.Large, indexCount + 2, D3DDP_NONE) == DD_OK;
    }

    // 0x600072a0
    void RenderTriangleMesh(RVX* vertexes, const u32* indexes, const u32 count)
    {
        if (RendererPrimitiveType != D3DPT_TRIANGLELIST) { RendererRenderScene(); }

        RendererPrimitiveType = D3DPT_TRIANGLELIST;

        for (u32 x = 0; x < count; x++)
        {
            if (MaximumRendererVertexCount - 3 < State.Data.Vertexes.Count) { RendererRenderScene(); }

            const u16 ia = *(u16*)((addr)indexes + (addr)(RendererIndexSize * (x * 3 + 0)));
            const u16 ib = *(u16*)((addr)indexes + (addr)(RendererIndexSize * (x * 3 + 1)));
            const u16 ic = *(u16*)((addr)indexes + (addr)(RendererIndexSize * (x * 3 + 2)));

            RVX* a = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * ia));
            RVX* b = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * ib));
            RVX* c = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * ic));

            if (State.Settings.Cull == 1 || ((u32)AcquireNormal((f32x3*)a, (f32x3*)b, (f32x3*)c) & 0x80000000) != State.Settings.Cull) { RenderTriangle(a, b, c); } // TODO
        }
    }

    // 0x60007e60
    BOOL RenderTriangleStrips(RVX* vertexes, const u32 vertexCount, const u32 indexCount, const u32* indexes)
    {
        if (RendererPrimitiveType != D3DPT_TRIANGLESTRIP) { RendererRenderScene(); }

        RendererPrimitiveType = D3DPT_TRIANGLESTRIP;

        if ((MaximumRendererVertexCount - vertexCount + 3) < State.Data.Vertexes.Count) { RendererRenderScene(); }

        for (u32 x = 0; x < vertexCount; x++)
        {
            // A
            {
                RVX* a = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * *(u16*)((addr)indexes + (addr)(RendererIndexSize * (x * 3 + 0)))));

                RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 0)));

                CopyMemory(v, a, RendererVertexSize);

                {
                    RTLVX* vertex = (RTLVX*)v;

                    if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                    if (State.Settings.IsFogActive && DAT_60018850 == 16) // TODO
                    {
                        vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                    }

                    vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
                }
            }

            // B
            {
                RVX* b = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * *(u16*)((addr)indexes + (addr)(RendererIndexSize * (x * 3 + 1)))));

                RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 1)));

                CopyMemory(v, b, RendererVertexSize);

                {
                    RTLVX* vertex = (RTLVX*)v;

                    if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                    if (State.Settings.IsFogActive && DAT_60018850 == 16) // TODO
                    {
                        vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                    }

                    vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
                }
            }

            // C
            {
                RVX* c = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * *(u16*)((addr)indexes + (addr)(RendererIndexSize * (x * 3 + 2)))));

                RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 2)));

                CopyMemory(v, c, RendererVertexSize);

                {
                    RTLVX* vertex = (RTLVX*)v;

                    if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                    if (State.Settings.IsFogActive && DAT_60018850 == 16) // TODO
                    {
                        vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                    }

                    vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
                }
            }

            State.Data.Indexes.Indexes[State.Data.Indexes.Count + 0] = State.Data.Vertexes.Count + 0;
            State.Data.Indexes.Indexes[State.Data.Indexes.Count + 1] = State.Data.Vertexes.Count + 1;
            State.Data.Indexes.Indexes[State.Data.Indexes.Count + 2] = State.Data.Vertexes.Count + 2;

            State.Data.Indexes.Count = State.Data.Indexes.Count + 3;
            State.Data.Vertexes.Count = State.Data.Vertexes.Count + 3;
        }

        return TRUE;
    }
}