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

#include "Renderer.Basic.hxx"
#include "RendererModule.Basic.hxx"

namespace RendererModule
{
    DLLAPI RendererModuleDescriptor* STDCALLAPI AcquireDescriptor(void);
    DLLAPI u32 STDCALLAPI ClearGameWindow(void);
    DLLAPI u32 STDCALLAPI ClipGameWindow(const u32 x0, const u32 y0, const u32 x1, const u32 y1);
    DLLAPI u32 STDCALLAPI CreateGameWindow(const u32 width, const u32 height, const u32 format, const u32 options);
    DLLAPI u32 STDCALLAPI DestroyGameWindow(const u32 indx);
    DLLAPI void STDCALLAPI DrawLine(Renderer::RVX* a, Renderer::RVX* b);
    DLLAPI void STDCALLAPI DrawLineMesh(const u32 count, Renderer::RVX* vertexes, const u32* indexes);
    DLLAPI void STDCALLAPI DrawLineStrip(const u32 count, Renderer::RVX* vertexes);
    DLLAPI void STDCALLAPI DrawLineStrips(const u32 count, Renderer::RVX* vertexes, const u32* indexes);
    DLLAPI void STDCALLAPI DrawPoint(Renderer::RVX* vertex);
    DLLAPI void STDCALLAPI DrawPointMesh(const u32 count, Renderer::RVX* vertexes, const u32* indexes);
    DLLAPI void STDCALLAPI DrawPointStrip(const u32 count, Renderer::RVX* vertexes);
    DLLAPI void STDCALLAPI DrawQuad(Renderer::RVX* a, Renderer::RVX* b, Renderer::RVX* c, Renderer::RVX* d);
    DLLAPI void STDCALLAPI DrawQuadMesh(const u32 count, Renderer::RVX* vertexes, const u32* indexes);
    DLLAPI void STDCALLAPI DrawSprite(Renderer::RVX* a, Renderer::RVX* b);
    DLLAPI void STDCALLAPI DrawSpriteMesh(const u32 count, Renderer::RVX* vertexes, const u32* indexes);
    DLLAPI void STDCALLAPI DrawTriangle(Renderer::RVX* a, Renderer::RVX* b, Renderer::RVX* c);
    DLLAPI void STDCALLAPI DrawTriangleFan(const u32 count, Renderer::RVX* vertexes);
    DLLAPI void STDCALLAPI DrawTriangleFans(const u32 count, Renderer::RVX* vertexes, const u32* indexes);
    DLLAPI void STDCALLAPI DrawTriangleMesh(const u32 count, Renderer::RVX* vertexes, const u32* indexes);
    DLLAPI void STDCALLAPI DrawTriangleStrip(const u32 count, Renderer::RVX* vertexes);
    DLLAPI void STDCALLAPI DrawTriangleStrips(const u32 count, Renderer::RVX* vertexes, const u32* indexes);
    DLLAPI u32 STDCALLAPI FlushGameWindow(void);
    DLLAPI u32 STDCALLAPI AcquireState(const u32 state);
    DLLAPI Renderer::RendererTexture* STDCALLAPI AcquireGameWindowTexture(const u32 indx);
    DLLAPI void STDCALLAPI Idle(void);
    DLLAPI u32 STDCALLAPI Init(void);
    DLLAPI u32 STDCALLAPI Is(void);
    DLLAPI RendererModuleWindowLock* STDCALLAPI LockGameWindow(void);
    DLLAPI u32 STDCALLAPI ToggleGameWindow(void);
    DLLAPI u32 STDCALLAPI ReadRectangle(const u32 x, const u32 y, const u32 width, const u32 height, u32* pixels);
    DLLAPI u32 STDCALLAPI ReadRectangles(void*, void*, void*, void*, void*, void*); // TODO
    DLLAPI u32 STDCALLAPI RestoreGameWindow(void);
    DLLAPI u32 STDCALLAPI SelectDevice(const s32 indx);
    DLLAPI u32 STDCALLAPI SelectState(const u32 state, void* value);
    DLLAPI u32 STDCALLAPI SelectTexture(Renderer::RendererTexture* tex);
    DLLAPI u32 STDCALLAPI SelectVideoMode(const u32 mode, const u32 pending, const u32 depth);
    DLLAPI u32 STDCALLAPI SyncGameWindow(const u32 type);
    DLLAPI Renderer::RendererTexture* STDCALLAPI AllocateTexture(const u32 width, const u32 height, const u32 format, const u32 options, const u32 state);
    DLLAPI u32 STDCALLAPI ReleaseTexture(Renderer::RendererTexture* tex);
    DLLAPI u32 STDCALLAPI ResetTextures(void);
    DLLAPI Renderer::RendererTexture* STDCALLAPI UpdateTexture(Renderer::RendererTexture* tex, const u32* pixels, const u32* palette);
    DLLAPI Renderer::RendererTexture* STDCALLAPI UpdateTextureRectangle(Renderer::RendererTexture* tex, const u32* pixels, const u32* palette, const s32 x, const s32 y, const s32 width, const s32 height, const s32 size, const s32 level);
    DLLAPI u32 STDCALLAPI UnlockGameWindow(const RendererModuleWindowLock* state);
    DLLAPI u32 STDCALLAPI SelectGameWindow(const u32 indx);
    DLLAPI u32 STDCALLAPI WriteRectangle(const u32 x, const u32 y, const u32 width, const u32 height, const u32* pixels);
    DLLAPI u32 STDCALLAPI WriteRectangles(void*, void*, void*, void*, void*, void*); // TODO
}