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

// NOTES
// It turns out that the original game was compiled with Watcom compiler,
// which had an interesting feature not present in modern Visual C++ linker,
// which is an ability to export extern "C" functions with overrides with the same name.
// 
// Examples include:
// _THRASH_drawlinestrip@8
// _THRASH_drawpointmesh@12
//
// I researched ways to achieve the same with Visual C++,
// unfortunately it looks like this ability was not picked up by other compilers and linkers,
// therefore, given the need to preserve binary compatibility of the renderers with the original game,
// modification the export symbol names is the way.
//
// This is achieved by putting # character into the .def file, and then using the tool
// through the post-build event chain to replace # with @.

#include "Watcom.hxx"

#include <stdio.h>

#define REQUIRED_ARGUMENT_COUNT 2
#define PATH_ARGUMENT_INDEX 1

BOOL FileExists(const char* path)
{
    const int attrs = GetFileAttributesA(path);

    return (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
}

int main(int argc, char* argv[])
{
    printf("Starting Watcom export symbol post-processor...\n");

    if (argc != REQUIRED_ARGUMENT_COUNT)
    {
        printf("File path not provided.\n");
        printf("Usage: <watcom>.exe <Full path of PE File>\n");

        return FAILURE_INVALID_ARGUMENT_COUNT;
    }

    printf("Loading %s...\n", argv[PATH_ARGUMENT_INDEX]);

    if (!FileExists(argv[PATH_ARGUMENT_INDEX]))
    {
        printf("ERROR: Unable to load %s...\n",
            argv[PATH_ARGUMENT_INDEX]); return FAILURE_READING_FILE;
    }

    const HANDLE file = CreateFileA(argv[PATH_ARGUMENT_INDEX], GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (file == INVALID_HANDLE_VALUE)
    {
        printf("ERROR: Unable to load %s...\n", argv[PATH_ARGUMENT_INDEX]);
        return FAILURE_READING_FILE;
    }

    const int result = HandleFile(file);

    printf("Closing %s...\n", argv[PATH_ARGUMENT_INDEX]);

    CloseHandle(file);

    return result;
}