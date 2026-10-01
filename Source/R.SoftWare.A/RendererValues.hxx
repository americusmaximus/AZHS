/*
vs

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

#include "Renderer.hxx"

#define DEFAULT_DEVICE_INDEX 0
#define INVALID_DEVICE_INDEX (-1)

namespace RendererModuleValues
{
    extern s32 RendererTextureFormatStates[MAX_USABLE_TEXTURE_FORMAT_COUNT]; // 0x6003f004

    extern s32 UnknownArray06[MAX_UNKNOWN_COUNT]; // 0x6003f030 // TODO
    
    extern s32 RendererDeviceIndex; // 0x6003f094

    extern s32 RendererVideoMode;	// 0x6003f09c

    extern u32 TextureColorDepth;   // 0x6003f0e8

    extern u32 RendererSurfaceStride;		// 0x6003f0f0

    extern u32 GreenRendererColorMask;      // 0x6003f340
    extern u32 RedRendererColorMask;        // 0x6003f344
    extern u32 BlueRendererColorMask;       // 0x6003f348
    extern u32 NonGreenRendererColorMask;   // 0x6003f34c
    extern u32 RedColorBitsOffset;			// 0x6003f350
    extern u32 GreenColorBitsSize;			// 0x6003f354

    extern RendererModule::RendererModuleDescriptor ModuleDescriptor; // 0x60041010

    extern u16 VertexColor;					// 0x6003f11c
    extern u32 OptimizedClearColor;			// 0x6003f120

    extern u32 Unknown32BitColors1[MAX_UNKNOWN_COLOR_ARRAY_COUNT];  // 0x6003f140
    extern u32 Unknown32BitColors2[MAX_UNKNOWN_COLOR_ARRAY_COUNT];  // 0x6003f1c0
    extern u32 Unknown32BitColors3[MAX_UNKNOWN_COLOR_ARRAY_COUNT];  // 0x6003f180
    extern u32 Unknown32BitColors4[MAX_UNKNOWN_COLOR_ARRAY_COUNT];  // 0x6003f200

    extern u32 Unknown16BitColors1[MAX_UNKNOWN_COLOR_ARRAY_COUNT];  // 0x6003f240
    extern u32 Unknown16BitColors2[MAX_UNKNOWN_COLOR_ARRAY_COUNT];  // 0x6003f2c0
    extern u32 Unknown16BitColors3[MAX_UNKNOWN_COLOR_ARRAY_COUNT];  // 0x6003f280
    extern u32 Unknown16BitColors4[MAX_UNKNOWN_COLOR_ARRAY_COUNT];  // 0x6003f300
}
