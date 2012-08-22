#ifndef TOUCHMIND_RIBBON_RIBBONFACTORY_H_
#define TOUCHMIND_RIBBON_RIBBONFACTORY_H_

#include "forwarddecl.h"

namespace touchmind
{
namespace ribbon
{

class RibbonFramework
{
private:
    IUIFramework *m_pFramework;
    CComPtr<RibbonCommandHandler> m_pRibbonCommandHandler;
    std::wstring m_resourceName;
    HWND m_hWnd;
    CComPtr<IUIImageFromBitmap> m_pifbFactory;

public:
    RibbonFramework();
    virtual ~RibbonFramework();
    HRESULT Initialize(HWND hWnd, touchmind::win::TouchMindApp* pTouchMindApp);
    void SetResourceName(const std::wstring &resourceName) {
        m_resourceName = resourceName;
    }
    RibbonCommandHandler* GetRibbonCommandHandler() const {
        return m_pRibbonCommandHandler;
    }
    IUIFramework* GetFramework() const {
        return m_pFramework;
    }
    UINT GetRibbonHeight();
    HWND GetHWnd() const {
        return m_hWnd;
    }
    void Destroy();
    HRESULT CreateUIImageFromBitmapResource(IN LPCTSTR resource, OUT IUIImage **ppImage);
    IUIImageFromBitmap* GetUIImageFromBitmap() const {
        return m_pifbFactory;
    }
};

} // ribbon
} // touchmind

#endif // TOUCHMIND_RIBBON_RIBBONFACTORY_H_