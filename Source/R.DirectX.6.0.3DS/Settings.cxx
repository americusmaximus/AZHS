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

#include "Native.Basic.hxx"
#include "Settings.hxx"

#ifdef __WATCOMC__
#include <RendererModule.Settings.hxx>
#else
#include "RendererModule.Settings.hxx"
#endif

namespace Settings
{
    SettingsContainer SettingsState;

    void InitializeSettings(void)
    {
        SettingsState.FlatShading = GetPrivateProfileIntA(RENDERER_MODULE_SETTINGS_SECTION_DX6_NAME,
            RENDERER_MODULE_SETTINGS_FLAT_SHADING_PROPERTY_NAME, TRUE, RENDERER_MODULE_SETTINGS_FILE_NAME);
        SettingsState.Accelerate = GetPrivateProfileIntA(RENDERER_MODULE_SETTINGS_SECTION_DX6_NAME,
            RENDERER_MODULE_SETTINGS_ACCELERATE_GRAPHICS_PROPERTY_NAME, FALSE, RENDERER_MODULE_SETTINGS_FILE_NAME);
    }
}