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
#include "Mathematics.Basic.hxx"
#include "Module.hxx"
#include "RendererValues.hxx"
#include "Settings.hxx"

#include <math.h>
#include <stdlib.h>

using namespace Mathematics;
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
        ModuleDescriptor.Signature = RENDERER_MODULE_SIGNATURE_D3D7;
        ModuleDescriptor.Unk1 = 0xd0; // TODO

        if (State.Device.Capabilities.MinTextureWidth == 0)
        {
            ModuleDescriptor.MaximumTextureWidth = 256;
            ModuleDescriptor.MaximumTextureHeight = 256;
            ModuleDescriptor.MinimumTextureWidth = 8;
            ModuleDescriptor.MinimumTextureHeight = 8;
            ModuleDescriptor.MultipleTextureWidth = 1;
            ModuleDescriptor.MultipleTextureHeight = 1;

            ModuleDescriptor.Caps = RENDERER_MODULE_CAPS_WINDOWED | ((State.DX.Active.IsSoft & 1) << 4)
                | RENDERER_MODULE_CAPS_SOFTWARE | RENDERER_MODULE_CAPS_TEXTURE_HEIGHT_POW2
                | RENDERER_MODULE_CAPS_TEXTURE_WIDTH_POW2 | RENDERER_MODULE_CAPS_TEXTURE_SQUARE | RENDERER_MODULE_CAPS_LINE_WIDTH;

            ModuleDescriptor.MaximumSimultaneousTextures = 1;
        }
        else
        {
            ModuleDescriptor.MaximumTextureWidth = State.Device.Capabilities.MaxTextureWidth;
            ModuleDescriptor.MaximumTextureHeight = State.Device.Capabilities.MaxTextureHeight;
            ModuleDescriptor.MinimumTextureWidth = State.Device.Capabilities.MinTextureWidth;
            ModuleDescriptor.MinimumTextureHeight = State.Device.Capabilities.MinTextureHeight;
            ModuleDescriptor.MultipleTextureWidth = State.Device.Capabilities.MultipleTextureWidth;
            ModuleDescriptor.MultipleTextureHeight = State.Device.Capabilities.MultipleTextureHeight;

            ModuleDescriptor.Caps = RENDERER_MODULE_CAPS_WINDOWED | ((State.DX.Active.IsSoft & 1) << 4)
                | (State.Device.Capabilities.IsPowerOfTwoTexturesHeight & 1) << 3
                | (State.Device.Capabilities.IsPowerOfTwoTexturesWidth & 1) << 2
                | (State.Device.Capabilities.IsSquareOnlyTextures & 1) << 1 | RENDERER_MODULE_CAPS_LINE_WIDTH;

            ModuleDescriptor.MaximumSimultaneousTextures = State.Device.Capabilities.MaximumSimultaneousTextures;
        }

        ModuleDescriptor.ClipAlign = 0;
        ModuleDescriptor.DXV = RENDERER_MODULE_VERSION_DX7;
        ModuleDescriptor.Author = RENDERER_MODULE_AUTHOR;
        ModuleDescriptor.Unk4 = 0x14; // TODO
        ModuleDescriptor.TextureFormatStates = RendererTextureFormatStates;
        ModuleDescriptor.Unk5 = 4; // TODO

        ModuleDescriptor.Unk6 = UnknownArray06;

        ModuleDescriptor.Capabilities.Capabilities = ModuleDescriptorDeviceCapabilities;

        strcpy(ModuleDescriptor.Name, RENDERER_MODULE_NAME);

        SelectRendererDevice();

        AcquireRendererModuleDescriptor(&ModuleDescriptor, RENDERER_MODULE_ENVIRONMENT_SECTION_NAME);

        return &ModuleDescriptor;
    }

    // 0x600012e0
    // a.k.a. THRASH_clearwindow
    DLLAPI u32 STDCALLAPI ClearGameWindow()
    {
        return ClearRendererViewPort(State.ViewPort.X0, State.ViewPort.Y0,
            State.ViewPort.X1 + State.ViewPort.X0, State.ViewPort.Y0 + State.ViewPort.Y1,
            State.DX.Surfaces.Window == State.DX.Surfaces.Depth);
    }

    // 0x600013c0
    // a.k.a. THRASH_clip
    DLLAPI u32 STDCALLAPI ClipGameWindow(const u32 x0, const u32 y0, const u32 x1, const u32 y1)
    {
        State.ViewPort.X0 = x0;
        State.ViewPort.Y0 = y0;
        State.ViewPort.X1 = x1 - x0;
        State.ViewPort.Y1 = y1 - y0;

        AttemptRenderScene();

        if (State.Scene.IsActive) { EndRendererScene(); }

        D3DVIEWPORT7 vp;
        ZeroMemory(&vp, sizeof(D3DVIEWPORT7));

        vp.dwX = x0;
        vp.dwY = y0;
        vp.dvMinZ = 0.0f;
        vp.dvMaxZ = 0.9999847f;
        vp.dwWidth = x1 - x0;
        vp.dwHeight = y1 - y0;

        State.DX.Device->SetViewport(&vp);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60001880
    // a.k.a. THRASH_createwindow
    // NOTE: Never being called by the application.
    DLLAPI u32 STDCALLAPI CreateGameWindow(const u32 width, const u32 height, const u32 format, void*)
    {
        if (DAT_6005ab5c != 0
            && (format == RENDERER_PIXEL_FORMAT_16_BIT_555 || format == RENDERER_PIXEL_FORMAT_16_BIT_565 || format == RENDERER_PIXEL_FORMAT_32_BIT))
        {
            State.Window.Count = State.Window.Count + 1;

            State.Windows[State.Window.Count + RENDERER_WINDOW_OFFSET].Texture = AllocateRendererTexture(width, height, format, 0, 0, TRUE);

            InitializeRendererDeviceDepthSurfaces(width, height,
                State.Windows[State.Window.Count + RENDERER_WINDOW_OFFSET].Surface,
                State.Windows[State.Window.Count + RENDERER_WINDOW_OFFSET].Texture->Texture);

            if (State.Windows[State.Window.Count + RENDERER_WINDOW_OFFSET].Texture != NULL) { return State.Window.Count + RENDERER_WINDOW_OFFSET; }
        }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001910
    // a.k.a. THRASH_destroywindow
    // NOTE: Never being called by the application.
    DLLAPI u32 STDCALLAPI DestroyGameWindow(const u32 indx)
    {
        if (indx < MAX_RENDERER_WINDOW_COUNT && State.Windows[indx].Texture != NULL && RENDERER_WINDOW_OFFSET < indx)
        {
            if (State.Windows[indx].Surface != NULL)
            {
                State.Windows[indx].Surface->Release();
                State.Windows[indx].Surface = NULL;
            }

            return ReleaseTexture(State.Windows[indx].Texture);
        }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001b10
    // // a.k.a. THRASH_drawline
    DLLAPI void STDCALLAPI DrawLine(RVX* a, RVX* b)
    {
        RenderLine(a, b);
    }

    // 0x60001b30
    // a.k.a. THRASH_drawlinemesh
    DLLAPI void STDCALLAPI DrawLineMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        RenderLineMesh(vertexes, indexes, count);
    }

    // 0x60001d20
    // a.k.a. THRASH_drawlinestrip
    DLLAPI void STDCALLAPI DrawLineStrip(const u32 count, RVX* vertexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawLine((RVX*)&vs[x + 0], (RVX*)&vs[x + 1]); }
    }

    // 0x60001b50
    // a.k.a. THRASH_drawlinestrip
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawLineStrips(const u32 count, RVX* vertexes, const u32* indexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawLine((RVX*)&vs[indexes[x + 0]], (RVX*)&vs[indexes[x + 1]]); }
    }

    // 0x60001b90
    // a.k.a. THRASH_drawpoint
    DLLAPI void STDCALLAPI DrawPoint(RVX* vertex)
    {
        RenderPoints(vertex, 1);
    }

    // 0x60001bb0
    // a.k.a. THRASH_drawpointmesh
    DLLAPI void STDCALLAPI DrawPointMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++)
        {
            const u16 index = *(u16*)((addr)indexes + (addr)(x * RendererIndexSize));

            DrawPoint((RVX*)&vs[index]);
        }
    }

    // 0x60001d50
    // a.k.a. THRASH_drawpointstrip
    DLLAPI void STDCALLAPI DrawPointStrip(const u32 count, RVX* vertexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawPoint((RVX*)&vs[x]); }
    }

    // 0x60001a90
    // a.k.a. THRASH_drawquad
    DLLAPI void STDCALLAPI DrawQuad(RVX* a, RVX* b, RVX* c, RVX* d)
    {
        if (State.Settings.Cull == 1 || ((u32)AcquireNormal((f32x3*)a, (f32x3*)b, (f32x3*)c) & 0x80000000) != State.Settings.Cull) { RenderQuad(a, b, c, d); } // TODO
    }

    // 0x60001af0
    // a.k.a. THRASH_drawquadmesh
    DLLAPI void STDCALLAPI DrawQuadMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        RenderQuadMesh(vertexes, indexes, count);
    }

    // 0x60001bf0
    // a.k.a. THRASH_drawsprite
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawSprite(RVX* a, RVX* b)
    {
        RTLVX ea;
        CopyMemory(&ea, b, sizeof(RTLVX));

        ea.XYZ.Y = ((RTLVX*)a)->XYZ.Y;
        ea.UV.Y = ((RTLVX*)a)->UV.Y;

        RTLVX eb;
        CopyMemory(&ea, b, sizeof(RTLVX));

        eb.XYZ.X = ((RTLVX*)a)->XYZ.X;
        eb.UV.X = ((RTLVX*)a)->UV.X;

        DrawQuad(a, (RVX*)&ea, b, (RVX*)&eb);
    }

    // 0x60001c50
    // a.k.a. THRASH_drawspritemesh
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawSpriteMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        for (u32 x = 0; x < count; x++)
        {
            const u16 ia = *(u16*)((addr)indexes + (addr)RendererIndexSize * (addr)(x + 0));
            const u16 ib = *(u16*)((addr)indexes + (addr)RendererIndexSize * (addr)(x + 1));

            RVX* a = (RVX*)((addr)vertexes + (addr)RendererVertexSize * (addr)ia);
            RVX* b = (RVX*)((addr)vertexes + (addr)RendererVertexSize * (addr)ib);

            DrawSprite(a, b);
        }
    }

    // 0x600019d0
    // a.k.a. THRASH_drawtri
    DLLAPI void STDCALLAPI DrawTriangle(RVX* a, RVX* b, RVX* c)
    {
        if (State.Settings.Cull == 1 || ((u32)AcquireNormal((f32x3*)a, (f32x3*)b, (f32x3*)c) & 0x80000000) != State.Settings.Cull) { RenderTriangle(a, b, c); } // TODO
    }

    // 0x60001cf0
    // a.k.a. THRASH_drawtrifan
    DLLAPI void STDCALLAPI DrawTriangleFan(const u32 count, RVX* vertexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawTriangle((RVX*)&vs[0], (RVX*)&vs[x + 1], (RVX*)&vs[x + 2]); }
    }

    // 0x60001a70
    // a.k.a. THRASH_drawtrifan
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawTriangleFans(const u32 count, RVX* vertexes, const u32* indexes)
    {
        RenderTriangleFans(vertexes, count + 2, count, indexes);
    }

    // 0x60001a30
    // a.k.a. THRASH_drawtrimesh
    DLLAPI void STDCALLAPI DrawTriangleMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        RenderTriangleMesh(vertexes, indexes, count);
    }

    // 0x60001ca0
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

    // 0x60001a50
    // a.k.a. THRASH_drawtristrip
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawTriangleStrips(const u32 count, RVX* vertexes, const u32* indexes)
    {
        RenderTriangleStrips(vertexes, count + 2, count, indexes);
    }

    // 0x60001320
    // a.k.a. THRASH_flushwindow
    DLLAPI u32 STDCALLAPI FlushGameWindow(void)
    {
        return EndRendererScene();
    }

    // 0x60001d80
    // a.k.a. THRASH_getstate
    // NOTE: Never being called by the application.
    DLLAPI u32 STDCALLAPI AcquireState(const u32 state)
    {
        const u32 actual = state & RENDERER_MODULE_SELECT_STATE_MASK;
        const u32 stage = MAKETEXTURESTAGEVALUE(state);

        const s32 indx = AcquireTextureStateStageIndex(actual);

        if (indx >= 0) { return State.Textures.StageStates[indx].Values[stage]; }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001960
    // a.k.a. THRASH_getwindowtexture
    // NOTE: Never being called by the application.
    DLLAPI RendererTexture* STDCALLAPI AcquireGameWindowTexture(const u32 indx)
    {
        if (indx < MAX_RENDERER_WINDOW_COUNT) { return State.Windows[indx].Texture; }

        return NULL;
    }

    // 0x60001370
    // a.k.a. THRASH_idle
    DLLAPI void STDCALLAPI Idle(void) { }

    // 0x60003670
    // a.k.a. THRASH_init
    DLLAPI u32 STDCALLAPI Init(void)
    {
        RendererState = RENDERER_STATE_INACTIVE;

        InitializeSettings();

        AcquireRendererDeviceCount();

        InitializeTextureStateStates();

        atexit(ReleaseRendererModule);

        return State.Devices.Count;
    }

    // 0x60003a50
    // a.k.a. THRASH_is
    DLLAPI u32 STDCALLAPI Is(void)
    {
        HWND hwnd = GetDesktopWindow();
        HDC hdc = GetWindowDC(hwnd);

        if (GetDeviceCaps(hdc, BITSPIXEL) < GRAPHICS_BITS_PER_PIXEL_8)
        {
            ReleaseDC(hwnd, hdc);

            return RENDERER_MODULE_FAILURE;
        }

        IDirectDraw7* instance = NULL;
        HRESULT result = DirectDrawCreateEx(NULL, (void**)&instance, IID_IDirectDraw7, NULL);

        if (result == DD_OK)
        {
            DDCAPS caps;
            ZeroMemory(&caps, sizeof(DDCAPS));

            caps.dwSize = sizeof(DDCAPS);

            result = instance->GetCaps(&caps, NULL);

            if ((caps.dwCaps & DDCAPS_3D) && result == DD_OK)
            {
                IDirect3D7* dx = NULL;
                result = instance->QueryInterface(IID_IDirect3D7, (void**)&dx);

                instance->Release();

                if (result == DD_OK && dx != NULL)
                {
                    dx->Release();

                    return RENDERER_MODULE_DX7_ACCELERATION_AVAILABLE;
                }

                if (dx != NULL) { dx->Release(); }
            }
        }

        if (instance != NULL) { instance->Release(); }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001480
    // a.k.a. THRASH_lockwindow
    DLLAPI RendererModuleWindowLock* STDCALLAPI LockGameWindow(void)
    {
        if (State.DX.Surfaces.Window == NULL) { return NULL; }

        if (State.Lock.IsActive) { LOGERROR("D3D lock called while locked\n"); }

        if (State.DX.Surfaces.Window->IsLost() != DD_OK) { return NULL; }

        if (State.Scene.IsActive) { EndRendererScene(); }

        if (State.Lambdas.Lambdas.LockWindow != NULL) { State.Lambdas.Lambdas.LockWindow(TRUE); }

        State.Lock.Surface = State.DX.Surfaces.Window;

        DDSURFACEDESC2 desc;
        ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

        desc.dwSize = sizeof(DDSURFACEDESC2);

        State.DX.Code = State.DX.Surfaces.Window->Lock(NULL, &desc, DDLOCK_NOSYSLOCK | DDLOCK_WAIT, NULL);

        if (State.DX.Code == DD_OK)
        {
            if (desc.ddpfPixelFormat.dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_16)
            {
                State.Lock.State.Format = (desc.ddpfPixelFormat.dwGBitMask == 0x7e0)
                    ? RENDERER_PIXEL_FORMAT_16_BIT_565
                    : RENDERER_PIXEL_FORMAT_UNKNOWN_11;
            }
            else if (desc.ddpfPixelFormat.dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_32)
            {
                State.Lock.State.Format = RENDERER_PIXEL_FORMAT_32_BIT;
            }
            else if (desc.ddpfPixelFormat.dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_24)
            {
                State.Lock.State.Format = RENDERER_PIXEL_FORMAT_24_BIT;
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

            return &State.Lock.State;
        }

        if (State.Lambdas.Lambdas.LockWindow != NULL) { State.Lambdas.Lambdas.LockWindow(FALSE); }

        return NULL;
    }

    // 0x60001330
    // a.k.a. THRASH_pageflip
    DLLAPI u32 STDCALLAPI ToggleGameWindow(void)
    {
        EndRendererScene();

        if (State.Lock.IsActive) { LOGERROR("D3D pageflip called in while locked\n"); }

        return ToggleRenderer();
    }

    // 0x60003b60
    // a.k.a. THRASH_readrect
    DLLAPI u32 STDCALLAPI ReadRectangle(const u32 x, const u32 y, const u32 width, const u32 height, u32* data)
    {
        RendererModuleWindowLock* state = RendererLock(RENDERER_LOCK_READ);

        if (state == NULL) { return RENDERER_MODULE_FAILURE; }

        const u32 multiplier = state->Format == RENDERER_PIXEL_FORMAT_24_BIT ? 4 : 2;
        const u32 length = multiplier * width;

        for (u32 xx = 0; xx < height; xx++)
        {
            const addr address = (xx * state->Stride) + (state->Stride * y) + (multiplier * x);

            CopyMemory((void*)((addr)state->Data + address), &data[xx * length], length);
        }

        return UnlockGameWindow(state);
    }

    // 0x600036c0
    // a.k.a. THRASH_restore
    DLLAPI u32 STDCALLAPI RestoreGameWindow(void)
    {
        if (RendererState == RENDERER_STATE_INACTIVE)
        {
            RendererState = RENDERER_STATE_ACTIVE;

            if (State.Lock.IsActive) { UnlockGameWindow(NULL); }

            ReleaseRendererDevice();

            RendererDeviceIndex = INVALID_RENDERER_DEVICE_INDEX;

            if (State.Lambdas.Lambdas.Execute != NULL)
            {
                if (GetWindowThreadProcessId(State.Window.Parent.HWND, NULL) != GetCurrentThreadId())
                {
                    ReleaseRendererWindow();

                    State.Lambdas.Log = NULL;

                    return RENDERER_MODULE_SUCCESS;
                }
            }

            ReleaseRendererDeviceInstance();

            State.Lambdas.Log = NULL;
        }

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x600020c0
    // a.k.a. THRASH_selectdisplay
    DLLAPI u32 STDCALLAPI SelectDevice(const s32 indx)
    {
        State.Device.Identifier = NULL;

        if (State.DX.Instance != NULL) { RestoreGameWindow(); }

        const char* name = NULL;

        if (indx < DEFAULT_RENDERER_DEVICE_INDEX || State.Devices.Count <= indx)
        {
            RendererDeviceIndex = DEFAULT_RENDERER_DEVICE_INDEX;
            State.Device.Identifier = State.Devices.Indexes[DEFAULT_RENDERER_DEVICE_INDEX];
            name = State.Devices.Enumeration.Names[DEFAULT_RENDERER_DEVICE_INDEX];
        }
        else
        {
            RendererDeviceIndex = indx;
            State.Device.Identifier = State.Devices.Indexes[indx];
            name = State.Devices.Enumeration.Names[indx];
        }

        strncpy(State.Device.Name, name, MAX_ENUMERATE_RENDERER_DEVICE_NAME_LENGTH);

        if (State.Lambdas.Lambdas.Execute != NULL)
        {
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE, (RENDERERMODULEEXECUTECALLBACK)InitializeRendererDeviceExecute);
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_RELEASE_DEVICE, (RENDERERMODULEEXECUTECALLBACK)ReleaseRendererDeviceExecute);
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES, (RENDERERMODULEEXECUTECALLBACK)InitializeRendererDeviceSurfacesExecute);

            if (State.Lambdas.Lambdas.Execute != NULL)
            {
                if (GetWindowThreadProcessId(State.Window.Parent.HWND, NULL) != GetCurrentThreadId())
                {
                    InitializeRendererDeviceLambdas();

                    return RENDERER_MODULE_SUCCESS;
                }
            }
        }

        InitializeRendererDevice();

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60003ca0
    // a.k.a. THRASH_setstate
    DLLAPI u32 STDCALLAPI SelectState(const u32 state, void* value)
    {
        const u32 actual = state & RENDERER_MODULE_SELECT_STATE_MASK;
        const u32 stage = MAKETEXTURESTAGEVALUE(state);

        u32 result = RENDERER_MODULE_FAILURE;

        switch (actual)
        {
        case RENDERER_MODULE_STATE_NONE:
        case RENDERER_MODULE_STATE_SELECT_CHROMATIC_COLOR:
        case RENDERER_MODULE_STATE_SELECT_LINE_WIDTH:
        case RENDERER_MODULE_STATE_SELECT_FLAT_FANS_STATE:
        case RENDERER_MODULE_STATE_49:
        case RENDERER_MODULE_STATE_50:
        case RENDERER_MODULE_STATE_51:
        case RENDERER_MODULE_STATE_446: { return RENDERER_MODULE_FAILURE; }
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

                State.Settings.Cull = 1; // TODO

                break;
            }
            case RENDERER_MODULE_CULL_COUNTER_CLOCK_WISE:
            {
                SelectRendererState(D3DRENDERSTATE_CULLMODE, D3DCULL_CCW);

                State.Settings.Cull = 0x80000000; // TODO

                break;
            }
            case RENDERER_MODULE_CULL_CLOCK_WISE:
            {
                SelectRendererState(D3DRENDERSTATE_CULLMODE, D3DCULL_CW);

                State.Settings.Cull = 0; // TODO

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_MATERIAL:
        {
            SelectRendererMaterial((u32)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_DEPTH_STATE:
        {
            if (!State.Device.Capabilities.IsDepthAvailable) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_DEPTH_DISABLE:
            {
                SelectRendererState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);
                SelectRendererState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
                SelectRendererState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);

                break;
            }
            case RENDERER_MODULE_DEPTH_ENABLE:
            {
                SelectRendererState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
                SelectRendererState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
                SelectRendererState(D3DRENDERSTATE_ZFUNC, RendererDepthFunction);

                break;
            }
            case RENDERER_MODULE_DEPTH_W:
            {
                if (State.Device.Capabilities.IsWBufferAvailable)
                {
                    SelectRendererState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
                    SelectRendererState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
                    SelectRendererState(D3DRENDERSTATE_ZFUNC, RendererDepthFunction);
                    SelectRendererState(D3DRENDERSTATE_ZENABLE, D3DZB_USEW);
                }
                else
                {
                    SelectRendererState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
                    SelectRendererState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
                    SelectRendererState(D3DRENDERSTATE_ZFUNC, RendererDepthFunction);

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
            switch ((u32)value)
            {
            case RENDERER_MODULE_SHADE_FLAT:
            case RENDERER_MODULE_SHADE_GOURAUD:
            {
                if ((u32)value == RENDERER_MODULE_SHADE_FLAT && SettingsState.FlatShading)
                {
                    SelectRendererState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);
                    SelectRendererState(D3DRENDERSTATE_SPECULARENABLE, FALSE);

                    RendererShadeMode = RENDERER_MODULE_SHADE_FLAT;
                }
                else
                {
                    SelectRendererState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
                    SelectRendererState(D3DRENDERSTATE_SPECULARENABLE, FALSE);

                    RendererShadeMode = RENDERER_MODULE_SHADE_GOURAUD;
                }

                break;
            }
            case RENDERER_MODULE_SHADE_GOURAUD_SPECULAR:
            {
                if (!State.Device.Capabilities.IsSpecularGouraudBlending) { return RENDERER_MODULE_FAILURE; }

                SelectRendererState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
                SelectRendererState(D3DRENDERSTATE_SPECULARENABLE, TRUE);

                RendererShadeMode = RENDERER_MODULE_SHADE_GOURAUD;

                break;
            }
            case RENDERER_MODULE_SHADE_3:
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_FILTER_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_FILTER_POINT:
            {
                SelectRendererTextureStage(stage, D3DTSS_MAGFILTER, D3DTFG_POINT);
                SelectRendererTextureStage(stage, D3DTSS_MINFILTER, D3DTFG_POINT);

                break;
            }
            case RENDERER_MODULE_TEXTURE_FILTER_LENEAR:
            {
                SelectRendererTextureStage(stage, D3DTSS_MAGFILTER, D3DTFG_LINEAR);
                SelectRendererTextureStage(stage, D3DTSS_MINFILTER, D3DTFG_LINEAR);

                break;
            }
            case RENDERER_MODULE_TEXTURE_FILTER_ANISOTROPY:
            {
                if (!State.Device.Capabilities.IsAnisotropyAvailable) { return RENDERER_MODULE_FAILURE; }

                SelectRendererTextureStage(stage, D3DTSS_MAGFILTER, D3DTFG_ANISOTROPIC);
                SelectRendererTextureStage(stage, D3DTSS_MINFILTER, D3DTFG_FLATCUBIC);
                SelectRendererTextureStage(stage, D3DTSS_MAXANISOTROPY, State.Device.Capabilities.MaxAnisotropy);

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_MIP_MAP_LOD_BIAS_STATE:
        {
            if (!State.Device.Capabilities.IsMipMapBiasAvailable) { return RENDERER_MODULE_FAILURE; }

            SelectRendererTextureStage(stage, D3DTSS_MIPMAPLODBIAS, (DWORD)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_ANTI_ALIAS_STATE:
        {
            u32 aliasing = RENDERER_MODULE_ANTI_ALIAS_NONE;

            switch ((u32)value)
            {
            case RENDERER_MODULE_ANTI_ALIAS_NONE: { aliasing = RENDERER_MODULE_ANTI_ALIAS_NONE; break; }
            case RENDERER_MODULE_ANTI_ALIAS_SORT_DEPENDENT:
            {
                aliasing = State.Device.Capabilities.AntiAliasing;

                if (State.Device.Capabilities.AntiAliasing != RENDERER_MODULE_ANTI_ALIAS_NONE
                    && State.Device.Capabilities.AntiAliasing != RENDERER_MODULE_ANTI_ALIAS_SORT_DEPENDENT)
                {
                    aliasing = RENDERER_MODULE_ANTI_ALIAS_SORT_INDEPENDENT;
                }

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            if (!SelectRendererState(D3DRENDERSTATE_ANTIALIAS, aliasing)) { return RENDERER_MODULE_FAILURE; }

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
            case RENDERER_MODULE_ALPHA_BLEND_UNKNOWN_1:
            case RENDERER_MODULE_ALPHA_BLEND_UNKNOWN_3: { return RENDERER_MODULE_FAILURE; }
            case RENDERER_MODULE_ALPHA_BLEND_ACTIVE: { SelectRendererState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE); break; }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_MIP_FILTER_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_MIP_FILTER_NONE: { SelectRendererTextureStage(stage, D3DTSS_MIPFILTER, D3DTFP_NONE); break; }
            case RENDERER_MODULE_TEXTURE_MIP_FILTER_POINT: { SelectRendererTextureStage(stage, D3DTSS_MIPFILTER, D3DTFP_POINT); break; }
            case RENDERER_MODULE_TEXTURE_MIP_FILTER_LINEAR:
            {
                if (!State.Device.Capabilities.IsTrilinearInterpolationAvailable) { return RENDERER_MODULE_FAILURE; }

                SelectRendererTextureStage(stage, D3DTSS_MIPFILTER, D3DTFP_LINEAR);

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_ADDRESS_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_ADDRESS_CLAMP: { SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP); break; }
            case RENDERER_MODULE_TEXTURE_ADDRESS_WRAP: { SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ADDRESS, D3DTADDRESS_WRAP); break; }
            case RENDERER_MODULE_TEXTURE_ADDRESS_MIRROR: { SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ADDRESS, D3DTADDRESS_MIRROR); break; }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_FOG_DENSITY:
        {
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
        case RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_STATE:
        {
            State.Settings.IsWindowMode = (BOOL)value;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_LAMBDAS:
        {
            if (value == NULL)
            {
                ZeroMemory(&State.Lambdas.Lambdas, sizeof(RendererModuleLambdaContainer));
            }
            else
            {
                const RendererModuleLambdaContainer* lambdas = (RendererModuleLambdaContainer*)value;

                State.Lambdas.Log = lambdas->Log;

                CopyMemory(&State.Lambdas.Lambdas, lambdas, sizeof(RendererModuleLambdaContainer));
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_FOG_ALPHAS:
        case RENDERER_MODULE_STATE_SELECT_FOG_ALPHAS_ALTERNATIVE:
        {
            SelectRendererFogAlphas((u8*)value, RendererFogAlphas);

            SelectRendererState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_NONE);

            State.Settings.IsFogActive = TRUE;

            DAT_60018850 = 0x10; // TODO

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
                if (!State.Device.Capabilities.IsWFogAvailable)
                {
                    SelectRendererState(D3DRENDERSTATE_FOGENABLE, TRUE);

                    if (DAT_60018850 == 16) { State.Settings.IsFogActive = TRUE; } // TODO
                }

                break;
            }
            case RENDERER_MODULE_FOG_ACTIVE_LINEAR:
            {
                if (!State.Device.Capabilities.IsWFogAvailable) { return RENDERER_MODULE_FAILURE; }

                DAT_60018850 = (u32)value;

                State.Settings.IsFogActive = FALSE;

                SelectRendererState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_LINEAR);
                SelectRendererState(D3DRENDERSTATE_FOGVERTEXMODE, D3DFOG_NONE);
                SelectRendererState(D3DRENDERSTATE_FOGCOLOR, RendererFogColor);
                SelectRendererState(D3DRENDERSTATE_FOGDENSITY, *(DWORD*)&RendererFogDensity);
                SelectRendererState(D3DRENDERSTATE_FOGSTART, *(DWORD*)&RendererFogStart);
                SelectRendererState(D3DRENDERSTATE_FOGEND, *(DWORD*)&RendererFogEnd);

                State.Settings.IsFogActive = FALSE;

                break;
            }
            case RENDERER_MODULE_FOG_ACTIVE_EXP:
            {
                if (!State.Device.Capabilities.IsWFogAvailable) { return RENDERER_MODULE_FAILURE; }

                DAT_60018850 = (u32)value;

                SelectRendererState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_EXP);
                SelectRendererState(D3DRENDERSTATE_FOGVERTEXMODE, D3DFOG_NONE);
                SelectRendererState(D3DRENDERSTATE_FOGCOLOR, RendererFogColor);
                SelectRendererState(D3DRENDERSTATE_FOGDENSITY, *(DWORD*)&RendererFogDensity);

                State.Settings.IsFogActive = FALSE;

                break;
            }
            case RENDERER_MODULE_FOG_ACTIVE_EXP2:
            {
                if (!State.Device.Capabilities.IsWFogAvailable) { return RENDERER_MODULE_FAILURE; }

                DAT_60018850 = (u32)value;

                State.Settings.IsFogActive = FALSE;

                SelectRendererState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_EXP2);
                SelectRendererState(D3DRENDERSTATE_FOGVERTEXMODE, D3DFOG_NONE);
                SelectRendererState(D3DRENDERSTATE_FOGCOLOR, RendererFogColor);
                SelectRendererState(D3DRENDERSTATE_FOGDENSITY, *(DWORD*)&RendererFogDensity);

                State.Settings.IsFogActive = FALSE;

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_FOG_START:
        case RENDERER_MODULE_STATE_SELECT_FOG_END: { return RENDERER_MODULE_SUCCESS; }
        case RENDERER_MODULE_STATE_SELECT_DEPTH_BIAS_STATE:
        case RENDERER_MODULE_STATE_SELECT_DEPTH_BIAS_STATE_ALTERNATIVE:
        {
            const f32 bias = *(f32*)&value;

            if (isnan(RendererDepthBias) || isnan(bias) == (RendererDepthBias == bias))
            {
                RendererDepthBias = bias * 0.000030517578f;
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW:
        {
            State.Window.HWND = (HWND)value;

            if (State.DX.Clipper != NULL && State.Settings.IsWindowMode) { State.DX.Clipper->SetHWnd(0, State.Window.HWND); }

            SelectBasicRendererState(RENDERER_MODULE_STATE_SELECT_WINDOW, value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_LOG_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_EXECUTE_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_LOCK_WINDOW_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_WINDOW_LOCK_STATE:
        case RENDERER_MODULE_STATE_SELECT_LINE_VERTEX_SIZE:
        case RENDERER_MODULE_STATE_SELECT_VERSION:
        case RENDERER_MODULE_STATE_SELECT_MEMORY_ALLOCATE_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_MEMORY_RELEASE_LAMBDA:
        case RENDERER_MODULE_STATE_34:
        case RENDERER_MODULE_STATE_37:
        case RENDERER_MODULE_STATE_SELECT_SELECT_STATE_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_BACK_BUFFER_STATE:
        case RENDERER_MODULE_STATE_SELECT_PALETTE:
        case RENDERER_MODULE_STATE_45:
        case RENDERER_MODULE_STATE_55:
        case RENDERER_MODULE_STATE_SELECT_DISPLAY_STATE:
        case RENDERER_MODULE_STATE_SELECT_GAME_WINDOW_INDEX:
        case RENDERER_MODULE_STATE_SELECT_SHAMELESS_PLUG_STATE:
        case RENDERER_MODULE_STATE_62:
        case RENDERER_MODULE_STATE_INDEX_SIZE:
        case RENDERER_MODULE_STATE_SELECT_LOG_STATE:
        case RENDERER_MODULE_STATE_72:
        case RENDERER_MODULE_STATE_73:
        case RENDERER_MODULE_STATE_74:
        case RENDERER_MODULE_STATE_75:
        case RENDERER_MODULE_STATE_76:
        case RENDERER_MODULE_STATE_77:
        case RENDERER_MODULE_STATE_78:
        case RENDERER_MODULE_STATE_79:
        case RENDERER_MODULE_STATE_80:
        case RENDERER_MODULE_STATE_81:
        case RENDERER_MODULE_STATE_82:
        case RENDERER_MODULE_STATE_83:
        case RENDERER_MODULE_STATE_84:
        case RENDERER_MODULE_STATE_85:
        case RENDERER_MODULE_STATE_86:
        case RENDERER_MODULE_STATE_87:
        case RENDERER_MODULE_STATE_88:
        case RENDERER_MODULE_STATE_89:
        case RENDERER_MODULE_STATE_90:
        case RENDERER_MODULE_STATE_91:
        case RENDERER_MODULE_STATE_92:
        case RENDERER_MODULE_STATE_93:
        case RENDERER_MODULE_STATE_94:
        case RENDERER_MODULE_STATE_95:
        case RENDERER_MODULE_STATE_96:
        case RENDERER_MODULE_STATE_97:
        case RENDERER_MODULE_STATE_98:
        case RENDERER_MODULE_STATE_99:
        case RENDERER_MODULE_STATE_100:
        {
            if (SelectBasicRendererState(actual, value) == RENDERER_MODULE_FAILURE) { return RENDERER_MODULE_FAILURE; }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_RESTORE_RENDERER_SURFACES:
        {
            if (!RestoreRendererSurfaces()) { return RENDERER_MODULE_FAILURE; }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_ALPHA_TEST_STATE:
        {
            if (!State.Device.Capabilities.IsAlphaComparisonAvailable) { return RENDERER_MODULE_FAILURE; }

            if ((u32)value == RENDERER_MODULE_ALPHA_TEST_0)
            {
                SelectRendererState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
                SelectRendererState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_ALWAYS);
            }
            else
            {
                SelectRendererState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE);
                SelectRendererState(D3DRENDERSTATE_ALPHAFUNC, RendererAlphaFunction);
                SelectRendererState(D3DRENDERSTATE_ALPHAREF, (DWORD)value);
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_RENDERER_INSTANCE:
        {
            if (value == NULL) { return RENDERER_MODULE_FAILURE; }

            *(IDirectDraw7**)value = State.DX.Active.Instance;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELCT_DEPTH_FUNCTION:
        {
            if (!((BOOL*)&State.Device.Capabilities.Capabilities)[(u32)value]) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_DEPTH_FUNCTION_NEVER:
            {
                RendererDepthFunction = D3DCMP_NEVER;

                SelectRendererState(D3DRENDERSTATE_ZFUNC, D3DCMP_NEVER);

                break;
            }
            case RENDERER_MODULE_DEPTH_FUNCTION_LESS:
            {
                RendererDepthFunction = D3DCMP_LESS;

                SelectRendererState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESS);

                break;
            }
            case RENDERER_MODULE_DEPTH_FUNCTION_EQUAL:
            {
                RendererDepthFunction = D3DCMP_EQUAL;

                SelectRendererState(D3DRENDERSTATE_ZFUNC, D3DCMP_EQUAL);

                break;
            }
            case RENDERER_MODULE_DEPTH_FUNCTION_LESS_EQUAL:
            {
                RendererDepthFunction = D3DCMP_LESSEQUAL;

                SelectRendererState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);

                break;
            }
            case RENDERER_MODULE_DEPTH_FUNCTION_GREATER:
            {
                RendererDepthFunction = D3DCMP_GREATER;

                SelectRendererState(D3DRENDERSTATE_ZFUNC, D3DCMP_GREATER);

                break;
            }
            case RENDERER_MODULE_DEPTH_FUNCTION_NOT_EQUAL:
            {
                RendererDepthFunction = D3DCMP_NOTEQUAL;

                SelectRendererState(D3DRENDERSTATE_ZFUNC, D3DCMP_NOTEQUAL);

                break;
            }
            case RENDERER_MODULE_DEPTH_FUNCTION_GREATER_EQUAL:
            {
                RendererDepthFunction = D3DCMP_GREATEREQUAL;

                SelectRendererState(D3DRENDERSTATE_ZFUNC, D3DCMP_GREATEREQUAL);

                break;
            }
            case RENDERER_MODULE_DEPTH_FUNCTION_ALWAYS:
            {
                RendererDepthFunction = D3DCMP_ALWAYS;

                SelectRendererState(D3DRENDERSTATE_ZFUNC, D3DCMP_ALWAYS);

                break;
            }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_STAGE_BLEND_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_NORMAL:
            {
                if (stage == RENDERER_TEXTURE_STAGE_0)
                {
                    if (!State.Device.Capabilities.IsModulateBlending)
                    {
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                    }
                    else
                    {
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
                    }
                }
                else if (State.Device.Capabilities.MaximumSimultaneousTextures < 2)
                {
                    SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_DISABLE);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
                }
                else
                {
                    SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
                }

                result = RENDERER_MODULE_SUCCESS; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_ADD:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_ADD);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_ADD);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, (stage != RENDERER_TEXTURE_STAGE_0) ? D3DTA_CURRENT : D3DTA_DIFFUSE);

                result = State.Textures.Stages[stage].Unk01; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_DISABLE:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_DISABLE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

                result = RENDERER_MODULE_SUCCESS; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_MODULATE:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_MODULATE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, (stage != RENDERER_TEXTURE_STAGE_0) ? D3DTA_CURRENT : D3DTA_DIFFUSE);

                result = State.Textures.Stages[stage].Unk03; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_SUBTRACT:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_SUBTRACT);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

                result = State.Textures.Stages[stage].Unk02; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_ARG2:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_ADDSIGNED);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_COMPLEMENT | D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

                result = State.Textures.Stages[stage].Unk07; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_MODULATE_2X:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_MODULATE2X);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, (stage != RENDERER_TEXTURE_STAGE_0) ? D3DTA_CURRENT : D3DTA_DIFFUSE);

                result = State.Textures.Stages[stage].Unk04; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_MODULATE_4X:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTSS_ALPHAARG2);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, (stage != RENDERER_TEXTURE_STAGE_0) ? D3DTA_CURRENT : D3DTA_DIFFUSE);

                result = State.Textures.Stages[stage].Unk05; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_TEXTURE_ALPHA:
            {
                if (stage == RENDERER_TEXTURE_STAGE_0)
                {
                    if (!State.Device.Capabilities.IsModulateBlending)
                    {
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                    }
                    else
                    {
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                        SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
                    }

                    result = State.Textures.Stages[RENDERER_TEXTURE_STAGE_0].Unk09;
                }
                else
                {
                    SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

                    result = State.Textures.Stages[stage].Unk09;
                }

                break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_ADD_SIGNED:
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_ADD_SIGNED_ALTERNATIVE:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_ADDSIGNED);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_ADDSIGNED);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, (stage != RENDERER_TEXTURE_STAGE_0) ? D3DTA_CURRENT : D3DTA_DIFFUSE);

                result = State.Textures.Stages[stage].Unk08; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_DOT_PRODUCT_3:
            {
                if (stage == RENDERER_TEXTURE_STAGE_0)
                {
                    SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_DOTPRODUCT3);
                    SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                    SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_CURRENT);
                    SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                    SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                }
                else
                {
                    SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_MODULATE);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
                    SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                    SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
                }

                result = State.Textures.Stages[stage].Unk10; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_BUMP_ENV_MAP:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_BUMPENVMAP);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, (stage != RENDERER_TEXTURE_STAGE_0) ? D3DTA_CURRENT : D3DTA_DIFFUSE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

                result = State.Textures.Stages[stage].Unk11; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_BUMP_ENV_MAP_LUMINANCE:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_BUMPENVMAPLUMINANCE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, (stage != RENDERER_TEXTURE_STAGE_0) ? D3DTA_CURRENT : D3DTA_DIFFUSE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

                result = State.Textures.Stages[stage].Unk12; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_ADD_BLEND_FACTOR_ALPHA:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_ADD);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_BLENDFACTORALPHA);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);

                result = RENDERER_MODULE_SUCCESS; break;
            }
            case RENDERER_MODULE_TEXTURE_STAGE_BLEND_BLEND_FACTOR_ALPHA_ARG1:
            {
                SelectRendererTextureStage(stage, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
                SelectRendererTextureStage(stage, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

                result = RENDERER_MODULE_SUCCESS; break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            if (result == RENDERER_MODULE_FAILURE) { return RENDERER_MODULE_FAILURE; }

            break;
        }
        case RENDERER_MODULE_STATE_SELECT_VERTEX_TYPE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_VERTEX_TYPE_RTLVX:
            {
                RendererVertexSize = sizeof(RTLVX);

                SelectRendererVertexCount();

                RendererVertexType = D3DFVF_TLVERTEX;

                SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_COORDINATE_INDEX_STATE, (void*)0);
                SelectState(MAKETEXTURESTAGEMASK(RENDERER_TEXTURE_STAGE_1) | RENDERER_MODULE_STATE_SELECT_TEXTURE_COORDINATE_INDEX_STATE, (void*)0);

                break;
            }
            case RENDERER_MODULE_VERTEX_TYPE_RTLVX2:
            {
                RendererVertexSize = sizeof(RTLVX2);

                SelectRendererVertexCount();

                RendererVertexType = D3DFVF_TEX2 | D3DFVF_SPECULAR | D3DFVF_DIFFUSE | D3DFVF_XYZRHW;

                SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_COORDINATE_INDEX_STATE, (void*)0);
                SelectState(MAKETEXTURESTAGEMASK(RENDERER_TEXTURE_STAGE_1) | RENDERER_MODULE_STATE_SELECT_TEXTURE_COORDINATE_INDEX_STATE, (void*)1);

                break;
            }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_GAMMA_CONTROL_STATE:
        case RENDERER_MODULE_STATE_SELECT_GAMMA_CONTROL_STATE_ALTERNATIVE:
        {
            if (!State.Device.Capabilities.IsPrimaryGammaAvailable) { return RENDERER_MODULE_FAILURE; }
            if (State.DX.GammaControl == NULL) { return RENDERER_MODULE_FAILURE; }

            const f32 modifier = Clamp(*(f32*)&value, 0.0f, 4.0f);

            DDGAMMARAMP gamma;

            for (u32 x = 0; x < 256; x++)
            {
                gamma.red[x] = (u16)Clamp<u32>((u32)(State.Settings.GammaControl.red[x] * modifier), U16_MIN, U16_MAX);
                gamma.green[x] = (u16)Clamp<u32>((u32)(State.Settings.GammaControl.green[x] * modifier), U16_MIN, U16_MAX);
                gamma.blue[x] = (u16)Clamp<u32>((u32)(State.Settings.GammaControl.blue[x] * modifier), U16_MIN, U16_MAX);
            }

            State.DX.GammaControl->SetGammaRamp(0, &gamma);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_STENCIL_STATE:
        {
            if (!State.Device.Capabilities.IsStencilBuffer) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_STENCIL_INACTIVE: { SelectRendererState(D3DRENDERSTATE_STENCILENABLE, FALSE); break; }
            case RENDERER_MODULE_STENCIL_ACTIVE: { SelectRendererState(D3DRENDERSTATE_STENCILENABLE, TRUE); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_STENCIL_FUNCTION:
        {
            if (!State.Device.Capabilities.IsStencilBuffer) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_STENCIL_FUNCTION_NEVER: { SelectRendererState(D3DRENDERSTATE_STENCILFUNC, D3DCMP_NEVER); break; }
            case RENDERER_MODULE_STENCIL_FUNCTION_LESS: { SelectRendererState(D3DRENDERSTATE_STENCILFUNC, D3DCMP_LESS); break; }
            case RENDERER_MODULE_STENCIL_FUNCTION_EQUAL: { SelectRendererState(D3DRENDERSTATE_STENCILFUNC, D3DCMP_EQUAL); break; }
            case RENDERER_MODULE_STENCIL_FUNCTION_LESS_EQUAL: { SelectRendererState(D3DRENDERSTATE_STENCILFUNC, D3DCMP_LESSEQUAL); break; }
            case RENDERER_MODULE_STENCIL_FUNCTION_GREATER: { SelectRendererState(D3DRENDERSTATE_STENCILFUNC, D3DCMP_GREATER); break; }
            case RENDERER_MODULE_STENCIL_FUNCTION_NOT_EQUAL: { SelectRendererState(D3DRENDERSTATE_STENCILFUNC, D3DCMP_NOTEQUAL); break; }
            case RENDERER_MODULE_STENCIL_FUNCTION_GREATER_EQUAL: { SelectRendererState(D3DRENDERSTATE_STENCILFUNC, D3DCMP_GREATEREQUAL); break; }
            case RENDERER_MODULE_STENCIL_FUNCTION_ALWAYS: { SelectRendererState(D3DRENDERSTATE_STENCILFUNC, D3DCMP_ALWAYS); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_STENCIL_FAIL_STATE:
        {
            if (!State.Device.Capabilities.IsStencilBuffer) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_STENCIL_FAIL_KEEP: { SelectRendererState(D3DRENDERSTATE_STENCILFAIL, D3DSTENCILOP_KEEP); break; }
            case RENDERER_MODULE_STENCIL_FAIL_ZERO: { SelectRendererState(D3DRENDERSTATE_STENCILFAIL, D3DSTENCILOP_ZERO); break; }
            case RENDERER_MODULE_STENCIL_FAIL_REPLACE: { SelectRendererState(D3DRENDERSTATE_STENCILFAIL, D3DSTENCILOP_REPLACE); break; }
            case RENDERER_MODULE_STENCIL_FAIL_INCREMENT_CLAMP: { SelectRendererState(D3DRENDERSTATE_STENCILFAIL, D3DSTENCILOP_INCRSAT); break; }
            case RENDERER_MODULE_STENCIL_FAIL_DECREMENT_CLAMP: { SelectRendererState(D3DRENDERSTATE_STENCILFAIL, D3DSTENCILOP_DECRSAT); break; }
            case RENDERER_MODULE_STENCIL_FAIL_INVERT: { SelectRendererState(D3DRENDERSTATE_STENCILFAIL, D3DSTENCILOP_INVERT); break; }
            case RENDERER_MODULE_STENCIL_FAIL_INCREMENT: { SelectRendererState(D3DRENDERSTATE_STENCILFAIL, D3DSTENCILOP_INCR); break; }
            case RENDERER_MODULE_STENCIL_FAIL_DECREMENT: { SelectRendererState(D3DRENDERSTATE_STENCILFAIL, D3DSTENCILOP_DECR); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_STENCIL_DEPTH_FAIL_STATE:
        {
            if (!State.Device.Capabilities.IsStencilBuffer) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_KEEP: { SelectRendererState(D3DRENDERSTATE_STENCILZFAIL, D3DSTENCILOP_KEEP); break; }
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_ZERO: { SelectRendererState(D3DRENDERSTATE_STENCILZFAIL, D3DSTENCILOP_ZERO); break; }
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_REPLACE: { SelectRendererState(D3DRENDERSTATE_STENCILZFAIL, D3DSTENCILOP_REPLACE); break; }
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_INCREMENT_CLAMP: { SelectRendererState(D3DRENDERSTATE_STENCILZFAIL, D3DSTENCILOP_INCRSAT); break; }
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_DECREMENT_CLAMP: { SelectRendererState(D3DRENDERSTATE_STENCILZFAIL, D3DSTENCILOP_DECRSAT); break; }
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_INVERT: { SelectRendererState(D3DRENDERSTATE_STENCILZFAIL, D3DSTENCILOP_INVERT); break; }
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_INCREMENT: { SelectRendererState(D3DRENDERSTATE_STENCILZFAIL, D3DSTENCILOP_INCR); break; }
            case RENDERER_MODULE_STENCIL_DEPTH_FAIL_DECREMENT: { SelectRendererState(D3DRENDERSTATE_STENCILZFAIL, D3DSTENCILOP_DECR); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_STENCIL_PASS_STATE:
        {
            if (!State.Device.Capabilities.IsStencilBuffer) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_STENCIL_PASS_KEEP: { SelectRendererState(D3DRENDERSTATE_STENCILPASS, D3DSTENCILOP_KEEP); break; }
            case RENDERER_MODULE_STENCIL_PASS_ZERO: { SelectRendererState(D3DRENDERSTATE_STENCILPASS, D3DSTENCILOP_ZERO); break; }
            case RENDERER_MODULE_STENCIL_PASS_REPLACE: { SelectRendererState(D3DRENDERSTATE_STENCILPASS, D3DSTENCILOP_REPLACE); break; }
            case RENDERER_MODULE_STENCIL_PASS_INCREMENT_CLAMP: { SelectRendererState(D3DRENDERSTATE_STENCILPASS, D3DSTENCILOP_INCRSAT); break; }
            case RENDERER_MODULE_STENCIL_PASS_DECREMENT_CLAMP: { SelectRendererState(D3DRENDERSTATE_STENCILPASS, D3DSTENCILOP_DECRSAT); break; }
            case RENDERER_MODULE_STENCIL_PASS_INVERT: { SelectRendererState(D3DRENDERSTATE_STENCILPASS, D3DSTENCILOP_INVERT); break; }
            case RENDERER_MODULE_STENCIL_PASS_INCREMENT: { SelectRendererState(D3DRENDERSTATE_STENCILPASS, D3DSTENCILOP_INCR); break; }
            case RENDERER_MODULE_STENCIL_PASS_DECREMENT: { SelectRendererState(D3DRENDERSTATE_STENCILPASS, D3DSTENCILOP_DECR); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_BLEND_STATE:
        case RENDERER_MODULE_STATE_SELECT_BLEND_STATE_ALTERNATIVE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_ALPHA_BLEND_SOURCE_INVERSE_SOURCE:
            {
                SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

                break;
            }
            case RENDERER_MODULE_ALPHA_BLEND_SOURCE_ONE:
            {
                SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
                SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);

                break;
            }
            case RENDERER_MODULE_ALPHA_BLEND_ZERO_INVERSE_SOURCE:
            {
                SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO);
                SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

                break;
            }
            case RENDERER_MODULE_ALPHA_BLEND_DESTINATION_COLOR_ZERO:
            {
                SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTCOLOR);
                SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);

                break;
            }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_DEPTH_BUFFER_WRITE_STATE:
        case RENDERER_MODULE_STATE_SELECT_DEPTH_BUFFER_WRITE_STATE_ALTERNATIVE:
        {
            SelectRendererState(D3DRENDERSTATE_ZWRITEENABLE, (u32)value == 0 ? FALSE : TRUE);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TOGGLE_STATE:
        case RENDERER_MODULE_STATE_SELECT_TOGGLE_STATE_ALTERNATIVE:
        {
            if (!State.Device.Capabilities.IsNoVerticalSyncAvailable) { return RENDERER_MODULE_FAILURE; }

            RendererBlitOptions = DDBLT_WAIT;
            RendererToggleOptions = DDFLIP_WAIT;

            if ((u32)value == RENDERER_MODULE_TOGGLE_NO_VERTICAL_SYNC) { RendererToggleOptions = DDFLIP_NOVSYNC | DDFLIP_WAIT; }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_GUARD_BANDS:
        {
            if (value != NULL)
            {
                const s32 left = (s32)State.Device.Capabilities.GuardBandLeft;
                const s32 right = (s32)State.Device.Capabilities.GuardBandRight;
                const s32 top = (s32)State.Device.Capabilities.GuardBandTop;
                const s32 bottom = (s32)State.Device.Capabilities.GuardBandBottom;

                if (left != 0 && right != 0 && top != 0 && bottom != 0)
                {
                    RendererGuardBands* output = (RendererGuardBands*)value;

                    output->Left = left;
                    output->Right = right;
                    output->Top = top;
                    output->Bottom = bottom;
                }
            }

            return RENDERER_MODULE_FAILURE;
        }
        case RENDERER_MODULE_STATE_SELCT_ALPHA_FUNCTION:
        {
            if (!((BOOL*)&State.Device.Capabilities.Capabilities)[(u32)value]) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_ALPHA_FUNCTION_NEVER:
            {
                RendererAlphaFunction = D3DCMP_NEVER;

                SelectRendererState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_NEVER);

                break;
            }
            case RENDERER_MODULE_ALPHA_FUNCTION_LESS:
            {
                RendererAlphaFunction = D3DCMP_LESS;

                SelectRendererState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_LESS);

                break;
            }
            case RENDERER_MODULE_ALPHA_FUNCTION_EQUAL:
            {
                RendererAlphaFunction = D3DCMP_EQUAL;

                SelectRendererState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_EQUAL);

                break;
            }
            case RENDERER_MODULE_ALPHA_FUNCTION_LESS_EQUAL:
            {
                RendererAlphaFunction = D3DCMP_LESSEQUAL;

                SelectRendererState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_LESSEQUAL);

                break;
            }
            case RENDERER_MODULE_ALPHA_FUNCTION_GREATER:
            {
                RendererAlphaFunction = D3DCMP_GREATER;

                SelectRendererState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATER);

                break;
            }
            case RENDERER_MODULE_ALPHA_FUNCTION_NOT_EQUAL:
            {
                RendererAlphaFunction = D3DCMP_NOTEQUAL;

                SelectRendererState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_NOTEQUAL);

                break;
            }
            case RENDERER_MODULE_ALPHA_FUNCTION_GREATER_EQUAL:
            {
                RendererAlphaFunction = D3DCMP_GREATEREQUAL;

                SelectRendererState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATEREQUAL);

                break;
            }
            case RENDERER_MODULE_ALPHA_FUNCTION_ALWAYS:
            {
                RendererAlphaFunction = D3DCMP_ALWAYS;

                SelectRendererState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_ALWAYS);

                break;
            }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_ADDRESS_STATE_U:
        {
            if (!State.Device.Capabilities.IsTextureIndependentUVs) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_ADDRESS_CLAMP: { SelectRendererTextureStage(stage, D3DTSS_ADDRESSU, D3DTADDRESS_CLAMP); break; }
            case RENDERER_MODULE_TEXTURE_ADDRESS_WRAP: { SelectRendererTextureStage(stage, D3DTSS_ADDRESSU, D3DTADDRESS_WRAP); break; }
            case RENDERER_MODULE_TEXTURE_ADDRESS_MIRROR: { SelectRendererTextureStage(stage, D3DTSS_ADDRESSU, D3DTADDRESS_MIRROR); break; }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_ADDRESS_STATE_V:
        {
            if (!State.Device.Capabilities.IsTextureIndependentUVs) { return RENDERER_MODULE_FAILURE; }

            switch ((u32)value)
            {
            case RENDERER_MODULE_TEXTURE_ADDRESS_CLAMP: { SelectRendererTextureStage(stage, D3DTSS_ADDRESSV, D3DTADDRESS_CLAMP); break; }
            case RENDERER_MODULE_TEXTURE_ADDRESS_WRAP: { SelectRendererTextureStage(stage, D3DTSS_ADDRESSV, D3DTADDRESS_WRAP); break; }
            case RENDERER_MODULE_TEXTURE_ADDRESS_MIRROR: { SelectRendererTextureStage(stage, D3DTSS_ADDRESSV, D3DTADDRESS_MIRROR); break; }
            default: { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_CLEAR_DEPTH_STATE:
        {
            RendererClearDepth = *(f32*)&value;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_COORDINATE_INDEX_STATE:
        {
            SelectRendererTextureStage(stage, D3DTSS_TEXCOORDINDEX, (DWORD)value);

            return RENDERER_MODULE_FAILURE;
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_ACTIVE_STATE:
        case RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_ACTIVE_STATE_ALTERNATIVE:
        {
            State.Settings.IsWindowModeActive = ((u32)value) != 0 ? TRUE : FALSE;

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

            RendererModuleDeviceCapabilities7* result = (RendererModuleDeviceCapabilities7*)value;

            result->IsAccelerated = State.Device.Capabilities.IsAccelerated;
            result->DepthBits = State.Device.Capabilities.RendererDepthBits;
            result->RenderBits = State.Device.Capabilities.RendererBits;
            result->RendererDeviceDepthBits = State.Device.Capabilities.RendererDeviceDepthBits;
            result->IsDepthVideoMemoryAvailable = State.Device.Capabilities.IsDepthVideoMemoryCapable;
            result->IsDepthAvailable = State.Device.Capabilities.IsDepthAvailable;
            result->IsGreenAllowSixBits = State.Device.Capabilities.IsGreenAllowSixBits;
            result->IsVideoMemoryAvailable = State.Device.Capabilities.IsVideoMemoryCapable;
            result->IsDitherAvailable = State.Device.Capabilities.IsDitherAvailable;
            result->IsNoVerticalSync = State.Device.Capabilities.IsNoVerticalSyncAvailable;
            result->IsWBufferAvailable = State.Device.Capabilities.IsWBufferAvailable;
            result->IsWFogAvailable = State.Device.Capabilities.IsWFogAvailable;
            result->IsWindowModeAvailable = State.Device.Capabilities.IsWindowMode;
            result->IsTrilinearInterpolationAvailable = State.Device.Capabilities.IsTrilinearInterpolationAvailable;
            result->IsDepthBufferRemovalAvailable = State.Device.Capabilities.IsDepthBufferRemovalAvailable;
            result->IsPerspectiveTextures = State.Device.Capabilities.IsPerspectiveTextures;
            result->IsAlphaFlatBlending = State.Device.Capabilities.IsAlphaFlatBlending;
            result->IsAlphaProperBlending = State.Device.Capabilities.IsAlphaProperBlending;
            result->IsAlphaTextures = State.Device.Capabilities.IsAlphaTextures;
            result->IsModulateBlending = State.Device.Capabilities.IsModulateBlending;
            result->IsSourceAlphaBlending = State.Device.Capabilities.IsSourceAlphaBlending;
            result->AntiAliasing = State.Device.Capabilities.AntiAliasing;
            result->IsColorBlending = State.Device.Capabilities.IsColorBlending;
            result->IsAnisotropyAvailable = State.Device.Capabilities.IsAnisotropyAvailable;
            result->IsPrimaryGammaAvailable = State.Device.Capabilities.IsPrimaryGammaAvailable;
            result->IsSpecularGouraudBlending = State.Device.Capabilities.IsSpecularGouraudBlending;
            result->IsStencilBuffer = State.Device.Capabilities.IsStencilBuffer;
            result->IsSpecularBlending = State.Device.Capabilities.IsSpecularBlending;
            result->Unk29 = DAT_6005ab50;
            result->IsTextureIndependentUVs = State.Device.Capabilities.IsTextureIndependentUVs;
            result->IsMipMapBiasAvailable = State.Device.Capabilities.IsMipMapBiasAvailable;
            result->MinTextureWidth = State.Device.Capabilities.MinTextureWidth;
            result->MaxTextureWidth = State.Device.Capabilities.MaxTextureWidth;
            result->MinTextureHeight = State.Device.Capabilities.MinTextureHeight;
            result->MaxTextureHeight = State.Device.Capabilities.MaxTextureHeight;
            result->MultipleTextureWidth = State.Device.Capabilities.MultipleTextureWidth;
            result->IsPowerOfTwoTexturesWidth = State.Device.Capabilities.IsPowerOfTwoTexturesWidth;
            result->MultipleTextureHeight = State.Device.Capabilities.MultipleTextureHeight;
            result->IsPowerOfTwoTexturesHeight = State.Device.Capabilities.IsPowerOfTwoTexturesHeight;
            result->MaximumSimultaneousTextures = State.Device.Capabilities.MaximumSimultaneousTextures;
            result->IsSquareOnlyTextures = State.Device.Capabilities.IsSquareOnlyTextures;
            result->IsAntiAliasEdges = State.Device.Capabilities.IsAntiAliasEdges;
            result->GuardBandLeft = State.Device.Capabilities.GuardBandLeft;
            result->GuardBandRight = State.Device.Capabilities.GuardBandRight;
            result->GuardBandTop = State.Device.Capabilities.GuardBandTop;
            result->GuardBandBottom = State.Device.Capabilities.GuardBandBottom;
            result->MaxTextureRepeat = (u32)State.Device.Capabilities.MaxTextureRepeat;

            return (addr)result;
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
        case RENDERER_MODULE_STATE_SELECT_FILL_MODE_STATE:
        {
            if (!State.Device.Capabilities.IsAntiAliasEdges) { return RENDERER_MODULE_FAILURE; }

            if ((u32)value == RENDERER_MODULE_FILL_WIRE)
            {
                SelectRendererState(D3DRENDERSTATE_EDGEANTIALIAS, TRUE);
                SelectRendererState(D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
            }
            else
            {
                SelectRendererState(D3DRENDERSTATE_EDGEANTIALIAS, FALSE);
                SelectRendererState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_FACTOR:
        {
            SelectRendererState(D3DRENDERSTATE_TEXTUREFACTOR, (DWORD)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_MAIN_RENDERER_SURFACE:
        {
            *(IDirectDrawSurface7**)value = State.DX.Active.Surfaces.Active.Main;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_BACK_RENDERER_SURFACE:
        {
            *(IDirectDrawSurface7**)value = State.DX.Active.Surfaces.Active.Back;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_BUMP_MAPPING_MATRIX:
        {
            const RendererTextureStageBumpMappingMatrix* matrix = (RendererTextureStageBumpMappingMatrix*)value;

            SelectRendererTextureStage(stage, D3DTSS_BUMPENVMAT00, *(DWORD*)&matrix->M00);
            SelectRendererTextureStage(stage, D3DTSS_BUMPENVMAT01, *(DWORD*)&matrix->M01);
            SelectRendererTextureStage(stage, D3DTSS_BUMPENVMAT10, *(DWORD*)&matrix->M10);
            SelectRendererTextureStage(stage, D3DTSS_BUMPENVMAT11, *(DWORD*)&matrix->M11);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_BUMP_MAPPING_LUMINANCE_SCALE:
        {
            SelectRendererTextureStage(stage, D3DTSS_BUMPENVLSCALE, (DWORD)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_BUMP_MAPPING_LUMINANCE_OFFSET:
        {
            SelectRendererTextureStage(stage, D3DTSS_BUMPENVLOFFSET, (DWORD)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TOGGLE_ACTIVE_STATE:
        {
            State.Settings.IsToggleAllowed = ((u32)value == 0) ? FALSE : TRUE;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_TEST_COOPERATIVE_LEVEL:
        {
            if (State.DX.Active.Instance != NULL)
            {
                if (State.DX.Active.Instance->TestCooperativeLevel() != DD_OK) { return RENDERER_MODULE_FAILURE; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_ACCELERATION_TYPE:
        {
            State.Settings.Acceleration = (u32)value;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TRANSFORM_WORLD:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_0: { State.DX.Device->SetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX*)value); break; }
            case RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_1: { State.DX.Device->SetTransform(D3DTRANSFORMSTATE_WORLD1, (D3DMATRIX*)value); break; }
            case RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_2: { State.DX.Device->SetTransform(D3DTRANSFORMSTATE_WORLD2, (D3DMATRIX*)value); break; }
            case RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_3: { State.DX.Device->SetTransform(D3DTRANSFORMSTATE_WORLD3, (D3DMATRIX*)value); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TRANSFORM_VIEW:
        {
            State.DX.Device->SetTransform(D3DTRANSFORMSTATE_VIEW, (D3DMATRIX*)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TRANSFORM_PROJECTION:
        {
            State.DX.Device->SetTransform(D3DTRANSFORMSTATE_PROJECTION, (D3DMATRIX*)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TRANSFORM_MULTIPLY_WORLD:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_0: { State.DX.Device->MultiplyTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX*)value); break; }
            case RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_1: { State.DX.Device->MultiplyTransform(D3DTRANSFORMSTATE_WORLD1, (D3DMATRIX*)value); break; }
            case RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_2: { State.DX.Device->MultiplyTransform(D3DTRANSFORMSTATE_WORLD2, (D3DMATRIX*)value); break; }
            case RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_3: { State.DX.Device->MultiplyTransform(D3DTRANSFORMSTATE_WORLD3, (D3DMATRIX*)value); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TRANSFORM_MULTIPLY_VIEW:
        {
            State.DX.Device->MultiplyTransform(D3DTRANSFORMSTATE_VIEW, (D3DMATRIX*)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TRANSFORM_MULTIPLY_PROJECTION:
        {
            State.DX.Device->MultiplyTransform(D3DTRANSFORMSTATE_PROJECTION, (D3DMATRIX*)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_TRANSFORM_WORLD:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_0: { State.DX.Device->GetTransform(D3DTRANSFORMSTATE_WORLD, (D3DMATRIX*)value); break; }
            case RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_1: { State.DX.Device->GetTransform(D3DTRANSFORMSTATE_WORLD1, (D3DMATRIX*)value); break; }
            case RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_2: { State.DX.Device->GetTransform(D3DTRANSFORMSTATE_WORLD2, (D3DMATRIX*)value); break; }
            case RENDERER_MODULE_TRANSFORM_WORLD_MATRIX_3: { State.DX.Device->GetTransform(D3DTRANSFORMSTATE_WORLD3, (D3DMATRIX*)value); break; }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_TRANSFORM_VIEW:
        {
            State.DX.Device->GetTransform(D3DTRANSFORMSTATE_VIEW, (D3DMATRIX*)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_TRANSFORM_PROJECTION:
        {
            State.DX.Device->GetTransform(D3DTRANSFORMSTATE_PROJECTION, (D3DMATRIX*)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_LIGHT:
        {
            const RendererLight* input = (RendererLight*)value;

            D3DLIGHT7 light;

            light.dltType = (D3DLIGHTTYPE)input->Type;

            light.dcvDiffuse.a = RGBA_GETALPHA(input->Diffuse) / 255.0f;
            light.dcvDiffuse.r = RGBA_GETRED(input->Diffuse) / 255.0f;
            light.dcvDiffuse.g = RGBA_GETGREEN(input->Diffuse) / 255.0f;
            light.dcvDiffuse.b = RGBA_GETBLUE(input->Diffuse) / 255.0f;

            light.dcvSpecular.a = RGBA_GETALPHA(input->Specular) / 255.0f;
            light.dcvSpecular.r = RGBA_GETRED(input->Specular) / 255.0f;
            light.dcvSpecular.g = RGBA_GETGREEN(input->Specular) / 255.0f;
            light.dcvSpecular.b = RGBA_GETBLUE(input->Specular) / 255.0f;

            light.dcvAmbient.a = RGBA_GETALPHA(input->Ambient) / 255.0f;
            light.dcvAmbient.r = RGBA_GETRED(input->Ambient) / 255.0f;
            light.dcvAmbient.g = RGBA_GETGREEN(input->Ambient) / 255.0f;
            light.dcvAmbient.b = RGBA_GETBLUE(input->Ambient) / 255.0f;

            light.dvPosition.x = input->Position.X;
            light.dvPosition.y = input->Position.Y;
            light.dvPosition.z = input->Position.Z;

            light.dvDirection.x = input->Direction.X;
            light.dvDirection.y = input->Direction.Y;
            light.dvDirection.z = input->Direction.Z;

            light.dvRange = input->Range;
            light.dvFalloff = input->Falloff;

            light.dvAttenuation0 = input->Attenuation0;
            light.dvAttenuation1 = input->Attenuation1;
            light.dvAttenuation2 = input->Attenuation2;

            light.dvTheta = input->Theta;
            light.dvPhi = input->Phi;

            State.DX.Device->SetLight(stage, &light);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_LIGHT:
        {
            D3DLIGHT7 light;

            State.DX.Device->GetLight(stage, &light);

            RendererLight* output = (RendererLight*)value;

            output->Type = (RendererLightType)light.dltType;

            output->Diffuse = RGBA_MAKE((u32)(light.dcvDiffuse.r * 255.0), (u32)(light.dcvDiffuse.g * 255.0),
                (u32)(light.dcvDiffuse.b * 255.0), (u32)(light.dcvDiffuse.a * 255.0));
            output->Specular = RGBA_MAKE((u32)(light.dcvSpecular.r * 255.0), (u32)(light.dcvSpecular.g * 255.0),
                (u32)(light.dcvSpecular.b * 255.0), (u32)(light.dcvSpecular.a * 255.0));
            output->Ambient = RGBA_MAKE((u32)(light.dcvAmbient.r * 255.0), (u32)(light.dcvAmbient.g * 255.0),
                (u32)(light.dcvAmbient.b * 255.0), (u32)(light.dcvAmbient.a * 255.0));

            output->Position.X = light.dvPosition.x;
            output->Position.Y = light.dvPosition.y;
            output->Position.Z = light.dvPosition.z;

            output->Direction.X = light.dvDirection.x;
            output->Direction.Y = light.dvDirection.y;
            output->Direction.Z = light.dvDirection.z;

            output->Range = light.dvRange;
            output->Falloff = light.dvFalloff;

            output->Attenuation0 = light.dvAttenuation0;
            output->Attenuation1 = light.dvAttenuation1;
            output->Attenuation2 = light.dvAttenuation2;

            output->Theta = light.dvTheta;
            output->Phi = light.dvPhi;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_LIGHT_STATE:
        {
            State.DX.Device->LightEnable(stage, (BOOL)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_CURRENT_MATERIAL:
        {
            const RendererMaterial* input = (RendererMaterial*)value;

            D3DMATERIAL7 material;

            material.diffuse.a = RGBA_GETALPHA(input->Diffuse) / 255.0f;
            material.diffuse.r = RGBA_GETRED(input->Diffuse) / 255.0f;
            material.diffuse.g = RGBA_GETGREEN(input->Diffuse) / 255.0f;
            material.diffuse.b = RGBA_GETBLUE(input->Diffuse) / 255.0f;

            material.ambient.a = RGBA_GETALPHA(input->Ambient) / 255.0f;
            material.ambient.r = RGBA_GETRED(input->Ambient) / 255.0f;
            material.ambient.g = RGBA_GETGREEN(input->Ambient) / 255.0f;
            material.ambient.b = RGBA_GETBLUE(input->Ambient) / 255.0f;

            material.specular.a = RGBA_GETALPHA(input->Specular) / 255.0f;
            material.specular.r = RGBA_GETRED(input->Specular) / 255.0f;
            material.specular.g = RGBA_GETGREEN(input->Specular) / 255.0f;
            material.specular.b = RGBA_GETBLUE(input->Specular) / 255.0f;

            material.emissive.a = RGBA_GETALPHA(input->Emissive) / 255.0f;
            material.emissive.r = RGBA_GETRED(input->Emissive) / 255.0f;
            material.emissive.g = RGBA_GETGREEN(input->Emissive) / 255.0f;
            material.emissive.b = RGBA_GETBLUE(input->Emissive) / 255.0f;

            material.power = input->Power;

            State.DX.Device->SetMaterial(&material);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_CURRENT_MATERIAL:
        {
            D3DMATERIAL7 material;

            State.DX.Device->GetMaterial(&material);

            RendererMaterial* output = (RendererMaterial*)value;

            output->Diffuse = RGBA_MAKE((u32)(material.diffuse.r * 255.0f), (u32)(material.diffuse.g * 255.0f),
                (u32)(material.diffuse.b * 255.0f), (u32)(material.diffuse.a * 255.0f));
            output->Ambient = RGBA_MAKE((u32)(material.ambient.r * 255.0f), (u32)(material.ambient.g * 255.0f),
                (u32)(material.ambient.b * 255.0f), (u32)(material.ambient.a * 255.0f));
            output->Specular = RGBA_MAKE((u32)(material.specular.r * 255.0f), (u32)(material.specular.g * 255.0f),
                (u32)(material.specular.b * 255.0f), (u32)(material.specular.a * 255.0f));
            output->Emissive = RGBA_MAKE((u32)(material.emissive.r * 255.0f), (u32)(material.emissive.g * 255.0f),
                (u32)(material.emissive.b * 255.0f), (u32)(material.emissive.a * 255.0f));

            output->Power = material.power;

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_RENDER_PACKET:
        {
            if (!State.Scene.IsActive)
            {
                BeginRendererScene();

                State.Scene.IsActive = TRUE;
            }

            AttemptRenderScene();

            const RendererPacket* packet = (RendererPacket*)value;

            if (packet->Indexes == NULL || packet->IndexCount == 0)
            {
                if (State.DX.Device->DrawPrimitive((D3DPRIMITIVETYPE)packet->Type, packet->FVF,
                    packet->Vertexes, packet->VertexCount, 0) != DD_OK) {
                    return RENDERER_MODULE_FAILURE;
                }
            }
            else
            {
                if (State.DX.Device->DrawIndexedPrimitive((D3DPRIMITIVETYPE)packet->Type, packet->FVF,
                    packet->Vertexes, packet->VertexCount, packet->Indexes, packet->IndexCount, 0) != DD_OK) {
                    return RENDERER_MODULE_FAILURE;
                }
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_TRANSFORM_AND_LIGHT_CAPABILITIES:
        {
            if (value != NULL)
            {
                RendererTransformAndLightCapabilites* output = (RendererTransformAndLightCapabilites*)value;

                output->IsActive = State.DX.Active.IsActive;
                output->MaxActiveLights = State.Device.Capabilities.MaxActiveLights;
                output->Unk03 = 0; // TODO
                output->MaxUserClipPlanes = State.Device.Capabilities.MaxUserClipPlanes;
                output->MaxVertexBlendMatrices = State.Device.Capabilities.MaxVertexBlendMatrices;
                output->IsTransformLightBufferSystemMemoryAvailable = State.Device.Capabilities.IsTransformLightBufferSystemMemoryAvailable;
                output->IsTransformLightBufferVideoMemoryAvailable = State.Device.Capabilities.IsTransformLightBufferVideoMemoryAvailable;
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_AMBIENT_STATE:
        {
            State.DX.Device->SetRenderState(D3DRENDERSTATE_AMBIENT, (DWORD)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_CLIPPING_STATE:
        {
            AttemptRenderScene();

            State.DX.Device->SetRenderState(D3DRENDERSTATE_CLIPPING, (DWORD)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_LIGHTING_STATE:
        {
            State.DX.Device->SetRenderState(D3DRENDERSTATE_LIGHTING, ((u32)value == 0) ? FALSE : TRUE);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_INITIALIZE_VERTEX_BUFFER:
        {
            RendererVertexBuffer* input = (RendererVertexBuffer*)value;

            D3DDEVICEDESC7 caps;
            ZeroMemory(&caps, sizeof(D3DDEVICEDESC7));

            caps.dpcLineCaps.dwSize = sizeof(D3DPRIMCAPS);
            caps.dpcTriCaps.dwSize = sizeof(D3DPRIMCAPS);

            State.DX.Device->GetCaps(&caps);

            D3DVERTEXBUFFERDESC desc;

            desc.dwSize = sizeof(D3DVERTEXBUFFERDESC);
            desc.dwCaps = input->Capabilities;
            desc.dwFVF = input->FVF;
            desc.dwNumVertices = input->VertexCount;

            State.DX.DirectX->CreateVertexBuffer(&desc, (IDirect3DVertexBuffer7**)input->Buffer, 0);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_441:
        {
            // TODO NOT IMPLEMENTED
            // (**(code **)(**value + 0xc))(*value,*(undefined4 *)((int)value + 0x10),(int)value + 0x20,0);

            DebugBreak();

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_442:
        {
            // TODO NOT IMPLEMENTED
            // (**(code **)(**value + 0x10))(*value);

            DebugBreak();

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_RENDER_BUFFERED_PACKET:
        {
            if (!State.Scene.IsActive)
            {
                BeginRendererScene();

                State.Scene.IsActive = TRUE;
            }

            const RendererBufferPacket* input = (RendererBufferPacket*)value;

            if (input->Indexes == NULL || input->IndexCount == 0)
            {
                State.DX.Device->DrawPrimitiveVB((D3DPRIMITIVETYPE)input->Type,
                    (LPDIRECT3DVERTEXBUFFER7)input->Vertexes, 0, input->VertexCount, 0);
            }
            else
            {
                State.DX.Device->DrawIndexedPrimitiveVB((D3DPRIMITIVETYPE)input->Type,
                    (LPDIRECT3DVERTEXBUFFER7)input->Vertexes, 0, input->VertexCount,
                    input->Indexes, input->IndexCount, 0);
            }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_OPTIMIZE_VERTEX_BUFFER:
        {
            const RendererVertexBuffer* input = (RendererVertexBuffer*)value;

            ((IDirect3DVertexBuffer7*)input->Buffer)->Optimize(State.DX.Device, 0);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_CLIP_PLANE:
        {
            if (value != NULL) { State.DX.Device->SetClipPlane(stage, (D3DVALUE*)value); }

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_CLIP_STATE:
        {
            State.Settings.ClipPlaneState = ((u32)value == 0)
                ? (State.Settings.ClipPlaneState ^ 1) << stage
                : (State.Settings.ClipPlaneState | 1) << stage;

            State.DX.Device->SetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE, State.Settings.ClipPlaneState);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_VERTEX_BLEND_STATE:
        {
            State.DX.Device->SetRenderState(D3DRENDERSTATE_VERTEXBLEND, (DWORD)value);

            result = RENDERER_MODULE_SUCCESS; break;
        }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_COORDINATE_INDEX_STATE_ALTERNATIVE:
        {
            if (RendererVertexType & D3DFVF_TEX1)
            {
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_TEXCOORDINDEX, 0);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_1, D3DTSS_TEXCOORDINDEX, 0);
            }

            if (RendererVertexType & D3DFVF_TEX2)
            {
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_TEXCOORDINDEX, 0);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_1, D3DTSS_TEXCOORDINDEX, 1);
            }

            if (RendererVertexType & D3DFVF_TEX3)
            {
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_TEXCOORDINDEX, 0);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_1, D3DTSS_TEXCOORDINDEX, 1);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_2, D3DTSS_TEXCOORDINDEX, 2);
            }

            if (RendererVertexType & D3DFVF_TEX4)
            {
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_0, D3DTSS_TEXCOORDINDEX, 0);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_1, D3DTSS_TEXCOORDINDEX, 1);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_2, D3DTSS_TEXCOORDINDEX, 2);
                SelectRendererTextureStage(RENDERER_TEXTURE_STAGE_3, D3DTSS_TEXCOORDINDEX, 3);
            }

            return RENDERER_MODULE_FAILURE;
        }
        case RENDERER_MODULE_STATE_ACQUIRE_MODULATE_BLENDING_STATE:
        {
            result = State.Device.Capabilities.IsModulateBlending;

            if (!result) { return RENDERER_MODULE_FAILURE; }

            break;
        }
        }

        SelectRendererStateValue(state, value);

        return result;
    }

    // 0x60008980
    // a.k.a. THRASH_settexture
    DLLAPI u32 STDCALLAPI SelectTexture(RendererTexture* tex)
    {
        RendererTexture* active = (RendererTexture*)AcquireState(RENDERER_MODULE_STATE_SELECT_TEXTURE);

        if (State.DX.Device == NULL) { return RENDERER_MODULE_FAILURE; }

        if (active == tex) { return RENDERER_MODULE_SUCCESS; }

        if (SelectRendererTexture(tex))
        {
            SelectRendererStateValue(RENDERER_MODULE_STATE_SELECT_TEXTURE, tex);

            return RENDERER_MODULE_SUCCESS;
        }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60003750
    // a.k.a. THRASH_setvideomode
    DLLAPI u32 STDCALLAPI SelectVideoMode(const u32 mode, const u32 pending, const u32 depth)
    {
        State.Window.Bits = depth;

        if (depth == 1) { State.Window.Bits = GRAPHICS_BITS_PER_PIXEL_16; } // TODO
        else if (depth == 2) { State.Window.Bits = GRAPHICS_BITS_PER_PIXEL_32; } // TODO

        if (State.Scene.IsActive) { LOGWARNING("D3D Setting videomode in 3d scene !!!!\n"); }

        SelectRendererDevice();

        if (ModuleDescriptor.Capabilities.Capabilities[mode].Bits < State.Window.Bits) { State.Window.Bits = GRAPHICS_BITS_PER_PIXEL_16; }

        u32 result = RENDERER_MODULE_FAILURE;

        if (State.DX.Instance != NULL)
        {
            if (State.Lambdas.Lambdas.AcquireWindow == NULL)
            {
                InitializeRendererDeviceSurfacesExecute(0, State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES, mode, pending, NULL);
            }
            else
            {
                if (GetWindowThreadProcessId(State.Window.Parent.HWND, NULL) == GetCurrentThreadId())
                {
                    InitializeRendererDeviceSurfacesExecute(0, State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES, mode, pending, NULL);
                }
                else
                {
                    SetForegroundWindow(State.Window.HWND);
                    PostMessageA(State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES, mode, pending);
                    WaitForSingleObject(State.Mutex, 30000);
                }
            }

            if (State.DX.Code == DD_OK)
            {
                DDSURFACEDESC2 desc;
                ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

                desc.dwSize = sizeof(DDSURFACEDESC2);

                const HRESULT code = State.DX.Surfaces.Active[1]->GetSurfaceDesc(&desc);

                if (code != DD_OK) { LOGWARNING("DX7_setvideomode:  Error getting Surface Description %8x\n", code); }
            }
            else { LOGERROR("DX7_setdisplay - error\n"); }

            result = State.DX.Code == DD_OK;
        }

        InitializeRendererModuleState(mode, pending, depth, RENDERER_MODULE_ENVIRONMENT_SECTION_NAME);
        SelectBasicRendererState(RENDERER_MODULE_STATE_62, (void*)(DAT_60058df8 + 1));

        SelectGameWindow(1); // TODO

        const RendererModuleWindowLock* lock = LockGameWindow();

        UnlockGameWindow(lock);

        SelectGameWindow(0); // TODO

        ZeroMemory(&State.Settings.Lock, sizeof(RendererModuleWindowLock));

        if (lock != NULL)
        {
            State.Settings.Lock.Data = NULL;

            State.Settings.Lock.Stride = lock->Stride;
            State.Settings.Lock.Format = lock->Format;
            State.Settings.Lock.Width = lock->Width;
            State.Settings.Lock.Height = lock->Height;

            SelectRendererStateValue(RENDERER_MODULE_STATE_SELECT_WINDOW_LOCK_STATE, &State.Settings.Lock);
        }

        ResetTextures();

        RendererState = RENDERER_STATE_INACTIVE;

        return result;
    }

    // 0x60001380
    // a.k.a. THRASH_sync
    DLLAPI u32 STDCALLAPI SyncGameWindow(const u32 type)
    {
        if (type == 0) // TODO
        {
            UnlockGameWindow(RendererLock(RENDERER_LOCK_WRITE));
        }
        else if (type == 2) // TODO
        {
            State.DX.Active.Instance->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
        }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60008e70
    // a.k.a. THRASH_talloc
    DLLAPI RendererTexture* STDCALLAPI AllocateTexture(const u32 width, const u32 height, const u32 format, void* p4, const u32 options)
    {
        if (State.DX.Active.Instance != NULL)
        {
            if (State.DX.Active.Instance->TestCooperativeLevel() == DD_OK)
            {
                return AllocateRendererTexture(width, height, format, p4, options, FALSE);
            }
        }

        return NULL;
    }

    // 0x60008eb0
    // a.k.a. THRASH_tfree
    // NOTE: Never being called by the application.
    DLLAPI u32 STDCALLAPI ReleaseTexture(RendererTexture* tex)
    {
        if (State.Textures.Recent == NULL) { return RENDERER_MODULE_FAILURE; }

        // Attempt to remove the input texture from the linked list.
        {
            RendererTexture* current = State.Textures.Recent;
            RendererTexture* previous = State.Textures.Recent;

            while (current != tex)
            {
                previous = current;
                current = current->Previous;

                if (current == NULL) { return RENDERER_MODULE_FAILURE; }
            }

            previous->Previous = current->Previous;
        }

        if (tex->Palette != NULL) { tex->Palette->Release(); }
        if (tex->Texture != NULL) { tex->Texture->Release(); }
        if (tex->Surface != NULL) { tex->Surface->Release(); }

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60008fc0
    // a.k.a. THRASH_treset
    DLLAPI u32 STDCALLAPI ResetTextures(void)
    {
        State.Textures.Count = 0;

        State.Textures.Recent = NULL;

        if (State.DX.DirectX == NULL) { return RENDERER_MODULE_FAILURE; }

        SelectRendererTexture(NULL);

        if (State.Scene.IsActive)
        {
            FlushGameWindow();
            SyncGameWindow(0);
            Idle();

            State.Scene.IsActive = FALSE;
        }

        while (State.Textures.Current != NULL)
        {
            if (State.Textures.Current->Palette != NULL) { State.Textures.Current->Palette->Release(); }
            if (State.Textures.Current->Texture != NULL) { State.Textures.Current->Texture->Release(); }
            if (State.Textures.Current->Surface != NULL) { State.Textures.Current->Surface->Release(); }

            RendererTexture* tex = State.Textures.Current;

            State.Textures.Current = State.Textures.Current->Previous;

            ReleaseRendererTexture(tex);
        }

        State.Textures.Current = NULL;

        State.Textures.Illegal = FALSE;

        InitializeRenderState55();

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60008f20
    // a.k.a. THRASH_tupdate
    DLLAPI RendererTexture* STDCALLAPI UpdateTexture(RendererTexture* tex, const u32* pixels, const u32* palette)
    {
        if (State.DX.Active.Instance != NULL)
        {
            if (State.DX.Active.Instance->TestCooperativeLevel() == DD_OK && tex != NULL)
            {
                return UpdateRendererTexture(tex, pixels, palette) ? tex : NULL;
            }
        }

        return NULL;
    }

    // 0x60008f60
    // a.k.a. THRASH_tupdaterect
    // NOTE: Never being called by the application.
    DLLAPI u32 STDCALLAPI UpdateTextureRectangle(RendererTexture* tex, const u32* pixels, const u32* palette, const u32 x, const u32 y, const s32 width, const s32 height, const u32 size, void*)
    {
        if (tex != NULL && pixels != NULL && 0 < width && 0 < height)
        {
            return UpdateRendererTexture(tex, pixels, palette, x, y, width, height, size) ? (u32)tex : NULL;
        }

        return NULL;
    }

    // 0x60001830
    // a.k.a. THRASH_unlockwindow
    DLLAPI u32 STDCALLAPI UnlockGameWindow(const RendererModuleWindowLock* state)
    {
        if (State.Lock.IsActive && State.Lock.Surface != NULL)
        {
            if (State.Lock.Surface->Unlock(NULL) != DD_OK) { return RENDERER_MODULE_FAILURE; }

            State.Lock.Surface = NULL;
            State.Lock.IsActive = FALSE;

            if (State.Lambdas.Lambdas.LockWindow != NULL) { State.Lambdas.Lambdas.LockWindow(FALSE); }
        }

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x600011a0
    // a.k.a. THRASH_window
    DLLAPI u32 STDCALLAPI SelectGameWindow(const u32 indx)
    {
        SelectRendererStateValue(RENDERER_MODULE_STATE_SELECT_GAME_WINDOW_INDEX, (void*)indx);

        if (indx < 9) // TODO
        {
            if (9 <= State.Window.Index) // TODO
            {
                const HRESULT result = State.DX.Device->SetRenderTarget(State.DX.Surfaces.Back == NULL
                    ? State.DX.Surfaces.Main : State.DX.Surfaces.Back, 0);

                if (result != DD_OK)
                {
                    LOGERROR("Could not attach itself to PRIMARY window!\n%s", AcquireRendererMessage(result));
                }
            }
        }
        else
        {
            State.DX.Surfaces.Window = State.Windows[indx].Texture->Texture;
            State.Window.Index = indx;

            const HRESULT result = State.DX.Device->SetRenderTarget(State.DX.Surfaces.Window, 0);

            if (result != DD_OK)
            {
                LOGERROR("Could not attach itself to user render window!\n%s", AcquireRendererMessage(result));
            }
        }

        State.DX.Surfaces.Window = State.DX.Surfaces.Depth;

        if (indx != 3 && indx != 5) // TODO
        {
            if (indx == 2 && State.DX.Surfaces.Active[2] == NULL) // TODO
            {
                State.DX.Surfaces.Window = State.DX.Surfaces.Active[1]; // TODO
                State.Window.Index = 1;

                return State.DX.Surfaces.Active[1] != NULL;
            }

            State.DX.Surfaces.Window = State.DX.Surfaces.Active[indx];

            if (indx == 0)
            {
                State.Window.Index = 0;
                State.DX.Surfaces.Window = NULL;

                return FALSE;
            }
        }

        State.Window.Index = indx;

        return State.DX.Surfaces.Window != NULL;
    }

    // 0x60003c00
    // a.k.a. THRASH_writerect
    DLLAPI u32 STDCALLAPI WriteRectangle(const u32 x, const u32 y, const u32 width, const u32 height, const u32* data)
    {
        RendererModuleWindowLock* state = RendererLock(RENDERER_LOCK_WRITE);

        if (state == NULL) { return RENDERER_MODULE_FAILURE; }

        const u32 multiplier = (state->Format == RENDERER_PIXEL_FORMAT_32_BIT)
            ? 4 : (state->Format == RENDERER_PIXEL_FORMAT_24_BIT) ? 3 : 2; // TODO
        const u32 length = multiplier * width;

        for (u32 xx = 0; xx < height; xx++)
        {
            const addr address = (xx * state->Stride) + (state->Stride * y) + (multiplier * x);

            CopyMemory((void*)((addr)state->Data + address), &data[xx * length], length);
        }

        return UnlockGameWindow(state);
    }
}