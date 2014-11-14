#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN

// c++ headers
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <queue>
#include <array>
#include <map>
#include <unordered_map>
#include <memory>
#include <locale>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <functional>
#include <regex>
#include <xiosbase>
#include <iomanip>
#include <ctime>
#include <locale>
#include <ctime>
#include <cstdlib>
#include <type_traits>
#include <iterator>
#include <random>

// windows headers
#include <windows.h>
#include <WindowsX.h>
#include <WinUser.h>
#include <wincodec.h>
#include <Richedit.h>
#include <Ole2.h>
#include <OCIdl.h>
#include <manipulations.h>
#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include <wincodec.h>
#include <atlbase.h>
#include <atlcom.h>
#include <initguid.h>
#include <UIRibbon.h>
#include <UIRibbonPropertyHelpers.h>
#include <UIAnimation.h>
#include <comutil.h>
#include <mshtmcid.h>
#include <msctf.h>
#include <OleCtl.h>
#include <initguid.h>
#include <tsattrs.h>
#include <d3d10_1.h>
#include <DirectXMath.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shellapi.h>
#include <Uxtheme.h>
#include <knownfolders.h>
#include <shlguid.h>
#include <xpsobjectmodel.h>
#include <XpsPrint.h>
#include <Prntvpt.h>
#include <commdlg.h>

// There are definitions that are conflicts in stdint.h and intsafe.h
#ifdef INT8_MIN
#undef INT8_MIN
#endif
#ifdef INT8_MAX
#undef INT8_MAX
#endif
#ifdef INT16_MIN
#undef INT16_MIN
#endif
#ifdef INT16_MAX
#undef INT16_MAX
#endif
#ifdef INT32_MIN
#undef INT32_MIN
#endif
#ifdef INT32_MAX
#undef INT32_MAX
#endif
#ifdef INT64_MIN
#undef INT64_MIN
#endif
#ifdef INT64_MAX
#undef INT64_MAX
#endif

#ifdef UINT8_MIN
#undef UINT8_MIN
#endif
#ifdef UINT8_MAX
#undef UINT8_MAX
#endif
#ifdef UINT16_MIN
#undef UINT16_MIN
#endif
#ifdef UINT16_MAX
#undef UINT16_MAX
#endif
#ifdef UINT32_MIN
#undef UINT32_MIN
#endif
#ifdef UINT32_MAX
#undef UINT32_MAX
#endif
#ifdef UINT64_MIN
#undef UINT64_MIN
#endif
#ifdef UINT64_MAX
#undef UINT64_MAX
#endif
#include <stdint.h>

#include "touchmind/logging/Logging.h"

// SafeAcquire

#ifndef __SAFEACQUIRE
#define __SAFEACQUIRE

template <typename InterfaceType>
inline InterfaceType* SafeAcquire(InterfaceType* newObject)
{
    if (newObject != nullptr)
        newObject->AddRef();

    return newObject;
}

#endif // __SAFEACQUIRE

// SafeRelease

#ifndef __SAFERELEASE
#define __SAFERELEASE

template<class Interface>
inline void SafeRelease(
    Interface **ppInterfaceToRelease
)
{
    if (*ppInterfaceToRelease != nullptr) {
#ifdef DEBUG_GPU_RESOURCE
        LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] Release [" << std::hex << *ppInterfaceToRelease << L"]" << std::dec;
#endif
        (*ppInterfaceToRelease)->Release();

        (*ppInterfaceToRelease) = nullptr;
    }
}

#endif // __SAFERELEASE

// Assert
#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#define Assert(b) do {if (!(b)) {OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif //DEBUG || _DEBUG
#endif

#include <assert.h>
#define ASSERT assert

// HINST_THISCOMPONENT
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

// MSXML
#ifndef __RENAME_MSXML
#define __RENAME_MSXML
#import "MSXML6.dll" rename_namespace(_T("MSXML"))
#endif
#include <msxml6.h>

#undef min
#undef max
#undef GetFirstChild

#pragma warning(disable: 4714) // disable inline warning
#pragma warning(disable: 4481) // disable override warning
