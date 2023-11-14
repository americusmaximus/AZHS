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

#define DEFAULT_RENDERER_VERTEX_COUNT 253
#define MAX_RENDERER_VERTEX_COUNT 8096
#define MAX_RENDERER_INDEX_COUNT 16192
#define MAX_RENDERER_LARGE_INDEX_COUNT 65536

#define RENDERER_MODULE_ENVIRONMENT_SECTION_NAME "DX6"

#define MAX_ENUMERATE_RENDERER_DEVICE_COUNT 16

#define MAX_ENUMERATE_RENDERER_DEVICE_NAME_LENGTH 80

#define MAX_ACTIVE_SURFACE_COUNT 8

#define MIN_RENDERER_DEVICE_AVAIABLE_VIDEO_MEMORY (16 * 1024 * 1024) /* ORIGINAL: 0x20000 (2 MB) */

#define DEPTH_BIT_MASK_32_BIT 0x100
#define DEPTH_BIT_MASK_24_BIT 0x200
#define DEPTH_BIT_MASK_16_BIT 0x400
#define DEPTH_BIT_MASK_8_BIT 0x800

#define INVALID_TEXTURE_FORMAT_COUNT (-1)
#define INVALID_TEXTURE_FORMAT_INDEX (-1)

#define MAX_TEXTURE_DEPTH_FORMAT_COUNT 16 /* ORIGINAL: 6 */

#define INVALID_RENDERER_DEVICE_CAPABILITIES_MODE_INDEX (-1)
#define MAX_RENDERER_DEVICE_CAPABILITIES_MODE_SCORE 10000

#define MAX_TEXTURE_FORMAT_COUNT 128 /* ORIGINAL: 14 */
#define MAX_USABLE_TEXTURE_FORMAT_COUNT 14

#define MAX_TEXTURE_PALETTE_COLOR_COUNT 256
#define MAX_FOG_ALPHA_COUNT 256

#define DEFAULT_FOG_DINSITY (1.0f)
#define DEFAULT_FOG_COLOR 0x00FF0000
#define DEFAULT_FOG_START (0.0f)
#define DEFAULT_FOG_END (1.0f)

#if _MSC_VER <= 1200
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
        u32 Stage;
        RendererTexture* Previous;
        u32 MemoryType; // TODO definitions for 0, 1, 2
        s32 Unk11; // TODO
        IDirectDrawSurface4* Surface1; // TODO
        IDirect3DTexture2* Texture1; // TODO
        IDirectDrawSurface4* Surface2; // TODO
        IDirect3DTexture2* Texture2; // TODO
        IDirectDrawPalette* Palette;
        DDSURFACEDESC2 Descriptor;
        u32 Colors;
    };
}

namespace RendererModule
{
    extern u32 DAT_6001d030; // 0x6001d030 // TODO
    extern u32 DAT_6001d5b0; // 0x6001d5b0 // TODO

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
            HRESULT Code; // 0x6001ce30

            IDirectDrawClipper* Clipper; // 0x6001ce48
            IDirectDrawGammaControl* GammaControl; // 0x6001ce4c
            IDirectDraw4* Instance; // 0x6001ce50

            IDirect3D3* DirectX; // 0x6001d578
            IDirect3DDevice3* Device; // 0x6001d57c
            IDirect3DViewport3* ViewPort; // 0x6001d580
            IDirect3DMaterial3* Material; // 0x6001d584

            struct
            {
                BOOL IsSoft; // 0x6001d54c
                BOOL IsInit; // 0x6001d550

                IDirectDraw4* Instance; // 0x6001d55c

                struct
                {
                    IDirectDrawSurface4* Main; // 0x6001d560
                    IDirectDrawSurface4* Back; // 0x6001d564
                    IDirectDrawSurface4* Depth; // 0x6001d570

                    struct
                    {
                        IDirectDrawSurface4* Main; // 0x6001d568
                        IDirectDrawSurface4* Back; // 0x6001d56c
                        IDirectDrawSurface4* Depth; // 0x6001d574
                    } Active;
                } Surfaces;
            } Active;

            struct
            {
                IDirectDrawSurface4* Main; // 0x6001ce54
                IDirectDrawSurface4* Back; // 0x6001ce58

                IDirectDrawSurface4* Active[MAX_ACTIVE_SURFACE_COUNT]; // 0x6001ce5c

                IDirectDrawSurface4* Window; // 0x6001ce7c
            } Surfaces;
        } DX;

        struct
        {
            struct
            {
                u32 Count; // 0x6002d2e0
                u16 Indexes[MAX_RENDERER_INDEX_COUNT]; // 0x60025458

                u16 Large[MAX_RENDERER_LARGE_INDEX_COUNT]; // 0x6002e5c0
            } Indexes;

            struct
            {
                u32 Count; // 0x6002d2dc
                IDirect3DVertexBuffer* Buffer; // 0x6002d2d8
                u32 Vertexes[MAX_RENDERER_VERTEX_COUNT]; // 0x6001d5d8
            } Vertexes;
        } Data;

        struct
        {
            GUID* Identifier; // 0x6001ce84

            char Name[MAX_ENUMERATE_RENDERER_DEVICE_NAME_LENGTH]; // 0x6004ed60

            struct
            {
                BOOL IsAccelerated; // 0x6004e5e0
                u32 DepthBits; // 0x6004e5e4
                u32 RendererBits; // 0x6004e5e8

                BOOL IsDepthAvailable; // 0x6004e5f0

                BOOL IsDither; // 0x6004e5fc
                BOOL IsWBuffer; // 0x6004e600
                BOOL IsWindowMode; // 0x6004e604
                BOOL IsDepthComparisonAvailable; // 0x6004e608
                BOOL IsStripplingAvailable; // 0x6004e60c
                BOOL IsPerspectiveTextures; // 0x6004e610
                BOOL IsAlphaFlatBlending; // 0x6004e614
                BOOL IsAlphaProperBlending; // 0x6004e618
                BOOL IsAlphaTextures; // 0x6004e61c
                BOOL IsModulateBlending; // 0x6004e620
                BOOL IsSourceAlphaBlending; // 0x6004e624
                u32 AntiAliasing; // 0x6004e628
                BOOL IsColorBlending; // 0x6004e62c
                BOOL IsAnisotropyAvailable; // 0x6004e630
                BOOL IsPimaryGammaAvailable; // 0x6004e634
                BOOL IsStencilBuffer; // 0x6004e638
                BOOL IsSpecularBlending; // 0x6004e63c

                BOOL IsMipMapBiasAvailable; // 0x6004e644
                u32 MinimumTextureWidth; // 0x6004e648
                u32 MaximumTextureWidth; // 0x6004e64c
                u32 MinimumTextureHeight; // 0x6004e650
                u32 MaximumTextureHeight; // 0x6004e654
                u32 MultipleTextureWidth; // 0x6004e658
                BOOL IsPowerOfTwoTexturesWidth; // 0x6004e65c
                u32 MultipleTextureHeight; // 0x6004e660
                BOOL IsPowerOfTwoTexturesHeight; // 0x6004e664
                u32 MaximumSimultaneousTextures; // 0x6004e668
                BOOL IsSquareOnlyTextures; // 0x6004e66c
                BOOL IsAntiAliasEdges; // 0x6004e670
            } Capabilities;
        } Device;

        struct
        {
            u32 Count; // 0x6001ce8c

            GUID* Indexes[MAX_ENUMERATE_RENDERER_DEVICE_COUNT]; // 0x6001ca00

            struct
            {
                u32 Count; // 0x6002d4b8
                BOOL IsAvailable; // 0x6002d4bc

                char Names[MAX_ENUMERATE_RENDERER_DEVICE_COUNT][MAX_ENUMERATE_RENDERER_DEVICE_NAME_LENGTH]; // 0x6001caf0

                struct
                {
                    GUID* Indexes[MAX_ENUMERATE_RENDERER_DEVICE_COUNT]; // 0x6002d2f8
                    GUID Identifiers[MAX_ENUMERATE_RENDERER_DEVICE_COUNT]; // 0x6002d338
                } Identifiers;

                struct
                {
                    HMONITOR* Indexes[MAX_ENUMERATE_RENDERER_DEVICE_COUNT]; // 0x6002d438
                    HMONITOR Monitors[MAX_ENUMERATE_RENDERER_DEVICE_COUNT]; // 0x6002d478
                } Monitors;

                DDDEVICEIDENTIFIER Identifier; // 0x6002d4c8
            } Enumeration;
        } Devices;

        struct
        {
            // NOTE: The original renderer has a default log function that just exits the process.
            RENDERERMODULELOGLAMBDA Log; // 0x600190d8

            RENDERERMODULESELECTSTATELAMBDA SelectState; // 0x6001d5c8
            RENDERERMODULEALLOCATEMEMORYLAMBDA AllocateMemory; // 0x6001d5cc
            RENDERERMODULERELEASEMEMORYLAMBDA ReleaseMemory; // 0x6001d5d0

            RendererModuleLambdaContainer Lambdas; // 0x6004eca0
        } Lambdas;

        struct
        {
            BOOL IsActive; // 0x6001c9e0
            IDirectDrawSurface4* Surface; // 0x6001c9e4

            RendererModuleWindowLock State; // 0x6001c9c8
        } Lock;

        struct
        {
            BOOL IsActive; // 0x6001c9dc
        } Scene;

        struct
        {
            BOOL IsWindowModeActive; // 0x6001d040

            u32 CooperativeLevel; // 0x6001ce34
            BOOL IsWindowMode; // 0x6001ce38

            u32 MaxAvailableMemory; // 0x6001ce40

            DDGAMMARAMP GammaControl; // 0x6004e680

            BOOL IsFogActive; // 0x6001d034

            u32 Cull; // 0x6001d5d4
        } Settings;

        HANDLE Mutex; // 0x6001ce80

        struct
        {
            BOOL IsInactive; // 0x6001ce44
        } State;

        struct
        {
            Renderer::RendererTexture* Current; // 0x6001d044
            u32 Count; // 0x6001d048
            BOOL Illegal; // 0x6001d04c

            struct
            {
                u32 Count; // 0x6002e180

                TextureFormat Formats[MAX_TEXTURE_FORMAT_COUNT]; // 0x6002d8f8
                s32 Indexes[MAX_USABLE_TEXTURE_FORMAT_COUNT]; // 0x6002e520

                struct
                {
                    u32 Formats[MAX_TEXTURE_DEPTH_FORMAT_COUNT]; // 0x6004ec80
                    u32 Count; // 0x6001ce94
                } Depth;
            } Formats;
        } Textures;

        struct
        {
            u32 X0; // 0x6001c9e8
            u32 Y0; // 0x6001c9ec
            u32 X1; // 0x6001c9f0
            u32 Y1; // 0x6001c9f4
        } ViewPort;

        struct
        {
            u32 Height; // 0x6004e5c0
            u32 Width; // 0x6004e5c4

            HWND HWND; // 0x6004ecc0

            u32 Bits; // 0x6001d558
        } Window;
    };

    extern RendererModuleState State;

    BOOL AcquireRendererDeviceAccelerationState(const u32 indx);
    BOOL AcquireRendererDeviceDepthBufferNotEqualComparisonCapabilities(void);
    BOOL AcquireRendererDeviceState(void);
    BOOL AcquireRendererDeviceStripplingCapabilities(void);
    BOOL CALLBACK EnumerateDirectDrawDevices(GUID* uid, LPSTR name, LPSTR description, LPVOID context, HMONITOR monitor);
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
    BOOL UpdateRendererTexture(Renderer::RendererTexture* tex, const u32* pixels);
    BOOL UpdateRendererTexture(Renderer::RendererTexture* tex, const u32* pixels, const u32* palette);
    const char* AcquireRendererMessage(const HRESULT code);
    const char* AcquireRendererMessageDescription(const HRESULT code);
    HRESULT CALLBACK EnumerateRendererDeviceDepthPixelFormats(LPDDPIXELFORMAT format, LPVOID context);
    HRESULT CALLBACK EnumerateRendererDeviceModes(LPDDSURFACEDESC2 desc, LPVOID context);
    HRESULT CALLBACK EnumerateRendererDevicePixelFormats(LPDDPIXELFORMAT format, LPVOID context);
    HRESULT CALLBACK EnumerateRendererDeviceTextureFormats(LPDDPIXELFORMAT format, LPVOID context);
    inline f32 AcquireNormal(const f32x3* a, const f32x3* b, const f32x3* c) { return (b->X - a->X) * (c->Y - a->Y) - (c->X - a->X) * (b->Y - a->Y); }
    inline u32 AcquireFogAlphaIndex(const f32 value) { return (u32)((1.0f - value * 0.000015259022f) * 255.0f + 0.5f); }
    Renderer::RendererTexture* AllocateRendererTexture(const u32 size);
    Renderer::RendererTexture* InitializeRendererTexture(void);
    s32 AcquireRendererDeviceModeIndex(const u32 width, const u32 height, const u32 bpp);
    s32 AcquireRendererDeviceModeScore(const RendererModuleDescriptorDeviceCapabilities* caps, const u32 width, const u32 height, const u32 bpp);
    s32 AcquireRendererDeviceTextureFormatIndex(const u32 palette, const u32 alpha, const u32 red, const u32 green, const u32 blue, const BOOL dxt);
    s32 AcquireSettingsValue(const s32 value, const char* section, const char* name);
    s32 InitializeRendererTextureDetails(Renderer::RendererTexture* tex);
    u32 AcquireDirectDrawDeviceCount(GUID** uids, HMONITOR** monitors, const char* section);
    u32 AcquirePixelFormat(const DDPIXELFORMAT* format);
    u32 AcquireRendererDeviceCount(void);
    u32 BeginRendererScene(void);
    u32 ClearRendererViewPort(const u32 x0, const u32 y0, const u32 x1, const u32 y1);
    u32 DisposeRendererTexture(Renderer::RendererTexture* tex);
    u32 EndRendererScene(void);
    u32 InitializeRendererDevice(void);
    u32 InitializeRendererDeviceAcceleration(void);
    u32 InitializeRendererDeviceLambdas(void);
    u32 InitializeRendererVertexBuffer(const u32 type, const u32 count);
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
    void AcquireRendererModuleDescriptor(RendererModuleDescriptor* desc, const char* section);
    void AcquireWindowModeCapabilities(void);
    void AssignRendererDeviceModeIndex(const s32 src, const u32 dst);
    void AttemptRenderScene(void);
    void InitializeConcreteRendererDevice(void);
    void InitializeRendererModuleState(const u32 mode, const u32 pending, const u32 depth, const char* section);
    void InitializeRendererState(void);
    void InitializeRenderState55(void); // TODO
    void InitializeVertex(Renderer::RTLVX* dst, const Renderer::RTLVX* src);
    void InitializeViewPort(void);
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
    void SelectRendererFogAlphas(const u32 value, u8* alphas);
    void SelectRendererMaterial(const f32 r, const f32 g, const f32 b);
    void SelectRendererVertexCount(void);
}