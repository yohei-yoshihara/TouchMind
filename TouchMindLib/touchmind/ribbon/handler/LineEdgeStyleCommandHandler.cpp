#include "stdafx.h"
#include "resource.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/ribbon/dispatch/RibbonRequestDispatcher.h"
#include "touchmind/ribbon/RibbonFramework.h"
#include "touchmind/ribbon/PropertySet.h"
#include "touchmind/ribbon/handler/LineEdgeStyleCommandHandler.h"

const std::array<int, NUMBER_OF_LINE_EDGE_STYLES> touchmind::ribbon::handler::LineEdgeStyleCommandHandler::s_IMAGE_ID
    = {
        IDB_LINE_EDGE_STYLE_NORMAL,      IDB_LINE_EDGE_STYLE_EDGE1_ARROW,  IDB_LINE_EDGE_STYLE_EDGE2_ARROW,
        IDB_LINE_EDGE_STYLE_BOTH_ARROW,  IDB_LINE_EDGE_STYLE_EDGE1_CIRCLE, IDB_LINE_EDGE_STYLE_EDGE2_CIRCLE,
        IDB_LINE_EDGE_STYLE_BOTH_CIRCLE, IDB_LINE_EDGE_STYLE_CIRCLE_ARROW, IDB_LINE_EDGE_STYLE_ARROW_CIRCLE,
};

const std::array<int, NUMBER_OF_LINE_EDGE_STYLES>
    touchmind::ribbon::handler::LineEdgeStyleCommandHandler::s_IMAGE36x36_ID = {
        IDB_LINE_EDGE_STYLE_NORMAL36x36,       IDB_LINE_EDGE_STYLE_EDGE1_ARROW36x36,
        IDB_LINE_EDGE_STYLE_EDGE2_ARROW36x36,  IDB_LINE_EDGE_STYLE_BOTH_ARROW36x36,
        IDB_LINE_EDGE_STYLE_EDGE1_CIRCLE36x36, IDB_LINE_EDGE_STYLE_EDGE2_CIRCLE36x36,
        IDB_LINE_EDGE_STYLE_BOTH_CIRCLE36x36,  IDB_LINE_EDGE_STYLE_CIRCLE_ARROW36x36,
        IDB_LINE_EDGE_STYLE_ARROW_CIRCLE36x36,
};

bool touchmind::ribbon::handler::LineEdgeStyleCommandHandler::imageInitialized = false;

touchmind::ribbon::handler::LineEdgeStyleCommandHandler::LineEdgeStyleCommandHandler()
    : m_refCount(0)
    , m_pRibbonFramework(nullptr)
    , m_pRibbonRequestDispatcher(nullptr)
    , m_pImages()
    , m_pImages36x36() {
  for (size_t i = 0; i < m_pImages.size(); ++i) {
    m_pImages[i] = nullptr;
    m_pImages36x36[i] = nullptr;
  }
}

touchmind::ribbon::handler::LineEdgeStyleCommandHandler::LineEdgeStyleCommandHandler(
    touchmind::ribbon::RibbonFramework *pRibbonFramework,
    touchmind::ribbon::dispatch::RibbonRequestDispatcher *pRibbonRequestDispatcher)
    : m_refCount(0)
    , m_pRibbonFramework(pRibbonFramework)
    , m_pRibbonRequestDispatcher(pRibbonRequestDispatcher)
    , m_pImages()
    , m_pImages36x36() {
  for (size_t i = 0; i < m_pImages.size(); ++i) {
    m_pImages[i] = nullptr;
    m_pImages36x36[i] = nullptr;
  }
}

touchmind::ribbon::handler::LineEdgeStyleCommandHandler::~LineEdgeStyleCommandHandler() {
}

HRESULT touchmind::ribbon::handler::LineEdgeStyleCommandHandler::CreateInstance(IUICommandHandler **ppCommandHandler) {
  HRESULT hr = S_OK;
  if (!ppCommandHandler) {
    hr = E_POINTER;
  } else {
    *ppCommandHandler = nullptr;
    LineEdgeStyleCommandHandler *pHandler = new LineEdgeStyleCommandHandler();
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

IFACEMETHODIMP_(ULONG) touchmind::ribbon::handler::LineEdgeStyleCommandHandler::AddRef() {
  return InterlockedIncrement(&m_refCount);
}

IFACEMETHODIMP_(ULONG) touchmind::ribbon::handler::LineEdgeStyleCommandHandler::Release() {
  long val = InterlockedDecrement(&m_refCount);
  if (val == 0) {
    delete this;
  }
  return val;
}

IFACEMETHODIMP touchmind::ribbon::handler::LineEdgeStyleCommandHandler::QueryInterface(REFIID riid,
                                                                                       void **ppInterface) {
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

IFACEMETHODIMP touchmind::ribbon::handler::LineEdgeStyleCommandHandler::Execute(
    UINT cmdID, UI_EXECUTIONVERB verb, const PROPERTYKEY *pKey, const PROPVARIANT *pPropvarValue,
    IUISimplePropertySet *pCommandExecutionProperties) {
  UNREFERENCED_PARAMETER(cmdID);
  UNREFERENCED_PARAMETER(pCommandExecutionProperties);

  HRESULT hr = E_FAIL;
  if (pKey && *pKey == UI_PKEY_SelectedItem) {
    UINT selected;
    hr = UIPropertyToUInt32(*pKey, *pPropvarValue, &selected);
    LINE_EDGE_STYLE lineEdgeStyle = _ConvertToLineEdgeStyle(selected);
    m_pRibbonRequestDispatcher->Execute_LineEdgeStyle(verb, lineEdgeStyle);
    m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineEdgeStyle, UI_INVALIDATIONS_PROPERTY,
                                                            &UI_PKEY_LargeImage);
  }
  return hr;
}

IFACEMETHODIMP touchmind::ribbon::handler::LineEdgeStyleCommandHandler::UpdateProperty(
    UINT cmdID, REFPROPERTYKEY key, const PROPVARIANT *pPropvarCurrentValue, PROPVARIANT *pPropvarNewValue) {
  UNREFERENCED_PARAMETER(cmdID);

  if (!imageInitialized) {
    for (size_t i = 0; i < s_IMAGE_ID.size(); ++i) {
      m_pImages[i] = nullptr;
      CHK_HR(m_pRibbonFramework->CreateUIImageFromBitmapResource(MAKEINTRESOURCE(s_IMAGE_ID[i]), &m_pImages[i]));
      m_pImages36x36[i] = nullptr;
      CHK_HR(
          m_pRibbonFramework->CreateUIImageFromBitmapResource(MAKEINTRESOURCE(s_IMAGE36x36_ID[i]), &m_pImages36x36[i]));
    }
  }

  HRESULT hr = E_FAIL;
  if (key == UI_PKEY_Enabled) {
    BOOL enabled = m_pRibbonRequestDispatcher->UpdateProperty_IsLineEdgeStyleChangeable() ? TRUE : FALSE;
    m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineEdgeStyle, UI_INVALIDATIONS_PROPERTY,
                                                            &UI_PKEY_SelectedItem);
    m_pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineEdgeStyle, UI_INVALIDATIONS_PROPERTY,
                                                            &UI_PKEY_LargeImage);
    hr = UIInitPropertyFromBoolean(UI_PKEY_BooleanValue, enabled, pPropvarNewValue);
  } else if (key == UI_PKEY_Categories) {
    hr = S_FALSE;
  } else if (UI_PKEY_ItemsSource == key) {
    CComPtr<IUICollection> collection = nullptr;
    CHK_HR(pPropvarCurrentValue->punkVal->QueryInterface(IID_PPV_ARGS(&collection)));
    for (size_t i = 0; i < s_IMAGE_ID.size(); ++i) {
      CComPtr<PropertySet> item(new PropertySet());
      item->SetImage(m_pImages[i]);
      collection->Add(item);
      hr = S_OK;
    }
  } else if (key == UI_PKEY_SelectedItem) {
    LINE_EDGE_STYLE lineEdgeStyle = m_pRibbonRequestDispatcher->UpdateProperty_GetLineEdgeStyle();
    hr = UIInitPropertyFromUInt32(UI_PKEY_SelectedItem, _ConvertToIndex(lineEdgeStyle), pPropvarNewValue);
  } else if (key == UI_PKEY_LargeImage) {
    LINE_EDGE_STYLE lineEdgeStyle = m_pRibbonRequestDispatcher->UpdateProperty_GetLineEdgeStyle();
    hr = UIInitPropertyFromImage(key, m_pImages36x36[_ConvertToIndex(lineEdgeStyle)], pPropvarNewValue);
  }
  return hr;
}

UINT32 touchmind::ribbon::handler::LineEdgeStyleCommandHandler::_ConvertToIndex(LINE_EDGE_STYLE lineEdgeStyle) {
  switch (lineEdgeStyle) {
  case LINE_EDGE_STYLE_NORMAL:
    return 0;
  case LINE_EDGE_STYLE_EDGE1_ARROW:
    return 1;
  case LINE_EDGE_STYLE_EDGE2_ARROW:
    return 2;
  case LINE_EDGE_STYLE_BOTH_ARROW:
    return 3;
  case LINE_EDGE_STYLE_EDGE1_CIRCLE:
    return 4;
  case LINE_EDGE_STYLE_EDGE2_CIRCLE:
    return 5;
  case LINE_EDGE_STYLE_BOTH_CIRCLE:
    return 6;
  case LINE_EDGE_STYLE_CIRCLE_ARROW:
    return 7;
  case LINE_EDGE_STYLE_ARROW_CIRCLE:
    return 8;
  }
  return 0;
}

touchmind::LINE_EDGE_STYLE
touchmind::ribbon::handler::LineEdgeStyleCommandHandler::_ConvertToLineEdgeStyle(UINT32 index) {
  const static LINE_EDGE_STYLE LINE_EDGE_STYLE_LIST[] = {
      LINE_EDGE_STYLE_NORMAL,      LINE_EDGE_STYLE_EDGE1_ARROW,  LINE_EDGE_STYLE_EDGE2_ARROW,
      LINE_EDGE_STYLE_BOTH_ARROW,  LINE_EDGE_STYLE_EDGE1_CIRCLE, LINE_EDGE_STYLE_EDGE2_CIRCLE,
      LINE_EDGE_STYLE_BOTH_CIRCLE, LINE_EDGE_STYLE_CIRCLE_ARROW, LINE_EDGE_STYLE_ARROW_CIRCLE,
  };
  return LINE_EDGE_STYLE_LIST[index];
}
