/*
Copyright (c) 2023 - 2024 Americus Maximus

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
#include "Module.hxx"
#include "RendererValues.hxx"
#include "Settings.hxx"

using namespace Renderer;
using namespace RendererModuleValues;
using namespace Settings;

namespace RendererModule
{
    // 0x60001000
    // a.k.a. THRASH_about
    DLLAPI RendererModuleDescriptor* STDCALLAPI AcquireDescriptor(void)
    {
        ModuleDescriptor.Version = RendererVersion;
        ModuleDescriptor.MaximumTextureWidth = 256;
        ModuleDescriptor.MaximumTextureHeight = 256;
        ModuleDescriptor.DXV = RENDERER_MODULE_VERSION_DX6; // ORIGINAL: 4

        ModuleDescriptor.Caps = (State.DX.Active.IsSoft & 1) << 4
            | (RENDERER_MODULE_CAPS_WINDOWED | RENDERER_MODULE_CAPS_TEXTURE_HEIGHT_POW2 | RENDERER_MODULE_CAPS_TEXTURE_WIDTH_POW2 | RENDERER_MODULE_CAPS_TEXTURE_SQUARE | RENDERER_MODULE_CAPS_LINE_WIDTH);

        ModuleDescriptor.Author = RENDERER_MODULE_AUTHOR;
        ModuleDescriptor.Signature = RENDERER_MODULE_SIGNATURE_D3D6;
        ModuleDescriptor.Size = sizeof(RendererModuleDescriptor);
        ModuleDescriptor.MinimumTextureWidth = 8;
        ModuleDescriptor.MultipleTextureWidth = 1;
        ModuleDescriptor.MinimumTextureHeight = 8;
        ModuleDescriptor.MultipleTextureHeight = 1;
        ModuleDescriptor.ClipAlign = 0;

        ModuleDescriptor.ActiveTextureFormatStatesCount = MAX_ACTIVE_USABLE_TEXTURE_FORMAT_COUNT;
        ModuleDescriptor.TextureFormatStates = RendererTextureFormatStates;

        ModuleDescriptor.ActiveUnknownValuesCount = MAX_ACTIVE_UNKNOWN_COUNT;
        ModuleDescriptor.UnknownValues = UnknownArray06;

        ModuleDescriptor.Capabilities.Capabilities = ModuleDescriptorDeviceCapabilities;

        strcpy(ModuleDescriptor.Name, RENDERER_MODULE_NAME);

        SelectRendererDevice();

        return &ModuleDescriptor;
    }

    // 0x60001150
    // a.k.a. THRASH_clearwindow
    DLLAPI u32 STDCALLAPI ClearGameWindow(void)
    {
        return ClearRendererViewPort(State.ViewPort.X0, State.ViewPort.Y0, State.ViewPort.X1, State.ViewPort.Y1);
    }

    // 0x600011f0
    // a.k.a. THRASH_clip
    DLLAPI u32 STDCALLAPI ClipGameWindow(const u32 x0, const u32 y0, const u32 x1, const u32 y1)
    {
        State.ViewPort.X0 = x0;
        State.ViewPort.Y0 = y0;
        State.ViewPort.X1 = x1;
        State.ViewPort.Y1 = y1;

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x600015f0
    // a.k.a. THRASH_drawline
    DLLAPI void STDCALLAPI DrawLine(RVX* a, RVX* b)
    {
        RTLVX vertexes[2];

        InitializeVertex(&vertexes[0], (RTLVX*)a);
        InitializeVertex(&vertexes[1], (RTLVX*)b);

        RenderLines(vertexes, 2);
    }

    // 0x60001630
    // a.k.a. THRASH_drawlinemesh
    DLLAPI void STDCALLAPI DrawLineMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawLine((RVX*)&vs[indexes[x * 2 + 0]], (RVX*)&vs[indexes[x * 2 + 1]]); }
    }

    // 0x600017a0
    // a.k.a. THRASH_drawlinestrip
    DLLAPI void STDCALLAPI DrawLineStrip(const u32 count, RVX* vertexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawLine((RVX*)&vs[x + 0], (RVX*)&vs[x + 1]); }
    }

    // 0z60001670
    // a.k.a. THRASH_drawlinestrip
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawLineStrips(const u32 count, RVX* vertexes, const u32* indexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawLine((RVX*)&vs[indexes[x + 0]], (RVX*)&vs[indexes[x + 1]]); }
    }

    // 0x600016b0
    // a.k.a. THRASH_drawpoint
    DLLAPI void STDCALLAPI DrawPoint(RVX* vertex)
    {
        RenderPoints((RTLVX*)vertex, 1);
    }

    // 0x600016d0
    // a.k.a. THRASH_drawpointmesh
    DLLAPI void STDCALLAPI DrawPointMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawPoint((RVX*)&vs[indexes[x]]); }
    }

    // 0x600017d0
    // a.k.a. THRASH_drawpointstrip
    DLLAPI void STDCALLAPI DrawPointStrip(const u32 count, RVX* vertexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawPoint((RVX*)&vs[x]); }
    }

    // 0x60001570
    // a.k.a. THRASH_drawquad
    DLLAPI void STDCALLAPI DrawQuad(RVX* a, RVX* b, RVX* c, RVX* d)
    {
        if ((AcquireNormal((f32x3*)a, (f32x3*)b, (f32x3*)c) & RENDERER_CULL_MODE_COUNTER_CLOCK_WISE) != State.Settings.Cull) { RenderQuad((RTLVX*)a, (RTLVX*)b, (RTLVX*)c, (RTLVX*)d); }
    }

    // 0x600015d0
    // a.k.a. THRASH_drawquadmesh
    DLLAPI void STDCALLAPI DrawQuadMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        RenderQuadMesh((RTLVX*)vertexes, indexes, count);
    }

    // 0x60001480
    // a.k.a. THRASH_drawtri
    DLLAPI void STDCALLAPI DrawTriangle(RVX* a, RVX* b, RVX* c)
    {
        if ((AcquireNormal((f32x3*)a, (f32x3*)b, (f32x3*)c) & RENDERER_CULL_MODE_COUNTER_CLOCK_WISE) != State.Settings.Cull) { RenderTriangle((RTLVX*)a, (RTLVX*)b, (RTLVX*)c); }
    }

    // 0x60001760
    // a.k.a. THRASH_drawtrifan
    DLLAPI void STDCALLAPI DrawTriangleFan(const u32 count, RVX* vertexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawTriangle((RVX*)&vs[0], (RVX*)&vs[x + 1], (RVX*)&vs[x + 2]); }
    }

    // 0x60001510
    // a.k.a. THRASH_drawtrifan
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawTriangleFans(const u32 count, RVX* vertexes, const u32* indexes)
    {
        for (u32 x = 0, i = 1; x < (count * 3 + 2) / 3; x = x + 3, i = i + 2)
        {
            State.Data.Indexes.Small[x + 0] = 0;
            State.Data.Indexes.Small[x + 1] = i + 0;
            State.Data.Indexes.Small[x + 2] = i + 1;
        }

        RenderTriangleFans((RTLVX*)vertexes, count + 2, count * 3, State.Data.Indexes.Small);
    }

    // 0x600014d0
    // a.k.a. THRASH_drawtrimesh
    DLLAPI void STDCALLAPI DrawTriangleMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        RenderTriangleMesh((RTLVX*)vertexes, indexes, count);
    }

    // 0x60001700
    // a.k.a. THRASH_drawtristrip
    // NOTE: Triangle strip vertex order: 0 1 2, 1 3 2, 2 3 4, 3 5 4, 4 5 6, ...
    DLLAPI void STDCALLAPI DrawTriangleStrip(const u32 count, RVX* vertexes)
    {
        if (count == 0) { return; }

        const RTLVX* vs = (RTLVX*)vertexes;

        DrawTriangle((RVX*)&vs[0], (RVX*)&vs[1], (RVX*)&vs[2]);

        for (u32 x = 1; x < count; x = x + 2)
        {
            DrawTriangle((RVX*)&vs[x + 0], (RVX*)&vs[x + 2], (RVX*)&vs[x + 1]);

            if ((x + 1) < count) { DrawTriangle((RVX*)&vs[x + 1], (RVX*)&vs[x + 2], (RVX*)&vs[x + 3]); }
        }
    }

    // 0x600014f0
    // a.k.a. THRASH_drawtristrip
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawTriangleStrips(const u32 count, RVX* vertexes, const u32* indexes)
    {
        RenderTriangleStrips((RTLVX*)vertexes, count + 2, count, indexes);
    }

    // 0x60001180
    // a.k.a. THRASH_flushwindow
    DLLAPI u32 STDCALLAPI FlushGameWindow(void)
    {
        return EndRendererScene();
    }

    // 0x600011d0
    // a.k.a. THRASH_idle
    DLLAPI void STDCALLAPI Idle(void) { }

    // 0x60002740
    // a.k.a. THRASH_init
    DLLAPI u32 STDCALLAPI Init(void)
    {
        InitializeSettings();

        AcquireRendererDeviceCount();

        return State.Devices.Count;
    }

    // 0x60002ea0
    // a.k.a. THRASH_is
    DLLAPI u32 STDCALLAPI Is(void)
    {
        const HWND hwnd = GetDesktopWindow();
        const HDC hdc = GetWindowDC(hwnd);

        if ((GRAPHICS_BITS_PER_PIXEL_8 - 1) < GetDeviceCaps(hdc, BITSPIXEL))
        {
            IDirectDraw* instance = NULL;
            if (DirectDrawCreate(NULL, &instance, NULL) == DD_OK)
            {
                IDirectDraw2* dd = NULL;
                HRESULT result = instance->QueryInterface(IID_IDirectDraw2, (void**)&dd);

                instance->Release();

                if (result == DD_OK)
                {
                    IDirect3D2* dx = NULL;
                    result = dd->QueryInterface(IID_IDirect3D2, (void**)&dx);

                    dd->Release();

                    if (result == DD_OK && dx != NULL)
                    {
                        dx->Release();

                        return RENDERER_MODULE_DX6_ACCELERATION_AVAILABLE;
                    }
                }
            }
        }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001240
    // a.k.a. THRASH_lockwindow
    DLLAPI RendererModuleWindowLock* STDCALLAPI LockGameWindow(void)
    {
        if (State.DX.Surfaces.Window != NULL)
        {
            if (State.Lock.IsActive) { LOGERROR("D3D lock called while locked\n"); }

            if (State.Scene.IsActive) { EndRendererScene(); }

            if (State.Lambdas.Lambdas.LockWindow != NULL) { State.Lambdas.Lambdas.LockWindow(TRUE); }

            State.Lock.Surface = State.DX.Surfaces.Window;

            DDSURFACEDESC2 desc;
            ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

            desc.dwSize = sizeof(DDSURFACEDESC2);

            State.DX.Code = State.DX.Surfaces.Window->Lock(NULL, &desc, DDLOCK_WAIT, NULL);

            if (State.DX.Code != DD_OK) { return NULL; }

            if (State.Settings.IsWindowMode)
            {
                GetClientRect(State.Window.HWND, &State.Lock.State.Rect);

                POINT point;
                ZeroMemory(&point, sizeof(POINT));

                ClientToScreen(State.Window.HWND, &point);

                OffsetRect(&State.Lock.State.Rect, point.x, point.y);

                if (State.DX.Surfaces.Window == State.DX.Surfaces.Active[1]) // TODO
                {
                    desc.lpSurface = (void*)((addr)desc.lpSurface + (addr)(State.Lock.State.Rect.left * 2 + desc.lPitch * State.Lock.State.Rect.top));
                }
            }

            State.Lock.State.Stride = desc.lPitch;
            State.Lock.State.Height = State.Window.Height;
            State.Lock.State.Width = State.Window.Width;

            if (State.Lock.Surface == State.DX.Active.Surfaces.Active.Depth)
            {
                State.Lock.State.Format = desc.ddpfPixelFormat.dwRGBBitCount; // TODO
            }
            else if (desc.ddpfPixelFormat.dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_16)
            {
                State.Lock.State.Format = (desc.ddpfPixelFormat.dwGBitMask == 0x7e0)
                    ? RENDERER_PIXEL_FORMAT_R5G6B5
                    : RENDERER_PIXEL_FORMAT_A1R5G5B5;
            }
            else if (desc.ddpfPixelFormat.dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_32)
            {
                State.Lock.State.Format = RENDERER_PIXEL_FORMAT_R8G8B8;
            }

            State.Lock.IsActive = TRUE;

            State.Lock.State.Data = desc.lpSurface;
        }

        return &State.Lock.State;
    }

    // 0x60001190
    // a.k.a. THRASH_pageflip
    DLLAPI u32 STDCALLAPI ToggleGameWindow(void)
    {
        if (State.Scene.IsActive) { EndRendererScene(); }

        if (State.Lock.IsActive) { LOGERROR("D3D pageflip called in while locked\n"); }

        return ToggleRenderer();
    }

    // 0x60002f60
    // a.k.a. THRASH_readrect
    DLLAPI u32 STDCALLAPI ReadRectangle(const u32 x, const u32 y, const u32 width, const u32 height, u32* pixels)
    {
        RendererModuleWindowLock* state = LockGameWindow();

        if (state == NULL) { return RENDERER_MODULE_FAILURE; }

        const u32 multiplier = state->Format == RENDERER_PIXEL_FORMAT_R8G8B8
            ? (GRAPHICS_BITS_PER_PIXEL_32 >> 3) : (GRAPHICS_BITS_PER_PIXEL_16 >> 3);

        const u32 length = multiplier * width;

        for (u32 xx = 0; xx < height; xx++)
        {
            const addr offset = (xx * state->Stride) + (state->Stride * y) + (multiplier * x);

            CopyMemory(&pixels[xx * length], (void*)((addr)state->Data + (addr)offset), length);
        }

        return UnlockGameWindow(state);
    }

    // 0x60002750
    // a.k.a. THRASH_restore
    DLLAPI u32 STDCALLAPI RestoreGameWindow(void)
    {
        if (State.Lock.IsActive) { UnlockGameWindow(NULL); }

        ReleaseRendererDevice();

        RendererDeviceIndex = INVALID_DEVICE_INDEX;

        if (State.Lambdas.Lambdas.AcquireWindow != NULL)
        {
            ReleaseRendererWindow();

            return RENDERER_MODULE_SUCCESS;
        }

        ReleaseRendererDeviceInstance();

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60001880
    // a.k.a. THRASH_selectdisplay
    DLLAPI u32 STDCALLAPI SelectDevice(const s32 indx)
    {
        State.Device.Identifier = NULL;

        if (State.DX.Instance != NULL) { RestoreGameWindow(); }

        const char* name = NULL;

        if (indx < DEFAULT_DEVICE_INDEX || State.Devices.Count <= indx)
        {
            RendererDeviceIndex = DEFAULT_DEVICE_INDEX;
            State.Device.Identifier = State.Devices.Indexes[DEFAULT_DEVICE_INDEX];
            name = State.Devices.Names[DEFAULT_DEVICE_INDEX];
        }
        else
        {
            RendererDeviceIndex = indx;
            State.Device.Identifier = State.Devices.Indexes[indx];
            name = State.Devices.Names[indx];
        }

        strcpy(ModuleDescriptor.Name, name);

        if (State.Lambdas.Lambdas.AcquireWindow == NULL)
        {
            InitializeRendererDevice();
        }
        else
        {
            InitializeRendererDeviceLambdas();
        }

        {
            const char* value = getenv(RENDERER_MODULE_DEVICE_TYPE_ENVIRONMENT_PROPERTY_NAME);

            if (value != NULL)
            {
                const s32 val = atoi(value);

                if (val != RENDERER_MODULE_DEVICE_TYPE_0_ACCELERATED)
                {
                    if (val == RENDERER_MODULE_DEVICE_TYPE_0_RGB)
                    {
                        SelectState(RENDERER_MODULE_STATE_SELECT_DEVICE_TYPE, (void*)RENDERER_MODULE_DEVICE_TYPE_0_RGB);
                    }

                    return RENDERER_MODULE_SUCCESS;
                }
            }
        }

        SelectState(RENDERER_MODULE_STATE_SELECT_DEVICE_TYPE, (void*)RENDERER_MODULE_DEVICE_TYPE_0_ACCELERATED);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60003310
    // a.k.a. THRASH_setstate
    DLLAPI u32 STDCALLAPI SelectState(const u32 state, void* value)
    {
        u32 result = RENDERER_MODULE_FAILURE;
        const u32 actual = state & RENDERER_MODULE_SELECT_STATE_MASK; // NOTE: The mask filter brought from the newer renderers.

        switch (actual)
        {
        case RENDERER_MODULE_STATE_NONE: { return RENDERER_MODULE_FAILURE; }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE:
        {
            SelectTexture((RendererTexture*)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_CULL_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_CULL_NONE:
            {
                SelectRendererState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);

                State.Settings.Cull = RENDERER_CULL_MODE_NONE;

                break;
            }
            case RENDERER_MODULE_CULL_COUNTER_CLOCK_WISE:
            {
                SelectRendererState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);

                State.Settings.Cull = RENDERER_CULL_MODE_COUNTER_CLOCK_WISE;

                break;
            }
            case RENDERER_MODULE_CULL_CLOCK_WISE:
            {
                SelectRendererState(D3DRENDERSTATE_CULLMODE, D3DCULL_CW);

                State.Settings.Cull = RENDERER_CULL_MODE_CLOCK_WISE;

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_MATERIAL:
        {
            const u32 color = (u32)value;

            const f32 r = RGBA_GETRED(color) / 255.0f;
            const f32 g = RGBA_GETGREEN(color) / 255.0f;
            const f32 b = RGBA_GETBLUE(color) / 255.0f;

            SelectRendererMaterial(r, g, b);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_DEPTH_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_DEPTH_INACTIVE:
            {
                SelectRendererState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);
                SelectRendererState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
                SelectRendererState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);

                break;
            }
            case RENDERER_MODULE_DEPTH_ACTIVE:
            {
                SelectRendererState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
                SelectRendererState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
                SelectRendererState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);

                break;
            }
            case RENDERER_MODULE_DEPTH_ACTIVE_W:
            {
                if (State.Device.Capabilities.IsWBufferAvailable)
                {
                    SelectRendererState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
                    SelectRendererState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
                    SelectRendererState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
                    SelectRendererState(D3DRENDERSTATE_ZENABLE, D3DZB_USEW);

                    SelectRendererTransforms(0.0f, 65000.0f);
                }
                else
                {
                    SelectRendererState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
                    SelectRendererState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
                    SelectRendererState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);

                    return RENDERER_MODULE_FAILURE;
                }

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_DITHER_STATE:
        {
            SelectRendererState(D3DRENDERSTATE_DITHERENABLE, ((u32)value) != 0 ? TRUE : FALSE);

            if (!State.Device.Capabilities.IsDitherAvailable) { return RENDERER_MODULE_FAILURE; }

            result = State.Device.Capabilities.IsDitherAvailable; break;
        }
        case RENDERER_MODULE_STATE_SELECT_SHADE_STATE:
        {
            if (SettingsState.FlatShading)
            {
                if ((u32)value == RENDERER_MODULE_SHADE_FLAT)
                {
                    SelectRendererState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);

                    RendererShadeMode = RENDERER_MODULE_SHADE_FLAT;
                }
                else
                {
                    SelectRendererState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);

                    RendererShadeMode = RENDERER_MODULE_SHADE_GOURAUD;
                }
            }
            else
            {
                SelectRendererState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);

                RendererShadeMode = RENDERER_MODULE_SHADE_GOURAUD;
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_FILTER_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_FILTER_POINT:
            {
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_MAGFILTER, D3DTFG_POINT);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_MINFILTER, D3DTFG_POINT);

                break;
            }
            case RENDERER_MODULE_TEXTURE_FILTER_LENEAR:
            {
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_MINFILTER, D3DTFG_LINEAR);

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_ANTI_ALIAS_STATE:
        {
            if (!SelectRendererState(D3DRENDERSTATE_ANTIALIAS, (DWORD)value)) { return RENDERER_MODULE_FAILURE; }

            if ((u32)value != RENDERER_MODULE_ANTIALIASING_NONE
                && State.Device.Capabilities.AntiAliasing == RENDERER_MODULE_ANTIALIASING_NONE)
            {
                return RENDERER_MODULE_FAILURE;
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_ALPHA_BLEND_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_ALPHA_BLEND_NONE: { SelectRendererState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE); break; }
            case RENDERER_MODULE_ALPHA_BLEND_UNKNOWN_1: { return RENDERER_MODULE_FAILURE; }
            case RENDERER_MODULE_ALPHA_BLEND_ACTIVE: { SelectRendererState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE); break; }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_MIP_FILTER_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_MIP_FILTER_NONE: { break; }
            case RENDERER_MODULE_TEXTURE_MIP_FILTER_POINT: { SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_MIPFILTER, D3DTFP_POINT); break; }
            case RENDERER_MODULE_TEXTURE_MIP_FILTER_POINT_ADVANCED: { SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_MIPFILTER, D3DTFP_POINT); return RENDERER_MODULE_FAILURE; }
            case RENDERER_MODULE_TEXTURE_MIP_FILTER_LINEAR:
            {
                if (!State.Device.Capabilities.IsTrilinearInterpolationAvailable)
                {
                    SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_MIPFILTER, D3DTFP_POINT);

                    return RENDERER_MODULE_FAILURE;
                }

                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_MIPFILTER, D3DTFP_LINEAR);

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_CHROMATIC_COLOR: { return RENDERER_MODULE_FAILURE; }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_ADDRESS_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_ADDRESS_CLAMP: { SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP); break; }
            case RENDERER_MODULE_TEXTURE_ADDRESS_WRAP: { SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ADDRESS, D3DTADDRESS_WRAP); break; }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_FOG_DENSITY:
        {
            // NOTE: Originally there are no checks for division for zero.
            // Bringing the logic from the newer renderers.

            const f32 density = *(f32*)&value;

            // NOTE: 0xfffff000 takes the high part of the float.
            const BOOL indivisible = (density == 0.0f || (((u32)value) & 0xfffff000) != 0);

            RendererFogDensity = indivisible ? density : (1.0f / density);

            SelectRendererState(D3DRENDERSTATE_FOGDENSITY, *(DWORD*)&RendererFogDensity);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_FOG_COLOR:
        {
            RendererFogColor = ((u32)value) & RENDERER_MODULE_FOG_COLOR_MASK;

            SelectRendererState(D3DRENDERSTATE_FOGCOLOR, RendererFogColor);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_LINE_WIDTH:
        case RENDERER_MODULE_STATE_SELECT_FLAT_FANS_STATE: { return RENDERER_MODULE_FAILURE; }
        case RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_STATE:
        {
            State.Settings.IsWindowMode = (BOOL)value;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_LAMBDAS:
        {
            if (value != NULL)
            {
                const RendererModuleLambdaContainer* lambdas = (RendererModuleLambdaContainer*)value;

                State.Lambdas.Log = lambdas->Log;

                CopyMemory(&State.Lambdas.Lambdas, lambdas, sizeof(RendererModuleLambdaContainer));
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_FOG_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_FOG_INACTIVE:
            {
                SelectRendererState(D3DRENDERSTATE_FOGENABLE, FALSE);

                State.Settings.IsFogActive = FALSE;

                break;
            }
            case RENDERER_MODULE_FOG_ACTIVE:
            {
                SelectRendererState(D3DRENDERSTATE_FOGENABLE, TRUE);

                State.Settings.IsFogActive = TRUE;

                break;
            }
            case RENDERER_MODULE_FOG_ACTIVE_LINEAR:
            {
                SelectRendererState(D3DRENDERSTATE_FOGENABLE, TRUE);
                SelectRendererState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_LINEAR);
                SelectRendererState(D3DRENDERSTATE_FOGCOLOR, RendererFogColor);
                SelectRendererState(D3DRENDERSTATE_FOGDENSITY, *(DWORD*)&RendererFogDensity);
                SelectRendererState(D3DRENDERSTATE_FOGSTART, *(DWORD*)&RendererFogStart);
                SelectRendererState(D3DRENDERSTATE_FOGEND, *(DWORD*)&RendererFogEnd);

                State.Settings.IsFogActive = FALSE;

                break;
            }
            case RENDERER_MODULE_FOG_ACTIVE_EXP:
            {
                SelectRendererState(D3DRENDERSTATE_FOGENABLE, TRUE);
                SelectRendererState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_EXP);
                SelectRendererState(D3DRENDERSTATE_FOGCOLOR, RendererFogColor);
                SelectRendererState(D3DRENDERSTATE_FOGDENSITY, *(DWORD*)&RendererFogDensity);
                SelectRendererState(D3DRENDERSTATE_FOGSTART, *(DWORD*)&RendererFogStart);
                SelectRendererState(D3DRENDERSTATE_FOGEND, *(DWORD*)&RendererFogEnd);

                State.Settings.IsFogActive = FALSE;

                break;
            }
            case RENDERER_MODULE_FOG_ACTIVE_EXP2:
            {
                SelectRendererState(D3DRENDERSTATE_FOGENABLE, TRUE);
                SelectRendererState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_EXP2);
                SelectRendererState(D3DRENDERSTATE_FOGCOLOR, RendererFogColor);
                SelectRendererState(D3DRENDERSTATE_FOGDENSITY, *(DWORD*)&RendererFogDensity);
                SelectRendererState(D3DRENDERSTATE_FOGSTART, *(DWORD*)&RendererFogStart);
                SelectRendererState(D3DRENDERSTATE_FOGEND, *(DWORD*)&RendererFogEnd);

                State.Settings.IsFogActive = FALSE;

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_FOG_START:
        {
            RendererFogStart = *(f32*)&value;

            SelectRendererState(D3DRENDERSTATE_FOGSTART, *(DWORD*)&RendererFogStart);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_FOG_END:
        {
            RendererFogEnd = *(f32*)&value;

            SelectRendererState(D3DRENDERSTATE_FOGEND, *(DWORD*)&RendererFogEnd);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_DEPTH_BIAS_STATE:
        case RENDERER_MODULE_STATE_SELECT_DEPTH_BIAS_STATE_ALTERNATIVE:
        {
            if (RendererDepthBias != *(f32*)&value)
            {
                AttemptRenderScene();

                RendererDepthBias = *(f32*)&value * 0.000030517578f;
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW:
        {
            State.Window.HWND = (HWND)value;

            if (State.DX.Clipper != NULL && State.Settings.IsWindowMode) { State.DX.Clipper->SetHWnd(0, State.Window.HWND); }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_RESTORE_RENDERER_SURFACES:
        {
            if (!RestoreRendererSurfaces()) { return RENDERER_MODULE_FAILURE; }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_ALPHA_TEST_STATE:
        {
            if ((u32)value == RENDERER_MODULE_ALPHA_TEST_0)
            {
                SelectRendererState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
                SelectRendererState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_ALWAYS);
            }
            else
            {
                SelectRendererState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE);
                SelectRendererState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATER);
                SelectRendererState(D3DRENDERSTATE_ALPHAREF, (DWORD)value);
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_RENDERER_INSTANCE:
        {
            *(IDirectDraw4**)value = State.DX.Active.Instance;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        default:
        {
            if (SelectBasicRendererState(actual, value) == RENDERER_MODULE_FAILURE) { return RENDERER_MODULE_FAILURE; }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_BLEND_STATE_ALTERNATIVE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_BLEND_SOURCE_ALPHA_INVERSE_SOURCE_ALPHA:
            {
                SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

                break;
            }
            case RENDERER_MODULE_BLEND_SOURCE_ALPHA_ONE:
            {
                SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);

                break;
            }
            case RENDERER_MODULE_BLEND_ZERO_INVERSE_SOURCE_ALPHA:
            {
                SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
                SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

                break;
            }
            case RENDERER_MODULE_BLEND_DESTINATION_COLOR_ZERO:
            {
                SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTCOLOR);
                SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_FOG_ALPHAS_ALTERNATIVE:
        {
            SelectRendererFogAlphas((u8*)value, RendererFogAlphas);

            State.Settings.IsFogActive = TRUE;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_DEPTH_BUFFER_WRITE_STATE_ALTERNATIVE:
        {
            SelectRendererState(D3DRENDERSTATE_ZWRITEENABLE, ((u32)value) != 0 ? TRUE : FALSE);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_DEVICE_TYPE:
        {
            SelectRendererDeviceType((u32)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_401: { result = RENDERER_MODULE_SUCCESS; break; }
        case RENDERER_MODULE_STATE_ACQUIRE_DEVICE_CAPABILITIES:
        {
            if (value == NULL) { return NULL; }

            RendererModuleDeviceCapabilities5* result = (RendererModuleDeviceCapabilities5*)value;

            result->IsAccelerated = State.Device.Capabilities.IsAccelerated;
            result->RenderBits = State.Device.Capabilities.RendererBits;
            result->DepthBits = State.Device.Capabilities.DepthBits;
            result->IsPerspectiveTextures = State.Device.Capabilities.IsPerspectiveTextures;
            result->IsAlphaTextures = State.Device.Capabilities.IsAlphaTextures;
            result->IsAlphaFlatBlending = State.Device.Capabilities.IsAlphaFlatBlending;
            result->IsAlphaProperBlending = State.Device.Capabilities.IsAlphaProperBlending;
            result->IsModulateBlending = State.Device.Capabilities.IsModulateBlending;
            result->IsSourceAlphaBlending = State.Device.Capabilities.IsSourceAlphaBlending;
            result->IsColorBlending = State.Device.Capabilities.IsColorBlending;
            result->IsSpecularBlending = State.Device.Capabilities.IsSpecularBlending;

            return (u32)result;
        }
        case RENDERER_MODULE_STATE_SELECT_SOURCE_BLEND_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_BLEND_ONE: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE); break; }
            case RENDERER_MODULE_BLEND_ZERO: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO); break; }
            case RENDERER_MODULE_BLEND_SOURCE_ALPHA: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA); break; }
            case RENDERER_MODULE_BLEND_INVERSE_SOURCE_ALPHA: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA); break; }
            case RENDERER_MODULE_BLEND_DESTINATION_ALPHA: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTALPHA); break; }
            case RENDERER_MODULE_BLEND_INVERSE_DESTINATION_ALPHA: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVDESTALPHA); break; }
            case RENDERER_MODULE_BLEND_SOURCE_COLOR: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCCOLOR); break; }
            case RENDERER_MODULE_BLEND_DESTINATION_COLOR: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTCOLOR); break; }
            case RENDERER_MODULE_BLEND_INVERSE_SOURCE_COLOR: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCCOLOR); break; }
            case RENDERER_MODULE_BLEND_INVERSE_DESTINATION_COLOR: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVDESTCOLOR); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_DESTINATION_BLEND_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_BLEND_ONE: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE); break; }
            case RENDERER_MODULE_BLEND_ZERO: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO); break; }
            case RENDERER_MODULE_BLEND_SOURCE_ALPHA: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCALPHA); break; }
            case RENDERER_MODULE_BLEND_INVERSE_SOURCE_ALPHA: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA); break; }
            case RENDERER_MODULE_BLEND_DESTINATION_ALPHA: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_DESTALPHA); break; }
            case RENDERER_MODULE_BLEND_INVERSE_DESTINATION_ALPHA: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVDESTALPHA); break; }
            case RENDERER_MODULE_BLEND_SOURCE_COLOR: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR); break; }
            case RENDERER_MODULE_BLEND_DESTINATION_COLOR: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_DESTCOLOR); break; }
            case RENDERER_MODULE_BLEND_INVERSE_SOURCE_COLOR: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCCOLOR); break; }
            case RENDERER_MODULE_BLEND_INVERSE_DESTINATION_COLOR: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVDESTCOLOR); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_STAGE_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_TEXTURE_COLOR:
            {
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

                break;
            }
            case RENDERER_MODULE_TEXTURE_TEXTURE_DIFFUSE_COLOR:
            {
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

                break;
            }
            case RENDERER_MODULE_TEXTURE_BLEND_TEXTURE_ALPHA_DIFFUSE:
            {
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

                break;
            }
            case RENDERER_MODULE_TEXTURE_MODULATE_TEXTURE_DIFFUSE_COLOR:
            {
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

                break;
            }
            case RENDERER_MODULE_TEXTURE_SELECT_TEXTURE_COLOR:
            {
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

                break;
            }
            case RENDERER_MODULE_TEXTURE_SUMMARIZE_TEXTURE_DIFFUSE_COLOR:
            {
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_ADD);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_HINT_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_HINT_NONE: { RendererTextureHint = RENDERER_MODULE_TEXTURE_HINT_NONE; break; }
            case RENDERER_MODULE_TEXTURE_HINT_DYNAMIC: { RendererTextureHint = RENDERER_MODULE_TEXTURE_HINT_DYNAMIC; break; }
            case RENDERER_MODULE_TEXTURE_HINT_STATIC: { RendererTextureHint = RENDERER_MODULE_TEXTURE_HINT_STATIC; break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_ACTIVE_STATE_ALTERNATIVE:
        {
            State.Settings.IsWindowModeActive = ((u32)value) != 0 ? TRUE : FALSE;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        }

        if (State.Lambdas.SelectState != NULL) { State.Lambdas.SelectState(actual, value); }

        return result;
    }

    // 0x600040f0
    // a.k.a. THRASH_settexture
    DLLAPI u32 STDCALLAPI SelectTexture(RendererTexture* tex)
    {
        const BOOL result = SelectRendererTexture(tex);

        if (result && State.Lambdas.SelectState != NULL) { State.Lambdas.SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE, tex); }

        return result;
    }

    // 0x600027a0
    // a.k.a. THRASH_setvideomode
    DLLAPI u32 STDCALLAPI SelectVideoMode(const u32 mode, const u32 pending, const u32 depth)
    {
        if (State.Scene.IsActive) { LOGERROR("D3D Setting videomode in 3d scene !!!!\n"); }

        SelectRendererDevice();

        s32 result = RENDERER_MODULE_FAILURE;

        if (State.DX.Instance != NULL)
        {
            if (State.Lambdas.Lambdas.AcquireWindow == NULL)
            {
                InitializeRendererDeviceSurfacesExecute(0, State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES, mode, pending, NULL);
            }
            else
            {
                SetForegroundWindow(State.Window.HWND);
                PostMessageA(State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES, mode, pending);
                WaitForSingleObject(State.Mutex, INFINITE);
            }

            if (State.DX.Code == DD_OK)
            {
                DDSURFACEDESC2 desc;
                ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

                desc.dwSize = sizeof(DDSURFACEDESC2);

                const HRESULT code = State.DX.Surfaces.Active[1]->GetSurfaceDesc(&desc);

                if (code != DD_OK) { LOGWARNING("DX6_setvideomode:  Error getting Surface Description %8x\n", code); }
            }
            else { LOGERROR("DX6_setdisplay - error\n"); }

            result = State.DX.Code == DD_OK;
        }

        InitializeRendererModuleState(pending, depth);

        return result;
    }

    // 0x600011e0
    // a.k.a. THRASH_sync
    DLLAPI u32 STDCALLAPI SyncGameWindow(const u32)
    {
        UnlockGameWindow(LockGameWindow());

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60004130
    // a.k.a. THRASH_talloc
    DLLAPI RendererTexture* STDCALLAPI AllocateTexture(const u32 width, const u32 height, const u32 format, const u32 options, const u32)
    {
        if (State.Textures.Illegal) { return NULL; }

        RendererTexture* tex = InitializeRendererTexture();

        tex->Width = width;
        tex->Height = height;

        tex->UnknownFormatIndexValue = UnknownFormatValues[format];
        tex->FormatIndex = State.Textures.Formats.Indexes[format];
        tex->FormatIndexValue = format;

        tex->MipMapCount = 1;

        tex->Is16Bit = (format == RENDERER_PIXEL_FORMAT_R5G5B5 || format == RENDERER_PIXEL_FORMAT_R4G4B4);

        tex->Options = options;
        tex->MemoryType = RENDERER_MODULE_TEXTURE_LOCATION_SYSTEM_MEMORY;

        tex->Surface1 = NULL;
        tex->Texture1 = NULL;
        tex->Surface2 = NULL;
        tex->Texture2 = NULL;
        tex->Palette = NULL;

        tex->Colors = 0;

        const s32 result = InitializeRendererTextureDetails(tex);

        if (result != RENDERER_INITIALIZE_TEXTURE_DETAILS_OK)
        {
            ReleaseRendererTexture(tex);

            if (result != RENDERER_INITIALIZE_TEXTURE_DETAILS_INVALID) { State.Textures.Illegal = TRUE; }

            return NULL;
        }

        tex->Previous = State.Textures.Current;
        State.Textures.Current = tex;

        return tex;
    }

    // 0x600042a0
    // a.k.a. THRASH_treset
    DLLAPI u32 STDCALLAPI ResetTextures(void)
    {
        if (State.Scene.IsActive)
        {
            FlushGameWindow();
            SyncGameWindow(0);
            Idle();

            State.Scene.IsActive = FALSE;
        }

        while (State.Textures.Current != NULL)
        {
            if (State.Textures.Current->Surface1 != NULL) { State.Textures.Current->Surface1->Release(); }
            if (State.Textures.Current->Texture1 != NULL) { State.Textures.Current->Texture1->Release(); }
            if (State.Textures.Current->Surface2 != NULL) { State.Textures.Current->Surface2->Release(); }
            if (State.Textures.Current->Texture2 != NULL) { State.Textures.Current->Texture2->Release(); }
            if (State.Textures.Current->Palette != NULL) { State.Textures.Current->Palette->Release(); }

            RendererTexture* tex = State.Textures.Current;

            State.Textures.Current = State.Textures.Current->Previous;

            ReleaseRendererTexture(tex);
        }

        State.Textures.Current = NULL;

        State.Textures.Illegal = FALSE;

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60004270
    // a.k.a. THRASH_tupdate
    DLLAPI RendererTexture* STDCALLAPI UpdateTexture(RendererTexture* tex, const u32* pixels, const u32* palette)
    {
        if (tex == NULL) { return NULL; }

        return UpdateRendererTexture(tex, pixels, palette) ? tex : NULL;
    }

    // 0x600013e0
    // a.k.a. THRASH_unlockwindow
    DLLAPI u32 STDCALLAPI UnlockGameWindow(const RendererModuleWindowLock* state)
    {
        if (State.Lock.IsActive && State.Lock.Surface != NULL)
        {
            State.Lock.Surface->Unlock(state == NULL ? NULL : (LPRECT)&state->Rect);

            State.Lock.Surface = NULL;
            State.Lock.IsActive = FALSE;

            if (State.Lambdas.Lambdas.LockWindow != NULL) { State.Lambdas.Lambdas.LockWindow(FALSE); }
        }

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60001100
    // a.k.a. THRASH_window
    DLLAPI u32 STDCALLAPI SelectGameWindow(const u32 indx)
    {
        if (indx == 3) // TODO
        {
            State.DX.Surfaces.Window = State.DX.Active.Surfaces.Active.Depth;

            return State.DX.Active.Surfaces.Active.Depth != NULL;
        }

        State.DX.Surfaces.Window = State.DX.Surfaces.Active[indx];

        if (indx == 0) { State.DX.Surfaces.Window = NULL; } // TODO

        return State.DX.Surfaces.Window != NULL;
    }

    // 0x60003000
    // a.k.a. THRASH_writerect
    DLLAPI u32 STDCALLAPI WriteRectangle(const u32 x, const u32 y, const u32 width, const u32 height, const u32* pixels)
    {
        RendererModuleWindowLock* state = LockGameWindow();

        if (state == NULL) { return RENDERER_MODULE_FAILURE; }

        const u32 multiplier = state->Format == RENDERER_PIXEL_FORMAT_R8G8B8
            ? (GRAPHICS_BITS_PER_PIXEL_32 >> 3) : (GRAPHICS_BITS_PER_PIXEL_16 >> 3);

        const u32 length = multiplier * width;

        for (u32 xx = 0; xx < height; xx++)
        {
            const addr offset = (xx * state->Stride) + (state->Stride * y) + (multiplier * x);

            CopyMemory((void*)((addr)state->Data + (addr)offset), &pixels[xx * length], length);
        }

        return UnlockGameWindow(state);
    }
}