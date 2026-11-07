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
#include "Mathematics.Basic.hxx"
#include "Module.hxx"
#include "RendererValues.hxx"

#include <math.h>
#include <stdlib.h>

using namespace Mathematics;
using namespace Renderer;
using namespace RendererModuleValues;

namespace RendererModule
{
    // 0x60002330
    // a.k.a. THRASH_about
    DLLAPI RendererModuleDescriptor* STDCALLAPI AcquireDescriptor(void)
    {
        ModuleDescriptor.Signature = RENDERER_MODULE_SIGNATURE_IRTS;
        ModuleDescriptor.Size = sizeof(RendererModuleDescriptor);
        ModuleDescriptor.Version = RENDERER_MODULE_VERSION_104;

        ModuleDescriptor.MinimumTextureWidth = 16;
        ModuleDescriptor.MaximumTextureWidth = 256;
        ModuleDescriptor.MultipleTextureWidth = 1;

        ModuleDescriptor.Caps = ModuleDescriptor.Caps
            | RENDERER_MODULE_CAPS_SOFTWARE | RENDERER_MODULE_CAPS_TEXTURE_HEIGHT_POW2
            | RENDERER_MODULE_CAPS_TEXTURE_WIDTH_POW2 | RENDERER_MODULE_CAPS_TEXTURE_SQUARE | RENDERER_MODULE_CAPS_LINE_WIDTH;

        ModuleDescriptor.MinimumTextureHeight = 16;
        ModuleDescriptor.MaximumTextureHeight = 256;
        ModuleDescriptor.MultipleTextureHeight = 1;

        ModuleDescriptor.MemorySize = 0;
        ModuleDescriptor.MemoryType = MIN_DEVICE_AVAIABLE_VIDEO_MEMORY;

        ModuleDescriptor.ClipAlign = 0;

        ModuleDescriptor.ActiveTextureFormatStatesCount = MAX_ACTIVE_USABLE_TEXTURE_FORMAT_COUNT;
        ModuleDescriptor.ActiveUnknownValuesCount = MAX_ACTIVE_UNKNOWN_COUNT;

        ModuleDescriptor.UnknownValues = UnknownArray06;
        ModuleDescriptor.TextureFormatStates = RendererTextureFormatStates;

        strcpy(ModuleDescriptor.Name, RENDERER_MODULE_NAME);

        ModuleDescriptor.SubType = 0;

        return &ModuleDescriptor;
    }
    
    // 0x60003470
    // a.k.a. THRASH_clearwindow
    DLLAPI u32 STDCALLAPI ClearGameWindow(void)
    {
        return RendererClearGameWindow();
    }

    // 0x60003490
    // a.k.a. THRASH_clip
    DLLAPI u32 STDCALLAPI ClipGameWindow(const s32 x0, const s32 y0, const s32 x1, const s32 y1)
    {
        const s32 x = x0 < 0 ? 0 : x0;
        const s32 y = y0 < 0 ? 0 : y0;

        const s32 width = State.Renderer.Settings.Width < x1 ? State.Renderer.Settings.Width : x1;
        const s32 height = State.Renderer.Settings.Height < y1 ? State.Renderer.Settings.Height : y1;

        State.ViewPort.X0 = x;
        State.ViewPort.Y0 = y;
        State.ViewPort.Width = width - x;
        State.ViewPort.X1 = width - 1;
        State.ViewPort.Height = height - y;
        State.ViewPort.Y1 = height - 1;

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60005150
    // a.k.a. THRASH_drawline
    DLLAPI void STDCALLAPI DrawLine(RVX* a, RVX* b)
    {
        RTLVX* begin = (RTLVX*)a;
        RTLVX* end = (RTLVX*)b;

        if (end->XYZ.X < begin->XYZ.X || end->XYZ.Y < begin->XYZ.Y)
        {
            State.Vertexes.Start = begin;

            begin = (RTLVX*)b;
            end = (RTLVX*)a;
        }

        CalculateVertexColor((s32)end->XYZ.X, (s32)end->XYZ.Y, end->Color);

        // Bresenham's line algorithm

        const int x0 = (int)begin->XYZ.X;
        const int y0 = (int)begin->XYZ.Y;
        const int x1 = (int)end->XYZ.X;
        const int y1 = (int)end->XYZ.Y;

        const int dx = abs(x1 - x0);
        const int dy = abs(y1 - y0);
        const int sx = (x0 < x1) ? 1 : -1;
        const int sy = (y0 < y1) ? 1 : -1;

        int err = dx - dy;

        int x = x0;
        int y = y0;

        while (TRUE)
        {
            u16* pixels = (u16*)((addr)State.Renderer.Surface.Surface + RendererSurfaceStride * y);
            pixels[x] = VertexColor;

            if (x == x1 && y == y1) { break; }

            int e2 = 2 * err;
            if (e2 > -dy)
            {
                err -= dy;
                x += sx;
            }

            if (e2 < dx)
            {
                err += dx;
                y += sy;
            }
        }
    }

    // 0x60005320
    // a.k.a. THRASH_drawlinemesh
    DLLAPI void STDCALLAPI DrawLineMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawLine((RVX*)&vs[indexes[x * 2 + 0]], (RVX*)&vs[indexes[x * 2 + 1]]); }
    }

    // 0x60005390
    // a.k.a. THRASH_drawlinestrip
    DLLAPI void STDCALLAPI DrawLineStrip(const u32 count, RVX* vertexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawLine((RVX*)&vs[x + 0], (RVX*)&vs[x + 1]); }
    }

    // 0x60005360
    // a.k.a. THRASH_drawlinestrip
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawLineStrips(const u32 count, RVX* vertexes, const u32* indexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawLine((RVX*)&vs[x + 0], (RVX*)&vs[x + 1]); }
    }

    // 0x600053c0
    // a.k.a. THRASH_drawpoint
    DLLAPI void STDCALLAPI DrawPoint(RVX* vertex)
    {
        const s32 x = (s32)((RTLVX*)vertex)->XYZ.X;
        const s32 y = (s32)((RTLVX*)vertex)->XYZ.Y;
        const u32 color = (s32)((RTLVX*)vertex)->Color;

        CalculateVertexColor(x, y, color);

        u16* pixels = (u16*)((addr)State.Renderer.Surface.Surface + RendererSurfaceStride * y);

        pixels[x] = VertexColor;
    }

    // 0x60005410
    // a.k.a. THRASH_drawpointmesh
    DLLAPI void STDCALLAPI DrawPointMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawPoint((RVX*)&vs[indexes[x]]); }
    }

    // 0x60005450
    // a.k.a. THRASH_drawpointstrip
    DLLAPI void STDCALLAPI DrawPointStrip(const u32 count, RVX* vertexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawPoint((RVX*)&vs[x]); }
    }

    // 0x60004f20
    // a.k.a. THRASH_drawquad
    DLLAPI void STDCALLAPI DrawQuad(RVX* a, RVX* b, RVX* c, RVX* d)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60004fd0
    // a.k.a. THRASH_drawquadmesh
    DLLAPI void STDCALLAPI DrawQuadMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++)
        {
            DrawQuad((RVX*)&vs[indexes[4 * x + 0]], (RVX*)&vs[indexes[4 * x + 1]],
                (RVX*)&vs[indexes[4 * x + 2]], (RVX*)&vs[indexes[4 * x + 3]]);
        }
    }

    // 0x60004e20
    // a.k.a. THRASH_drawtri
    DLLAPI void STDCALLAPI DrawTriangle(RVX* a, RVX* b, RVX* c)
    {
        if ((AcquireNormal((f32x3*)a, (f32x3*)b, (f32x3*)c) & RENDERER_CULL_MODE_COUNTER_CLOCK_WISE) == CullMode)
        {
            RTLVX* va = (RTLVX*)a;
            RTLVX* vb = (RTLVX*)b;
            RTLVX* vc = (RTLVX*)c;

            const u32 alpha = ((va->Color >> 24) + (vb->Color >> 24) + (vc->Color >> 24)) / 3;

            State.Renderer.Lambdas.Active->Lambdas[alpha >> 5](va, vb, vc);
        }
    }

    // 0x60005110
    // a.k.a. THRASH_drawtrifan
    DLLAPI void STDCALLAPI DrawTriangleFan(const u32 count, RVX* vertexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++) { DrawTriangle((RVX*)&vs[0], (RVX*)&vs[x + 1], (RVX*)&vs[x + 2]); }
    }

    // 0x600050d0
    // a.k.a. THRASH_drawtrifan
    DLLAPI void STDCALLAPI DrawTriangleFans(const u32 count, RVX* vertexes, const u32* indexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++)
        {
            DrawTriangle((RVX*)&vs[0], (RVX*)&vs[3 * x + 1], (RVX*)&vs[3 * x + 2]);
        }
    }

    // 0x60004ed0
    // a.k.a. THRASH_drawtrimesh
    DLLAPI void STDCALLAPI DrawTriangleMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        const RTLVX* vs = (RTLVX*)vertexes;

        for (u32 x = 0; x < count; x++)
        {
            DrawTriangle((RVX*)&vs[indexes[3 * x + 0]],
                (RVX*)&vs[indexes[3 * x + 1]], (RVX*)&vs[indexes[3 * x + 2]]);
        }
    }

    // 0x60005080
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

    // 0x60005030
    // a.k.a. THRASH_drawtristrip
    DLLAPI void STDCALLAPI DrawTriangleStrips(const u32 count, RVX* vertexes, const u32* indexes)
    {
        DrawTriangleStrip(count, vertexes);
    }

    // 0x60003480
    // a.k.a. THRASH_flushwindow
    DLLAPI u32 STDCALLAPI FlushGameWindow(void) { return RENDERER_MODULE_SUCCESS; }

    // 0x60003480
    // a.k.a. THRASH_idle
    DLLAPI void STDCALLAPI Idle(void) { }

    // 0x60002bd0
    // a.k.a. THRASH_init
    DLLAPI u32 STDCALLAPI Init(void)
    {
        SelectRendererLambdas();

        State.Renderer.Lambdas.IndexGroup = 0;

        SelectIndexedRendererLambdas();

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60002f50
    // a.k.a. THRASH_is
    DLLAPI u32 STDCALLAPI Is(void) { return RENDERER_MODULE_SW_ACCELERATION_AVAILABLE; }

    // 0x60003530
    // a.k.a. THRASH_lockwindow
    DLLAPI RendererModuleWindowLock* STDCALLAPI LockGameWindow(void)
    {
        if (State.DX.Surfaces.Window == State.DX.Surfaces.Active[2])
        {
            State.Lock.State.Data = AcquireRendererSurface();
            State.Lock.State.Stride = RendererSurfaceStride;

            State.Lock.State.Format = State.DX.Surfaces.Bits == GRAPHICS_BITS_PER_PIXEL_15
                ? RENDERER_PIXEL_FORMAT_R5G5B5 : RENDERER_PIXEL_FORMAT_R5G6B5;

            State.Lock.State.Width = State.Window.Width;
            State.Lock.State.Height = State.Window.Height;

            return &State.Lock.State;
        }

        if (State.DX.Surfaces.Window == State.DX.Surfaces.Active[1])
        {
            RECT rect;
            GetClientRect(State.Window.HWND, &rect);

            POINT point;
            ZeroMemory(&point, sizeof(POINT));

            ClientToScreen(State.Window.HWND, &point);
            OffsetRect(&rect, point.x, point.y);

            State.Lambdas.Lambdas.LockWindow(TRUE);

            State.Lock.Surface = State.DX.Surfaces.Window;

            DDSURFACEDESC desc;
            ZeroMemory(&desc, sizeof(DDSURFACEDESC));

            desc.dwSize = sizeof(DDSURFACEDESC);

            State.DX.Code = State.DX.Surfaces.Window->Lock(NULL, &desc, DDLOCK_WAIT, NULL);

            if (State.DX.Code == DD_OK)
            {
                State.Lock.State.Data = (void*)((addr)desc.lpSurface + (addr)(rect.left * sizeof(u16) + rect.top * desc.lPitch));

                State.Lock.State.Width = State.Window.Width;
                State.Lock.State.Height = State.Window.Height;

                State.Lock.State.Stride = desc.lPitch;

                if (desc.ddpfPixelFormat.dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_16)
                {
                    State.Lock.State.Format = desc.ddpfPixelFormat.dwGBitMask == 0x7e0
                        ? RENDERER_PIXEL_FORMAT_R5G6B5 : RENDERER_PIXEL_FORMAT_R5G5B5;
                }
                else if (desc.ddpfPixelFormat.dwRGBBitCount == GRAPHICS_BITS_PER_PIXEL_32) { State.Lock.State.Format = RENDERER_PIXEL_FORMAT_R8G8B8; }

                State.Lock.IsActive = TRUE;

                return &State.Lock.State;
            }
        }

        return NULL;
    }

    // 0x60003750
    // a.k.a. THRASH_pageflip
    DLLAPI void STDCALLAPI ToggleGameWindow(void)
    {
        State.Lambdas.Lambdas.LockWindow(TRUE);

        DDSURFACEDESC desc;
        ZeroMemory(&desc, sizeof(DDSURFACEDESC));
        desc.dwSize = sizeof(DDSURFACEDESC);

        State.DX.Code = State.DX.Surfaces.Active[1]->Lock(NULL, &desc, DDLOCK_WAIT, NULL);

        if (State.DX.Code == DD_OK)
        {
            State.Window.Stride = desc.lPitch;
            if (!State.Settings.IsWindowMode)
            {
                CopyViewPortSurface(desc.lpSurface);
            }
            else
            {
                RECT rect;
                GetClientRect(State.Window.HWND, &rect);

                POINT point;
                ZeroMemory(&point, sizeof(POINT));

                ClientToScreen(State.Window.HWND, &point);
                OffsetRect(&rect, point.x, point.y);

                const void* src = AcquireRendererSurface();
                void* dst = desc.lpSurface;

                for (u32 y = 0; y < State.Renderer.Settings.Height; y++)
                {
                    CopyMemory(dst, src, RendererSurfaceStride);

                    src = (void*)((addr)src + RendererSurfaceStride);
                    dst = (void*)((addr)dst + desc.lPitch);
                }
            }

            State.DX.Surfaces.Active[1]->Unlock(desc.lpSurface);
            State.Lambdas.Lambdas.LockWindow(FALSE);
        }
        else
        {
            OutputDebugStringA("lock failed on writebuffer\n");
        }
    }

    // 0x60002f60
    // a.k.a. THRASH_readrect
    DLLAPI u32 STDCALLAPI ReadRectangle(const u32 x, const u32 y, const u32 width, const u32 height, u32* pixels)
    {
        const RendererModuleWindowLock* state = LockGameWindow();

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

    // 0x60002bf0
    // a.k.a. THRASH_restore
    DLLAPI u32 STDCALLAPI RestoreGameWindow(void)
    {
        if (State.Lock.IsActive) { UnlockGameWindow(NULL); }

        RendererVideoMode = DEFAULT_RENDERER_MODE;

        return ReleaseRendererWindow() == DD_OK;
    }

    // 0x60002c20
    // a.k.a. THRASH_selectdisplay
    DLLAPI u32 STDCALLAPI SelectDevice(const s32 indx)
    {
        RendererDeviceIndex = indx;

        s32 index = DEFAULT_DEVICE_INDEX;

        DirectDrawEnumerateA(EnumerateRendererDevices, &index);

        return index == INVALID_DEVICE_INDEX;
    }

    // 0x600030c0
    // a.k.a. THRASH_setstate
    DLLAPI u32 STDCALLAPI SelectState(const u32 state, void* value)
    {
        switch (state) {
        case RENDERER_MODULE_STATE_NONE:
        case RENDERER_MODULE_STATE_SELECT_DITHER_STATE:
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_FILTER_STATE:
        case RENDERER_MODULE_STATE_SELECT_ALPHA_BLEND_STATE:
        case RENDERER_MODULE_STATE_SELECT_TEXTURE_MIP_FILTER_STATE:
        case RENDERER_MODULE_STATE_SELECT_CHROMATIC_COLOR:
        case RENDERER_MODULE_STATE_SELECT_FOG_DENSITY:
        case RENDERER_MODULE_STATE_SELECT_FOG_COLOR:
        case RENDERER_MODULE_STATE_SELECT_LINE_WIDTH:
        case RENDERER_MODULE_STATE_SELECT_FLAT_FANS_STATE:
        case RENDERER_MODULE_STATE_SELECT_FOG_STATE:
        case RENDERER_MODULE_STATE_SELECT_FOG_START:
        case RENDERER_MODULE_STATE_SELECT_FOG_END: { return RENDERER_MODULE_SUCCESS; }
        case RENDERER_MODULE_STATE_SELECT_TEXTURE:
        {
            SelectTexture((RendererTexture*)value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_SELECT_CULL_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_CULL_NONE:
            {
                CullMode = RENDERER_CULL_MODE_NONE;

                return RENDERER_MODULE_SUCCESS;
            }
            case RENDERER_MODULE_CULL_COUNTER_CLOCK_WISE:
            {
                CullMode = RENDERER_CULL_MODE_COUNTER_CLOCK_WISE;

                return RENDERER_MODULE_SUCCESS;
            }
            case RENDERER_MODULE_CULL_CLOCK_WISE:
            {
                CullMode = RENDERER_CULL_MODE_CLOCK_WISE;

                return RENDERER_MODULE_SUCCESS;
            }
            }

            return RENDERER_MODULE_FAILURE;
        }
        case RENDERER_MODULE_STATE_SELECT_MATERIAL:
        {
            SelectRendererMaterial((u32)value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_SELECT_SHADE_STATE:
        {
            const u32 depth = (u32)value;
            if (depth == RENDERER_MODULE_DEPTH_INACTIVE
                || depth == RENDERER_MODULE_DEPTH_ACTIVE)
            {
                SetRendererShadeMode(depth);
            }

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_STATE:
        {
            State.Settings.IsWindowMode = (BOOL)value;

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_SELECT_LAMBDAS:
        {
            CopyMemory(&State.Lambdas.Lambdas, value, sizeof(RendererModuleLambdaContainer));

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW_LOCK_STATE:
        {
            if (value == NULL) { return RENDERER_MODULE_SUCCESS; }

            const RendererModuleWindowLock* lock = (RendererModuleWindowLock*)value;

            State.Renderer.Surface.Surface = lock->Data;
            RendererSurfaceStride = lock->Stride;
            State.Renderer.Settings.Width = lock->Width;
            State.Renderer.Settings.Height = lock->Height;

            if (lock->Format == RENDERER_PIXEL_FORMAT_R5G6B5)
            {
                State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_16;
            }
            else if (lock->Format == RENDERER_PIXEL_FORMAT_A1R5G5B5
                || lock->Format == RENDERER_PIXEL_FORMAT_R5G5B5)
            {
                State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_15;
            }
            else
            {
                Message("STATE_RENDERINFO -- texture type not supported.\n");
            }

            SelectRendererColorMasks(State.DX.Surfaces.Bits);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_SELECT_BLEND_STATE_ALTERNATIVE:
        {
            SetRendererUnknownMode1((u32)value);
            SetRendererUnknownMode2((u32)value);
            SetRendererUnknownMode3((u32)value);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_701:
        {
            SetRendererUnknownFunction(value);
            SetActiveRendererLambda(FUN_60034050);

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_702:
        {
            switch ((u32)value)
            {
            case 0: // TODO
            {
                SetRendererUnknownMode4(0, 0);

                return RENDERER_MODULE_SUCCESS;
            }
            case 3: // TODO
            {
                SetRendererUnknownMode4(3, 1);

                return RENDERER_MODULE_SUCCESS;
            }
            case 4: // TODO
            {
                SetRendererUnknownMode4(4, 1);

                return RENDERER_MODULE_SUCCESS;
            }
            case 7: // TODO
            {
                SetRendererUnknownMode4(7, 1);

                return RENDERER_MODULE_SUCCESS;
            }
            default:
            {
                SelectRendererLambdas();

                return RENDERER_MODULE_SUCCESS;
            }
            }

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_703:
        {
            State.UnknownQuadValue = (u32)value;

            return RENDERER_MODULE_SUCCESS;
        }
        case RENDERER_MODULE_STATE_TEXTURE_PALETTE_COUNT:
        {
            TexturePaletteCount = (s32)value;
            CurrentTexturePaletteCount = (s32)value;

            return RENDERER_MODULE_FAILURE;
        }
        case RENDERER_MODULE_STATE_CURRENT_TEXTURE_PALETTE_COUNT:
        {
            CurrentTexturePaletteCount = (s32)value;

            return RENDERER_MODULE_FAILURE;
        }
        case RENDERER_MODULE_STATE_706:
        {
            State.Renderer.Lambdas.IndexGroup = ((u32)value) == 0 ? 0 : 1;
            SelectIndexedRendererLambdas();

            return RENDERER_MODULE_SUCCESS;
        }
        }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60003f70
    // a.k.a. THRASH_settexture
    DLLAPI u32 STDCALLAPI SelectTexture(RendererTexture* tex)
    {
        if (tex == NULL)
        {
            State.Renderer.Lambdas.Index = 0;
            State.Textures.Selected.UnknownSize = 0;
        }
        else
        {
            State.Textures.Selected.Pixels = tex->Pixels;
            State.Textures.Selected.Palette = tex->Palette;
            State.Textures.Selected.Width = tex->Width;
            State.Textures.Selected.WidthAsFloat = (f32)State.Textures.Selected.Width;

            State.Textures.Selected.WidthRatio1 = 1.0f / (State.Textures.Selected.WidthAsFloat * 2.0f);
            State.Textures.Selected.WidthSquaredAsFloat =
                State.Textures.Selected.WidthAsFloat * State.Textures.Selected.WidthAsFloat;

            State.Textures.Selected.Height = tex->Height;

            State.Renderer.Lambdas.Index = tex->Format2;

            State.Textures.Selected.WidthOffset =
                (State.Textures.Selected.Width == 0 || State.Textures.Selected.Width == 1)
                ? 0
                : ((State.Textures.Selected.Width - 1) * 0x8000);

            switch (State.Textures.Selected.Width)
            {
            case 1: { State.Textures.Selected.UnknownSize = 0; break; }
            case 2: { State.Textures.Selected.UnknownSize = 0x10000; break; }
            case 4: { State.Textures.Selected.UnknownSize = 0x60000; break; }
            case 8: { State.Textures.Selected.UnknownSize = 0x1c0000; break; }
            case 16: { State.Textures.Selected.UnknownSize = 0x780000; break; }
            case 32: { State.Textures.Selected.UnknownSize = 0x1f00000; break; }
            case 64: { State.Textures.Selected.UnknownSize = 0x7e00000; break; }
            case 128: { State.Textures.Selected.UnknownSize = 0x1fc00000; break; }
            case 256: { State.Textures.Selected.UnknownSize = 0x7f800000; break; }
            }
        }

        SelectIndexedRendererLambdas();

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60002c80
    // a.k.a. THRASH_setvideomode
    DLLAPI u32 STDCALLAPI SelectVideoMode(const s32 mode, const u32 pending, const u32 depth)
    {
        if (mode == RendererVideoMode) { return RENDERER_MODULE_SUCCESS; }

        if (State.DX.Instance == NULL) { InitializeRendererDeviceLambdas(); }
        else if (State.Lock.IsActive) { UnlockGameWindow(NULL); }

        if (State.DX.Instance == NULL) { return RENDERER_MODULE_FAILURE; }

        SetForegroundWindow(State.Window.HWND);
        PostMessageA(State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES, mode, pending);
        WaitForSingleObject(State.Mutex, INFINITE);

        if (State.DX.Code != DD_OK) { Message("SOFTTRI_setdisplaymode - ERROR CODE (softtristatus) %8x\n", State.DX.Code); }

        SelectRendererSettings(State.Window.Width, State.Window.Height, State.DX.Surfaces.Bits);

        SelectGameWindow(1); // TODO

        RendererModuleWindowLock* lock = LockGameWindow();

        if (lock != NULL)
        {
            switch (lock->Format)
            {
            case RENDERER_PIXEL_FORMAT_P8: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_8; break; }
            case RENDERER_PIXEL_FORMAT_R5G5B5:
            case RENDERER_PIXEL_FORMAT_A1R5G5B5: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_15; break; }
            case RENDERER_PIXEL_FORMAT_R8G8B8: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_24; break; }
            case RENDERER_PIXEL_FORMAT_A8R8G8B8: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_32; break; }
            default: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_16; break; }
            }

            UnlockGameWindow(NULL);

            State.DX.Bits = State.DX.Surfaces.Bits;

            SelectRendererColorMasks(State.DX.Surfaces.Bits);
        }

        return State.DX.Code == DD_OK ? RENDERER_MODULE_SUCCESS : RENDERER_MODULE_FAILURE;
    }

    // 0x60003460
    // a.k.a. THRASH_sync
    DLLAPI u32 STDCALLAPI SyncGameWindow(const u32) { return RENDERER_MODULE_FAILURE; }

    // 0x60003c90
    // a.k.a. THRASH_talloc
    DLLAPI RendererTexture* STDCALLAPI AllocateTexture(const u32 width, const u32 height, const u32 format, const BOOL options, const u32 state)
    {
        RendererTexture* texture = (RendererTexture*)malloc(sizeof(RendererTexture));

        if (texture != NULL)
        {
            texture->Width = width;
            texture->Height = height;
            texture->Format1 = format;
            texture->Format2 = format;

            texture->Previous = State.Textures.Current;
            State.Textures.Current = texture;

            switch (format) {
            case RENDERER_PIXEL_FORMAT_P8:
            {
                texture->Bits = GRAPHICS_BITS_PER_PIXEL_8;
                texture->PaletteCount = TexturePaletteCount;
                texture->Stride = (texture->Bits >> 3) * width;
                texture->Size = texture->Stride * height;

                const size_t pal_size = TexturePaletteCount * RENDERER_PALETTE_COLOR_COUNT * sizeof(u32);

                texture->Data = (u16*)malloc(texture->Size + pal_size + 0x20 + width);
                texture->Palette = (u16*)(((addr)texture->Data + 0x20) & 0xFFFFFFE0);
                texture->Pixels = (u16*)((addr)texture->Palette + pal_size);

                break;
            }
            case RENDERER_PIXEL_FORMAT_R5G5B5:
            case RENDERER_PIXEL_FORMAT_R5G6B5:
            {
                texture->Bits = GRAPHICS_BITS_PER_PIXEL_16;
                texture->Stride = (texture->Bits >> 3) * width;
                texture->Size = texture->Stride * height;

                texture->Data = (u16*)malloc(texture->Stride * 2 + texture->Size + 0x20);
                texture->Palette = NULL;
                texture->Pixels = (u16*)((((addr)texture->Data + 0x20) & 0xFFFFFFE0) + texture->Stride);

                break;
            }
            case RENDERER_PIXEL_FORMAT_R4G4B4:
            {
                texture->Bits = GRAPHICS_BITS_PER_PIXEL_32;
                texture->Stride = (texture->Bits >> 3) * width;
                texture->Size = texture->Stride * height;

                texture->Data = (u16*)malloc(texture->Stride * 2 + texture->Size + 0x20);
                texture->Palette = NULL;
                texture->Pixels = (u16*)((((addr)texture->Data + 0x20) & 0xFFFFFFE0) + texture->Stride);

                break;
            }
            }
        }

        return texture;
    }

    // 0x600040c0
    // a.k.a. THRASH_treset
    DLLAPI u32 STDCALLAPI ResetTextures(void)
    {
        while (State.Textures.Current != NULL)
        {
            if (State.Textures.Current->Data != NULL)
            {
                free(State.Textures.Current->Data);
            }

            RendererTexture* prev = State.Textures.Current->Previous;

            free(State.Textures.Current);

            State.Textures.Current = prev;
        }

        State.Textures.Current = NULL;

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60003e00
    // a.k.a. THRASH_tupdate
    DLLAPI RendererTexture* STDCALLAPI UpdateTexture(RendererTexture* tex, const u32* pixels, const u32* palette)
    {
        tex->Format2 = tex->Format1;

        switch (tex->Format1)
        {
        case RENDERER_PIXEL_FORMAT_P8:
        {
            if (pixels != NULL)
            {
                if (RendererSetPaletteTexturePixels(tex, pixels))
                {
                    tex->Format2 = RENDERER_PIXEL_FORMAT_1;
                }
            }

            if (palette != NULL)
            {
                RendererSetPaletteTexturePalette(tex, palette);
            }

            break;
        }
        case RENDERER_PIXEL_FORMAT_R5G5B5:
        case RENDERER_PIXEL_FORMAT_R5G6B5:
        {
            if (pixels == NULL)
            {
                return NULL;
            }

            if (State.DX.Surfaces.Bits == GRAPHICS_BITS_PER_PIXEL_16)
            {
                if (tex->Format2 == RENDERER_PIXEL_FORMAT_R5G6B5)
                {
                    CopyMemory(tex->Pixels, pixels, tex->Size);
                }
                else
                {
                    if (RendererSetTexturePixelsA1R5G6B5(tex->Pixels, (u16*)pixels, tex->Size / sizeof(u16)))
                    {
                        tex->Format1 = RENDERER_PIXEL_FORMAT_R5G5B5;

                        break;
                    }
                }
            }
            else if (tex->Format2 == RENDERER_PIXEL_FORMAT_R5G6B5)
            {
                RendererSetTexturePixelsR5G6B5(tex->Pixels, (u16*)pixels, tex->Size / sizeof(u16));
                tex->Format2 = RENDERER_PIXEL_FORMAT_R5G6B5;

                break;
            }
            else
            {
                if (RendererSetTexturePixelsR5G5B5(tex->Pixels, (u16*)pixels, tex->Size / sizeof(u16)))
                {
                    tex->Format1 = RENDERER_PIXEL_FORMAT_R5G5B5;

                    break;
                }
            }

            tex->Format1 = RENDERER_PIXEL_FORMAT_R5G6B5;

            break;
        }
        case RENDERER_PIXEL_FORMAT_R4G4B4:
        {
            if (pixels != NULL)
            {
                RendererSetTexturePixelsA4R4G4B4((u32*)tex->Pixels, (u16*)pixels, tex->Size / sizeof(u32));
                CopyMemory((void*)((addr)tex->Pixels + tex->Size), tex->Pixels, tex->Stride);
            }
        }
        }

        return tex;
    }

    // 0x600036d0
    // a.k.a. THRASH_unlockwindow
    DLLAPI u32 STDCALLAPI UnlockGameWindow(const RendererModuleWindowLock* state)
    {
        if (State.DX.Surfaces.Window == State.DX.Surfaces.Active[2]) { return RENDERER_MODULE_SUCCESS; }

        if (State.DX.Surfaces.Window != State.DX.Surfaces.Active[1]) { return RENDERER_MODULE_FAILURE; }

        if (!State.Lock.IsActive || State.Lock.Surface == NULL)
        {
            Message("SOFTTRI_unlockwindow - ATTEMPTING TO UNLOCK WINDOW THAT ISN`T LOCKED.\n");

            return RENDERER_MODULE_SUCCESS;
        }

        State.Lock.Surface->Unlock(state == NULL ? NULL : state->Data);

        State.Lock.Surface = NULL;
        State.Lock.IsActive = FALSE;

        State.Lambdas.Lambdas.LockWindow(FALSE);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x600033f0
    // a.k.a. THRASH_window
    DLLAPI u32 STDCALLAPI SelectGameWindow(const u32 indx)
    {
        State.DX.Surfaces.Window = State.DX.Surfaces.Active[indx];

        if (State.DX.Surfaces.Window == State.DX.Surfaces.Active[2])
        {
            State.Renderer.Surface.Surface = State.Renderer.Active.Surface;

            RendererSurfaceStride = State.Renderer.Active.Stride;

            State.Renderer.Settings.Width = State.Renderer.Active.Width;
            State.Renderer.Settings.Height = State.Renderer.Active.Height;

            State.DX.Surfaces.Bits = State.DX.Bits;

            SelectRendererColorMasks(State.DX.Bits);
        }

        return State.DX.Surfaces.Window == NULL ? RENDERER_MODULE_FAILURE : RENDERER_MODULE_SUCCESS;
    }

    // 0x60003000
    // a.k.a. THRASH_writerect
    DLLAPI u32 STDCALLAPI WriteRectangle(const u32 x, const u32 y, const u32 width, const u32 height, const u32* pixels)
    {
        const RendererModuleWindowLock* state = LockGameWindow();

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
