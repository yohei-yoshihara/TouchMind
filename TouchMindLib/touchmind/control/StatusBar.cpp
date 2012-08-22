#include "StdAfx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/control/StatusBar.h"

touchmind::control::StatusBar::StatusBar(void) :
    m_hWnd(nullptr),
    m_parehtHWnd(nullptr),
    m_numberOfParts(0)
{
}


touchmind::control::StatusBar::~StatusBar(void)
{
}

HRESULT touchmind::control::StatusBar::Initialize( HWND hwndParent, int idStatus, HINSTANCE hInstance, int numberOfParts )
{
    HRESULT hr;

    m_parehtHWnd = hwndParent;

    INITCOMMONCONTROLSEX initCtrls;
    initCtrls.dwSize = sizeof(initCtrls);
    initCtrls.dwICC = ICC_BAR_CLASSES;
    if (!InitCommonControlsEx(&initCtrls)) {
        LOG(SEVERITY_LEVEL_ERROR) << L"InitCommonControlsEx failed";
        return E_FAIL;
    }

    m_hWnd = CreateWindowEx(
                 0,
                 STATUSCLASSNAME,
                 nullptr,
                 SBARS_SIZEGRIP | WS_CHILD | WS_VISIBLE,
                 0, 0, 0, 0,
                 hwndParent,
                 (HMENU)idStatus,
                 hInstance,
                 nullptr);
    hr = m_hWnd != nullptr ? S_OK : E_FAIL;
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << L"Create status bar failed";
        return hr;
    }
    SetParts(numberOfParts);
    return hr;
}

RECT touchmind::control::StatusBar::GetRect()
{
    RECT rect;
    GetClientRect(m_hWnd, &rect);
    return rect;
}

void touchmind::control::StatusBar::OnSize(WPARAM wParam, LPARAM lParam)
{
    SendMessage(m_hWnd, WM_SIZE, wParam, lParam);
    SetParts(m_numberOfParts);
}

void touchmind::control::StatusBar::SetParts( int numberOfParts )
{
    m_numberOfParts = numberOfParts;

    RECT clientRect;
    GetClientRect(m_parehtHWnd, &clientRect);

    HLOCAL hlocal = LocalAlloc(LHND, sizeof(int) * numberOfParts);
    PINT paParts = (PINT) LocalLock(hlocal);

    int nWidth = clientRect.right / numberOfParts;
    for (int i = 0; i < numberOfParts; ++i) {
        paParts[i] = nWidth;
        nWidth += nWidth;
    }

    SendMessage(m_hWnd, SB_SETPARTS, (WPARAM) numberOfParts, (LPARAM)paParts);

    LocalUnlock(hlocal);
    LocalFree(hlocal);
}

void touchmind::control::StatusBar::SetColor( COLORREF color )
{
    SendMessage(m_hWnd, SB_SETBKCOLOR, 0, (LPARAM)color);
}

void touchmind::control::StatusBar::SetText( int index, const std::wstring& text )
{
    SendMessage(m_hWnd, SB_SETTEXT, index, (LPARAM)text.c_str());
}

void touchmind::control::StatusBar::SetText( int index, const wchar_t* text )
{
    SendMessage(m_hWnd, SB_SETTEXT, index, (LPARAM)text);
}

void touchmind::control::StatusBar::ClearText( int index )
{
    WCHAR* s = L"";
    SendMessage(m_hWnd, SB_SETTEXT, index, (LPARAM)s);
}
