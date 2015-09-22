#include "StdAfx.h"
#include <Strsafe.h>
#include "Resource.h"
#include "TouchMindApp.h"
#include "touchmind/win/CanvasPanel.h"
#include "touchmind/Common.h"
#include "touchmind/Configuration.h"
#include "touchmind/MUI.h"
#include "touchmind/control/StatusBar.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/MapModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/converter/NodeModelToTextConverter.h"
#include "touchmind/ribbon/RibbonFramework.h"
#include "touchmind/ribbon/RibbonCommandHandler.h"
#include "touchmind/ribbon/handler/NodeShapeCommandHandler.h"
#include "touchmind/ribbon/handler/NodeBackgroundColorCommandHandler.h"
#include "touchmind/ribbon/handler/LineWidthCommandHandler.h"
#include "touchmind/ribbon/handler/LineEdgeStyleCommandHandler.h"
#include "touchmind/ribbon/handler/LineStyleCommandHandler.h"
#include "touchmind/ribbon/handler/LineColorCommandHandler.h"
#include "touchmind/shell/Clipboard.h"
#include "touchmind/util/Utils.h"
#include "touchmind/util/OSVersionChecker.h"
#include "touchmind/view/node/NodeViewManager.h"
#include "touchmind/selection/SelectionManager.h"
#include "touchmind/ribbon/dispatch/RibbonRequestDispatcher.h"
#include "touchmind/operation/FileOperation.h"
#include "touchmind/tsf/TSFManager.h"
#include "touchmind/shell/MRUManager.h"
#include "touchmind/control/DWriteEditControlManager.h"
#include "touchmind/model/node/NodeModel.h"

const wchar_t *APPLICATION_ID = _APPLICATION_ID;

const int touchmind::win::TouchMindApp::NUMBER_OF_DIALOG_FILTERS = 2;
COMDLG_FILTERSPEC touchmind::win::TouchMindApp::DIALOG_FILTERS[]
    = {{_OPEN_DIALOG_BOX_LONG, _OPEN_DIALOG_BOX_SHORT}, {L"All types(*.*)", L"*.*"}};

const UINT_PTR touchmind::win::TouchMindApp::IDT_STATUSBAR_TIMER = 1;

touchmind::win::TouchMindApp::TouchMindApp(void)
    : m_hwnd(nullptr)
    , m_ribbonHeight(0)
    , m_pCanvasPanel(std::make_unique<CanvasPanel>())
    , m_pConfiguration(std::make_unique<touchmind::Configuration>())
    , m_pTsfManager(std::make_unique<touchmind::tsf::TSFManager>())
    , m_pMruManager(std::make_unique<touchmind::shell::MRUManager>())
    , m_pStatusBar(std::make_unique<touchmind::control::StatusBar>())
    , m_pRibbonFramework(std::make_unique<touchmind::ribbon::RibbonFramework>())
    , m_ribbonRequestDispatcher(std::make_unique<touchmind::ribbon::dispatch::RibbonRequestDispatcher>())
    , m_fileOperation(std::make_unique<touchmind::operation::FileOperation>()) {
  std::wstring defaultRootElementText = touchmind::MUI::GetString(IDS_ROOT_ELEMENT_TEXT);
  touchmind::model::MapModel::SetDefaultRootNodeText(defaultRootElementText);
}

touchmind::win::TouchMindApp::~TouchMindApp(void) {
}

void touchmind::win::TouchMindApp::DiscardResources() {
}

void touchmind::win::TouchMindApp::RunMessageLoop() {
  m_pTsfManager->RunMessageLoop();
}

void touchmind::win::TouchMindApp::Initialize() {
  HICON hIcon = LoadIcon(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDI_TOUCHMIND));
  if (hIcon == nullptr) {
    DWORD lastError = GetLastError();
    LOG(SEVERITY_LEVEL_ERROR) << util::LastError(util::LastErrorArgs(L"LoadIcon(IDI_TOUCHMIND)", lastError));
    throw std::runtime_error("LoadIcon");
  }

  HACCEL hAccel = LoadAccelerators(HINST_THISCOMPONENT, MAKEINTRESOURCE(IDR_ACCELERATOR_DEFAULT));
  if (hAccel == nullptr) {
    DWORD lastError = GetLastError();
    LOG(SEVERITY_LEVEL_ERROR) << util::LastError(
        util::LastErrorArgs(L"LoadAccelerators(IDR_ACCELERATOR_DEFAULT)", lastError));
    throw std::runtime_error("LoadAccelerators");
  }

  WNDCLASSEX wcex = {sizeof(WNDCLASSEX)};
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = touchmind::win::TouchMindApp::WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = sizeof(LONG_PTR);
  wcex.hInstance = HINST_THISCOMPONENT;
  wcex.hbrBackground = nullptr;
  wcex.lpszMenuName = nullptr;
  wcex.hIcon = hIcon;
  wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wcex.lpszClassName = L"TouchMindApp";

  RegisterClassEx(&wcex);

  HDC hdc = GetDC(nullptr);
  int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
  int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);
  ReleaseDC(nullptr, hdc);

  m_hwnd = CreateWindow(L"TouchMindApp", _WINDOW_TITLE, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT,
                        CW_USEDEFAULT, static_cast<UINT>(ceil(1024.f * dpiX / 96.f)),
                        static_cast<UINT>(ceil(768.f * dpiY / 96.f)), nullptr, nullptr, HINST_THISCOMPONENT, this);
  if (m_hwnd == nullptr) {
    DWORD lastError = GetLastError();
    LOG(SEVERITY_LEVEL_ERROR) << util::LastError(util::LastErrorArgs(L"CreateWindow", lastError));
    throw std::runtime_error("CreateWindow");
  }
  m_pTsfManager->SetHAccel(hAccel);
  m_pTsfManager->SetHWnd(m_hwnd);
  m_pTsfManager->Initialize();
  _InitializeRibbon();
  _InitializeShellLibrary();
  ::ShowWindow(m_hwnd, SW_SHOWNORMAL);
  ::UpdateWindow(m_hwnd);
}

INT_PTR CALLBACK DialogProc(__in HWND hwndDlg, __in UINT uMsg, __in WPARAM wParam, __in LPARAM lParam) {
  UNREFERENCED_PARAMETER(lParam);
  switch (uMsg) {
  case WM_INITDIALOG:
    BringWindowToTop(hwndDlg);
    return TRUE;

  case WM_COMMAND:
    switch (wParam) {
    case IDOK:
      EndDialog(hwndDlg, 0);
      return TRUE;
    }
    break;
  }

  return FALSE;
}

void touchmind::win::TouchMindApp::_InitializeRibbon() {
  m_ribbonRequestDispatcher->SetRibbonFramework(m_pRibbonFramework.get());
  m_ribbonRequestDispatcher->AddInvalidateCallback([this]() { m_pCanvasPanel->Invalidate(); });

  // NodeShapeCommandHandler
  CComPtr<IUICommandHandler> nodeShapeCommandHandler(new touchmind::ribbon::handler::NodeShapeCommandHandler(
      m_pRibbonFramework.get(), m_ribbonRequestDispatcher.get()));
  CHK_RES(nodeShapeCommandHandler, S_OK);
  m_pRibbonFramework->GetRibbonCommandHandler()->AddCommandHandler(cmdNodeShape, nodeShapeCommandHandler);
  m_ribbonRequestDispatcher->AddSelectionChangedListner([](
      touchmind::ribbon::RibbonFramework *pRibbonFramework,
      std::shared_ptr<touchmind::selection::SelectableSupport> oldSelectedItem,
      std::shared_ptr<touchmind::selection::SelectableSupport> newSelectedItem) {
    pRibbonFramework->GetFramework()->InvalidateUICommand(cmdNodeShape, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_Enabled);
    pRibbonFramework->GetFramework()->InvalidateUICommand(cmdNodeShape, UI_INVALIDATIONS_PROPERTY,
                                                          &UI_PKEY_SelectedItem);
    pRibbonFramework->GetFramework()->InvalidateUICommand(cmdNodeShape, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_LargeImage);
  });

  // NodeBackgroundColorCommandHandler2
  CComPtr<IUICommandHandler> nodeBackgroundCommandHandler(
      new touchmind::ribbon::handler::NodeBackgroundColorCommandHandler(
          m_pCanvasPanel->GetContext(), m_pRibbonFramework.get(), m_ribbonRequestDispatcher.get()));
  CHK_RES(nodeBackgroundCommandHandler, S_OK);
  m_pRibbonFramework->GetRibbonCommandHandler()->AddCommandHandler(cmdBackgroundColor, nodeBackgroundCommandHandler);
  m_ribbonRequestDispatcher->AddSelectionChangedListner(
      [](touchmind::ribbon::RibbonFramework *pRibbonFramework,
         std::shared_ptr<touchmind::selection::SelectableSupport> oldSelectedItem,
         std::shared_ptr<touchmind::selection::SelectableSupport> newSelectedItem) {
        pRibbonFramework->GetFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY,
                                                              &UI_PKEY_Enabled);
        pRibbonFramework->GetFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY,
                                                              &UI_PKEY_ColorType);
        pRibbonFramework->GetFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY,
                                                              &UI_PKEY_Color);
        pRibbonFramework->GetFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY,
                                                              &UI_PKEY_LargeImage);
      });

  // LineWidthCommandHandler
  CComPtr<IUICommandHandler> lineWidthCommandHandler(new touchmind::ribbon::handler::LineWidthCommandHandler(
      m_pRibbonFramework.get(), m_ribbonRequestDispatcher.get()));
  CHK_RES(lineWidthCommandHandler, S_OK);
  m_pRibbonFramework->GetRibbonCommandHandler()->AddCommandHandler(cmdLineWidth, lineWidthCommandHandler);
  m_ribbonRequestDispatcher->AddSelectionChangedListner([](
      touchmind::ribbon::RibbonFramework *pRibbonFramework,
      std::shared_ptr<touchmind::selection::SelectableSupport> oldSelectedItem,
      std::shared_ptr<touchmind::selection::SelectableSupport> newSelectedItem) {
    pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineWidth, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_Enabled);
    pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineWidth, UI_INVALIDATIONS_PROPERTY,
                                                          &UI_PKEY_SelectedItem);
    pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineWidth, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_LargeImage);
  });

  // LineStyleCommandHandler
  CComPtr<IUICommandHandler> lineStyleCommandHandler(new touchmind::ribbon::handler::LineStyleCommandHandler(
      m_pRibbonFramework.get(), m_ribbonRequestDispatcher.get()));
  CHK_RES(lineStyleCommandHandler, S_OK);
  m_pRibbonFramework->GetRibbonCommandHandler()->AddCommandHandler(cmdLineStyle, lineStyleCommandHandler);
  m_ribbonRequestDispatcher->AddSelectionChangedListner([](
      touchmind::ribbon::RibbonFramework *pRibbonFramework,
      std::shared_ptr<touchmind::selection::SelectableSupport> oldSelectedItem,
      std::shared_ptr<touchmind::selection::SelectableSupport> newSelectedItem) {
    pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineStyle, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_Enabled);
    pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineStyle, UI_INVALIDATIONS_PROPERTY,
                                                          &UI_PKEY_SelectedItem);
    pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineStyle, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_LargeImage);
  });

  // LineEdgeStyleCommandHandler
  CComPtr<IUICommandHandler> lineEdgeStyleCommandHandler(new touchmind::ribbon::handler::LineEdgeStyleCommandHandler(
      m_pRibbonFramework.get(), m_ribbonRequestDispatcher.get()));
  CHK_RES(lineEdgeStyleCommandHandler, S_OK);
  m_pRibbonFramework->GetRibbonCommandHandler()->AddCommandHandler(cmdLineEdgeStyle, lineEdgeStyleCommandHandler);
  m_ribbonRequestDispatcher->AddSelectionChangedListner(
      [](touchmind::ribbon::RibbonFramework *pRibbonFramework,
         std::shared_ptr<touchmind::selection::SelectableSupport> oldSelectedItem,
         std::shared_ptr<touchmind::selection::SelectableSupport> newSelectedItem) {
        pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineEdgeStyle, UI_INVALIDATIONS_PROPERTY,
                                                              &UI_PKEY_Enabled);
        pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineEdgeStyle, UI_INVALIDATIONS_PROPERTY,
                                                              &UI_PKEY_SelectedItem);
        pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineEdgeStyle, UI_INVALIDATIONS_PROPERTY,
                                                              &UI_PKEY_LargeImage);
      });

  // LineColorCommandHandler
  CComPtr<IUICommandHandler> lineColorCommandHandler(new touchmind::ribbon::handler::LineColorCommandHandler(
      m_pCanvasPanel->GetContext(), m_pRibbonFramework.get(), m_ribbonRequestDispatcher.get()));
  CHK_RES(lineColorCommandHandler, S_OK);
  m_pRibbonFramework->GetRibbonCommandHandler()->AddCommandHandler(cmdLineColor, lineColorCommandHandler);
  m_ribbonRequestDispatcher->AddSelectionChangedListner([](
      touchmind::ribbon::RibbonFramework *pRibbonFramework,
      std::shared_ptr<touchmind::selection::SelectableSupport> oldSelectedItem,
      std::shared_ptr<touchmind::selection::SelectableSupport> newSelectedItem) {
    pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_Enabled);
    pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_ColorType);
    pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_Color);
    pRibbonFramework->GetFramework()->InvalidateUICommand(cmdLineColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_LargeImage);
  });

  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdMRUList, [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *key, const PROPVARIANT *pPropvarValue,
                         IUISimplePropertySet *) -> HRESULT {
        if (key != nullptr && UI_PKEY_SelectedItem == *key) {
          std::wstring displayName;
          std::wstring fullPath;
          GetSelectedItem(key, pPropvarValue, displayName, fullPath);
          OnOpenFileByExtension(fullPath);
        }
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnUpdatePropertyListener(
      cmdMRUList, [this](UINT, REFPROPERTYKEY key, const PROPVARIANT *, PROPVARIANT *pPropvarNewValue) -> HRESULT {
        HRESULT hr = E_NOTIMPL;
        if (UI_PKEY_Label == key) {
          hr = UIInitPropertyFromString(UI_PKEY_Label, MUI::GetString(IDS_RECENT_FILES), pPropvarNewValue);
        } else if (UI_PKEY_RecentItems == key) {
          hr = PopulateRibbonRecentItems(pPropvarNewValue);
        }
        return hr;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdNew,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnNew();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdOpenFreeMind,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnOpenFreeMind();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdOpenFile,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnOpenFile();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdSave,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnSave();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdSaveAs,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnSaveAs();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdCreateChildNode,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnCreateChildNode();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdEditNode,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnEditNode();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdDeleteNode,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnEditDelete();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdExit,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnExit();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdCopy,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnEditCopy();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdPaste,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnEditPaste();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdCut,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnEditCut();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdUndo,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnEditUndo();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdDeleteLink,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnDeleteLink();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdExpandNodes,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnExpandNodes();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdCollapseNodes,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnCollapseNodes();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdVersionInfo,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        DialogBox(MUI::GetHModule(), MAKEINTRESOURCE(IDD_VERSION_DIALOG), m_pRibbonFramework->GetHWnd(), DialogProc);
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdPrint,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnPrint();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdPrintSetup,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnPrintSetup();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnExecuteListener(
      cmdCreateLink,
      [this](UINT, UI_EXECUTIONVERB, const PROPERTYKEY *, const PROPVARIANT *, IUISimplePropertySet *) -> HRESULT {
        OnCreateLink();
        return S_OK;
      });
  m_pRibbonFramework->GetRibbonCommandHandler()->AddOnUpdatePropertyListener(
      cmdCreateLink, [this](UINT nCmdID, REFPROPERTYKEY key, const PROPVARIANT *pPropvarCurrentValue,
                            PROPVARIANT *pPropvarNewValue) -> HRESULT {
        UNREFERENCED_PARAMETER(nCmdID);
        UNREFERENCED_PARAMETER(key);
        UNREFERENCED_PARAMETER(pPropvarCurrentValue);
        UNREFERENCED_PARAMETER(pPropvarNewValue);
        HRESULT hr = E_FAIL;
        if (key == UI_PKEY_BooleanValue) {
          hr = UIInitPropertyFromBoolean(key, OnCreateLink_UpdateProperty() ? TRUE : FALSE, pPropvarNewValue);
        }
        return hr;
      });
  CHK_HR(m_pRibbonFramework->Initialize(m_hwnd, this));
}

void touchmind::win::TouchMindApp::_InitializeShellLibrary() {
  bool win7 = touchmind::util::OSVersionChecker::IsWin7();
  if (!win7) {
    LOG(SEVERITY_LEVEL_INFO) << L"OS is not Win7. Skip initializing shell library";
    return;
  }

  HRESULT hr = S_OK;
  // The following code only run with Windows7
  if (touchmind::util::OSVersionChecker::IsWin7()) {
    hr = SetCurrentProcessExplicitAppUserModelID(APPLICATION_ID);
    if (FAILED(hr)) {
      DWORD lastError = GetLastError();
      LOG(SEVERITY_LEVEL_ERROR) << util::LastError(
          util::LastErrorArgs(L"SetCurrentProcessExplicitAppUserModelID", lastError));
      return;
    }
  }

  ICustomDestinationList *pJumpList = nullptr;
  if (SUCCEEDED(hr)) {
    hr = CoCreateInstance(CLSID_DestinationList, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pJumpList));
  }
  if (SUCCEEDED(hr)) {
    hr = pJumpList->SetAppID(APPLICATION_ID);
  }
  UINT uMaxSlots;
  IObjectArray *pRemoved = nullptr;
  if (SUCCEEDED(hr)) {
    hr = pJumpList->BeginList(&uMaxSlots, IID_PPV_ARGS(&pRemoved));
  }
  if (SUCCEEDED(hr)) {
    hr = pJumpList->AppendKnownCategory(KDC_RECENT);
  }
  if (SUCCEEDED(hr)) {
    hr = pJumpList->CommitList();
  }
  if (FAILED(hr)) {
    DWORD lastError = GetLastError();
    LOG(SEVERITY_LEVEL_ERROR) << util::LastError(util::LastErrorArgs(L"InitializeShell", lastError));
  }

  if (pRemoved != nullptr) {
    pRemoved->Release();
  }
  if (pJumpList != nullptr) {
    pJumpList->Release();
  }
}

void touchmind::win::TouchMindApp::OnCreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  UNREFERENCED_PARAMETER(message);
  UNREFERENCED_PARAMETER(lParam);
  m_hwnd = hwnd;
  m_pStatusBar->Initialize(m_hwnd, 1, HINST_THISCOMPONENT, 1);
  m_pStatusBar->SetColor(RGB(220, 231, 245));
  m_pCanvasPanel->SetTouchMindApp(this);
  m_pCanvasPanel->Initialize(static_cast<UINT>(wParam));
}

LRESULT CALLBACK touchmind::win::TouchMindApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
#ifdef _DEBUG
  LOG(SEVERITY_LEVEL_DEBUG_L3) << touchmind::util::WMessage(message);
#endif
  LRESULT result = 0;

  if (message == WM_CREATE) {
    LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
    touchmind::win::TouchMindApp *pTouchMindApp = reinterpret_cast<touchmind::win::TouchMindApp *>(pcs->lpCreateParams);

    ::SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pTouchMindApp));

    result = 1;
  } else {
    touchmind::win::TouchMindApp *pTouchMindApp = reinterpret_cast<touchmind::win::TouchMindApp *>(
        static_cast<LONG_PTR>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

    bool wasHandled = false;

    if (pTouchMindApp) {
      switch (message) {
      case WM_COMMAND: {
        if (HIWORD(wParam) == 1) {
          // Accelerator
          switch (LOWORD(wParam)) {
          case ID_ACCELERATOR_COPY:
            pTouchMindApp->OnEditCopy();
            break;
          case ID_ACCELERATOR_CUT:
            pTouchMindApp->OnEditCut();
            break;
          case ID_ACCELERATOR_PASTE:
            pTouchMindApp->OnEditPaste();
            break;
          case ID_ACCELERATOR_NEW:
            pTouchMindApp->OnNew();
            break;
          case ID_ACCELERATOR_OPEN:
            pTouchMindApp->OnOpenFile();
            break;
          case ID_ACCELERATOR_SAVE:
            pTouchMindApp->OnSave();
            break;
          case ID_ACCELERATOR_UNDO:
            pTouchMindApp->OnEditUndo();
            break;
          case ID_ACCELERATOR_DUMP:
            pTouchMindApp->OnDebugDump();
            break;
          }
        }
      }
        result = 0;
        wasHandled = true;
        break;
      case WM_TIMER: {
        switch (wParam) {
        case IDT_STATUSBAR_TIMER:
          pTouchMindApp->ClearStatusBarText();
          break;
        }
      }
        result = 0;
        wasHandled = true;
        break;
      case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam)) {
          // do nothing
          break;
        }
      // do SetFocus
      case WM_SETFOCUS:
        if (pTouchMindApp->m_pCanvasPanel != nullptr && pTouchMindApp->m_pCanvasPanel->IsInitialized()) {
          ::SetFocus(pTouchMindApp->m_pCanvasPanel->GetHwnd());
        }
        break;
      case WM_RIBBON_RESIZED:
        if (pTouchMindApp->m_pCanvasPanel != nullptr && !pTouchMindApp->m_pCanvasPanel->IsInitialized()) {
          pTouchMindApp->OnCreate(hwnd, message, wParam, lParam);
        } else if (pTouchMindApp->m_pCanvasPanel->IsInitialized()) {
          pTouchMindApp->m_pCanvasPanel->ResizeByRibbonResized(static_cast<UINT>(wParam));
        }
        result = 0;
        wasHandled = true;
        break;

      case WM_SIZE: {
        // we have to check whether child components are initialized before invoking.
        if (pTouchMindApp->m_pCanvasPanel != nullptr && pTouchMindApp->m_pCanvasPanel->IsInitialized()) {
          pTouchMindApp->m_pCanvasPanel->ResizeByParentResized();
        }
        if (pTouchMindApp->m_pStatusBar != nullptr && pTouchMindApp->m_pStatusBar->IsInitialized()) {
          pTouchMindApp->m_pStatusBar->OnSize(wParam, lParam);
        }
      }
        result = 0;
        wasHandled = true;
        break;

      case WM_DISPLAYCHANGE:
      case WM_PAINT:
        result = 0;
        wasHandled = false;
        break;

      case WM_CLOSE:
        if (!pTouchMindApp->OnClose()) {
          wasHandled = true;
          result = 1;
        }
        break;
      case WM_DESTROY: {
        pTouchMindApp->m_pCanvasPanel->OnDestroy();
        pTouchMindApp->GetRibbonFramework()->Destroy();
        LOG(SEVERITY_LEVEL_INFO) << L"exit application";
        PostQuitMessage(0);
      }
        result = 1;
        wasHandled = true;
        break;
      }
    }

    if (!wasHandled) {
      result = DefWindowProc(hwnd, message, wParam, lParam);
    }
  }
  return result;
}

void touchmind::win::TouchMindApp::_GetDisplayLocation(HWND hWnd, POINT &pt) {
  if (pt.x == -1 && pt.y == -1) {
    HRESULT hr = E_FAIL;

    IUIRibbon *pRibbon;
    hr = m_pRibbonFramework->GetFramework()->GetView(0, IID_PPV_ARGS(&pRibbon));
    if (SUCCEEDED(hr)) {
      UINT32 uRibbonHeight = 0;
      hr = pRibbon->GetHeight(&uRibbonHeight);
      if (SUCCEEDED(hr)) {
        pt.x = 0;
        pt.y = uRibbonHeight;
        ClientToScreen(hWnd, &pt);
      }
      pRibbon->Release();
    }
    if (FAILED(hr)) {
      pt.x = 0;
      pt.y = 0;
    }
  }
}

void touchmind::win::TouchMindApp::OnContextMenu(HWND hWnd, POINT ptLocation) {
  _GetDisplayLocation(hWnd, ptLocation);
  IUIContextualUI *pContextualUI = nullptr;
  int viewId = m_pRibbonFramework->GetRibbonCommandHandler()->GetCurrentContext();
  CHK_HR(m_pRibbonFramework->GetFramework()->GetView(viewId, IID_PPV_ARGS(&pContextualUI)));
  CHK_HR(pContextualUI->ShowAtLocation(ptLocation.x, ptLocation.y));
  pContextualUI->Release();
}

void touchmind::win::TouchMindApp::OnCreateChildNode() {
  auto selected = std::dynamic_pointer_cast<touchmind::model::node::NodeModel>(
      m_pCanvasPanel->GetSelectionManager()->GetSelected());
  if (selected == nullptr) {
    return;
  }

  m_pCanvasPanel->GetNodeViewManager()->SyncFontAttributesFromTextLayout();
  GetMapModel()->BeginTransaction();
  std::shared_ptr<touchmind::model::node::NodeModel> newChildNode
      = touchmind::model::node::NodeModel::Create(m_pCanvasPanel->GetSelectionManager());
  SYSTEMTIME currentTime;
  GetLocalTime(&currentTime);
  newChildNode->SetCreatedTime(currentTime);
  newChildNode->SetModifiedTime(currentTime);
  selected->AddChild(newChildNode);
  GetMapModel()->EndTransaction();

  m_pCanvasPanel->ArrangeNodes();
  m_pCanvasPanel->EditNode(newChildNode);
}

void touchmind::win::TouchMindApp::OnEditDelete() {
  m_pCanvasPanel->OnEditDelete();
}

void touchmind::win::TouchMindApp::OnExit() {
  ::PostMessage(m_hwnd, WM_CLOSE, 0, 0);
}

void touchmind::win::TouchMindApp::OnEditCopy() {
  m_pCanvasPanel->OnEditCopy();
}

void touchmind::win::TouchMindApp::OnEditPaste() {
  m_pCanvasPanel->OnEditPaste();
}

void touchmind::win::TouchMindApp::OnEditCut() {
  m_pCanvasPanel->OnEditCut();
}

void touchmind::win::TouchMindApp::OnEditNode() {
  auto selected = std::dynamic_pointer_cast<touchmind::model::node::NodeModel>(
      m_pCanvasPanel->GetSelectionManager()->GetSelected());
  if (selected != nullptr) {
    m_pCanvasPanel->EditNode(selected);
  }
}

void touchmind::win::TouchMindApp::OnEditUndo() {
  m_pCanvasPanel->OnEditUndo();
}

IUIFramework *touchmind::win::TouchMindApp::GetUIFramework() {
  return m_pRibbonFramework->GetFramework();
}

void touchmind::win::TouchMindApp::_OnOpenFile(const std::wstring &fileName) {
  if (PathFileExistsW(fileName.c_str())) {
    GetMapModel()->Open(fileName);
  } else {
    wchar_t buf[1024];
    swprintf_s(buf, 1024, touchmind::MUI::GetString(IDS_OPEN_ERROR_MESSAGE), fileName.c_str());
    MessageBox(m_hwnd, buf, touchmind::MUI::GetString(IDS_OPEN_ERROR_CAPTION),
               MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST);
  }
  ClearStatusBarText();
}

void touchmind::win::TouchMindApp::_OnOpenFreeMind(const std::wstring &fileName) {
  if (PathFileExistsW(fileName.c_str())) {
    GetMapModel()->OpenFromFreeMind(fileName);
  } else {
    wchar_t buf[1024];
    swprintf_s(buf, 1024, touchmind::MUI::GetString(IDS_OPEN_ERROR_MESSAGE), fileName.c_str());
    MessageBox(m_hwnd, buf, touchmind::MUI::GetString(IDS_OPEN_ERROR_CAPTION),
               MB_OK | MB_ICONERROR | MB_SETFOREGROUND | MB_TOPMOST);
  }
  ClearStatusBarText();
}

void touchmind::win::TouchMindApp::OnOpenFileByExtension(const std::wstring &fileName) {
  CheckRequiredSave();
  std::wregex rx(L".*\\.mm");
  if (std::regex_match(fileName.begin(), fileName.end(), rx)) {
    _OnOpenFreeMind(fileName);
  } else {
    _OnOpenFile(fileName);
  }
}

void touchmind::win::TouchMindApp::_OnSave() {
  GetMapModel()->Save();
  SetStatusBarText(L"Saved");
}

void touchmind::win::TouchMindApp::_OnSaveAs(const std::wstring &fileName) {
  GetMapModel()->SaveAs(fileName);
  SetStatusBarText(L"Saved");
}

void touchmind::win::TouchMindApp::OnNew() {
  CheckRequiredSave();
  GetMapModel()->New();
  ClearStatusBarText();
}

HRESULT touchmind::win::TouchMindApp::PopulateRibbonRecentItems(PROPVARIANT *pvarValue) {
  return m_pMruManager->PopulateRibbonRecentItems(pvarValue);
}

HRESULT touchmind::win::TouchMindApp::GetSelectedItem(IN const PROPERTYKEY *key, IN const PROPVARIANT *ppropvarValue,
                                                      OUT std::wstring &displayName, OUT std::wstring &fullPath) {
  return m_pMruManager->GetSelectedItem(key, ppropvarValue, displayName, fullPath);
}

void touchmind::win::TouchMindApp::OnExpandNodes() {
  auto selectedNode = std::dynamic_pointer_cast<touchmind::model::node::NodeModel>(
      m_pCanvasPanel->GetSelectionManager()->GetSelected());
  if (selectedNode != nullptr) {
    selectedNode->SetCollapsed(false);
    m_pCanvasPanel->Invalidate();
  }
}

void touchmind::win::TouchMindApp::OnCollapseNodes() {
  auto selectedNode = std::dynamic_pointer_cast<touchmind::model::node::NodeModel>(
      m_pCanvasPanel->GetSelectionManager()->GetSelected());
  if (selectedNode != nullptr) {
    selectedNode->SetCollapsed(true);
    m_pCanvasPanel->Invalidate();
  }
}

bool touchmind::win::TouchMindApp::OnClose() {
  bool postQuit = true;

  std::shared_ptr<touchmind::model::node::NodeModel> root = GetMapModel()->GetRootNodeModel();
  if (root->IsDescendantSaveRequired(m_fileOperation->GetSaveCounter())) {
    int ret = MessageBox(m_hwnd, touchmind::MUI::GetString(IDS_QUERY_SAVE_MESSAGE),
                         touchmind::MUI::GetString(IDS_QUERY_SAVE_CAPTION),
                         MB_YESNOCANCEL | MB_ICONQUESTION | MB_SETFOREGROUND | MB_TOPMOST);
    if (ret == IDYES) {
      if (GetMapModel()->HasFileName()) {
        OnSave();
      } else {
        OnSaveAs();
      }
      postQuit = true;
    } else if (ret == IDCANCEL) {
      postQuit = false;
    } else if (ret == IDNO) {
      postQuit = true;
    }
  }
  return postQuit;
}

void touchmind::win::TouchMindApp::CheckRequiredSave() {
  std::shared_ptr<touchmind::model::node::NodeModel> root = GetMapModel()->GetRootNodeModel();
  if (root->IsDescendantSaveRequired(m_fileOperation->GetSaveCounter())) {
    int ret = MessageBox(m_hwnd, touchmind::MUI::GetString(IDS_QUERY_SAVE_MESSAGE),
                         touchmind::MUI::GetString(IDS_QUERY_SAVE_CAPTION),
                         MB_YESNO | MB_ICONQUESTION | MB_SETFOREGROUND | MB_TOPMOST);
    if (ret != IDNO) {
      if (GetMapModel()->HasFileName()) {
        OnSave();
      } else {
        OnSaveAs();
      }
    }
  }
}

void touchmind::win::TouchMindApp::OnOpenFreeMind(void) {
  static COMDLG_FILTERSPEC dialogFiltersMM[] = {
      {L"Free Mind(.mm)", L"*.mm"}, {L"All types(*.*)", L"*.*"},
  };

  CheckRequiredSave();

  IFileOpenDialog *pFileOpenDialog = nullptr;

  HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileOpenDialog));

  if (SUCCEEDED(hr)) {
    hr = pFileOpenDialog->SetFileTypes(2, dialogFiltersMM);
  }
  IShellItem *pResult = nullptr;
  if (SUCCEEDED(hr)) {
    hr = pFileOpenDialog->Show(m_hwnd);
  }
  if (SUCCEEDED(hr)) {
    hr = pFileOpenDialog->GetResult(&pResult);
  }
  WCHAR *pPath = nullptr;
  if (SUCCEEDED(hr)) {
    hr = pResult->GetDisplayName(SIGDN_FILESYSPATH, &pPath);
  }
  if (SUCCEEDED(hr)) {
    std::wstring fileName(pPath);
    _OnOpenFreeMind(fileName);
  }
  if (pPath != nullptr) {
    CoTaskMemFree(pPath);
  }
  SafeRelease(&pResult);
  SafeRelease(&pFileOpenDialog);
}

void touchmind::win::TouchMindApp::OnOpenFile(void) {
  CheckRequiredSave();

  IFileOpenDialog *pFileOpenDialog = nullptr;
  HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileOpenDialog));
  if (SUCCEEDED(hr)) {
    hr = pFileOpenDialog->SetFileTypes(NUMBER_OF_DIALOG_FILTERS, DIALOG_FILTERS);
  }
  if (SUCCEEDED(hr)) {
    hr = pFileOpenDialog->Show(m_hwnd);
  }
  IShellItem *pResult = nullptr;
  if (SUCCEEDED(hr)) {
    hr = pFileOpenDialog->GetResult(&pResult);
  }
  WCHAR *pPath = nullptr;
  if (SUCCEEDED(hr)) {
    hr = pResult->GetDisplayName(SIGDN_FILESYSPATH, &pPath);
  }
  if (SUCCEEDED(hr)) {
    std::wstring fileName(pPath);
    _OnOpenFile(fileName);
  }
  if (pPath != nullptr) {
    CoTaskMemFree(pPath);
  }
  SafeRelease(&pResult);
  SafeRelease(&pFileOpenDialog);
}

void touchmind::win::TouchMindApp::OnSave() {
  if (GetMapModel()->HasFileName()) {
    _OnSave();
  } else {
    OnSaveAs();
  }
}

void touchmind::win::TouchMindApp::OnSaveAs() {
  IFileSaveDialog *pFileSaveDialog = nullptr;
  HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileSaveDialog));
  if (SUCCEEDED(hr)) {
    hr = pFileSaveDialog->SetFileTypes(NUMBER_OF_DIALOG_FILTERS, DIALOG_FILTERS);
  }
  if (SUCCEEDED(hr)) {
    hr = pFileSaveDialog->Show(m_hwnd);
  }
  IShellItem *pResult = nullptr;
  if (SUCCEEDED(hr)) {
    hr = pFileSaveDialog->GetResult(&pResult);
  }
  WCHAR *pPath = nullptr;
  if (SUCCEEDED(hr)) {
    hr = pResult->GetDisplayName(SIGDN_FILESYSPATH, &pPath);
  }
  if (SUCCEEDED(hr)) {
    std::wstring fileName(pPath);
    std::wregex rx(L".*\\" _EXTENSION);
    bool ret = std::regex_match(fileName.begin(), fileName.end(), rx);
    if (!ret) {
      fileName += _EXTENSION;
    }
    _OnSaveAs(fileName);
  }
  if (pPath != nullptr) {
    CoTaskMemFree(pPath);
  }
  SafeRelease(&pResult);
  SafeRelease(&pFileSaveDialog);
}

UINT touchmind::win::TouchMindApp::GetStatusBarHeight() const {
  RECT rect = m_pStatusBar->GetRect();
  return rect.bottom - rect.top;
}

void touchmind::win::TouchMindApp::SetStatusBarText(const wchar_t *text) {
  m_pStatusBar->SetText(0, text);
  SetTimer(m_hwnd, IDT_STATUSBAR_TIMER, 3 * 1000, nullptr);
}

void touchmind::win::TouchMindApp::ClearStatusBarText() {
  m_pStatusBar->ClearText(0);
  KillTimer(m_hwnd, IDT_STATUSBAR_TIMER);
}

void touchmind::win::TouchMindApp::OnDebugDump() {
  LOG(SEVERITY_LEVEL_INFO) << L"start";
  std::wofstream of("c:\\temp\\dump.txt");
  GetMapModel()->GetRootNodeModel()->DumpAll(of);

  m_pCanvasPanel->OnDebugDump(of);

  of.close();
  SetStatusBarText(L"Dumped");
  LOG(SEVERITY_LEVEL_INFO) << L"finished";
}

void touchmind::win::TouchMindApp::OnPrint() {
  m_pCanvasPanel->OnPrint();
}

void touchmind::win::TouchMindApp::OnPrintSetup() {
  m_pCanvasPanel->OnPrintSetup();
}

void touchmind::win::TouchMindApp::OnCreateLink() {
  m_pCanvasPanel->OnCreateLink();
}

bool touchmind::win::TouchMindApp::OnCreateLink_UpdateProperty() {
  LOG_ENTER;
  bool ret = m_pCanvasPanel->GetState() == CANVAS_STATE_CREATING_LINK;
  LOG_LEAVE_ARG(L"ret = " << ret);
  return ret;
}

touchmind::model::MapModel *touchmind::win::TouchMindApp::GetMapModel() const {
  return m_pCanvasPanel->GetMapModel();
}

void touchmind::win::TouchMindApp::OnDeleteLink() {
  m_pCanvasPanel->OnDeleteLink();
}
