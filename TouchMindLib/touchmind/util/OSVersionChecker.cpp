#include "StdAfx.h"
#include <strsafe.h>
#include "touchmind/logging/Logging.h"
#include "touchmind/util/OSVersionChecker.h"
#include <VersionHelpers.h>

touchmind::util::OSVersion touchmind::util::OSVersionChecker::GetOSVersion()
{
	if (IsWindows7OrGreater()) {
		return OSVersion_Windows7;
	}
	else if (IsWindowsVistaOrGreater()) {
		return OSVersion_WindowsVista;
	}
	else {
		return OSVersion_WindowsXP;
	}
	/*
    OSVERSIONINFOEX osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if (!GetVersionEx((OSVERSIONINFO *) &osvi)) {
        return OSVersion_OlderThanXP;
    }

    if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId && osvi.dwMajorVersion > 4) {
        if (osvi.dwMajorVersion == 6) {
            if (osvi.dwMinorVersion == 0) {
                return OSVersion_WindowsVista;
            } else if (osvi.dwMinorVersion == 1) {
                return OSVersion_Windows7;
            }
        } else if (osvi.dwMajorVersion == 5) {
            if (osvi.dwMinorVersion == 2) {
                return OSVersion_Windows2003;
            } else if (osvi.dwMinorVersion == 1) {
                return OSVersion_WindowsXP;
            }
        }
    }
    return OSVersion_OlderThanXP;
	*/
}

bool touchmind::util::OSVersionChecker::IsVista()
{
    OSVERSIONINFOEX ver;
    DWORDLONG condMask = 0;
    ZeroMemory(&ver, sizeof(OSVERSIONINFOEX));
    ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    ver.dwMajorVersion = 6;
    ver.dwMinorVersion = 0;
    VER_SET_CONDITION(condMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(condMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    BOOL bRet = VerifyVersionInfo(&ver, VER_MAJORVERSION | VER_MINORVERSION, condMask);
    return bRet == TRUE;
}

bool touchmind::util::OSVersionChecker::IsWin7()
{
    OSVERSIONINFOEX ver;
    DWORDLONG condMask = 0;
    ZeroMemory(&ver, sizeof(OSVERSIONINFOEX));
    ver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    ver.dwMajorVersion = 6;
    ver.dwMinorVersion = 1;
    VER_SET_CONDITION(condMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
    VER_SET_CONDITION(condMask, VER_MINORVERSION, VER_GREATER_EQUAL);
    BOOL bRet = VerifyVersionInfo(&ver, VER_MAJORVERSION | VER_MINORVERSION, condMask);
    return bRet == TRUE;
}
