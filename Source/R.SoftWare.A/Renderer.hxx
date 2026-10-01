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

#pragma once

#ifdef __WATCOMC__
#include <RendererModule.Export.hxx>
#else
#include "RendererModule.Export.hxx"
#endif

#include "DirectDraw.hxx"

#define DEFAULT_DEVICE_AVAIABLE_VIDEO_MEMORY (16 * 1024 * 1024) /* ORIGINAL: 0x200000 (2 MB) */
#define DEFAULT_RENDERER_MODE (-1)
#define DEFAULT_RENDERER_SURFACE_STRIDE (GRAPHICS_RESOLUTION_640 * sizeof(u16))
#define MAX_ACTIVE_SURFACE_COUNT 8
#define MAX_ACTIVE_UNKNOWN_COUNT 4
#define MAX_ACTIVE_USABLE_TEXTURE_FORMAT_COUNT 9
#define MAX_UNKNOWN_COLOR_ARRAY_COUNT 16
#define MAX_UNKNOWN_COUNT (MAX_ACTIVE_UNKNOWN_COUNT + 2)
#define MAX_USABLE_TEXTURE_FORMAT_COUNT (MAX_ACTIVE_USABLE_TEXTURE_FORMAT_COUNT + 2)
#define MIN_DEVICE_AVAIABLE_VIDEO_MEMORY (16 * 1024 * 1024) /* ORIGINAL: 0x8000 (32 KB) */
#define RENDERER_SURFACE_ALIGNMENT_MASK 0xFFFFFF00
#define RENDERER_SURFACE_SIZE_MODIFIER 256

namespace Renderer
{
    struct RendererTexture
    {
        u32 Width;                      // 0x00
        u32 Height;                     // 0x04
        u16* Data;                      // 0x08
        u16* Pixels;                    // 0x0C
        u16* Palette;                   // 0x10
        u32 Bits;                       // 0x14
        u32 Stride;                     // 0x18
        u32 Format1;                    // 0x1C
        u32 Format2;                    // 0x20
        u32 Size;                       // 0x24
        u32 ColorDepth;                 // 0x28
        RendererTexture* Previous;      // 0x2C
    };
}

namespace RendererModule
{
    struct RendererModuleState
    {
        struct
        {
            GUID* ID;   // 0x6003e098
            GUID Value; // 0x60040090
        } Device;

        struct
        {
            u32 Bits;       // 0x60041110

            HRESULT Code;   // 0x6003f04c

            IDirectDraw2* Instance; // 0x6003f060

            struct
            {
                u32 Bits;   // 0x6004111c

                IDirectDrawSurface* Main; // 0x6003e064
                IDirectDrawSurface* Back; // 0x6003e068

                IDirectDrawSurface2* Active[MAX_ACTIVE_SURFACE_COUNT]; // 0x6003f074

                IDirectDrawSurface2* Window; // 0x6003f08c
            } Surfaces;
        } DX;

        struct
        {

            RendererModuleLambdaContainer Lambdas; // 0x600410f0
        } Lambdas;

        struct
        {
            BOOL IsActive;                  // 0x6003f0b0
            IDirectDrawSurface2* Surface;   // 0x6003f0b4

            RendererModuleWindowLock State; // 0x60041134
        } Lock;

        HANDLE Mutex;               // 0x6003f090

        struct
        {
            struct
            {
                u32 Stride;         // 0x6003f108
                u32 Length;         // 0x6003f10c
                u32 Width;          // 0x6003f110
                u32 Height;         // 0x6003f114
                void* Surface;      // 0x6003f118
            } Active;

            struct
            {
                u32* Unknown4;      // 0x606c2398
                u32* Unknown3;      // 0x606c239c
                u32* Unknown2;      // 0x606c23a0
                u32* Unknown1;      // 0x606c23a4

                u16 UnknownValue3;  // 0x606c23a8
                u16 UnknownValue1;  // 0x606c23aa
                u16 UnknownValue2;  // 0x606c23ac
            } Colors;

            struct
            {
                void* Surface;      // 0x6003f100
                void* Allocated;    // 0x6003f104
            } Surface;

            struct
            {
                u32 Length; // 0x6003f0f4
                u32 Width;  // 0x6003f0f8
                u32 Height; // 0x6003f0fc
            } Settings;
        } Renderer;

        struct
        {
            u32 CooperativeLevel;   // 0x6003e050
            BOOL IsWindowMode;      // 0x6003f054

            u32 MaxAvailableMemory; // 0x6003e05c
        } Settings;

        struct
        {
            u32 X0;     // 0x6003e0b8
            u32 Y0;     // 0x6003e0bc
            u32 Width;  // 0x6003e0c0
            u32 Height; // 0x6003e0c4
            u32 X1;     // 0x6003e0c8
            u32 Y1;     // 0x6003e0cc
        } ViewPort;

        struct {
            Renderer::RendererTexture* Current; // 0x6003f0d4
        } Textures;

        struct
        {
            u32 Width;  // 0x60041114
            u32 Height; // 0x60041118
            u32 Bits;   // 0x60041120

            u32 Stride; // 0x60041130

            HWND HWND;  // 0x6003f048
        } Window;
    };

    extern RendererModuleState State;

    void Message(const char* format, ...);

    void CopyViewPortSurface(void* surface);

    u32 CalculateColor(u32 color, u32 fallback);
    void CalculateVertexColor(s32 x, s32 y, u32 color);

    u32 RendererClearGameWindow(void);
    void* AcquireRendererSurface(void);
    BOOL CALLBACK EnumerateRendererDevices(GUID* uid, LPSTR name, LPSTR description, LPVOID context);
    HRESULT CALLBACK EnumerateRendererDeviceModes(LPDDSURFACEDESC desc, LPVOID context);
    u32 AcquirePixelFormat(const DDPIXELFORMAT* format);
    u32 InitializeRendererDeviceLambdas(void);
    u32 ReleaseRendererWindow(void);
    u32 STDCALLAPI InitializeRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result);
    u32 STDCALLAPI InitializeRendererDeviceSurfacesExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result);
    u32 STDCALLAPI ReleaseRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result);
    void ReleaseRendererDeviceSurfaces(void);
    void SelectRendererColorMasks(const u32 bits);
    void SelectRendererSettings(const u32 width, const u32 height, const u32 bits);
}
