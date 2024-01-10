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

#pragma once

#include "Renderer.hxx"

#define DEFAULT_DEVICE_INDEX 0
#define INVALID_DEVICE_INDEX (-1)

namespace RendererModuleValues
{
    extern s32 RendererDeviceIndex; // 0x60019080

    extern u32 RendererDepthFunction; // 0x6001908c

    extern u32 RendererFogColor; // 0x60019094
    extern f32 RendererFogDensity; // 0x60019098
    extern u32 RendererShadeMode; // 0x6001909c

    extern u32 RendererVertexType; // 0x600190a0
    extern u32 RendererTextureHint; // 0x600190a4

    extern u32 RendererVertexSize; // 0x600190ac

    extern f32 RendererFogStart; // 0x6001d02c
    extern f32 RendererFogEnd; // 0x60019090

    extern f32 RendererDepthBias; // 0x6001d03c

    extern s32 RendererDeviceType; // 0x600190b4
    extern u32 RendererVersion; // 0x600190b8
    extern u32 RendererLineVertexSize; // 0x600190bc

    extern u32 MaximumRendererVertexCount; // 0x600190c4

    extern u8 RendererFogAlphas[MAX_OUTPUT_FOG_ALPHA_COUNT]; // 0x6001cf28

    extern s32 RendererTextureFormatStates[MAX_USABLE_TEXTURE_FORMAT_COUNT]; // 0x60019030
    extern u32 RendererPaletteValues[MAX_TEXTURE_PALETTE_COLOR_COUNT]; // 0x60019114

    extern s32 UnknownArray06[MAX_UNKNOWN_COUNT]; // 0x60019068 // TODO

    extern u32 UnknownFormatValues[MAX_USABLE_TEXTURE_FORMAT_COUNT]; // 0x600190dc

    extern RendererModule::RendererModuleDescriptor2 ModuleDescriptor; // 0x6004ece0
    extern RendererModule::RendererModuleDescriptorDeviceCapabilities ModuleDescriptorDeviceCapabilities[MAX_DEVICE_CAPABILITIES_COUNT]; // 0x6001c4c8
}