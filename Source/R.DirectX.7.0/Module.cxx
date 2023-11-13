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
#include "Module.hxx"
#include "Native.Basic.hxx"

#include <stdio.h>
#include <stdlib.h>

namespace RendererModule
{
    ModuleState State;

    FILE* file; // TODO DEBUG

    // 0x60001000
    // a.k.a. _THRASH_about
    DLLAPI RendererModuleDescriptor* STDCALLAPI AcquireDescriptor(void)
    {
        State.Descriptor.Version = Version;
        State.Descriptor.Signature = RENDERER_MODULE_SIGNATURE_D3D7;
        State.Descriptor.Unk01 = 0xd0; // TODO

        // TODO NOT IMPLEMENTED

        State.Descriptor.DirectVersion = 7;
        State.Descriptor.Author = RENDERER_MODULE_AUTHOR;

        // TODO NOT IMPLEMENTED
        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG

        return &State.Descriptor;
    }

    // 0x600012e0
    // a.k.a. _THRASH_clearwindow
    DLLAPI void STDCALLAPI ClearGameWindow(void)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x600013c0
    // a.k.a. _THRASH_clip
    DLLAPI u32 STDCALLAPI Clip(void*, void*, void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60001880
    // a.k.a. _THRASH_createwindow
    DLLAPI u32 STDCALLAPI CreateGameWindow(void*, void*, void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60001910
    // a.k.a. _THRASH_destroywindow
    DLLAPI u32 STDCALLAPI DestroyGameWindow(void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001b10
    // a.k.a. _THRASH_drawline
    DLLAPI void STDCALLAPI DrawLine(void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001b30
    // a.k.a. _THRASH_drawlinemesh
    DLLAPI void STDCALLAPI DrawLineMesh(void*, void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001d20
    // a.k.a. _THRASH_drawlinestrip
    DLLAPI void STDCALLAPI DrawLineStrip(const u32 count, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001b50
    // a.k.a. _THRASH_drawlinestrip
    DLLAPI void STDCALLAPI DrawLineStrips(void*, void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001b90
    // a.k.a. _THRASH_drawpoint
    DLLAPI void STDCALLAPI DrawPoint(void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001bb0
    // a.k.a. _THRASH_drawpointmesh
    DLLAPI void STDCALLAPI DrawPointMesh(void*, void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001d50
    // a.k.a. _THRASH_drawpointstrip
    DLLAPI void STDCALLAPI DrawPointStrip(void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001a90
    // a.k.a. _THRASH_drawquad
    DLLAPI void STDCALLAPI DrawQuad(void*, void*, void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001af0
    // a.k.a. _THRASH_drawquadmesh
    DLLAPI void STDCALLAPI DrawQuadMesh(void*, void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001bf0
    // a.k.a. _THRASH_drawsprite
    DLLAPI void STDCALLAPI DrawSprite(void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001c50
    // a.k.a. _THRASH_drawspritemesh
    DLLAPI void STDCALLAPI DrawSpriteMesh(void*, void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x600019d0
    // a.k.a. _THRASH_drawtri
    DLLAPI void STDCALLAPI DrawTriangle(void*, void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001cf0
    // a.k.a. _THRASH_drawtrifan
    DLLAPI void STDCALLAPI DrawTriangleFan(void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001a70
    // a.k.a. _THRASH_drawtrifan
    DLLAPI void STDCALLAPI DrawTriangleFans(void*, void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001a30
    // a.k.a. _THRASH_drawtrimesh
    DLLAPI void STDCALLAPI DrawTriangleMesh(void*, void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001ca0
    // a.k.a. _THRASH_drawtristrip
    DLLAPI void STDCALLAPI DrawTriangleStrip(void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001a50
    // a.k.a. _THRASH_drawtristrip
    DLLAPI void STDCALLAPI DrawTriangleStrips(void*, void*, void*)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
    }

    // 0x60001320
    // a.k.a. _THRASH_flushwindow
    DLLAPI u32 STDCALLAPI FlushGameWindow(void)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG

        return RENDERER_MODULE_SUCCESS;
    }

    // 0x60001d80
    // a.k.a. _THRASH_getstate
    DLLAPI u32 STDCALLAPI AcquireState(const u32 options)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG

        return 0; // TODO
    }

    // 0x60001960
    // a.k.a. _THRASH_getwindowtexture
    DLLAPI RendererTexture* STDCALLAPI AcquireGameWindowTexture(const u32 indx)
    {
        // if(indx < MAX_RENDERER_MODULE_TEXTURE_COUNT) { return ...

        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG

        return NULL;
    }

    // 0x60001370
    // a.k.a. _THRASH_idle
    DLLAPI void STDCALLAPI Idle(void) { }

    // 0x60003670
    // a.k.a. _THRASH_init
    DLLAPI u32 STDCALLAPI Init(void)
    {
        // TODO NOT IMPLEMENTED

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG

        return 0; // TODO
    }

    // 0x60003a50
    // a.k.a. _THRASH_is
    DLLAPI u32 STDCALLAPI Is(void)
    {
        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG

        const HWND hwnd = GetDesktopWindow();
        const HDC hdc = GetWindowDC(hwnd);

        if (GetDeviceCaps(hdc, BITSPIXEL) < GRAPHICS_BITS_PER_PIXEL_8)
        {
            ReleaseDC(hwnd, hdc);

            return RENDERER_MODULE_FAILURE;
        }

        ReleaseDC(hwnd, hdc);

        IDirectDraw7* instance;
        if (DirectDrawCreateEx(NULL, (void**)&instance, IID_IDirectDraw7, NULL) == DD_OK)
        {
            DDCAPS caps;
            ZeroMemory(&caps, sizeof(DDCAPS));
            
            caps.dwSize = sizeof(DDCAPS);

            if (instance->GetCaps(&caps, NULL) == DD_OK && (caps.dwCaps & DDCAPS_3D))
            {
                IDirect3D7* dx;
                if (instance->QueryInterface(IID_IDirect3D7, (void**)&dx) == DD_OK)
                {
                    instance->Release();

                    if (dx != NULL)
                    {
                        dx->Release();

                        return RENDERER_MODULE_ACCELERATION_PRESENT;
                    }

                    return RENDERER_MODULE_FAILURE;
                }
            }
        }

        if (instance != NULL) { instance->Release(); }

        return RENDERER_MODULE_FAILURE;
    }

    // 0x60001480
    // a.k.a. _THRASH_lockwindow
    DLLAPI void* STDCALLAPI LockGameWindow(void)
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG

        // TODO NOT IMPLEMENTED

        return NULL; // TODO
    }

    // 0x60001330
    // a.k.a. _THRASH_pageflip
    DLLAPI void STDCALLAPI ToggleGameWindow(void)
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG

        // TODO NOT IMPLEMENTED
    }

    // 0x60003b60
    // a.k.a. _THRASH_readrect
    DLLAPI u32 STDCALLAPI ReadRectangle(void*, void*, void*, void*, void*)
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
        // TODO NOT IMPLEMENTED

        return 0; // TODO
    }

    // 0x600036c0
    // a.k.a.  _THRASH_restore
    DLLAPI u32 STDCALLAPI RestoreGameWindow(void)
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
        // TODO NOT IMPLEMENTED

        return 0; // TODO
    }

    // 0x600020c0
    // a.k.a. _THRASH_selectdisplay
    DLLAPI u32 STDCALLAPI SelectDisplay(const u32 indx)
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
        // TODO NOT IMPLEMENTED

        return 0; // TODO
    }

    // 0x60003ca0
    // a.k.a. _THRASH_setstate
    DLLAPI void* STDCALLAPI SelectState(const u32 state, void* value)
    {
        // TODO

        s32 pvVar12 = 0; // TODO type, name

        u32 stage = RENDERER_TEXTURE_STAGE_0;

        switch (state & RENDERER_MODULE_STATE_STAGE_MASK)
        {
        case RENDERER_TEXTURE_STAGE_1: { stage = RENDERER_TEXTURE_STAGE_1; break; }
        case RENDERER_TEXTURE_STAGE_2: { stage = RENDERER_TEXTURE_STAGE_2; break; }
        case RENDERER_TEXTURE_STAGE_3: { stage = RENDERER_TEXTURE_STAGE_3; break; }
        case RENDERER_TEXTURE_STAGE_4: { stage = RENDERER_TEXTURE_STAGE_4; break; }
        case RENDERER_TEXTURE_STAGE_5: { stage = RENDERER_TEXTURE_STAGE_5; break; }
        case RENDERER_TEXTURE_STAGE_6: { stage = RENDERER_TEXTURE_STAGE_6; break; }
        case RENDERER_TEXTURE_STAGE_7: { stage = RENDERER_TEXTURE_STAGE_7; break; }
        }

        const u32 type = state & RENDERER_MODULE_STATE_TYPE_MASK;

        if (RENDERER_MODULE_STATE_MIN_VALUE < type)
        {
            switch (type)
            {
            case RENDERER_MODULE_STATE_0x191:
            {
                DAT_600170c4 = (s32)value; // TODO type

                pvVar12 = 1;

                break;
            }
            default:
            {
                if (FUN_6000b2c0(type, value) == 0) { return NULL; } break;

            }
            case RENDERER_MODULE_STATE_0x193:
            {
                if (value == NULL) { return RENDERER_MODULE_FAILURE; }

                // TODO: fill the struct

                return value;
            }
            case RENDERER_MODULE_STATE_SELECT_SOURCE_BLEND:
            {
                const u32 st = (u32)value; // TODO Enum: RENDRERER_BLEND_TYPE

                switch (st)
                {
                case 0: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE); break; }
                case 1: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ZERO); break; }
                case 2: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA); break; }
                case 3: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCALPHA); break; }
                case 4: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTALPHA); break; }
                case 5: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVDESTALPHA); break; }
                case 6: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCCOLOR); break; }
                case 7: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_DESTCOLOR); break; }
                case 8: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVSRCCOLOR); break; }
                case 9: { SelectRendererState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_INVDESTCOLOR); break; }
                }

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_DESTINATION_BLEND:
            {
                const u32 dt = (u32)value; // TODO Enum: RENDRERER_BLEND_TYPE

                switch (dt)
                {
                case 0: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE); break; }
                case 1: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO); break; }
                case 2: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCALPHA); break; }
                case 3: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA); break; }
                case 4: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_DESTALPHA); break; }
                case 5: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVDESTALPHA); break; }
                case 6: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR); break; }
                case 7: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_DESTCOLOR); break; }
                case 8: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCCOLOR); break; }
                case 9: { SelectRendererState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVDESTCOLOR); break; }
                }

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_0x197:
            {
                const u32 val = (u32)value; // TODO enum
                
                switch (val)
                {
                case 0: {DAT_600170f4 = 0; break; }
                case 1: {DAT_600170f4 = 1; break; }
                case 2: {DAT_600170f4 = 2; break; }
                }

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_0x198: { goto LAB_60005b7a;}
            case RENDERER_MODULE_STATE_0x199:
            {
                if (DAT_6005ab88 == 0) { return NULL; } // TODO

                if ((u32)value == 1) // TODO
                {
                    SelectRendererState(D3DRENDERSTATE_EDGEANTIALIAS, TRUE);
                    SelectRendererState(D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
                }
                else
                {
                    SelectRendererState(D3DRENDERSTATE_EDGEANTIALIAS, FALSE);
                    SelectRendererState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
                }

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_0x19a:
            {
                SelectRendererState(D3DRENDERSTATE_TEXTUREFACTOR, (f32)value);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_0x19c:
            {
                if (value == NULL) { return NULL; }

                *value = State.DX.Active.Surfaces.Unknown11;

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_0x19d:
            {
                if (value == NULL) { return NULL; }

                *value = State.DX.Active.Surfaces.Unknown12;

                pvVar12 = 1; break;

            }
            case RENDERER_MODULE_STATE_SELECT_BUMP_MAPPING_MATRIX:
            {
                const RendererTextureStageBumpMappingMatrix* mat = (RendererTextureStageBumpMappingMatrix*)value;

                SelectTextureStageState(stage, D3DTSS_BUMPENVMAT00, mat->M00);
                SelectTextureStageState(stage, D3DTSS_BUMPENVMAT01, mat->M01);
                SelectTextureStageState(stage, D3DTSS_BUMPENVMAT10, mat->M10);
                SelectTextureStageState(stage, D3DTSS_BUMPENVMAT11, mat->M11);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_BUMP_MAPPING_LUMINANCE_SCALE:
            {
                SelectTextureStageState(stage, D3DTSS_BUMPENVLSCALE, value);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_BUMP_MAPPING_LUMINANCE_OFFSET:
            {
                SelectTextureStageState(stage, D3DTSS_BUMPENVLOFFSET, value);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_0x1a2:
            {
                if ((u32)value == 1)
                {
                    DAT_60018864 = 0;
                }
                else
                {
                    DAT_600fc890 = value;
                    DAT_60018864 = 1;
                }

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_0x1a3:
            {
                if (State.DX.Active.Instance != NULL)
                {
                    if (State.DX.Active.Instance->TestCooperativeLevel() != DD_OK) { return NULL; }
                }

                pvVar12 = 1; break;

            }
            case RENDERER_MODULE_STATE_0x1a4:
            {
                State.DX.Settings.UnknownValue104 = value;

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_TRANSFORM_WORLD:
            {
                switch (stage)
                {
                case RENDERER_TRANSFORM_STATE_WORLD_0: { State.DX.Active.Device->SetTransform(D3DTRANSFORMSTATE_WORLD, value); break; }
                case RENDERER_TRANSFORM_STATE_WORLD_1: { State.DX.Active.Device->SetTransform(D3DTRANSFORMSTATE_WORLD1, value); break; }
                case RENDERER_TRANSFORM_STATE_WORLD_2: { State.DX.Active.Device->SetTransform(D3DTRANSFORMSTATE_WORLD2, value); break; }
                case RENDERER_TRANSFORM_STATE_WORLD_3: { State.DX.Active.Device->SetTransform(D3DTRANSFORMSTATE_WORLD3, value); break; }
                }

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_TRANSFORM_VIEW:
            {
                State.DX.Active.Device->SetTransform(D3DTRANSFORMSTATE_VIEW, value);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_TRANSFORM_PROJECTION:
            {
                State.DX.Active.Device->SetTransform(D3DTRANSFORMSTATE_PROJECTION, value);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_TRANSFORM_MULTIPLY_WORLD:
            {
                switch (stage)
                {
                case RENDERER_TRANSFORM_STATE_MULTIPLY_WORLD_0: { State.DX.Active.Device->MultiplyTransform(D3DTRANSFORMSTATE_WORLD, value); break; }
                case RENDERER_TRANSFORM_STATE_MULTIPLY_WORLD_1: { State.DX.Active.Device->MultiplyTransform(D3DTRANSFORMSTATE_WORLD1, value); break; }
                case RENDERER_TRANSFORM_STATE_MULTIPLY_WORLD_2: { State.DX.Active.Device->MultiplyTransform(D3DTRANSFORMSTATE_WORLD2, value); break; }
                case RENDERER_TRANSFORM_STATE_MULTIPLY_WORLD_3: { State.DX.Active.Device->MultiplyTransform(D3DTRANSFORMSTATE_WORLD3, value); break; }
                }

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_TRANSFORM_MULTIPLY_VIEW:
            {
                State.DX.Active.Device->MultiplyTransform(D3DTRANSFORMSTATE_VIEW, value);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_TRANSFORM_MULTIPLY_PROJECTION:
            {
                State.DX.Active.Device->SetTransform(D3DTRANSFORMSTATE_PROJECTION, value);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_ACQUIRE_TRANSFORM_WORLD:
            {
                switch (stage)
                {
                case RENDERER_TRANSFORM_STATE_WORLD_0: { State.DX.Active.Device->GetTransform(D3DTRANSFORMSTATE_WORLD, value); break; }
                case RENDERER_TRANSFORM_STATE_WORLD_1: { State.DX.Active.Device->GetTransform(D3DTRANSFORMSTATE_WORLD1, value); break; }
                case RENDERER_TRANSFORM_STATE_WORLD_2: { State.DX.Active.Device->GetTransform(D3DTRANSFORMSTATE_WORLD2, value); break; }
                case RENDERER_TRANSFORM_STATE_WORLD_3: { State.DX.Active.Device->GetTransform(D3DTRANSFORMSTATE_WORLD3, value); break; }
                }

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_ACQUIRE_TRANSFORM_VIEW:
            {
                State.DX.Active.Device->GetTransform(D3DTRANSFORMSTATE_VIEW, value);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_ACQUIRE_TRANSFORM_PROJECTION:
            {
                State.DX.Active.Device->GetTransform(D3DTRANSFORMSTATE_PROJECTION, value);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_LIGHT:
            {
                D3DLIGHT7 dl;

                const RendererLight* light = (RendererLight*)value;

                dl.dltType = (D3DLIGHTTYPE)light->Type;

                dl.dcvDiffuse.a = (f32)((light->Diffuse >> 24) & 0xff) / 255.0f;
                dl.dcvDiffuse.r = (f32)((light->Diffuse >> 16) & 0xff) / 255.0f;
                dl.dcvDiffuse.g = (f32)((light->Diffuse >> 8) & 0xff) / 255.0f;
                dl.dcvDiffuse.b = (f32)((light->Diffuse >> 0) & 0xff) / 255.0f;

                dl.dcvSpecular.a = (f32)((light->Specular >> 24) & 0xff) / 255.0f;
                dl.dcvSpecular.r = (f32)((light->Specular >> 16) & 0xff) / 255.0f;
                dl.dcvSpecular.g = (f32)((light->Specular >> 8) & 0xff) / 255.0f;
                dl.dcvSpecular.b = (f32)((light->Specular >> 0) & 0xff) / 255.0f;

                dl.dcvAmbient.a = (f32)((light->Ambient >> 24) & 0xff) / 255.0f;
                dl.dcvAmbient.r = (f32)((light->Ambient >> 16) & 0xff) / 255.0f;
                dl.dcvAmbient.g = (f32)((light->Ambient >> 8) & 0xff) / 255.0f;
                dl.dcvAmbient.b = (f32)((light->Ambient >> 0) & 0xff) / 255.0f;

                dl.dvPosition.x = light->Position.X;
                dl.dvPosition.y = light->Position.Y;
                dl.dvPosition.z = light->Position.Z;

                dl.dvDirection.x = light->Direction.X;
                dl.dvDirection.y = light->Direction.Y;
                dl.dvDirection.z = light->Direction.Z;

                dl.dvRange = light->Range;
                dl.dvFalloff = light->Falloff;

                dl.dvAttenuation0 = light->Attenuation0;
                dl.dvAttenuation1 = light->Attenuation1;
                dl.dvAttenuation2 = light->Attenuation2;

                dl.dvTheta = light->Theta;
                dl.dvPhi = light->Phi;

                State.DX.Active.Device->SetLight(stage, &dl);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_ACQUIRE_LIGHT:
            {
                D3DLIGHT7 dl;
                State.DX.Active.Device->GetLight(stage, &dl);

                RendererLight* light = (RendererLight*)value;

                light->Type = (RENDRERER_LIGHT_TYPE)dl.dltType;

                light->Diffuse = (((u32)dl.dcvDiffuse.a) << 24) | (((u32)dl.dcvDiffuse.r) << 16) | (((u32)dl.dcvDiffuse.g) << 8) | (((u32)dl.dcvDiffuse.b) << 0);
                light->Specular = (((u32)dl.dcvSpecular.a) << 24) | (((u32)dl.dcvSpecular.r) << 16) | (((u32)dl.dcvSpecular.g) << 8) | (((u32)dl.dcvSpecular.b) << 0);
                light->Ambient = (((u32)dl.dcvAmbient.a) << 24) | (((u32)dl.dcvAmbient.r) << 16) | (((u32)dl.dcvAmbient.g) << 8) | (((u32)dl.dcvAmbient.b) << 0);

                light->Position.X = dl.dvPosition.x;
                light->Position.Y = dl.dvPosition.y;
                light->Position.Z = dl.dvPosition.z;

                light->Direction.X = dl.dvDirection.x;
                light->Direction.Y = dl.dvDirection.y;
                light->Direction.Z = dl.dvDirection.z;

                light->Range = dl.dvRange;
                light->Falloff = dl.dvFalloff;

                light->Attenuation0 = dl.dvAttenuation0;
                light->Attenuation1 = dl.dvAttenuation1;
                light->Attenuation2 = dl.dvAttenuation2;

                light->Theta = dl.dvTheta;
                light->Phi = dl.dvPhi;

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_LIGHT_STATE:
            {
                State.DX.Active.Device->LightEnable(stage, value);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_MATERIAL:
            {
                D3DMATERIAL7 dm;

                const RendererMaterial* material = (RendererMaterial*)value;

                dl.diffuse.a = (f32)((light->Diffuse >> 24) & 0xff) / 255.0f;
                dl.diffuse.r = (f32)((light->Diffuse >> 16) & 0xff) / 255.0f;
                dl.diffuse.g = (f32)((light->Diffuse >> 8) & 0xff) / 255.0f;
                dl.diffuse.b = (f32)((light->Diffuse >> 0) & 0xff) / 255.0f;

                dl.ambient.a = (f32)((light->Ambient >> 24) & 0xff) / 255.0f;
                dl.ambient.r = (f32)((light->Ambient >> 16) & 0xff) / 255.0f;
                dl.ambient.g = (f32)((light->Ambient >> 8) & 0xff) / 255.0f;
                dl.ambient.b = (f32)((light->Ambient >> 0) & 0xff) / 255.0f;

                dl.specular.a = (f32)((light->Specular >> 24) & 0xff) / 255.0f;
                dl.specular.r = (f32)((light->Specular >> 16) & 0xff) / 255.0f;
                dl.specular.g = (f32)((light->Specular >> 8) & 0xff) / 255.0f;
                dl.specular.b = (f32)((light->Specular >> 0) & 0xff) / 255.0f;

                dl.emissive.a = (f32)((light->Specular >> 24) & 0xff) / 255.0f;
                dl.emissive.r = (f32)((light->Specular >> 16) & 0xff) / 255.0f;
                dl.emissive.g = (f32)((light->Specular >> 8) & 0xff) / 255.0f;
                dl.emissive.b = (f32)((light->Specular >> 0) & 0xff) / 255.0f;

                dm.power = material->Power;

                State.DX.Active.Device->SetMaterial(&dm);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_ACQUIRE_MATERIAL:
            {
                D3DMATERIAL7 dm;
                State.DX.Active.Device->GetMaterial(&dm);

                RendererMaterial* material = (RendererMaterial*)value;

                material->Diffuse = (((u32)dm.diffuse.a) << 24) | (((u32)dm.diffuse.r) << 16) | (((u32)dm.diffuse.g) << 8) | (((u32)dm.diffuse.b) << 0);
                material->Ambient = (((u32)dm.ambient.a) << 24) | (((u32)dm.ambient.r) << 16) | (((u32)dm.ambient.g) << 8) | (((u32)dm.ambient.b) << 0);
                material->Specular = (((u32)dm.specular.a) << 24) | (((u32)dm.specular.r) << 16) | (((u32)dm.specular.g) << 8) | (((u32)dm.specular.b) << 0);
                material->Emissive = (((u32)dm.emissive.a) << 24) | (((u32)dm.emissive.r) << 16) | (((u32)dm.emissive.g) << 8) | (((u32)dm.emissive.b) << 0);

                material->Power = dm.power;

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_RENDER_PACKET:
            {
                if (!State.Scene.IsActive)
                {
                    BeginScene();

                    State.Scene.IsActive = TRUE;
                }

                AttemptRender();

                const RendererPacket* packet = (RendererPacket*)value;

                if (packet->IndexCount == 0 || packet->Indexes == NULL)
                {
                    if (State.DX.Active.Device->DrawPrimitive(packet->Type,
                        packet->FVF, packet->Vertexes, packet->VertexCount, D3DDP_NONE) != DD_OK) { return NULL; }
                }
                else
                {
                    if (State.DX.Active.Device->DrawIndexedPrimitive(packet->Type,
                        packet->FVF, packet->Vertexes, packet->VertexCount,
                        packet->Indexes, packet->IndexCount, D3DDP_NONE) != DD_OK) { return NULL; }
                }

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_0x1b4:
            {
                if (value != NULL)
                {
                    RendererUnknown* todo = (RendererUnknown*)value;

                    todo->Unk01 = State.DX.Settings.UnknownValue103;
                    todo->Unk02 = State.DX.Capabilities.MaxActiveLights;
                    todo->Unk03 = DAT_6005abc8;
                    todo->Unk04 = DAT_6005abcc;
                    todo->Unk05 = DAT_6005abd0;
                    todo->Unk06 = State.DX.Capabilities.MemoryOperationMode;
                    todo->Unk07 = DAT_6005abd8;
                }

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_AMBIENT:
            {
                State.DX.Active.Device->SetRenderState(D3DRENDERSTATE_AMBIENT, value);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_CLIPPING:
            {
                AttemptRender();

                State.DX.Active.Device->SetRenderState(D3DRENDERSTATE_CLIPPING, value);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_LIGHTING:
            {
                State.DX.Active.Device->SetRenderState(D3DRENDERSTATE_LIGHTING, value != NULL ? TRUE : FALSE); // TODO bool

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_0x1b8:
            {
                D3DDEVICEDESC7 caps;
                ZeroMemory(&caps, sizeof(D3DDEVICEDESC7));

                caps.dpcLineCaps.dwSize = sizeof(D3DPRIMCAPS);
                caps.dpcTriCaps.dwSize = sizeof(D3DPRIMCAPS);

                State.DX.Active.Device->GetCaps(&caps);

                D3DVERTEXBUFFERDESC desc;

                // TODO What is happening here?

                desc.dwSize = sizeof(D3DVERTEXBUFFERDESC);
                desc.dwCaps = D3DVBCAPS_NONE;
                desc.dwFVF = todo;
                desc.dwNumVertices = 65535; // TODO

                State.DX.Active.DirectX->CreateVertexBuffer(&desc, value, D3DVBCAPS_NONE);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_0x1b9:
            {
                // TODO (**(code**)(**value + 0xc))(*value, *(undefined4*)((int)value + 0x10), (int)value + 0x20, 0);
                
                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_0x1ba:
            {
                // TODO (**(code **)(**value + 0x10))(*value);
                
                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_RENDER_BUFFERED_PACKET:
            {
                if (!State.Scene.IsActive)
                {
                    BeginScene();

                    State.Scene.IsActive = TRUE;
                }

                const RendererBufferPacket* packet = (RendererBufferPacket*)value;

                if (packet->IndexCount == 0 || packet->Indexes == NULL)
                {
                    State.DX.Active.Device->DrawPrimitiveVB(packet->Vertexes,
                        packet->Type, 0, packet->VertexCount, D3DDP_NONE);
                }
                else
                {
                    State.DX.Active.Device->DrawIndexedPrimitiveVB(packet->Vertexes,
                        packet->Type, 0, packet->VertexCount, packet->Indexes, packet->IndexCount, D3DDP_NONE);
                }

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_0x1bc:
            {
                // TODO is this correct?
                IDirect3DVertexBuffer7* buffer = (IDirect3DVertexBuffer7*)value;

                buffer->Optimize(State.DX.Active.Device, D3DVBOPTIMIZE_NONE);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_CLIP_PLANE:
            {
                State.DX.Active.Device->SetClipPlane(stage, value);

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_0x1be: { return NULL; }
            case RENDERER_MODULE_STATE_SELECT_CLIP_STATE:
            {
                if (value == NULL) // TODO bool
                {
                    DAT_60018848 = DAT_60018848 ^ 1 << stage;
                }
                else
                {
                    DAT_60018848 = DAT_60018848 | 1 << stage;
                }

                State.DX.Active.Device->SetRenderState(D3DRENDERSTATE_CLIPPLANEENABLE, DAT_60018848)

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_SELECT_VERTEX_BLEND:
            {
                State.DX.Active.Device->SetRenderState(D3DRENDERSTATE_VERTEXBLEND, value)

                pvVar12 = 1; break;
            }
            case RENDERER_MODULE_STATE_0x1c1:
            {
                if (DAT_600170f0 & 0x100) // TODO
                {
                    SelectTextureStageState(0, 0xb, 0);
                    SelectTextureStageState(1, 0xb, 0);
                }

                if (DAT_600170f0 & 0x200) // TODO
                {
                    SelectTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
                    SelectTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
                }

                if (DAT_600170f0 & 0x300) // TODO
                {
                    SelectTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
                    SelectTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
                    SelectTextureStageState(2, D3DTSS_TEXCOORDINDEX, 2);
                }

                if (DAT_600170f0 & 0x400) // TODO
                {
                    SelectTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
                    SelectTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
                    SelectTextureStageState(2, D3DTSS_TEXCOORDINDEX, 2);
                    SelectTextureStageState(3, D3DTSS_TEXCOORDINDEX, 3);
                }

                return NULL;
            }
            case RENDERER_MODULE_STATE_0x1c2:
            {
                FUN_60001e70(state, value);

                return NULL;
            }
            case RENDERER_MODULE_STATE_0x1c3:
            {
                pvVar12 = DAT_6005ab2c;

                if (pvVar12 == 0) { return NULL; } break;
            }
            }
        }

        switch (type)
        {
        case 0: { return NULL; } // TODO
        case 1: // TODO
        {
            SelectTexture(value);

            pvVar12 = 1; break;
        }
        case 2: // TODO
        {
            switch ((u32)value)
            {
            case 0: // TODO
            {
                SelectRendererState(0x16, 1); // TODO

                DAT_60058e1c = 1; // TODO

                break;
            }
            case 1: // TODO
            {
                SelectRendererState(0x16, 3); // TODO

                DAT_60058e1c = 0x80000000; // TODO

                break;
            }
            case 2: // TODO
            {
                SelectRendererState(0x16, 2); // TODO

                DAT_60058e1c = 0; // TODO

                break;
            }
            default: { return NULL; }
            }

            pvVar12 = 1; break;
        }
        case 3: // TODO
        {
            FUN_60008d00(value);

            pvVar12 = 1; break;
        }
        case 4:
        {
            if (!State.Capabilities.DepthBuffer) { return NULL; }

            switch ((u32)value)
            {
            case 0: // TODO
            {
                SelectRendererState(0xe, 0); // TODO

                if (DAT_60058d78 != 0xd) // TODO
                {
                    SelectRendererState(7, 0); // TODO
                }

                SelectRendererState(0x17, 8); // TODO

                break;
            }
            case 1: // TODO
            {
                SelectRendererState(0xe, 1); // TODO
                SelectRendererState(7, 1); // TODO
                SelectRendererState(0x17, DAT_600170d4); // TODO

                break;
            }
            case 2: // TODO
            {
                if (DAT_6005ab08 == 0) // TODO
                {
                    SelectRendererState(0xe, 1); // TODO
                    SelectRendererState(7, 1); // TODO
                    SelectRendererState(0x17, DAT_600170d4); // TODO

                    return NULL;
                }

                SelectRendererState(7, 1); // TODO
                SelectRendererState(0xe, 1); // TODO
                SelectRendererState(0x17, DAT_600170d4); // TODO
                SelectRendererState(7, 2); // TODO

                break;
            }
            default: { return NULL; }
            }

            pvVar12 = 1; break;
        }
        case 5: // TODO
        {
            SelectRendererState(0x1a, (value != (void*)0x0)); // TODO

            pvVar12 = DAT_6005ab00;

            if (pvVar12 == 0) { return NULL; }

            break;
        }
        case 6: // TODO
        {
            switch ((u32)value)
            {
            case 0: // TODO
            {
                SelectRendererState(9, 1); // TODO
                SelectRendererState(0x1d, 0); // TODO

                DAT_600170ec = 0;

                break;
            }
            case 1: // TODO
            {
                SelectRendererState(9, 2); // TODO
                SelectRendererState(0x1d, 0); // TODO

                DAT_600170ec = 1;

                break;
            }
            case 2: // TODO
            {
                if (DAT_6005ab44 == 0) { return NULL; }

                SelectRendererState(9, 2); // TODO
                SelectRendererState(0x1d, 1); // TODO

                DAT_600170ec = 1;
            }
            case 3: { return NULL; } // TODO
            default: { return NULL; } // TODO
            }

            pvVar12 = 1; break;
        }
        case 7: // TODO
        {
            switch ((u32)value)
            {
            case 0: // TODO
            {
                SelectTextureStageState(stage, 0x10, 1); // TODO
                SelectTextureStageState(stage, 0x11, 1); // TODO

                break;
            }
            case 1: // TODO
            {
                SelectTextureStageState(stage, 0x10, 2); // TODO
                SelectTextureStageState(stage, 0x11, 2); // TODO

                break;
            }
            case 2: // TODO
            {
                if (DAT_6005ab3c == 0) { return NULL; }

                SelectTextureStageState(stage, 0x10, 5); // TODO
                SelectTextureStageState(stage, 0x11, 3); // TODO
                SelectTextureStageState(stage, 0x15, DAT_6005aba4); // TODO
            }
            default: { return NULL; } // TODO
            }

            pvVar12 = 1; break;
        }
        case 8: // TODO
        {
            pvVar12 = DAT_6005ab58;

            if (DAT_6005ab58 != 0)
            {
                SelectTextureStageState(stage, 0x13, value); // TODO

                pvVar12 = DAT_6005ab58;
            }

            if (pvVar12 == 0) { return NULL; }

            break;
        }
        case 9: // TODO
        {
            const s32 val = (s32)value;

            if (val < 0) { return NULL; }

            switch (val)
            {
            case 0: // TODO
            case 1: // TODO
            {
                stage = 0; break;
            }
            case 2: // TODO
            {
                stage = DAT_6005ab34;

                if (1 < DAT_6005ab34) { stage = 2; }

                break;
            }
            default: { return NULL; }
            }

            if (SelectRendererState(2, stage) == 0) { return NULL; } // TODO

            if (val != 0 && DAT_6005ab34 == 0) { return NULL; }

            pvVar12 = 1; break;
        }
        case 10: // TODO
        {
            todo
        }
        // TODO
        }


        // TODO NOT IMPLEMENTED

        if (file == NULL)
        {
            file = fopen("C:\\GIT\\AZHS-Inject\\Bin\\x86\\Release\\log.txt", "wt");
        }

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG

        return 0; // TODO
    }

    // 0x60008980
    // a.k.a. _THRASH_settexture
    DLLAPI u32 STDCALLAPI SelectTexture(void*)
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
        // TODO NOT IMPLEMENTED

        return 0; // TODO
    }

    // 0x60003750
    // a.k.a. _THRASH_setvideomode
    DLLAPI u32 STDCALLAPI SelectVideoMode(void*, void*, void*)
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
        // TODO NOT IMPLEMENTED

        return 0; // TODO
    }

    // 0x60001380
    // a.k.a. _THRASH_sync
    DLLAPI u32 STDCALLAPI Sync(const u32 mode)
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
        // TODO NOT IMPLEMENTED

        return 0; // TODO
    }

    // 0x60008e70
    // a.k.a.  _THRASH_talloc
    DLLAPI void* STDCALLAPI AllocateTexture(void*, void*, void*, void*, void*)
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
        // TODO NOT IMPLEMENTED

        return 0; // TODO
    }

    // 0x60008eb0
    // a.k.a. _THRASH_tfree
    DLLAPI u32 STDCALLAPI ReleaseTexture(void*)
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
        // TODO NOT IMPLEMENTED

        return 0; // TODO
    }

    // 0x60008fc0
    // a.k.a. _THRASH_treset
    DLLAPI u32 STDCALLAPI ResetTextures(void)
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
        // TODO NOT IMPLEMENTED

        return 0; // TODO
    }

    // 0x60008f20
    // a.k.a.  _THRASH_tupdate
    DLLAPI u32 STDCALLAPI UpdateTexture(void* p1, void*, void*) // TODO
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
        // TODO NOT IMPLEMENTED

        return 0; // TODO
    }

    // 0x60008f60
    // a.k.a. _THRASH_tupdaterect
    DLLAPI u32 STDCALLAPI UpdateTextureRectangle(void*, void*, void*, void*, void*, void*, void*, void*, void*)
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
        // TODO NOT IMPLEMENTED

        return 0; // TODO
    }

    // 0x60001830
    // a.k.a.  _THRASH_unlockwindow
    DLLAPI u32 STDCALLAPI UnlockGameWindow(const RendererModuleWindowLock* state)
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
        // TODO NOT IMPLEMENTED

        return 0; // TODO
    }

    // 0x600011a0
    // a.k.a. _THRASH_window
    DLLAPI u32 STDCALLAPI SelectGameWindow(const u32 indx)
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
        // TODO NOT IMPLEMENTED

        return 0; // TODO
    }

    // 0x60003c00
    // a.k.a. _THRASH_writerect
    DLLAPI u32 STDCALLAPI WriteRectangle(void*, void*, void*, void*, void*)
    {

        fprintf(file, "%s\n", __FUNCTION__); // TODO DEBUG
        // TODO NOT IMPLEMENTED

        return 0; // TODO
    }
}