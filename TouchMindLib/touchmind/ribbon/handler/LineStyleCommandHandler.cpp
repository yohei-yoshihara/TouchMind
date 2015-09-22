#include "stdafx.h"
#include "resource.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/ribbon/dispatch/RibbonRequestDispatcher.h"
#include "touchmind/ribbon/RibbonFramework.h"
#include "touchmind/ribbon/PropertySet.h"
#include "touchmind/ribbon/handler/LineStyleCommandHandler.h"

const std::array<int, NUMBER_OF_LINE_STYLES> touchmind::ribbon::handler::LineStyleCommandHandler::s_IMAGE_ID = {
    IDB_LINESTYLE_SOLID, IDB_LINESTYLE_DASHED, IDB_LINESTYLE_DOTTED,
};

const std::array<int, NUMBER_OF_LINE_STYLES> touchmind::ribbon::handler::LineStyleCommandHandler::s_IMAGE64x24_ID = {
    IDB_LINESTYLE_SOLID64x24, IDB_LINESTYLE_DASHED64x24, IDB_LINESTYLE_DOTTED64x24,
};

bool touchmind::ribbon::handler::LineStyleCommandHandler::s_imageInitialized = false;

touchmind::ribbon::handler::LineStyleCommandHandler::LineStyleCommandHandler()
    : m_refCount(0)
    , m_pRibbonFramework(nullptr)
    , m_pRibbonRequestDispatcher(nullptr)
    , m_pImages() {
  for (size_t i = 0; i < m_pImages.size(); ++i) {
    m_pImages[i] = nullptr;
    m_pImages64x24[i] = nullptr;
  }
}

touchmind::ribbon::handler::LineStyleCommandHandler::LineStyleCommandHandler(
    touchmind::ribbon::RibbonFramework *pRibbonFramework,
    touchmind::ribbon::dispatch::RibbonRequestDispatcher *pRibbonRequestDispatcher)
    : m_refCount(0)
    , m_pRibbonFramework(pRibbonFramework)
    , m_pRibbonRequestDispatcher(pRibbonRequestDispatcher)
    , m_pImages() {
  for (size_t i = 0; i < m_pImages.size(); ++i) {
    m_pImages[i] = nullptr;
    m_pImages64x24[i] = nullptr;
  }
}

touchmind::ribbon::handler::LineStyleCommandHandler::~LineStyleCommandHandler() {
}

HRESULT touchmind::ribbon::handler::LineStyleCommandHandler::CreateInstance(IUICommandHandler **ppCommandHandler) {
  HRESULT hr = S_OK;
  if (!ppCommandHandler) {
    hr = E_POINTER;
  } else {
    *ppCommandHandler = nullptr;
    LineStyleCommandHandler *pHandler = new LineStyleCommandHandler();
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

IFACEMETHODIMP_(ULONG) touchmind::ribbon::handler::LineStyleCommandHandler::AddRef() {
  return InterlockedIncrement(&m_refCount);
}

IFACEMETHODIMP_(ULONG) touchmind::ribbon::handler::LineStyleCommandHandler::Release() {
  long val = InterlockedDecrement(&m_refCount);
  if (val == 0) {
    delete this;
  }
  return val;
}

IFACEMETHODIMP touchmind::ribbon::handler::LineStyleCommandHandler::QueryInterface(REFIID riid, void **ppInterface) {
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
touchmind::ribbon::handler::LineStyleCommandHandler::Execute(UINT cmdID, UI_EXECUTIONVERB verb, const PROPERTYKEY *pKey,
                                                             const PROPVARIANT *pPropvarValue,
                                                             IUISimplePropertySet *pCommandExecutionProperties) {
  UNREFERENCED_PARAMETER(cmdID);
  UNREFERENCED_PARAMETER(pCommandExecutionProperties);

  HRESULT hr = E_FAIL;
  if (pKey && *pKey == UI_PKEY_SelectedItem) {
    UINT selected;
    hr = UIPropertyToUInt32(*pKey, *pPropvarValue, &selected);
    LINE_STYLE lineStyle = _ConvertToLineStyle(selected);
    m_pRibbonRequestDispatcher->Execute_LineStyle(verb, lineStyle);
    m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineStyle, UI_INVALIDATIONS_PROPERTY,
                                                            &UI_PKEY_LargeImage);
  }
  return hr;
}

IFACEMETHODIMP touchmind::ribbon::handler::LineStyleCommandHandler::UpdateProperty(
    UINT cmdID, REFPROPERTYKEY key, const PROPVARIANT *pPropvarCurrentValue, PROPVARIANT *pPropvarNewValue) {
  UNREFERENCED_PARAMETER(cmdID);
  if (!s_imageInitialized) {
    for (size_t i = 0; i < s_IMAGE_ID.size(); ++i) {
      m_pImages[i] = nullptr;
      CHK_HR(m_pRibbonFramework->CreateUIImageFromBitmapResource(MAKEINTRESOURCE(s_IMAGE_ID[i]), &m_pImages[i]));
      m_pImages64x24[i] = nullptr;
      CHK_HR(
          m_pRibbonFramework->CreateUIImageFromBitmapResource(MAKEINTRESOURCE(s_IMAGE64x24_ID[i]), &m_pImages64x24[i]));
    }
    s_imageInitialized = true;
  }

  HRESULT hr = E_FAIL;
  if (key == UI_PKEY_Enabled) {
    BOOL enabled = m_pRibbonRequestDispatcher->UpdateProperty_IsLineStyleChangeable() ? TRUE : FALSE;
    hr = UIInitPropertyFromBoolean(UI_PKEY_BooleanValue, enabled, pPropvarNewValue);
    m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineStyle, UI_INVALIDATIONS_PROPERTY,
                                                            &UI_PKEY_SelectedItem);
    m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineStyle, UI_INVALIDATIONS_PROPERTY,
                                                            &UI_PKEY_LargeImage);
  } else if (key == UI_PKEY_Categories) {
    hr = S_FALSE;
  } else if (UI_PKEY_ItemsSource == key) {
    CComPtr<IUICollection> collection = nullptr;
    CHK_HR(pPropvarCurrentValue->punkVal->QueryInterface(IID_PPV_ARGS(&collection)));
    for (size_t i = 0; i < s_IMAGE_ID.size(); ++i) {
      CComPtr<PropertySet> item(new PropertySet());
      item->SetImage(m_pImages64x24[i]);
      collection->Add(item);
      hr = S_OK;
    }
  } else if (key == UI_PKEY_SelectedItem) {
    LINE_STYLE lineStyle = m_pRibbonRequestDispatcher->UpdateProperty_GetLineStyle();
    hr = UIInitPropertyFromUInt32(UI_PKEY_SelectedItem, _ConvertToIndex(lineStyle), pPropvarNewValue);
  } else if (key == UI_PKEY_LargeImage) {
    LINE_STYLE lineStyle = m_pRibbonRequestDispatcher->UpdateProperty_GetLineStyle();
    hr = UIInitPropertyFromImage(key, m_pImages[_ConvertToIndex(lineStyle)], pPropvarNewValue);
  }
  return hr;
}

UINT32 touchmind::ribbon::handler::LineStyleCommandHandler::_ConvertToIndex(LINE_STYLE lineStyle) {
  switch (lineStyle) {
  case LINE_STYLE_SOLID:
    return 0;
  case LINE_STYLE_DASHED:
    return 1;
  case LINE_STYLE_DOTTED:
    return 2;
  }
  return 0;
}

touchmind::LINE_STYLE touchmind::ribbon::handler::LineStyleCommandHandler::_ConvertToLineStyle(UINT32 index) {
  const static LINE_STYLE LINE_STYLE_LIST[] = {LINE_STYLE_SOLID, LINE_STYLE_DASHED, LINE_STYLE_DOTTED};
  return LINE_STYLE_LIST[index];
}
