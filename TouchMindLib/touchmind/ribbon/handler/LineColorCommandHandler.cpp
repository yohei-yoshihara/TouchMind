#include "stdafx.h"
#include "resource.h"
#include "touchmind/Common.h"
#include "touchmind/Context.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/ribbon/dispatch/RibbonRequestDispatcher.h"
#include "touchmind/ribbon/RibbonFramework.h"
#include "touchmind/ribbon/PropertySet.h"
#include "touchmind/ribbon/handler/LineColorCommandHandler.h"
#include "touchmind/util/BitmapHelper.h"
#include "touchmind/util/ColorUtil.h"
#include "touchmind/model/CurvePoints.h"
#include "touchmind/view/GeometryBuilder.h"

touchmind::ribbon::handler::LineColorCommandHandler::LineColorCommandHandler()
    : m_refCount(0)
    , m_pContext(nullptr)
    , m_pRibbonFramework(nullptr)
    , m_pRibbonRequestDispatcher(nullptr) {
}

touchmind::ribbon::handler::LineColorCommandHandler::LineColorCommandHandler(
    touchmind::Context *pContext, touchmind::ribbon::RibbonFramework *pRibbonFramework,
    touchmind::ribbon::dispatch::RibbonRequestDispatcher *pRibbonRequestDispatcher)
    : m_refCount(0)
    , m_pContext(pContext)
    , m_pRibbonFramework(pRibbonFramework)
    , m_pRibbonRequestDispatcher(pRibbonRequestDispatcher) {
}

touchmind::ribbon::handler::LineColorCommandHandler::~LineColorCommandHandler() {
}

HRESULT touchmind::ribbon::handler::LineColorCommandHandler::CreateInstance(IUICommandHandler **ppCommandHandler) {
  HRESULT hr = S_OK;
  if (!ppCommandHandler) {
    hr = E_POINTER;
  } else {
    *ppCommandHandler = nullptr;
    LineColorCommandHandler *pHandler = new LineColorCommandHandler();
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

IFACEMETHODIMP_(ULONG) touchmind::ribbon::handler::LineColorCommandHandler::AddRef() {
  return InterlockedIncrement(&m_refCount);
}

IFACEMETHODIMP_(ULONG) touchmind::ribbon::handler::LineColorCommandHandler::Release() {
  long refCount = InterlockedDecrement(&m_refCount);
  if (refCount == 0) {
    delete this;
  }
  return refCount;
}

IFACEMETHODIMP touchmind::ribbon::handler::LineColorCommandHandler::QueryInterface(REFIID riid, void **ppInterface) {
  if (riid == __uuidof(IUnknown)) {
    *ppInterface = static_cast<IUnknown *>(this);
  } else if (riid == __uuidof(IUICommandHandler)) {
    *ppInterface = static_cast<IUICommandHandler *>(this);
  } else {
    *ppInterface = nullptr;
    return E_NOINTERFACE;
  }
  (static_cast<IUnknown *>(*ppInterface))->AddRef();
  return S_OK;
}

IFACEMETHODIMP
touchmind::ribbon::handler::LineColorCommandHandler::Execute(UINT cmdID, UI_EXECUTIONVERB verb, const PROPERTYKEY *pKey,
                                                             const PROPVARIANT *pPropvarValue,
                                                             IUISimplePropertySet *pCommandExecutionProperties) {
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
    m_pRibbonRequestDispatcher->Execute_LineColor(verb, colorF);
    m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineColor, UI_INVALIDATIONS_PROPERTY,
                                                            &UI_PKEY_LargeImage);
  }
  return hr;
}

IFACEMETHODIMP touchmind::ribbon::handler::LineColorCommandHandler::UpdateProperty(
    UINT cmdID, REFPROPERTYKEY key, const PROPVARIANT *pPropvarCurrentValue, PROPVARIANT *pPropvarNewValue) {
  LOG_ENTER;

  UNREFERENCED_PARAMETER(cmdID);
  UNREFERENCED_PARAMETER(key);
  UNREFERENCED_PARAMETER(pPropvarCurrentValue);
  UNREFERENCED_PARAMETER(pPropvarNewValue);

  HRESULT hr = E_FAIL;
  if (key == UI_PKEY_Enabled) {
    BOOL enabled = m_pRibbonRequestDispatcher->UpdateProperty_IsLineColorChangeable() ? TRUE : FALSE;
    m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineColor, UI_INVALIDATIONS_PROPERTY,
                                                            &UI_PKEY_ColorType);
    m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_Color);
    m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineColor, UI_INVALIDATIONS_PROPERTY,
                                                            &UI_PKEY_LargeImage);
    hr = UIInitPropertyFromBoolean(UI_PKEY_BooleanValue, enabled, pPropvarNewValue);
  } else if (key == UI_PKEY_ColorType) {
    hr = UIInitPropertyFromUInt32(key, UI_SWATCHCOLORTYPE_RGB, pPropvarNewValue);
  } else if (key == UI_PKEY_Color) {
    if (pPropvarCurrentValue == nullptr) {
      hr = UIInitPropertyFromUInt32(key, RGB(0, 0, 0), pPropvarNewValue);
    } else {
      D2D1_COLOR_F colorF = m_pRibbonRequestDispatcher->UpdateProperty_GetLineColor();
      COLORREF colorref = touchmind::util::ColorUtil::ToColorref(colorF);
      hr = UIInitPropertyFromUInt32(key, colorref, pPropvarNewValue);
    }
  } else if (key == UI_PKEY_LargeImage) {
    if (pPropvarCurrentValue != nullptr) {
      CComPtr<IUIImage> uiImage = nullptr;
      D2D1_COLOR_F colorF = m_pRibbonRequestDispatcher->UpdateProperty_GetLineColor();
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

HRESULT touchmind::ribbon::handler::LineColorCommandHandler::_CreateUIImage(D2D1_COLOR_F color, IUIImage **ppUIImage) {
  LOG_ENTER;
  const static FLOAT width = 48;
  const static FLOAT height = 48;
  const static FLOAT drawHeight = 32.0f;
  const static FLOAT strokeWidth = 5.0f;
  HRESULT hr = S_OK;

  if (m_pContext->GetWICImagingFactory() == nullptr || m_pContext->GetD2DFactory() == nullptr
      || m_pRibbonFramework->GetUIImageFromBitmap() == nullptr) {
    return E_FAIL;
  }
  CComPtr<IWICBitmap> pWICBitmap = nullptr;
  CHK_RES(pWICBitmap,
          touchmind::util::BitmapHelper::CreateBitmap(m_pContext->GetWICImagingFactory(), static_cast<UINT>(width),
                                                      static_cast<UINT>(height), &pWICBitmap));

  CComPtr<ID2D1RenderTarget> pRenderTarget = nullptr;
  CHK_RES(pRenderTarget, touchmind::util::BitmapHelper::CreateBitmapRenderTarget(
                             pWICBitmap, m_pContext->GetD2DFactory(), &pRenderTarget));

  model::CurvePoints curvePoints;
  curvePoints.SetX(0, 0.0f);
  curvePoints.SetY(0, 0.0f + strokeWidth / 2.0f);
  curvePoints.SetX(1, width / 2.0f);
  curvePoints.SetY(1, 0.0f);
  curvePoints.SetX(2, width / 2.0f);
  curvePoints.SetY(2, drawHeight - strokeWidth / 2.0f);
  curvePoints.SetX(3, width);
  curvePoints.SetY(3, drawHeight - strokeWidth / 2.0f);

  CComPtr<ID2D1PathGeometry> pGeometry = nullptr;
  CHK_RES(pGeometry,
          view::GeometryBuilder::CreateCurvePathGeometry(m_pContext->GetD2DFactory(), curvePoints, &pGeometry));

  pRenderTarget->BeginDraw();
  CComPtr<ID2D1SolidColorBrush> pBrush = nullptr;
  CHK_RES(pBrush, pRenderTarget->CreateSolidColorBrush(color, &pBrush));
  pRenderTarget->DrawGeometry(pGeometry, pBrush, strokeWidth);
  pRenderTarget->EndDraw();

  HBITMAP hBitmap = nullptr;
  touchmind::util::BitmapHelper::CreateBitmapFromWICBitmapSource(pWICBitmap, &hBitmap);

  hr = m_pRibbonFramework->GetUIImageFromBitmap()->CreateImage(hBitmap, UI_OWNERSHIP_TRANSFER, ppUIImage);

  LOG_LEAVE;
  return hr;
}
