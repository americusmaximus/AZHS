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

#include "RendererModule.Basic.hxx"
#include "RendererModule.Import.hxx"

namespace RendererModule
{
    struct RendererModuleContainer
    {
        struct
        {
            HMODULE* _Handle; // 0x005c3d18
        } Module;

        RENDERERACQUIREDESCRIPTOR* _AcquireDescriptor; // 0x009e801c
        RENDERERCLEARGAMEWINDOW* _ClearGameWindow; // 0x009e8010
        RENDERERCLIPGAMEWINDOW* _ClipGameWindow; // 0x009e7fcc
        RENDERERDRAWLINE* _DrawLine; // 0x009e7fc8
        RENDERERDRAWLINEMESH* _DrawLineMesh; // 0x009e7fac
        RENDERERDRAWLINESTRIP* _DrawLineStrip; // 0x009e7fa8
        RENDERERDRAWPOINT* _DrawPoint; // 0x009e7fb0
        RENDERERDRAWPOINTSTRIP* _DrawPointStrip; // 0x009e7fa4
        RENDERERDRAWPOINTMESH* _DrawPointMesh; // 0x009e7fa0
        RENDERERDRAWQUAD* _DrawQuad; // 0x009e7fec
        RENDERERDRAWQUADMESH* _DrawQuadMesh; // 0x009e7fd0
        RENDERERDRAWTRIANGLE* _DrawTriangle; // 0x009e8014
        RENDERERDRAWTRIANGLEFAN* _DrawTriangleFan; // 0x009e8000
        RENDERERDRAWTRIANGLEMESH* _DrawTriangleMesh; // 0x009e7fc4
        RENDERERDRAWTRIANGLESTRIP* _DrawTriangleStrip; // 0x009e7fbc
        RENDERERFLUSHGAMEWINDOW* _FlushGameWindow; // 0x009e7ff0
        RENDERERINIT* _Init; // 0x009e8018
        RENDERERIS* _Is; // 0x009e8020
        RENDERERLOCKGAMEWINDOW* _LockGameWindow; // 0x009e7ff4
        RENDERERTOGGLEGAMEWINDOW* _ToggleGameWindow; // 0x009e7fe0
        RENDERERREADRECTANGLE* _ReadRectangle; // 0x009e7fc0
        RENDERERRESTOREGAMEWINDOW* _RestoreGameWindow; // 0x009e7fd4
        RENDERERSELECTDEVICE* _SelectDevice; // 0x009e7fe4
        RENDERERSELECTSTATE* _SelectState; // 0x009e800c
        RENDERERSELECTTEXTURE* _SelectTexture; // 0x009e7fd8
        RENDERERSELECTVIDEOMODE* _SelectVideoMode; // 0x009e7ff8
        RENDERERSYNCGAMEWINDOW* _SyncGameWindow; // 0x009e7fe8
        RENDERERALLOCATETEXTURE* _AllocateTexture; // 0x009e8008
        RENDERERRESETTEXTURES* _ResetTextures; // 0x009e7fb8
        RENDERERUPDATETEXTURE* _UpdateTexture; // 0x009e8004
        RENDERERUNLOCKGAMEWINDOW* _UnlockGameWindow; // 0x009e7ffc
        RENDERERSELECTGAMEWINDOW* _SelectGameWindow; // 0x009e7fdc
        RENDERERWRITERECTANGLE* _WriteRectangle; // 0x009e7fb4
    };

    extern RendererModuleContainer RendererModuleState;

    BOOL InitializeRendererModule(const char* name);
}