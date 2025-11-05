#include "stdafx.h"
#include <Shobjidl.h>
#include <propvarutil.h>
#include "touchmind/win/TouchMindApp.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/ribbon/RibbonFramework.h"
#include "touchmind/ribbon/RibbonCommandHandler.h"
#include "touchmind/MUI.h"

touchmind::ribbon::RibbonFramework::RibbonFramework()
    : m_pFramework(nullptr)
    , m_pRibbonCommandHandler(new touchmind::ribbon::RibbonCommandHandler())
    , m_resourceName(L"APPLICATION_RIBBON")
    , m_hWnd(0)
    , m_pifbFactory(nullptr) {}

touchmind::ribbon::RibbonFramework::~RibbonFramework() {
  Destroy();
}

HRESULT touchmind::ribbon::RibbonFramework::Initialize(HWND hWnd, touchmind::win::TouchMindApp *pTouchMindApp) {
  m_hWnd = hWnd;

  HRESULT hr = CoCreateInstance(CLSID_UIRibbonFramework, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pFramework));
  if (FAILED(hr)) {
    SPDLOG_ERROR(L"CoCreateInstance(CLSID_UIRibbonFramework) failed, hr = {:x}", hr);
    return hr;
  }

  hr = CoCreateInstance(CLSID_UIRibbonImageFromBitmapFactory, NULL, CLSCTX_ALL, IID_PPV_ARGS(&m_pifbFactory));
  if (FAILED(hr)) {
    SPDLOG_ERROR(L"CoCreateInstance(CLSID_UIRibbonImageFromBitmapFactory) failed, hr = {:x}", hr);
    return hr;
  }

  // hr = CComObject<touchmind::ribbon::RibbonCommandHandler>::CreateInstance(&m_pRibbonCommandHandler);
  // if (FAILED(hr)) {
  //    SPDLOG_ERROR(L"CComObject<Ribbon>::CreateInstance failed, hr = " << hr;
  //    return hr;
  //}
  m_pRibbonCommandHandler->SetRibbonFramework(this);
  m_pRibbonCommandHandler->SetTouchMindApp(pTouchMindApp);
  hr = m_pRibbonCommandHandler->Initialize();
  if (FAILED(hr)) {
    SPDLOG_ERROR(L"Ribbon::Initialize failed, hr = {:x}", hr);
    return hr;
  }

  hr = m_pFramework->Initialize(hWnd, m_pRibbonCommandHandler);
  if (FAILED(hr)) {
    SPDLOG_ERROR(L"IUIFramework::Initialize failed, hr = {:x}", hr);
    return hr;
  }

  hr = m_pFramework->LoadUI(touchmind::MUI::GetHModule(), L"APPLICATION_RIBBON");
  if (FAILED(hr)) {
    SPDLOG_ERROR(L"IUIFramework::LoadUI failed, hr = {:x}", hr);
    return hr;
  }
  return hr;
}

void touchmind::ribbon::RibbonFramework::Destroy() {
  if (m_pFramework != nullptr) {
    m_pFramework->Destroy();
    m_pFramework->Release();
    m_pFramework = nullptr;
  }
}

UINT touchmind::ribbon::RibbonFramework::GetRibbonHeight() {
  UINT ribbonHeight = 0;
  if (m_pFramework != nullptr) {
    IUIRibbon *pRibbon;
    m_pFramework->GetView(0, IID_PPV_ARGS(&pRibbon));
    pRibbon->GetHeight(&ribbonHeight);
    pRibbon->Release();
  }
  return ribbonHeight;
}

HRESULT touchmind::ribbon::RibbonFramework::CreateUIImageFromBitmapResource(IN LPCTSTR resource,
                                                                            OUT IUIImage **ppImage) {
  HRESULT hr = E_FAIL;

  *ppImage = nullptr;

  HBITMAP hbm = (HBITMAP)LoadImage(HINST_THISCOMPONENT, resource, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
  if (hbm) {
    hr = m_pifbFactory->CreateImage(hbm, UI_OWNERSHIP_TRANSFER, ppImage);
    if (FAILED(hr)) {
      DeleteObject(hbm);
    }
  }
  return hr;
}
