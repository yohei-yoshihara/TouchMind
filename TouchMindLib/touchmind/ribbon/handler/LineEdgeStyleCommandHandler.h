#ifndef TOUCHMIND_RIBBON_HANDLER_LINEARROWCOMMANDHANDLER_H_
#define TOUCHMIND_RIBBON_HANDLER_LINEARROWCOMMANDHANDLER_H_

#include "forwarddecl.h"

namespace touchmind
{
namespace ribbon
{
namespace handler
{

class LineEdgeStyleCommandHandler :
    public IUICommandHandler
{
private:
    const static std::array<int, NUMBER_OF_LINE_EDGE_STYLES> s_IMAGE_ID;
    const static std::array<int, NUMBER_OF_LINE_EDGE_STYLES> s_IMAGE36x36_ID;
    static bool imageInitialized;
    ULONG m_refCount;
    touchmind::ribbon::RibbonFramework *m_pRibbonFramework;
    touchmind::ribbon::dispatch::RibbonRequestDispatcher *m_pRibbonRequestDispatcher;
    std::array<CComPtr<IUIImage>, NUMBER_OF_LINE_EDGE_STYLES> m_pImages;
    std::array<CComPtr<IUIImage>, NUMBER_OF_LINE_EDGE_STYLES> m_pImages36x36;

protected:
    static UINT32 _ConvertToIndex(LINE_EDGE_STYLE lineEdgeStyle);
    static LINE_EDGE_STYLE _ConvertToLineEdgeStyle(UINT32 index);

public:
    static HRESULT CreateInstance(IUICommandHandler **ppCommandHandler);
    LineEdgeStyleCommandHandler();
    LineEdgeStyleCommandHandler(
        touchmind::ribbon::RibbonFramework *pRibbonFramework,
        touchmind::ribbon::dispatch::RibbonRequestDispatcher *pRibbonRequestDispatcher);
    virtual ~LineEdgeStyleCommandHandler();

    // IUnknown interface
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;
    IFACEMETHODIMP QueryInterface(REFIID riid, void**ppInterface) override;

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

#endif // TOUCHMIND_RIBBON_HANDLER_LINEARROWCOMMANDHANDLER_H_