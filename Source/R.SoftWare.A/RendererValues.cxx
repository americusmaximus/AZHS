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
#include "RendererValues.hxx"

using namespace Renderer;
using namespace RendererModule;

namespace RendererModuleValues
{
    s32 RendererDeviceIndex = DEFAULT_DEVICE_INDEX;

    s32 RendererVideoMode = DEFAULT_RENDERER_MODE;

    s32 TexturePaletteCount = 16;
    s32 CurrentTexturePaletteCount = -1;
    u32 RendererSurfaceStride = DEFAULT_RENDERER_SURFACE_STRIDE;

    u32 GreenRendererColorMask = 0x3E0;
    u32 RedRendererColorMask = 0x7C00;
    u32 BlueRendererColorMask = 0x1F;
    u32 NonGreenRendererColorMask = 0x7C1F;
    u32 RedColorBitsOffset = 10;
    u32 GreenColorBitsSize = 5;

    RendererModuleDescriptor ModuleDescriptor;

    u16 VertexColor = 0x000F;
    u32 OptimizedClearColor;

    s32 UnknownArray06[MAX_UNKNOWN_COUNT] =
    {
        0, 0, 0, 0, 1, -1
    }; // TODO

    s32 RendererTextureFormatStates[MAX_USABLE_TEXTURE_FORMAT_COUNT] =
    {
        0, 0, 1, 1, 1, 0, 0, 1, 0, 0, -1
    }; // TODO

    u32 Unknown32BitColors1[MAX_UNKNOWN_COLOR_ARRAY_COUNT] =
    {
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF, 0x3DEF
    }; // TODO

    u32 Unknown32BitColors2[MAX_UNKNOWN_COLOR_ARRAY_COUNT] =
    {
        0x0, 0x0, 0xC63, 0xC63, 0x0, 0x0, 0xC63, 0xC63,
        0x0, 0x0, 0xC63, 0xC63, 0x0, 0x0, 0xC63, 0xC63
    }; // TODO

    u32 Unknown32BitColors3[MAX_UNKNOWN_COLOR_ARRAY_COUNT] =
    {
        0x0, 0x0, 0x0, 0x0, 0x1CE7, 0x1CE7, 0x1CE7, 0x1CE7,
        0x0, 0x0, 0x0, 0x0, 0x1CE7, 0x1CE7, 0x1CE7, 0x1CE7
    }; // TODO

    u32 Unknown32BitColors4[MAX_UNKNOWN_COLOR_ARRAY_COUNT] =
    {
        0x0, 0x421, 0x0, 0x421, 0x0, 0x421, 0x0, 0x421,
        0x0, 0x421, 0x0, 0x421, 0x0, 0x421, 0x0, 0x421
    }; // TODO

    u32 Unknown16BitColors1[MAX_UNKNOWN_COLOR_ARRAY_COUNT] =
    {
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF, 0x7BEF
    }; // TODO

    u32 Unknown16BitColors2[MAX_UNKNOWN_COLOR_ARRAY_COUNT] =
    {
        0x0, 0x0, 0x18E3, 0x18E3, 0x0, 0x0, 0x18E3, 0x18E3,
        0x0, 0x0, 0x18E3, 0x18E3, 0x0, 0x0, 0x18E3, 0x18E3
    }; // TODO

    u32 Unknown16BitColors3[MAX_UNKNOWN_COLOR_ARRAY_COUNT] =
    {
        0x0, 0x0, 0x0, 0x0, 0x39E7, 0x39E7, 0x39E7, 0x39E7,
        0x0, 0x0, 0x0, 0x0, 0x39E7, 0x39E7, 0x39E7, 0x39E7
    }; // TODO

    u32 Unknown16BitColors4[MAX_UNKNOWN_COLOR_ARRAY_COUNT] =
    {
        0x0, 0x861, 0x0, 0x861, 0x0, 0x861, 0x0, 0x861,
        0x0, 0x861, 0x0, 0x861, 0x0, 0x861, 0x0, 0x861
    }; // TODO
}
