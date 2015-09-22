#include "StdAfx.h"
#include "touchmind/Common.h"
#include "FileAssociation.h"
#include <Strsafe.h>

const WCHAR touchmind::shell::FileAssociation::APPEXE[] = _EXE_NAME;
const WCHAR touchmind::shell::FileAssociation::PROGID[] = _APPLICATION_ID;
PCWSTR const touchmind::shell::FileAssociation::EXTENSIONS[] = {_EXTENSION};

touchmind::shell::FileAssociation::FileAssociation(void) {
}

touchmind::shell::FileAssociation::~FileAssociation(void) {
}

HRESULT touchmind::shell::FileAssociation::RegSetString(HKEY hkey, PCWSTR pszSubKey, PCWSTR pszValue, PCWSTR pszData) {
  size_t lenData = lstrlen(pszData);
  return HRESULT_FROM_WIN32(
      SHSetValue(hkey, pszSubKey, pszValue, REG_SZ, pszData, static_cast<DWORD>((lenData + 1) * sizeof(*pszData))));
}

HRESULT touchmind::shell::FileAssociation::RegSetString(HKEY hKey, std::wstring &subkey, std::wstring &value,
                                                        std::wstring &data) {
  HRESULT hr = S_OK;
  hr = HRESULT_FROM_WIN32(SHSetValue(hKey, subkey.c_str(), value.c_str(), REG_SZ, data.c_str(),
                                     static_cast<DWORD>((data.length() + 1) * sizeof(wchar_t))));
  return hr;
}

/// <summary>register ProgID</summary>
HRESULT touchmind::shell::FileAssociation::RegisterProgId() {
  HRESULT hr = S_OK;

  // create "HKEY_CLASSES_ROOT\[ProgId]"
  HKEY hkeyProgid = nullptr;
  if (SUCCEEDED(hr)) {
    hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CLASSES_ROOT, PROGID, 0, nullptr, REG_OPTION_NON_VOLATILE,
                                           KEY_SET_VALUE | KEY_CREATE_SUB_KEY, nullptr, &hkeyProgid, nullptr));
  }

  // register "HKEY_CLASS_ROOT\[ProgId]\FriendlyTypeName"
  if (SUCCEEDED(hr)) {
    hr = RegSetString(hkeyProgid, nullptr, L"FriendlyTypeName", _FIRENDLY_TYPE_NAME);
  }

  // get the module path(.exe)
  WCHAR szAppPath[MAX_PATH];
  if (SUCCEEDED(hr)) {
    hr = (GetModuleFileName(nullptr, szAppPath, ARRAYSIZE(szAppPath)) > 0) ? S_OK : HRESULT_FROM_WIN32(GetLastError());
  }

  // register "HKEY_CLASS_ROOT\[ProgId]\DefaultIcon"
  WCHAR szIcon[MAX_PATH + 3];
  if (SUCCEEDED(hr)) {
    hr = StringCchPrintf(szIcon, ARRAYSIZE(szIcon), L"%s,0", szAppPath);
    if (SUCCEEDED(hr)) {
      hr = RegSetString(hkeyProgid, L"DefaultIcon", nullptr, szAppPath);
    }
  }

  // register "HKEY_CLASS_ROOT\[ProgId]\CurVer"
  if (SUCCEEDED(hr)) {
    hr = RegSetString(hkeyProgid, L"CurVer", nullptr, PROGID);
  }

  // register "HEKY_CLASS_ROOT\[ProgId]\shell\Open\Command"
  if (SUCCEEDED(hr)) {
    HKEY hkeyShell;
    hr = HRESULT_FROM_WIN32(RegCreateKeyEx(hkeyProgid, L"shell", 0, nullptr, REG_OPTION_NON_VOLATILE,
                                           KEY_SET_VALUE | KEY_CREATE_SUB_KEY, nullptr, &hkeyShell, nullptr));
    if (SUCCEEDED(hr)) {
      // register verb("Open") which are provided by ProgID under the "shell" key.
      WCHAR szCmdLine[MAX_PATH * 2];
      hr = StringCchPrintf(szCmdLine, ARRAYSIZE(szCmdLine), L"%s %%1", szAppPath);
      if (SUCCEEDED(hr)) {
        hr = RegSetString(hkeyShell, L"Open\\Command", nullptr, szCmdLine);
        if (SUCCEEDED(hr)) {
          // set "Open" as the default verb of ProgID
          hr = RegSetString(hkeyShell, nullptr, nullptr, L"Open");
        }
      }
      RegCloseKey(hkeyShell);
    }
  }
  if (hkeyProgid != nullptr) {
    RegCloseKey(hkeyProgid);
  }

  return hr;
}

/// <summary>delete ProgID</summary>
HRESULT touchmind::shell::FileAssociation::UnregisterProgId() {
  long lRes = RegDeleteTree(HKEY_CLASSES_ROOT, PROGID);
  HRESULT hr = (ERROR_SUCCESS == lRes || ERROR_FILE_NOT_FOUND == lRes) ? S_OK : HRESULT_FROM_WIN32(lRes);
  return hr;
}

HRESULT touchmind::shell::FileAssociation::RegisterToHandleExt(const WCHAR *ext) {
  HRESULT hr = S_OK;

  std::wstring key(ext);
  key += L"\\OpenWithProgids";

  HKEY hkeyProgidList = nullptr;

  // create "HKEY_CLASS_ROOT\[.ext]\OpenWithProgids"
  if (SUCCEEDED(hr)) {
    hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CLASSES_ROOT, key.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE,
                                           KEY_SET_VALUE, nullptr, &hkeyProgidList, nullptr));
  }

  // register ProgId at "HKEY_CLASS_ROOT\[.ext]\OpenWithProgids"
  if (SUCCEEDED(hr)) {
    hr = HRESULT_FROM_WIN32(RegSetValueEx(hkeyProgidList, PROGID, 0, REG_NONE, nullptr, 0));
  }

  if (hkeyProgidList != nullptr) {
    RegCloseKey(hkeyProgidList);
  }

  return hr;
}

HRESULT touchmind::shell::FileAssociation::UnregisterToHandleExt(const WCHAR *ext) {
  HRESULT hr = S_OK;

  std::wstring key(ext);
  key += L"\\OpenWithProgids";

  HKEY hkeyProgidList = nullptr;

  // create "HKEY_CLASS_ROOT\[.ext]\OpenWithProgids"
  if (SUCCEEDED(hr)) {
    hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CLASSES_ROOT, key.c_str(), 0, nullptr, REG_OPTION_NON_VOLATILE,
                                           KEY_SET_VALUE, nullptr, &hkeyProgidList, nullptr));
  }

  // delete ProgID under "HKEY_CLASS_ROOT\[.ext]\OpenWithProgids"
  if (SUCCEEDED(hr)) {
    hr = HRESULT_FROM_WIN32(RegDeleteKeyValue(hkeyProgidList, nullptr, PROGID));
  }

  if (hkeyProgidList != nullptr) {
    RegCloseKey(hkeyProgidList);
  }

  return hr;
}

HRESULT touchmind::shell::FileAssociation::RegisterToHandleFileTypes() {
  HRESULT hr = RegisterProgId();
  if (SUCCEEDED(hr)) {
    for (UINT i = 0; SUCCEEDED(hr) && i < ARRAYSIZE(EXTENSIONS); ++i) {
      hr = RegisterToHandleExt(EXTENSIONS[i]);
    }

    if (SUCCEEDED(hr)) {
      // Notify that file associations have changed
      SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }
  }
  return hr;
}

bool touchmind::shell::FileAssociation::AreFileTypesRegistered() {
  bool fRet = false;
  HKEY hkeyProgid;
  if (SUCCEEDED(HRESULT_FROM_WIN32(RegOpenKey(HKEY_CLASSES_ROOT, PROGID, &hkeyProgid)))) {
    fRet = true;
    RegCloseKey(hkeyProgid);
  }
  return fRet;
}

HRESULT touchmind::shell::FileAssociation::UnRegisterFileTypeHandlers() {
  HRESULT hr = UnregisterProgId();
  if (SUCCEEDED(hr)) {
    for (UINT i = 0; SUCCEEDED(hr) && i < ARRAYSIZE(EXTENSIONS); ++i) {
      hr = UnregisterToHandleExt(EXTENSIONS[i]);
    }

    if (SUCCEEDED(hr)) {
      // Notify that file associations have changed
      SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    }
  }
  return hr;
}
