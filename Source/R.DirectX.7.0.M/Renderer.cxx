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
#include "Renderer.hxx"
#include "RendererValues.hxx"
#include "Settings.hxx"

#include <malloc.h>
#include <math.h>
#include <stdio.h>

#define MAX_MESSAGE_BUFFER_LENGTH 512

#define MAX_RENDERER_MESSAGE_BUFFER_LENGTH 80
#define MAX_RENDERER_MESSAGE_DESCRIPTION_BUFFER_LENGTH 64

#define MAX_SETTINGS_BUFFER_LENGTH 80

#define MAX_DEVICE_FORMAT_SCORE 10000

using namespace Renderer;
using namespace RendererModuleValues;
using namespace Settings;

namespace RendererModule
{
    RendererModuleState State;

    // 0x60009250
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

    // 0x600036b0
    void ReleaseRendererModule(void)
    {
        RestoreGameWindow();
    }

    // 0x60001f40
    u32 AcquireRendererDeviceCount(void)
    {
        State.Devices.Count = 0;
        State.Device.Identifier = NULL;

        GUID* uids[MAX_ENUMERATE_DEVICE_COUNT];

        State.Devices.Count = AcquireDirectDrawDeviceCount(uids, NULL, ENVIRONMENT_SECTION_NAME);

        u32 indx = 0;

        for (u32 x = 0; x < State.Devices.Count; x++)
        {
            if (AcquireRendererDeviceAccelerationState(x))
            {
                State.Devices.Indexes[indx] = uids[x];

                IDirectDraw* instance = NULL;
                DirectDrawCreate(uids[x], &instance, NULL);

                IDirectDraw7* dd = NULL;
                if (instance->QueryInterface(IID_IDirectDraw7, (void**)&dd) != DD_OK) { instance->Release(); continue; }

                DDDEVICEIDENTIFIER2 identifier;
                ZeroMemory(&identifier, sizeof(DDDEVICEIDENTIFIER2));

                dd->GetDeviceIdentifier(&identifier, DDGDI_GETHOSTIDENTIFIER);

                strncpy(State.Devices.Enumeration.Names[indx], identifier.szDescription, MAX_ENUMERATE_DEVICE_NAME_LENGTH);

                if (dd != NULL) { dd->Release(); dd = NULL; }
                if (instance != NULL) { instance->Release(); instance = NULL; }

                indx = indx + 1;
            }
        }

        State.Device.Identifier = State.Devices.Indexes[0];
        State.Devices.Count = indx;

        return State.Devices.Count;
    }

    // 0x6000bb10
    u32 AcquireDirectDrawDeviceCount(GUID** uids, HMONITOR** monitors, const char* section)
    {
        State.Devices.Enumeration.Count = 0;
        State.Devices.Enumeration.IsAvailable = FALSE;

        {
            const char* value = getenv(RENDERER_MODULE_DISPLAY_ENVIRONMENT_PROPERTY_NAME);

            if (value == NULL || atoi(value) != 0)
            {
                if (!AcquireState(RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_STATE))
                {
                    DirectDrawEnumerateExA(EnumerateDirectDrawDevices, NULL, DDENUM_ATTACHEDSECONDARYDEVICES);
                    DirectDrawEnumerateExA(EnumerateDirectDrawDevices, NULL, DDENUM_NONDISPLAYDEVICES);
                }
                else
                {
                    State.Devices.Enumeration.Count = 1;
                    State.Devices.Enumeration.IsAvailable = TRUE;
                }
            }
            else
            {
                State.Devices.Enumeration.Count = 1;
                State.Devices.Enumeration.IsAvailable = TRUE;
            }
        }

        if (uids != NULL)
        {
            for (u32 x = 0; x < State.Devices.Enumeration.Count; x++)
            {
                uids[x] = State.Devices.Enumeration.Identifiers.Indexes[x];
            }
        }

        if (monitors != NULL)
        {
            for (u32 x = 0; x < State.Devices.Enumeration.Count; x++)
            {
                monitors[x] = State.Devices.Enumeration.Monitors.Indexes[x];
            }
        }

        State.Devices.Enumeration.Count = AcquireSettingsValue(State.Devices.Enumeration.Count, section, "displays");

        return State.Devices.Enumeration.Count;
    }

    // 0x6000bbd0
    BOOL CALLBACK EnumerateDirectDrawDevices(GUID* uid, LPSTR name, LPSTR description, LPVOID context, HMONITOR monitor)
    {
        IDirectDraw* instance = NULL;
        if (DirectDrawCreate(uid, &instance, NULL) != DD_OK) { return FALSE; }

        IDirectDraw4* dd = NULL;
        if (instance->QueryInterface(IID_IDirectDraw4, (void**)&dd) != DD_OK) { return FALSE; }

        instance->Release();

        DDDEVICEIDENTIFIER idn;
        ZeroMemory(&idn, sizeof(DDDEVICEIDENTIFIER));

        dd->GetDeviceIdentifier(&idn, DDGDI_NONE);

        DDDEVICEIDENTIFIER idh;
        ZeroMemory(&idh, sizeof(DDDEVICEIDENTIFIER));

        dd->GetDeviceIdentifier(&idh, DDGDI_GETHOSTIDENTIFIER);

        BOOL skip = FALSE;

        if (State.Devices.Enumeration.Count == 0)
        {
            CopyMemory(&State.Devices.Enumeration.Identifier, &idh, sizeof(DDDEVICEIDENTIFIER));
        }
        else
        {
            const BOOL same = strcmp(State.Devices.Enumeration.Identifier.szDescription, idh.szDescription) == 0;

            if (!same && (!State.Devices.Enumeration.IsAvailable || uid == NULL))
            {
                skip = TRUE;

                if (uid != NULL) { State.Devices.Enumeration.IsAvailable = TRUE; }
            }
        }

        if (dd != NULL) { dd->Release(); }

        if (!skip && State.Devices.Enumeration.Count < MAX_ENUMERATE_DEVICE_COUNT)
        {
            if (uid != NULL)
            {
                State.Devices.Enumeration.Identifiers.Identifiers[State.Devices.Enumeration.Count] = *uid;
                State.Devices.Enumeration.Monitors.Monitors[State.Devices.Enumeration.Count] = monitor;
            }

            State.Devices.Enumeration.Identifiers.Indexes[State.Devices.Enumeration.Count] = &State.Devices.Enumeration.Identifiers.Identifiers[State.Devices.Enumeration.Count];
            State.Devices.Enumeration.Monitors.Indexes[State.Devices.Enumeration.Count] = &State.Devices.Enumeration.Monitors.Monitors[State.Devices.Enumeration.Count];

            State.Devices.Enumeration.Count = State.Devices.Enumeration.Count + 1;
        }

        return TRUE;
    }

    // 0x6000bd60
    BOOL AcquireRendererDeviceAccelerationState(const u32 indx)
    {
        IDirectDraw* instance = NULL;
        if (DirectDrawCreate(State.Devices.Enumeration.Identifiers.Indexes[indx], &instance, NULL) != DD_OK) { return FALSE; }

        IDirectDraw4* dd = NULL;
        if (instance->QueryInterface(IID_IDirectDraw4, (void**)&dd) != DD_OK) { return FALSE; }

        instance->Release();

        DDCAPS hal;
        ZeroMemory(&hal, sizeof(DDCAPS));

        hal.dwSize = sizeof(DDCAPS);

        dd->GetCaps(&hal, NULL);

        dd->Release();

        return hal.dwCaps & DDCAPS_3D;
    }

    // 0x60001f20
    void InitializeTextureStateStates(void)
    {
        ZeroMemory(State.Textures.StageStates, MAX_TEXTURE_STATE_STATE_COUNT * sizeof(TextureStageState));
    }
    
    // 0x60002060
    void SelectRendererDevice(void)
    {
        if (RendererDeviceIndex < DEFAULT_DEVICE_INDEX)
        {
            State.Device.Capabilities.MinTextureWidth = 1;

            if (State.Lambdas.Lambdas.AcquireWindow != NULL || State.Window.HWND != NULL)
            {
                const char* value = getenv(RENDERER_MODULE_DISPLAY_ENVIRONMENT_PROPERTY_NAME);

                SelectDevice(value == NULL ? DEFAULT_DEVICE_INDEX : atoi(value));
            }
        }
    }

    // 0x600092c0
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

    // 0x60008dc0
    u32 ClearRendererViewPort(const u32 x0, const u32 y0, const u32 x1, const u32 y1, const BOOL window)
    {
        AttemptRenderScene();

        D3DRECT rect;

        DWORD options = (window == FALSE); // D3DCLEAR_TARGET

        if (State.Device.Capabilities.IsDepthAvailable && State.Window.Bits != 0) { options = options | D3DCLEAR_ZBUFFER; }

        if (State.Device.Capabilities.IsStencilBufferAvailable) { options = options | D3DCLEAR_STENCIL; }

        if (x1 == 0)
        {
            rect.x1 = 0;
            rect.y1 = 0;
            rect.x2 = State.Window.Width;
            rect.y2 = State.Window.Height;
        }
        else
        {
            rect.x1 = x0;
            rect.x2 = x1;
            rect.y1 = y0;
            rect.y2 = y1;
        }

        return State.DX.Device->Clear(1, &rect, options, RendererClearColor, RendererClearDepth, 0) == DD_OK;
    }

    // 0x60006f10
    void AttemptRenderScene(void)
    {
        if (State.Data.Vertexes.Count != 0) { RendererRenderScene(); }
    }

    // 0x60006f20
    void RendererRenderScene(void)
    {
        if (State.Data.Vertexes.Count != 0 && State.Data.Indexes.Count != 0)
        {
            if (!State.Scene.IsActive) { BeginRendererScene(); }

            State.DX.Device->DrawIndexedPrimitive(RendererPrimitiveType, RendererVertexType,
                State.Data.Vertexes.Vertexes, State.Data.Vertexes.Count,
                State.Data.Indexes.Indexes, State.Data.Indexes.Count, 0);

            State.Data.Vertexes.Count = 0;
            State.Data.Indexes.Count = 0;
        }
    }

    // 0x60008810
    BOOL BeginRendererScene(void)
    {
        if (State.Lock.IsActive) { UnlockGameWindow(NULL); }

        State.Scene.IsActive = State.DX.Device->BeginScene() == DD_OK;

        return State.Scene.IsActive;
    }

    // 0x60008860
    BOOL EndRendererScene(void)
    {
        if (State.Data.Vertexes.Count != 0) { RendererRenderScene(); }

        if (State.Scene.IsActive)
        {
            RendererDepthBias = 0.0f;

            const HRESULT result = State.DX.Device->EndScene();

            State.Scene.IsActive = FALSE;

            return result == DD_OK;
        }

        return TRUE;
    }
}