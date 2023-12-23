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

#include "Basic.hxx"

#define RENDERER_PIXEL_FORMAT_NONE 0
#define RENDERER_PIXEL_FORMAT_P8 2
#define RENDERER_PIXEL_FORMAT_R5G5B5 3
#define RENDERER_PIXEL_FORMAT_R5G6B5 4
#define RENDERER_PIXEL_FORMAT_R8G8B8 5
#define RENDERER_PIXEL_FORMAT_A8R8G8B8 6
#define RENDERER_PIXEL_FORMAT_R4G4B4 7
#define RENDERER_PIXEL_FORMAT_A4L4 8
#define RENDERER_PIXEL_FORMAT_A8P8 9
#define RENDERER_PIXEL_FORMAT_YUV2 10
#define RENDERER_PIXEL_FORMAT_A1R5G5B5 11
#define RENDERER_PIXEL_FORMAT_DXT1 12
#define RENDERER_PIXEL_FORMAT_DXT3 13
#define RENDERER_PIXEL_FORMAT_DXT5 14
#define RENDERER_PIXEL_FORMAT_V8U8 16
#define RENDERER_PIXEL_FORMAT_BUMPDUDV_1 18
#define RENDERER_PIXEL_FORMAT_BUMPDUDV_2 19
#define RENDERER_PIXEL_FORMAT_BUMPDUDV_3 20
#define RENDERER_PIXEL_FORMAT_A8 23
#define RENDERER_PIXEL_FORMAT_L8 25
#define RENDERER_PIXEL_FORMAT_A8L8 26
#define RENDERER_PIXEL_FORMAT_D16 27
#define RENDERER_PIXEL_FORMAT_D24S8 28
#define RENDERER_PIXEL_FORMAT_D16L 29
#define RENDERER_PIXEL_FORMAT_D32 30
#define RENDERER_PIXEL_FORMAT_D15S1 31
#define RENDERER_PIXEL_FORMAT_D24X8 32
#define RENDERER_PIXEL_FORMAT_D24X4S4 33

#define RENDERER_RESOLUTION_MODE_INVALID (-1)
#define RENDERER_RESOLUTION_MODE_NONE 0
#define RENDERER_RESOLUTION_MODE_640_480_16 1
#define RENDERER_RESOLUTION_MODE_800_600_16 2
#define RENDERER_RESOLUTION_MODE_1024_768_16 3
#define RENDERER_RESOLUTION_MODE_1280_1024_16 4
#define RENDERER_RESOLUTION_MODE_1600_1200_16 5

namespace Renderer
{
    struct RendererTexture;

    struct RVX;

    struct RTLVX
    {
        f32x3 XYZ;
        f32 RHW;
        u32 Color;
        u32 Specular;
        f32x2 UV;
    };

    struct RTLVX2
    {
        f32x3 XYZ;
        f32 RHW;
        u32 Color;
        u32 Specular;
        f32x2 UV1;
        f32x2 UV2;
    };
}