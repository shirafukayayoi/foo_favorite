#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cassert>

#ifndef FB2K_DEPRECATED
#if defined(_MSC_VER)
#define FB2K_DEPRECATED __declspec(deprecated)
#else
#define FB2K_DEPRECATED __attribute__((deprecated))
#endif
#endif

typedef float audio_sample;
using audio_math = pfc::audio_math;

namespace fb2k
{
#ifdef _WIN32
    using hwnd_t = HWND;
    using hmenu_t = HMENU;
    using hicon_t = HICON;
    using hfont_t = HFONT;
#else
    using hwnd_t = void *;
    using hmenu_t = void *;
    using hicon_t = void *;
    using hfont_t = void *;
#endif
}

inline void FB2K_BugCheckEx(const char *)
{
#ifdef _WIN32
    __debugbreak();
#endif
    std::abort();
}

inline void uBugCheck()
{
    FB2K_BugCheckEx("uBugCheck");
}

inline void FB2K_BugCheck()
{
    FB2K_BugCheckEx("FB2K_BugCheck");
}

inline int stricmp_utf8(const char *s1, const char *s2)
{
    return pfc::stringCompareCaseInsensitive(s1, s2);
}

inline int stricmp_utf8_ex(const char *s1, size_t n1, const char *s2, size_t n2)
{
    return pfc::stringCompareCaseInsensitiveEx({s1, n1}, {s2, n2});
}

inline pfc::eventHandle_t GetInfiniteWaitEvent()
{
    return nullptr;
}
