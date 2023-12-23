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

#define MAX_SETTINGS_BUFFER_LENGTH 80
#define MAX_RENDERER_MESSAGE_BUFFER_LENGTH 404

using namespace Renderer;
using namespace RendererModuleValues;
using namespace Settings;

namespace RendererModule
{
    RendererModuleState State;

    u32 DAT_6001d030; // TODO
    u32 DAT_6001d5b0; // TODO

    // 0x60005b10
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

    // 0x60005b80
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

    // 0x60009810
    // 0x60009840
    // 0x60009870
    // NOTE: Originally there are 3 different methods for error, warning, and info (which is never being called).
    void Message(const u32 severity, const char* format, ...)
    {
        char buffer[MAX_MESSAGE_BUFFER_LENGTH];

        va_list args;
        va_start(args, format);
        vsnprintf_s(buffer, MAX_MESSAGE_BUFFER_LENGTH, format, args);
        va_end(args);

        if (severity == RENDERER_MODULE_MESSAGE_SEVERITY_ERROR && State.Lambdas.Log != NULL)
        {
            State.Lambdas.Log(severity, buffer);
        }
    }

    // 0x60001520
    void InitializeVertex(RTLVX* dst, const RTLVX* src)
    {
        dst->XYZ.X = src->XYZ.X;
        dst->XYZ.Y = src->XYZ.Y;
        dst->XYZ.Z = src->XYZ.Z;

        dst->RHW = src->RHW;

        dst->Color = src->Color;
        dst->Specular = src->Specular;

        dst->UV.X = src->UV.X;
        dst->UV.Y = src->UV.Y;
    }

    // 0x60007550
    const char* AcquireRendererMessageDescription(const HRESULT code)
    {
        switch (code)
        {
        case DD_OK: { return "DD_OK"; }
        case DDERR_ALREADYINITIALIZED: { return "DDERR_ALREADYINITIALIZED"; }
        case DDERR_BLTFASTCANTCLIP: { return "DDERR_BLTFASTCANTCLIP"; }
        case DDERR_CANNOTATTACHSURFACE: { return "DDERR_CANNOTATTACHSURFACE"; }
        case DDERR_CANNOTDETACHSURFACE: { return "DDERR_CANNOTDETACHSURFACE"; }
        case DDERR_CANTCREATEDC: { return "DDERR_CANTCREATEDC"; }
        case DDERR_CANTDUPLICATE: { return "DDERR_CANTDUPLICATE"; }
        case DDERR_CANTLOCKSURFACE: { return "DDERR_CANTLOCKSURFACE"; }
        case DDERR_CANTPAGELOCK: { return "DDERR_CANTPAGELOCK"; }
        case DDERR_CANTPAGEUNLOCK: { return "DDERR_CANTPAGEUNLOCK"; }
        case DDERR_CLIPPERISUSINGHWND: { return "DDERR_CLIPPERISUSINGHWND"; }
        case DDERR_COLORKEYNOTSET: { return "DDERR_COLORKEYNOTSET"; }
        case DDERR_CURRENTLYNOTAVAIL: { return "DDERR_CURRENTLYNOTAVAIL"; }
        case DDERR_DCALREADYCREATED: { return "DDERR_DCALREADYCREATED"; }
        case DDERR_DIRECTDRAWALREADYCREATED: { return "DDERR_DIRECTDRAWALREADYCREATED"; }
        case DDERR_EXCEPTION: { return "DDERR_EXCEPTION"; }
        case DDERR_EXCLUSIVEMODEALREADYSET: { return "DDERR_EXCLUSIVEMODEALREADYSET"; }
        case DDERR_GENERIC: { return "DDERR_GENERIC"; }
        case DDERR_HEIGHTALIGN: { return "DDERR_HEIGHTALIGN"; }
        case DDERR_HWNDALREADYSET: { return "DDERR_HWNDALREADYSET"; }
        case DDERR_HWNDSUBCLASSED: { return "DDERR_HWNDSUBCLASSED"; }
        case DDERR_IMPLICITLYCREATED: { return "DDERR_IMPLICITLYCREATED"; }
        case DDERR_INCOMPATIBLEPRIMARY: { return "DDERR_INCOMPATIBLEPRIMARY"; }
        case DDERR_INVALIDCAPS: { return "DDERR_INVALIDCAPS"; }
        case DDERR_INVALIDCLIPLIST: { return "DDERR_INVALIDCLIPLIST"; }
        case DDERR_INVALIDDIRECTDRAWGUID: { return "DDERR_INVALIDDIRECTDRAWGUID"; }
        case DDERR_INVALIDMODE: { return "DDERR_INVALIDMODE"; }
        case DDERR_INVALIDOBJECT: { return "DDERR_INVALIDOBJECT"; }
        case DDERR_INVALIDPARAMS: { return "DDERR_INVALIDPARAMS"; }
        case DDERR_INVALIDPIXELFORMAT: { return "DDERR_INVALIDPIXELFORMAT"; }
        case DDERR_INVALIDPOSITION: { return "DDERR_INVALIDPOSITION"; }
        case DDERR_INVALIDRECT: { return "DDERR_INVALIDRECT"; }
        case DDERR_INVALIDSURFACETYPE: { return "DDERR_INVALIDSURFACETYPE"; }
        case DDERR_LOCKEDSURFACES: { return "DDERR_LOCKEDSURFACES"; }
        case DDERR_NO3D: { return "DDERR_NO3D"; }
        case DDERR_NOALPHAHW: { return "DDERR_NOALPHAHW"; }
        case DDERR_NOBLTHW: { return "DDERR_NOBLTHW"; }
        case DDERR_NOCLIPLIST: { return "DDERR_NOCLIPLIST"; }
        case DDERR_NOCLIPPERATTACHED: { return "DDERR_NOCLIPPERATTACHED"; }
        case DDERR_NOCOLORCONVHW: { return "DDERR_NOCOLORCONVHW"; }
        case DDERR_NOCOLORKEY: { return "DDERR_NOCOLORKEY"; }
        case DDERR_NOCOLORKEYHW: { return "DDERR_NOCOLORKEYHW"; }
        case DDERR_NOCOOPERATIVELEVELSET: { return "DDERR_NOCOOPERATIVELEVELSET"; }
        case DDERR_NODC: { return "DDERR_NODC"; }
        case DDERR_NODDROPSHW: { return "DDERR_NODDROPSHW"; }
        case DDERR_NODIRECTDRAWHW: { return "DDERR_NODIRECTDRAWHW"; }
        case DDERR_NODIRECTDRAWSUPPORT: { return "DDERR_NODIRECTDRAWSUPPORT"; }
        case DDERR_NOEMULATION: { return "DDERR_NOEMULATION"; }
        case DDERR_NOEXCLUSIVEMODE: { return "DDERR_NOEXCLUSIVEMODE"; }
        case DDERR_NOFLIPHW: { return "DDERR_NOFLIPHW"; }
        case DDERR_NOGDI: { return "DDERR_NOGDI"; }
        case DDERR_NOHWND: { return "DDERR_NOHWND"; }
        case DDERR_NOMIPMAPHW: { return "DDERR_NOMIPMAPHW"; }
        case DDERR_NOMIRRORHW: { return "DDERR_NOMIRRORHW"; }
        case DDERR_NOOVERLAYDEST: { return "DDERR_NOOVERLAYDEST"; }
        case DDERR_NOOVERLAYHW: { return "DDERR_NOOVERLAYHW"; }
        case DDERR_NOPALETTEATTACHED: { return "DDERR_NOPALETTEATTACHED"; }
        case DDERR_NOPALETTEHW: { return "DDERR_NOPALETTEHW"; }
        case DDERR_NORASTEROPHW: { return "DDERR_NORASTEROPHW"; }
        case DDERR_NOROTATIONHW: { return "DDERR_NOROTATIONHW"; }
        case DDERR_NOSTRETCHHW: { return "DDERR_NOSTRETCHHW"; }
        case DDERR_NOT4BITCOLOR: {return "DDERR_NOT4BITCOLOR"; }
        case DDERR_NOT4BITCOLORINDEX: { return "DDERR_NOT4BITCOLORINDEX"; }
        case DDERR_NOT8BITCOLOR: { return "DDERR_NOT8BITCOLOR"; }
        case DDERR_NOTAOVERLAYSURFACE: { return "DDERR_NOTAOVERLAYSURFACE"; }
        case DDERR_NOTEXTUREHW: { return "DDERR_NOTEXTUREHW"; }
        case DDERR_NOTFLIPPABLE: { return "DDERR_NOTFLIPPABLE"; }
        case DDERR_NOTFOUND: { return "DDERR_NOTFOUND"; }
        case DDERR_NOTINITIALIZED: { return "DDERR_NOTINITIALIZED"; }
        case DDERR_NOTLOCKED: { return "DDERR_NOTLOCKED"; }
        case DDERR_NOTPAGELOCKED: { return "DDERR_NOTPAGELOCKED"; }
        case DDERR_NOTPALETTIZED: { return "DDERR_NOTPALETTIZED"; }
        case DDERR_NOVSYNCHW: { return "DDERR_NOVSYNCHW"; }
        case DDERR_NOZBUFFERHW: { return "DDERR_NOZBUFFERHW"; }
        case DDERR_NOZOVERLAYHW: { return "DDERR_NOZOVERLAYHW"; }
        case DDERR_OUTOFCAPS: { return "DDERR_OUTOFCAPS"; }
        case DDERR_OUTOFMEMORY: { return "DDERR_OUTOFMEMORY"; }
        case DDERR_OUTOFVIDEOMEMORY: { return "DDERR_OUTOFVIDEOMEMORY"; }
        case DDERR_OVERLAYCANTCLIP: { return "DDERR_OVERLAYCANTCLIP"; }
        case DDERR_OVERLAYCOLORKEYONLYONEACTIVE: { return "DDERR_OVERLAYCOLORKEYONLYONEACTIVE"; }
        case DDERR_OVERLAYNOTVISIBLE: { return "DDERR_OVERLAYNOTVISIBLE"; }
        case DDERR_PALETTEBUSY: { return "DDERR_PALETTEBUSY"; }
        case DDERR_PRIMARYSURFACEALREADYEXISTS: { return "DDERR_PRIMARYSURFACEALREADYEXISTS"; }
        case DDERR_REGIONTOOSMALL: { return "DDERR_REGIONTOOSMALL"; }
        case DDERR_SURFACEALREADYATTACHED: { return "DDERR_SURFACEALREADYATTACHED"; }
        case DDERR_SURFACEALREADYDEPENDENT: { return "DDERR_SURFACEALREADYDEPENDENT"; }
        case DDERR_SURFACEBUSY: { return "DDERR_SURFACEBUSY"; }
        case DDERR_SURFACEISOBSCURED: { return "DDERR_SURFACEISOBSCURED"; }
        case DDERR_SURFACELOST: { return "DDERR_SURFACELOST"; }
        case DDERR_SURFACENOTATTACHED: { return "DDERR_SURFACENOTATTACHED"; }
        case DDERR_TOOBIGHEIGHT: { return "DDERR_TOOBIGHEIGHT"; }
        case DDERR_TOOBIGSIZE: { return "DDERR_TOOBIGSIZE"; }
        case DDERR_TOOBIGWIDTH: { return "DDERR_TOOBIGWIDTH"; }
        case DDERR_UNSUPPORTED: { return "DDERR_UNSUPPORTED"; }
        case DDERR_UNSUPPORTEDFORMAT: { return "DDERR_UNSUPPORTEDFORMAT"; }
        case DDERR_UNSUPPORTEDMASK: { return "DDERR_UNSUPPORTEDMASK"; }
        case DDERR_UNSUPPORTEDMODE: { return "DDERR_UNSUPPORTEDMODE"; }
        case DDERR_VERTICALBLANKINPROGRESS: { return "DDERR_VERTICALBLANKINPROGRESS"; }
        case DDERR_WASSTILLDRAWING: { return "DDERR_WASSTILLDRAWING"; }
        case DDERR_WRONGMODE: { return "DDERR_WRONGMODE"; }
        case DDERR_XALIGN: { return "DDERR_XALIGN"; }
        }

        static char buffer[MAX_RENDERER_MESSAGE_BUFFER_LENGTH]; // 0x6001d58c

        sprintf(buffer, "Unknown error code (0x%lx, %d)", code, code & 0xffff);

        return buffer;
    }

    // 0x60003870
    const char* AcquireRendererMessage(const HRESULT code)
    {
        static char buffer[MAX_RENDERER_MESSAGE_BUFFER_LENGTH]; // 0x6001ce98

        sprintf(buffer, "DX6 Error Code: %s (%8x)", AcquireRendererMessageDescription(code), code);

        return buffer;
    }

    // 0x60001f50
    void SelectRendererDevice(void)
    {
        if (RendererDeviceIndex < DEFAULT_DEVICE_INDEX)
        {
            State.Device.Capabilities.MinimumTextureWidth = 1;

            if (State.Lambdas.Lambdas.AcquireWindow != NULL || State.Window.HWND != NULL)
            {
                const char* value = getenv(RENDERER_MODULE_DISPLAY_ENVIRONMENT_PROPERTY_NAME);

                SelectDevice(value == NULL ? DEFAULT_DEVICE_INDEX : atoi(value));
            }
        }
    }

    // 0x600096a0
    u32 ClearRendererViewPort(const u32 x0, const u32 y0, const u32 x1, const u32 y1)
    {
        D3DRECT rect;

        DWORD options = (State.Device.Capabilities.IsDepthAvailable && State.Window.Bits != 0)
            ? (D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET) : D3DCLEAR_TARGET;

        if (State.Device.Capabilities.IsStencilBuffer) { options = options | D3DCLEAR_STENCIL; }

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

        return State.DX.ViewPort->Clear(1, &rect, options) == DD_OK;
    }

    // 0x60009290
    u32 EndRendererScene(void)
    {
        if (State.Data.Vertexes.Count != 0) { RendererRenderScene(); }

        if (State.Scene.IsActive)
        {
            State.Scene.IsActive = FALSE;

            RendererDepthBias = 0.0f;

            State.DX.Device->SetRenderState(D3DRENDERSTATE_FLUSHBATCH, TRUE);

            return State.DX.Device->EndScene() == DD_OK;
        }

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60007d10
    u32 RendererRenderScene(void)
    {
        if (!State.Scene.IsActive)
        {
            BeginRendererScene();

            State.Scene.IsActive = TRUE;
        }

        if (SettingsState.VertexBuffer)
        {
            State.Data.Vertexes.Buffer->Unlock();

            State.DX.Device->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST,
                State.Data.Vertexes.Buffer, State.Data.Indexes.Indexes, State.Data.Indexes.Count,
                D3DDP_DONOTLIGHT | D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);

            State.Data.Vertexes.Count = 0;
            State.Data.Indexes.Count = 0;

            return State.Data.Vertexes.Buffer->Lock(DDLOCK_NOSYSLOCK | DDLOCK_WRITEONLY | DDLOCK_WAIT, (void**)&State.Data.Vertexes.Vertexes, NULL);
        }
        else
        {
            const HRESULT result = State.DX.Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
                RendererVertexType,
                State.Data.Vertexes.Vertexes, State.Data.Vertexes.Count,
                State.Data.Indexes.Indexes, State.Data.Indexes.Count,
                D3DDP_DONOTLIGHT | D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP);

            State.Data.Vertexes.Count = 0;
            State.Data.Indexes.Count = 0;

            return result;
        }
    }

    // 0x60009230
    // a.k.a. startrender
    BOOL BeginRendererScene(void)
    {
        if (State.Lock.IsActive)
        {
            LOGERROR("D3D startrender called in while locked\n");

            if (State.Lock.IsActive) { UnlockGameWindow(NULL); }
        }

        return State.DX.Device->BeginScene() == DD_OK;
    }

    // 0x60001e20
    u32 AcquireRendererDeviceCount(void)
    {
        State.Devices.Count = 0;
        State.Device.Identifier = NULL;

        GUID* uids[MAX_ENUMERATE_DEVICE_COUNT];

        State.Devices.Count = AcquireDirectDrawDeviceCount(uids, NULL, ENVIRONMENT_SECTION_NAME);

        for (u32 x = 0; x < State.Devices.Count; x++)
        {
            if (AcquireRendererDeviceAccelerationState(x))
            {
                IDirectDraw* instance = NULL;
                DirectDrawCreate(uids[x], &instance, NULL);

                IDirectDraw4* dd = NULL;
                if (instance->QueryInterface(IID_IDirectDraw4, (void**)&dd) != DD_OK) { instance->Release(); continue; }

                DDDEVICEIDENTIFIER identifier;
                ZeroMemory(&identifier, sizeof(DDDEVICEIDENTIFIER));

                dd->GetDeviceIdentifier(&identifier, DDGDI_GETHOSTIDENTIFIER);

                strncpy(State.Devices.Enumeration.Names[x], identifier.szDescription, MAX_ENUMERATE_DEVICE_NAME_LENGTH);

                if (dd != NULL) { dd->Release(); dd = NULL; }
                if (instance != NULL) { instance->Release(); instance = NULL; }
            }
        }

        State.Device.Identifier = State.Devices.Indexes[0];

        return State.Devices.Count;
    }

    // 0x60009930
    u32 AcquireDirectDrawDeviceCount(GUID** uids, HMONITOR** monitors, const char* section)
    {
        State.Devices.Enumeration.Count = 0;
        State.Devices.Enumeration.IsAvailable = FALSE;

        DirectDrawEnumerateExA(EnumerateDirectDrawDevices, NULL, DDENUM_ATTACHEDSECONDARYDEVICES);
        DirectDrawEnumerateExA(EnumerateDirectDrawDevices, NULL, DDENUM_NONDISPLAYDEVICES);

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

    // 0x600099b0
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

    // 0x60009b30
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

    // 0x60009460
    // a.k.a. showbackbuffer
    u32 ToggleRenderer(void)
    {
        if (!State.Settings.IsWindowMode)
        {
            if (State.DX.Active.Surfaces.Back != NULL)
            {
                const HRESULT result = State.DX.Active.Surfaces.Main->Flip(NULL, DDFLIP_WAIT);

                if (result == DDERR_SURFACELOST || result == DDERR_NOTFLIPPABLE) { return RENDERER_MODULE_FAILURE; }

                if (result != DD_OK)
                {
                    LOGERROR("D3D Flipping complex display surface failed err %s\n", AcquireRendererMessage(result));

                    return RENDERER_MODULE_FAILURE;
                }
            }
        }
        else
        {
            POINT point;
            ZeroMemory(&point, sizeof(POINT));

            ClientToScreen(State.Window.HWND, &point);

            RECT r1, r2;

            SetRect(&r1, point.x, point.y, State.Window.Width + point.x, State.Window.Height + point.y);
            SetRect(&r2, 0, 0, State.Window.Width, State.Window.Height);

            const HRESULT result = State.DX.Active.Surfaces.Active.Main->Blt(&r1,
                State.DX.Active.Surfaces.Active.Back, &r2, DDBLT_WAIT, NULL);

            if (result == DDERR_SURFACELOST) { return RENDERER_MODULE_FAILURE; }

            if (result != DD_OK)
            {
                LOGERROR("D3D showbackbuffer - error %s\n", AcquireRendererMessage(result));

                return RENDERER_MODULE_FAILURE;
            }
        }

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60005f10
    BOOL AcquireRendererDeviceState(void)
    {
        return State.DX.Active.IsInit;
    }

    // 0x60005dc0
    void ReleaseRendererDevice(void)
    {
        if (AcquireRendererDeviceState() && State.Scene.IsActive)
        {
            FlushGameWindow();
            SyncGameWindow(0);
            Idle();

            State.Scene.IsActive = FALSE;
        }

        if (State.DX.Clipper != NULL)
        {
            State.DX.Clipper->Release();
            State.DX.Clipper = NULL;
        }

        if (State.DX.GammaControl != NULL)
        {
            State.DX.GammaControl->Release();
            State.DX.GammaControl = NULL;
        }

        if (State.Data.Vertexes.Buffer != NULL)
        {
            State.Data.Vertexes.Buffer->Unlock();
            State.Data.Vertexes.Buffer->Release();
            State.Data.Vertexes.Buffer = NULL;
        }

        ResetTextures();

        State.DX.Active.Instance = NULL;

        State.DX.Active.Surfaces.Main = NULL;
        State.DX.Active.Surfaces.Back = NULL;
        State.DX.Active.Surfaces.Active.Main = NULL;
        State.DX.Active.Surfaces.Active.Back = NULL;

        if (State.DX.Active.Surfaces.Active.Depth != NULL)
        {
            State.DX.Device->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE);
            State.DX.Device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);

            State.DX.Active.Surfaces.Active.Depth->Release();
            State.DX.Active.Surfaces.Active.Depth = NULL;
        }

        if (State.DX.Active.Surfaces.Depth != NULL)
        {
            State.DX.Active.Surfaces.Depth->Release();
            State.DX.Active.Surfaces.Depth = NULL;
        }

        if (State.DX.Material != NULL)
        {
            State.DX.Material->Release();
            State.DX.Material = NULL;
        }

        if (State.DX.ViewPort != NULL)
        {
            State.DX.ViewPort->Release();
            State.DX.ViewPort = NULL;
        }

        if (State.DX.Device != NULL)
        {
            State.DX.Device->Release();
            State.DX.Device = NULL;
        }

        if (State.DX.DirectX != NULL)
        {
            State.DX.DirectX->Release();
            State.DX.DirectX = NULL;
        }

        State.DX.Active.IsInit = FALSE;
    }

    // 0x600034c0
    u32 ReleaseRendererWindow(void)
    {
        if (State.DX.Instance != NULL)
        {
            SetForegroundWindow(State.Window.HWND);
            PostMessageA(State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_RELEASE_DEVICE, 0, 0);
            WaitForSingleObject(State.Mutex, INFINITE);
            CloseHandle(State.Mutex);

            State.Mutex = NULL;
            State.Window.HWND = NULL;

            return State.DX.Code;
        }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60003530
    u32 ReleaseRendererDeviceInstance(void)
    {
        ReleaseRendererDeviceSurfaces();

        State.DX.Instance->Release();
        State.DX.Instance = NULL;

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60003550
    void ReleaseRendererDeviceSurfaces(void)
    {
        for (s32 x = (MAX_ACTIVE_SURFACE_COUNT - 1); x >= 0; x--)
        {
            if (State.DX.Surfaces.Active[x] != NULL)
            {
                State.DX.Surfaces.Active[x]->Release();
                State.DX.Surfaces.Active[x] = NULL;
            }
        }

        if (State.DX.Surfaces.Back != NULL)
        {
            State.DX.Surfaces.Back->Release();
            State.DX.Surfaces.Back = NULL;
        }

        if (State.DX.Surfaces.Main != NULL)
        {
            State.DX.Surfaces.Main->Release();
            State.DX.Surfaces.Main = NULL;
        }
    }

    // 0x60002780
    u32 InitializeRendererDevice(void)
    {
        if (State.Window.HWND != NULL)
        {
            State.Settings.CooperativeLevel = (State.Settings.CooperativeLevel & ~(DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN))
                | (DDSCL_MULTITHREADED | DDSCL_NORMAL);

            AcquireWindowModeCapabilities();

            State.Settings.CooperativeLevel = State.Settings.IsWindowMode
                ? State.Settings.CooperativeLevel | (DDSCL_MULTITHREADED | DDSCL_NORMAL)
                : State.Settings.CooperativeLevel | (DDSCL_MULTITHREADED | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);

            IDirectDraw* instance = NULL;
            State.DX.Code = DirectDrawCreate(State.Device.Identifier, &instance, NULL);

            if (State.DX.Code == DD_OK)
            {
                State.DX.Code = instance->QueryInterface(IID_IDirectDraw4, (void**)&State.DX.Instance);

                instance->Release();

                if (State.DX.Code == DD_OK)
                {
                    State.DX.Code = State.DX.Instance->SetCooperativeLevel(State.Window.HWND, State.Settings.CooperativeLevel);

                    if (State.DX.Code == DD_OK)
                    {
                        {
                            DWORD total = 0, free = 0;

                            {
                                DDSCAPS2 caps;
                                ZeroMemory(&caps, sizeof(DDSCAPS2));

                                caps.dwCaps = DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;

                                const HRESULT result = State.DX.Instance->GetAvailableVidMem(&caps, &total, &free);

                                ModuleDescriptor.MemorySize = result == DD_OK ? free : 0;
                            }

                            {

                                DDSCAPS2 caps;
                                ZeroMemory(&caps, sizeof(DDSCAPS2));

                                caps.dwCaps = DDSCAPS_NONLOCALVIDMEM | DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;

                                const HRESULT result = State.DX.Instance->GetAvailableVidMem(&caps, &total, &free);

                                ModuleDescriptor.MemoryType = result == DD_OK ? total : 0;
                            }
                        }

                        {
                            DDCAPS hal;
                            ZeroMemory(&hal, sizeof(DDCAPS));

                            hal.dwSize = sizeof(DDCAPS);

                            DDCAPS hel;
                            ZeroMemory(&hel, sizeof(DDCAPS));

                            hel.dwSize = sizeof(DDCAPS);

                            if (State.DX.Instance->GetCaps(&hal, &hel) == DD_OK)
                            {
                                State.Device.Capabilities.IsAccelerated = hal.dwCaps & DDCAPS_3D;
                            }
                        }

                        ZeroMemory(ModuleDescriptor.Capabilities.Capabilities,
                            MAX_DEVICE_CAPABILITIES_COUNT * sizeof(RendererModuleDescriptorDeviceCapabilities));

                        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Width = GRAPHICS_RESOLUTION_640;
                        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Height = GRAPHICS_RESOLUTION_480;
                        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Bits = GRAPHICS_BITS_PER_PIXEL_16;
                        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Format = RENDERER_PIXEL_FORMAT_R5G6B5;
                        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Unk03 = 1;
                        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Unk04 = 1;
                        ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].IsActive = TRUE;

                        ModuleDescriptor.Capabilities.Count = 2;

                        State.DX.Instance->EnumDisplayModes(DDEDM_NONE, NULL,
                            &ModuleDescriptor.Capabilities.Count, EnumerateRendererDeviceModes);

                        return RENDERER_MODULE_SUCCESS;
                    }
                }
            }
        }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x600025a0
    void AcquireWindowModeCapabilities(void)
    {
        IDirectDraw* instance = NULL;
        State.DX.Code = DirectDrawCreate(State.Device.Identifier, &instance, NULL);
        State.DX.Code = instance->SetCooperativeLevel(State.Window.HWND, State.Settings.CooperativeLevel);

        IDirectDraw4* dd = NULL;
        State.DX.Code = instance->QueryInterface(IID_IDirectDraw4, (void**)&dd);

        DDCAPS hal;
        ZeroMemory(&hal, sizeof(DDCAPS));

        hal.dwSize = sizeof(DDCAPS);

        DDCAPS hel;
        ZeroMemory(&hel, sizeof(DDCAPS));

        hel.dwSize = sizeof(DDCAPS);

        dd->GetCaps(&hal, &hel);

        if ((hal.dwCaps2 & DDCAPS2_CANRENDERWINDOWED) == 0) { State.Settings.IsWindowMode = FALSE; }

        const HWND hwnd = GetDesktopWindow();
        const HDC hdc = GetWindowDC(hwnd);

        const u32 bits = GetDeviceCaps(hdc, BITSPIXEL);

        if (bits < (GRAPHICS_BITS_PER_PIXEL_8 + 1)) { State.Settings.IsWindowMode = FALSE; }

        if (bits == GRAPHICS_BITS_PER_PIXEL_24 || bits == GRAPHICS_BITS_PER_PIXEL_32)
        {
            IDirect3D3* dx = NULL;

            {
                const HRESULT result = instance->QueryInterface(IID_IDirect3D3, (void**)&dx);

                if (result != DD_OK) { LOGERROR("SetDesktopMode Test failed! %s\n", AcquireRendererMessage(result)); }
            }

            {
                const HRESULT result = dx->EnumZBufferFormats(IID_IDirect3DHALDevice, EnumerateRendererDeviceDepthPixelFormats, NULL);

                if (result != DD_OK) { LOGERROR("DX6_SetDesktopModeIfNotWin: %s\n", AcquireRendererMessage(result)); }
            }

            BOOL found = FALSE;

            for (u32 x = 0; x < MAX_TEXTURE_DEPTH_FORMAT_COUNT; x++)
            {
                if (State.Textures.Formats.Depth.Formats[x] == bits) { found = TRUE; }
            }

            if (!found) { State.Settings.IsWindowMode = FALSE; }

            if (dx != NULL) { dx->Release(); }
        }

        dd->Release();
    }

    // 0x60002750
    HRESULT CALLBACK EnumerateRendererDeviceDepthPixelFormats(LPDDPIXELFORMAT format, LPVOID context)
    {
        State.Textures.Formats.Depth.Formats[State.Textures.Formats.Depth.Count] = format->dwRGBBitCount;
        State.Textures.Formats.Depth.Count = State.Textures.Formats.Depth.Count + 1;

        // NOTE: The original does not have this check,
        // thus it is prone the array overflow that can cause crash in some cases.
        if (MAX_TEXTURE_DEPTH_FORMAT_COUNT <= (State.Textures.Formats.Depth.Count + 1)) { return D3DENUMRET_CANCEL; }

        return D3DENUMRET_OK;
    }

    // 0x60002a30
    HRESULT CALLBACK EnumerateRendererDeviceModes(LPDDSURFACEDESC2 desc, LPVOID context)
    {
        const u32 format = AcquirePixelFormat(&desc->ddpfPixelFormat);

        if (format != RENDERER_PIXEL_FORMAT_NONE)
        {
            if (desc->dwWidth < GRAPHICS_RESOLUTION_640 || desc->dwHeight < GRAPHICS_RESOLUTION_480) { return DDENUMRET_OK; }

            const u32 bits = desc->ddpfPixelFormat.dwRGBBitCount;
            const u32 bytes = bits == (GRAPHICS_BITS_PER_PIXEL_16 - 1) ? 2 : (bits >> 3);

            const u32 width = desc->dwWidth;
            const u32 height = desc->dwHeight;
            const u32 count = State.Settings.MaxAvailableMemory / (height * width * bytes);

            u32 indx = 0;

            {
                indx = *(u32*)context;

                if ((MAX_DEVICE_CAPABILITIES_COUNT - 1) < indx) { return DDENUMRET_CANCEL; }

                *(u32*)context = indx + 1;
            }

            ModuleDescriptor.Capabilities.Capabilities[indx].Width = width;
            ModuleDescriptor.Capabilities.Capabilities[indx].Height = height;
            ModuleDescriptor.Capabilities.Capabilities[indx].Bits =
                format == RENDERER_PIXEL_FORMAT_R5G5B5 ? (GRAPHICS_BITS_PER_PIXEL_16 - 1) : bits;

            ModuleDescriptor.Capabilities.Capabilities[indx].Format = format;

            if (count < 4) // TODO
            {
                ModuleDescriptor.Capabilities.Capabilities[indx].Unk03 = count;
                ModuleDescriptor.Capabilities.Capabilities[indx].Unk04 = count - 1;
            }
            else
            {
                ModuleDescriptor.Capabilities.Capabilities[indx].Unk03 = 3;
                ModuleDescriptor.Capabilities.Capabilities[indx].Unk04 = 3;
            }

            ModuleDescriptor.Capabilities.Capabilities[indx].IsActive = TRUE;
        }

        return DDENUMRET_OK;
    }

    // 0x600037b0
    u32 AcquirePixelFormat(const DDPIXELFORMAT* format)
    {
        const u32 bits = format->dwRGBBitCount;

        const u32 red = format->dwRBitMask;
        const u32 green = format->dwGBitMask;
        const u32 blue = format->dwBBitMask;

        if (bits == GRAPHICS_BITS_PER_PIXEL_16)
        {
            if (red == 0x7c00 && green == 0x3e0 && blue == 0x1f) { return RENDERER_PIXEL_FORMAT_R5G5B5; }
            else if (red == 0xf800 && green == 0x7e0 && blue == 0x1f) { return RENDERER_PIXEL_FORMAT_R5G6B5; }
            else if (red == 0xf00 && green == 0xf0 && blue == 0xf && format->dwRGBAlphaBitMask == 0xf000) { return RENDERER_PIXEL_FORMAT_R4G4B4; }
        }
        else if (red == 0xff0000 && green == 0xff00 && blue == 0xff)
        {
            if (bits == GRAPHICS_BITS_PER_PIXEL_24) { return RENDERER_PIXEL_FORMAT_R8G8B8; }
            else if (bits == GRAPHICS_BITS_PER_PIXEL_32) { return RENDERER_PIXEL_FORMAT_A8R8G8B8; }
        }

        return RENDERER_PIXEL_FORMAT_NONE;
    }

    // 0x60002b70
    u32 InitializeRendererDeviceLambdas(void)
    {
        if (State.Mutex == NULL) { State.Mutex = CreateEventA(NULL, FALSE, FALSE, NULL); }

        State.Window.HWND = State.Lambdas.Lambdas.AcquireWindow();

        if (State.Window.HWND != NULL)
        {
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE, (RENDERERMODULEEXECUTECALLBACK)InitializeRendererDeviceExecute);
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_RELEASE_DEVICE, (RENDERERMODULEEXECUTECALLBACK)ReleaseRendererDeviceExecute);
            State.Lambdas.Lambdas.Execute(RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_SURFACES, (RENDERERMODULEEXECUTECALLBACK)InitializeRendererDeviceSurfacesExecute);

            State.Settings.CooperativeLevel = (State.Settings.CooperativeLevel & ~(DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN))
                | (DDSCL_MULTITHREADED | DDSCL_NORMAL);

            AcquireWindowModeCapabilities();

            State.Settings.CooperativeLevel = State.Settings.IsWindowMode
                ? State.Settings.CooperativeLevel | DDSCL_MULTITHREADED | DDSCL_NORMAL
                : State.Settings.CooperativeLevel | DDSCL_MULTITHREADED | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN;

            SetForegroundWindow(State.Window.HWND);
            PostMessageA(State.Window.HWND, RENDERER_MODULE_WINDOW_MESSAGE_INITIALIZE_DEVICE, 0, 0);
            WaitForSingleObject(State.Mutex, 10000);

            return State.DX.Code;
        }

        State.DX.Code = RENDERER_MODULE_INITIALIZE_DEVICE_SUCCESS;

        return RENDERER_MODULE_INITIALIZE_DEVICE_SUCCESS;
    }

    // 0x60002c50
    u32 STDCALLAPI InitializeRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        IDirectDraw* instance;
        State.DX.Code = DirectDrawCreate(State.Device.Identifier, &instance, NULL);

        if (State.DX.Code == DD_OK)
        {
            State.DX.Code = instance->QueryInterface(IID_IDirectDraw4, (void**)&State.DX.Instance);

            State.Lambdas.Lambdas.SelectInstance(instance);

            instance->Release();

            State.DX.Code = State.DX.Instance->SetCooperativeLevel(State.Window.HWND, State.Settings.CooperativeLevel);

            if (State.DX.Code == DD_OK)
            {
                u32 pitch = 0;
                u32 height = 0;

                {
                    DDSURFACEDESC2 desc;
                    ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

                    desc.dwSize = sizeof(DDSURFACEDESC2);
                    desc.dwFlags = DDSD_CAPS;
                    desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

                    IDirectDrawSurface4* surface = NULL;
                    State.DX.Code = State.DX.Instance->CreateSurface(&desc, &surface, NULL);

                    if (State.DX.Code == DD_OK)
                    {
                        surface->GetSurfaceDesc(&desc);

                        DDCAPS hal;
                        ZeroMemory(&hal, sizeof(DDCAPS));

                        hal.dwSize = sizeof(DDCAPS);
                        hal.dwCaps = DDCAPS_BLTDEPTHFILL | DDCAPS_OVERLAYSTRETCH;

                        DDCAPS hel;
                        ZeroMemory(&hel, sizeof(DDCAPS));

                        hel.dwSize = sizeof(DDCAPS);

                        State.DX.Instance->GetCaps(&hal, &hel);

                        pitch = desc.lPitch;
                        height = desc.dwHeight;
                    }
                    else
                    {
                        LOGWARNING("*** FAILURE in creating primary surface (error code: %8x)***\n", State.DX.Code);
                    }

                    if (surface != NULL) { surface->Release(); }
                }

                {
                    DWORD free = 0;
                    DWORD total = 0;

                    {
                        DDSCAPS2 caps;
                        ZeroMemory(&caps, sizeof(DDSCAPS2));

                        caps.dwCaps = DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;

                        const HRESULT result = State.DX.Instance->GetAvailableVidMem(&caps, &total, &free);

                        State.Settings.MaxAvailableMemory = result == DD_OK
                            ? height * pitch + total
                            : MIN_DEVICE_AVAIABLE_VIDEO_MEMORY;
                    }

                    {
                        DDSCAPS2 caps;
                        ZeroMemory(&caps, sizeof(DDSCAPS2));

                        caps.dwCaps = DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;

                        if (State.DX.Instance->GetAvailableVidMem(&caps, &total, &free) == DD_OK)
                        {
                            ModuleDescriptor.MemoryType = 3; // TODO
                            ModuleDescriptor.MemorySize = State.Settings.MaxAvailableMemory;
                        }
                        else
                        {
                            ModuleDescriptor.MemoryType = 0; // TODO
                            ModuleDescriptor.MemorySize = 0;
                        }
                    }

                    {
                        DDSCAPS2 caps;
                        ZeroMemory(&caps, sizeof(DDSCAPS2));

                        caps.dwCaps = DDSCAPS_NONLOCALVIDMEM | DDSCAPS_VIDEOMEMORY | DDSCAPS_TEXTURE;

                        if (State.DX.Instance->GetAvailableVidMem(&caps, &total, &free) == DD_OK)
                        {
                            if (total != 0) { ModuleDescriptor.MemoryType = ModuleDescriptor.MemoryType | 0x8000; } // TODO
                        }
                    }
                }

                {
                    DDCAPS hal;
                    ZeroMemory(&hal, sizeof(DDCAPS));

                    hal.dwSize = sizeof(DDCAPS);

                    DDCAPS hel;
                    ZeroMemory(&hel, sizeof(DDCAPS));

                    hel.dwSize = sizeof(hel);

                    if (State.DX.Instance->GetCaps(&hal, &hel) == DD_OK)
                    {
                        State.Device.Capabilities.IsAccelerated = hal.dwCaps & DDCAPS_3D;
                    }
                }

                ZeroMemory(ModuleDescriptor.Capabilities.Capabilities,
                    MAX_DEVICE_CAPABILITIES_COUNT * sizeof(RendererModuleDescriptorDeviceCapabilities));

                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Width = GRAPHICS_RESOLUTION_640;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Height = GRAPHICS_RESOLUTION_480;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Bits = GRAPHICS_BITS_PER_PIXEL_16;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Format = RENDERER_PIXEL_FORMAT_R5G6B5;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Unk03 = 3;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].Unk04 = 2;
                ModuleDescriptor.Capabilities.Capabilities[RENDERER_RESOLUTION_MODE_640_480_16].IsActive = TRUE;

                ModuleDescriptor.Capabilities.Count = 2;

                State.DX.Instance->EnumDisplayModes(DDEDM_NONE, NULL,
                    &ModuleDescriptor.Capabilities.Count, EnumerateRendererDeviceModes);
            }
        }

        SetEvent(State.Mutex);

        *result = State.DX.Code;

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60003480
    u32 STDCALLAPI ReleaseRendererDeviceExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        ReleaseRendererDeviceSurfaces();

        State.Lambdas.Lambdas.SelectInstance(NULL);

        State.DX.Instance->Release();
        State.DX.Instance = NULL;

        SetEvent(State.Mutex);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60003020
    u32 STDCALLAPI InitializeRendererDeviceSurfacesExecute(const void*, const HWND hwnd, const u32 msg, const u32 wp, const u32 lp, HRESULT* result)
    {
        ReleaseRendererDevice();
        ReleaseRendererDeviceSurfaces();

        if (!State.Settings.IsWindowMode)
        {
            State.Window.Width = ModuleDescriptor.Capabilities.Capabilities[wp].Width;
            State.Window.Height = ModuleDescriptor.Capabilities.Capabilities[wp].Height;
        }
        else
        {
            const HDC hdc = GetDC(hwnd);

            State.Window.Width = GetDeviceCaps(hdc, HORZRES);
            State.Window.Height = GetDeviceCaps(hdc, VERTRES);

            ReleaseDC(hwnd, hdc);

            RECT rect;
            GetClientRect(State.Window.HWND, &rect);

            State.Window.Width = rect.right - rect.left;
            State.Window.Height = rect.bottom - rect.top;
        }

        const u32 bits = ModuleDescriptor.Capabilities.Capabilities[wp].Bits == (GRAPHICS_BITS_PER_PIXEL_16 - 1)
            ? GRAPHICS_BITS_PER_PIXEL_16 : ModuleDescriptor.Capabilities.Capabilities[wp].Bits;

        State.DX.Code = DD_OK;

        if (!State.Settings.IsWindowMode)
        {
            State.DX.Code = State.DX.Instance->SetDisplayMode(ModuleDescriptor.Capabilities.Capabilities[wp].Width,
                ModuleDescriptor.Capabilities.Capabilities[wp].Height, bits, 0, DDSDM_NONE);

            if (State.DX.Code == DD_OK)
            {
                DDSURFACEDESC2 desc;
                ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

                desc.dwSize = sizeof(DDSURFACEDESC2);

                if (lp - 1 == 0)
                {
                    State.DX.Active.Surfaces.Active.Back = NULL;
                    State.DX.Active.Surfaces.Back = NULL;
                    State.DX.Surfaces.Active[2] = NULL;
                }
                else
                {
                    desc.dwFlags = DDSD_BACKBUFFERCOUNT | DDSD_CAPS;
                    desc.dwBackBufferCount = lp - 1;
                }

                const u32 options = (lp - 1 == 0)
                    ? DDSCAPS_3DDEVICE | DDSCAPS_PRIMARYSURFACE
                    : DDSCAPS_3DDEVICE | DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

                desc.ddsCaps.dwCaps = State.Device.Capabilities.IsAccelerated
                    ? options | DDSCAPS_VIDEOMEMORY
                    : options | DDSCAPS_SYSTEMMEMORY;

                State.DX.Code = State.DX.Instance->CreateSurface(&desc, &State.DX.Surfaces.Main, NULL);

                while (State.DX.Code == DDERR_OUTOFVIDEOMEMORY && 1 < desc.dwBackBufferCount)
                {
                    desc.dwBackBufferCount = desc.dwBackBufferCount - 1;

                    State.DX.Code = State.DX.Instance->CreateSurface(&desc, &State.DX.Surfaces.Main, NULL);
                }

                if (State.DX.Surfaces.Main != NULL)
                {
                    if (State.DX.Surfaces.Back == NULL && desc.dwBackBufferCount != 0)
                    {
                        DDSCAPS2 caps;
                        ZeroMemory(&caps, sizeof(DDSCAPS2));

                        caps.dwCaps = DDSCAPS_BACKBUFFER;

                        State.DX.Code = State.DX.Surfaces.Main->GetAttachedSurface(&caps, &State.DX.Surfaces.Back);
                    }
                }

                if (State.DX.Surfaces.Main != NULL)
                {
                    State.DX.Surfaces.Main->QueryInterface(IID_IDirectDrawSurface4, (void**)&State.DX.Surfaces.Active[1]); // TODO
                }

                if (State.DX.Surfaces.Back != NULL)
                {
                    State.DX.Surfaces.Back->QueryInterface(IID_IDirectDrawSurface4, (void**)&State.DX.Surfaces.Active[2]); // TODO
                }
            }
        }
        else
        {
            DDSURFACEDESC2 desc;
            ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

            desc.dwSize = sizeof(DDSURFACEDESC2);
            desc.dwFlags = DDSD_CAPS;
            desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

            State.DX.Code = State.DX.Instance->CreateSurface(&desc, &State.DX.Surfaces.Main, NULL);

            if (State.DX.Code == DD_OK)
            {
                if (State.Settings.IsWindowMode)
                {
                    State.DX.Code = State.DX.Instance->CreateClipper(0, &State.DX.Clipper, NULL);
                    State.DX.Code = State.DX.Surfaces.Main->SetClipper(State.DX.Clipper);
                    State.DX.Code = State.DX.Clipper->SetHWnd(0, State.Window.HWND);
                }

                if (State.DX.Surfaces.Main != NULL)
                {
                    State.DX.Surfaces.Main->QueryInterface(IID_IDirectDrawSurface4, (void**)&State.DX.Surfaces.Active[1]); // TODO
                }

                desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
                desc.dwHeight = State.Window.Height;
                desc.dwWidth = State.Window.Width;
                desc.ddsCaps.dwCaps = RendererDeviceType == RENDERER_MODULE_DEVICE_TYPE_0_ACCELERATED
                    ? DDSCAPS_VIDEOMEMORY | DDSCAPS_3DDEVICE | DDSCAPS_OFFSCREENPLAIN
                    : DDSCAPS_3DDEVICE | DDSCAPS_SYSTEMMEMORY | DDSCAPS_OFFSCREENPLAIN;

                State.DX.Code = State.DX.Instance->CreateSurface(&desc, &State.DX.Surfaces.Back, NULL);

                if (State.DX.Code == DD_OK)
                {
                    State.DX.Surfaces.Back->QueryInterface(IID_IDirectDrawSurface4, (void**)&State.DX.Surfaces.Active[2]); // TODO
                }
                else if (State.DX.Code == DDERR_OUTOFMEMORY || State.DX.Code == DDERR_OUTOFVIDEOMEMORY) { LOGWARNING("There was not enough video memory to create the rendering surface.\nTo run this program in a window of this size, please adjust your display settings for a smaller desktop area or a lower palette size and restart the program.\n"); }
                else { LOGWARNING("CreateSurface for window back buffer failed %s.\n", AcquireRendererMessage(State.DX.Code)); }
            }
            else if (State.DX.Code == DDERR_OUTOFMEMORY || State.DX.Code == DDERR_OUTOFVIDEOMEMORY) { LOGWARNING("There was not enough video memory to create the rendering surface.\nTo run this program in a window of this size, please adjust your display settings for a smaller desktop area or a lower palette size and restart the program.\n"); }
            else { LOGWARNING("CreateSurface for window front buffer failed %s.\n", AcquireRendererMessage(State.DX.Code)); }
        }

        InitializeRendererDeviceAcceleration();

        State.DX.GammaControl->GetGammaRamp(0, &State.Settings.GammaControl);

        if (State.Lambdas.Lambdas.AcquireWindow != NULL)
        {
            SetEvent(State.Mutex);

            *result = State.DX.Code;
        }

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60005f20
    // a.k.a. createD3D
    u32 InitializeRendererDeviceAcceleration(void)
    {
        State.DX.Active.Instance = State.DX.Instance;

        State.DX.Active.Surfaces.Main = State.DX.Surfaces.Main;
        State.DX.Active.Surfaces.Back = State.DX.Surfaces.Back;

        State.DX.Active.Surfaces.Active.Main = State.DX.Surfaces.Active[1]; // TODO
        State.DX.Active.Surfaces.Active.Back = State.DX.Surfaces.Active[2]; // TODO

        State.DX.Surfaces.Main->QueryInterface(IID_IDirectDrawGammaControl, (void**)&State.DX.GammaControl);

        if (State.DX.Instance->QueryInterface(IID_IDirect3D3, (void**)&State.DX.DirectX) != DD_OK)
        {
            LOGERROR("Creation of IDirect3D2 failed.\nCheck DX6 installed.\n");
        }

        InitializeConcreteRendererDevice();

        D3DDEVICEDESC hal;
        ZeroMemory(&hal, sizeof(D3DDEVICEDESC));

        hal.dwSize = sizeof(D3DDEVICEDESC);

        D3DDEVICEDESC hel;
        ZeroMemory(&hel, sizeof(D3DDEVICEDESC));

        hel.dwSize = sizeof(D3DDEVICEDESC);

        if (State.DX.Device->GetCaps(&hal, &hel) != DD_OK) { LOGERROR("GetCaps of IDirect3D3 Failed\n"); }

        if (hal.dcmColorModel == D3DCOLOR_NONE)
        {
            State.Device.Capabilities.IsAccelerated = FALSE;

            State.Device.Capabilities.RendererBits = hel.dwDeviceRenderBitDepth;
            State.Device.Capabilities.IsPerspectiveTextures = hel.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_PERSPECTIVE;
            State.Device.Capabilities.IsAlphaTextures = FALSE;
            State.Device.Capabilities.IsAlphaFlatBlending = FALSE;
            State.Device.Capabilities.IsModulateBlending = FALSE;
            State.Device.Capabilities.IsSourceAlphaBlending = FALSE;
            State.Device.Capabilities.IsColorBlending = FALSE;
            State.Device.Capabilities.IsSpecularBlending = FALSE;
            State.Device.Capabilities.DepthBits = hel.dwDeviceZBufferBitDepth;
        }
        else
        {
            State.Device.Capabilities.IsAccelerated = TRUE;

            State.Device.Capabilities.MinimumTextureWidth = hal.dwMinTextureWidth;
            State.Device.Capabilities.MaximumTextureWidth = hal.dwMaxTextureWidth;
            State.Device.Capabilities.MinimumTextureHeight = hal.dwMinTextureHeight;
            State.Device.Capabilities.MaximumTextureHeight = hal.dwMaxTextureHeight;

            State.Device.Capabilities.IsSquareOnlyTextures = (hal.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY) != 0;

            State.Device.Capabilities.MultipleTextureWidth = 1;
            State.Device.Capabilities.MultipleTextureHeight = 1;

            State.Device.Capabilities.IsPowerOfTwoTexturesHeight = (hal.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2) != 0;
            State.Device.Capabilities.IsPowerOfTwoTexturesWidth = (hal.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2) != 0;

            State.Device.Capabilities.RendererBits = hal.dwDeviceRenderBitDepth;
            State.Device.Capabilities.DepthBits = hal.dwDeviceZBufferBitDepth;

            State.Device.Capabilities.AntiAliasing = (hal.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANTIALIASSORTDEPENDENT) != 0;

            if (hal.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANTIALIASSORTINDEPENDENT)
            {
                State.Device.Capabilities.AntiAliasing = State.Device.Capabilities.AntiAliasing | RENDERER_MODULE_ANTIALIASING_SORT_INDEPENDENT;
            }

            State.Device.Capabilities.IsAntiAliasEdges = (hal.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANTIALIASEDGES) != 0;
            State.Device.Capabilities.IsAnisotropyAvailable = (hal.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ANISOTROPY) != 0;
            State.Device.Capabilities.IsMipMapBiasAvailable = (hal.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS) != 0;
            State.Device.Capabilities.IsWBufferAvailable = (hal.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_WBUFFER) != 0;
            State.Device.Capabilities.IsDitherAvailable = (hal.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_DITHER) != 0;

            State.Device.Capabilities.IsPerspectiveTextures = hal.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_PERSPECTIVE;
            State.Device.Capabilities.IsAlphaTextures = (hal.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_ALPHA) != 0;

            {
                const u32 phong = hal.dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_ALPHAPHONGBLEND;
                const u32 gouraud = hal.dpcTriCaps.dwShadeCaps & (D3DPSHADECAPS_ALPHAGOURAUDBLEND | D3DPSHADECAPS_ALPHAFLATBLEND);

                State.Device.Capabilities.IsAlphaFlatBlending = (phong || gouraud) ? TRUE : FALSE;
            }

            {
                const u32 phong = hal.dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_ALPHAPHONGBLEND;
                const u32 gouraud = hal.dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_ALPHAGOURAUDBLEND;

                State.Device.Capabilities.IsAlphaProperBlending = (phong || gouraud) ? TRUE : FALSE;
            }

            State.Device.Capabilities.IsModulateBlending = (hal.dpcTriCaps.dwTextureBlendCaps & D3DPTBLENDCAPS_MODULATEALPHA) != 0;

            if ((hal.dpcTriCaps.dwSrcBlendCaps & D3DPBLENDCAPS_SRCALPHA) == 0)
            {
                State.Device.Capabilities.IsSourceAlphaBlending = TRUE;

                if ((hal.dpcTriCaps.dwDestBlendCaps & D3DPBLENDCAPS_ONE) == 0)
                {
                    State.Device.Capabilities.IsSourceAlphaBlending = FALSE;
                }
            }

            State.Device.Capabilities.IsColorBlending = (hal.dpcTriCaps.dwShadeCaps & (D3DPSHADECAPS_COLORPHONGRGB | D3DPSHADECAPS_COLORGOURAUDRGB)) != 0;
            State.Device.Capabilities.IsSpecularBlending = (hal.dpcTriCaps.dwShadeCaps & (D3DPSHADECAPS_SPECULARPHONGRGB | D3DPSHADECAPS_SPECULARGOURAUDRGB)) != 0;

            if (!State.Device.Capabilities.IsColorBlending) { State.Device.Capabilities.IsSourceAlphaBlending = FALSE; }

            if (State.Window.Bits == GRAPHICS_BITS_PER_PIXEL_32
                && (hal.dwDeviceZBufferBitDepth & (DEPTH_BIT_MASK_24_BIT | DEPTH_BIT_MASK_32_BIT)) == 0)
            {
                State.Window.Bits = GRAPHICS_BITS_PER_PIXEL_16;
            }

            for (u32 x = 0; x < ModuleDescriptor.Capabilities.Count; x++)
            {
                BOOL found = FALSE;
                u32 value = 0;

                switch (ModuleDescriptor.Capabilities.Capabilities[x].Bits)
                {
                case GRAPHICS_BITS_PER_PIXEL_8: { value = hal.dwDeviceRenderBitDepth & DEPTH_BIT_MASK_8_BIT; found = TRUE; break; }
                case GRAPHICS_BITS_PER_PIXEL_16: { value = hal.dwDeviceRenderBitDepth & DEPTH_BIT_MASK_16_BIT; found = TRUE; break; }
                case GRAPHICS_BITS_PER_PIXEL_24: { value = hal.dwDeviceRenderBitDepth & DEPTH_BIT_MASK_24_BIT; found = TRUE; break; }
                case GRAPHICS_BITS_PER_PIXEL_32: { value = hal.dwDeviceRenderBitDepth & DEPTH_BIT_MASK_32_BIT; found = TRUE; break; }
                }

                if (found && value == 0)
                {
                    ModuleDescriptor.Capabilities.Capabilities[x].Width = 0;
                    ModuleDescriptor.Capabilities.Capabilities[x].Height = 0;
                    ModuleDescriptor.Capabilities.Capabilities[x].Bits = 0;
                    ModuleDescriptor.Capabilities.Capabilities[x].Format = RENDERER_PIXEL_FORMAT_NONE;
                    ModuleDescriptor.Capabilities.Capabilities[x].IsActive = FALSE;
                    ModuleDescriptor.Capabilities.Capabilities[x].Unk03 = 0;
                    ModuleDescriptor.Capabilities.Capabilities[x].Unk04 = 0;
                }
            }

            State.Device.Capabilities.MaximumSimultaneousTextures = hal.wMaxSimultaneousTextures;

            {
                DDCAPS hal;
                ZeroMemory(&hal, sizeof(DDCAPS));

                hal.dwSize = sizeof(DDCAPS);

                DDCAPS hel;
                ZeroMemory(&hel, sizeof(DDCAPS));

                hel.dwSize = sizeof(DDCAPS);

                const HRESULT result = State.DX.Active.Instance->GetCaps(&hal, &hel);

                if (result != DD_OK) { LOGERROR("GetCaps of IDirectDraw4 Failed %s\n", AcquireRendererMessage(result)); }

                State.Device.Capabilities.IsWindowMode = (hal.dwCaps2 & DDCAPS2_CANRENDERWINDOWED) != 0;
                State.Device.Capabilities.IsPrimaryGammaAvailable = (hal.dwCaps2 & DDCAPS2_PRIMARYGAMMA) != 0;
            }

            State.Device.Capabilities.IsTrilinearInterpolationAvailable = AcquireRendererDeviceTrilinearInterpolationCapabilities();

            State.Device.Capabilities.IsDepthBufferRemovalAvailable = AcquireRendererDeviceDepthBufferRemovalCapabilities();
        }

        if ((State.Device.Capabilities.DepthBits & DEPTH_BIT_MASK_32_BIT) == 0)
        {
            if ((State.Device.Capabilities.DepthBits & DEPTH_BIT_MASK_24_BIT) == 0)
            {
                if ((State.Device.Capabilities.DepthBits & DEPTH_BIT_MASK_16_BIT) == 0)
                {
                    State.Device.Capabilities.DepthBits = (State.Device.Capabilities.DepthBits >> 8) & 8;
                }
                else
                {
                    State.Device.Capabilities.DepthBits = GRAPHICS_BITS_PER_PIXEL_16;
                }
            }
            else
            {
                State.Device.Capabilities.DepthBits = GRAPHICS_BITS_PER_PIXEL_24;
            }
        }
        else
        {
            State.Device.Capabilities.DepthBits = GRAPHICS_BITS_PER_PIXEL_32;
        }

        State.Device.Capabilities.IsDepthAvailable = FALSE;

        if (State.Window.Bits != 0 && InitializeRendererDeviceDepthSurfaces(State.Window.Width, State.Window.Height))
        {
            State.Device.Capabilities.IsDepthAvailable = TRUE;
        }

        State.DX.Device->Release();

        InitializeConcreteRendererDevice();

        AcquireRendererDeviceTextureFormats();

        State.DX.DirectX->CreateViewport(&State.DX.ViewPort, NULL);
        State.DX.Device->AddViewport(State.DX.ViewPort);

        D3DVIEWPORT2 vp;
        ZeroMemory(&vp, sizeof(D3DVIEWPORT2));

        vp.dwSize = sizeof(D3DVIEWPORT2);
        vp.dwX = 0;
        vp.dwY = 0;
        vp.dwWidth = State.Window.Width;
        vp.dwHeight = State.Window.Height;
        vp.dvClipX = -1.0f;
        vp.dvClipY = 0.5f * State.Window.Height;
        vp.dvClipWidth = 2.0f;
        vp.dvClipHeight = (State.Window.Height + State.Window.Height) / (f32)State.Window.Width;
        vp.dvMinZ = 0.0f;
        vp.dvMaxZ = 1.0f;

        State.DX.ViewPort->SetViewport2(&vp);

        State.DX.DirectX->CreateMaterial(&State.DX.Material, NULL);

        D3DMATERIAL material;
        ZeroMemory(&material, sizeof(D3DMATERIAL));

        material.dwSize = sizeof(D3DMATERIAL);
        material.dwRampSize = 1;

        State.DX.Material->SetMaterial(&material);

        D3DMATERIALHANDLE handle;
        State.DX.Material->GetHandle(State.DX.Device, &handle);

        State.DX.ViewPort->SetBackground(handle);

        InitializeRendererState();
        InitializeViewPort();

        State.DX.Active.IsInit = TRUE;

        return TRUE;
    }

    // 0x60006610
    void InitializeConcreteRendererDevice(void)
    {
        HRESULT result = DD_OK;

        if (SettingsState.Accelerate)
        {
            const HRESULT res = State.DX.DirectX->CreateDevice(IID_IDirect3DHALDevice,
                State.DX.Active.Surfaces.Back != NULL ? State.DX.Active.Surfaces.Back : State.DX.Active.Surfaces.Main,
                &State.DX.Device, NULL);

            if (res == DD_OK) { return; }
        }

        switch (RendererDeviceType)
        {
        case RENDERER_MODULE_DEVICE_TYPE_0_RAMP:
        {
            State.DX.Active.IsSoft = TRUE;

            result = State.DX.DirectX->CreateDevice(IID_IDirect3DRampDevice,
                State.DX.Active.Surfaces.Back, &State.DX.Device, NULL);

            break;
        }
        case RENDERER_MODULE_DEVICE_TYPE_0_RGB:
        {
            State.DX.Active.IsSoft = TRUE;

            result = State.DX.DirectX->CreateDevice(IID_IDirect3DRGBDevice,
                State.DX.Active.Surfaces.Back, &State.DX.Device, NULL);

            break;
        }
        case RENDERER_MODULE_DEVICE_TYPE_0_MMX:
        {
            State.DX.Active.IsSoft = TRUE;

            result = State.DX.DirectX->CreateDevice(IID_IDirect3DMMXDevice,
                State.DX.Active.Surfaces.Back, &State.DX.Device, NULL);

            break;
        }
        case RENDERER_MODULE_DEVICE_TYPE_0_INVALID:
        case RENDERER_MODULE_DEVICE_TYPE_0_ACCELERATED:
        {
            State.DX.Active.IsSoft = FALSE;

            result = State.DX.DirectX->CreateDevice(IID_IDirect3DHALDevice,
                State.DX.Active.Surfaces.Back != NULL ? State.DX.Active.Surfaces.Back : State.DX.Active.Surfaces.Main, &State.DX.Device, NULL);

            break;
        }
        default: { LOGERROR("D3D device type not recognised\n"); return; }
        }

        if (result != DD_OK) { LOGERROR(AcquireRendererMessage(result)); }
    }

    // 0x60006d20
    BOOL AcquireRendererDeviceTrilinearInterpolationCapabilities(void)
    {
        D3DDEVICEDESC hal;
        ZeroMemory(&hal, sizeof(D3DDEVICEDESC));

        hal.dwSize = sizeof(D3DDEVICEDESC);

        D3DDEVICEDESC hel;
        ZeroMemory(&hel, sizeof(D3DDEVICEDESC));

        hel.dwSize = sizeof(D3DDEVICEDESC);

        State.DX.Device->GetCaps(&hal, &hel);

        return (hal.dpcTriCaps.dwTextureFilterCaps & D3DPTFILTERCAPS_LINEARMIPLINEAR) != 0;
    }

    // 0x60006d90
    BOOL AcquireRendererDeviceDepthBufferRemovalCapabilities(void)
    {
        D3DDEVICEDESC hal;
        ZeroMemory(&hal, sizeof(D3DDEVICEDESC));

        hal.dwSize = sizeof(D3DDEVICEDESC);

        D3DDEVICEDESC hel;
        ZeroMemory(&hel, sizeof(D3DDEVICEDESC));

        hel.dwSize = sizeof(D3DDEVICEDESC);

        State.DX.Device->GetCaps(&hal, &hel);

        return (hal.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ZBUFFERLESSHSR) != 0;
    }

    // 0x60006750
    // a.k.a. createzbuffer
    BOOL InitializeRendererDeviceDepthSurfaces(const u32 width, const u32 height)
    {
        if (State.Device.Capabilities.IsDepthBufferRemovalAvailable || !State.Device.Capabilities.IsAccelerated) { return TRUE; }

        DDPIXELFORMAT format;
        ZeroMemory(&format, sizeof(DDPIXELFORMAT));

        {
            const HRESULT result = State.DX.DirectX->EnumZBufferFormats(IID_IDirect3DHALDevice, EnumerateRendererDevicePixelFormats, &format);

            if (result != DD_OK) { LOGERROR("EnumZBufferFormats failure! %8x\n", result); }
        }

        if (format.dwSize != sizeof(DDPIXELFORMAT)) { return FALSE; }

        DDSURFACEDESC2 desc;
        ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

        desc.dwSize = sizeof(DDSURFACEDESC2);
        desc.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
        desc.dwHeight = height;
        desc.dwWidth = width;

        CopyMemory(&desc.ddpfPixelFormat, &format, sizeof(DDPIXELFORMAT));

        State.Device.Capabilities.IsStencilBuffer = (format.dwFlags & DDPF_STENCILBUFFER) != 0;

        desc.ddsCaps.dwCaps = State.Device.Capabilities.IsAccelerated
            ? DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY
            : DDSCAPS_ZBUFFER | DDSCAPS_SYSTEMMEMORY;

        {
            const HRESULT result = State.DX.Active.Instance->CreateSurface(&desc, &State.DX.Active.Surfaces.Depth, NULL);

            if (result != DD_OK) { LOGERROR("CreateSurface failure! %s\n", AcquireRendererMessage(result)); }
        }

        {
            const HRESULT result = State.DX.Active.Surfaces.Depth->QueryInterface(IID_IDirectDrawSurface4, (void**)&State.DX.Active.Surfaces.Active.Depth);

            if (result != DD_OK)
            {
                if (result != DDERR_OUTOFMEMORY && result != DDERR_OUTOFVIDEOMEMORY)
                {
                    LOGWARNING("CreateSurface for Z-buffer failed %s.\n", AcquireRendererMessage(result));

                    return FALSE;
                }

                LOGWARNING("There was not enough video memory to create the Z-buffer surface.\nPlease restart the program and try another fullscreen mode with less resolution or lower bit depth.\n");

                return FALSE;
            }
        }

        {
            IDirectDrawSurface4* surface = State.DX.Active.Surfaces.Back != NULL
                ? State.DX.Active.Surfaces.Back : State.DX.Active.Surfaces.Main;

            HRESULT result = surface->AddAttachedSurface(State.DX.Active.Surfaces.Depth);

            if (result == DD_OK)
            {
                DDSURFACEDESC2 sdesc;
                ZeroMemory(&sdesc, sizeof(DDSURFACEDESC2));

                sdesc.dwSize = sizeof(DDSURFACEDESC2);

                result = State.DX.Active.Surfaces.Active.Depth->GetSurfaceDesc(&sdesc);

                if (result == DD_OK)
                {
                    if (!State.Device.Capabilities.IsAccelerated) { return TRUE; }

                    if (sdesc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY) { return TRUE; }

                    LOGWARNING("Could not fit the Z-buffer in video memory for this hardware device.\n");
                }
                else { LOGWARNING("Failed to get surface description of Z buffer %d.\n", result); }
            }
            else { LOGWARNING("AddAttachedBuffer failed for Z-Buffer %d.\n", result); }
        }

        State.DX.Device->SetRenderState(D3DRENDERSTATE_ZENABLE, FALSE);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);

        if (State.DX.Active.Surfaces.Active.Depth != NULL)
        {
            State.DX.Active.Surfaces.Active.Depth->Release();
            State.DX.Active.Surfaces.Active.Depth = NULL;
        }

        if (State.DX.Active.Surfaces.Depth != NULL)
        {
            State.DX.Active.Surfaces.Depth->Release();
            State.DX.Active.Surfaces.Depth = NULL;
        }

        return FALSE;
    }

    // 0x60006a10
    HRESULT CALLBACK EnumerateRendererDevicePixelFormats(LPDDPIXELFORMAT format, LPVOID context)
    {
        if ((format->dwFlags & DDPF_ZBUFFER) && format->dwRGBBitCount == State.Window.Bits)
        {
            CopyMemory(context, format, sizeof(DDPIXELFORMAT));

            return D3DENUMRET_CANCEL;
        }

        return D3DENUMRET_OK;
    }

    // 0x6000a020
    void AcquireRendererDeviceTextureFormats(void)
    {
        State.Textures.Formats.Count = 0;

        s32 count = INVALID_TEXTURE_FORMAT_COUNT;
        State.DX.Device->EnumTextureFormats(EnumerateRendererDeviceTextureFormats, &count);

        State.Textures.Formats.Indexes[0] = INVALID_TEXTURE_FORMAT_INDEX;
        State.Textures.Formats.Indexes[1] = AcquireRendererDeviceTextureFormatIndex(4, 0, 0, 0, 0, FALSE);
        State.Textures.Formats.Indexes[2] = AcquireRendererDeviceTextureFormatIndex(8, 0, 0, 0, 0, FALSE);
        State.Textures.Formats.Indexes[3] = AcquireRendererDeviceTextureFormatIndex(0, 1, 5, 5, 5, FALSE);
        State.Textures.Formats.Indexes[4] = AcquireRendererDeviceTextureFormatIndex(0, 0, 5, 6, 5, FALSE);
        State.Textures.Formats.Indexes[5] = AcquireRendererDeviceTextureFormatIndex(0, 0, 8, 8, 8, FALSE);
        State.Textures.Formats.Indexes[6] = AcquireRendererDeviceTextureFormatIndex(0, 8, 8, 8, 8, FALSE);
        State.Textures.Formats.Indexes[7] = AcquireRendererDeviceTextureFormatIndex(0, 4, 4, 4, 4, FALSE);
        State.Textures.Formats.Indexes[12] = AcquireRendererDeviceTextureFormatIndex(0, 0, 0, 0, 0, TRUE);

        RendererTextureFormatStates[0] = 0; // TODO
        RendererTextureFormatStates[1] = State.Textures.Formats.Indexes[1] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[2] = State.Textures.Formats.Indexes[2] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[4] = State.Textures.Formats.Indexes[4] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[5] = State.Textures.Formats.Indexes[5] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[6] = State.Textures.Formats.Indexes[6] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[7] = State.Textures.Formats.Indexes[7] != INVALID_TEXTURE_FORMAT_INDEX;
        RendererTextureFormatStates[12] = State.Textures.Formats.Indexes[12] != INVALID_TEXTURE_FORMAT_INDEX;

        if (State.Textures.Formats.Indexes[3] == INVALID_TEXTURE_FORMAT_INDEX)
        {
            State.Textures.Formats.Indexes[3] = AcquireRendererDeviceTextureFormatIndex(0, 0, 5, 5, 5, FALSE);
            RendererTextureFormatStates[3] = (State.Textures.Formats.Indexes[3] != INVALID_TEXTURE_FORMAT_INDEX) ? 5 : 0; // TODO
        }
        else
        {
            RendererTextureFormatStates[3] = 1; // TODO
        }
    }

    // 0x6000a260
    HRESULT CALLBACK EnumerateRendererDeviceTextureFormats(LPDDPIXELFORMAT format, LPVOID context)
    {
        ZeroMemory(&State.Textures.Formats.Formats[State.Textures.Formats.Count], sizeof(TextureFormat));

        CopyMemory(&State.Textures.Formats.Formats[State.Textures.Formats.Count].Descriptor.ddpfPixelFormat, format, sizeof(DDPIXELFORMAT));

        State.Textures.Formats.Formats[State.Textures.Formats.Count].IsDXT = FALSE;

        if (format->dwFlags & DDPF_FOURCC)
        {
            if (format->dwFourCC == FOURCC_DXT1)
            {
                State.Textures.Formats.Formats[State.Textures.Formats.Count].IsDXT = TRUE;
                State.Textures.Formats.Formats[State.Textures.Formats.Count].DXT = FOURCC_DXT1;
            }
            else if (format->dwFourCC == FOURCC_DXT2)
            {
                State.Textures.Formats.Formats[State.Textures.Formats.Count].IsDXT = TRUE;
                State.Textures.Formats.Formats[State.Textures.Formats.Count].DXT = FOURCC_DXT2;
            }
            else if (format->dwFourCC == FOURCC_DXT3)
            {
                State.Textures.Formats.Formats[State.Textures.Formats.Count].IsDXT = TRUE;
                State.Textures.Formats.Formats[State.Textures.Formats.Count].DXT = FOURCC_DXT3;
            }
        }

        if (format->dwFlags & DDPF_PALETTEINDEXED8)
        {
            State.Textures.Formats.Formats[State.Textures.Formats.Count].IsPalette = TRUE;
            State.Textures.Formats.Formats[State.Textures.Formats.Count].PaletteColorBits = 8;
        }
        else
        {
            if (format->dwFlags & DDPF_PALETTEINDEXED4)
            {
                State.Textures.Formats.Formats[State.Textures.Formats.Count].IsPalette = TRUE;
                State.Textures.Formats.Formats[State.Textures.Formats.Count].PaletteColorBits = 4;
            }
            else
            {
                if (format->dwFlags & DDPF_RGB)
                {
                    State.Textures.Formats.Formats[State.Textures.Formats.Count].IsPalette = FALSE;
                    State.Textures.Formats.Formats[State.Textures.Formats.Count].PaletteColorBits = 0;

                    u32 red = 0;

                    {
                        u32 value = format->dwRBitMask;

                        while ((value & 1) == 0) { value = value >> 1; }

                        while ((value & 1) != 0) { red = red + 1; value = value >> 1; }
                    }

                    u32 green = 0;

                    {
                        u32 value = format->dwGBitMask;

                        while ((value & 1) == 0) { value = value >> 1; }

                        while ((value & 1) != 0) { green = green + 1; value = value >> 1; }
                    }

                    u32 blue = 0;

                    {
                        u32 value = format->dwBBitMask;

                        while ((value & 1) == 0) { value = value >> 1; }

                        while ((value & 1) != 0) { blue = blue + 1; value = value >> 1; }
                    }

                    State.Textures.Formats.Formats[State.Textures.Formats.Count].RedBitCount = red;
                    State.Textures.Formats.Formats[State.Textures.Formats.Count].GreenBitCount = green;
                    State.Textures.Formats.Formats[State.Textures.Formats.Count].BlueBitCount = blue;

                    if (format->dwFlags & DDPF_ALPHAPIXELS)
                    {
                        u32 alpha = 0;

                        {
                            u32 value = format->dwRGBAlphaBitMask;

                            while ((value & 1) == 0) { value = value >> 1; }

                            while ((value & 1) != 0) { alpha = alpha + 1; value = value >> 1; }
                        }

                        State.Textures.Formats.Formats[State.Textures.Formats.Count].AlphaBitCount = alpha;
                    }
                }
            }
        }

        if (*(s32*)context == INVALID_TEXTURE_FORMAT_COUNT)
        {
            *(s32*)context = State.Textures.Formats.Count;
        }

        // NOTE: Added check to avoid writing outside the array boundaries.
        if (MAX_TEXTURE_FORMAT_COUNT <= State.Textures.Formats.Count + 1) { return DDENUMRET_CANCEL; }

        State.Textures.Formats.Count = State.Textures.Formats.Count + 1;

        return DDENUMRET_OK;
    }

    // 0x6000a1d0
    s32 AcquireRendererDeviceTextureFormatIndex(const u32 palette, const u32 alpha, const u32 red, const u32 green, const u32 blue, const BOOL dxt)
    {
        for (u32 x = 0; x < State.Textures.Formats.Count; x++)
        {
            const TextureFormat* format = &State.Textures.Formats.Formats[x];

            if (format->RedBitCount == red && format->GreenBitCount == green && format->BlueBitCount == blue)
            {
                if (format->PaletteColorBits == palette)
                {
                    if (format->AlphaBitCount == alpha && format->IsDXT == dxt) { return x; }
                    else if (format->PaletteColorBits == palette && palette != 0) { return x; }
                }
            }
            else if (format->PaletteColorBits == palette && palette != 0) { return x; }
        }

        return INVALID_TEXTURE_FORMAT_INDEX;
    }
    
    // 0x60006a50
    void InitializeRendererState(void)
    {
        State.DX.Device->BeginScene();

        State.DX.Device->SetCurrentViewport(State.DX.ViewPort);

        State.DX.Device->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, TRUE);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATER);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_ALPHAREF, 0);

        if (State.Device.Capabilities.IsDepthAvailable)
        {
            State.DX.Device->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_TRUE);
            State.DX.Device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, TRUE);
            State.DX.Device->SetRenderState(D3DRENDERSTATE_ZFUNC, D3DCMP_LESSEQUAL);
        }
        else
        {
            State.DX.Device->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
            State.DX.Device->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE);
        }

        State.DX.Device->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE);

        State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_MAGFILTER, D3DTFN_LINEAR);
        State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_MINFILTER, D3DTFN_LINEAR);

        if (!State.Device.Capabilities.IsModulateBlending)
        {
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        }
        else
        {
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLOROP, D3DTOP_MODULATE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        }

        State.DX.Device->SetTextureStageState(RENDERER_TEXTURE_STAGE_0, D3DTSS_ADDRESS, D3DTADDRESS_CLAMP);

        State.DX.Device->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);

        {
            const char* value = getenv(RENDERER_MODULE_WIRE_FRAME_DX6_ENVIRONMENT_PROPERTY_NAME);

            BOOL found = FALSE;

            if (value != NULL) { if (atoi(value) != 0) { found = TRUE; } }

            State.DX.Device->SetRenderState(D3DRENDERSTATE_FILLMODE, found ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
        }

        State.DX.Device->SetRenderState(D3DRENDERSTATE_DITHERENABLE, FALSE); // ORIGINAL: TRUE
        State.DX.Device->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, FALSE);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_ANTIALIAS, D3DANTIALIAS_NONE);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_FOGENABLE, FALSE);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_FOGCOLOR, 0xff0000);
        State.DX.Device->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE, D3DFOG_NONE);

        {
            const f32 value = 0.0f;
            State.DX.Device->SetRenderState(D3DRENDERSTATE_FOGSTART, *(DWORD*)(&value));
        }

        {
            const f32 value = 1.0f; // ORIGINAL: 1.4013e-45f
            State.DX.Device->SetRenderState(D3DRENDERSTATE_FOGEND, *(DWORD*)(&value));
        }

        State.DX.Device->EndScene();

        InitializeRendererTransforms();
    }

    // 0x60012af0
    void InitializeRendererTransforms(void)
    {
        SelectRendererTransforms(1.0f, 65535.0f);
    }

    // 0x60003c10
    u32 SelectRendererTransforms(const f32 zNear, const f32 zFar)
    {
        if (zFar <= zNear) { return TRUE; }

        D3DMATRIX world;

        world._11 = 1.0f;
        world._12 = 0.0f;
        world._13 = 0.0f;
        world._14 = 0.0f;

        world._21 = 0.0f;
        world._22 = 1.0f;
        world._23 = 0.0f;
        world._24 = 0.0f;

        world._31 = 0.0f;
        world._32 = 0.0f;
        world._33 = 1.0f;
        world._34 = 0.0f;

        world._41 = 0.0f;
        world._42 = 0.0f;
        world._43 = 0.0f;
        world._44 = 1.0f;

        D3DMATRIX view;

        view._11 = 1.0f;
        view._12 = 0.0f;
        view._13 = 0.0f;
        view._14 = 0.0f;

        view._21 = 0.0f;
        view._22 = 1.0f;
        view._23 = 0.0f;
        view._24 = 0.0f;

        view._31 = 0.0f;
        view._32 = 0.0f;
        view._33 = 1.0f;
        view._34 = 0.0f;

        view._41 = 0.0f;
        view._42 = 0.0f;
        view._43 = 0.0f;
        view._44 = 1.0f;

        D3DMATRIX projection;

        projection._11 = 1.0f;
        projection._12 = 0.0f;
        projection._13 = 0.0f;
        projection._14 = 0.0f;

        projection._21 = 0.0f;
        projection._22 = 1.0f;
        projection._23 = 0.0f;
        projection._24 = 0.0f;

        projection._31 = 0.0f;
        projection._32 = 0.0f;
        projection._33 = 1.0f;
        projection._34 = 0.0f;

        projection._41 = 0.0f;
        projection._42 = 0.0f;
        projection._43 = 0.0f;
        projection._44 = 1.0f;

        HRESULT result = State.DX.Device->SetTransform(D3DTRANSFORMSTATE_WORLD, &world);

        if (result == DD_OK)
        {
            result = State.DX.Device->SetTransform(D3DTRANSFORMSTATE_VIEW, &view);

            if (result == DD_OK)
            {
                projection._44 = zNear;
                projection._34 = 1.0f;
                projection._33 = zNear / (zFar - zNear) + 1.0f;
                projection._43 = 0.0f;

                result = State.DX.Device->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &projection);
            }
        }

        return result;
    }

    // 0x60001300
    void InitializeViewPort(void)
    {
        State.ViewPort.X0 = 0;
        State.ViewPort.Y0 = 0;
        State.ViewPort.X1 = 0;
        State.ViewPort.Y1 = 0;
    }

    // 0x60007290
    void InitializeRendererModuleState(const u32 mode, const u32 pending, const u32 depth, const char* section)
    {
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE, NULL);
        SelectState(RENDERER_MODULE_STATE_SELECT_CULL_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_CULL_COUNTER_CLOCK_WISE, section, "CULL"));
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_FILTER_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_TEXTURE_FILTER_LENEAR, section, "FILTER"));
        SelectState(RENDERER_MODULE_STATE_SELECT_SHADE_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_SHADE_GOURAUD, section, "SHADE"));
        SelectState(RENDERER_MODULE_STATE_SELECT_ALPHA_BLEND_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_ALPHA_BLEND_ACTIVE, section, "TRANSPARENCY"));
        SelectState(RENDERER_MODULE_STATE_SELECT_ALPHA_TEST_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_ALPHA_TEST_16, section, "ALPHATEST"));
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_MIP_FILTER_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_TEXTURE_MIP_FILTER_POINT, section, "MIPMAP"));
        SelectState(RENDERER_MODULE_STATE_SELECT_MATERIAL,
            (void*)AcquireSettingsValue(0x00000000, section, "BACKGROUNDCOLOUR"));
        SelectState(RENDERER_MODULE_STATE_SELECT_CHROMATIC_COLOR,
            (void*)AcquireSettingsValue(0x00000000, section, "CHROMACOLOUR"));
        SelectState(RENDERER_MODULE_STATE_SELECT_DITHER_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_DITHER_INACTIVE, section, "DITHER")); // ORIGINAL: ACTIVE
        SelectState(RENDERER_MODULE_STATE_SELECT_FOG_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_FOG_INACTIVE, section, "FOGMODE"));

        {
            const f32 value = 0.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_FOG_DENSITY,
                (void*)AcquireSettingsValue((s32)(*(s32*)&value), section, "FOGDENSITY"));
        }

        SelectState(RENDERER_MODULE_STATE_SELECT_FOG_COLOR,
            (void*)AcquireSettingsValue(GRAPCHICS_COLOR_WHITE, section, "FOGCOLOUR"));
        SelectState(RENDERER_MODULE_STATE_SELECT_BLEND_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_ALPHA_BLEND_SOURCE_INVERSE_SOURCE, section, "ALPHA"));
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_ADDRESS_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_TEXTURE_ADDRESS_CLAMP, section, "TEXTURECLAMP"));

        {
            const f32 value = 1.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_GAMMA_CONTROL_STATE, (void*)(u32)(*(u32*)&value));
        }

        {
            const f32 value = 0.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_DEPTH_BIAS_STATE,
                (void*)AcquireSettingsValue((u32)(*(u32*)&value), section, "DEPTHBIAS"));
        }
        
        {
            const f32 value = 0.0f;
            SelectState(RENDERER_MODULE_STATE_SELECT_MIP_MAP_LOD_BIAS_STATE, (void*)(u32)(*(u32*)&value));
        }

        SelectState(RENDERER_MODULE_STATE_SELECT_DEPTH_STATE, (void*)(((depth < 1) - 1) & 2)); // TODO
        SelectState(RENDERER_MODULE_STATE_MAX_PENDING_STATE,
            (void*)AcquireSettingsValue(pending - 2U & ((s32)(pending - 2U) < 0) - 1, section, "MAXPENDING")); // TODO
        SelectState(RENDERER_MODULE_STATE_SELECT_VERTEX_TYPE, (void*)RENDERER_MODULE_VERTEX_TYPE_RTLVX);
        SelectState(RENDERER_MODULE_STATE_SELECT_TEXTURE_STAGE_BLEND_STATE, (void*)RENDERER_MODULE_TEXTURE_STAGE_BLEND_NORMAL);
        SelectState(MAKETEXTURESTAGEMASK(RENDERER_TEXTURE_STAGE_1) | RENDERER_MODULE_STATE_SELECT_WINDOW_LOCK_STATE, (void*)NULL);
        SelectState(RENDERER_MODULE_STATE_SELECT_BACK_BUFFER_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_BACK_BUFFER_ACTIVE, section, "BACKBUFFERTYPE"));
        SelectState(RENDERER_MODULE_STATE_SELECT_FLAT_FANS_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_STATE_FLAT_FANS_ACTIVE, section, "FLATFANS"));
        SelectState(RENDERER_MODULE_STATE_SELECT_LINE_WIDTH,
            (void*)AcquireSettingsValue(1, section, "LINEWIDTH"));
        SelectState(RENDERER_MODULE_STATE_SELECT_PALETTE,
            (void*)AcquireSettingsValue((u32)(u32*)(void*)RendererPaletteValues, section, "DEFAULTPALETTE"));
        SelectState(RENDERER_MODULE_STATE_SELECT_STENCIL_STATE,
            (void*)AcquireSettingsValue(RENDERER_MODULE_STENCIL_INACTIVE, section, "STENCILBUFFER"));
        SelectState(RENDERER_MODULE_STATE_SELECT_DISPLAY_STATE,
            (void*)AcquireSettingsValue(mode, section, "DISPLAYMODE"));
    }

    // 0x600059a0
    RendererTexture* InitializeRendererTexture(void)
    {
        RendererTexture* result = (RendererTexture*)AllocateRendererTexture(sizeof(RendererTexture));

        if (result == NULL) { LOGERROR("D3D texture allocation ran out of memory\n"); }

        return result;
    }

    // 0x60007050
    RendererTexture* AllocateRendererTexture(const u32 size)
    {
        if (State.Lambdas.AllocateMemory != NULL) { return (RendererTexture*)State.Lambdas.AllocateMemory(size); }

        return (RendererTexture*)malloc(size);
    }

    // 0x600059e0
    void ReleaseRendererTexture(RendererTexture* tex)
    {
        if (tex != NULL) { DisposeRendererTexture(tex); }
    }

    // 0x60007080
    u32 DisposeRendererTexture(RendererTexture* tex)
    {
        if (State.Lambdas.ReleaseMemory != NULL) { return State.Lambdas.ReleaseMemory(tex); }

        free(tex);

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x6000a400
    s32 InitializeRendererTextureDetails(RendererTexture* tex) // TODO returns -1, 0, 1, where 1 is success, -1 is total failure and no further allocations allowed
    {
        if (tex->Surface2 != NULL)
        {
            tex->Surface2->Release();
            tex->Surface2 = NULL;
        }

        if (tex->Texture2 != NULL)
        {
            tex->Texture2->Release();
            tex->Texture2 = NULL;
        }

        DDSURFACEDESC2 desc;
        CopyMemory(&desc, &State.Textures.Formats.Formats[tex->FormatIndex].Descriptor, sizeof(DDSURFACEDESC2));

        desc.dwSize = sizeof(DDSURFACEDESC2);

        // Attempt to create texture in RAM.
        {
            if (tex->MipMapCount == 0)
            {
                desc.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
                desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY;
            }
            else
            {
                desc.dwFlags = DDSD_MIPMAPCOUNT | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
                desc.ddsCaps.dwCaps = DDSCAPS_MIPMAP | DDSCAPS_TEXTURE | DDSCAPS_SYSTEMMEMORY | DDSCAPS_COMPLEX;
                desc.dwMipMapCount = tex->MipMapCount;
            }

            if (RendererTextureHint == RENDERER_MODULE_TEXTURE_HINT_DYNAMIC) { desc.ddsCaps.dwCaps2 = DDSCAPS2_HINTDYNAMIC; }
            else if (RendererTextureHint == RENDERER_MODULE_TEXTURE_HINT_STATIC) { desc.ddsCaps.dwCaps2 = DDSCAPS2_HINTSTATIC; }

            desc.dwHeight = tex->Height;
            desc.dwWidth = tex->Width;

            if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1)
            {
                ZeroMemory(&desc.ddpfPixelFormat, sizeof(DDPIXELFORMAT));

                desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
                desc.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
                desc.ddpfPixelFormat.dwFourCC = FOURCC_DXT1;
            }

            IDirectDrawSurface4* surf = NULL;

            {
                const HRESULT result = State.DX.Active.Instance->CreateSurface(&desc, &surf, NULL);

                if (result != DD_OK) { return (result != DDERR_INVALIDPIXELFORMAT) - 1; } // TODO
            }

            IDirectDrawSurface4* surface = NULL;
            if (surf->QueryInterface(IID_IDirectDrawSurface4, (void**)&surface) != DD_OK)
            {
                if (surf != NULL) { surf->Release(); }

                return 0; // TODO
            }

            surf->Release();

            IDirect3DTexture2* texture = NULL;
            if (surface->QueryInterface(IID_IDirect3DTexture2, (void**)&texture) != DD_OK)
            {
                if (surface != NULL) { surface->Release(); }

                return 0; // TODO
            }

            tex->Surface1 = surface;
            tex->Texture1 = texture;

            ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

            desc.dwSize = sizeof(DDSURFACEDESC2);

            if (surface->GetSurfaceDesc(&desc) != DD_OK)
            {
                if (surface != NULL) { surface->Release(); }
                if (texture != NULL) { texture->Release(); }

                return 0; // TODO
            }
        }

        CopyMemory(&tex->Descriptor, &desc, sizeof(DDSURFACEDESC2));

        // Attempt to create texture in Video Memory or RAM, depending on the device capabilities.
        {
            if (tex->MipMapCount == 0)
            {
                desc.dwFlags = DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
                desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
            }
            else
            {
                desc.dwFlags = DDSD_MIPMAPCOUNT | DDSD_PIXELFORMAT | DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
                desc.ddsCaps.dwCaps = DDSCAPS_MIPMAP | DDSCAPS_TEXTURE | DDSCAPS_COMPLEX;
                desc.dwMipMapCount = tex->MipMapCount;
            }

            if (RendererTextureHint == RENDERER_MODULE_TEXTURE_HINT_DYNAMIC) { desc.ddsCaps.dwCaps2 = DDSCAPS2_HINTDYNAMIC; }
            else if (RendererTextureHint == RENDERER_MODULE_TEXTURE_HINT_STATIC) { desc.ddsCaps.dwCaps2 = DDSCAPS2_HINTSTATIC; }

            desc.dwTextureStage = tex->Stage;

            if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1)
            {
                ZeroMemory(&desc.ddpfPixelFormat, sizeof(DDPIXELFORMAT));

                desc.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
                desc.ddpfPixelFormat.dwFlags = DDPF_FOURCC;
                desc.ddpfPixelFormat.dwFourCC = FOURCC_DXT1;
            }

            desc.ddsCaps.dwCaps = State.Device.Capabilities.IsAccelerated
                ? desc.ddsCaps.dwCaps | DDSCAPS_VIDEOMEMORY
                : desc.ddsCaps.dwCaps | DDSCAPS_SYSTEMMEMORY;

            IDirectDrawSurface4* surf = NULL;

            {
                const HRESULT result = State.DX.Active.Instance->CreateSurface(&desc, &surf, NULL);

                if (result != DD_OK)
                {
                    if (tex->Surface1 != NULL) { tex->Surface1->Release(); }
                    if (tex->Texture1 != NULL) { tex->Texture1->Release(); }

                    return (result != DDERR_INVALIDPIXELFORMAT) - 1; // TODO
                }
            }

            IDirectDrawSurface4* surface = NULL;

            if (surf->QueryInterface(IID_IDirectDrawSurface4, (void**)&surface) != DD_OK)
            {
                if (surf != NULL) { surf->Release(); }

                if (tex->Surface1 != NULL) { tex->Surface1->Release(); }
                if (tex->Texture1 != NULL) { tex->Texture1->Release(); }

                return 0; // TODO
            }

            surf->Release();

            IDirectDrawPalette* palette = NULL;

            if (desc.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8)
            {
                tex->Colors = 256;

                PALETTEENTRY entries[MAX_TEXTURE_PALETTE_COLOR_COUNT];
                
                for (u32 x = 0; x < MAX_TEXTURE_PALETTE_COLOR_COUNT; x++)
                {
                    entries[x].peRed = 0;
                    entries[x].peGreen = 0xff;
                    entries[x].peBlue = 0;
                    entries[x].peFlags = 0;
                }

                if (State.DX.Active.Instance->CreatePalette(DDPCAPS_ALLOW256 | DDPCAPS_8BIT, entries, &palette, NULL) != DD_OK)
                {
                    if (surface != NULL) { surface->Release(); }

                    if (tex->Surface1 != NULL) { tex->Surface1->Release(); }
                    if (tex->Texture1 != NULL) { tex->Texture1->Release(); }

                    return 0; // TODO
                }

                if (surface->SetPalette(palette) != DD_OK)
                {
                    if (surface != NULL) { surface->Release(); }

                    if (tex->Surface1 != NULL) { tex->Surface1->Release(); }
                    if (tex->Texture1 != NULL) { tex->Texture1->Release(); }

                    if (palette != NULL) { palette->Release(); }

                    return 0; // TODO
                }

                tex->Palette = palette;
            }
            else if (desc.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4)
            {
                tex->Colors = 16;

                PALETTEENTRY entries[MAX_TEXTURE_PALETTE_COLOR_COUNT];

                for (u32 x = 0; x < MAX_TEXTURE_PALETTE_COLOR_COUNT; x++)
                {
                    entries[x].peRed = 0;
                    entries[x].peGreen = 0xff;
                    entries[x].peBlue = 0;
                    entries[x].peFlags = 0;
                }

                if (State.DX.Active.Instance->CreatePalette(DDPCAPS_4BIT, entries, &palette, NULL) != DD_OK)
                {
                    if (surface != NULL) { surface->Release(); }

                    if (tex->Surface1 != NULL) { tex->Surface1->Release(); }
                    if (tex->Texture1 != NULL) { tex->Texture1->Release(); }

                    return 0; // TODO
                }

                if (surface->SetPalette(palette) != DD_OK)
                {
                    if (surface != NULL) { surface->Release(); }

                    if (tex->Surface1 != NULL) { tex->Surface1->Release(); }
                    if (tex->Texture1 != NULL) { tex->Texture1->Release(); }

                    if (palette != NULL) { palette->Release(); }

                    return 0; // TODO
                }

                tex->Palette = palette;
            }
            else
            {
                tex->Colors = 0;
                tex->Palette = NULL;
            }

            IDirect3DTexture2* texture = NULL;

            if (surface->QueryInterface(IID_IDirect3DTexture2, (void**)&texture) != DD_OK)
            {
                if (texture != NULL) { texture->Release(); }

                if (tex->Surface1 != NULL) { tex->Surface1->Release(); }
                if (tex->Texture1 != NULL) { tex->Texture1->Release(); }

                if (palette != NULL) { palette->Release(); }

                return 0; // TODO
            }

            if (surface->Blt(NULL, tex->Surface1, NULL, DDBLT_WAIT, NULL) != DD_OK)
            {
                if (surface != NULL) { surface->Release(); }

                if (tex->Surface1 != NULL) { tex->Surface1->Release(); }
                if (tex->Texture1 != NULL) { tex->Texture1->Release(); }

                if (texture != NULL) { texture->Release(); }

                if (palette != NULL) { palette->Release(); }

                return 0; // TODO
            }

            ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

            desc.dwSize = sizeof(DDSURFACEDESC2);

            if (tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1) { desc.dwFlags = DDSD_LINEARSIZE; }

            if (surface->GetSurfaceDesc(&desc) != DD_OK)
            {
                if (surface != NULL) { surface->Release(); }

                if (tex->Surface1 != NULL) { tex->Surface1->Release(); }
                if (tex->Texture1 != NULL) { tex->Texture1->Release(); }

                if (texture != NULL) { texture->Release(); }

                if (palette != NULL) { palette->Release(); }
            }

            tex->MemoryType = desc.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY
                ? (desc.ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM ? RENDERER_MODULE_TEXTURE_LOCATION_NON_LOCAL_VIDEO_MEMORY : RENDERER_MODULE_TEXTURE_LOCATION_LOCAL_VIDEO_MEMORY)
                : (desc.ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY ? RENDERER_MODULE_TEXTURE_LOCATION_NON_LOCAL_VIDEO_MEMORY : RENDERER_MODULE_TEXTURE_LOCATION_SYSTEM_MEMORY);

            tex->Surface2 = surface;
            tex->Texture2 = texture;

            return 1; // TODO
        }

        return 0; // TODO
    }

    // 0x600092f0
    // NOTE: Looks like the engineers made an interesting decision to extend the API in an unconventional way, such that
    // a stage index is passed into the function, and not a valid pointer, to reset current texture.
    BOOL SelectRendererTexture(RendererTexture* tex)
    {
        if (!State.Scene.IsActive)
        {
            BeginRendererScene();

            State.Scene.IsActive = TRUE;
        }

        if (State.Data.Vertexes.Count != 0) { RendererRenderScene(); }

        if ((u32)tex < 16) // TODO
        {
            if ((u32)tex == 0) // TODO
            {
                State.DX.Device->SetTexture(RENDERER_TEXTURE_STAGE_0, NULL);

                return State.DX.Device->SetTexture(RENDERER_TEXTURE_STAGE_1, NULL) == DD_OK;
            }

            if ((u32)tex == 1) { return State.DX.Device->SetTexture(RENDERER_TEXTURE_STAGE_0, NULL) == DD_OK; } // TODO

            if ((u32)tex == 2) { return State.DX.Device->SetTexture(RENDERER_TEXTURE_STAGE_1, NULL) == DD_OK; } // TODO

            return TRUE;
        }

        return State.DX.Device->SetTexture(tex->Stage, tex->Texture2) == DD_OK;
    }

    // 0x60007d00
    void AttemptRenderScene(void)
    {
        if (State.Data.Vertexes.Count != 0) { RendererRenderScene(); }
    }

    // 0x6000a9c0
    BOOL UpdateRendererTexture(RendererTexture* tex, const u32* pixels, const u32* palette)
    {
        if (pixels != NULL)
        {
            tex->Descriptor.lpSurface = tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1
                ? (void*)(((addr)pixels) + (addr)8) : (void*)pixels; // TODO

            tex->Descriptor.dwFlags = DDSD_LPSURFACE;

            if (tex->Surface1->SetSurfaceDesc(&tex->Descriptor, 0) != DD_OK) { return FALSE; }

            if (State.Scene.IsActive) { AttemptRenderScene(); }

            tex->Surface2->Blt(NULL, tex->Surface1, NULL, DDBLT_WAIT, NULL);
        }

        if (palette != NULL && tex->Options != 0)
        {
            PALETTEENTRY entries[MAX_TEXTURE_PALETTE_COLOR_COUNT];

            for (u32 x = 0; x < MAX_TEXTURE_PALETTE_COLOR_COUNT; x++)
            {
                entries[x].peRed = (u8)RGBA_GETRED(palette[x]);
                entries[x].peGreen = (u8)RGBA_GETGREEN(palette[x]);
                entries[x].peBlue = (u8)RGBA_GETBLUE(palette[x]);
                entries[x].peFlags = 0;
            }

            if (tex->Palette->SetEntries(0, 0, tex->Colors, entries) != DD_OK) { return FALSE; }

            if (tex->Texture2->PaletteChanged(0, tex->Colors) != DD_OK) { return FALSE; }
        }

        if (tex->MipMapCount != 0) { UpdateRendererTexture(tex, pixels); };

        return TRUE;
    }

    // 0x6000ab10
    BOOL UpdateRendererTexture(RendererTexture* tex, const u32* pixels)
    {
        if (tex->MipMapCount == 0 || tex->MipMapCount == 1) { return TRUE; }

        IDirectDrawSurface4* s1 = tex->Surface1;
        IDirectDrawSurface4* s2 = tex->Surface2;

        s1->AddRef();
        s2->AddRef();

        DDSCAPS2 caps;
        ZeroMemory(&caps, sizeof(DDSCAPS2));

        DDSURFACEDESC2 desc;
        ZeroMemory(&desc, sizeof(DDSURFACEDESC2));

        desc.dwSize = sizeof(DDSURFACEDESC2);

        u32 offset = 0;

        void* allocated = _alloca((desc.lPitch * desc.dwHeight + 3) & 0xfffffffc);
        memset(allocated, 0xff, (desc.lPitch * desc.dwHeight + 3) & 0xfffffffc);

        u32* data = NULL;

        for (u32 x = 0; x < tex->MipMapCount; x++)
        {
            if (pixels != NULL)
            {
                s1->GetSurfaceDesc(&desc);

                offset = offset + desc.lPitch * desc.dwHeight;

                caps.dwCaps = DDSCAPS_MIPMAP | DDSCAPS_TEXTURE;

                {
                    IDirectDrawSurface4* attached = NULL;
                    s1->GetAttachedSurface(&caps, &attached);
                    s1->Release();

                    s1 = attached;
                }

                {
                    IDirectDrawSurface4* attached = NULL;
                    s2->GetAttachedSurface(&caps, &attached);
                    s2->Release();

                    s2 = attached;
                }

                s1->GetSurfaceDesc(&tex->Descriptor);

                const u32 pitch = (tex->Descriptor.ddpfPixelFormat.dwRGBBitCount * ((tex->Descriptor.dwWidth + 7) >> 3));

                if (pitch == tex->Descriptor.lPitch)
                {
                    tex->Descriptor.lpSurface = tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1
                        ? (void*)(((addr)pixels) + (addr)(offset + 8)) // TOOD
                        : (void*)(((addr)pixels) + (addr)offset);
                }
                else
                {
                    if (data == NULL)
                    {
                        data = tex->FormatIndexValue == RENDERER_PIXEL_FORMAT_DXT1
                            ? (u32*)(((addr)pixels) + (addr)(offset + 8)) // TOOD
                            : (u32*)(((addr)pixels) + (addr)offset);
                    }

                    tex->Descriptor.lpSurface = allocated;

                    for (u32 xx = 0; xx < tex->Descriptor.dwHeight; xx++)
                    {
                        CopyMemory(allocated, &data[xx * pitch], pitch);
                    }
                }

                s1->SetSurfaceDesc(&tex->Descriptor, 0);
                s2->Blt(NULL, s1, NULL, DDBLT_WAIT, NULL);
            }

            s1->Release();
            s2->Release();
        }

        return TRUE;
    }

    // 0x60009410
    BOOL SelectRendererState(const D3DRENDERSTATETYPE type, const DWORD value)
    {
        if (!State.Scene.IsActive)
        {
            BeginRendererScene();

            State.Scene.IsActive = TRUE;
        }

        if (State.Data.Vertexes.Count != 0) { RendererRenderScene(); }

        return State.DX.Device->SetRenderState(type, value) == DD_OK;
    }

    // 0x600093c0
    BOOL SelectRendererTextureStage(const DWORD stage, const D3DTEXTURESTAGESTATETYPE type, const DWORD value)
    {
        if (!State.Scene.IsActive)
        {
            BeginRendererScene();

            State.Scene.IsActive = TRUE;
        }

        if (State.Data.Vertexes.Count != 0) { RendererRenderScene(); }

        return State.DX.Device->SetTextureStageState(stage, type, value) == DD_OK;
    }

    // 0x60009630
    void SelectRendererMaterial(const f32 r, const f32 g, const f32 b)
    {
        D3DMATERIAL material;
        ZeroMemory(&material, sizeof(D3DMATERIAL));

        material.dwSize = sizeof(D3DMATERIAL);

        material.diffuse.r = r;
        material.diffuse.g = g;
        material.diffuse.b = b;

        material.ambient.r = r;
        material.ambient.g = g;
        material.ambient.b = b;

        material.dwRampSize = 1;

        State.DX.Material->SetMaterial(&material);
    }

    // 0x60005db0
    void SelectRendererDeviceType(const u32 type)
    {
        RendererDeviceType = type;
    }

    // 0x600057b0
    void SelectRendererFogAlphas(const u8* input, u8* output)
    {
        if (input == NULL) { return; }

        for (u32 x = 0; x < MAX_OUTPUT_FOG_ALPHA_COUNT; x++)
        {
            const f32 value = roundf(x / 255.0f) * 63.0f;
            const u32 indx = (u32)value;

            const f32 diff = value - indx;

            if (0.0f < diff)
            {
                const u8 result = (u8)roundf(input[indx] + (input[indx + 1] - input[indx]) * diff);
                output[x] = (u8)(MAX_OUTPUT_FOG_ALPHA_VALUE - result);
            }
            else
            {

                output[x] = (u8)(MAX_OUTPUT_FOG_ALPHA_VALUE - input[indx]);
            }
        }
    }

    // 0x600070b0
    u32 SelectBasicRendererState(const u32 state, void* value)
    {
        switch (state)
        {
        case RENDERER_MODULE_STATE_SELECT_CULL_STATE:
        {
            switch ((u32)value)
            {
            case RENDERER_MODULE_CULL_NONE: { State.Settings.Cull = 1; break; } // TODO
            case RENDERER_MODULE_CULL_COUNTER_CLOCK_WISE: { State.Settings.Cull = 0x80000000; break; }  // TODO
            case RENDERER_MODULE_CULL_CLOCK_WISE: { State.Settings.Cull = 0; break; } // TODO
            default: { return RENDERER_MODULE_FAILURE; }
            }

            break;
        }
        case RENDERER_MODULE_STATE_SELECT_WINDOW_MODE_STATE:
        case RENDERER_MODULE_STATE_SELECT_WINDOW:
        case RENDERER_MODULE_STATE_SELECT_EXECUTE_LAMBDA:
        case RENDERER_MODULE_STATE_SELECT_LOCK_WINDOW_LAMBDA: { break; }
        case RENDERER_MODULE_STATE_SELECT_LAMBDAS:
        {
            const RendererModuleLambdaContainer* lambdas = (RendererModuleLambdaContainer*)value;

            State.Lambdas.Log = lambdas == NULL ? NULL : lambdas->Log;

            break;
        }
        case RENDERER_MODULE_STATE_SELECT_LOG_LAMBDA: { State.Lambdas.Log = (RENDERERMODULELOGLAMBDA)value; break; }
        case RENDERER_MODULE_STATE_SELECT_LINE_VERTEX_SIZE: { RendererLineVertexSize = (u32)value; break; }
        case RENDERER_MODULE_STATE_SELECT_VERSION:
        {
            const u32 version = (u32)value;

            RendererVersion = version;

            if (version < RENDERER_MODULE_VERSION_104 || version < RENDERER_MODULE_VERSION_106)
            {
                RendererVersion = RENDERER_MODULE_VERSION_106;
            }

            break;
        }
        case RENDERER_MODULE_STATE_SELECT_MEMORY_ALLOCATE_LAMBDA: { State.Lambdas.AllocateMemory = (RENDERERMODULEALLOCATEMEMORYLAMBDA)value; break; }
        case RENDERER_MODULE_STATE_SELECT_MEMORY_RELEASE_LAMBDA: { State.Lambdas.ReleaseMemory = (RENDERERMODULERELEASEMEMORYLAMBDA)value; break; }
        case RENDERER_MODULE_STATE_SELECT_SELECT_STATE_LAMBDA: { State.Lambdas.SelectState = (RENDERERMODULESELECTSTATELAMBDA)value; break; }
        case RENDERER_MODULE_STATE_55: { DAT_6001d5b0 = (u32)value; break; }
        default: { return RENDERER_MODULE_FAILURE; }
        }

        if (State.Lambdas.SelectState != NULL) { State.Lambdas.SelectState(state, value); }

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x600095e0
    BOOL RestoreRendererSurfaces(void)
    {
        const HRESULT mr = State.DX.Active.Surfaces.Active.Main->Restore();
        const HRESULT dr = State.DX.Active.Surfaces.Active.Depth->Restore();

        return RestoreRendererTextures() && mr == DD_OK && dr == DD_OK;
    }

    // 0x60005ad0
    BOOL RestoreRendererTextures(void)
    {
        RendererTexture* tex = State.Textures.Current;

        while (tex != NULL)
        {
            if (tex->Surface2 != NULL) { tex->Surface2->Restore(); }

            tex = tex->Previous;
        }

        return TRUE;
    }

    // 0x60007d90
    void SelectRendererVertexCount(void)
    {
        AttemptRenderScene();

        MaximumRendererVertexCount = MAX_VERTEX_COUNT / RendererVertexSize;
    }

    // 0x60009730
    u32 InitializeRendererVertexBuffer(const u32 type, const u32 count)
    {
        if (State.Data.Vertexes.Buffer != NULL)
        {
            RendererRenderScene();

            State.Data.Vertexes.Buffer->Unlock();
            State.Data.Vertexes.Buffer->Release();

            State.Data.Vertexes.Buffer = NULL;
        }

        D3DVERTEXBUFFERDESC desc;

        desc.dwSize = sizeof(D3DVERTEXBUFFERDESC);
        desc.dwCaps = D3DVBCAPS_WRITEONLY;
        desc.dwFVF = type;
        desc.dwNumVertices = count;

        const HRESULT result = State.DX.DirectX->CreateVertexBuffer(&desc, &State.Data.Vertexes.Buffer, D3DDP_DONOTCLIP, NULL);

        State.Data.Vertexes.Buffer->Lock(DDLOCK_NOSYSLOCK | DDLOCK_WRITEONLY | DDLOCK_WAIT, (void**)&State.Data.Vertexes.Vertexes, NULL);

        return result;
    }

    // 0x60008f90
    BOOL RenderLines(RVX* vertexes, const u32 count)
    {
        if (!State.Scene.IsActive)
        {
            BeginRendererScene();

            State.Scene.IsActive = TRUE;
        }

        for (u32 x = 0; x < count; x++)
        {
            RTLVX* vertex = (RTLVX*)((addr)vertexes + (addr)(x * RendererVertexSize));

            if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

            if (State.Settings.IsFogActive && DAT_6001d030 == 16) // TODO
            {
                vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
            }

            vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
        }

        return State.DX.Device->DrawPrimitive(D3DPT_LINESTRIP, RendererVertexType,
            vertexes, count, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP) == DD_OK;
    }

    // 0x60009150
    BOOL RenderPoints(RVX* vertexes, const u32 count)
    {
        if (!State.Scene.IsActive)
        {
            BeginRendererScene();

            State.Scene.IsActive = TRUE;
        }

        for (u32 x = 0; x < count; x++)
        {
            RTLVX* vertex = (RTLVX*)((addr)vertexes + (addr)(RendererVertexSize * x));

            if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

            if (State.Settings.IsFogActive && DAT_6001d030 == 16) // TODO
            {
                vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
            }

            vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
        }

        return State.DX.Device->DrawPrimitive(D3DPT_POINTLIST, RendererVertexType,
            vertexes, count, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP) == DD_OK;
    }

    // 0x600087c0
    void RenderQuad(RVX* a, RVX* b, RVX* c, RVX* d)
    {
        if (MaximumRendererVertexCount - 4 < State.Data.Vertexes.Count) { RendererRenderScene(); }

        // A
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 0)));

            CopyMemory(v, a, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_6001d030 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        // B
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 1)));

            CopyMemory(v, b, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_6001d030 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        // C
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 2)));

            CopyMemory(v, c, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_6001d030 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        // D
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 3)));

            CopyMemory(v, d, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_6001d030 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 0] = State.Data.Vertexes.Count + 0;
        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 1] = State.Data.Vertexes.Count + 1;
        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 2] = State.Data.Vertexes.Count + 2;
        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 3] = State.Data.Vertexes.Count + 0;
        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 4] = State.Data.Vertexes.Count + 2;
        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 5] = State.Data.Vertexes.Count + 3;

        State.Data.Indexes.Count = State.Data.Indexes.Count + 6;
        State.Data.Vertexes.Count = State.Data.Vertexes.Count + 4;
    }

    // 0x600083a0
    void RenderQuadMesh(RVX* vertexes, const u32* indexes, const u32 count)
    {
        for (u32 x = 0; x < count; x++)
        {
            if (MaximumRendererVertexCount - 4 < State.Data.Vertexes.Count) { RendererRenderScene(); }

            RVX* a = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * indexes[x * 4 + 0]));
            RVX* b = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * indexes[x * 4 + 1]));
            RVX* c = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * indexes[x * 4 + 2]));
            RVX* d = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * indexes[x * 4 + 3]));

            if (((u32)AcquireNormal((f32x3*)a, (f32x3*)b, (f32x3*)c) & 0x80000000) != State.Settings.Cull) { RenderQuad(a, b, c, d); } // TODO
        }
    }

    // 0x60007dd0
    void RenderTriangle(RVX* a, RVX* b, RVX* c)
    {
        if (MaximumRendererVertexCount - 3 < State.Data.Vertexes.Count) { RendererRenderScene(); }

        // A
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 0)));

            CopyMemory(v, a, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_6001d030 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        // B
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 1)));

            CopyMemory(v, b, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_6001d030 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        // C
        {
            RVX* v = (RVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 2)));

            CopyMemory(v, c, RendererVertexSize);

            {
                RTLVX* vertex = (RTLVX*)v;

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_6001d030 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 0] = State.Data.Vertexes.Count + 0;
        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 1] = State.Data.Vertexes.Count + 1;
        State.Data.Indexes.Indexes[State.Data.Indexes.Count + 2] = State.Data.Vertexes.Count + 2;

        State.Data.Indexes.Count = State.Data.Indexes.Count + 3;
        State.Data.Vertexes.Count = State.Data.Vertexes.Count + 3;
    }

    // 0x60008d60
    BOOL RenderTriangleFans(RVX* vertexes, const u32 vertexCount, const u32 indexCount, const u32* indexes)
    {
        u32 max = 0;

        for (u32 x = 0; x < indexCount + 2; x++)
        {
            State.Data.Indexes.Large[x] = indexes[x];

            if (max < indexes[x]) { max = indexes[x]; }
        }

        if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT
            || (State.Settings.IsFogActive && DAT_6001d030 == 16) || RendererDepthBias != 0.0f) // TODO
        {
            for (u32 x = 0; x < vertexCount; x++)
            {
                RTLVX* vertex = (RTLVX*)((addr)State.Data.Vertexes.Vertexes + (addr)(RendererVertexSize * (State.Data.Vertexes.Count + 2)));

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_6001d030 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        if (!State.Scene.IsActive)
        {
            BeginRendererScene();

            State.Scene.IsActive = TRUE;
        }

        return State.DX.Device->DrawIndexedPrimitive(D3DPT_TRIANGLEFAN, RendererVertexType,
            vertexes, max + 1, State.Data.Indexes.Large, indexCount + 2, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP) == DD_OK;
    }

    // 0x60005960
    void RenderTriangleMesh(RVX* vertexes, const u32* indexes, const u32 count)
    {
        for (u32 x = 0; x < count; x++)
        {
            if (MaximumRendererVertexCount - 3 < State.Data.Vertexes.Count) { RendererRenderScene(); }

            RVX* a = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * indexes[x * 3 + 0]));
            RVX* b = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * indexes[x * 3 + 1]));
            RVX* c = (RVX*)((addr)vertexes + (addr)(RendererVertexSize * indexes[x * 3 + 2]));

            if (((u32)AcquireNormal((f32x3*)a, (f32x3*)b, (f32x3*)c) & 0x80000000) != State.Settings.Cull) { RenderTriangle(a, b, c); } // TODO
        }
    }

    // 0x60008b00
    BOOL RenderTriangleStrips(RVX* vertexes, const u32 vertexCount, const u32 indexCount, const u32* indexes)
    {
        u32 max = 0;

        for (u32 x = 0; x < indexCount + 2; x++)
        {
            State.Data.Indexes.Large[x] = indexes[x];

            if (max < indexes[x]) { max = indexes[x]; }
        }

        if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT
            || (State.Settings.IsFogActive && DAT_6001d030 == 16) || RendererDepthBias != 0.0f) // TODO
        {
            for (u32 x = 0; x < vertexCount; x++)
            {
                RTLVX* vertex = (RTLVX*)((addr)vertexes + (addr)(RendererVertexSize * x));

                if (RendererShadeMode == RENDERER_MODULE_SHADE_FLAT) { vertex->Color = GRAPCHICS_COLOR_WHITE; }

                if (State.Settings.IsFogActive && DAT_6001d030 == 16) // TODO
                {
                    vertex->Specular = ((u32)RendererFogAlphas[(u32)(vertex->XYZ.Z * 255.0f)]) << 24;
                }

                vertex->XYZ.Z = RendererDepthBias + vertex->XYZ.Z;
            }
        }

        if (!State.Scene.IsActive)
        {
            BeginRendererScene();

            State.Scene.IsActive = TRUE;
        }

        return State.DX.Device->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, RendererVertexType,
            vertexes, max + 1, State.Data.Indexes.Large, indexCount + 2, D3DDP_DONOTUPDATEEXTENTS | D3DDP_DONOTCLIP) == DD_OK;
    }

    // 0x60009910
    void InitializeRenderState55(void)
    {
        SelectBasicRendererState(RENDERER_MODULE_STATE_55, (void*)(DAT_6001d5b0 + 1));
    }
}