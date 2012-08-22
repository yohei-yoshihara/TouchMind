#include "stdafx.h"
#include "resource.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/MUI.h"
#include "touchmind/print/XPSPrintSetupDialog.h"

struct PaperInfo {
    touchmind::print::PRINTSETUP_PAGE_SIZE pageSize;
    wchar_t *label;
    XPS_SIZE xpsPageSize;
};

const int g_paperInfoSize = 9;
const int g_defaultPaperInfoIndex = 1;

PaperInfo g_paperInfoList[] = {
    {
        touchmind::print::PRINTSETUP_PAGE_SIZE_A3,
        L"A3",
        {297.0f / 25.4f * 96.0f, 420.0f / 25.4f * 96.0f}
    },
    {
        touchmind::print::PRINTSETUP_PAGE_SIZE_A4,
        L"A4",
        {210.0f / 25.4f * 96.0f, 297.0f / 25.4f * 96.0f}
    },
    {
        touchmind::print::PRINTSETUP_PAGE_SIZE_A5,
        L"A5",
        {148.0f / 25.4f * 96.0f, 210.0f / 25.4f * 96.0f}
    },
    {
        touchmind::print::PRINTSETUP_PAGE_SIZE_B4,
        L"B4",
        {257.0f / 25.4f * 96.0f, 364.0f / 25.4f * 96.0f}
    },
    {
        touchmind::print::PRINTSETUP_PAGE_SIZE_B5,
        L"B5",
        {182.0f / 25.4f * 96.0f, 257.0f / 25.4f * 96.0f}
    },
    {
        touchmind::print::PRINTSETUP_PAGE_SIZE_LETTER,
        L"Letter 8.5x11",
        {8.5f * 96.0f, 11.0f * 96.0f}
    },
    {
        touchmind::print::PRINTSETUP_PAGE_SIZE_LEGAL,
        L"Legal 8.5x14",
        {8.5f * 96.0f, 14.0f * 96.0f}
    },
    {
        touchmind::print::PRINTSETUP_PAGE_SIZE_US4X8,
        L"US 4x8",
        {4.0f * 96.0f, 8.0f * 96.0f}
    },
    {
        touchmind::print::PRINTSETUP_PAGE_SIZE_US5X7,
        L"US 5x7",
        {5.0f * 96.0f, 7.0f * 96.0f}
    },
};

void floatToWChar(FLOAT f, wchar_t *s, size_t n)
{
    swprintf_s(s, n, L"%.1f", f);
}

bool wcharToFloat(const wchar_t *s, FLOAT *pf)
{
    std::wregex rx(L"^[0-9]*$|^[0-9]*\\.[0-9]*$");
    if (std::regex_match(s, rx)) {
        int n = swscanf_s(s, L"%f", pf);
        return n == 1;
    }
    return false;
}

touchmind::print::XPSPrintSetupDialog *g_pPrintSetupDialog = nullptr;

touchmind::print::XPSPrintSetupDialog::XPSPrintSetupDialog(void) :
    m_parentHWnd(nullptr),
    m_selectedPageSize(PRINTSETUP_PAGE_SIZE_A4),
    m_selectedPageDirection(PRINTSETUP_PAGE_DIRECTION_PORTRAIT),
    m_leftMargin(15.0f),
    m_rightMargin(15.0f),
    m_topMargin(15.0f),
    m_bottomMargin(15.0f)
{
    UpdateUI();
}

touchmind::print::XPSPrintSetupDialog::~XPSPrintSetupDialog(void)
{
}

void touchmind::print::XPSPrintSetupDialog::Initialize()
{
    HWND hWnd = GetDlgItem(GetHWnd(), IDC_PAGE_SIZE);
    for (int i = 0; i < g_paperInfoSize; ++i) {
        ComboBox_AddString(hWnd, g_paperInfoList[i].label);
    }

    UpdateUI();
}

void touchmind::print::XPSPrintSetupDialog::UpdateUI()
{
    HWND hWnd = GetDlgItem(GetHWnd(), IDC_PAGE_SIZE);
    for (int i = 0; i < g_paperInfoSize; ++i) {
        if (m_selectedPageSize == g_paperInfoList[i].pageSize) {
            int index = ComboBox_FindString(hWnd, 0, g_paperInfoList[i].label);
            if (index != CB_ERR) {
                ComboBox_SetCurSel(hWnd, index);
                break;
            }
        }
    }

    if (m_selectedPageDirection == PRINTSETUP_PAGE_DIRECTION_LANDSCAPE) {
        HWND hWnd = GetDlgItem(GetHWnd(), IDC_HORIZONTAL);
        Button_SetCheck(hWnd, BST_CHECKED);
    } else {
        HWND hWnd = GetDlgItem(GetHWnd(), IDC_VERTICAL);
        Button_SetCheck(hWnd, BST_CHECKED);
    }

    wchar_t buf[1024];

    hWnd = GetDlgItem(GetHWnd(), IDC_LEFT_MARGIN);
    floatToWChar(m_leftMargin, buf, 1024);
    Edit_SetText(hWnd, buf);

    hWnd = GetDlgItem(GetHWnd(), IDC_RIGHT_MARGIN);
    floatToWChar(m_rightMargin, buf, 1024);
    Edit_SetText(hWnd, buf);

    hWnd = GetDlgItem(GetHWnd(), IDC_TOP_MARGIN);
    floatToWChar(m_topMargin, buf, 1024);
    Edit_SetText(hWnd, buf);

    hWnd = GetDlgItem(GetHWnd(), IDC_BOTTOM_MARGIN);
    floatToWChar(m_bottomMargin, buf, 1024);
    Edit_SetText(hWnd, buf);
}

bool touchmind::print::XPSPrintSetupDialog::UpdateValues()
{
    HWND hWnd = GetDlgItem(GetHWnd(), IDC_PAGE_SIZE);
    int index = ComboBox_GetCurSel(hWnd);
    if (index != CB_ERR) {
        wchar_t buf[1024];
        ComboBox_GetText(hWnd, buf, 1024);
        for (int i = 0; i < g_paperInfoSize; ++i) {
            if (wcscmp(buf, g_paperInfoList[i].label) == 0) {
                m_selectedPageSize = g_paperInfoList[i].pageSize;
                break;
            }
        }
    }

    hWnd = GetDlgItem(GetHWnd(), IDC_HORIZONTAL);
    if (Button_GetCheck(hWnd) == BST_CHECKED) {
        m_selectedPageDirection = PRINTSETUP_PAGE_DIRECTION_LANDSCAPE;
    } else {
        m_selectedPageDirection = PRINTSETUP_PAGE_DIRECTION_PORTRAIT;
    }

    wchar_t buf[1024];

    hWnd = GetDlgItem(GetHWnd(), IDC_LEFT_MARGIN);
    Edit_GetText(hWnd, buf, 1024);
    FLOAT leftMargin;
    if (wcharToFloat(buf, &leftMargin)) {
        m_leftMargin = leftMargin;
    } else {
        int len = Button_GetTextLength(hWnd);
        Edit_SetSel(hWnd, 0, len);
        SetFocus(hWnd);
        MessageBeep(MB_ICONERROR);
        return false;
    }

    hWnd = GetDlgItem(GetHWnd(), IDC_RIGHT_MARGIN);
    Edit_GetText(hWnd, buf, 1024);
    FLOAT rightMargin;
    if (wcharToFloat(buf, &rightMargin)) {
        m_rightMargin = rightMargin;
    } else {
        int len = Button_GetTextLength(hWnd);
        Edit_SetSel(hWnd, 0, len);
        SetFocus(hWnd);
        MessageBeep(MB_ICONERROR);
        return false;
    }

    hWnd = GetDlgItem(GetHWnd(), IDC_TOP_MARGIN);
    Edit_GetText(hWnd, buf, 1024);
    FLOAT topMargin;
    if (wcharToFloat(buf, &topMargin)) {
        m_topMargin = topMargin;
    } else {
        int len = Button_GetTextLength(hWnd);
        Edit_SetSel(hWnd, 0, len);
        SetFocus(hWnd);
        MessageBeep(MB_ICONERROR);
        return false;
    }

    hWnd = GetDlgItem(GetHWnd(), IDC_BOTTOM_MARGIN);
    Edit_GetText(hWnd, buf, 1024);
    FLOAT bottomMargin;
    if (wcharToFloat(buf, &bottomMargin)) {
        m_bottomMargin = bottomMargin;
    } else {
        int len = Button_GetTextLength(hWnd);
        Edit_SetSel(hWnd, 0, len);
        SetFocus(hWnd);
        MessageBeep(MB_ICONERROR);
        return false;
    }
    return true;
}

BOOL CALLBACK PrintSetupDialogProc(HWND hwndDlg,
                                   UINT message,
                                   WPARAM wParam,
                                   LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
    case WM_INITDIALOG:
        g_pPrintSetupDialog->SetHWnd(hwndDlg);
        g_pPrintSetupDialog->Initialize();
        break;
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
            if (g_pPrintSetupDialog->UpdateValues()) {
                EndDialog(hwndDlg, wParam);
                return TRUE;
            }
            break;
        case IDCANCEL:
            EndDialog(hwndDlg, wParam);
            return TRUE;
        }
    }
    return FALSE;
}

bool touchmind::print::XPSPrintSetupDialog::Show()
{
    if (g_pPrintSetupDialog != nullptr) {
        return false;
    }
    g_pPrintSetupDialog = this;
    bool ret = false;
    if (DialogBox(MUI::GetHModule(),
                  MAKEINTRESOURCE(IDD_PRINT_SETTING),
                  m_parentHWnd,
                  (DLGPROC)PrintSetupDialogProc) == IDOK) {
        ret = true;
    } else {
        ret = false;
    }
    g_pPrintSetupDialog = nullptr;

    return ret;
}

XPS_SIZE touchmind::print::XPSPrintSetupDialog::GetSelectedPageXpsSize() const
{
    for (int i = 0; i < g_paperInfoSize; ++i) {
        if (m_selectedPageSize == g_paperInfoList[i].pageSize) {
            if (m_selectedPageDirection == PRINTSETUP_PAGE_DIRECTION_PORTRAIT) {
                return g_paperInfoList[i].xpsPageSize;
            } else {
                XPS_SIZE pageSize = {g_paperInfoList[i].xpsPageSize.height, g_paperInfoList[i].xpsPageSize.width};
                return pageSize;
            }
        }
    }
    XPS_SIZE pageSize = {210.0f / 25.4f * 96.0f, 297.0f / 25.4f * 96.0f};
    return pageSize;
}
