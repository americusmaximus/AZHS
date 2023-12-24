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
                        + (addr)((desc.ddpfPixelFormat.dwRGBBitCount >> 3) * State.Lock.State.Rect.left)
                        + (addr)(desc.lPitch * State.Lock.State.Rect.top));
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
}