#include "stdafx.h"
#include "resource.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/ribbon/dispatch/RibbonRequestDispatcher.h"
#include "touchmind/ribbon/handler/LineWidthCommandHandler.h"
#include "touchmind/ribbon/RibbonFramework.h"
#include "touchmind/ribbon/PropertySet.h"

const std::array<int, 4> touchmind::ribbon::handler::LineWidthCommandHandler::s_IMAGE_ID = {
    IDB_LINEWIDTH_1, IDB_LINEWIDTH_3, IDB_LINEWIDTH_5, IDB_LINEWIDTH_8,
};

touchmind::ribbon::handler::LineWidthCommandHandler::LineWidthCommandHandler()
    : m_refCount(0)
    , m_pRibbonFramework(nullptr)
    , m_pRibbonRequestDispatcher(nullptr)
    , m_pImages() {
  for (size_t i = 0; i < m_pImages.size(); ++i) {
    m_pImages[i] = nullptr;
  }
}

touchmind::ribbon::handler::LineWidthCommandHandler::LineWidthCommandHandler(
    touchmind::ribbon::RibbonFramework *pRibbonFramework,
    touchmind::ribbon::dispatch::RibbonRequestDispatcher *pRibbonRequestDispatcher)
    : m_refCount(0)
    , m_pRibbonFramework(pRibbonFramework)
    , m_pRibbonRequestDispatcher(pRibbonRequestDispatcher)
    , m_pImages() {
  for (size_t i = 0; i < m_pImages.size(); ++i) {
    m_pImages[i] = nullptr;
  }
}

touchmind::ribbon::handler::LineWidthCommandHandler::~LineWidthCommandHandler() {
}

HRESULT touchmind::ribbon::handler::LineWidthCommandHandler::CreateInstance(IUICommandHandler **ppCommandHandler) {
  HRESULT hr = S_OK;
  if (!ppCommandHandler) {
    hr = E_POINTER;
  } else {
    *ppCommandHandler = nullptr;
    LineWidthCommandHandler *pHandler = new LineWidthCommandHandler();
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

IFACEMETHODIMP_(ULONG) touchmind::ribbon::handler::LineWidthCommandHandler::AddRef() {
  return InterlockedIncrement(&m_refCount);
}

IFACEMETHODIMP_(ULONG) touchmind::ribbon::handler::LineWidthCommandHandler::Release() {
  long val = InterlockedDecrement(&m_refCount);
  if (val == 0) {
    delete this;
  }
  return val;
}

IFACEMETHODIMP touchmind::ribbon::handler::LineWidthCommandHandler::QueryInterface(REFIID riid, void **ppInterface) {
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
touchmind::ribbon::handler::LineWidthCommandHandler::Execute(UINT cmdID, UI_EXECUTIONVERB verb, const PROPERTYKEY *pKey,
                                                             const PROPVARIANT *pPropvarValue,
                                                             IUISimplePropertySet *pCommandExecutionProperties) {
  UNREFERENCED_PARAMETER(cmdID);
  UNREFERENCED_PARAMETER(pCommandExecutionProperties);

  HRESULT hr = E_FAIL;
  if (pKey && *pKey == UI_PKEY_SelectedItem) {
    UINT selected;
    hr = UIPropertyToUInt32(*pKey, *pPropvarValue, &selected);
    LINE_WIDTH lineWidth = _ConvertToLineWidth(selected);
    m_pRibbonRequestDispatcher->Execute_LineWidth(verb, lineWidth);
    m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineWidth, UI_INVALIDATIONS_PROPERTY,
                                                            &UI_PKEY_LargeImage);
  }
  return hr;
}

IFACEMETHODIMP touchmind::ribbon::handler::LineWidthCommandHandler::UpdateProperty(
    UINT cmdID, REFPROPERTYKEY key, const PROPVARIANT *pPropvarCurrentValue, PROPVARIANT *pPropvarNewValue) {
  UNREFERENCED_PARAMETER(cmdID);

  bool isImageInitialized = std::all_of(m_pImages.begin(), m_pImages.end(),
                                        [](CComPtr<IUIImage> &pImage) { return pImage != nullptr; });
  if (!isImageInitialized) {
    for (size_t i = 0; i < s_IMAGE_ID.size(); ++i) {
      m_pImages[i] = nullptr;
      CHK_HR(m_pRibbonFramework->CreateUIImageFromBitmapResource(MAKEINTRESOURCE(s_IMAGE_ID[i]), &m_pImages[i]));
    }
  }

  HRESULT hr = E_FAIL;
  if (key == UI_PKEY_Enabled) {
    BOOL enabled = m_pRibbonRequestDispatcher->UpdateProperty_IsLineWidthChangeable() ? TRUE : FALSE;
    hr = UIInitPropertyFromBoolean(UI_PKEY_BooleanValue, enabled, pPropvarNewValue);
    m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineWidth, UI_INVALIDATIONS_PROPERTY,
                                                            &UI_PKEY_SelectedItem);
    m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineWidth, UI_INVALIDATIONS_PROPERTY,
                                                            &UI_PKEY_LargeImage);
  } else if (key == UI_PKEY_Categories) {
    hr = S_FALSE;
  } else if (UI_PKEY_ItemsSource == key) {
    CComPtr<IUICollection> collection = nullptr;
    CHK_HR(pPropvarCurrentValue->punkVal->QueryInterface(IID_PPV_ARGS(&collection)));
    for (size_t i = 0; i < s_IMAGE_ID.size(); ++i) {
      CComPtr<PropertySet> item(new PropertySet());
      // CComPtr<IUIImage> pImage = nullptr;
      // CHK_HR(
      //    m_pRibbonFramework->CreateUIImageFromBitmapResource(MAKEINTRESOURCE(s_IMAGE_ID[i]), &pImage));
      // item->SetImage(pImage);
      item->SetImage(m_pImages[i]);
      collection->Add(item);
      hr = S_OK;
    }
  } else if (key == UI_PKEY_SelectedItem) {
    LINE_WIDTH lineWidth = m_pRibbonRequestDispatcher->UpdateProperty_GetLineWidth();
    hr = UIInitPropertyFromUInt32(UI_PKEY_SelectedItem, _ConvertToIndex(lineWidth), pPropvarNewValue);
  } else if (key == UI_PKEY_LargeImage) {
    LINE_WIDTH lineWidth = m_pRibbonRequestDispatcher->UpdateProperty_GetLineWidth();
    // CComPtr<IUIImage> pImage = nullptr;
    // hr = m_pRibbonFramework->CreateUIImageFromBitmapResource(
    //    MAKEINTRESOURCE(s_IMAGE_ID[_ConvertToIndex(lineWidth)]), &pImage);
    // if (SUCCEEDED(hr)) {
    // hr = UIInitPropertyFromImage(key, pImage, pPropvarNewValue);
    hr = UIInitPropertyFromImage(key, m_pImages[_ConvertToIndex(lineWidth)], pPropvarNewValue);
    //}
  }
  return hr;
}

UINT32 touchmind::ribbon::handler::LineWidthCommandHandler::_ConvertToIndex(LINE_WIDTH lineWidth) {
  switch (lineWidth) {
  case LINE_WIDTH_1:
    return 0;
  case LINE_WIDTH_3:
    return 1;
  case LINE_WIDTH_5:
    return 2;
  case LINE_WIDTH_8:
    return 3;
  }
  return 0;
}

touchmind::LINE_WIDTH touchmind::ribbon::handler::LineWidthCommandHandler::_ConvertToLineWidth(UINT32 index) {
  const static LINE_WIDTH LINE_WIDTH_LIST[] = {LINE_WIDTH_1, LINE_WIDTH_3, LINE_WIDTH_5, LINE_WIDTH_8};
  return LINE_WIDTH_LIST[index];
}
