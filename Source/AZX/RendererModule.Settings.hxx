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

#define RENDERER_MODULE_SETTINGS_FILE_NAME ".\\azhs.ini"

#define RENDERER_MODULE_SETTINGS_SECTION_DX6_NAME "DX6"
#define RENDERER_MODULE_SETTINGS_SECTION_DX7_NAME "DX7"
#define RENDERER_MODULE_SETTINGS_SECTION_DX8_NAME "DX8"

// Indicates whether flat shading is active/enabled.
// In practice means whether to overwrite the vertex color with white color or not.
// In my case enabling this leads to disappearance of transparency in the game.
// DEFAULT: TRUE
#define RENDERER_MODULE_SETTINGS_FLAT_SHADING_PROPERTY_NAME "FlatShading"

// Indicates whether vertex buffer is used for rendering instead of just a pointer to an array of vertexes.
// In practice, on some systems, like mine, leads to a drastic performance drop, thus making this a configuration item.
// DEFAULT: TRUE
#define RENDERER_MODULE_SETTINGS_VERTEX_BUFFER_PROPERTY_NAME "VertexBuffer"

// Indicates whether the hardware accelerated device should be picked first, despite the command from the game.
// DEFAULT: FALSE
#define RENDERER_MODULE_SETTINGS_ACCELERATE_GRAPHICS_PROPERTY_NAME "Accelerate"