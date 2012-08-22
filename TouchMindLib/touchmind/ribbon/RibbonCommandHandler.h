#ifndef TOUCHMIND_RIBBON_RIBBONCOMMANDHANDLER_H_
#define TOUCHMIND_RIBBON_RIBBONCOMMANDHANDLER_H_

#include "forwarddecl.h"

#define WM_RIBBON_RESIZED WM_USER+1

namespace touchmind
{
namespace ribbon
{

typedef std::function <
HRESULT(UINT nCmdID,
        UI_EXECUTIONVERB verb,
        const PROPERTYKEY* key,
        const PROPVARIANT* pPropvarValue,
        IUISimplePropertySet* pCommandExecutionProperties) > OnExecuteListener;

typedef std::function <
HRESULT(UINT nCmdID,
        REFPROPERTYKEY key,
        const PROPVARIANT* pPropvarCurrentValue,
        PROPVARIANT* pPropvarNewValue) > OnUpdatePropertyListener;

class RibbonCommandHandler :
    public IUIApplication,
    public IUICommandHandler
{
private:
    ULONG m_refCount;
    IUIRibbon *m_pRibbon;
    RibbonFramework *m_pRibbonFramework;
    touchmind::win::TouchMindApp *m_pTouchMindApp;
    int m_currentContext;
    IUICommandHandler *m_pFontControlCommandHandler;
    IUIImageFromBitmap *m_pifbFactory;

    std::map<UINT, OnExecuteListener> m_executeListeners;
    std::map<UINT, OnUpdatePropertyListener> m_updatePropertyListeners;
    std::map<UINT, CComPtr<IUICommandHandler>> m_commandHandlers;

public:
    RibbonCommandHandler();
    virtual ~RibbonCommandHandler();

    // IUnknown interface
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();
    IFACEMETHODIMP QueryInterface(REFIID riid, void**ppInterface);

    void SetRibbonFramework(RibbonFramework *pRibbonFramework) {
        m_pRibbonFramework = pRibbonFramework;
    }
    void SetTouchMindApp(touchmind::win::TouchMindApp* pTouchMindApp) {
        m_pTouchMindApp = pTouchMindApp;
    }
    int GetCurrentContext() const {
        return m_currentContext;
    }
    void SetCurrentContext(int newContext) {
        m_currentContext = newContext;
    }
    HRESULT Initialize();

    // IUIApplication (start)
    IFACEMETHODIMP OnCreateUICommand(
        UINT32 nCmdID,
        UI_COMMANDTYPE typeID,
        IUICommandHandler** ppCommandHandler) override;
    IFACEMETHODIMP OnViewChanged(
        UINT32 nViewID,
        UI_VIEWTYPE typeID,
        IUnknown* pView,
        UI_VIEWVERB verb,
        INT32 uReasonCode) override;
    IFACEMETHODIMP OnDestroyUICommand(
        UINT32 commandId,
        UI_COMMANDTYPE typeID,
        IUICommandHandler* pCommandHandler) override;
    // IUIApplication (end)

    // IUICommandHandler (start)
    IFACEMETHODIMP Execute(
        UINT nCmdID,
        UI_EXECUTIONVERB verb,
        const PROPERTYKEY* key,
        const PROPVARIANT* ppropvarValue,
        IUISimplePropertySet* pCommandExecutionProperties) override;
    IFACEMETHODIMP UpdateProperty(
        UINT nCmdID,
        REFPROPERTYKEY key,
        const PROPVARIANT* ppropvarCurrentValue,
        PROPVARIANT* ppropvarNewValue) override;
    // IUICommandHandler (end)

    void AddOnExecuteListener(UINT cmdId, OnExecuteListener l) {
        m_executeListeners.insert(std::make_pair<UINT, OnExecuteListener>(cmdId, l));
    }
    void AddOnUpdatePropertyListener(UINT cmdId, OnUpdatePropertyListener l) {
        m_updatePropertyListeners.insert(std::make_pair<UINT, OnUpdatePropertyListener>(cmdId, l));
    }
    void AddCommandHandler(UINT cmdId, CComPtr<IUICommandHandler> commandHandler) {
        m_commandHandlers.insert(std::make_pair<UINT, CComPtr<IUICommandHandler>>(cmdId, commandHandler));
    }
};

} // ribbon
} // touchmind

#endif // TOUCHMIND_RIBBON_RIBBONCOMMANDHANDLER_H_