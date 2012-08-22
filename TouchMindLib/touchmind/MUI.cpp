#include "StdAfx.h"
#include "MUI.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/Common.h"

touchmind::MUI::MUI(void)
{
}

touchmind::MUI::~MUI(void)
{
}

HMODULE touchmind::MUI::s_hModule = nullptr;

HRESULT touchmind::MUI::Initialize()
{
    if (s_hModule != nullptr) {
        // already loaded
        return S_OK;
    }
    s_hModule = LoadLibraryExW(L"TouchMindMUI.dll", 0,
                               LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    if (s_hModule == nullptr) {
        assert(!L"Could not load TouchMindMUI.dll");
        LOG(SEVERITY_LEVEL_ERROR) << L"Could not load TouchMindMUI.dll";
        return E_FAIL;
    }
    return S_OK;
}

wchar_t buf[4096];
const wchar_t * touchmind::MUI::GetString(UINT uID)
{
    if (LoadStringW(s_hModule, uID, buf, 4096) != 0) {
        return buf;
    }
    LOG(SEVERITY_LEVEL_WARN) << L"Could not load the string '" << uID << L"'";
    buf[0] = '\0';
    return buf;
}

