#include "Stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "TouchMind.h"
#include "touchmind/win/TouchMindApp.h"
#include "touchmind/shell/FileAssociation.h"
#include "touchmind/util/OSVersionChecker.h"
#include "touchmind/MUI.h"

class CDummyModule : public CAtlExeModuleT<CDummyModule> {};
CDummyModule _Module;

HINSTANCE hInst;

HINSTANCE GetHINSTANCE() {
  return hInst;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine_, int nCmdShow) {
  UNREFERENCED_PARAMETER(hInstance);
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine_);
  UNREFERENCED_PARAMETER(nCmdShow);

  HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);

  PWSTR ppszPath = nullptr;
  HRESULT hr = ::SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &ppszPath);
  if (SUCCEEDED(hr)) {
    std::wstring path(ppszPath);
    path += _LOG_FOLDERNAME;
    int ret = ::SHCreateDirectory(nullptr, path.c_str());
    hr = (ret == ERROR_SUCCESS || ret == ERROR_FILE_EXISTS || ret == ERROR_ALREADY_EXISTS) ? S_OK : E_FAIL;
    if (SUCCEEDED(hr)) {
      path += _LOG_FILENAME;
      touchmind::logging::InitLogger(path.c_str());
    }
  }
  if (FAILED(hr)) {
    TCHAR lpTempPathBuffer[MAX_PATH];
    GetTempPath(MAX_PATH, lpTempPathBuffer);
    std::wstring tempPath(lpTempPathBuffer);
    tempPath += _LOG_FILENAME;
    touchmind::logging::InitLogger(tempPath.c_str());
  }
  if (ppszPath != nullptr) {
    CoTaskMemFree(ppszPath);
  }

  LPWSTR lpCmdLine = GetCommandLineW();
  LOG(SEVERITY_LEVEL_INFO) << L"cmdLine=\"" << lpCmdLine << L"\"";

  LPWSTR *szArglist = nullptr;
  int nArgs = 0;
  szArglist = CommandLineToArgvW(lpCmdLine, &nArgs);
  if (nullptr == szArglist) {
    LOG(SEVERITY_LEVEL_ERROR) << L"CommandLineToArgvW failed\n";
    return 0;
  }

  if (nArgs >= 2 && wcscmp(szArglist[1], L"-r") == 0) {
    LOG(SEVERITY_LEVEL_INFO) << L"Registering file associations";
    touchmind::shell::FileAssociation fa;
    HRESULT hr = fa.RegisterToHandleFileTypes();
    if (SUCCEEDED(hr)) {
      LOG(SEVERITY_LEVEL_INFO) << L"Register file associations done";
    } else {
      LOG(SEVERITY_LEVEL_INFO) << L"Failed to register file associations, hr = " << hr;
    }
    return 2;
  } else if (nArgs >= 2 && wcscmp(szArglist[1], L"-ur") == 0) {
    LOG(SEVERITY_LEVEL_INFO) << L"Unregistering file associations";
    touchmind::shell::FileAssociation fa;
    HRESULT hr = fa.UnRegisterFileTypeHandlers();
    if (SUCCEEDED(hr)) {
      LOG(SEVERITY_LEVEL_INFO) << L"Unregistering file associations done";
    } else {
      LOG(SEVERITY_LEVEL_INFO) << L"Failed to unregister file associations, hr = " << hr;
    }
    return 2;
  }

  // ULONG numLangs;
  // SetProcessPreferredUILanguages(MUI_LANGUAGE_NAME, L"en-US", &numLangs);
  // SetProcessPreferredUILanguages(MUI_LANGUAGE_NAME, L"de-DE", &numLangs);
  // SetProcessPreferredUILanguages(MUI_LANGUAGE_NAME, L"nl-NL", &numLangs);
  if (touchmind::MUI::Initialize() != S_OK) {
    return 3;
  }
  // MessageBox(nullptr, MUI::GetString(5001), L"test", MB_OK);

  // check OS version
  bool vista = touchmind::util::OSVersionChecker::IsVista();
  bool win7 = touchmind::util::OSVersionChecker::IsWin7();
  LOG(SEVERITY_LEVEL_INFO) << L"Vista = " << std::boolalpha << vista;
  LOG(SEVERITY_LEVEL_INFO) << L"Win7 = " << std::boolalpha << win7;

  if (!vista && !win7) {
    MessageBox(nullptr, L"TouchMind only works Vista and Win7", L"OS check error", MB_OK | MB_ICONERROR);
    return -1;
  }

  if (SUCCEEDED(CoInitialize(nullptr))) {
    {
      touchmind::win::TouchMindApp app;
      app.Initialize();
      app.RunMessageLoop();
    }
    CoUninitialize();
  }
  return 0;
}
