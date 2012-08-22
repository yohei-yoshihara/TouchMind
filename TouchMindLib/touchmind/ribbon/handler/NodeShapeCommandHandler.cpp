#include "stdafx.h"
#include "resource.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/ribbon/dispatch/RibbonRequestDispatcher.h"
#include "touchmind/ribbon/RibbonFramework.h"
#include "touchmind/ribbon/PropertySet.h"
#include "touchmind/ribbon/handler/NodeShapeCommandHandler.h"

const std::array<int, NUMBER_OF_NODE_SHAPES> touchmind::ribbon::handler::NodeShapeCommandHandler::s_IMAGE_ID = {
    IDB_NODESTYLE_ROUNDEDRECT,
    IDB_NODESTYLE_RECTANGLE,
};

UINT32 touchmind::ribbon::handler::NodeShapeCommandHandler::_ConvertToIndex(NODE_SHAPE nodeShape)
{
    UINT32 index = static_cast<UINT32>(nodeShape);
    if (0 <= index && index < NUMBER_OF_NODE_SHAPES) {
        return index;
    }
    return 0;
}

touchmind::NODE_SHAPE touchmind::ribbon::handler::NodeShapeCommandHandler::_ConvertToNodeShape(UINT32 index)
{
    return static_cast<NODE_SHAPE>(index);
}

touchmind::ribbon::handler::NodeShapeCommandHandler::NodeShapeCommandHandler() :
    m_refCount(0),
    m_pRibbonFramework(nullptr),
    m_pRibbonRequestDispatcher(nullptr),
    m_pImages()
{
    for (size_t i = 0; i < m_pImages.size(); ++i) {
        m_pImages[i] = nullptr;
    }
}

touchmind::ribbon::handler::NodeShapeCommandHandler::NodeShapeCommandHandler(
    touchmind::ribbon::RibbonFramework *pRibbonFramework,
    touchmind::ribbon::dispatch::RibbonRequestDispatcher *pRibbonRequestDispatcher) :
    m_refCount(0),
    m_pRibbonFramework(pRibbonFramework),
    m_pRibbonRequestDispatcher(pRibbonRequestDispatcher),
    m_pImages()
{
    for (size_t i = 0; i < m_pImages.size(); ++i) {
        m_pImages[i] = nullptr;
    }
}

touchmind::ribbon::handler::NodeShapeCommandHandler::~NodeShapeCommandHandler()
{
}

HRESULT touchmind::ribbon::handler::NodeShapeCommandHandler::CreateInstance(
    touchmind::ribbon::RibbonFramework *pRibbonFramework,
    touchmind::ribbon::dispatch::RibbonRequestDispatcher *pRibbonRequestDispatcher,
    IUICommandHandler **ppCommandHandler)
{
    HRESULT hr = S_OK;
    if (!ppCommandHandler) {
        hr = E_POINTER;
    } else {
        *ppCommandHandler = nullptr;
        NodeShapeCommandHandler *pHandler = new NodeShapeCommandHandler(pRibbonFramework, pRibbonRequestDispatcher);
        if (pHandler != nullptr) {
            *ppCommandHandler = pHandler;
            hr = S_OK;
        } else {
            hr = E_OUTOFMEMORY;
        }
    }
    return hr;
}

// IUnknown methods

IFACEMETHODIMP_(ULONG) touchmind::ribbon::handler::NodeShapeCommandHandler::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

IFACEMETHODIMP_(ULONG) touchmind::ribbon::handler::NodeShapeCommandHandler::Release()
{
    LONG refCount = InterlockedDecrement(&m_refCount);
    if (refCount == 0) {
        delete this;
    }
    return refCount;
}

IFACEMETHODIMP touchmind::ribbon::handler::NodeShapeCommandHandler::QueryInterface(REFIID riid, void** ppInterface)
{
    if (riid == __uuidof(IUnknown)) {
        *ppInterface = static_cast<IUnknown*>(this);
    } else if (riid == __uuidof(IUICommandHandler)) {
        *ppInterface = static_cast<IUICommandHandler*>(this);
    } else {
        *ppInterface = nullptr;
        return E_NOINTERFACE;
    }
    (static_cast<IUnknown*>(*ppInterface))->AddRef();
    return S_OK;
}

IFACEMETHODIMP touchmind::ribbon::handler::NodeShapeCommandHandler::Execute(
    UINT cmdID,
    UI_EXECUTIONVERB verb,
    const PROPERTYKEY* pKey,
    const PROPVARIANT* pPropvarValue,
    IUISimplePropertySet* pCommandExecutionProperties)
{
    UNREFERENCED_PARAMETER(cmdID);
    UNREFERENCED_PARAMETER(pCommandExecutionProperties);

    HRESULT hr = E_FAIL;
    if ( pKey && *pKey == UI_PKEY_SelectedItem) {
        UINT selected;
        hr = UIPropertyToUInt32(*pKey, *pPropvarValue, &selected);
        NODE_SHAPE nodeShape = _ConvertToNodeShape(selected);
        m_pRibbonRequestDispatcher->Execute_NodeShape(verb, nodeShape);
    }
    return hr;
}

IFACEMETHODIMP touchmind::ribbon::handler::NodeShapeCommandHandler::UpdateProperty(
    UINT cmdID,
    REFPROPERTYKEY key,
    const PROPVARIANT* pPropvarCurrentValue,
    PROPVARIANT* pPropvarNewValue)
{
    UNREFERENCED_PARAMETER(cmdID);

    bool isImageInitialized = std::all_of(
                                  std::begin(m_pImages),
                                  std::end(m_pImages),
    [](CComPtr<IUIImage> &pImage) {
        return pImage != nullptr;
    });
    if (!isImageInitialized) {
        for (size_t i = 0; i < s_IMAGE_ID.size(); ++i) {
            m_pImages[i] = nullptr;
            CHK_HR(
                m_pRibbonFramework->CreateUIImageFromBitmapResource(MAKEINTRESOURCE(s_IMAGE_ID[i]), &m_pImages[i]));
        }
    }

    HRESULT hr = E_FAIL;
    if (key == UI_PKEY_Enabled) {
        BOOL enabled = m_pRibbonRequestDispatcher->UpdateProperty_IsNodeShapeChangeable() ? TRUE : FALSE;
        m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdNodeShape, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_SelectedItem);
        hr = UIInitPropertyFromBoolean(UI_PKEY_BooleanValue, enabled, pPropvarNewValue);
    } else if (key == UI_PKEY_Categories) {
        hr = S_FALSE;
    } else if (UI_PKEY_ItemsSource == key) {
        CComPtr<IUICollection> collection = nullptr;
        CHK_HR(pPropvarCurrentValue->punkVal->QueryInterface(IID_PPV_ARGS(&collection)));
        for (size_t i = 0; i < s_IMAGE_ID.size(); ++i) {
            CComPtr<PropertySet> item(new PropertySet());
            //CComPtr<IUIImage> pImage = nullptr;
            //CHK_HR(
            //    m_pRibbonFramework->CreateUIImageFromBitmapResource(MAKEINTRESOURCE(s_IMAGE_ID[i]), &pImage));
            //item->SetImage(pImage);
            item->SetImage(m_pImages[i]);
            collection->Add(item);
            hr = S_OK;
        }
    } else if (key == UI_PKEY_SelectedItem) {
        NODE_SHAPE nodeShape = m_pRibbonRequestDispatcher->UpdateProperty_GetNodeShape();
        hr = UIInitPropertyFromUInt32(UI_PKEY_SelectedItem, _ConvertToIndex(nodeShape), pPropvarNewValue);
    } else if (key == UI_PKEY_LargeImage) {
        NODE_SHAPE nodeShape = m_pRibbonRequestDispatcher->UpdateProperty_GetNodeShape();
        hr = UIInitPropertyFromImage(key, m_pImages[_ConvertToIndex(nodeShape)], pPropvarNewValue);
    }
    return hr;
}
