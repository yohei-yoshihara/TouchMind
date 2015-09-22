#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/ribbon/PropertySet.h"

touchmind::ribbon::PropertySet::PropertySet()
    : m_refCount(0)
    , m_label()
    , m_image(nullptr)
    , m_categoryId(UI_COLLECTION_INVALIDINDEX)
    , m_commandId(-1)
    , m_commandType(UI_COMMANDTYPE_UNKNOWN) {
}

touchmind::ribbon::PropertySet::~PropertySet() {
}

// IUnknown methods

IFACEMETHODIMP_(ULONG) touchmind::ribbon::PropertySet::AddRef() {
  return InterlockedIncrement(&m_refCount);
}

IFACEMETHODIMP_(ULONG) touchmind::ribbon::PropertySet::Release() {
  long val = InterlockedDecrement(&m_refCount);
  if (val == 0) {
    delete this;
  }
  return val;
}

IFACEMETHODIMP touchmind::ribbon::PropertySet::QueryInterface(REFIID riid, void **ppInterface) {
  if (riid == __uuidof(IUnknown)) {
    *ppInterface = static_cast<IUnknown *>(this);
  } else if (riid == __uuidof(IUISimplePropertySet)) {
    *ppInterface = static_cast<IUISimplePropertySet *>(this);
  } else {
    *ppInterface = nullptr;
    return E_NOINTERFACE;
  }
  (static_cast<IUnknown *>(*ppInterface))->AddRef();
  return S_OK;
}

// IUISimplePropertySet interface

STDMETHODIMP touchmind::ribbon::PropertySet::GetValue(__in REFPROPERTYKEY key, __out PROPVARIANT *ppropvar) {
  UNREFERENCED_PARAMETER(key);
  UNREFERENCED_PARAMETER(ppropvar);

  if (key == UI_PKEY_ItemImage) {
    if (m_image != nullptr) {
      return UIInitPropertyFromImage(UI_PKEY_ItemImage, m_image, ppropvar);
    }
    return S_FALSE;
  } else if (key == UI_PKEY_Label) {
    return UIInitPropertyFromString(UI_PKEY_Label, m_label.c_str(), ppropvar);
  } else if (key == UI_PKEY_CategoryId) {
    return UIInitPropertyFromUInt32(UI_PKEY_CategoryId, m_categoryId, ppropvar);
  } else if (key == UI_PKEY_CommandId) {
    if (m_commandId != -1) {
      return UIInitPropertyFromUInt32(UI_PKEY_CommandId, m_commandId, ppropvar);
    }
    return S_FALSE;
  } else if (key == UI_PKEY_CommandType) {
    return UIInitPropertyFromUInt32(UI_PKEY_CommandType, m_commandType, ppropvar);
  }
  return E_FAIL;
}