/*
Copyright (c) 2024 - 2026 Americus Maximus

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

#include <malloc.h>
#include <math.h>
#include <stdio.h>

#define MAX_MESSAGE_BUFFER_LENGTH 512

#define ALIGNVIEWPORT(X)    (X & 0xFFFFFFF0)

using namespace Renderer;
using namespace RendererModuleValues;

namespace RendererModule
{
    RendererModuleState State;

    // 0x60002e20
    // NOTE: In the original renderer the method does not perform any meaningful actions.
    // I combined message and error methods, because error method does not halt the execution as expected.
    void Message(const char* format, ...) { }

    // 0x60001010
    void CopyViewPortSurface(void* surface)
    {
        const u32 offset =
            State.ViewPort.Y0 * RendererSurfaceStride + ALIGNVIEWPORT(State.ViewPort.X0) * sizeof(u16);

        const void* src = (void*)((addr)State.Renderer.Surface.Surface + offset);
        void* dst = (void*)((addr)surface + offset);

        for (u32 y = 0; y < State.ViewPort.Height; y++)
        {
            CopyMemory(dst, src, State.ViewPort.Width * sizeof(u16));

            src = (void*)((addr)src + RendererSurfaceStride);
            dst = (void*)((addr)dst + State.Window.Stride);
        }
    }

    // 0x60001090
    // 0x60004e10
    u32 RendererClearGameWindow(void)
    {
        const u32 color = OptimizedClearColor;
        const u32 offset =
            State.ViewPort.Y0 * RendererSurfaceStride + ALIGNVIEWPORT(State.ViewPort.X0) * sizeof(u16);

        u32* pixels = (u32*)((addr)State.Renderer.Surface.Surface + offset);

        for (u32 y = 0; y < State.ViewPort.Height; y++)
        {
            for (u32 x = 0; x < State.ViewPort.Width; x++)
            {
                pixels[x] = color;
            }

            pixels = (u32*)((addr)pixels + RendererSurfaceStride);
        }

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60002420
    BOOL CALLBACK EnumerateRendererDevices(GUID* uid, LPSTR name, LPSTR description, LPVOID context)
    {
        const s32 indx = *(s32*)context;

        if (indx == RendererDeviceIndex)
        {
            State.Device.ID = NULL;

            if (uid != NULL)
            {
                State.Device.ID = &State.Device.Value;

                State.Device.Value = *uid;
            }

            *(s32*)context = INVALID_DEVICE_INDEX;

            return FALSE;
        }

        *(s32*)context = indx + 1;

        return TRUE;
    }
    
    // 0x60002480
    HRESULT CALLBACK EnumerateRendererDeviceModes(LPDDSURFACEDESC desc, LPVOID context)
    {
        u32 bits = desc->ddpfPixelFormat.dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_15
            ? GRAPHICS_BITS_PER_PIXEL_16 : desc->ddpfPixelFormat.dwRGBBitCount;

        const u32 format = AcquirePixelFormat(&desc->ddpfPixelFormat);

        if (format != RENDERER_PIXEL_FORMAT_NONE)
        {
            const u32 width = desc->dwWidth;
            const u32 height = desc->dwHeight;

            RendererModuleDescriptorDeviceCapabilities* caps = NULL;

            if (width == GRAPHICS_RESOLUTION_640 && height == GRAPHICS_RESOLUTION_480 && bits == GRAPHICS_BITS_PER_PIXEL_16)
            {
                caps = &ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16];
            }
            else if (width == GRAPHICS_RESOLUTION_800 && height == GRAPHICS_RESOLUTION_600 && bits == GRAPHICS_BITS_PER_PIXEL_16)
            {
                caps = &ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16];
            }
            else
            {
                SYSTEM_INFO info;
                GetSystemInfo(&info);

                const u32 indx = *(u32*)context;

                if (info.dwPageSize / sizeof(RendererModuleDescriptorDeviceCapabilities) <= indx) { return DDENUMRET_CANCEL; }

                *(u32*)context = indx + 1;

                caps = &ModuleDescriptor.Capabilities.Capabilities[indx];
            }

            caps->Width = width;
            caps->Height = height;
            caps->Bits = format == RENDERER_PIXEL_FORMAT_R5G5B5 ? GRAPHICS_BITS_PER_PIXEL_15 : bits;
            caps->Format = format;
            caps->Unk03 = 2;
            caps->Unk04 = 0;
            caps->IsActive = TRUE;
        }

        return DDENUMRET_OK;
    }

    // 0x60002580
    u32 STDCALLAPI InitializeRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        {
            const char* value = getenv(RENDERER_MODULE_DISPLAY_ENVIRONMENT_PROPERTY_NAME);

            if (value == NULL)
            {
                if (State.Device.ID == NULL) { SelectDevice(DEFAULT_DEVICE_INDEX); }
            }
            else
            {
                const s32 index = atoi(value);

                if (DEFAULT_DEVICE_INDEX < index)
                {
                    if (SelectDevice(index) == RENDERER_MODULE_FAILURE) { SelectDevice(DEFAULT_DEVICE_INDEX); }
                }
                else { SelectDevice(DEFAULT_DEVICE_INDEX); }
            }
        }

        IDirectDraw* instance = NULL;

        State.DX.Code = DirectDrawCreate(NULL, &instance, NULL);

        if (State.DX.Code == DD_OK)
        {
            State.DX.Code = instance->QueryInterface(IID_IDirectDraw2, (LPVOID*)&State.DX.Instance);

            State.Lambdas.Lambdas.SelectInstance(instance);

            instance->Release();

            if (State.DX.Code == DD_OK)
            {
                State.DX.Code = State.DX.Instance->SetCooperativeLevel(State.Window.HWND, State.Settings.CooperativeLevel);

                if (State.DX.Code == DD_OK)
                {
                    DWORD free = 0, total = 0;

                    DDSCAPS caps = { DDSCAPS_VIDEOMEMORY };

                    State.Settings.MaxAvailableMemory =
                        State.DX.Instance->GetAvailableVidMem(&caps, &total, &free) == DD_OK
                        ? free : DEFAULT_DEVICE_AVAIABLE_VIDEO_MEMORY;

                    DDCAPS hal;
                    ZeroMemory(&hal, sizeof(DDCAPS));

                    hal.dwSize = sizeof(DDCAPS);

                    DDCAPS hel;
                    ZeroMemory(&hel, sizeof(DDCAPS));

                    hel.dwSize = sizeof(DDCAPS);

                    State.DX.Instance->GetCaps(&hal, &hel);

                    SYSTEM_INFO info;
                    GetSystemInfo(&info);

                    ModuleDescriptor.Capabilities.Capabilities =
                        (RendererModuleDescriptorDeviceCapabilities*)VirtualAlloc(NULL, info.dwPageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

                    ZeroMemory(ModuleDescriptor.Capabilities.Capabilities, info.dwPageSize);

                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Width = GRAPHICS_RESOLUTION_640;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Height = GRAPHICS_RESOLUTION_480;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Bits = GRAPHICS_BITS_PER_PIXEL_16;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Format = RENDERER_PIXEL_FORMAT_R5G6B5;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Unk03 = 2;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Unk04 = 0;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].IsActive = TRUE;

                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Width = GRAPHICS_RESOLUTION_800;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Height = GRAPHICS_RESOLUTION_600;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Bits = GRAPHICS_BITS_PER_PIXEL_16;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Format = RENDERER_PIXEL_FORMAT_R5G6B5;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Unk03 = 2;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].Unk04 = 0;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_800_600_16].IsActive = TRUE;

                    ModuleDescriptor.Capabilities.Count = 3;

                    State.DX.Instance->EnumDisplayModes(DDEDM_NONE, NULL,
                        &ModuleDescriptor.Capabilities.Count, EnumerateRendererDeviceModes);

                    DWORD previous = 0;
                    VirtualProtect(ModuleDescriptor.Capabilities.Capabilities, info.dwPageSize, PAGE_READONLY, &previous);
                }
            }
        }

        SetEvent(State.Mutex);

        *result = State.DX.Code;

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x600027f0
    u32 STDCALLAPI InitializeRendererDeviceSurfacesExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        ReleaseRendererDeviceSurfaces();

        State.Window.Width = ModuleDescriptor.Capabilities.Capabilities[wp].Width;
        State.Window.Height = ModuleDescriptor.Capabilities.Capabilities[wp].Height;

        State.DX.Bits = ModuleDescriptor.Capabilities.Capabilities[wp].Bits;
        State.DX.Surfaces.Bits = ModuleDescriptor.Capabilities.Capabilities[wp].Bits;
        State.Window.Bits = ModuleDescriptor.Capabilities.Capabilities[wp].Bits == GRAPHICS_BITS_PER_PIXEL_15
            ? GRAPHICS_BITS_PER_PIXEL_16 : ModuleDescriptor.Capabilities.Capabilities[wp].Bits;

        SelectRendererColorMasks(State.DX.Bits);

        State.DX.Code = DD_OK;

        if (!State.Settings.IsWindowMode)
        {
            State.DX.Code = State.DX.Instance->SetDisplayMode(ModuleDescriptor.Capabilities.Capabilities[wp].Width,
                ModuleDescriptor.Capabilities.Capabilities[wp].Height, ModuleDescriptor.Capabilities.Capabilities[wp].Bits, 0, 0);

            if (State.DX.Code == DD_OK)
            {
                DDSURFACEDESC desc;
                ZeroMemory(&desc, sizeof(DDSURFACEDESC));

                desc.dwSize = sizeof(DDSURFACEDESC);
                desc.dwFlags = DDSD_CAPS;
                desc.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_PRIMARYSURFACE;

                State.DX.Code = State.DX.Instance->CreateSurface(&desc, &State.DX.Surfaces.Main, NULL);

                if (State.DX.Surfaces.Main != NULL)
                {
                    State.DX.Surfaces.Main->QueryInterface(IID_IDirectDrawSurface2, (LPVOID*)&State.DX.Surfaces.Active[1]);
                }
            }
        }
        else
        {
            DDSURFACEDESC desc;
            ZeroMemory(&desc, sizeof(DDSURFACEDESC));

            desc.dwSize = sizeof(DDSURFACEDESC);
            desc.dwFlags = DDSD_CAPS;
            desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

            State.DX.Code = State.DX.Instance->CreateSurface(&desc, &State.DX.Surfaces.Main, NULL);

            if (State.DX.Code != DD_OK)
            {
                if (State.DX.Code == DDERR_OUTOFMEMORY || State.DX.Code == DDERR_OUTOFVIDEOMEMORY)
                {
                    Message("There was not enough video memory to create the rendering surface.\nTo run this program in a window of this size, please adjust your display settings for a smaller desktop area or a lower palette size and restart the program.");
                }
                else { Message("CreateSurface for window front buffer failed %8x.\n", State.DX.Code); }

                SetEvent(State.Mutex);

                *result = State.DX.Code;

                return RENDERER_MODULE_SUCCESS;
            }

            if (State.DX.Surfaces.Main != NULL)
            {
                State.DX.Surfaces.Main->QueryInterface(IID_IDirectDrawSurface2, (LPVOID*)&State.DX.Surfaces.Active[1]);
            }
        }

        State.DX.Surfaces.Active[2] = (IDirectDrawSurface2*)0x1234; // TODO

        SetEvent(State.Mutex);

        *result = State.DX.Code;

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x600029b0
    u32 STDCALLAPI ReleaseRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        ReleaseRendererDeviceSurfaces();

        State.Lambdas.Lambdas.SelectInstance(NULL);

        State.DX.Instance->Release();
        State.DX.Instance = NULL;

        SetEvent(State.Mutex);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x600029f0
    u32 InitializeRendererDeviceLambdas(void)
    {
        if (State.Mutex == NULL) { State.Mutex = CreateEventA(NULL, FALSE, FALSE, NULL); }

        State.Window.HWND = State.Lambdas.Lambdas.AcquireWindow();

        if (State.Window.HWND != NULL)
        {
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE, (RENDERERMODULEEXECUTECALLBACK)InitializeRendererDeviceExecute);
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_RELEASE_DEVICE, (RENDERERMODULEEXECUTECALLBACK)ReleaseRendererDeviceExecute);
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES, (RENDERERMODULEEXECUTECALLBACK)InitializeRendererDeviceSurfacesExecute);

            State.Settings.CooperativeLevel = State.Settings.CooperativeLevel & ~(DDSCL_EXCLUSIVE | DDSCL_NORMAL | DDSCL_FULLSCREEN);

            State.Settings.CooperativeLevel = State.Settings.IsWindowMode
                ? State.Settings.CooperativeLevel | DDSCL_NORMAL
                : State.Settings.CooperativeLevel | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN;

            SetForegroundWindow(State.Window.HWND);
            PostMessageA(State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE, 0, 0);
            WaitForSingleObject(State.Mutex, INFINITE);

            return State.DX.Code;
        }

        State.DX.Code = RENDERER_MODULE_INITIALIZE_DEVICE_SUCCESS;

        return RENDERER_MODULE_INITIALIZE_DEVICE_SUCCESS;
    }

    // 0x60002ae0
    u32 ReleaseRendererWindow(void)
    {
        if (State.DX.Instance == NULL) { return RENDERER_MODULE_FAILURE; }

        SetForegroundWindow(State.Window.HWND);
        PostMessageA(State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_RELEASE_DEVICE, 0, 0);
        WaitForSingleObject(State.Mutex, INFINITE);
        CloseHandle(State.Mutex);

        State.Mutex = NULL;
        State.Window.HWND = NULL;

        return State.DX.Code;
    }

    // 0x60002b50
    void ReleaseRendererDeviceSurfaces(void)
    {
        for (s32 x = (MAX_ACTIVE_SURFACE_COUNT - 1); x >= 0; x--)
        {
            if (State.DX.Surfaces.Active[x] != NULL)
            {
                if (State.DX.Surfaces.Active != (void*)0x1234) // TODO
                {
                    State.DX.Surfaces.Active[x]->Release();
                    State.DX.Surfaces.Active[x] = NULL;
                }
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

    // 0x60002e80
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

    // 0x600038d0
    void RendererSetTexturePixelsR5G6B5(u16* dst, const u16* src, const u32 count)
    {
        for (u32 x = 0; x < count; x++)
        {
            const u16 color = src[x];
            dst[x] = ((color >> 1) & 0x7FE0) | (color & 0x1F);
        }
    }

    // 0x60003910
    BOOL RendererSetTexturePixelsA1R5G6B5(u16* dst, const u16* src, const u32 count)
    {
        BOOL match = FALSE;

        for (u32 x = 0; x < count; x++)
        {
            u16 color = src[x];

            if ((color & 0x8000) == 0)
            {
                match = TRUE;
                dst[x] = 0x0000;
            }
            else
            {
                color = ((color & 0xFFE0) << 1) | (color & 0x1F);
                dst[x] = color == 0x0000 ? 0x20 : color;
            }
        }

        return match;
    }

    // 0x60003980
    BOOL RendererSetTexturePixelsR5G5B5(u16* dst, const u16* src, const u32 count)
    {
        BOOL match = FALSE;

        for (u32 x = 0; x < count; x++)
        {
            u16 color = src[x];

            if ((color & 0x8000) == 0)
            {
                match = TRUE;
                dst[x] = 0x0000;
            }
            else
            {
                dst[x] = color;
            }
        }

        return match;
    }

    // 0x600039d0
    void RendererSetTexturePixelsA4R4G4B4(u32* dst, const u16* src, const u32 count)
    {
        for (u32 x = 0; x < count; x++)
        {
            const u16 color = src[x];

            s32 b = (color >> 12) + 1;
            u32 r = (color >> 8 & 0xF) * b >> 4;
            u32 g = (color >> 4 & 0xF) * b >> 4;

            if (State.DX.Surfaces.Bits == GRAPHICS_BITS_PER_PIXEL_16)
            {
                r = r << 12;
                g = g << 7;
            }
            else
            {
                r = r << 11;
                g = g << 6;
            }

            dst[x] = (((color & 0xF) * b >> 4) << 1) | g | r | b * -0x10000 + 0x100000U;
        }
    }

    // 0x60003a80
    u32 AcquireWeightedColorValue(u32 color, f32 modifier)
    {
        const u32 a = (u32)((color >> 24) * modifier);
        const u32 r = (u32)(((color >> 16) & 0xFF) * modifier);
        const u32 g = (u32)(((color >> 8) & 0xFF) * modifier);
        const u32 b = (u32)((color & 0xFF) * modifier);

        return (a << 24) | (r << 16) | (g << 8) | b;
    }

    // 0x60003b50
    BOOL RendererSetPaletteTexturePixels(Renderer::RendererTexture* tex, const u32* pixels)
    {
        BOOL match = FALSE;

        u8* dst = (u8*)tex->Pixels;
        const u8* src = (u8*)pixels;

        for (u32 x = 0; x < tex->Size; x++)
        {
            const u8 pixel = pixels[x];
            dst[x] = pixel;

            if (pixel == 0xFF)
            {
                match = TRUE;
            }
        }

        return match;
    }

    // 0x60003b90
    void RendererSetPaletteTexturePalette(Renderer::RendererTexture* tex, const u32* palette)
    {
        u32* dst = (u32*)tex->Palette;

        if (CurrentTexturePaletteCount == -1)
        {
            f32 multiplier = 0.0f;
            const u32* pal = palette;

            for (u32 x = 0; x < tex->PaletteCount; x++)
            {
                for (u32 xx = 0; xx < RENDERER_PALETTE_COLOR_COUNT; xx++)
                {
                    const u32 c = AcquireWeightedColorValue(pal[xx], multiplier);
                    const u32 color = CalculateColor(c, c);

                    dst[xx] = color;

                    if ((color & 0xFFFF) == 0)
                    {
                        dst[xx] = 1;
                    }
                }

                multiplier += 1.0f / (f32)tex->PaletteCount;

                dst[255] = 0x00000000;
                dst = (u32*)((addr)dst + RENDERER_PALETTE_COLOR_COUNT * sizeof(u32));
                pal = (u32*)((addr)pal + RENDERER_PALETTE_COLOR_COUNT * sizeof(u32));
            }
        }
        else
        {
            if (CurrentTexturePaletteCount < tex->PaletteCount)
            {
                dst = (u32*)((addr)dst
                    + CurrentTexturePaletteCount * RENDERER_PALETTE_COLOR_COUNT);

                for (u32 x = 0; x < RENDERER_PALETTE_COLOR_COUNT; x++)
                {
                    dst[x] = CalculateColor(palette[x], palette[x]);
                }
            }
            else
            {
                Message("Clut tupdate out of range - change STATE_UPDATECLUTNUM\n");
            }
        }
    }

    // 0x60004100
    void SelectRendererColorMasks(const u32 bits)
    {
        if (bits == GRAPHICS_BITS_PER_PIXEL_16)
        {
            RedRendererColorMask = 0xf800;
            GreenRendererColorMask = 0x7e0;
            BlueRendererColorMask = 0x1f;
            NonGreenRendererColorMask = 0xf81f;

            State.Renderer.Colors.Unknown1 = Unknown16BitColors1;
            State.Renderer.Colors.Unknown2 = Unknown16BitColors2;
            State.Renderer.Colors.Unknown3 = Unknown16BitColors3;
            State.Renderer.Colors.Unknown4 = Unknown16BitColors4;

            RedColorBitsOffset = 11;
            GreenColorBitsSize = 6;
        }
        else
        {
            RedRendererColorMask = 0x7c00;
            GreenRendererColorMask = 0x3e0;
            BlueRendererColorMask = 0x1f;
            NonGreenRendererColorMask = 0x7c1f;

            State.Renderer.Colors.Unknown1 = Unknown32BitColors1;
            State.Renderer.Colors.Unknown2 = Unknown32BitColors2;
            State.Renderer.Colors.Unknown3 = Unknown32BitColors3;
            State.Renderer.Colors.Unknown4 = Unknown32BitColors4;

            RedColorBitsOffset = 10;
            GreenColorBitsSize = 5;
        }
    }

    // 0x60004c40
    u32 CalculateColor(u32 color, u32 fallback)
    {
        const u32 b = (color & 0xff) >> 3;
        const u32 g = color >> 8 & 0xff;
        const u32 r = (color >> 0x10 & 0xff) >> 3;

        if (State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_16)
        {
            return b | (g >> 2) << 5 | r << 11;
        }

        if (State.DX.Surfaces.Bits == GRAPHICS_BITS_PER_PIXEL_15) {
            return b | (g >> 3) << 5 | r << 10;
        }

        return fallback;
    }

    // 0x60004cb0
    void CalculateVertexColor(s32 x, s32 y, u32 color)
    {
        VertexColor = CalculateColor(color, y);

        State.Renderer.Colors.UnknownValue1 = State.Renderer.Colors.Unknown1[8] & ((VertexColor & 0xffff) >> 1);
        State.Renderer.Colors.UnknownValue2 = State.Renderer.Colors.Unknown3[4] & ((VertexColor & 0xffff) >> 2);
        State.Renderer.Colors.UnknownValue3 = State.Renderer.Colors.UnknownValue1 + State.Renderer.Colors.UnknownValue2;
    }
    
    // 0x60004d40
    void SelectRendererSettings(const u32 width, const u32 height, const u32 bits)
    {
        if (State.Renderer.Surface.Allocated != NULL) { free(State.Renderer.Surface.Allocated); }

        RendererSurfaceStride = width * (bits >> 3);

        State.Renderer.Settings.Length = RendererSurfaceStride * height;

        State.Renderer.Settings.Width = width;
        State.Renderer.Settings.Height = height;

        ClipGameWindow(0, 0, width, height);

        State.Renderer.Surface.Allocated = malloc(State.Renderer.Settings.Length + RENDERER_SURFACE_SIZE_MODIFIER);

        State.Renderer.Surface.Surface = (void*)(((addr)State.Renderer.Surface.Allocated & RENDERER_SURFACE_ALIGNMENT_MASK) + RENDERER_SURFACE_SIZE_MODIFIER);

        State.Renderer.Active.Stride = RendererSurfaceStride;

        State.Renderer.Active.Length = State.Renderer.Settings.Length;

        State.Renderer.Active.Width = State.Renderer.Settings.Width;
        State.Renderer.Active.Height = State.Renderer.Settings.Height;

        State.Renderer.Active.Surface = State.Renderer.Surface.Surface;
    }

    // 0x60004e00
    void* AcquireRendererSurface(void)
    {
        return State.Renderer.Surface.Surface;
    }
}
