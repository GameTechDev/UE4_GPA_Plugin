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

#include "igpa-config.h"

#include <string>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <map>
#include <list>
#include <string>
#include <functional>
#include <sstream>
#include <cstdint>

namespace {
// Windows APIs can handle paths longer than MAX_PATH with specific syntax.
// https://msdn.microsoft.com/en-us/library/930f87yf.aspx
constexpr size_t kMaxPathLength = 16384;  // Lowered from 32k because of Dota2 lower stack size

// The actual limit is a lot higher (can be obtained by winapi call)
// however we don't need more than 64 chars for now
constexpr size_t kMaxWindowTitleLength = 64;
}  // namespace

namespace gpa {
namespace utility {

enum {
    kNameAndArgsSep = ':',
    kEntryDelimiter = ',',
    kKeyValueSeparator = '='
};

enum HelpTextOutputMode {
    PLAIN,
    MARKDOWN,
    RESTRUCTURED_TEXT
};

enum FileFormat {
    JPG,
    PNG,
    BMP
};

enum HookApiFlagBits : uint32_t {
    kHookD3D10 = 1,
    kHookD3D11 = 1 << 1,
    kHookD3D12 = 1 << 2,
    kHookVulkan = 1 << 3,
    kHookMetal = 1 << 4,
    kHookWin32 = 1 << 10,
    kHookD3D = kHookD3D10 | kHookD3D11 | kHookD3D12,
    kHookAll = UINT32_MAX
};
typedef uint32_t HookApiFlags;

enum InjectionMode : uint32_t {
    kSetThreadContext = 0,
    kCreateRemoteThread = 1
};

static constexpr int kMaxLayers = 16;
static constexpr int kLayerNameLength = 64;
static constexpr int kLogLevelLength = 16;
static constexpr int kLogFilenameLength = 2 * 1024;
static constexpr int kMaxProcesses = 128;
static constexpr int kMaxLayerArgs = 16;
static constexpr int kMaxLayerArgLength = 512;

struct KeyValPair
{
    TCHAR key[utility::kMaxLayerArgLength];
    TCHAR value[utility::kMaxLayerArgLength];
    KeyValPair() {}
    KeyValPair(TCHAR const inKey[utility::kMaxLayerArgLength],
               TCHAR const inValue[utility::kMaxLayerArgLength])
    {
        TSCPY(key, inKey, utility::kMaxLayerArgLength - 1);
        TSCPY(value, inValue, utility::kMaxLayerArgLength - 1);
    }
};

struct LayerArgInfo
{
    TCHAR const *argName;
    TCHAR const *argDescription;
    TCHAR const *argType;
};

struct LayerInfo
{
    TCHAR const *layerName;
    TCHAR const *layerUsage;
    TCHAR const *miscInformation;
};

enum RequiredLayerOrder {
    kBefore,  //<! The required layer will be loaded before the dependent layer (required layer will be earlier in the layer stack than dependent layer)
    kAfter    //<! The required layer will be loaded after the dependent layer (dependent layer will be earlier in the layer stack than required layer)
};

struct RequiredLayer
{
    TCHAR const *layerName;         //<! Name of requested layer
    RequiredLayerOrder layerOrder;  //<! See @RequiredLayerOrder
    size_t layerArgsCount;          //<! Number of arguments for requested layer
    KeyValPair const *layerArgs;    //<! Arguments for requested layer
};

size_t ParseKeyValuePairsString(STD_STRING stringToParse, std::vector<KeyValPair> &keyValPairs, size_t maxPairs);
void SplitRawLayerString(STD_STRING rawLayerString, STD_STRING &layerName, STD_STRING &layerArgsString);
void CopyKeyValuePair(TCHAR *keyDest, TCHAR const *keySrc, TCHAR *valueDest, TCHAR const *valueSrc, size_t maxSize);

std::map<STD_STRING, std::list<STD_STRING>> CheckForExpectedLayerArgs(gpa::utility::KeyValPair const *keyValPairs, size_t keyValPairCount, LayerArgInfo *expectedArgsList, size_t numExpectedArgsList);
STD_STRING BuildHelpMessage(LayerInfo const &layerInfo, std::vector<LayerArgInfo> layerArgInfo, HelpTextOutputMode outputMode);

template<typename stringType, typename charType, typename callbackType>
void EnumerateSplitString(stringType const &in, charType delim, callbackType callback)
{
    stringType substring;
    std::basic_stringstream<charType, std::char_traits<charType>, std::allocator<charType>> ss(in);
    while (std::getline(ss, substring, delim)) {
        callback(substring);
    }
}

}  // namespace utility
}  // namespace gpa
