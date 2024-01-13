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

        ModuleDescriptor.Caps = RENDERER_MODULE_CAPS_UNKNOWN | RENDERER_MODULE_CAPS_WINDOWED | RENDERER_MODULE_CAPS_LINE_WIDTH
            | (((State.Device.Capabilities.IsPowerOfTwoTexturesHeight & 1) << 3) | ((State.Device.Capabilities.IsPowerOfTwoTexturesWidth & 1) << 2) | ((State.Device.Capabilities.IsSquareOnlyTextures & 1) << 1));

        ModuleDescriptor.MinimumTextureWidth = State.Device.Capabilities.MinTextureWidth;
        ModuleDescriptor.MaximumTextureWidth = State.Device.Capabilities.MaxTextureWidth;
        ModuleDescriptor.MultipleTextureWidth = State.Device.Capabilities.MultipleTextureWidth;
        ModuleDescriptor.MinimumTextureHeight = State.Device.Capabilities.MinTextureHeight;
        ModuleDescriptor.MaximumTextureHeight = State.Device.Capabilities.MaxTextureHeight;
        ModuleDescriptor.MultipleTextureHeight = State.Device.Capabilities.MultipleTextureHeight;
        ModuleDescriptor.MaximumSimultaneousTextures = State.Device.Capabilities.MaximumSimultaneousTextures;

        ModuleDescriptor.SubType = State.Device.SubType;

        ModuleDescriptor.Signature = RENDERER_MODULE_SIGNATURE_D3D8;

        ModuleDescriptor.Size = sizeof(RendererModuleDescriptor2);
        ModuleDescriptor.ClipAlign = 0;
        ModuleDescriptor.DXV = RENDERER_MODULE_VERSION_DX8;
        ModuleDescriptor.Author = RENDERER_MODULE_AUTHOR;
        ModuleDescriptor.ActiveTextureFormatStatesCount = MAX_USABLE_TEXTURE_FORMAT_COUNT;
        ModuleDescriptor.TextureFormatStates = RendererTextureFormatStates;
        ModuleDescriptor.ActiveUnknownValuesCount = MAX_ACTIVE_UNKNOWN_COUNT;
        ModuleDescriptor.UnknownValues = UnknownArray06;
        ModuleDescriptor.Capabilities.Capabilities = ModuleDescriptorDeviceCapabilities;

        strcpy(ModuleDescriptor.Name, RENDERER_MODULE_NAME);

        AcquireRendererModuleDescriptor((RendererModuleDescriptor*)&ModuleDescriptor, ENVIRONMENT_SECTION_NAME);

        return (RendererModuleDescriptor*)&ModuleDescriptor;
    }

    // 0x600012d0
    // a.k.a. THRASH_clearwindow
    DLLAPI u32 STDCALLAPI ClearGameWindow(void)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x600013f0
    // a.k.a. THRASH_clip
    DLLAPI u32 STDCALLAPI ClipGameWindow(const u32 x0, const u32 y0, const u32 x1, const u32 y1)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001630
    // a.k.a. THRASH_createwindow
    // NOTE: Never being called by the application.
    DLLAPI u32 STDCALLAPI CreateGameWindow(const u32 width, const u32 height, const u32 format, const u32 options)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001840
    // a.k.a. THRASH_destroywindow
    // NOTE: Never being called by the application.
    DLLAPI u32 STDCALLAPI DestroyGameWindow(const u32 indx)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60002aa0
    // a.k.a. THRASH_drawline
    DLLAPI void STDCALLAPI DrawLine(RVX* a, RVX* b)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002be0
    // a.k.a. THRASH_drawlinemesh
    DLLAPI void STDCALLAPI DrawLineMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60003000
    // a.k.a. THRASH_drawlinestrip
    DLLAPI void STDCALLAPI DrawLineStrip(const u32 count, RVX* vertexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002c70
    // a.k.a. THRASH_drawlinestrip
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawLineStrips(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002dc0
    // a.k.a. THRASH_drawpoint
    DLLAPI void STDCALLAPI DrawPoint(RVX* vertex)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002ec0
    // a.k.a. THRASH_drawpointmesh
    DLLAPI void STDCALLAPI DrawPointMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60003030
    // a.k.a. THRASH_drawpointstrip
    DLLAPI void STDCALLAPI DrawPointStrip(const u32 count, RVX* vertexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002790
    // a.k.a. THRASH_drawquad
    DLLAPI void STDCALLAPI DrawQuad(RVX* a, RVX* b, RVX* c, RVX* d)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x600029b0
    // a.k.a. THRASH_drawquadmesh
    DLLAPI void STDCALLAPI DrawQuadMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001960
    // a.k.a. THRASH_drawsprite
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawSprite(RVX* a, RVX* b)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x600019e0
    // a.k.a. THRASH_drawspritemesh
    // NOTE: Never being called by the application.
    DLLAPI void STDCALLAPI DrawSpriteMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002290
    // a.k.a. THRASH_drawtri
    DLLAPI void STDCALLAPI DrawTriangle(RVX* a, RVX* b, RVX* c)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002fd0
    // a.k.a. THRASH_drawtrifan
    DLLAPI void STDCALLAPI DrawTriangleFan(const u32 count, RVX* vertexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002650
    // a.k.a. THRASH_drawtrifan
    DLLAPI void STDCALLAPI DrawTriangleFans(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002460
    // a.k.a. THRASH_drawtrimesh
    DLLAPI void STDCALLAPI DrawTriangleMesh(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002f80
    // a.k.a. THRASH_drawtristrip
    DLLAPI void STDCALLAPI DrawTriangleStrip(const u32 count, RVX* vertexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60002510
    // a.k.a. THRASH_drawtristrip
    DLLAPI void STDCALLAPI DrawTriangleStrips(const u32 count, RVX* vertexes, const u32* indexes)
    {
        // TODO NOT IMPLEMENTED
    }

    // 0x60001300
    // a.k.a. THRASH_flushwindow
    DLLAPI u32 STDCALLAPI FlushGameWindow(void)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }
    
    // 0x600030d0
    // a.k.a. THRASH_getstate
    DLLAPI u32 STDCALLAPI AcquireState(const u32 state)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001800
    // a.k.a. THRASH_getwindowtexture
    DLLAPI RendererTexture* STDCALLAPI AcquireGameWindowTexture(const u32 indx)
    {
        // TODO NOT IMPLEMENTED

        return NULL;
    }

    // 0x60003060
    // a.k.a. THRASH_idle
    DLLAPI void STDCALLAPI Idle(void) { return; }

    // 0x60003740
    // a.k.a. THRASH_init
    DLLAPI u32 STDCALLAPI Init(void)
    {
        InitializeSettings();

        if (State.DX.Instance == NULL)
        {
            State.DX.Instance = Direct3DCreate8(D3D_SDK_VERSION);

            if (State.DX.Instance == NULL) { return RENDERER_MODULE_FAILURE; }
        }

        AcquireRendererDeviceCount();

        AcquireRendererDeviceFormats();

        InitializeTextureStateStates();

        if (State.Lambdas.Lambdas.Execute != NULL)
        {
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE, (RENDERERMODULEEXECUTECALLBACK)InitializeRendererDeviceExecute);
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES, (RENDERERMODULEEXECUTECALLBACK)InitializeRendererDeviceSurfacesExecute);
        }

        atexit(ReleaseRendererModule);

        for (u32 x = 0; x < MAX_OUTPUT_FOG_ALPHA_COUNT; x++)
        {
            RendererFogAlphas[x] = MAX_OUTPUT_FOG_ALPHA_VALUE - x;
        }

        for (u32 x = 0; x < MAX_WINDOW_COUNT; x++)
        {
            State.Windows[x].Texture = NULL;
            State.Windows[x].Stencil = NULL;

            ZeroMemory(&State.Windows[x].Details, sizeof(RendererModuleWindowDetail));
        }

        State.DX.IsInit = TRUE;

        return State.Devices.Count;
    }

    // 0x60004f10
    // a.k.a. THRASH_is
    DLLAPI u32 STDCALLAPI Is(void)
    {
        return RENDERER_MODULE_DX8_ACCELERATION_AVAILABLE;
    }

    // 0x600014c0
    // a.k.a. THRASH_lockwindow
    DLLAPI RendererModuleWindowLock* STDCALLAPI LockGameWindow(void)
    {
        // TODO NOT IMPLEMENTED

        return NULL;
    }

    // 0x60001310
    // a.k.a. THRASH_pageflip
    DLLAPI u32 STDCALLAPI ToggleGameWindow(void)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60003070
    // a.k.a. THRASH_readrect
    DLLAPI u32 STDCALLAPI ReadRectangle(const u32 x, const u32 y, const u32 width, const u32 height, u32* pixels)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60004f20
    // a.k.a. THRASH_readrect
    // NOTE: Never being called by the application.
    DLLAPI u32 STDCALLAPI ReadRectangles(const u32 x, const u32 y, const u32 width, const u32 height, u32* pixels, const u32 stride)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60003a00
    // a.k.a. THRASH_restore
    DLLAPI u32 STDCALLAPI RestoreGameWindow(void)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60003800
    // a.k.a. THRASH_selectdisplay
    DLLAPI u32 STDCALLAPI SelectDevice(const s32 indx)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60005490
    // a.k.a. THRASH_setstate
    DLLAPI u32 STDCALLAPI SelectState(const u32 state, void* value)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001a50
    // a.k.a. THRASH_settexture
    DLLAPI u32 STDCALLAPI SelectTexture(RendererTexture* tex)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x600038b0
    // a.k.a. THRASH_setvideomode
    DLLAPI u32 STDCALLAPI SelectVideoMode(const u32 mode, const u32 pending, const u32 depth)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001380
    // a.k.a. THRASH_sync
    DLLAPI u32 STDCALLAPI SyncGameWindow(const u32 type)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x600077c0
    // a.k.a. THRASH_talloc
    DLLAPI RendererTexture* STDCALLAPI AllocateTexture(const u32 width, const u32 height, const u32 format, const u32 options, const u32 state)
    {
        // TODO NOT IMPLEMENTED

        return NULL;
    }

    // 0x60007950
    // a.k.a. THRASH_tfree
    DLLAPI u32 STDCALLAPI ReleaseTexture(RendererTexture* tex)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60007a80
    // a.k.a. THRASH_treset
    DLLAPI u32 STDCALLAPI ResetTextures(void)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x600079e0
    // a.k.a. THRASH_tupdate
    DLLAPI RendererTexture* STDCALLAPI UpdateTexture(RendererTexture* tex, const u32* pixels, const u32* palette)
    {
        // TODO NOT IMPLEMENTED

        return NULL;
    }

    // 0x60007a20
    // a.k.a. THRASH_tupdaterect
    DLLAPI RendererTexture* STDCALLAPI UpdateTextureRectangle(RendererTexture* tex, const u32* pixels, const u32* palette, const s32 x, const s32 y, const s32 width, const s32 height, const s32 size, const s32 level)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }
    
    // 0x600015c0
    // a.k.a. THRASH_unlockwindow
    DLLAPI u32 STDCALLAPI UnlockGameWindow(const RendererModuleWindowLock* state)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001160
    // a.k.a. THRASH_window
    DLLAPI u32 STDCALLAPI SelectGameWindow(const u32 indx)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x600030a0
    // a.k.a. THRASH_writerect
    DLLAPI u32 STDCALLAPI WriteRectangle(const u32 x, const u32 y, const u32 width, const u32 height, const u32* pixels)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60004fd0
    // a.k.a. THRASH_writerect
    // NOTE: Never being called by the application.
    DLLAPI u32 STDCALLAPI WriteRectangles(const u32 x, const u32 y, const u32 width, const u32 height, const u32* pixels, const u32 stride)
    {
        // TODO NOT IMPLEMENTED

        return RENDERER_MODULE_FAILURE;
    }
}