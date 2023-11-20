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

#define DIRECTDRAW_VERSION 0x600
#include <ddraw.h>

#ifdef __WATCOMC__
#undef PURE
#define PURE
#endif

#define DIRECT3D_VERSION 0x600
#include <d3d.h>

#ifdef __WATCOMC__
#include <mmsystem.h>
#endif

#define DDGDI_NONE 0
#define DDEDM_NONE 0
#define D3DDP_NONE 0
#define D3DVBCAPS_NONE 0
#define D3DVBOPTIMIZE_NONE 0
#define DDSDM_NONE 0
#define D3DCOLOR_NONE 0

#define D3DRENDERSTATE_FOGSTART (D3DRENDERSTATETYPE)36
#define D3DRENDERSTATE_FOGEND (D3DRENDERSTATETYPE)37
#define D3DRENDERSTATE_FOGDENSITY (D3DRENDERSTATETYPE)38

#define MAX_RENDERER_VERTEX_COUNT 253
#define MAX_RENDERER_SMALL_INDEX_COUNT 256
#define MAX_RENDERER_MEDIUM_INDEX_COUNT 8096
#define MAX_RENDERER_LARGE_INDEX_COUNT 65536

#define MAX_RENDERER_DEVICE_COUNT 16 /* ORIGINAL: 10 */

#define MAX_DEVICE_NAME_LENGTH 32

#define MAX_ACTIVE_SURFACE_COUNT 8

#define MIN_RENDERER_DEVICE_AVAIABLE_VIDEO_MEMORY (16 * 1024 * 1024) /* ORIGINAL: 0x200000 (2 MB) */

#define MAX_RENDERER_MODULE_DEVICE_CAPABILITIES_COUNT 128 /* ORIGINAL: 30, MODIFIED: 100 */

#define DEPTH_BIT_MASK_32_BIT 0x100
#define DEPTH_BIT_MASK_24_BIT 0x200
#define DEPTH_BIT_MASK_16_BIT 0x400
#define DEPTH_BIT_MASK_8_BIT 0x800

#define INVALID_TEXTURE_FORMAT_COUNT (-1)
#define INVALID_TEXTURE_FORMAT_INDEX (-1)

#define MAX_TEXTURE_FORMAT_COUNT 128 /* ORIGINAL: 14 */
#define MAX_USABLE_TEXTURE_FORMAT_COUNT 14

#define MAX_TEXTURE_PALETTE_COLOR_COUNT 256

#define MAX_INPUT_FOG_ALPHA_COUNT 64
#define MAX_OUTPUT_FOG_ALPHA_VALUE 255
#define MAX_OUTPUT_FOG_ALPHA_COUNT 256

#define DEFAULT_FOG_DINSITY (0.5f)
#define DEFAULT_FOG_COLOR 0x00FFFFFF
#define DEFAULT_FOG_START (0.0f)
#define DEFAULT_FOG_END (1.0f)

#if !defined(__WATCOMC__) && _MSC_VER <= 1200
inline void LOGERROR(...) { }
inline void LOGWARNING(...) { }
inline void LOGMESSAGE(...) { }
#else
#define LOGERROR(...) Message(RENDERER_MODULE_MESSAGE_SEVERITY_ERROR, __VA_ARGS__)
#define LOGWARNING(...) Message(RENDERER_MODULE_MESSAGE_SEVERITY_WARNING, __VA_ARGS__)
#define LOGMESSAGE(...) Message(RENDERER_MODULE_MESSAGE_SEVERITY_MESSAGE, __VA_ARGS__)
#endif

namespace Renderer
{
    struct RendererTexture
    {
        u32 Width;
        u32 Height;

        u32 UnknownFormatIndexValue; // TODO
        s32 FormatIndex; // TODO
        s32 FormatIndexValue; // TODO

        void* Unk06; // TODO

        u32 MipMapCount;
        RendererTexture* Previous;
        u32 MemoryType;

        BOOL Unk10; // TODO

        IDirectDrawSurface4* Surface1;
        IDirect3DTexture2* Texture1;
        IDirectDrawSurface4* Surface2;
        IDirect3DTexture2* Texture2;
        IDirectDrawPalette* Palette;

        DDSURFACEDESC2 Descriptor;

        u32 Colors;
    };
}

namespace RendererModule
{
    struct TextureFormat
    {
        DDSURFACEDESC2 Descriptor;

        BOOL IsPalette;
        u32 RedBitCount;
        u32 BlueBitCount;
        u32 GreenBitCount;
        u32 PaletteColorBits;
        u32 AlphaBitCount;

        BOOL IsDXT;
        u32 DXT;
    };

    struct RendererModuleState
    {
        struct
        {
            HRESULT Code; // 0x60014970

            IDirectDrawClipper* Clipper; // 0x60014984
            IDirectDraw4* Instance; // 0x60014988

            IDirect3D3* DirectX; // 0x6001509c
            IDirect3DDevice3* Device; // 0x600150a0
            IDirect3DViewport3* ViewPort; // 0x600150a4
            IDirect3DMaterial3* Material; // 0x600150a8

            struct
            {
                BOOL IsSoft; // 0x60015074
                BOOL IsInit; // 0x60015078

                IDirectDraw4* Instance; // 0x60015080

                struct
                {
                    IDirectDrawSurface4* Main; // 0x60015084
                    IDirectDrawSurface4* Back; // 0x60015088
                    IDirectDrawSurface4* Depth; // 0x60015094

                    struct
                    {
                        IDirectDrawSurface4* Main; // 0x6001508c
                        IDirectDrawSurface4* Back; // 0x60015090
                        IDirectDrawSurface4* Depth; // 0x60015098
                    } Active;
                } Surfaces;
            } Active;

            struct
            {
                IDirectDrawSurface4* Main; // 0x6001498c
                IDirectDrawSurface4* Back; // 0x60014990

                IDirectDrawSurface4* Active[MAX_ACTIVE_SURFACE_COUNT]; // 0x60014994

                IDirectDrawSurface4* Window; // 0x600149b4
            } Surfaces;
        } DX;

        struct
        {
            struct
            {
                u32 Count; // 0x6001afc4

                u32 Small[MAX_RENDERER_SMALL_INDEX_COUNT]; // 0x60014368
                u16 Medium[MAX_RENDERER_MEDIUM_INDEX_COUNT]; // 0x60017080
                u16 Large[MAX_RENDERER_LARGE_INDEX_COUNT]; // 0x6001bc40
            } Indexes;

            struct
            {
                u32 Count; // 0x6001afc0
                Renderer::RTLVX Vertexes[MAX_RENDERER_VERTEX_COUNT]; // 0x600150e0
            } Vertexes;
        } Data;

        struct
        {
            GUID* Identifier; // 0x600149bc

            struct
            {
                BOOL IsAccelerated; // 0x6003bc60
                u32 DepthBits; // 0x6003bc64
                u32 RendererBits; // 0x6003bc68

                BOOL IsDepthAvailable; // 0x6003bc70

                BOOL IsDither; // 0x6003bc78
                BOOL IsWBuffer; // 0x6003bc7c
                BOOL IsWindowMode; // 0x6003bc80
                BOOL IsDepthComparisonAvailable; // 0x6003bc84
                BOOL IsStripplingAvailable; // 0x6003bc88
                BOOL IsPerspectiveTextures; // 0x6003bc8c
                BOOL IsAlphaFlatBlending; // 0x6003bc90
                BOOL IsAlphaProperBlending; // 0x6003bc94
                BOOL IsAlphaTextures; // 0x6003bc98
                BOOL IsModulateBlending; // 0x6003bc9c
                BOOL IsSourceAlphaBlending; // 0x6003bca0
                u32 AntiAliasing; // 0x6003bca4
                BOOL IsColorBlending; // 0x6003bca8
                BOOL IsSpecularBlending; // 0x6003bcac
            } Capabilities;
        } Device;

        struct
        {
            u32 Count; // 0x600149c0

            GUID* Indexes[MAX_RENDERER_DEVICE_COUNT]; // 0x60014768
            GUID Identifiers[MAX_RENDERER_DEVICE_COUNT]; // 0x60014790
            char Names[MAX_RENDERER_DEVICE_COUNT][MAX_DEVICE_NAME_LENGTH]; // 0x60014830
        } Devices;

        struct
        {
            RENDERERMODULELOGLAMBDA Log; // 0x600150b8

            RENDERERMODULESELECTSTATELAMBDA SelectState; // 0x600150c8
            RENDERERMODULEALLOCATEMEMORYLAMBDA AllocateMemory; // 0x600150cc
            RENDERERMODULERELEASEMEMORYLAMBDA ReleaseMemory; // 0x600150d0

            RendererModuleLambdaContainer Lambdas; // 0x6003bcc0
        } Lambdas;

        struct
        {
            BOOL IsActive; // 0x60014350
            IDirectDrawSurface4* Surface; // 0x60014354

            RendererModuleWindowLock State; // 0x60014338
        } Lock;

        struct
        {
            BOOL IsActive; // 0x6001434c
        } Scene;

        struct
        {
            BOOL IsWindowModeActive; // 0x60014b68

            u32 CooperativeLevel; // 0x60014974
            BOOL IsWindowMode; // 0x60014978

            u32 MaxAvailableMemory; // 0x60014980

            BOOL IsFogActive; // 0x60014b5c

            u32 Cull; // 0x600150d4
        } Settings;

        HANDLE Mutex; // 0x600149b8

        struct
        {
            Renderer::RendererTexture* Current; // 0x60014b6c

            BOOL Illegal; // 0x60014b70

            struct
            {
                u32 Count; // 0x6001b880

                TextureFormat Formats[MAX_TEXTURE_FORMAT_COUNT]; // 0x6001aff8
                s32 Indexes[MAX_USABLE_TEXTURE_FORMAT_COUNT]; // 0x6001bc00
            } Formats;
        } Textures;

        struct
        {
            u32 X0; // 0x60014358
            u32 Y0; // 0x6001435c
            u32 X1; // 0x60014360
            u32 Y1; // 0x60014364
        } ViewPort;

        struct
        {
            u32 Height; // 0x6003bc40
            u32 Width; // 0x6003bc44

            HWND HWND; // 0x6003bce0
        } Window;
    };

    extern RendererModuleState State;

    BOOL AcquireRendererDeviceDepthBufferNotEqualComparisonCapabilities(void);
    BOOL AcquireRendererDeviceState(void);
    BOOL AcquireRendererDeviceStripplingCapabilities(void);
    BOOL BeginRendererScene(void);
    BOOL CALLBACK EnumerateRendererDevices(GUID* uid, LPSTR name, LPSTR description, LPVOID context);
    BOOL InitializeRendererDeviceDepthSurfaces(const u32 width, const u32 height);
    BOOL RenderLines(Renderer::RTLVX* vertexes, const u32 count);
    BOOL RenderPoints(Renderer::RTLVX* vertexes, const u32 count);
    BOOL RenderTriangleFans(Renderer::RTLVX* vertexes, const u32 vertexCount, const u32 indexCount, const u32* indexes);
    BOOL RenderTriangleStrips(Renderer::RTLVX* vertexes, const u32 vertexCount, const u32 indexCount, const u32* indexes);
    BOOL RestoreRendererSurfaces(void);
    BOOL RestoreRendererTextures(void);
    BOOL SelectRendererState(const D3DRENDERSTATETYPE type, const DWORD value);
    BOOL SelectRendererTexture(Renderer::RendererTexture* tex);
    BOOL SelectRendererTextureStage(const DWORD stage, const D3DTEXTURESTAGESTATETYPE type, const DWORD value);
    BOOL UpdateRendererTexture(Renderer::RendererTexture* tex, const u32* pixels, const u32* palette);
    HRESULT CALLBACK EnumerateRendererDeviceModes(LPDDSURFACEDESC2 desc, LPVOID context);
    HRESULT CALLBACK EnumerateRendererDevicePixelFormats(LPDDPIXELFORMAT format, LPVOID context);
    HRESULT CALLBACK EnumerateRendererDeviceTextureFormats(LPDDPIXELFORMAT format, LPVOID context);
    inline f32 AcquireNormal(const f32x3* a, const f32x3* b, const f32x3* c) { return (b->X - a->X) * (c->Y - a->Y) - (c->X - a->X) * (b->Y - a->Y); }
    inline u32 AcquireFogAlphaIndex(const f32 value) { return (u32)((1.0f - value * 0.000015259022f) * 255.0f + 0.5f); }
    Renderer::RendererTexture* AllocateRendererTexture(const u32 size);
    Renderer::RendererTexture* InitializeRendererTexture(void);
    s32 AcquireRendererDeviceTextureFormatIndex(const u32 palette, const u32 alpha, const u32 red, const u32 green, const u32 blue, const BOOL dxt);
    s32 InitializeRendererTextureDetails(Renderer::RendererTexture* tex);
    u32 AcquirePixelFormat(const DDPIXELFORMAT* format);
    u32 AcquireRendererDeviceCount(void);
    u32 ClearRendererViewPort(const u32 x0, const u32 y0, const u32 x1, const u32 y1);
    u32 DisposeRendererTexture(Renderer::RendererTexture* tex);
    u32 EndRendererScene(void);
    u32 InitializeRendererDevice(void);
    u32 InitializeRendererDeviceAcceleration(void);
    u32 InitializeRendererDeviceLambdas(void);
    u32 ReleaseRendererDeviceInstance(void);
    u32 ReleaseRendererWindow(void);
    u32 RendererRenderScene(void);
    u32 SelectBasicRendererState(const u32 state, void* value);
    u32 SelectRendererTransforms(const f32 zNear, const f32 zFar);
    u32 STDCALLAPI InitializeRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result); // TODO
    u32 STDCALLAPI InitializeRendererDeviceSurfacesExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result); // TODO
    u32 STDCALLAPI ReleaseRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result); // TODO
    u32 ToggleRenderer(void);
    void AcquireRendererDeviceTextureFormats(void);
    void AcquireWindowModeCapabilities(void);
    void AttemptRenderScene(void);
    void InitializeConcreteRendererDevice(void);
    void InitializeRendererModuleState(const u32 pending, const u32 depth);
    void InitializeRendererState(void);
    void InitializeVertex(Renderer::RTLVX* dst, const Renderer::RTLVX* src);
    void InitializeViewPort(void);
    void Message(const u32 severity, const char* format, ...);
    void ReleaseRendererDevice(void);
    void ReleaseRendererDeviceSurfaces(void);
    void ReleaseRendererTexture(Renderer::RendererTexture* tex);
    void RenderQuad(Renderer::RTLVX* a, Renderer::RTLVX* b, Renderer::RTLVX* c, Renderer::RTLVX* d);
    void RenderQuadMesh(Renderer::RTLVX* vertexes, const u32* indexes, const u32 count);
    void RenderTriangle(Renderer::RTLVX* a, Renderer::RTLVX* b, Renderer::RTLVX* c);
    void RenderTriangleMesh(Renderer::RTLVX* vertexes, const u32* indexes, const u32 count);
    void SelectRendererDevice(void);
    void SelectRendererDeviceType(const u32 type);
    void SelectRendererFogAlphas(const u8* input, u8* output);
    void SelectRendererMaterial(const f32 r, const f32 g, const f32 b);
}