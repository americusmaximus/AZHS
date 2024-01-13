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
#include "Renderer.hxx"
#include "RendererValues.hxx"

#include <math.h>
#include <stdio.h>

#define MAX_SETTINGS_BUFFER_LENGTH 80

using namespace RendererModuleValues;

namespace RendererModule
{
    RendererModuleState State;

    // 0x600042e0
    void ReleaseRendererModule(void)
    {
        RestoreGameWindow();
    }

    // 0x60007ea0
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

    // 0x60007f10
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

    // 0x600042f0
    u32 AcquireRendererDeviceCount(void)
    {
        State.Devices.Count = State.DX.Instance->GetAdapterCount();

        if (State.Devices.Count == 0) { return State.Devices.Count; }

        const static D3DFORMAT formats[MAX_RENDERER_DEVICE_FORMAT_COUNT] = { D3DFMT_R5G6B5, D3DFMT_X1R5G5B5 };

        u32 actual = 0;

        for (u32 x = 0; x < State.Devices.Count; x++)
        {
            BOOL found = TRUE;

            for (u32 xx = 0; xx < MAX_RENDERER_DEVICE_FORMAT_COUNT; xx++)
            {
                if (State.DX.Instance->CheckDeviceType(x, D3DDEVTYPE_HAL, formats[xx], formats[xx], FALSE) == D3D_OK) { found = TRUE; break; }
            }

            if (found)
            {
                D3DADAPTER_IDENTIFIER8 identifier;
                ZeroMemory(&identifier, sizeof(D3DADAPTER_IDENTIFIER8));

                State.DX.Instance->GetAdapterIdentifier(x, D3DENUM_NO_WHQL_LEVEL, &identifier);

                strncpy_s(State.Devices.Enumeration.Names[actual], identifier.Description, MAX_RENDERER_MODULE_DEVICE_LONG_NAME_LENGTH);

                actual = actual + 1;
            }
        }

        State.Devices.Count = actual;

        return State.Devices.Count;
    }

    // 0x60004550
    void AcquireRendererDeviceFormats(void)
    {
        ModuleDescriptor.Capabilities.Capabilities = ModuleDescriptorDeviceCapabilities;
        ZeroMemory(ModuleDescriptor.Capabilities.Capabilities, MAX_DEVICE_CAPABILITIES_COUNT * sizeof(RendererModuleDescriptorDeviceCapabilities));

        BOOL found = FALSE;
        u32 actual = MIN_ACTUAL_DEVICE_CAPABILITIES_INDEX;
        u32 indx = RENDERER_RESOLUTION_MODE_NONE;

        const u32 count = State.DX.Instance->GetAdapterModeCount(State.Device.Index);

        for (u32 x = 0; x < count; x++)
        {
            D3DDISPLAYMODE mode;
            State.DX.Instance->EnumAdapterModes(State.Device.Index, x, &mode);

            const u32 format = AcquireRendererDeviceFormat(mode.Format);
            const u32 bits = AcquireRendererDeviceFormatSize(mode.Format, RendererDeviceFormatSizeBits);

            if (format != RENDERER_PIXEL_FORMAT_NONE && bits != 0
                && (GRAPHICS_RESOLUTION_640 - 1) < mode.Width && (GRAPHICS_RESOLUTION_480 - 1) < mode.Height)
            {
                if (mode.Width == GRAPHICS_RESOLUTION_640 && mode.Height == GRAPHICS_RESOLUTION_480 && bits == GRAPHICS_BITS_PER_PIXEL_16)
                {
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Width = mode.Width;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Height = mode.Height;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Bits = bits;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Format = format;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].IsActive = TRUE;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Unk03 = 3;
                    ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Unk04 = 2;
                }
                else if (actual < MAX_DEVICE_CAPABILITIES_COUNT)
                {
                    ModuleDescriptor.Capabilities.Capabilities[actual].Width = mode.Width;
                    ModuleDescriptor.Capabilities.Capabilities[actual].Height = mode.Height;
                    ModuleDescriptor.Capabilities.Capabilities[actual].Bits = bits;
                    ModuleDescriptor.Capabilities.Capabilities[actual].Format = format;
                    ModuleDescriptor.Capabilities.Capabilities[actual].IsActive = TRUE;
                    ModuleDescriptor.Capabilities.Capabilities[actual].Unk03 = 3;
                    ModuleDescriptor.Capabilities.Capabilities[actual].Unk04 = 2;

                    actual = actual + 1;
                }
            }
        }

        ModuleDescriptor.Capabilities.Count = actual;
    }

    // 0x600051c0
    u32 AcquireRendererDeviceFormat(const D3DFORMAT format)
    {
        switch (format)
        {
        case D3DFMT_UNKNOWN:
        case D3DFMT_UNKNOWN_1:
        case D3DFMT_UNKNOWN_2:
        case D3DFMT_UNKNOWN_3:
        case D3DFMT_UNKNOWN_4:
        case D3DFMT_UNKNOWN_5:
        case D3DFMT_UNKNOWN_6:
        case D3DFMT_UNKNOWN_7:
        case D3DFMT_UNKNOWN_8:
        case D3DFMT_UNKNOWN_9:
        case D3DFMT_UNKNOWN_10:
        case D3DFMT_UNKNOWN_11:
        case D3DFMT_UNKNOWN_12:
        case D3DFMT_UNKNOWN_13:
        case D3DFMT_UNKNOWN_14:
        case D3DFMT_UNKNOWN_15:
        case D3DFMT_UNKNOWN_16:
        case D3DFMT_UNKNOWN_17:
        case D3DFMT_UNKNOWN_18:
        case D3DFMT_UNKNOWN_19: { return RENDERER_PIXEL_FORMAT_L8; }
        case D3DFMT_R8G8B8: { return RENDERER_PIXEL_FORMAT_R8G8B8; }
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8: { return RENDERER_PIXEL_FORMAT_A8R8G8B8; }
        case D3DFMT_R5G6B5: { return RENDERER_PIXEL_FORMAT_R5G6B5; }
        case D3DFMT_X1R5G5B5: { return RENDERER_PIXEL_FORMAT_A1R5G5B5; }
        case D3DFMT_A1R5G5B5: { return RENDERER_PIXEL_FORMAT_R5G5B5; }
        case D3DFMT_A4R4G4B4: { return RENDERER_PIXEL_FORMAT_R4G4B4; }
        case D3DFMT_A8: { return RENDERER_PIXEL_FORMAT_A8; }
        case D3DFMT_A8P8: { return RENDERER_PIXEL_FORMAT_A8P8; }
        case D3DFMT_P8: { return RENDERER_PIXEL_FORMAT_P8; }
        case D3DFMT_UNKNOWN_42:
        case D3DFMT_UNKNOWN_43:
        case D3DFMT_UNKNOWN_44:
        case D3DFMT_UNKNOWN_45:
        case D3DFMT_UNKNOWN_46:
        case D3DFMT_UNKNOWN_47:
        case D3DFMT_UNKNOWN_48:
        case D3DFMT_UNKNOWN_49:
        case D3DFMT_L8: { return RENDERER_PIXEL_FORMAT_L8; }
        case D3DFMT_A8L8: { return RENDERER_PIXEL_FORMAT_A8L8; }
        case D3DFMT_A4L4: { return RENDERER_PIXEL_FORMAT_A4L4; }
        case D3DFMT_V8U8: { return RENDERER_PIXEL_FORMAT_V8U8; }
        case D3DFMT_L6V5U5: { return RENDERER_PIXEL_FORMAT_BUMPDUDV_1; }
        case D3DFMT_D16_LOCKABLE: { return RENDERER_PIXEL_FORMAT_D16L; }
        case D3DFMT_D32: { return RENDERER_PIXEL_FORMAT_D32; }
        case D3DFMT_D15S1: { return RENDERER_PIXEL_FORMAT_D15S1; }
        case D3DFMT_UNKNOWN_74:
        case D3DFMT_D24S8: { return RENDERER_PIXEL_FORMAT_D24S8; }
        case D3DFMT_D24X8: { return RENDERER_PIXEL_FORMAT_D24X8; }
        case D3DFMT_D24X4S4: { return RENDERER_PIXEL_FORMAT_D24X4S4; }
        case D3DFMT_D16: { return RENDERER_PIXEL_FORMAT_D16; }
        case D3DFMT_DXT1: { return RENDERER_PIXEL_FORMAT_DXT1; }
        case D3DFMT_YUY2: { return RENDERER_PIXEL_FORMAT_YUV2; }
        case D3DFMT_DXT3: { return RENDERER_PIXEL_FORMAT_DXT3; }
        case D3DFMT_DXT5: { return RENDERER_PIXEL_FORMAT_DXT5; }
        }

        return RENDERER_PIXEL_FORMAT_NONE;
    }

    // 0x60005350
    u32 AcquireRendererDeviceFormatSize(const D3DFORMAT format, const RendererDeviceFormatSize size)
    {
        u32 bits = 0;
        u32 bytes = 0;

        switch (format)
        {
        case D3DFMT_UNKNOWN:
        case D3DFMT_UNKNOWN_1:
        case D3DFMT_UNKNOWN_2:
        case D3DFMT_UNKNOWN_3:
        case D3DFMT_UNKNOWN_4:
        case D3DFMT_UNKNOWN_5:
        case D3DFMT_UNKNOWN_6:
        case D3DFMT_UNKNOWN_7:
        case D3DFMT_UNKNOWN_8:
        case D3DFMT_UNKNOWN_9:
        case D3DFMT_UNKNOWN_10:
        case D3DFMT_UNKNOWN_11:
        case D3DFMT_UNKNOWN_12:
        case D3DFMT_UNKNOWN_13:
        case D3DFMT_UNKNOWN_14:
        case D3DFMT_UNKNOWN_15:
        case D3DFMT_UNKNOWN_16:
        case D3DFMT_UNKNOWN_17:
        case D3DFMT_UNKNOWN_18:
        case D3DFMT_UNKNOWN_19:
        case D3DFMT_A8:
        case D3DFMT_P8:
        case D3DFMT_UNKNOWN_42:
        case D3DFMT_UNKNOWN_43:
        case D3DFMT_UNKNOWN_44:
        case D3DFMT_UNKNOWN_45:
        case D3DFMT_UNKNOWN_46:
        case D3DFMT_UNKNOWN_47:
        case D3DFMT_UNKNOWN_48:
        case D3DFMT_UNKNOWN_49:
        case D3DFMT_L8:
        case D3DFMT_A4L4: { bits = GRAPHICS_BITS_PER_PIXEL_8; bytes = 1; break; }
        case D3DFMT_R8G8B8: { bits = GRAPHICS_BITS_PER_PIXEL_24; bytes = 3; break; }
        case D3DFMT_A8R8G8B8:
        case D3DFMT_X8R8G8B8:
        case D3DFMT_D32:
        case D3DFMT_UNKNOWN_74:
        case D3DFMT_D24S8:
        case D3DFMT_D24X8:
        case D3DFMT_D24X4S4: { bits = GRAPHICS_BITS_PER_PIXEL_32; bytes = 4; break; }
        case D3DFMT_R5G6B5:
        case D3DFMT_X1R5G5B5:
        case D3DFMT_A1R5G5B5:
        case D3DFMT_A4R4G4B4:
        case D3DFMT_A8P8:
        case D3DFMT_A8L8:
        case D3DFMT_V8U8:
        case D3DFMT_L6V5U5:
        case D3DFMT_D16_LOCKABLE:
        case D3DFMT_D15S1:
        case D3DFMT_D16: { bits = GRAPHICS_BITS_PER_PIXEL_16; bytes = 2; break; }
        case D3DFMT_DXT1: { bits = GRAPHICS_BITS_PER_PIXEL_8; bytes = 4; break; }
        case D3DFMT_DXT3:
        case D3DFMT_DXT5: { bits = GRAPHICS_BITS_PER_PIXEL_16; bytes = 8; break; }
        }

        return size == RendererDeviceFormatSizeBits ? bits : bytes;
    }

    // 0x60003270
    void InitializeTextureStateStates(void)
    {
        ZeroMemory(State.Textures.StageStates, MAX_TEXTURE_STATE_STATE_COUNT * sizeof(TextureStageState));
    }

    // 0x60003b80
    u32 STDCALLAPI InitializeRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        u32 bits = 0;
        u32 height = 0;
        u32 width = 0;

        const RendererModuleDescriptorDeviceCapabilities* caps = &ModuleDescriptor.Capabilities.Capabilities[wp];

        if (caps->IsActive)
        {
            width = caps->Width;
            height = caps->Height;
            bits = caps->Bits;
        }
        else
        {
            width = GRAPHICS_RESOLUTION_640;
            height = GRAPHICS_RESOLUTION_480;
            bits = GRAPHICS_BITS_PER_PIXEL_16;
        }

        D3DFORMAT format = D3DFMT_UNKNOWN;
        HWND window = NULL;
        BOOL windowed = FALSE;

        if (!State.Settings.IsWindowMode) { window = State.Window.HWND; }
        else
        {
            windowed = AcquireRendererDeviceDepthWindowFormat(&width, &height, &bits, &format);

            if (windowed) { window = hwnd; }
        }

        if (!windowed)
        {
            if (!AcquireRendererDeviceDepthFormat(&bits, &format))
            {
                if (result != NULL)
                {
                    SetEvent(State.Mutexes.Device);

                    *result = RENDERER_MODULE_FAILURE;
                }

                return RENDERER_MODULE_FAILURE;
            }
        }

        ZeroMemory(&State.Device.Presentation, sizeof(D3DPRESENT_PARAMETERS));

        State.Device.Presentation.EnableAutoDepthStencil = (State.DX.Surfaces.Bits != 0);
        State.Device.Presentation.hDeviceWindow = window;
        State.Device.Presentation.SwapEffect = D3DSWAPEFFECT_FLIP;
        State.Device.Presentation.MultiSampleType = D3DMULTISAMPLE_NONE;
        State.Device.Presentation.AutoDepthStencilFormat = format;
        State.Device.Presentation.FullScreen_RefreshRateInHz = 0;
        State.Device.Presentation.FullScreen_PresentationInterval = 0;
        State.Device.Presentation.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
        State.Device.Presentation.BackBufferWidth = width;
        State.Device.Presentation.BackBufferHeight = height;

        if (bits == GRAPHICS_BITS_PER_PIXEL_16) { State.Device.Presentation.BackBufferFormat = D3DFMT_R5G6B5; }
        else if (bits == GRAPHICS_BITS_PER_PIXEL_32) { State.Device.Presentation.BackBufferFormat = D3DFMT_X8R8G8B8; }

        State.Device.Presentation.BackBufferCount = lp - 1;
        State.Device.Presentation.Windowed = windowed;

        if (!IsRendererInit)
        {
            if (State.Scene.IsActive)
            {
                ToggleGameWindow();
                SyncGameWindow(0);
            }

            ResetTextures();

            ReleaseRendererWindows();

            ReleaseRendererObjects();

            AttemptRenderPackets();
        }

        HRESULT code = D3D_OK;

        while (TRUE)
        {
            code = D3D_OK;

            State.Device.Presentation.BackBufferWidth = width;
            State.Device.Presentation.BackBufferHeight = height;

            if (!IsRendererInit)
            {
                code = State.DX.Device->Reset(&State.Device.Presentation);
            }
            else
            {
                code = State.DX.Instance->CreateDevice(State.Device.Index, D3DDEVTYPE_HAL, window,
                    D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &State.Device.Presentation, &State.DX.Device);
            }

            if (code == D3D_OK || code == D3DERR_OUTOFVIDEOMEMORY) { break; }

            switch (width)
            {
            case GRAPHICS_RESOLUTION_2048: { width = GRAPHICS_RESOLUTION_1600; height = GRAPHICS_RESOLUTION_1200; break; }
            case GRAPHICS_RESOLUTION_1600: { width = GRAPHICS_RESOLUTION_1280; height = GRAPHICS_RESOLUTION_1024; break; }
            case GRAPHICS_RESOLUTION_1280: { width = GRAPHICS_RESOLUTION_1024; height = GRAPHICS_RESOLUTION_768; break; }
            case GRAPHICS_RESOLUTION_1024: { width = GRAPHICS_RESOLUTION_800; height = GRAPHICS_RESOLUTION_600; break; }
            case GRAPHICS_RESOLUTION_800: { width = GRAPHICS_RESOLUTION_640; height = GRAPHICS_RESOLUTION_480; break; }
            default: { break; }
            }
        }

        u32 exit = RENDERER_MODULE_FAILURE;

        if (code == D3D_OK)
        {
            for (u32 x = 0; x < MAX_DEVICE_CAPABILITIES_COUNT; x++)
            {
                if (ModuleDescriptor.Capabilities.Capabilities[x].Width != 0)
                {
                    ModuleDescriptor.Capabilities.Capabilities[x].Unk03 = State.Device.Presentation.BackBufferCount + 1;
                    ModuleDescriptor.Capabilities.Capabilities[x].Unk04 = State.Device.Presentation.BackBufferCount + 1;

                    if (State.Device.Presentation.EnableAutoDepthStencil)
                    {
                        ModuleDescriptor.Capabilities.Capabilities[x].Unk03 = ModuleDescriptor.Capabilities.Capabilities[x].Unk03 + 1;
                    }
                }
            }

            {
                D3DVIEWPORT8 vp;

                vp.X = 0;
                vp.Y = 0;
                vp.Width = width;
                vp.Height = height;
                vp.MinZ = 0.0f;
                vp.MaxZ = 1.0f;

                State.DX.Device->SetViewport(&vp);
            }

            BeginRendererScene();

            State.DX.Device->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &State.DX.Surfaces.Surfaces[2]);

            State.DX.Surfaces.Surfaces[2]->Release();

            State.Window.Surface = State.DX.Surfaces.Surfaces[2];

            State.Window.Index = 2;

            State.DX.Surfaces.Surfaces[3] = NULL;
            State.Window.Stencil = NULL;

            State.Device.Capabilities.IsDepthAvailable = FALSE;

            if (State.Device.Presentation.EnableAutoDepthStencil)
            {
                State.Device.Capabilities.IsDepthAvailable = TRUE;

                State.DX.Device->GetDepthStencilSurface(&State.DX.Surfaces.Surfaces[3]);
                State.DX.Surfaces.Surfaces[3]->Release();
            }

            State.DX.Surfaces.Width = width;
            State.DX.Surfaces.Height = height;

            State.Window.Stencil = State.DX.Surfaces.Surfaces[3];

            if (State.DX.Formats.Back != State.Device.Presentation.BackBufferFormat)
            {
                AcquireRendererTextureFormats(State.Device.Presentation.BackBufferFormat);
            }

            State.DX.Formats.Back = State.Device.Presentation.BackBufferFormat;

            if (IsRendererInit)
            {
                AcquireRendererDeviceCapabilities();

                State.DX.Device->GetGammaRamp(&State.DX.State.Gamma);

                InitializeViewPort();

                if ((State.Device.Presentation.EnableAutoDepthStencil != 0) && (State.DX.Surfaces.Bits != 0))
                {
                    State.Device.Capabilities.IsDepthAvailable = TRUE;

                    if (format == D3DFMT_D15S1 || format == D3DFMT_D24S8 || format == D3DFMT_D24X4S4)
                    {
                        SelectState(RENDERER_MODULE_STATE_SELECT_STENCIL_FUNCTION, (void*)RENDERER_MODULE_STENCIL_FUNCTION_ALWAYS);
                        SelectState(RENDERER_MODULE_STATE_SELECT_STENCIL_FAIL_STATE, (void*)RENDERER_MODULE_STENCIL_FAIL_ZERO);
                        SelectState(RENDERER_MODULE_STATE_SELECT_STENCIL_DEPTH_FAIL_STATE, (void*)RENDERER_MODULE_STENCIL_DEPTH_FAIL_ZERO);
                        SelectState(RENDERER_MODULE_STATE_SELECT_STENCIL_PASS_STATE, (void*)RENDERER_MODULE_STENCIL_PASS_ZERO);
                        SelectState(RENDERER_MODULE_STATE_SELECT_STENCIL_STATE, (void*)RENDERER_MODULE_STENCIL_INACTIVE);
                    }
                }

                IsRendererInit = FALSE;
            }

            AcquireRendererDeviceMemorySize();

            if (State.Data.Vertexes.Buffer == NULL) { InitializeVertexBuffer(); }

            exit = RENDERER_MODULE_SUCCESS;
        }

        if (result != NULL)
        {
            SetEvent(State.Mutexes.Device);

            *result = exit;
        }

        return exit;
    }

    // 0x60004140
    BOOL AcquireRendererDeviceDepthFormat(u32* bits, D3DFORMAT* result)
    {
        const D3DFORMAT x16[MAX_RENDERER_DEVICE_FORMAT_COUNT] = { D3DFMT_R5G6B5, D3DFMT_X8R8G8B8 };
        const D3DFORMAT x32[MAX_RENDERER_DEVICE_FORMAT_COUNT] = { D3DFMT_X8R8G8B8, D3DFMT_R5G6B5 };

        if (State.DX.Instance != NULL)
        {
            const D3DFORMAT* formats = (*bits == GRAPHICS_BITS_PER_PIXEL_32) ? x32 : x16;

            for (u32 x = 0; x < MAX_RENDERER_DEVICE_FORMAT_COUNT; x++)
            {
                if (State.DX.Instance->CheckDeviceType(State.Device.Index, D3DDEVTYPE_HAL, formats[x], formats[x], FALSE) == D3D_OK)
                {
                    if (State.DX.Instance->CheckDeviceFormat(State.Device.Index, D3DDEVTYPE_HAL, formats[x],
                        D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, formats[x]) == D3D_OK)
                    {
                        if (AcquireRendererDeviceDepthFormat(State.Device.Index, formats[x], formats[x], result))
                        {
                            *bits = formats[x] == D3DFMT_R5G6B5 ? GRAPHICS_BITS_PER_PIXEL_16 : GRAPHICS_BITS_PER_PIXEL_32;

                            return TRUE;
                        }
                    }
                }
            }
        }

        return FALSE;
    }

    // 0x600043e0
    BOOL AcquireRendererDeviceDepthFormat(const u32 device, const D3DFORMAT adapter, const D3DFORMAT target, D3DFORMAT* result)
    {
        const D3DFORMAT x16[MAX_RENDERER_DEVICE_DEPTH_FORMAT_COUNT] = { D3DFMT_D16, D3DFMT_D32, D3DFMT_D24S8, D3DFMT_D24X8, D3DFMT_D24X4S4 };
        const D3DFORMAT x32[MAX_RENDERER_DEVICE_DEPTH_FORMAT_COUNT] = { D3DFMT_D32, D3DFMT_D24S8, D3DFMT_D24X8, D3DFMT_D24X4S4, D3DFMT_D16 };

        const D3DFORMAT* formats = NULL;

        switch (State.DX.Surfaces.Bits)
        {
        case GRAPHICS_BITS_PER_PIXEL_16: { formats = x16; break; }
        case GRAPHICS_BITS_PER_PIXEL_32: { formats = x32; break; }
        default: { return TRUE; }
        }

        for (u32 x = 0; x < MAX_RENDERER_DEVICE_DEPTH_FORMAT_COUNT; x++)
        {
            if (State.DX.Instance->CheckDeviceFormat(device, D3DDEVTYPE_HAL, adapter, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, formats[x]) == D3D_OK)
            {
                if (State.DX.Instance->CheckDepthStencilMatch(device, D3DDEVTYPE_HAL, adapter, target, formats[x]) == D3D_OK)
                {
                    *result = formats[x];

                    switch (formats[x])
                    {
                    case D3DFMT_D32:
                    case D3DFMT_D24S8:
                    case D3DFMT_D24X8:
                    case D3DFMT_D24X4S4: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_32; break; }
                    case D3DFMT_D15S1: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_16; State.Device.Capabilities.IsStencilBufferAvailable = TRUE; break; }
                    case D3DFMT_D16: { State.DX.Surfaces.Bits = GRAPHICS_BITS_PER_PIXEL_16; break; }
                    }

                    return TRUE;
                }
            }
        }

        *result = D3DFMT_UNKNOWN;

        return FALSE;
    }

    // 0x60004010
    BOOL AcquireRendererDeviceDepthWindowFormat(u32* width, u32* height, u32* bits, D3DFORMAT* format)
    {
        u32 awidth = *width;
        u32 aheight = *height;

        HDC hdc = GetDC(NULL);
        const u32 w = GetDeviceCaps(hdc, HORZRES);
        const u32 h = GetDeviceCaps(hdc, VERTRES);
        const u32 b = GetDeviceCaps(hdc, BITSPIXEL);

        if (b != GRAPHICS_BITS_PER_PIXEL_16 && b != GRAPHICS_BITS_PER_PIXEL_32) { return FALSE; }

        if (w < GRAPHICS_RESOLUTION_640) { return FALSE; }

        if (w < awidth) { awidth = w; }
        if (h < aheight) { aheight = h; }

        D3DFORMAT adapter = D3DFMT_UNKNOWN;
        D3DFORMAT target = D3DFMT_UNKNOWN;

        if (b == GRAPHICS_BITS_PER_PIXEL_16) { adapter = D3DFMT_R5G6B5; target = D3DFMT_R5G6B5; }
        else if (b == GRAPHICS_BITS_PER_PIXEL_32) { adapter = D3DFMT_X8R8G8B8; target = D3DFMT_X8R8G8B8; }

        if (State.DX.Instance->CheckDeviceFormat(State.Device.Index, D3DDEVTYPE_HAL,
            adapter, D3DUSAGE_RENDERTARGET, D3DRTYPE_SURFACE, target) == D3D_OK)
        {
            if (State.DX.Instance->CheckDeviceType(State.Device.Index, D3DDEVTYPE_HAL, adapter, target, TRUE) != D3D_OK) { return FALSE; }
        }

        if (!AcquireRendererDeviceDepthFormat(State.Device.Index, adapter, target, format)) { return FALSE; }

        *width = awidth;
        *height = aheight;
        *bits = b;

        return TRUE;
    }

    // 0x600018d0
    void ReleaseRendererWindows(void)
    {
        for (u32 x = MIN_WINDOW_INDEX; x < State.Window.Count + MIN_WINDOW_INDEX; x++)
        {
            DestroyGameWindow(x);
        }

        State.Window.Count = 0;
    }

    // 0x60004250
    void ReleaseRendererObjects(void)
    {
        if (State.DX.Surfaces.Surfaces[3] != NULL)
        {
            State.DX.Surfaces.Surfaces[3]->Release();
            State.DX.Surfaces.Surfaces[3] = NULL;
        }

        if (State.DX.Surfaces.Surfaces[2] != NULL)
        {
            State.DX.Surfaces.Surfaces[2]->Release();
            State.DX.Surfaces.Surfaces[2] = NULL;
        }

        for (s32 x = (MAX_ACTIVE_SURFACE_COUNT - 1); x >= 0; x--)
        {
            if (State.DX.Surfaces.Surfaces[x] != NULL)
            {
                State.DX.Surfaces.Surfaces[x]->Release();
                State.DX.Surfaces.Surfaces[x] = NULL;
            }
        }

        if (State.Data.Vertexes.Buffer != NULL)
        {
            while (State.Data.Vertexes.Buffer->Release() != D3D_OK) {}

            State.Data.Vertexes.Buffer = NULL;
        }
    }

    // 0x60002250
    void AttemptRenderPackets(void)
    {
        if (!State.Scene.IsActive) { return; }

        if (State.DX.Device != NULL)
        {
            RenderPackets();

            State.Scene.IsActive = State.DX.Device->EndScene() != D3D_OK;
        }
    }

    // 0x60001f20
    void RenderPackets(void)
    {
        BeginRendererScene();

        State.DX.Device->SetVertexShader(RendererCurrentShader);
        State.DX.Device->SetStreamSource(0, State.Data.Vertexes.Buffer, RendererVertexSize);

        for (u32 x = 0; x < State.Data.Packets.Count; x++)
        {
            const RendererPacket* packet = &State.Data.Packets.Packets[x];

            State.DX.Device->DrawPrimitive(packet->PrimitiveType, State.Data.Vertexes.StartIndex, packet->PrimitiveCount);

            State.Data.Vertexes.StartIndex = State.Data.Vertexes.StartIndex + packet->StartIndex;
        }

        State.Data.Packets.Count = 0;
    }

    // 0x60002200
    BOOL BeginRendererScene(void)
    {
        if (State.Scene.IsActive) { return TRUE; }

        if (State.DX.Device != NULL)
        {
            if (State.Lock.IsActive) { UnlockGameWindow(NULL); }

            State.Scene.IsActive = State.DX.Device->BeginScene() == D3D_OK;
        }

        return State.Scene.IsActive;
    }

    // 0x60004df0
    void AcquireRendererTextureFormats(const D3DFORMAT format)
    {
        for (u32 x = 0; x < MAX_TEXTURE_FORMAT_COUNT; x++)
        {
            State.Textures.Formats[x] = AcquireRendererTextureFormat(x);
        };

        for (u32 x = 0; x < MAX_TEXTURE_FORMAT_COUNT; x++)
        {
            if (State.Textures.Formats[x] != D3DFMT_UNKNOWN)
            {
                if (State.DX.Instance->CheckDeviceFormat(0, D3DDEVTYPE_HAL,
                    format, D3DUSAGE_NONE, D3DRTYPE_TEXTURE, State.Textures.Formats[x]) != D3D_OK)
                {
                    State.Textures.Formats[x] = D3DFMT_UNKNOWN;
                }
            }
        }

        for (u32 x = 0; x < MAX_USABLE_TEXTURE_FORMAT_COUNT; x++)
        {
            RendererTextureFormatStates[x] = State.Textures.Formats[x] != D3DFMT_UNKNOWN;
        }
    }

    // 0x60005080
    D3DFORMAT AcquireRendererTextureFormat(const u32 indx)
    {
        switch (indx)
        {
        case RENDERER_PIXEL_FORMAT_P8: { return D3DFMT_P8; }
        case RENDERER_PIXEL_FORMAT_R5G5B5: { return D3DFMT_A1R5G5B5; }
        case RENDERER_PIXEL_FORMAT_R5G6B5: { return D3DFMT_R5G6B5; }
        case RENDERER_PIXEL_FORMAT_R8G8B8: { return D3DFMT_R8G8B8; }
        case RENDERER_PIXEL_FORMAT_A8R8G8B8: { return D3DFMT_A8R8G8B8; }
        case RENDERER_PIXEL_FORMAT_R4G4B4: { return D3DFMT_A4R4G4B4; }
        case RENDERER_PIXEL_FORMAT_A4L4: { return D3DFMT_A4L4; }
        case RENDERER_PIXEL_FORMAT_A8P8: { return D3DFMT_A8P8; }
        case RENDERER_PIXEL_FORMAT_YUV2: { return D3DFMT_YUY2; }
        case RENDERER_PIXEL_FORMAT_A1R5G5B5: { return D3DFMT_X1R5G5B5; }
        case RENDERER_PIXEL_FORMAT_DXT1: { return D3DFMT_DXT1; }
        case RENDERER_PIXEL_FORMAT_DXT3: { return D3DFMT_DXT3; }
        case RENDERER_PIXEL_FORMAT_DXT5: { return D3DFMT_DXT5; }
        case RENDERER_PIXEL_FORMAT_V8U8: { return D3DFMT_V8U8; }
        case RENDERER_PIXEL_FORMAT_BUMPDUDV_1: { return D3DFMT_L6V5U5; }
        case RENDERER_PIXEL_FORMAT_A8: { return D3DFMT_A8; }
        case RENDERER_PIXEL_FORMAT_L8: { return D3DFMT_L8; }
        case RENDERER_PIXEL_FORMAT_A8L8: { return D3DFMT_A8L8; }
        case RENDERER_PIXEL_FORMAT_D16: { return D3DFMT_D16; }
        case RENDERER_PIXEL_FORMAT_D24S8: { return D3DFMT_D24S8; }
        case RENDERER_PIXEL_FORMAT_D16L: { return D3DFMT_D16_LOCKABLE; }
        case RENDERER_PIXEL_FORMAT_D32: { return D3DFMT_D32; }
        case RENDERER_PIXEL_FORMAT_D15S1: { return D3DFMT_D15S1; }
        case RENDERER_PIXEL_FORMAT_D24X8: { return D3DFMT_D24X8; }
        case RENDERER_PIXEL_FORMAT_D24X4S4: { return D3DFMT_D24X4S4; }
        }

        return D3DFMT_UNKNOWN;
    }

    // 0x60008140
    BOOL AcquireRendererDeviceCapabilities(void)
    {
        D3DCAPS8 caps;
        ZeroMemory(&caps, sizeof(D3DCAPS8));

        ZeroMemory(&State.Device.Capabilities, sizeof(RendererModuleDeviceCapabilities8));

        if (State.DX.Device->GetDeviceCaps(&caps) != D3D_OK) { return FALSE; }

        if (caps.DevCaps & D3DDEVCAPS_HWRASTERIZATION) { State.Device.Capabilities.IsAccelerated = TRUE; }

        State.Data.MaxPrimitiveCount = caps.MaxPrimitiveCount;

        MaxRendererSimultaneousTextures = MIN_SIMULTANEOUS_TEXTURE_COUNT;

        if ((caps.DevCaps & D3DDEVCAPS_SEPARATETEXTUREMEMORIES) && MIN_SIMULTANEOUS_TEXTURE_COUNT < caps.MaxSimultaneousTextures)
        {
            MaxRendererSimultaneousTextures = caps.MaxSimultaneousTextures;
        }

        State.Device.Capabilities.MaximumSimultaneousTextures = caps.MaxSimultaneousTextures;
        State.Device.Capabilities.MaxTextureRepeat = (f32)caps.MaxTextureRepeat;
        State.Device.Capabilities.MinTextureWidth = 1;
        State.Device.Capabilities.MinTextureHeight = 1;
        State.Device.Capabilities.MaxTextureWidth = caps.MaxTextureWidth;
        State.Device.Capabilities.MaxTextureHeight = caps.MaxTextureHeight;

        if (caps.TextureCaps & D3DPTEXTURECAPS_SQUAREONLY) { State.Device.Capabilities.IsSquareOnlyTextures = TRUE; }

        State.Device.Capabilities.MultipleTextureWidth = 1;
        State.Device.Capabilities.MultipleTextureHeight = 1;

        if (caps.TextureCaps & D3DPTEXTURECAPS_POW2)
        {
            State.Device.Capabilities.IsPowerOfTwoTexturesWidth = TRUE;
            State.Device.Capabilities.IsPowerOfTwoTexturesHeight = TRUE;
        }

        if (caps.TextureAddressCaps & D3DPTADDRESSCAPS_INDEPENDENTUV) { State.Device.Capabilities.IsTextureIndependentUVs = TRUE; }

        State.Device.Capabilities.IsGreenAllowSixBits = TRUE;

        if (caps.RasterCaps & D3DPRASTERCAPS_ANTIALIASEDGES) { State.Device.Capabilities.IsAntiAliasEdges = TRUE; }

        if (caps.RasterCaps & D3DPRASTERCAPS_ANISOTROPY)
        {
            State.Device.Capabilities.IsAnisotropyAvailable = TRUE;
            State.Device.Capabilities.MaxAnisotropy = caps.MaxAnisotropy;
        }

        if (caps.RasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS) { State.Device.Capabilities.IsMipMapBiasAvailable = TRUE; }

        if (caps.RasterCaps & D3DPRASTERCAPS_WBUFFER) { State.Device.Capabilities.IsWBufferAvailable = TRUE; }

        if (caps.RasterCaps & D3DPRASTERCAPS_WFOG) { State.Device.Capabilities.IsWFogAvailable = TRUE; }
        if (caps.RasterCaps & D3DPRASTERCAPS_DITHER) { State.Device.Capabilities.IsDitherAvailable = TRUE; }

        State.Device.Capabilities.IsPerspectiveTextures = (caps.TextureCaps & D3DPTEXTURECAPS_PERSPECTIVE) != 0;
        State.Device.Capabilities.IsAlphaTextures = (caps.TextureCaps & D3DPTEXTURECAPS_ALPHA) != 0;
        State.Device.Capabilities.IsAlphaBlending = (caps.ShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND) != 0;
        State.Device.Capabilities.IsSpecularBlending = (caps.ShadeCaps & D3DPSHADECAPS_SPECULARGOURAUDRGB) != 0;

        if (State.Device.Capabilities.IsSpecularBlending) { State.Device.Capabilities.IsSpecularGouraudBlending = 1; }

        State.Device.Capabilities.IsModulateBlending = TRUE;

        if ((caps.SrcBlendCaps & D3DPBLENDCAPS_SRCALPHA) == 0)
        {
            State.Device.Capabilities.IsSourceAlphaBlending = TRUE;

            if ((caps.DestBlendCaps & D3DPBLENDCAPS_ONE) == 0) { State.Device.Capabilities.IsSourceAlphaBlending = FALSE; }
        }

        State.Device.Capabilities.IsColorBlending = (caps.ShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB) != 0;

        if (!State.Device.Capabilities.IsColorBlending) { State.Device.Capabilities.IsSourceAlphaBlending = FALSE; }

        if (isnan(caps.GuardBandLeft) == (caps.GuardBandLeft == 0.0f))
        {
            State.Device.Capabilities.GuardBandLeft = caps.GuardBandLeft;
            State.Device.Capabilities.GuardBandRight = caps.GuardBandRight;
            State.Device.Capabilities.GuardBandTop = caps.GuardBandTop;
            State.Device.Capabilities.GuardBandBottom = caps.GuardBandBottom;
        }

        State.Device.Capabilities.Unk29 = 0;
        State.Device.Capabilities.Unk32 = 1;
        State.Device.Capabilities.IsAlphaProperBlending = State.Device.Capabilities.IsAlphaBlending;

        State.Device.SubType = 0; // TODO

        if (caps.Caps2 & D3DCAPS2_CANRENDERWINDOWED) { State.Device.Capabilities.IsWindowModeAvailable = TRUE; }
        if (caps.Caps2 & D3DCAPS2_FULLSCREENGAMMA) { State.Device.Capabilities.IsGammaAvailable = TRUE; }

        if (caps.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR) { State.Device.Capabilities.IsInterpolationAvailable = TRUE; }

        if (caps.MaxSimultaneousTextures != 0)
        {
            const BOOL op1 = (caps.TextureOpCaps & (D3DTEXOPCAPS_MODULATE2X | D3DTEXOPCAPS_SELECTARG2 | D3DTEXOPCAPS_SELECTARG1)) != 0;
            const BOOL op2 = (caps.TextureOpCaps & (D3DTEXOPCAPS_MODULATE2X | D3DTEXOPCAPS_SELECTARG2 | D3DTEXOPCAPS_SELECTARG1 | D3DTEXOPCAPS_DISABLE)) != 0;
            const BOOL op3 = (caps.TextureOpCaps & (D3DTEXOPCAPS_MODULATE2X | D3DTEXOPCAPS_MODULATE)) != 0;

            for (u32 x = 0; x < State.Device.Capabilities.MaximumSimultaneousTextures; x++)
            {
                State.Textures.Stages[x].Unk11 = op1; // TODO
                State.Textures.Stages[x].Unk12 = op2; // TODO
                State.Textures.Stages[x].Unk10 = op3; // TODO
            }
        }

        return TRUE;
    }

    // 0x600014a0
    void InitializeViewPort(void)
    {
        State.ViewPort.X0 = 0;
        State.ViewPort.Y0 = 0;
        State.ViewPort.X1 = 0;
        State.ViewPort.Y1 = 0;
    }

    // 0x60004210
    void AcquireRendererDeviceMemorySize(void)
    {
        ModuleDescriptor.MemorySize = State.DX.Device->GetAvailableTextureMem();

        ModuleDescriptor.MemoryType = 0;

        if (MIN_SIMULTANEOUS_TEXTURE_COUNT < MaxRendererSimultaneousTextures)
        {
            ModuleDescriptor.MemorySize = ModuleDescriptor.MemorySize / MaxRendererSimultaneousTextures;
        }
    }

    // 0x60001dd0
    void InitializeVertexBuffer(void)
    {
        if (State.DX.Device->CreateVertexBuffer(MAX_VERTEX_BUFFER_SIZE, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
            D3DFVF_NONE, D3DPOOL_DEFAULT, &State.Data.Vertexes.Buffer) == D3D_OK)
        {
            ZeroMemory(State.Data.Packets.Packets, MAX_RENDER_PACKET_COUNT * sizeof(RendererPacket));

            State.Data.Packets.Count = 0;

            State.Data.Vertexes.Count = 0;
            State.Data.Vertexes.StartIndex = 0;
        }
    }

    // 0x60003ab0
    u32 STDCALLAPI InitializeRendererDeviceSurfacesExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        if (State.Lock.IsActive) { UnlockGameWindow(NULL); }

        AttemptRenderPackets();

        ReleaseRendererWindows();

        ResetTextures();

        if (State.DX.Device != NULL)
        {
            ReleaseRendererObjects();

            while (State.DX.Device->Release() != D3D_OK) {}

            State.DX.Device = NULL;
        }

        if (State.DX.Instance != NULL)
        {
            while (State.DX.Instance->Release() != D3D_OK) {}

            State.DX.Instance = NULL;
        }

        State.Scene.IsActive = FALSE;

        State.DX.Surfaces.Surfaces[2] = NULL;
        State.DX.Surfaces.Surfaces[3] = NULL;

        State.Device.Index = DEFAULT_DEVICE_INDEX;

        State.Lambdas.Log = NULL;

        IsRendererInit = FALSE;

        State.Window.HWND = NULL;

        if (result != NULL)
        {
            SetEvent(State.Mutexes.Surface);

            *result = RENDERER_MODULE_SUCCESS;
        }

        return RENDERER_MODULE_SUCCESS;
    }
}