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
    extern s32 RendererState; // 0x600170b8
    extern s32 RendererDeviceIndex; // 0x600170bc

    extern u32 RendererToggleOptions; // 0x600170cc
    extern u32 RendererBlitOptions; // 0x600170d0
    extern D3DCMPFUNC RendererDepthFunction; // 0x600170d4
    extern D3DCMPFUNC RendererAlphaFunction; // 0x600170d8
    extern f32 RendererFogEnd; // 0x600170dc
    extern f32 RendererClearDepth; // 0x600170e0
    extern u32 RendererFogColor; // 0x600170e4
    extern f32 RendererFogDensity; // 0x600170e8
    extern u32 RendererShadeMode; // 0x600170ec
    extern u32 RendererVertexType; // 0x600170f0
    extern u32 RendererTextureHint; // 0x600170f4

    extern u32 RendererVertexSize; // 0x600170fc
    extern u32 MaximumRendererVertexCount; // 0x60017100

    extern D3DPRIMITIVETYPE RendererPrimitiveType; // 0x60017104

    extern u32 RendererClearColor; // 0x60017118

    extern u32 RendererVersion; // 0x60017128
    extern u32 RendererLineVertexSize; // 0x6001712c
    extern u32 RendererIndexSize; // 0x60017130

    extern f32 RendererFogStart; // 0x6001884c

    extern f32 RendererDepthBias; // 0x6001885c

    extern s32 RendererDeviceType; // 0x60058d70

    extern u8 RendererFogAlphas[MAX_OUTPUT_FOG_ALPHA_COUNT]; // 0x60018740

    extern RendererModule::MinMax MinMax[6]; // 0x60017088 // TODO

    extern RendererModule::RendererModuleDescriptor2 ModuleDescriptor; // 0x600fc7c0
    extern RendererModule::RendererModuleDescriptorDeviceCapabilities ModuleDescriptorDeviceCapabilities[MAX_DEVICE_CAPABILITIES_COUNT]; // 0x60017250

    extern s32 RendererTextureFormatStates[MAX_USABLE_TEXTURE_FORMAT_COUNT]; // 0x60017010

    extern s32 UnknownArray06[6]; // 0x60017068 // TODO

    extern u32 UnknownFormatValues[MAX_USABLE_TEXTURE_FORMAT_INDEX_COUNT]; // 0x600171c8
}