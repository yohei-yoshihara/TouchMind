#ifndef TOUCHMIND_RIBBON_HANDLER_LINESTYLECOMMANDHANDLER_H_
#define TOUCHMIND_RIBBON_HANDLER_LINESTYLECOMMANDHANDLER_H_

#include "forwarddecl.h"

namespace touchmind
{
namespace ribbon
{
namespace handler
{

class LineStyleCommandHandler :
    public IUICommandHandler
{
private:
    const static std::array<int, NUMBER_OF_LINE_STYLES> s_IMAGE_ID;
    const static std::array<int, NUMBER_OF_LINE_STYLES> s_IMAGE64x24_ID;
    static bool s_imageInitialized;
    ULONG m_refCount;
    touchmind::ribbon::RibbonFramework *m_pRibbonFramework;
    touchmind::ribbon::dispatch::RibbonRequestDispatcher *m_pRibbonRequestDispatcher;
    std::array<CComPtr<IUIImage>, NUMBER_OF_LINE_STYLES> m_pImages;
    std::array<CComPtr<IUIImage>, NUMBER_OF_LINE_STYLES> m_pImages64x24;

protected:
    static UINT32 _ConvertToIndex(LINE_STYLE lineStyle);
    static LINE_STYLE _ConvertToLineStyle(UINT32 index);

public:
    static HRESULT CreateInstance(IUICommandHandler **ppCommandHandler);
    LineStyleCommandHandler();
    LineStyleCommandHandler(
        touchmind::ribbon::RibbonFramework *pRibbonFramework,
        touchmind::ribbon::dispatch::RibbonRequestDispatcher *pRibbonRequestDispatcher);
    virtual ~LineStyleCommandHandler();

    // IUnknown interface
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();
    IFACEMETHODIMP QueryInterface(REFIID riid, void**ppInterface);

    // IUICommandHandler interface
    IFACEMETHODIMP Execute(UINT cmdID,
                           UI_EXECUTIONVERB verb,
                           const PROPERTYKEY* pKey,
                           const PROPVARIANT* pPropvarValue,
                           IUISimplePropertySet* pCommandExecutionProperties);
    IFACEMETHODIMP UpdateProperty(UINT cmdID,
                                  REFPROPERTYKEY key,
                                  const PROPVARIANT* pPropvarCurrentValue,
                                  PROPVARIANT* pPropvarNewValue);
};

} // handler
} // ribbon
} // touchmind

#endif // TOUCHMIND_RIBBON_HANDLER_LINESTYLECOMMANDHANDLER_H_