#ifndef TOUCHMIND_RIBBON_HANDLER_STROKEWIDTHCOMMANDHANDLER_H_
#define TOUCHMIND_RIBBON_HANDLER_STROKEWIDTHCOMMANDHANDLER_H_

#include "forwarddecl.h"

namespace touchmind
{
namespace ribbon
{
namespace handler
{

class LineWidthCommandHandler :
    public IUICommandHandler
{
private:
    const static std::array<int, 4> s_IMAGE_ID;
    ULONG m_refCount;
    touchmind::ribbon::RibbonFramework *m_pRibbonFramework;
    touchmind::ribbon::dispatch::RibbonRequestDispatcher *m_pRibbonRequestDispatcher;
    std::array<CComPtr<IUIImage>, 4> m_pImages;

protected:
    static UINT32 _ConvertToIndex(LINE_WIDTH lineWidth);
    static LINE_WIDTH _ConvertToLineWidth(UINT32 index);

public:
    static HRESULT CreateInstance(IUICommandHandler **ppCommandHandler);
    LineWidthCommandHandler();
    LineWidthCommandHandler(
        touchmind::ribbon::RibbonFramework *pRibbonFramework,
        touchmind::ribbon::dispatch::RibbonRequestDispatcher *pRibbonRequestDispatcher);
    virtual ~LineWidthCommandHandler();

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

#endif // TOUCHMIND_RIBBON_HANDLER_STROKEWIDTHCOMMANDHANDLER_H_