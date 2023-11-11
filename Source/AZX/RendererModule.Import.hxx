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

#if ORIGINAL_RENDERER_MODULE_NAME_PATTERN
#define RENDERER_MODULE_ACQUIRE_DESCRIPTOR_NAME "_THRASH_about@0"
#define RENDERER_MODULE_CLEAR_GAME_WINDOW_NAME "_THRASH_clearwindow@0"
#define RENDERER_MODULE_CLIP_GAME_WINDOW_NAME "_THRASH_clip@16"
#define RENDERER_MODULE_DRAW_LINE_NAME "_THRASH_drawline@8"
#define RENDERER_MODULE_DRAW_LINE_MESH_NAME "_THRASH_drawlinemesh@12"
#define RENDERER_MODULE_DRAW_LINE_STRIP_NAME "_THRASH_drawlinestrip@8"
#define RENDERER_MODULE_DRAW_POINT_NAME "_THRASH_drawpoint@4"
#define RENDERER_MODULE_DRAW_POINT_STRIP_NAME "_THRASH_drawpointstrip@8"
#define RENDERER_MODULE_DRAW_POINT_MESH_NAME "_THRASH_drawpointmesh@12"
#define RENDERER_MODULE_DRAW_QUAD_NAME "_THRASH_drawquad@16"
#define RENDERER_MODULE_DRAW_QUAD_MESH_NAME "_THRASH_drawquadmesh@12"
#define RENDERER_MODULE_DRAW_TRIANGLE_NAME "_THRASH_drawtri@12"
#define RENDERER_MODULE_DRAW_TRIANGLE_FAN_NAME "_THRASH_drawtrifan@8"
#define RENDERER_MODULE_DRAW_TRIANGLE_MESH_NAME "_THRASH_drawtrimesh@12"
#define RENDERER_MODULE_DRAW_TRIANGLE_STRIP_NAME "_THRASH_drawtristrip@8"
#define RENDERER_MODULE_FLUSH_GAME_WINDOW_NAME "_THRASH_flushwindow@0"
#define RENDERER_MODULE_INIT_NAME "_THRASH_init@0"
#define RENDERER_MODULE_IS_NAME "_THRASH_is@0"
#define RENDERER_MODULE_LOCK_GAME_WINDOW_NAME "_THRASH_lockwindow@0"
#define RENDERER_MODULE_TOGGLE_GAME_WINDOW_NAME "_THRASH_pageflip@0"
#define RENDERER_MODULE_READ_RECTANGLE_NAME "_THRASH_readrect@20"
#define RENDERER_MODULE_RESTORE_GAME_WINDOW_NAME "_THRASH_restore@0"
#define RENDERER_MODULE_SELECT_DEVICE_NAME "_THRASH_selectdisplay@4"
#define RENDERER_MODULE_SELECT_STATE_NAME "_THRASH_setstate@8"
#define RENDERER_MODULE_SELECT_TEXTURE_NAME "_THRASH_settexture@4"
#define RENDERER_MODULE_SELECT_VIDEO_MODE "_THRASH_setvideomode@12"
#define RENDERER_MODULE_SYNC_GAME_WINDOW "_THRASH_sync@4"
#define RENDERER_MODULE_ALLOCATE_TEXTURE_NAME "_THRASH_talloc@20"
#define RENDERER_MODULE_RESET_TEXTURES_NAME "_THRASH_treset@0"
#define RENDERER_MODULE_UPDATE_TEXTURE_NAME "_THRASH_tupdate@12"
#define RENDERER_MODULE_UNLOCK_GAME_WINDOW_NAME "_THRASH_unlockwindow@4"
#define RENDERER_MODULE_SELECT_GAME_WINDOW_NAME "_THRASH_window@4"
#define RENDERER_MODULE_WRITE_RECTANGLE_NAME "_THRASH_writerect@20"
#else
#define RENDERER_MODULE_ACQUIRE_DESCRIPTOR_NAME "_THRASH_about"
#define RENDERER_MODULE_CLEAR_GAME_WINDOW_NAME "_THRASH_clearwindow"
#define RENDERER_MODULE_CLIP_GAME_WINDOW_NAME "_THRASH_clip"
#define RENDERER_MODULE_DRAW_LINE_NAME "_THRASH_drawline"
#define RENDERER_MODULE_DRAW_LINE_MESH_NAME "_THRASH_drawlinemesh"
#define RENDERER_MODULE_DRAW_LINE_STRIP_NAME "_THRASH_drawlinestrip"
#define RENDERER_MODULE_DRAW_POINT_NAME "_THRASH_drawpoint"
#define RENDERER_MODULE_DRAW_POINT_STRIP_NAME "_THRASH_drawpointstrip"
#define RENDERER_MODULE_DRAW_POINT_MESH_NAME "_THRASH_drawpointmesh"
#define RENDERER_MODULE_DRAW_QUAD_NAME "_THRASH_drawquad"
#define RENDERER_MODULE_DRAW_QUAD_MESH_NAME "_THRASH_drawquadmesh"
#define RENDERER_MODULE_DRAW_TRIANGLE_NAME "_THRASH_drawtri"
#define RENDERER_MODULE_DRAW_TRIANGLE_FAN_NAME "_THRASH_drawtrifan"
#define RENDERER_MODULE_DRAW_TRIANGLE_MESH_NAME "_THRASH_drawtrimesh"
#define RENDERER_MODULE_DRAW_TRIANGLE_STRIP_NAME "_THRASH_drawtristrip"
#define RENDERER_MODULE_FLUSH_GAME_WINDOW_NAME "_THRASH_flushwindow"
#define RENDERER_MODULE_INIT_NAME "_THRASH_init"
#define RENDERER_MODULE_IS_NAME "_THRASH_is"
#define RENDERER_MODULE_LOCK_GAME_WINDOW_NAME "_THRASH_lockwindow"
#define RENDERER_MODULE_TOGGLE_GAME_WINDOW_NAME "_THRASH_pageflip"
#define RENDERER_MODULE_READ_RECTANGLE_NAME "_THRASH_readrect"
#define RENDERER_MODULE_RESTORE_GAME_WINDOW_NAME "_THRASH_restore"
#define RENDERER_MODULE_SELECT_DEVICE_NAME "_THRASH_selectdisplay"
#define RENDERER_MODULE_SELECT_STATE_NAME "_THRASH_setstate"
#define RENDERER_MODULE_SELECT_TEXTURE_NAME "_THRASH_settexture"
#define RENDERER_MODULE_SELECT_VIDEO_MODE "_THRASH_setvideomode"
#define RENDERER_MODULE_SYNC_GAME_WINDOW "_THRASH_sync"
#define RENDERER_MODULE_ALLOCATE_TEXTURE_NAME "_THRASH_talloc"
#define RENDERER_MODULE_RESET_TEXTURES_NAME "_THRASH_treset"
#define RENDERER_MODULE_UPDATE_TEXTURE_NAME "_THRASH_tupdate"
#define RENDERER_MODULE_UNLOCK_GAME_WINDOW_NAME "_THRASH_unlockwindow"
#define RENDERER_MODULE_SELECT_GAME_WINDOW_NAME "_THRASH_window"
#define RENDERER_MODULE_WRITE_RECTANGLE_NAME "_THRASH_writerect"
#endif

namespace RendererModule
{
    typedef RendererModuleDescriptor* (STDCALLAPI* RENDERERACQUIREDESCRIPTOR)(void);
    typedef const u32(STDCALLAPI* RENDERERCLEARGAMEWINDOW)(void);
    typedef const u32(STDCALLAPI* RENDERERCLIPGAMEWINDOW)(const u32 x0, const u32 y0, const u32 x1, const u32 y1);
    typedef const u32(STDCALLAPI* RENDERERCREATEGAMEWINDOW)(void*, void*, void*, void*);
    typedef const u32(STDCALLAPI* RENDERERDESTROYGAMEWINDOW)(void*);
    typedef const void (STDCALLAPI* RENDERERDRAWLINE)(void* a, void* b);
    typedef const void (STDCALLAPI* RENDERERDRAWLINEMESH)(const u32 count, void*, void*);
    typedef const void (STDCALLAPI* RENDERERDRAWLINESTRIP)(const u32 count, void* vertexes);
    typedef const void (STDCALLAPI* RENDERERDRAWLINESTRIPS)(const u32 count, void*, void*);
    typedef const void (STDCALLAPI* RENDERERDRAWPOINT)(void* vertex);
    typedef const void (STDCALLAPI* RENDERERDRAWPOINTMESH)(const u32 count, void*, void*);
    typedef const void (STDCALLAPI* RENDERERDRAWPOINTSTRIP)(const u32 count, void* vertexes);
    typedef const void (STDCALLAPI* RENDERERDRAWQUAD)(void*, void*, void*, void*);
    typedef const void (STDCALLAPI* RENDERERDRAWQUADMESH)(void*, void*, void*);
    typedef const void (STDCALLAPI* RENDERERDRAWSPRITE)(void*, void*);
    typedef const void (STDCALLAPI* RENDERERDRAWSPRITEMESH)(void*, void*, void*);
    typedef const void (STDCALLAPI* RENDERERDRAWTRIANGLE)(void*, void*, void*);
    typedef const void (STDCALLAPI* RENDERERDRAWTRIANGLEFAN)(void*, void*);
    typedef const void (STDCALLAPI* RENDERERDRAWTRIANGLEFANS)(void*, void*, void*);
    typedef const void (STDCALLAPI* RENDERERDRAWTRIANGLEMESH)(void*, void*, void*);
    typedef const void (STDCALLAPI* RENDERERDRAWTRIANGLESTRIP)(void*, void*);
    typedef const void (STDCALLAPI* RENDERERDRAWTRIANGLESTRIPS)(void*, void*, void*);
    typedef const u32(STDCALLAPI* RENDERERFLUSHGAMEWINDOW)(void);
    typedef const u32(STDCALLAPI* RENDERERACQUIRESTATE)(const u32 options);
    typedef Renderer::RendererTexture* (STDCALLAPI* RENDERERACQUIREGAMEWINDOWTEXTURE)(const u32 indx);
    typedef const void(STDCALLAPI* RENDERERIDLE)(void);
    typedef const u32(STDCALLAPI* RENDERERINIT)(void);
    typedef const u32(STDCALLAPI* RENDERERIS)(void);
    typedef const RendererModuleWindowLock* (STDCALLAPI* RENDERERLOCKGAMEWINDOW)(void);
    typedef const u32(STDCALLAPI* RENDERERTOGGLEGAMEWINDOW)(void);
    typedef const u32(STDCALLAPI* RENDERERREADRECTANGLE)(const u32 x, const u32 y, const u32 width, const u32 height, u32* data);
    typedef const u32(STDCALLAPI* RENDERERRESTOREGAMEWINDOW)(void);
    typedef const u32(STDCALLAPI* RENDERERSELECTDEVICE)(const s32 indx);
    typedef const u32(STDCALLAPI* RENDERERSELECTSTATE)(const u32 state, void* value);
    typedef const u32(STDCALLAPI* RENDERERSELECTTEXTURE)(Renderer::RendererTexture* tex);
    typedef const u32(STDCALLAPI* RENDERERSELECTVIDEOMODE)(const u32 mode, const u32 pending, const u32 depth);
    typedef const u32(STDCALLAPI* RENDERERSYNCGAMEWINDOW)(const u32);
    typedef Renderer::RendererTexture* (STDCALLAPI* RENDERERALLOCATETEXTURE)(const u32 width, const u32 height, const u32 format, void*, const u32 options); // TODO
    typedef const u32(STDCALLAPI* RENDERERRELEASETEXTURE)(void*);
    typedef const u32(STDCALLAPI* RENDERERRESETTEXTURES)(void);
    typedef const u32(STDCALLAPI* RENDERERUPDATETEXTURE)(Renderer::RendererTexture* tex, void* data, const u32* palette);
    typedef const u32(STDCALLAPI* RENDERERUPDATETEXTURERECTANGLE)(void*, void*, void*, void*, void*, void*, void*, void*, void*);
    typedef const u32(STDCALLAPI* RENDERERUNLOCKGAMEWINDOW)(const RendererModuleWindowLock* state);
    typedef const u32(STDCALLAPI* RENDERERSELECTGAMEWINDOW)(const u32 indx);
    typedef const u32(STDCALLAPI* RENDERERWRITERECTANGLE)(const u32 x, const u32 y, const u32 width, const u32 height, const u32* data);
}