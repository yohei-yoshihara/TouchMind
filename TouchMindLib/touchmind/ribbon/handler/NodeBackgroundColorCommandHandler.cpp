#include "stdafx.h"
#include "resource.h"
#include "touchmind/Common.h"
#include "touchmind/Context.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/ribbon/dispatch/RibbonRequestDispatcher.h"
#include "touchmind/ribbon/RibbonFramework.h"
#include "touchmind/ribbon/PropertySet.h"
#include "touchmind/util/BitmapHelper.h"
#include "touchmind/util/ColorUtil.h"
#include "touchmind/model/CurvePoints.h"
#include "touchmind/view/GeometryBuilder.h"
#include "touchmind/view/node/NodeViewManager.h"
#include "touchmind/view/node/NodeFigureHelper.h"
#include "touchmind/ribbon/handler/NodeBackgroundColorCommandHandler.h"

HRESULT touchmind::ribbon::handler::NodeBackgroundColorCommandHandler::_CreateUIImage( D2D1_COLOR_F color, IUIImage **ppUIImage )
{
    LOG_ENTER;
    static UINT dpiList[] = {96, 120, 144, 192};
    static UINT sizeList[] = {32, 40, 48, 64};

    HRESULT hr = S_OK;

    FLOAT dpiX, dpiY;
    m_pContext->GetD2DFactory()->GetDesktopDpi(&dpiX, &dpiY);
    UINT width = 0, height = 0;
    for (UINT i = 0; i < ARRAYSIZE(dpiList); ++i) {
        if (static_cast<UINT>(dpiX) <= dpiList[i]) {
            width = height = sizeList[i];
            break;
        }
    }
    if (width == 0) {
        width = height = 64;
    }

    CComPtr<IWICBitmap> pWICBitmap = nullptr;
    CHK_RES(pWICBitmap,
            touchmind::util::BitmapHelper::CreateBitmap(
                m_pContext->GetWICImagingFactory(),
                static_cast<UINT>(width),
                static_cast<UINT>(height),
                &pWICBitmap));

    CComPtr<ID2D1RenderTarget> pRenderTarget = nullptr;
    CHK_RES(pRenderTarget,
            touchmind::util::BitmapHelper::CreateBitmapRenderTarget(
                pWICBitmap,
                m_pContext->GetD2DFactory(),
                &pRenderTarget));

    pRenderTarget->BeginDraw();
    COLORREF colorref = util::ColorUtil::ToColorref(color);
    D2D1_COLOR_F startColor;
    D2D1_COLOR_F endColor;
    touchmind::view::node::NodeFigureHelper plateFigure;
    plateFigure.SetStrokeColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f));
    touchmind::view::node::NodeViewManager::CreateBodyColor(colorref, startColor, endColor);
    plateFigure.SetPlateStartColor(startColor);
    plateFigure.SetPlateEndColor(endColor);
    plateFigure.DrawPlate(D2D1::RectF(0.0f, 0.0f, static_cast<FLOAT>(width), static_cast<FLOAT>(height) * 0.7f),
                          plateFigure.GetCornerRoundSize(),
                          m_pContext->GetD2DFactory(), pRenderTarget);
    pRenderTarget->EndDraw();

    HBITMAP hBitmap = nullptr;
    touchmind::util::BitmapHelper::CreateBitmapFromWICBitmapSource(pWICBitmap, &hBitmap);

    hr = m_pRibbonFramework->GetUIImageFromBitmap()->CreateImage(hBitmap, UI_OWNERSHIP_TRANSFER, ppUIImage);
    LOG_LEAVE;
    return hr;
}

touchmind::ribbon::handler::NodeBackgroundColorCommandHandler::NodeBackgroundColorCommandHandler() :
    m_refCount(0),
    m_pContext(nullptr),
    m_pRibbonFramework(nullptr),
    m_pRibbonRequestDispatcher(nullptr)
{
}

touchmind::ribbon::handler::NodeBackgroundColorCommandHandler::NodeBackgroundColorCommandHandler(
    touchmind::Context *pContext,
    touchmind::ribbon::RibbonFramework *pRibbonFramework,
    touchmind::ribbon::dispatch::RibbonRequestDispatcher *pRibbonRequestDispatcher) :
    m_refCount(0),
    m_pContext(pContext),
    m_pRibbonFramework(pRibbonFramework),
    m_pRibbonRequestDispatcher(pRibbonRequestDispatcher)
{
}

touchmind::ribbon::handler::NodeBackgroundColorCommandHandler::~NodeBackgroundColorCommandHandler()
{
}

HRESULT touchmind::ribbon::handler::NodeBackgroundColorCommandHandler::CreateInstance(IUICommandHandler **ppCommandHandler)
{
    HRESULT hr = S_OK;
    if (!ppCommandHandler) {
        hr = E_POINTER;
    } else {
        *ppCommandHandler = nullptr;
        NodeBackgroundColorCommandHandler *pHandler = new NodeBackgroundColorCommandHandler();
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

IFACEMETHODIMP_(ULONG) touchmind::ribbon::handler::NodeBackgroundColorCommandHandler::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

IFACEMETHODIMP_(ULONG) touchmind::ribbon::handler::NodeBackgroundColorCommandHandler::Release()
{
    long refCount = InterlockedDecrement(&m_refCount);
    if (refCount == 0) {
        delete this;
    }
    return refCount;
}

IFACEMETHODIMP touchmind::ribbon::handler::NodeBackgroundColorCommandHandler::QueryInterface(REFIID riid, void** ppInterface)
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

IFACEMETHODIMP touchmind::ribbon::handler::NodeBackgroundColorCommandHandler::Execute(
    UINT cmdID,
    UI_EXECUTIONVERB verb,
    const PROPERTYKEY* pKey,
    const PROPVARIANT* pPropvarValue,
    IUISimplePropertySet* pCommandExecutionProperties)
{
    UNREFERENCED_PARAMETER(cmdID);
    UNREFERENCED_PARAMETER(pCommandExecutionProperties);

    LOG(SEVERITY_LEVEL_DEBUG) << L"key = " << *pKey;

    HRESULT hr = E_FAIL;
    if (pKey && *pKey == UI_PKEY_ColorType) {
        UINT color = 0;
        UINT type = UI_SWATCHCOLORTYPE_NOCOLOR;
        // The Ribbon framework passes color type as the primary property.
        if (pPropvarValue != nullptr) {
            // Retrieve color type.
            hr = UIPropertyToUInt32(UI_PKEY_ColorType, *pPropvarValue, &type);
            if (FAILED(hr)) {
                return hr;
            }
        }

        // The Ribbon framework passes color as additional property if the color type is RGB.
        if (type == UI_SWATCHCOLORTYPE_RGB && pCommandExecutionProperties != nullptr) {
            // Retrieve color.
            PROPVARIANT var;
            hr = pCommandExecutionProperties->GetValue(UI_PKEY_Color, &var);
            if (FAILED(hr)) {
                return hr;
            }
            UIPropertyToUInt32(UI_PKEY_Color, var, &color);
        }
        D2D1_COLOR_F colorF = touchmind::util::ColorUtil::ToColorF(static_cast<COLORREF>(color));
        m_pRibbonRequestDispatcher->Execute_NodeBackgroundColor(verb, colorF);
        m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_LargeImage);
    }
    return hr;
}

IFACEMETHODIMP touchmind::ribbon::handler::NodeBackgroundColorCommandHandler::UpdateProperty(
    UINT cmdID,
    REFPROPERTYKEY key,
    const PROPVARIANT* pPropvarCurrentValue,
    PROPVARIANT* pPropvarNewValue)
{
    LOG_ENTER;
    UNREFERENCED_PARAMETER(cmdID);
    UNREFERENCED_PARAMETER(key);
    UNREFERENCED_PARAMETER(pPropvarCurrentValue);
    UNREFERENCED_PARAMETER(pPropvarNewValue);

    HRESULT hr = E_FAIL;
    if (key == UI_PKEY_Enabled) {
        BOOL enabled = m_pRibbonRequestDispatcher->UpdateProperty_IsNodeBackgroundColorChangeable() ? TRUE : FALSE;
        m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_ColorType);
        m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_Color);
        m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_LargeImage);
        hr = UIInitPropertyFromBoolean(UI_PKEY_BooleanValue, enabled, pPropvarNewValue);
    } else if (key == UI_PKEY_ColorType) {
        hr = UIInitPropertyFromUInt32(key, UI_SWATCHCOLORTYPE_RGB, pPropvarNewValue);
    } else if (key == UI_PKEY_Color) {
        if (pPropvarCurrentValue == nullptr) {
            hr = UIInitPropertyFromUInt32(key, RGB(255, 255, 255), pPropvarNewValue);
        } else {
            D2D1_COLOR_F colorF = m_pRibbonRequestDispatcher->UpdateProperty_GetNodeBackgroundColor();
            COLORREF colorref = touchmind::util::ColorUtil::ToColorref(colorF);
            hr = UIInitPropertyFromUInt32(key, colorref, pPropvarNewValue);
        }
    } else if (key == UI_PKEY_LargeImage) {
        if (pPropvarCurrentValue != nullptr) {
            CComPtr<IUIImage> uiImage = nullptr;
            D2D1_COLOR_F colorF = m_pRibbonRequestDispatcher->UpdateProperty_GetNodeBackgroundColor();
            hr = _CreateUIImage(colorF, &uiImage);
            CHK_RES(uiImage, S_OK);
            if (SUCCEEDED(hr)) {
                hr = UIInitPropertyFromImage(key, uiImage, pPropvarNewValue);
            }
        }
    }
    LOG_LEAVE;
    return hr;
}
