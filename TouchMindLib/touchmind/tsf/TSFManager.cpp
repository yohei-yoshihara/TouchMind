#include "StdAfx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "TSFManager.h"

touchmind::tsf::TSFManager::TSFManager(void)
    : m_hAccel(nullptr)
    , m_hWnd(nullptr)
    , m_pThreadMgr(nullptr)
    , m_pKeyMgr(nullptr)
    , m_pMsgPump(nullptr)
    , m_tfClientID(0)
    , m_isThreadMgrActivated(false)
    , m_pCategoryMgr(nullptr)
    , m_pDisplayAttrMgr(nullptr) {
}

touchmind::tsf::TSFManager::~TSFManager(void) {
  SafeRelease(&m_pDisplayAttrMgr);
  SafeRelease(&m_pCategoryMgr);
  SafeRelease(&m_pMsgPump);
  SafeRelease(&m_pKeyMgr);
  SafeRelease(&m_pThreadMgr);
}

void touchmind::tsf::TSFManager::Initialize() {
  HRESULT hr
      = CoCreateInstance(CLSID_TF_ThreadMgr, nullptr, CLSCTX_INPROC_SERVER, IID_ITfThreadMgr, (void **)&m_pThreadMgr);
  if (FAILED(hr)) {
    LOG(SEVERITY_LEVEL_ERROR) << L"instantiation a thread manager failed";
  }

  if (SUCCEEDED(hr)) {
    ActivateThreadMgr();
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_ERROR) << L"activate the thread manager failed";
    }
  }

  if (SUCCEEDED(hr)) {
    hr = m_pThreadMgr->QueryInterface(IID_ITfKeystrokeMgr, (void **)&m_pKeyMgr);
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_ERROR) << L"initialize a key manager failed";
    }
  }

  if (SUCCEEDED(hr)) {
    hr = m_pThreadMgr->QueryInterface(IID_ITfMessagePump, (void **)&m_pMsgPump);
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_ERROR) << L"initialize message pump failed";
    }
  }

  if (SUCCEEDED(hr)) {
    hr = CoCreateInstance(CLSID_TF_CategoryMgr, nullptr, CLSCTX_INPROC_SERVER, IID_ITfCategoryMgr,
                          (LPVOID *)&m_pCategoryMgr);
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_ERROR) << L"Failed to create an instance of ITfCategoryMgr, hr = " << hr;
    }
  }

  if (SUCCEEDED(hr)) {
    hr = CoCreateInstance(CLSID_TF_DisplayAttributeMgr, nullptr, CLSCTX_INPROC_SERVER, IID_ITfDisplayAttributeMgr,
                          (LPVOID *)&m_pDisplayAttrMgr);
    if (FAILED(hr)) {
      LOG(SEVERITY_LEVEL_ERROR) << L"Failed to create an instance of ITfDisplayAttributeMgr, hr = " << hr;
    }
  }
}

int touchmind::tsf::TSFManager::RunMessageLoop() {
  int nReturn = 0;
  BOOL fResult = TRUE;

  while (fResult) {
    MSG msg;
    BOOL fEaten;

    BOOL focus;
    m_pThreadMgr->IsThreadFocus(&focus);
    HRESULT hr = GetMessagePump()->GetMessage(&msg, nullptr, 0, 0, &fResult);
    if (FAILED(hr)) {
      fResult = FALSE;
    }
    if (SUCCEEDED(hr)) {
      if (WM_KEYDOWN == msg.message) {
        if (GetKeystrokeMgr()->TestKeyDown(msg.wParam, msg.lParam, &fEaten) == S_OK && fEaten
            && GetKeystrokeMgr()->KeyDown(msg.wParam, msg.lParam, &fEaten) == S_OK && fEaten) {
          continue;
        }
      } else if (WM_KEYUP == msg.message) {
        if (GetKeystrokeMgr()->TestKeyUp(msg.wParam, msg.lParam, &fEaten) == S_OK && fEaten
            && GetKeystrokeMgr()->KeyUp(msg.wParam, msg.lParam, &fEaten) == S_OK && fEaten) {
          continue;
        }
      }
    }

    if (fResult) {
      int ret = 0;
      if (m_hAccel != nullptr && m_hWnd != nullptr) {
        ret = TranslateAccelerator(m_hWnd, m_hAccel, &msg);
      }
      if (!ret) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
    }

    if (WM_QUIT == msg.message) {
      nReturn = (int)msg.wParam;
    }
  }
  return nReturn;
}

HRESULT touchmind::tsf::TSFManager::ActivateThreadMgr() {
  HRESULT hr = S_OK;
  if (!m_isThreadMgrActivated) {
    hr = m_pThreadMgr->Activate(&m_tfClientID);
    if (SUCCEEDED(hr)) {
      m_isThreadMgrActivated = true;
    } else {
      LOG(SEVERITY_LEVEL_ERROR) << L"Failed to activate thread manager, " << hr;
    }
  }
  return hr;
}

HRESULT touchmind::tsf::TSFManager::DeactivateThreadMagr() {
  HRESULT hr = S_OK;
  if (m_isThreadMgrActivated) {
    hr = m_pThreadMgr->Deactivate();
    if (SUCCEEDED(hr)) {
      m_isThreadMgrActivated = false;
    } else {
      LOG(SEVERITY_LEVEL_ERROR) << L"Failed to deactivate thread manager, " << hr;
    }
  }
  return hr;
}
