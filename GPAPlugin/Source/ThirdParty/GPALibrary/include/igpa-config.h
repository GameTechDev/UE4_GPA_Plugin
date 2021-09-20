/******************************************************************************
© Intel Corporation.
 
This software and the related documents are Intel copyrighted materials,
and your use of them is governed by the express license under which they
were provided to you ("License"). Unless the License provides otherwise,
you may not use, modify, copy, publish, distribute, disclose or transmit
this software or the related documents without Intel's prior written
permission.


 This software and the related documents are provided as is, with no express
or implied warranties, other than those that are expressly stated in the
License.

******************************************************************************/
#pragma once

/* #undef HAVE_PTHREAD_H */
#define HAVE_VULKAN 1
/* #undef HAVE_METAL */
#define HAVE_DIRECTX 1
/* #undef HAVE_CPATHAPI */
/* #undef HAVE_COMPATHAPI */
/* #undef HAVE_OBJCPATHAPI */

#if defined(HAVE_VULKAN)
    #if defined(__ANDROID__)
        #define VK_USE_PLATFORM_ANDROID_KHR 1
    #elif defined(_WIN32) || defined(_WIN64)
        #define VK_USE_PLATFORM_WIN32_KHR 1
    #elif defined(__linux__)
        #define VK_USE_PLATFORM_XLIB_KHR 1
        #define VK_USE_PLATFORM_XCB_KHR 1
        #define VK_USE_PLATFORM_WAYLAND_KHR 1
    #elif defined(__APPLE__)
        #define VK_USE_PLATFORM_MACOS_MVK 1
    #endif
    #ifndef VK_ENABLE_BETA_EXTENSIONS
    #define VK_ENABLE_BETA_EXTENSIONS 1
    #endif
#endif /* HAVE_VULKAN */

#if defined(UNICODE) || defined(_UNICODE)
    #define GPA_UNICODE
#endif

#if defined(_WIN32)
    #if defined(GPA_EXPORTS)
        #define GPA_EXPORT extern "C" __declspec(dllexport)
        #define GPA_CPP_EXPORT extern __declspec(dllexport)
        #define GPA_CLASS_EXPORT __declspec(dllexport)
    #else
        #define GPA_EXPORT extern "C" __declspec(dllimport)
        #define GPA_CPP_EXPORT extern __declspec(dllimport)
        #define GPA_CLASS_EXPORT __declspec(dllimport)
    #endif

    // for layers, we don't want to re-use GPA_EXPORT[S] (because this
    // can cause issues for symbols exported from DLLs that the layer use)
    // and instead define macros specifically for layers
    #if defined(GPA_LAYER_EXPORTS)
        #define GPA_LAYER_EXPORT extern "C" __declspec(dllexport)
    #else
        #define GPA_LAYER_EXPORT extern "C" __declspec(dllimport)
    #endif

    #if defined(GPA_CAPTURE_SUPPORT_EXPORTS)
        #define GPA_CAPTURE_SUPPORT_EXPORT extern "C" __declspec(dllexport)
    #else
        #define GPA_CAPTURE_SUPPORT_EXPORT extern "C" __declspec(dllimport)
    #endif

    #if defined(GPA_FUNCTIONAL_TEST_SUPPORT_EXPORTS)
        #define GPA_FUNCTIONAL_TEST_SUPPORT_EXPORT extern "C" __declspec(dllexport)
    #else
        #define GPA_FUNCTIONAL_TEST_SUPPORT_EXPORT extern "C" __declspec(dllimport)
    #endif

    #include <tchar.h>

    #define STD_SSTREAM std::basic_stringstream<TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR>>

    #if defined(UNICODE) || defined(_UNICODE)
        #include <wctype.h>
        #include <string>

        #define STD_STRING std::wstring
        #define STD_TO_STRING std::to_wstring
        #define SPRINTF(dst, count, fmt, ...) swprintf_s(dst, count, fmt, __VA_ARGS__)
        #define STRCMP(a, b) wcscmp(a, b)
        #define ATOI(str) _wtoi(str)
        #define TCHAR_FORMAT_SPECIFIER "%ls"
        #define TO_STRING std::to_wstring
        #define TO_LOWER(c) towlower(c)
        #define STRTOD(str, endptr) wcstod(str, endptr)
        #define STRRCHR(str, c) wcsrchr(str, c)
        #define TSCANF(str, fmt, ...) swscanf(str, fmt, __VA_ARGS__)
    #else
        #include <ctype.h>

        #define STD_STRING std::string
        #define STD_TO_STRING std::to_string
        #define SPRINTF(dst, count, fmt, ...) sprintf_s sprintf_s(dst, count, fmt, __VA_ARGS__)
        #define STRCMP(a, b) strcmp(a, b)
        #define ATOI(str) atoi(str)
        #define TCHAR_FORMAT_SPECIFIER "%s"
        #define TO_STRING std::to_string
        #define TO_LOWER(c) tolower(c)
        #define STRTOD(str, endptr) strtod(str, endptr)
        #define STRRCHR(str, c) strrchr(str, c)
        #define TSCANF(str, fmt, ...) sscanf(str, fmt, __VA_ARGS__)
    #endif
    #define STRTOUL(str, endPtr, base) _tcstoul(str, endPtr, base);
    #define STRLEN(str) _tcslen(str)
    #define TSCPY(dst, src, size) _tcscpy_s(dst, size, src)
    #define STRCPY(dst, src, dstSize) strncpy_s(dst, dstSize, src, dstSize - 1)
#else
    #if defined(__APPLE__)
        #define GPA_EXPORT extern "C" __attribute__((visibility("default")))
        #define GPA_LAYER_EXPORT extern "C" __attribute__((visibility("default")))
        #define GPA_CAPTURE_SUPPORT_EXPORT extern "C" __attribute__((visibility("default")))
        #define GPA_FUNCTIONAL_TEST_SUPPORT_EXPORT extern "C" __attribute__((visibility("default")))
        #define GPA_CPP_EXPORT __attribute__((visibility("default")))
        #define GPA_CLASS_EXPORT __attribute__((visibility("default")))
    #else
        #define GPA_EXPORT extern "C" __attribute__((visibility("default")))
        #define GPA_LAYER_EXPORT extern "C" __attribute__((visibility("default")))
        #define GPA_CAPTURE_SUPPORT_EXPORT extern "C" __attribute__((visibility("default")))
        #define GPA_FUNCTIONAL_TEST_SUPPORT_EXPORT extern "C" __attribute__((visibility("default")))
        #define GPA_CPP_EXPORT __attribute__((visibility("default")))
        #define GPA_CLASS_EXPORT __attribute__((visibility("default")))
    #endif

    #include <ctype.h>
    #include <stdio.h>
    #include <string.h>
    #include <string>

    #if defined(__APPLE__)
        #define TSCPY(dst, src, size) strlcpy(dst, src, size)
        #define STRCPY(dst, src, dstSize) strlcpy(dst, src, dstSize)
    #else
        #define TSCPY(dst, src, size) strncpy(dst, src, size)
        #define STRCPY(dst, src, dstSize) strncpy(dst, src, dstSize)
    #endif
    #define TCHAR char
    #define _T(str) str
    #define _tmain main
    #define STD_STRING std::string
    #define STD_TO_STRING std::to_string
    #define SPRINTF(dst, count, fmt, ...) snprintf(dst, count, fmt, __VA_ARGS__)
    #define STRCMP(a, b) strcmp(a, b)
    #define ATOI(str) atoi(str)
    #define TCHAR_FORMAT_SPECIFIER "%s"
    #define TO_STRING std::to_string
    #define TO_LOWER(c) tolower(c)
    #define STRTOUL(str, endPtr, base) strtoul(str, endPtr, base);
    #define STRLEN(str) strlen(str)
    #define STRTOD(str, endptr) strtod(str, endptr)
    #define STRRCHR(str, c) strrchr(str, c)
    #define TSCANF(str, fmt, ...) sscanf(str, fmt, __VA_ARGS__)

#endif


#if defined(GPA_NO_EXCEPTIONS)
    #define __GPA_NOEXCEPT
    #define __GPA_THROW
#else
    #define __GPA_NOEXCEPT noexcept
    #define __GPA_THROW throw()
#endif // GPA_NO_EXCEPTIONS

#if defined(NDEBUG)
    #define GPA_ASSERT_EQ(expected, actual) (void)actual
#else
    #define GPA_ASSERT_EQ(expected, actual) assert(expected == actual)
#endif // NDEBUG

#if defined(GPA_FRAMEWORK_BUILD)
    #define GPA_DEPRECATED(GPA_DEPRECATED_MESSAGE)
#else
    #define GPA_DEPRECATED(GPA_DEPRECATED_MESSAGE) [[deprecated(GPA_DEPRECATED_MESSAGE)]]
#endif // GPA_FRAMEWORK_BUILD

