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

#ifdef NOOPTIMIZATION
#pragma optimize("", off)
#endif

#define ORIGINAL_SOUND_MODULE_NAME_PATTERN TRUE /* ORIGINAL: TRUE */
#define ORIGINAL_RENDERER_MODULE_NAME_PATTERN TRUE /* ORIGINAL: TRUE */

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

#if _MSC_VER <= 1200
typedef unsigned __int64 u64;
#else
typedef unsigned long long u64;
#endif

typedef signed char s8;
typedef short s16;
typedef int s32;

#if _MSC_VER <= 1200
typedef __int64 s64;
#else
typedef long long s64;
#endif

typedef float f32;
typedef double f64;

typedef int BOOL;

#ifdef _WIN64
typedef unsigned long long addr;
#else
typedef unsigned int addr;
#endif

struct f32x2 { f32 X, Y; };
struct f32x3 { f32 X, Y, Z; };
struct f32x4 { f32 X, Y, Z, W; };

struct f64x2 { f64 X, Y; };
struct f64x3 { f64 X, Y, Z; };
struct f64x4 { f64 X, Y, Z, W; };

struct u32x2 { u32 X, Y; };
struct u32x3 { u32 X, Y, Z; };
struct u32x4 { u32 X, Y, Z, W; };

#define DLLAPI extern "C"

#define STDCALLAPI __stdcall
#define CDECLAPI __cdecl

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define FALSE 0
#define TRUE 1

#define NULL 0

#define U8_MIN 0
#define U8_MAX 256

#define S16_MIN (-32768)
#define S16_MAX 32767

#define U16_MIN 0
#define U16_MAX 65535

#define S32_MIN (-2147483648)
#define S32_MAX 2147483647

#define U32_MIN 0U
#define U32_MAX 4294967295U

#define F32_MIN (1.1754943508e-38f)
#define F32_MAX (3.4028234664e+38f)

#if defined(__WATCOMC__) || _MSC_VER <= 1200
#define vsnprintf_s _vsnprintf
#endif

#if !defined(__WATCOMC__) && _MSC_VER <= 1200
#define roundf(x) (x >= 0.0f ? floorf(x + 0.5f) : ceilf(x - 0.5f))
#endif

#if defined(__WATCOMC__)
#define roundf round
#endif