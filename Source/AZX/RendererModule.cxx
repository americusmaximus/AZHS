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

#include "RendererModule.hxx"

#define MAX_RENDERER_MODULE_NAME_LENGTH 512

#include <stdio.h>

namespace RendererModule
{
    RendererModuleContainer RendererModuleState;

    // 0x0055a580
    BOOL InitializeRendererModule(const char* name)
    {
        *RendererModuleState.Module._Handle = LoadLibraryA(name);

        if (*RendererModuleState.Module._Handle == NULL)
        {
            char buffer[MAX_RENDERER_MODULE_NAME_LENGTH];

            sprintf(buffer, "%sa", name);

            *RendererModuleState.Module._Handle = LoadLibraryA(buffer);
        }

        if (*RendererModuleState.Module._Handle != NULL)
        {
            *RendererModuleState._AcquireDescriptor = (RENDERERACQUIREDESCRIPTOR)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_ACQUIRE_DESCRIPTOR_NAME);
            *RendererModuleState._ClearGameWindow = (RENDERERCLEARGAMEWINDOW)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_CLEAR_GAME_WINDOW_NAME);
            *RendererModuleState._ClipGameWindow = (RENDERERCLIPGAMEWINDOW)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_CLIP_GAME_WINDOW_NAME);
            *RendererModuleState._DrawLine = (RENDERERDRAWLINE)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_DRAW_LINE_NAME);
            *RendererModuleState._DrawLineMesh = (RENDERERDRAWLINEMESH)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_DRAW_LINE_MESH_NAME);
            *RendererModuleState._DrawLineStrip = (RENDERERDRAWLINESTRIP)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_DRAW_LINE_STRIP_NAME);
            *RendererModuleState._DrawPoint = (RENDERERDRAWPOINT)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_DRAW_POINT_NAME);
            *RendererModuleState._DrawPointStrip = (RENDERERDRAWPOINTSTRIP)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_DRAW_POINT_STRIP_NAME);
            *RendererModuleState._DrawPointMesh = (RENDERERDRAWPOINTMESH)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_DRAW_POINT_MESH_NAME);
            *RendererModuleState._DrawQuad = (RENDERERDRAWQUAD)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_DRAW_QUAD_NAME);
            *RendererModuleState._DrawQuadMesh = (RENDERERDRAWQUADMESH)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_DRAW_QUAD_MESH_NAME);
            *RendererModuleState._DrawTriangle = (RENDERERDRAWTRIANGLE)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_DRAW_TRIANGLE_NAME);
            *RendererModuleState._DrawTriangleFan = (RENDERERDRAWTRIANGLEFAN)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_DRAW_TRIANGLE_FAN_NAME);
            *RendererModuleState._DrawTriangleMesh = (RENDERERDRAWTRIANGLEMESH)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_DRAW_TRIANGLE_MESH_NAME);
            *RendererModuleState._DrawTriangleStrip = (RENDERERDRAWTRIANGLESTRIP)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_DRAW_TRIANGLE_STRIP_NAME);
            *RendererModuleState._FlushGameWindow = (RENDERERFLUSHGAMEWINDOW)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_FLUSH_GAME_WINDOW_NAME);
            *RendererModuleState._Init = (RENDERERINIT)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_INIT_NAME);
            *RendererModuleState._Is = (RENDERERIS)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_IS_NAME);
            *RendererModuleState._LockGameWindow = (RENDERERLOCKGAMEWINDOW)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_LOCK_GAME_WINDOW_NAME);
            *RendererModuleState._ToggleGameWindow = (RENDERERTOGGLEGAMEWINDOW)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_TOGGLE_GAME_WINDOW_NAME);
            *RendererModuleState._ReadRectangle = (RENDERERREADRECTANGLE)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_READ_RECTANGLE_NAME);
            *RendererModuleState._RestoreGameWindow = (RENDERERRESTOREGAMEWINDOW)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_RESTORE_GAME_WINDOW_NAME);
            *RendererModuleState._SelectDevice = (RENDERERSELECTDEVICE)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_SELECT_DEVICE_NAME);
            *RendererModuleState._SelectState = (RENDERERSELECTSTATE)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_SELECT_STATE_NAME);
            *RendererModuleState._SelectTexture = (RENDERERSELECTTEXTURE)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_SELECT_TEXTURE_NAME);
            *RendererModuleState._SelectVideoMode = (RENDERERSELECTVIDEOMODE)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_SELECT_VIDEO_MODE);
            *RendererModuleState._SyncGameWindow = (RENDERERSYNCGAMEWINDOW)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_SYNC_GAME_WINDOW);
            *RendererModuleState._AllocateTexture = (RENDERERALLOCATETEXTURE)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_ALLOCATE_TEXTURE_NAME);
            *RendererModuleState._ResetTextures = (RENDERERRESETTEXTURES)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_RESET_TEXTURES_NAME);
            *RendererModuleState._UpdateTexture = (RENDERERUPDATETEXTURE)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_UPDATE_TEXTURE_NAME);
            *RendererModuleState._UnlockGameWindow = (RENDERERUNLOCKGAMEWINDOW)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_UNLOCK_GAME_WINDOW_NAME);
            *RendererModuleState._SelectGameWindow = (RENDERERSELECTGAMEWINDOW)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_SELECT_GAME_WINDOW_NAME);
            *RendererModuleState._WriteRectangle = (RENDERERWRITERECTANGLE)GetProcAddress(*RendererModuleState.Module._Handle, RENDERER_MODULE_WRITE_RECTANGLE_NAME);

            if (*RendererModuleState._SelectState != NULL)
            {
                (*RendererModuleState._SelectState)(RENDERER_MODULE_STATE_SELECT_VERSION, (void*)RENDERER_MODULE_VERSION_104);
            }
        }

        return *RendererModuleState.Module._Handle != NULL;
    }
}