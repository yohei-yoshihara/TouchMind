#ifndef TOUCHMIND_PRINT_XPSPRINTSETUPDIALOG_H_
#define TOUCHMIND_PRINT_XPSPRINTSETUPDIALOG_H_

#include "forwarddecl.h"
#include "touchmind/print/XPSPrintCommon.h"

namespace touchmind
{
namespace print
{

class XPSPrintSetupDialog
{
private:
    HWND m_parentHWnd;
    HWND m_hWnd;
    PRINTSETUP_PAGE_SIZE m_selectedPageSize;
    PRINTSETUP_PAGE_DIRECTION m_selectedPageDirection;
    FLOAT m_leftMargin;
    FLOAT m_rightMargin;
    FLOAT m_topMargin;
    FLOAT m_bottomMargin;

public:
    XPSPrintSetupDialog(void);
    ~XPSPrintSetupDialog(void);
    void SetParentHWnd(HWND parentHWnd) {
        m_parentHWnd = parentHWnd;
    }
    HWND GetParentHWnd() const {
        return m_parentHWnd;
    }
    void SetHWnd(HWND hWnd) {
        m_hWnd = hWnd;
    }
    HWND GetHWnd() const {
        return m_hWnd;
    }
    PRINTSETUP_PAGE_SIZE GetSelectedPageSize() const {
        return m_selectedPageSize;
    }
    XPS_SIZE GetSelectedPageXpsSize() const;
    PRINTSETUP_PAGE_DIRECTION GetSelectedPageDirection() const {
        return m_selectedPageDirection;
    }
    FLOAT GetLeftMargin() const {
        return m_leftMargin;
    }
    FLOAT GetRightMargin() const {
        return m_rightMargin;
    }
    FLOAT GetTopMargin() const {
        return m_topMargin;
    }
    FLOAT GetBottomMargin() const {
        return m_bottomMargin;
    }
    XPSPRINT_MARGIN GetMargin() const {
        XPSPRINT_MARGIN xpsMargin;
        xpsMargin.left = m_leftMargin / 25.4f * 96.0f;
        xpsMargin.top = m_topMargin / 25.4f * 96.0f;
        xpsMargin.right = m_rightMargin / 25.4f * 96.0f;
        xpsMargin.bottom = m_bottomMargin / 25.4f * 96.0f;
        return xpsMargin;
    }
    bool Show();
    void UpdateUI();
    bool UpdateValues();
    void Initialize();
};

} // print
} // touchmind

#endif // TOUCHMIND_PRINT_XPSPRINTSETUPDIALOG_H_