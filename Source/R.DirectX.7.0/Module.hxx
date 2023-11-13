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

#include "Native.Basic.hxx"
#include "RendererModule.Export.hxx"

#define DIRECTDRAW_VERSION 0x700
#include <ddraw.h>

#ifdef __WATCOMC__
#undef PURE
#define PURE
#endif

#define DIRECT3D_VERSION 0x700
#include <d3d.h>

#define DDGDI_NONE 0
#define DDEDM_NONE 0
#define D3DDP_NONE 0
#define D3DVBCAPS_NONE 0
#define D3DVBOPTIMIZE_NONE 0

#ifdef _WIN64
#define RENDERER_MODULE_TITLE_NAME "DirectX 7.0 (x64)"
#else
#define RENDERER_MODULE_TITLE_NAME "DirectX 7.0 (x32)"
#endif

#define RENDERER_MODULE_AUTHOR "Americus Maximus"

#define MIN_RENDERER_MODULE_VERSION 104
#define MAX_RENDERER_MODULE_VERSION 107

#define MAX_DX_CAPABILITIES_DEPTH_BUFFER_COUNT 6
#define MAX_DX_CAPABILITIES_DISPLAY_COUNT 16

#define MIN_DX_VIDEO_MEMORY_SIZE (2 * 1024 * 1024)

#define MAX_DX_CAPABILITIES_DISPLAY_MODE_COUNT 99

#define MAX_RENDERER_MODULE_TEXTURE_COUNT 65535

#define DX_CAPABILITIES_DISPLAY_MODE_640_480_16_INDEX 1
#define DX_CAPABILITIES_DISPLAY_MODE_800_600_16_INDEX 2
#define DX_CAPABILITIES_DISPLAY_MODE_1024_768_16_INDEX 3
#define DX_CAPABILITIES_DISPLAY_MODE_1280_1024_16_INDEX 4
#define DX_CAPABILITIES_DISPLAY_MODE_1600_1200_16_INDEX 5

#define STANDARD_DX_CAPABILITIES_DISPLAY_MODE_COUNT 5

#define INVALID_DX_CAPABILITIES_DISPLAY_MODE_INDEX (-1)

#define MAX_DX_CAPABILITIES_DISPLAY_MODE_SCORE 10000

#define MAX_DX_CAPABILITIES_TEXTURE_FORMAT_COUNT 32

#define DEFAULT_SETTINGS_BUFFER_LENGTH 80

#define DEFAULT_SETTINGS_PROPERTY_TEMPLATE "%s_%s"
#define THRASH_SETTINGS_PROPERTY_TEMPLATE "THRASH_%s"

#define DX_DISPLAY_ENVIRONEMNT_PROPERTY_NAME "THRASH_DISPLAY"
#define DX_DISPLAY_SETTINGS_PROPERTY_NAME "displays"

#define DX_DEVICE_TYPE_ENVIRONEMNT_PROPERTY_NAME "THRASH_D3DDEVICETYPE"

namespace RendererModule
{
    struct ModuleState
    {
        struct
        {
             // TODO
        } DX;

        // 0x600fc7c0
        RendererModuleDescriptor Descriptor;

        struct
        {
            HWND HWND; // 0x6007b880
        } Window;

        HANDLE Mutex; // 0x600186d4
    };

    extern ModuleState State;

    u32 Version = MAX_RENDERER_MODULE_VERSION;// 0x60017128



    // TODO

    s32 DAT_6005ab88; // 0x6005ab88
    
    s32 DAT_600170c4 = 1; // 0x600170c4 // TODO TYPE
    s32 DAT_600170f4 = 1; // 0x600170f4 // TODO TYPE
    s32 DAT_600fc890; // 0x600fc890
    s32 DAT_60018864; // 0x60018864
    s32 DAT_6005abc8; // 0x6005abc8
    s32 DAT_6005abcc; // 0x6005abcc
    s32 DAT_6005abd0; // 0x6005abd0
    s32 DAT_6005abd8; // 0x6005abd8
    u32 DAT_60018848; // 0x60018848
    u32 DAT_600170f0 = 0x1C4; // 0x600170f0
    u32 DAT_6005ab2c; // 0x6005ab2c
    u32 DAT_60058e1c; // 0x60058e1c
    u32 DAT_600170d4 = 4; // 0x600170d4
    u32 DAT_6005ab08; // 0x6005ab08 // TODO TYPE
    u32 DAT_600170ec = 1; // 0x600170ec // TODO TYPE
    u32 DAT_6005ab44; // 0x6005ab44
    u32 DAT_6005ab3c; // 0x6005ab3c
    u32 DAT_6005aba4; // 0x6005aba4
    u32 DAT_6005ab58; // 0x6005ab58
}