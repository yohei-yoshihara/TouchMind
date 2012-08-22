#ifndef TOUCHMIND_RIBBON_HANDLER_LINECOLORCOMMANDHANDLER_H_
#define TOUCHMIND_RIBBON_HANDLER_LINECOLORCOMMANDHANDLER_H_

#include "forwarddecl.h"

namespace touchmind
{
namespace ribbon
{
namespace handler
{

class LineColorCommandHandler :
    public IUICommandHandler
{
private:
    ULONG m_refCount;
    touchmind::Context *m_pContext;
    touchmind::ribbon::RibbonFramework *m_pRibbonFramework;
    touchmind::ribbon::dispatch::RibbonRequestDispatcher *m_pRibbonRequestDispatcher;

protected:
    HRESULT _CreateUIImage(D2D1_COLOR_F color, IUIImage **ppUIImage);

public:
    static HRESULT CreateInstance(IUICommandHandler **ppCommandHandler);
    LineColorCommandHandler();
    LineColorCommandHandler(
        touchmind::Context *pContext,
        touchmind::ribbon::RibbonFramework *pRibbonFramework,
        touchmind::ribbon::dispatch::RibbonRequestDispatcher *pRibbonRequestDispatcher);
    virtual ~LineColorCommandHandler();

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

#endif // TOUCHMIND_RIBBON_HANDLER_LINECOLORCOMMANDHANDLER_H_