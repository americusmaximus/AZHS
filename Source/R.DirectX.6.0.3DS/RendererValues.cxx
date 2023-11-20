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

#include "RendererValues.hxx"

using namespace RendererModule;

namespace RendererModuleValues
{
    s32 RendererDeviceIndex = INVALID_RENDERER_DEVICE_INDEX;

    u32 RendererFogColor = DEFAULT_FOG_COLOR;
    f32 RendererFogDensity = DEFAULT_FOG_DINSITY;
    u32 RendererShadeMode = RENDERER_MODULE_SHADE_GOURAUD;
    u32 RendererTextureHint = RENDERER_MODULE_TEXTURE_HINT_DYNAMIC;

    f32 RendererFogStart = DEFAULT_FOG_START;
    f32 RendererFogEnd = DEFAULT_FOG_END;

    f32 RendererDepthBias;

    u32 RendererVersion = RENDERER_MODULE_VERSION_105;
    s32 RendererDeviceType = RENDERER_MODULE_DEVICE_TYPE_ACCELERATED;

    u8 RendererFogAlphas[MAX_OUTPUT_FOG_ALPHA_COUNT];

    u32 MaximumRendererVertexCount = MAX_RENDERER_VERTEX_COUNT;

    RendererModuleDescriptor ModuleDescriptor;
    RendererModuleDescriptorDeviceCapabilities ModuleDescriptorDeviceCapabilities[MAX_RENDERER_MODULE_DEVICE_CAPABILITIES_COUNT];

    s32 UnknownArray06[6] =
    {
        0, 0, 0, 0, 5, -1
    }; // TODO

    u32 UnknownFormatValues[MAX_USABLE_TEXTURE_FORMAT_COUNT] =
    {
        0, 1, 1, 2, 2, 3, 4, 2, 1, 2, 2, 0, 0, 0
    }; // TODO

    s32 RendererTextureFormatStates[MAX_USABLE_TEXTURE_FORMAT_COUNT] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1
    }; // TODO
}