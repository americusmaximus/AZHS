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

#ifdef __WATCOMC__
#include <RendererModule.Export.hxx>
#else
#include "RendererModule.Export.hxx"
#endif

#include "DirectDraw.hxx"

#define DEPTH_BIT_MASK_32_BIT 0x100
#define DEPTH_BIT_MASK_24_BIT 0x200
#define DEPTH_BIT_MASK_16_BIT 0x400
#define DEPTH_BIT_MASK_8_BIT 0x800

#define STATE_INITIAL (-1)
#define STATE_INACTIVE 0
#define STATE_ACTIVE 1

#define CLEAR_DEPTH_VALUE (1.0f)
#define ENVIRONMENT_SECTION_NAME "DX7"
#define MAX_DEVICE_CAPABILITIES_COUNT 128 /* ORIGINAL: 98 */
#define MAX_ENUMERATE_DEVICE_COUNT 60 /* ORIGINAL: 16 */
#define MAX_ENUMERATE_DEVICE_NAME_COUNT 60 /* ORIGINAL: 10 */
#define MAX_ENUMERATE_DEVICE_NAME_LENGTH 80
#define MAX_LARGE_INDEX_COUNT 65536
#define MAX_TEXTURE_FORMAT_COUNT 128 /* ORIGINAL: 32 */
#define MAX_TEXTURE_PALETTE_COLOR_COUNT 256
#define MAX_TEXTURE_STAGE_COUNT 8
#define MAX_TEXTURE_STATE_STATE_COUNT 120
#define MAX_USABLE_TEXTURE_FORMAT_COUNT 22
#define MAX_VERTEX_COUNT 32768
#define MAX_WINDOW_COUNT 65536
#define WINDOW_OFFSET 8

#if !defined(__WATCOMC__) && _MSC_VER <= 1200
inline void LOGERROR(...) { }
inline void LOGWARNING(...) { }
inline void LOGMESSAGE(...) { }
#else
#define LOGERROR(...) Message(RENDERER_MODULE_MESSAGE_SEVERITY_ERROR, __VA_ARGS__)
#define LOGWARNING(...) Message(RENDERER_MODULE_MESSAGE_SEVERITY_WARNING, __VA_ARGS__)
#define LOGMESSAGE(...) Message(RENDERER_MODULE_MESSAGE_SEVERITY_MESSAGE, __VA_ARGS__)
#endif

#if _MSC_VER <= 1200
#define isnan _isnan
#endif

namespace Renderer
{
    struct RendererTexture
    {
        s32 Unk00; // TODO
        u32 Width;
        u32 Height;
        s32 FormatIndexValue; // TODO
        u32 Options;
        s32 MipMapCount;
        u32 Stage;
        RendererTexture* Previous;
        u32 UnknownFormatIndexValue; // TODO
        s32 FormatIndex; // TODO
        u32 MemoryType;
        BOOL Is16Bit;
        IDirectDrawSurface7* Surface;
        IDirectDrawSurface7* Texture;
        IDirectDrawPalette* Palette;
        DDSURFACEDESC2 Descriptor;
        u32 Colors;
    };
}

namespace RendererModule
{
    extern u32 DAT_6005ab5c; // 0x6005ab5c

    struct MinMax
    {
        u32 Min;
        u32 Max;
    };

    struct TextureStageState
    {
        s32 Values[MAX_TEXTURE_STAGE_COUNT];
    };

    struct TextureFormat
    {
        DDSURFACEDESC2 Descriptor;

        BOOL IsPalette;
        u32 RedBitCount;
        u32 BlueBitCount;
        u32 GreenBitCount;
        u32 PaletteColorBits;
        u32 AlphaBitCount;

        u32 DXTF; // Format
        u32 DXTT; // Type
        u32 DXTN; // Name
    };

    struct RendererModuleWindow
    {
        Renderer::RendererTexture* Texture;
        IDirectDrawSurface7* Surface;
    };

    struct RendererModuleState
    {
        struct
        {
            IDirect3D7* DirectX; // 0x60058d98
            IDirect3DDevice7* Device; // 0x60058d9c

            struct
            {
                BOOL IsSoft; // 0x60058d6c

                IDirectDraw7* Instance; // 0x60058d80

                struct
                {
                    struct
                    {

                    } Active;

                    IDirectDrawSurface7* Main; // 0x60058d84
                    IDirectDrawSurface7* Back; // 0x60058d88
                } Surfaces;
            } Active;

            struct
            {
                IDirectDrawSurface7* Window; // 0x600186cc

                IDirectDrawSurface7* Depth; // 0x60058d94
            } Surfaces;
        } DX;

        struct
        {
            struct
            {
                u32 Count; // 0x60058878

                u16 Indexes[MAX_LARGE_INDEX_COUNT]; // 0x60038868
            } Indexes;

            struct
            {
                u32 Count; // 0x60058874

                u32 Vertexes[MAX_VERTEX_COUNT]; // 0x60018868
            } Vertexes;
        } Data;

        struct
        {
            GUID* Identifier; // 0x600186d8

            struct
            {
                BOOL IsAccelerated; // 0x6005aae0

                BOOL IsDepthVideoMemoryCapable; // 0x6005aaf0
                BOOL IsDepthAvailable; // 0x6005aaf4

                BOOL IsDepthBufferRemovalAvailable; // 0x6005ab18

                BOOL IsStencilBufferAvailable; // 0x6005ab48

                u32 MinTextureWidth; // 0x6005ab60
                u32 MaxTextureWidth; // 0x6005ab64
                u32 MinTextureHeight; // 0x6005ab68
                u32 MaxTextureHeight; // 0x6005ab6c

                u32 MultipleTextureWidth; // 0x6005ab70
                BOOL IsPowerOfTwoTexturesWidth; // 0x6005ab74
                u32 MultipleTextureHeight; // 0x6005ab78
                BOOL IsPowerOfTwoTexturesHeight; // 0x6005ab7c
                u32 MaximumSimultaneousTextures; // 0x6005ab80
                BOOL IsSquareOnlyTextures; // 0x6005ab84
            } Capabilities;
        } Device;

        struct
        {
            u32 Count; // 0x600186e0

            GUID* Indexes[MAX_ENUMERATE_DEVICE_COUNT]; // 0x60018250

            struct
            {
                u32 Count; // 0x60059000
                BOOL IsAvailable; // 0x60059004

                char Names[MAX_ENUMERATE_DEVICE_NAME_COUNT][MAX_ENUMERATE_DEVICE_NAME_LENGTH]; // 0x60018340

                struct
                {
                    GUID* Indexes[MAX_ENUMERATE_DEVICE_COUNT]; // 0x60058e40
                    GUID Identifiers[MAX_ENUMERATE_DEVICE_COUNT]; // 0x60058e80
                } Identifiers;

                struct
                {
                    HMONITOR* Indexes[MAX_ENUMERATE_DEVICE_COUNT]; // 0x60058f80
                    HMONITOR Monitors[MAX_ENUMERATE_DEVICE_COUNT]; // 0x60058fc0
                } Monitors;

                DDDEVICEIDENTIFIER Identifier; // 0x60059010
            } Enumeration;
        } Devices;

        struct
        {
            RENDERERMODULELOGLAMBDA Log; // 0x6001713c

            RENDERERMODULEALLOCATEMEMORYLAMBDA AllocateMemory; // 0x60058e14
            RENDERERMODULERELEASEMEMORYLAMBDA ReleaseMemory; // 0x60058e18

            RendererModuleLambdaContainer Lambdas; // 0x6007b860
        } Lambdas;

        struct
        {
            BOOL IsActive; // 0x60018230
        } Lock;

        struct
        {
            BOOL IsActive; // 0x6001822c
        } Scene;

        struct
        {
            u32 Count; // 0x60058884
            BOOL Illegal; // 0x60058888

            Renderer::RendererTexture* Current; // 0x60058e2c

            TextureStageState StageStates[MAX_TEXTURE_STATE_STATE_COUNT]; // 0x6007b8a0

            struct
            {
                TextureFormat Formats[MAX_TEXTURE_FORMAT_COUNT]; // 0x60059440
                s32 Indexes[MAX_USABLE_TEXTURE_FORMAT_COUNT]; // 0x6005a860

                struct
                {

                } Depth;
            } Formats;
        } Textures;

        struct
        {
            u32 X0; // 0x60018238
            u32 Y0; // 0x6001823c
            u32 X1; // 0x60018240
            u32 Y1; // 0x60018244
        } ViewPort;

        struct
        {
            u32 Count; // 0x6001824c

            HWND HWND; // 0x6007b880

            u32 Height; // 0x6005a934

            u32 Width; // 0x6005aac0

            u32 Bits; // 0x60058d7c
        } Window;

        RendererModuleWindow Windows[MAX_WINDOW_COUNT]; // 0x6007c7c0
    };

    extern RendererModuleState State;

    void Message(const u32 severity, const char* format, ...);

    BOOL AcquireRendererDeviceAccelerationState(const u32 indx);
    BOOL BeginRendererScene(void);
    BOOL CALLBACK EnumerateDirectDrawDevices(GUID* uid, LPSTR name, LPSTR description, LPVOID context, HMONITOR monitor);
    BOOL EndRendererScene(void);
    BOOL InitializeRendererDeviceDepthSurfaces(const u32 width, const u32 height, IDirectDrawSurface7* depth, IDirectDrawSurface7* surf);
    const char* AcquireRendererMessage(const HRESULT code);
    const char* AcquireRendererMessageDescription(const HRESULT code);
    HRESULT CALLBACK EnumerateRendererDevicePixelFormats(LPDDPIXELFORMAT format, LPVOID context);
    Renderer::RendererTexture* AllocateRendererTexture(const u32 size);
    Renderer::RendererTexture* AllocateRendererTexture(const u32 width, const u32 height, const u32 format, const u32 options, const u32 state, const BOOL destination);
    Renderer::RendererTexture* InitializeRendererTexture(void);
    s32 AcquireSettingsValue(const s32 value, const char* section, const char* name);
    s32 InitializeRendererTextureDetails(Renderer::RendererTexture* tex, const BOOL destination);
    u32 AcquireDirectDrawDeviceCount(GUID** uids, HMONITOR** monitors, const char* section);
    u32 AcquireRendererDeviceCount(void);
    u32 ClearRendererViewPort(const u32 x0, const u32 y0, const u32 x1, const u32 y1, const BOOL window);
    u32 DisposeRendererTexture(Renderer::RendererTexture* tex);
    void AcquireRendererModuleDescriptor(RendererModuleDescriptor* desc, const char* section);
    void AttemptRenderScene(void);
    void InitializeTextureStateStates(void);
    void ReleaseRendererTexture(Renderer::RendererTexture* tex);
    void RendererRenderScene(void);
    void SelectRendererDevice(void);
}