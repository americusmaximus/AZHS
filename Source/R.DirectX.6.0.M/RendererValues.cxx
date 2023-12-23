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

using namespace Renderer;
using namespace RendererModule;

namespace RendererModuleValues
{
    s32 RendererDeviceIndex = INVALID_DEVICE_INDEX;

    u32 RendererDepthFunction = D3DCMP_LESSEQUAL;

    u32 RendererFogColor = DEFAULT_FOG_COLOR;
    f32 RendererFogDensity = DEFAULT_FOG_DINSITY;
    u32 RendererShadeMode = RENDERER_MODULE_SHADE_GOURAUD;

    u32 RendererVertexType = D3DFVF_TLVERTEX;
    u32 RendererTextureHint = RENDERER_MODULE_TEXTURE_HINT_DYNAMIC;

    u32 RendererVertexSize = sizeof(RTLVX);

    f32 RendererDepthBias;

    f32 RendererFogStart = DEFAULT_FOG_START;
    f32 RendererFogEnd = DEFAULT_FOG_END;

    s32 RendererDeviceType = RENDERER_MODULE_DEVICE_TYPE_0_ACCELERATED;
    u32 RendererVersion = RENDERER_MODULE_VERSION_106;
    u32 RendererLineVertexSize = sizeof(RTLVX);

    u32 MaximumRendererVertexCount = DEFAULT_VERTEX_COUNT;

    u8 RendererFogAlphas[MAX_OUTPUT_FOG_ALPHA_COUNT];

    RendererModuleDescriptor ModuleDescriptor;
    RendererModuleDescriptorDeviceCapabilities ModuleDescriptorDeviceCapabilities[MAX_DEVICE_CAPABILITIES_COUNT];

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

    u32 RendererPaletteValues[MAX_TEXTURE_PALETTE_COLOR_COUNT] =
    {
           0x00000000,
           0x00A80000,
           0x0000A800,
           0x00A8A800,
           0x000000A8,
           0x00A800A8,
           0x000054A8,
           0x00A8A8A8,
           0x00545454,
           0x00FC5454,
           0x0054FC54,
           0x00FCFC54,
           0x005454FC,
           0x00FC54FC,
           0x0054FCFC,
           0x00FCFCFC,
           0x00FCFCFC,
           0x00DCDCDC,
           0x00CCCCCC,
           0x00C0C0C0,
           0x00B0B0B0,
           0x00A4A4A4,
           0x00989898,
           0x00888888,
           0x007C7C7C,
           0x00707070,
           0x00606060,
           0x00545454,
           0x00484848,
           0x00383838,
           0x002C2C2C,
           0x00202020,
           0x00D8D8FC,
           0x00B8B8FC,
           0x009C9CFC,
           0x007C7CFC,
           0x005C5CFC,
           0x004040FC,
           0x002020FC,
           0x000000FC,
           0x000000E4,
           0x000000CC,
           0x000000B4,
           0x0000009C,
           0x00000084,
           0x00000070,
           0x00000058,
           0x00000040,
           0x00D8E8FC,
           0x00C0DCFC,
           0x00ACD4FC,
           0x0094C8FC,
           0x0080C0FC,
           0x0068B8FC,
           0x0054ACFC,
           0x003CA4FC,
           0x00289CFC,
           0x002088E0,
           0x001C78C4,
           0x001464A8,
           0x00105490,
           0x00084074,
           0x00043058,
           0x00002040,
           0x00D8FCFC,
           0x00B8FCFC,
           0x009CFCFC,
           0x007CFCFC,
           0x005CF8FC,
           0x0040F4FC,
           0x0020F4FC,
           0x0000F4FC,
           0x0000D8E4,
           0x0000C0CC,
           0x0000A4B4,
           0x00008C9C,
           0x00007484,
           0x0000586C,
           0x00004054,
           0x00002840,
           0x00D8FCF8,
           0x00B8FCF4,
           0x009CFCE8,
           0x007CFCE0,
           0x005CFCD0,
           0x0040FCC4,
           0x0020FCB4,
           0x0000FCA0,
           0x0000E490,
           0x0000CC80,
           0x0000B474,
           0x00009C60,
           0x00008450,
           0x00007044,
           0x00005834,
           0x00004028,
           0x00D8FCD8,
           0x009CFC9C,
           0x0090EC90,
           0x0084E084,
           0x0078D078,
           0x006CC470,
           0x0064B864,
           0x0058A858,
           0x004C9C50,
           0x00409044,
           0x00348038,
           0x002C742C,
           0x00206824,
           0x00145818,
           0x00084C0C,
           0x00004004,
           0x00FCFCD8,
           0x00FCFCB8,
           0x00FCFC9C,
           0x00F8FC7C,
           0x00FCFC5C,
           0x00FCFC40,
           0x00FCFC20,
           0x00FCFC00,
           0x00E4E400,
           0x00CCCC00,
           0x00B4B400,
           0x009C9C00,
           0x00848400,
           0x00707000,
           0x00585800,
           0x00404000,
           0x00FCECD8,
           0x00FCE0B8,
           0x00FCD49C,
           0x00FCC87C,
           0x00FCBC5C,
           0x00FCB040,
           0x00FC9C00,
           0x00E48C00,
           0x00D08000,
           0x00BC7400,
           0x00A86400,
           0x00905800,
           0x007C4C00,
           0x00683C00,
           0x00543000,
           0x00402400,
           0x00FCD8D8,
           0x00FCBCB8,
           0x00FC9C9C,
           0x00FC807C,
           0x00FC605C,
           0x00FC4040,
           0x00FC0400,
           0x00E40000,
           0x00D00000,
           0x00BC0000,
           0x00A80000,
           0x00900000,
           0x007C0000,
           0x00680000,
           0x00540000,
           0x00400000,
           0x00FCD8F0,
           0x00FCB8E4,
           0x00FC9CD8,
           0x00FC7CD0,
           0x00FC5CC8,
           0x00FC40BC,
           0x00FC20B4,
           0x00FC00A8,
           0x00E40098,
           0x00CC0080,
           0x00B40074,
           0x009C0060,
           0x00840050,
           0x00700044,
           0x00580034,
           0x00400028,
           0x00FCD8FC,
           0x00FCB8FC,
           0x00FC9CFC,
           0x00FC7CFC,
           0x00FC5CFC,
           0x00FC40FC,
           0x00FC20FC,
           0x00E400E0,
           0x00CC00CC,
           0x00B800B8,
           0x00A400A4,
           0x00900090,
           0x007C007C,
           0x00680068,
           0x00540054,
           0x00400040,
           0x00DCE8FC,
           0x00C4D4F0,
           0x00ACC4E4,
           0x0098B4D8,
           0x0080A0CC,
           0x006C90C0,
           0x005480B4,
           0x004070AC,
           0x0038649C,
           0x00345C8C,
           0x002C5480,
           0x00284C70,
           0x00204060,
           0x001C3854,
           0x00143044,
           0x00102838,
           0x00CCD8FC,
           0x00B8CCF8,
           0x00A8C0F4,
           0x0094B4F0,
           0x0084A8EC,
           0x00749CE8,
           0x006494E4,
           0x00588CE0,
           0x004C80D8,
           0x004078D4,
           0x00346CC8,
           0x002C60C0,
           0x002454B4,
           0x001C48A8,
           0x00143C9C,
           0x000C3094,
           0x00000000,
           0x00080808,
           0x00101010,
           0x00181818,
           0x00202020,
           0x00282828,
           0x00303030,
           0x00383838,
           0x00404040,
           0x00484848,
           0x00505050,
           0x00585858,
           0x00606060,
           0x00686868,
           0x00707070,
           0x00787878,
           0x00808080,
           0x00888888,
           0x00909090,
           0x00989898,
           0x00A0A0A0,
           0x00A8A8A8,
           0x00B0B0B0,
           0x00B8B8B8,
           0x00C0C0C0,
           0x00C8C8C8,
           0x00D0D0D0,
           0x00D8D8D8,
           0x00E0E0E0,
           0x00E8E8E8,
           0x00F0F0F0,
           0x00FFFFFF
    };
}