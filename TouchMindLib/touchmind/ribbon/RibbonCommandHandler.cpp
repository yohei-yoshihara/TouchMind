#include "StdAfx.h"
#include <Shobjidl.h>
#include "resource.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/model/MapModel.h"
#include "touchmind/win/TouchMindApp.h"
#include "touchmind/win/CanvasPanel.h"
#include "touchmind/ribbon/handler/FontControlCommandHandler.h"
#include "touchmind/ribbon/RibbonCommandHandler.h"
#include "touchmind/ribbon/RibbonFramework.h"
#include "touchmind/shell/MRUManager.h"
#include "touchmind/MUI.h"

touchmind::ribbon::RibbonCommandHandler::RibbonCommandHandler() :
    m_refCount(0),
    m_pRibbon(nullptr),
    m_pRibbonFramework(nullptr),
    m_pTouchMindApp(nullptr),
    m_currentContext(IDC_CMD_CONTEXTMAP1),
    m_pFontControlCommandHandler(nullptr),
    m_pifbFactory(nullptr)
{
}

touchmind::ribbon::RibbonCommandHandler::~RibbonCommandHandler()
{
    SafeRelease(&m_pifbFactory);
}

// IUnknown methods

IFACEMETHODIMP_(ULONG) touchmind::ribbon::RibbonCommandHandler::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

IFACEMETHODIMP_(ULONG) touchmind::ribbon::RibbonCommandHandler::Release()
{
    long val = InterlockedDecrement(&m_refCount);
    if (val == 0) {
        delete this;
    }
    return val;
}

IFACEMETHODIMP touchmind::ribbon::RibbonCommandHandler::QueryInterface(REFIID riid, void** ppInterface)
{
    if (riid == __uuidof(IUnknown)) {
        *ppInterface = static_cast<IUIApplication*>(this);
    } else if (riid == __uuidof(IUIApplication)) {
        *ppInterface = static_cast<IUIApplication*>(this);
    } else if (riid == __uuidof(IUICommandHandler)) {
        *ppInterface = static_cast<IUICommandHandler*>(this);
    } else {
        *ppInterface = nullptr;
        return E_NOINTERFACE;
    }
    (static_cast<IUnknown*>(*ppInterface))->AddRef();
    return S_OK;
}

HRESULT touchmind::ribbon::RibbonCommandHandler::Initialize()
{
    HRESULT hr = S_OK;
    LOG_ENTER;
    if (m_pifbFactory == nullptr) {
        hr = CoCreateInstance(
                 CLSID_UIRibbonImageFromBitmapFactory,
                 nullptr,
                 CLSCTX_ALL,
                 IID_PPV_ARGS(&m_pifbFactory));
    }
    LOG_LEAVE_HRESULT(hr);
    return hr;
}

// IUIApplication (start)
/**
 * Called for each Command specified in the Windows Ribbon framework markup to bind the Command
 * to an IUICommandHandler.
 */
HRESULT touchmind::ribbon::RibbonCommandHandler::OnCreateUICommand(
    UINT32 nCmdID,
    UI_COMMANDTYPE typeID,
    IUICommandHandler** ppCommandHandler)
{
    LOG_ENTER_ARG(L"nCmdID = " << nCmdID);
    UNREFERENCED_PARAMETER(typeID);
    HRESULT hr = S_OK;
    if (m_pFontControlCommandHandler == nullptr) {
        hr = ribbon::handler::FontControlCommandHandler::CreateInstance(&m_pFontControlCommandHandler);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_FATAL) << L"FontControlCommandHandler::CreateInstance failed, hr = " << hr;
            goto exit_func;
        }
    }

    if (m_executeListeners.count(nCmdID) > 0 || m_updatePropertyListeners.count(nCmdID) > 0) {
        hr =  QueryInterface(IID_PPV_ARGS(ppCommandHandler));
    } else if (m_commandHandlers.count(nCmdID) > 0) {
        hr = m_commandHandlers[nCmdID]->QueryInterface(IID_PPV_ARGS(ppCommandHandler));
    } else {
        *ppCommandHandler = nullptr;
        switch (nCmdID) {
        case cmdMRUList:
        case cmdNew:
        case cmdOpenFile:
        case cmdOpenFreeMind:
        case cmdSave:
        case cmdSaveAs:
        case cmdCreateChildNode:
        case cmdCreateLink:
        case cmdEditNode:
        case cmdDeleteNode:
        case cmdExit:
        case cmdCopy:
        case cmdPaste:
        case cmdCut:
        case cmdUndo:
        case cmdExpandNodes:
        case cmdCollapseNodes:
        case cmdVersionInfo:
        case cmdPrint:
        case cmdPrintSetup:
            hr =  QueryInterface(IID_PPV_ARGS(ppCommandHandler));
            break;
        case cmdFontControl: {
            touchmind::control::DWriteEditControlManager *pEditControlManager =
                m_pTouchMindApp->GetCanvasPanel()->GetEditControlManager();
            auto pFontControlHandler = dynamic_cast<touchmind::ribbon::handler::FontControlCommandHandler*>(m_pFontControlCommandHandler);
            pFontControlHandler->SetEditControlManager(pEditControlManager);
            hr = m_pFontControlCommandHandler->QueryInterface(IID_PPV_ARGS(ppCommandHandler));
        }
        break;
        }
    }
exit_func:
    LOG_LEAVE_HRESULT(hr);
    return hr;
}

/**
 * Called when the state of a View changes.
 */
HRESULT touchmind::ribbon::RibbonCommandHandler::OnViewChanged(
    UINT32 nViewID,
    UI_VIEWTYPE typeID,
    IUnknown* pView,
    UI_VIEWVERB verb,
    INT32 uReasonCode)
{
    LOG_ENTER;
    UNREFERENCED_PARAMETER(nViewID);
    UNREFERENCED_PARAMETER(uReasonCode);

    HRESULT hr = E_NOTIMPL;
    if (UI_VIEWTYPE_RIBBON == typeID) {
        switch (verb) {
        case UI_VIEWVERB_CREATE:
            if (nullptr == m_pRibbon) {
                hr = pView->QueryInterface(&m_pRibbon);
            }
            break;
        case UI_VIEWVERB_SIZE:
            if (m_pRibbonFramework != nullptr) {
                HWND hWnd = m_pRibbonFramework->GetHWnd();
                UINT ribbonHeight = m_pRibbonFramework->GetRibbonHeight();
                PostMessage(hWnd, WM_RIBBON_RESIZED, static_cast<WPARAM>(ribbonHeight), 0);
            }
            break;
        case UI_VIEWVERB_DESTROY:
            m_pRibbon = nullptr;
            hr = S_OK;
            break;
        }
    }
    LOG_LEAVE_HRESULT(hr);
    return hr;
}

/**
 * Called for each Command specified in the Windows Ribbon framework markup
 * when the application window is destroyed.
 */
HRESULT touchmind::ribbon::RibbonCommandHandler::OnDestroyUICommand(
    UINT32 commandId,
    UI_COMMANDTYPE typeID,
    IUICommandHandler* pCommandHandler)
{
    UNREFERENCED_PARAMETER(commandId);
    UNREFERENCED_PARAMETER(typeID);
    UNREFERENCED_PARAMETER(pCommandHandler);

    return E_NOTIMPL;
}
// IUIApplication (end)

// IUICommandHandler (start)
/**
 * Responds to execute events on Commands bound to the Command handler.
 */
HRESULT touchmind::ribbon::RibbonCommandHandler::Execute(
    UINT nCmdID,
    UI_EXECUTIONVERB verb,
    const PROPERTYKEY* key,
    const PROPVARIANT* pPropvarValue,
    IUISimplePropertySet* pCommandExecutionProperties)
{
    LOG_ENTER_ARG(L"nCmdID = " << nCmdID);
    HRESULT hr = S_OK;

    if (m_executeListeners.count(nCmdID) > 0) {
        hr = m_executeListeners[nCmdID](nCmdID, verb, key, pPropvarValue, pCommandExecutionProperties);
    }
    LOG_LEAVE_HRESULT(hr);
    return hr;
}

/**
 * Responds to property update requests from the Windows Ribbon framework.
 */
STDMETHODIMP touchmind::ribbon::RibbonCommandHandler::UpdateProperty(
    UINT nCmdID,
    REFPROPERTYKEY key,
    const PROPVARIANT* pPropvarCurrentValue,
    PROPVARIANT* pPropvarNewValue)
{
    LOG_ENTER_ARG(L"nCmdID = " << nCmdID);
    HRESULT hr = E_NOTIMPL;
    if (m_updatePropertyListeners.count(nCmdID) > 0) {
        hr = m_updatePropertyListeners[nCmdID](nCmdID, key, pPropvarCurrentValue, pPropvarNewValue);
    }
    LOG_LEAVE_HRESULT(hr);
    return hr;
}
// IUICommandHandler (end)
