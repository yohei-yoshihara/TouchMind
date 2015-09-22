#include "StdAfx.h"
#include "touchmind/Context.h"
#include "touchmind/Configuration.h"
#include "touchmind/animation/AnimationManagerEventHandler.h"
#include "touchmind/animation/IAnimationStatusChangedListener.h"
#include "touchmind/control/DWriteTextSizeProvider.h"
#include "touchmind/control/DWriteEditControlManager.h"
#include "touchmind/layout/LayoutManager.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/model/MapModel.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/print/XPSPrintSetupDialog.h"
#include "touchmind/print/XPSPrint.h"
#include "touchmind/ribbon/RibbonFramework.h"
#include "touchmind/shell/Clipboard.h"
#include "touchmind/touch/ManipulationHelper.h"
#include "touchmind/util/ColorUtil.h"
#include "touchmind/util/ScrollBarHelper.h"
#include "touchmind/util/Utils.h"
#include "touchmind/util/OSVersionChecker.h"
#include "touchmind/util/CoordinateConversion.h"
#include "touchmind/view/node/NodeViewManager.h"
#include "touchmind/view/link/LinkViewManager.h"
#include "touchmind/visitor/NodeDrawVisitor.h"
#include "touchmind/win/TouchMindApp.h"
#include "touchmind/win/CanvasPanel.h"
#include "touchmind/operation/CreateLinkOperation.h"
#include "touchmind/operation/NodeMovingOperation.h"
#include "touchmind/operation/NodeResizingOperation.h"
#include "touchmind/selection/SelectionManager.h"
#include "touchmind/ribbon/dispatch/RibbonRequestDispatcher.h"
#include "touchmind/converter/NodeModelXMLEncoder.h"
#include "touchmind/converter/NodeModelXMLDecoder.h"

LRESULT CALLBACK touchmind::win::CanvasPanel::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef _DEBUG
    LOG(SEVERITY_LEVEL_DEBUG_L3) << touchmind::util::WMessage(message);
#endif
	LOG(SEVERITY_LEVEL_DEBUG) << touchmind::util::WMessage(message);

    LRESULT result = 0;

    if (message == WM_CREATE) {
        LPCREATESTRUCT pcs = (LPCREATESTRUCT) lParam;
        touchmind::win::CanvasPanel *pCanvasPanel = (touchmind::win::CanvasPanel *) pcs->lpCreateParams;

        ::SetWindowLongPtrW(
            hWnd,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(pCanvasPanel)
        );

        pCanvasPanel->OnCreate();
        result = 1;
    } else {
        touchmind::touch::ManipulationResult manipulationResult;
        auto *pCanvasPanel = reinterpret_cast<touchmind::win::CanvasPanel*>(static_cast<LONG_PTR>(
                                 ::GetWindowLongPtrW(
                                     hWnd,
                                     GWLP_USERDATA
                                 )));
        if (pCanvasPanel) {
            bool eaten = false;
            if (pCanvasPanel->m_pEditControlManager != nullptr) {
                pCanvasPanel->m_pEditControlManager->WndProc(hWnd, message, wParam, lParam, &eaten);
            }
            if (eaten) {
                manipulationResult.WasHandled = true;
                manipulationResult.IsNeedInvalidateRect = true;
            } else {
                switch (message) {
                case WM_SETCURSOR:
                    manipulationResult = pCanvasPanel->m_pManipulationHelper->OnSetCursor(hWnd, message, wParam, lParam);
                    break;
                case WM_ACTIVATE:
                    if (WA_INACTIVE == LOWORD(wParam)) {
                        // do nothing
                        break;
                    }
                    // do SetFocus
                case WM_SETFOCUS:
                    ::SetFocus(pCanvasPanel->m_hwnd);
                    manipulationResult.WasHandled = true;
                    break;
                case WM_SIZE: {
                    UINT width = LOWORD(lParam);
                    UINT height = HIWORD(lParam);
                    pCanvasPanel->OnResize(width, height);
                }
                manipulationResult.WasHandled = true;
                break;

                case WM_DISPLAYCHANGE:
                case WM_PAINT:
                    PAINTSTRUCT ps;
                    BeginPaint(hWnd, &ps);
                    pCanvasPanel->OnRender();
                    EndPaint(hWnd, &ps);
                    manipulationResult.WasHandled = true;
                    break;

                case WM_HSCROLL:
                    pCanvasPanel->OnHScroll(hWnd, wParam);
                    manipulationResult.WasHandled = true;
                    break;

                case WM_VSCROLL:
                    pCanvasPanel->OnVScroll(hWnd, wParam);
                    manipulationResult.WasHandled = true;
                    break;

                case WM_KEYUP: {
                }
                break;

                case WM_GESTURE:
                    manipulationResult = pCanvasPanel->m_pManipulationHelper->OnGesture(hWnd, message, wParam, lParam);
                    break;

                case WM_GESTURENOTIFY:
                    pCanvasPanel->SetGestureConfigs(hWnd);
                    break;

                case WM_MOUSEMOVE:
                    manipulationResult = pCanvasPanel->m_pManipulationHelper->OnMouseMove(hWnd, message, wParam, lParam);
                    break;

                case WM_RBUTTONDOWN:
                    manipulationResult = pCanvasPanel->m_pManipulationHelper->OnRightMouseDown(hWnd, message, wParam, lParam);
                    break;

                case WM_LBUTTONDOWN:
                    manipulationResult = pCanvasPanel->m_pManipulationHelper->OnLeftMouseDown(hWnd, message, wParam, lParam);
                    break;

                case WM_LBUTTONUP:
                    manipulationResult = pCanvasPanel->m_pManipulationHelper->OnLeftMouseUp(hWnd, message, wParam, lParam);
                    break;

                case WM_LBUTTONDBLCLK:
                    manipulationResult = pCanvasPanel->m_pManipulationHelper->OnLeftMouseDoubleClick(hWnd, message, wParam, lParam);
                    break;

                case WM_TIMER:
                    manipulationResult = pCanvasPanel->m_pManipulationHelper->OnTimer(hWnd, message, wParam, lParam);
                    break;

                case WM_CONTEXTMENU: {
                    POINT point;
                    point.x = GET_X_LPARAM(lParam);
                    point.y = GET_Y_LPARAM(lParam);
                    pCanvasPanel->GetTouchMindApp()->OnContextMenu(hWnd, point);
                }
                manipulationResult.WasHandled = true;
                break;

                case WM_DESTROY:
                    PostQuitMessage(0);
                    result = 1;
                    manipulationResult.WasHandled = true;
                    break;
                }
            }
        }

        if (!manipulationResult.WasHandled) {
            result = DefWindowProc(hWnd, message, wParam, lParam);
        }
        if (manipulationResult.IsNeedInvalidateRect) {
            pCanvasPanel->Invalidate();
        }
    }
    return result;
}

touchmind::win::CanvasPanel::CanvasPanel(void) :
    m_canvasState(CANVAS_STATE_NORMAL),
    m_isInitialized(false),
    m_hwnd(nullptr),
    m_ribbonHeight(0),
    m_pTouchMindApp(nullptr),
    m_pMapModel(std::make_unique<touchmind::model::MapModel>()),
    m_pContext(std::make_unique<touchmind::Context>()),
    m_pNodeViewManager(std::make_unique<touchmind::view::node::NodeViewManager>()),
    m_pLinkViewManager(std::make_unique<touchmind::view::link::LinkViewManager>()),
    m_pLayoutManager(std::make_unique<touchmind::layout::LayoutManager>()),
    m_pDWriteTextSizeProvider(std::make_unique<touchmind::control::DWriteTextSizeProvider>()),
    m_pScrollBarHelper(std::make_unique<touchmind::util::ScrollBarHelper>()),
    m_pManipulationHelper(std::make_unique<touchmind::touch::ManipulationHelper>()),
    m_pEditControlManager(std::make_unique<touchmind::control::DWriteEditControlManager>()),
    m_pClipboard(std::make_unique<touchmind::shell::Clipboard>()),
    m_pPrintSetupDialog(std::make_unique<touchmind::print::XPSPrintSetupDialog>()),
    m_pXPSPrint(std::make_unique<touchmind::print::XPSPrint>()),
    m_createLinkOperation(std::make_unique<touchmind::operation::CreateLinkOperation>()),
    m_nodeMovingOperation(std::make_unique<touchmind::operation::NodeMovingOperation>()),
    m_nodeResizingOperation(std::make_unique<touchmind::operation::NodeResizingOperation>()),
    m_selectionManager(std::make_unique<touchmind::selection::SelectionManager>()),
    m_nodeModelXMLEncoder(std::make_unique<touchmind::converter::NodeModelXMLEncoder>()),
    m_nodeModelXMLDecoder(std::make_unique<touchmind::converter::NodeModelXMLDecoder>()),
    m_pEdittingNode()
{
    m_pManipulationHelper->SetCanvasPanel(this);
    m_pContext->SetRenderEventListener(this);
    m_pContext->SetAnimationStatusChangedListener(this);
    m_pNodeViewManager->SetContext(GetContext());
    m_pEditControlManager->AddEditControlChangeListener(this);
    m_pEditControlManager->AddSelectionChangeListener(this);
    m_createLinkOperation->SetCanvasPanel(this);
    m_nodeModelXMLDecoder->SetSelectionManager(m_selectionManager.get());
    m_nodeModelXMLDecoder->SetMapModel(m_pMapModel.get());
    m_pMapModel->SetNodeModelXMLEncoder(m_nodeModelXMLEncoder.get());
    m_pMapModel->SetNodeModelXMLDecoder(m_nodeModelXMLDecoder.get());
    m_pClipboard->SetSelectionManager(m_selectionManager.get());
    m_pClipboard->SetNodeModelXMLEncoder(m_nodeModelXMLEncoder.get());
    m_pClipboard->SetNodeModelXMLDecoder(m_nodeModelXMLDecoder.get());
}

touchmind::win::CanvasPanel::~CanvasPanel(void)
{
}

void touchmind::win::CanvasPanel::Initialize(UINT ribbonHeight)
{
    m_ribbonHeight = ribbonHeight;

    CHK_HR(m_pContext->CreateDeviceIndependentResources());

    WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
    wcex.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc   = touchmind::win::CanvasPanel::WndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = sizeof(LONG_PTR);
    wcex.hInstance     = HINST_THISCOMPONENT;
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName  = nullptr;
    wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wcex.lpszClassName = L"CanvasPanel";
    RegisterClassEx(&wcex);

    FLOAT dpiX, dpiY;
    m_pContext->GetD2DFactory()->GetDesktopDpi(&dpiX, &dpiY);

    HWND parentHwnd = GetTouchMindApp()->GetHwnd();
    RECT rc;
    ::GetClientRect(parentHwnd, &rc);
    rc.top = ribbonHeight;
    rc.bottom -= ribbonHeight;
    rc.bottom -= GetTouchMindApp()->GetStatusBarHeight();

    m_hwnd = CreateWindow(
                 L"CanvasPanel",
                 L"CanvasPanel",
                 WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_CLIPCHILDREN,
                 rc.left, rc.top, rc.right, rc.bottom,
                 parentHwnd,
                 nullptr,
                 HINST_THISCOMPONENT,
                 this
             );
    if (m_hwnd == nullptr) {
        DWORD lastError = GetLastError();
        LOG(SEVERITY_LEVEL_ERROR) << util::LastError(util::LastErrorArgs(L"CreateWindow", lastError));
        throw std::runtime_error("CreateWindow");
    }
    _InitializeAfterCreateWindow();
    SetGestureConfigs(m_hwnd);
    ::ShowWindow(m_hwnd, SW_SHOWNORMAL);
    ::UpdateWindow(m_hwnd);
    ::SetFocus(m_hwnd);

    m_pTouchMindApp->GetUIFramework()->InvalidateUICommand(cmdFontControl, UI_INVALIDATIONS_ALLPROPERTIES, 0);
    m_pTouchMindApp->GetUIFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_Enabled);
    m_pTouchMindApp->GetUIFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_ColorType);
    m_pTouchMindApp->GetUIFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_Color);
    m_pTouchMindApp->GetUIFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_LargeImage);

    int nArgs = 0;
    LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if (nArgs >= 2 && szArglist != nullptr) {
        std::wstring fileName(szArglist[1]);
        touchmind::model::MapModelIOResult result = GetMapModel()->Open(fileName);
        if (result != touchmind::model::MapModelIOResult_OK) {
            MessageBox(m_hwnd, L"File I/O Error", L"Could not save the file because of I/O error.",
                       MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
        }
    }

    m_isInitialized = true;
}

void touchmind::win::CanvasPanel::_InitializeAfterCreateWindow()
{
    m_pContext->SetHWnd(m_hwnd);
    m_pEditControlManager->Initialize(
        m_hwnd,
        m_pTouchMindApp->GetConfiguration(),
        m_pTouchMindApp->GetTSFManager(),
        m_pContext->GetD2DFactory(),
        m_pContext->GetDWriteFactory());
    m_pEditControlManager->SetScrollBarHelper(GetScrollBarHelper());
    m_pNodeViewManager->SetConfiguration(m_pTouchMindApp->GetConfiguration());
    m_pNodeViewManager->SetMapModel(GetMapModel());
    m_pNodeViewManager->SetEditControlManager(m_pEditControlManager.get());
    m_pLinkViewManager->SetMapModel(GetMapModel());
    m_pLinkViewManager->SetConfiguration(m_pTouchMindApp->GetConfiguration());
    m_pDWriteTextSizeProvider->SetNodeViewManager(m_pNodeViewManager.get());
    m_pDWriteTextSizeProvider->SetEditControlManager(m_pEditControlManager.get());
    m_pManipulationHelper->SetHitTestMargin(m_pTouchMindApp->GetConfiguration()->GetHitTestMargin());
    m_pPrintSetupDialog->SetParentHWnd(m_hwnd);
    m_pXPSPrint->SetParentHWnd(m_hwnd);
    m_pXPSPrint->SetNodeViewManager(m_pNodeViewManager.get());
    m_pXPSPrint->SetEditControlManager(m_pEditControlManager.get());
    m_pXPSPrint->SetConfiguration(GetTouchMindApp()->GetConfiguration());
    m_pXPSPrint->Initialize();
    m_nodeResizingOperation->SetConfiguration(m_pTouchMindApp->GetConfiguration());
    m_nodeResizingOperation->SetManipulationHelper(m_pManipulationHelper.get());
    m_nodeMovingOperation->SetHWnd(m_hwnd);
    m_nodeMovingOperation->SetConfiguration(m_pTouchMindApp->GetConfiguration());
    m_nodeMovingOperation->SetManipulationHelper(m_pManipulationHelper.get());
    m_nodeMovingOperation->SetNodeViewManager(m_pNodeViewManager.get());
    m_nodeMovingOperation->SetScrollBarHelper(m_pScrollBarHelper.get());

    m_pTouchMindApp->GetRibbonRequestDispatcher()->SetSelectionManager(m_selectionManager.get());
    m_selectionManager->AddSelectionChangeListener([&](
                std::shared_ptr<touchmind::selection::SelectableSupport> oldSelectedItem,
    std::shared_ptr<touchmind::selection::SelectableSupport> newSelectedItem) {
        m_pTouchMindApp->GetRibbonRequestDispatcher()->SelectionChanged(oldSelectedItem, newSelectedItem);
    });
    m_pLayoutManager->SetConfiguration(m_pTouchMindApp->GetConfiguration());
    m_pMapModel->SetConfiguration(m_pTouchMindApp->GetConfiguration());
    m_pMapModel->SetSelectionManager(m_selectionManager.get());
    m_pMapModel->SetFileOperation(m_pTouchMindApp->GetFileOperation());
    m_pMapModel->AddMapModelChangeListener(this);
    m_pMapModel->Initialize();
}

HRESULT touchmind::win::CanvasPanel::CreateD2DResources(
    touchmind::Context* pContext,
    ID2D1Factory* pD2DFactory,
    ID2D1RenderTarget* pRenderTarget)
{
    UNREFERENCED_PARAMETER(pD2DFactory);
    m_createLinkOperation->CreateDeviceDependentResources(pContext, pRenderTarget);
    m_pNodeViewManager->CreateSharedDeviceResources(pContext, pRenderTarget);
    m_pLinkViewManager->CreateSharedDeviceResources(pContext, pRenderTarget);
    m_nodeResizingOperation->CreateDeviceDependentResources(pContext, pRenderTarget);
    return S_OK;
}

void touchmind::win::CanvasPanel::DiscardDeviceResources()
{
    m_pNodeViewManager->DiscardSharedDeviceResources();
    m_pLinkViewManager->DiscardSharedDeviceResources();
}

void touchmind::win::CanvasPanel::OnCreate()
{
}

void touchmind::win::CanvasPanel::OnRender()
{
    UI_ANIMATION_SECONDS secondsNow;
    CHK_HR(m_pContext->GetAnimationTimer()->GetTime(&secondsNow));
    CHK_HR(m_pContext->GetAnimationManager()->Update(secondsNow));
    CHK_HR(m_pContext->OnRender());
    UI_ANIMATION_MANAGER_STATUS status;
    CHK_HR(m_pContext->GetAnimationManager()->GetStatus(&status));
    if (status == UI_ANIMATION_MANAGER_BUSY) {
        Invalidate();
    }
}

HRESULT touchmind::win::CanvasPanel::Render2D(
    touchmind::Context* pContext,
    ID2D1RenderTarget* pRenderTarget)
{
    HRESULT hr = S_OK;

    m_pEditControlManager->SetD2DRenderTarget(pRenderTarget);

    // layout
    m_pNodeViewManager->SynchronizeViewBeforeArrange();
    m_pLayoutManager->SetModel(GetRootNodeModel());
    m_pLayoutManager->SetTextSizeProvider(m_pDWriteTextSizeProvider.get());
    m_pLayoutManager->Arrange();

    // ***** start draw *****
    pRenderTarget->BeginDraw();
    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

    // calculate scroll bar positions
    D2D1_SIZE_F rtSize = pRenderTarget->GetSize();
    CalculateScrolls(rtSize.width, rtSize.height);

    // coordinate transform
    FLOAT centerX = rtSize.width / 2.0f + m_pScrollBarHelper->GetTransformX();
    FLOAT centerY = rtSize.height / 2.0f + m_pScrollBarHelper->GetTransformY();
    pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(centerX, centerY));

    m_pLinkViewManager->Draw(pContext, pRenderTarget);
    m_pNodeViewManager->DrawNodes(pContext, pRenderTarget, m_pEdittingNode);
    if (m_pEdittingNode != nullptr) {
        m_pNodeViewManager->DrawNode(pContext, pRenderTarget, m_pEdittingNode);
    }
    if ((m_pManipulationHelper->GetManipulationStatus() == touch::MANIPULATION_STATUS_MOVING_NODE ||
            m_pManipulationHelper->GetManipulationStatus() == touch::MANIPULATION_STATUS_MOVING_NODE_WITH_PAN_GESTURE) &&
            m_pManipulationHelper->GetMovingNode() != nullptr) {
        m_nodeMovingOperation->Draw(pContext, pRenderTarget, m_pEdittingNode);
    }
    if (m_pManipulationHelper->IsResizing()) {
        m_nodeResizingOperation->Draw(pContext, pRenderTarget);
    }
    if (GetState() == CANVAS_STATE_CREATING_LINK) {
        m_createLinkOperation->Draw(pContext, pRenderTarget);
    }
    CHK_ERROR_HRESULT(hr, pRenderTarget->EndDraw());
    // ***** end draw *****

    // update scroll bars
    m_pScrollBarHelper->UpdateScrollInfo(m_hwnd);

    if (hr == D2DERR_RECREATE_TARGET) {
        hr = S_OK;
        DiscardDeviceResources();
    }

    return hr;
}

void touchmind::win::CanvasPanel::ResizeByParentResized()
{
    ResizeByRibbonResized(m_ribbonHeight);
}

void touchmind::win::CanvasPanel::ResizeByRibbonResized(UINT ribbonHeight)
{
    UINT statusBarHeight = GetTouchMindApp()->GetStatusBarHeight();
    m_ribbonHeight = ribbonHeight;
    HWND parentHwnd = GetTouchMindApp()->GetHwnd();
    RECT rc;
    ::GetClientRect(parentHwnd, &rc);
    rc.top = ribbonHeight;
    rc.bottom -= ribbonHeight;
    rc.bottom -= statusBarHeight;
    ::MoveWindow(m_hwnd, rc.left, rc.top, rc.right, rc.bottom, TRUE);
}

void touchmind::win::CanvasPanel::OnResize(UINT width, UINT height)
{
    //m_pNodeViewManager->DiscardAllDeviceResources();
    //m_pLinkViewManager->DiscardAllDeviceResources();

    m_pContext->OnResize(width, height);
    m_pScrollBarHelper->UpdateScrollInfo(m_hwnd);
}

void touchmind::win::CanvasPanel::CalculateScrolls(FLOAT width, FLOAT height)
{
    auto root = GetRootNodeModel();
    touchmind::layout::TreeRectVisitor treeRectVisitor;
    root->ApplyVisitor(treeRectVisitor);

    FLOAT marginX = width / 4;
    FLOAT marginY = height / 4;
    m_pScrollBarHelper->SetModelRect(
        treeRectVisitor.treeRect.x1 - marginX,
        treeRectVisitor.treeRect.y1 - marginY,
        treeRectVisitor.treeRect.GetWidth() + marginX * 2,
        treeRectVisitor.treeRect.GetHeight() + marginY * 2);
    m_pScrollBarHelper->SetWindowSize((UINT) width, (UINT) height);
}

void touchmind::win::CanvasPanel::OnHScroll(HWND hWnd, WPARAM wParam)
{
    UNREFERENCED_PARAMETER(hWnd);
    FinishEditting();

    SCROLLINFO hScrollInfo;
    touchmind::Configuration* config = m_pTouchMindApp->GetConfiguration();
    WORD type = LOWORD(wParam);
    bool posChanged = false;
    switch (type) {
    case SB_LINEDOWN:
        m_pScrollBarHelper->MoveWindowRight(config->GetLineScrollDistance());
        posChanged = true;
        break;
    case SB_LINEUP:
        m_pScrollBarHelper->MoveWindowLeft(config->GetLineScrollDistance());
        posChanged = true;
        break;
    case SB_PAGEDOWN:
        m_pScrollBarHelper->MoveWindowRight(config->GetPageScrollDistance());
        posChanged = true;
        break;
    case SB_PAGEUP:
        m_pScrollBarHelper->MoveWindowLeft(config->GetPageScrollDistance());
        posChanged = true;
        break;
    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
        hScrollInfo.cbSize = sizeof(hScrollInfo);
        hScrollInfo.fMask = SIF_TRACKPOS;
        ::GetScrollInfo(m_hwnd, SB_HORZ, &hScrollInfo);
        m_pScrollBarHelper->MoveHScrollPositionTo(hScrollInfo.nTrackPos);
        posChanged = true;
        break;
    default:
        LOG(SEVERITY_LEVEL_WARN) << L"type = UNKNOWN(" << type << ")";
        break;
    }

    if (posChanged) {
        Invalidate();
        m_pScrollBarHelper->GetHScrollInfoForPositionChange(&hScrollInfo);
        ::SetScrollInfo(m_hwnd, SB_HORZ, &hScrollInfo, TRUE);
    }
}

void touchmind::win::CanvasPanel::OnVScroll(HWND hWnd, WPARAM wParam)
{
    UNREFERENCED_PARAMETER(hWnd);
    FinishEditting();

    SCROLLINFO vScrollInfo;
    touchmind::Configuration* config = m_pTouchMindApp->GetConfiguration();
    WORD type = LOWORD(wParam);
    bool posChanged = false;
    switch (type) {
    case SB_LINEDOWN:
        m_pScrollBarHelper->MoveWindowDown(config->GetLineScrollDistance());
        posChanged = true;
        break;
    case SB_LINEUP:
        m_pScrollBarHelper->MoveWindowUp(config->GetLineScrollDistance());
        posChanged = true;
        break;
    case SB_PAGEDOWN:
        m_pScrollBarHelper->MoveWindowDown(config->GetPageScrollDistance());
        posChanged = true;
        break;
    case SB_PAGEUP:
        m_pScrollBarHelper->MoveWindowUp(config->GetPageScrollDistance());
        posChanged = true;
        break;
    case SB_THUMBTRACK:
    case SB_THUMBPOSITION:
        vScrollInfo.cbSize = sizeof(vScrollInfo);
        vScrollInfo.fMask = SIF_TRACKPOS;
        ::GetScrollInfo(m_hwnd, SB_VERT, &vScrollInfo);
        m_pScrollBarHelper->MoveVScrollPositionTo(vScrollInfo.nTrackPos);
        posChanged = true;
        break;
    case SB_ENDSCROLL:
        break;
    default:
        LOG(SEVERITY_LEVEL_WARN) << L"type = UNKNOWN(" << type << ")";
        break;
    }

    if (posChanged) {
        Invalidate();
        m_pScrollBarHelper->GetHScrollInfoForPositionChange(&vScrollInfo);
        ::SetScrollInfo(m_hwnd, SB_HORZ, &vScrollInfo, TRUE);
    }
}

void touchmind::win::CanvasPanel::EditNode(const std::shared_ptr<touchmind::model::node::NodeModel> &node)
{
    m_pScrollBarHelper->MoveWindowTo(node->GetX() + node->GetWidth() / 2.0f,
                                     node->GetY() + node->GetHeight() / 2.0f);

    Insets insets = GetTouchMindApp()->GetConfiguration()->GetInsets();
    D2D1_POINT_2F nodeClientPoint;
    nodeClientPoint.x = node->GetX();
    nodeClientPoint.y = node->GetY();
    POINT nodeWindowPoint;

    touchmind::util::CoordinateConversion::ConvertModelToWindowCoordinate(m_hwnd, GetScrollBarHelper(), nodeClientPoint, &nodeWindowPoint);
    m_pEdittingNode = node;
    m_pNodeViewManager->SynchronizeWithModel();
    auto editControlIndex = m_pNodeViewManager->GetEditControlIndexFromNodeId(m_pEdittingNode->GetId());
    if (editControlIndex >= 0) {
        auto editControl = m_pEditControlManager->GetEditControl(editControlIndex);
        m_pEditControlManager->SetFocus(editControlIndex);
    }
    Invalidate();
}

void touchmind::win::CanvasPanel::FinishEditting()
{
    if (m_pEdittingNode != nullptr) {
        SYSTEMTIME currentTime;
        GetLocalTime(&currentTime);
        auto editControl = m_pEditControlManager->GetFocusedEditControl();
        D2D1_RECT_F actualRect = editControl->GetActualRect();
        editControl->SetLayoutRect(actualRect);
        std::wstring text;
        editControl->GetText(text);
        if (text != m_pEdittingNode->GetText()) {
            m_pNodeViewManager->SyncFontAttributesFromTextLayout();
            m_pTouchMindApp->GetMapModel()->BeginTransaction();
            m_pEdittingNode->SetText(text);
            m_pEdittingNode->SetModifiedTime(currentTime);
            m_pTouchMindApp->GetMapModel()->EndTransaction();
        }
        m_pEditControlManager->KillFocus();
        m_pEdittingNode = nullptr;
    }

    Invalidate();
}

void touchmind::win::CanvasPanel::ArrangeNodes()
{
    m_pLayoutManager->SetModel(GetRootNodeModel());
    m_pLayoutManager->SetTextSizeProvider(m_pDWriteTextSizeProvider.get());
    m_pLayoutManager->Arrange();
    Invalidate();
}

void touchmind::win::CanvasPanel::SetSelectedNode(const std::shared_ptr<touchmind::model::node::NodeModel> &node)
{
    if (node != nullptr) {
        node->SetSelected();
        auto editControlIndex = m_pNodeViewManager->GetEditControlIndexFromNodeId(node->GetId());
        m_pEditControlManager->SetSelectedNode(editControlIndex);
    } else {
        GetSelectionManager()->ClearSelected();
        m_pEditControlManager->CancelSelectedNode();
    }
    m_pTouchMindApp->GetUIFramework()->InvalidateUICommand(cmdFontControl, UI_INVALIDATIONS_ALLPROPERTIES, 0);
    m_pTouchMindApp->GetUIFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_Enabled);
    m_pTouchMindApp->GetUIFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_ColorType);
    m_pTouchMindApp->GetUIFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_Color);
    m_pTouchMindApp->GetUIFramework()->InvalidateUICommand(cmdBackgroundColor, UI_INVALIDATIONS_PROPERTY, &UI_PKEY_LargeImage);
}

void touchmind::win::CanvasPanel::CancelSelectedNode()
{
    SetSelectedNode(nullptr);
}

/// <summary>notified that an edit control has been changed</summary>
// Control::IDWLightEditControlChangeListener
void touchmind::win::CanvasPanel::NotifyEditControlHasChanged(int editControlIndex)
{
    touchmind::NODE_ID nodeId = m_pNodeViewManager->GetNodeIdFromEditControlIndex(editControlIndex);
    if (nodeId >= 0) {
        auto node = GetRootNodeModel()->FindByNodeId(nodeId);
        if (node != nullptr) {
            node->IncrementRepaintRequiredCounter();
            node->IncrementSaveRequiredCounter();
            Invalidate();
        }
    }
}

/// <summary>notified that an edit control's selection has been changed </summary>
// Control::IDWLightEditControlSelectionChangeListener
void touchmind::win::CanvasPanel::NotifySelectionHasChanged(int editControlIndex)
{
    UNREFERENCED_PARAMETER(editControlIndex);
    m_pTouchMindApp->GetUIFramework()->InvalidateUICommand(cmdFontControl, UI_INVALIDATIONS_ALLPROPERTIES, 0);
}

void touchmind::win::CanvasPanel::ResizeLayoutRect(
    const std::shared_ptr<touchmind::model::node::NodeModel> &node, 
    FLOAT width, 
    FLOAT height)
{
    auto editControlIndex = m_pNodeViewManager->GetEditControlIndexFromNodeId(node->GetId());
    if (editControlIndex >= 0) {
        auto editControl = m_pEditControlManager->GetEditControl(editControlIndex);
        if (editControl != nullptr) {
            editControl->ResizeLayoutRect(width, height);
            node->IncrementRepaintRequiredCounter();
        }
    }
    Invalidate();
}

void touchmind::win::CanvasPanel::ExpandLayoutRect(
    const std::shared_ptr<touchmind::model::node::NodeModel> &node, 
    FLOAT dx, 
    FLOAT dy)
{
    auto editControlIndex = m_pNodeViewManager->GetEditControlIndexFromNodeId(node->GetId());
    std::shared_ptr<control::DWriteEditControl> editControl = m_pEditControlManager->GetEditControl(editControlIndex);
    editControl->ExpandLayoutRect(dx, dy);
    node->IncrementRepaintRequiredCounter();
    Invalidate();
}

void touchmind::win::CanvasPanel::Invalidate()
{
    ::InvalidateRect(m_hwnd, nullptr, TRUE);
}

void touchmind::win::CanvasPanel::BeforeNew(touchmind::model::MapModel* pMapModel)
{
    UNREFERENCED_PARAMETER(pMapModel);
}

void touchmind::win::CanvasPanel::AfterNew(touchmind::model::MapModel* pMapModel)
{
    UNREFERENCED_PARAMETER(pMapModel);
    m_pNodeViewManager->DiscardAllResources();
    m_pNodeViewManager->SynchronizeWithModel();
    m_pLinkViewManager->Resynchronize(
        m_pContext.get(),
        m_pContext->GetD2DRenderTarget());
    Invalidate();
}

void touchmind::win::CanvasPanel::BeforeOpen(touchmind::model::MapModel* pMapModel)
{
    UNREFERENCED_PARAMETER(pMapModel);
}

void touchmind::win::CanvasPanel::AfterOpen(touchmind::model::MapModel* pMapModel)
{
    UNREFERENCED_PARAMETER(pMapModel);
    // discard all visual resources
    m_pNodeViewManager->DiscardAllResources();
    // create visual resources for nodes within opened mind map
    m_pNodeViewManager->SynchronizeWithModel();
    // copy all font attributes to text layouts of the visual resources
    m_pNodeViewManager->SyncFontAttributesToTextLayout();
    // synchronize again
    m_pNodeViewManager->SynchronizeWithModel();
    Invalidate();
}

void touchmind::win::CanvasPanel::BeforeSave(touchmind::model::MapModel* pMapModel)
{
    UNREFERENCED_PARAMETER(pMapModel);
    // copy all font attributes of text layouts to node's font attributes
    m_pNodeViewManager->SyncFontAttributesFromTextLayout();
}

void touchmind::win::CanvasPanel::AfterSave(touchmind::model::MapModel* pMapModel)
{
    UNREFERENCED_PARAMETER(pMapModel);
}

void touchmind::win::CanvasPanel::OnEditCopy()
{
    auto selectedNode =
        std::dynamic_pointer_cast<touchmind::model::node::NodeModel>(GetSelectionManager()->GetSelected());
    if (selectedNode != nullptr) {
        m_pNodeViewManager->SyncFontAttributesFromTextLayout();
        m_pClipboard->CopyNodeModel(m_hwnd, selectedNode);
    }
    Invalidate();
}

void touchmind::win::CanvasPanel::OnEditPaste()
{
    auto node = touchmind::model::node::NodeModel::Create(GetSelectionManager());
    std::vector<std::shared_ptr<model::link::LinkModel>> links;
    m_pClipboard->PasteNodeModel(m_hwnd, node, links);
    auto selectedNode =
        std::dynamic_pointer_cast<touchmind::model::node::NodeModel>(GetSelectionManager()->GetSelected());
    if (selectedNode != nullptr) {
        m_pNodeViewManager->SyncFontAttributesFromTextLayout();
        m_pTouchMindApp->GetMapModel()->BeginTransaction();
        selectedNode->AddChild(node);
        m_pMapModel->AddLinks(links);
        m_pTouchMindApp->GetMapModel()->EndTransaction();

        ArrangeNodes();

        m_pNodeViewManager->SynchronizeWithModel();
        m_pNodeViewManager->SyncFontAttributesToTextLayout();
        m_pNodeViewManager->SynchronizeWithModel();
    }
    Invalidate();
}

void touchmind::win::CanvasPanel::OnEditCut()
{
    auto selectedNode =
        std::dynamic_pointer_cast<touchmind::model::node::NodeModel>(GetSelectionManager()->GetSelected());
    if (selectedNode != nullptr && selectedNode->GetParent() != nullptr) {
        m_pNodeViewManager->SyncFontAttributesFromTextLayout();
        if (SUCCEEDED(m_pClipboard->CopyNodeModel(m_hwnd, selectedNode))) {
            auto parent = selectedNode->GetParent();
            m_pNodeViewManager->SyncFontAttributesFromTextLayout();
            m_pMapModel->BeginTransaction();
            parent->RemoveChild(selectedNode);
            m_pMapModel->NormalizeLinks();
            m_pMapModel->EndTransaction();

            ArrangeNodes();
        }
    }

    Invalidate();
}

void touchmind::win::CanvasPanel::OnEditDelete()
{
    auto selectedNode =
        std::dynamic_pointer_cast<touchmind::model::node::NodeModel>(GetSelectionManager()->GetSelected());
    if (selectedNode != nullptr) {
        auto parent = selectedNode->GetParent();
        if (parent != nullptr) {
            m_pNodeViewManager->SyncFontAttributesFromTextLayout();
            m_pMapModel->BeginTransaction();
            parent->RemoveChild(selectedNode);
            m_pMapModel->NormalizeLinks();
            m_pMapModel->EndTransaction();
            ArrangeNodes();
        }
    }

    Invalidate();
}

void touchmind::win::CanvasPanel::OnEditUndo()
{
    m_pTouchMindApp->GetMapModel()->Roleback();
    m_pNodeViewManager->SynchronizeWithModel();
    m_pNodeViewManager->SyncFontAttributesToTextLayout();
    m_pNodeViewManager->SynchronizeWithModel();
    m_pLinkViewManager->Resynchronize(
        m_pContext.get(),
        m_pContext->GetD2DRenderTarget());
    Invalidate();
}

void touchmind::win::CanvasPanel::OnDebugDump( std::wofstream& os )
{
    m_pNodeViewManager->OnDebugDump(os);
}

void touchmind::win::CanvasPanel::OnPrintSetup()
{
    m_pPrintSetupDialog->Show();
}

void touchmind::win::CanvasPanel::OnPrint()
{
    XPS_SIZE xpsSize = m_pPrintSetupDialog->GetSelectedPageXpsSize();
    touchmind::print::XPSPRINT_MARGIN xpsMargin = m_pPrintSetupDialog->GetMargin();
    m_pXPSPrint->SetPageSize(xpsSize);
    m_pXPSPrint->SetMargin(xpsMargin);
    m_pXPSPrint->Print(GetTouchMindApp()->GetMapModel());
}

void touchmind::win::CanvasPanel::OnCreateLink()
{
    if (m_canvasState == CANVAS_STATE_NORMAL) {
        m_canvasState = CANVAS_STATE_CREATING_LINK;
        m_createLinkOperation->StartSelectFirstNode(m_pContext.get());
    } else {
        m_createLinkOperation->EndOperation();
        OnCreateLink_End();
    }
    Invalidate();
}

void touchmind::win::CanvasPanel::OnCreateLink_End()
{
    m_canvasState = CANVAS_STATE_NORMAL;
    m_pTouchMindApp->GetRibbonFramework()->GetFramework()->InvalidateUICommand(
        cmdCreateLink,
        UI_INVALIDATIONS_VALUE,
        nullptr);
}

// IAnimationStatusChangedListener (start)
HRESULT touchmind::win::CanvasPanel::OnAnimationStatusChanged(
    UI_ANIMATION_MANAGER_STATUS newStatus,
    UI_ANIMATION_MANAGER_STATUS previousStatus )
{
    UNREFERENCED_PARAMETER(newStatus);
    UNREFERENCED_PARAMETER(previousStatus);
    Invalidate();
    return S_OK;
}

// IAnimationStatusChangedListener (end)

touchmind::model::MapModel* touchmind::win::CanvasPanel::GetMapModel() const
{
    return m_pMapModel.get();
}

std::shared_ptr<touchmind::model::node::NodeModel> touchmind::win::CanvasPanel::GetRootNodeModel() const
{
    return GetMapModel()->GetRootNodeModel();
}

void touchmind::win::CanvasPanel::SetGestureConfigs(HWND hWnd)
{
    if (!touchmind::util::OSVersionChecker::IsWin7()) {
        return;
    }
    GESTURECONFIG gc[5];
    UINT uiGcs = 5;
    gc[0].dwID = GID_ZOOM;
    gc[0].dwWant = GC_ZOOM;
    gc[0].dwBlock = 0;
    gc[1].dwID = GID_PAN;
    gc[1].dwWant = GC_PAN_WITH_INERTIA;
    gc[1].dwBlock = 0;
    gc[2].dwID = GID_ROTATE;
    gc[2].dwWant = 0;
    gc[2].dwBlock = GC_ROTATE;
    gc[3].dwID = GID_TWOFINGERTAP;
    gc[3].dwWant = GC_TWOFINGERTAP;
    gc[3].dwBlock = 0;
    gc[4].dwID = GID_PRESSANDTAP;
    gc[4].dwWant = GC_PRESSANDTAP;
    gc[4].dwBlock = 0;
    BOOL bResult = SetGestureConfig(hWnd, 0, uiGcs, gc, sizeof(GESTURECONFIG));
    if (!bResult) {
        DWORD lastError = GetLastError();
        LOG(SEVERITY_LEVEL_ERROR) << util::LastError(util::LastErrorArgs(L"SetGestureConfig", lastError));
    }
}

touchmind::operation::FileOperation* touchmind::win::CanvasPanel::GetFileOperation() const
{
    return m_pTouchMindApp->GetFileOperation();
}

void touchmind::win::CanvasPanel::OnDeleteLink()
{
    auto selectedLink =
        std::dynamic_pointer_cast<touchmind::model::link::LinkModel>(GetSelectionManager()->GetSelected());
    if (selectedLink != nullptr) {
        m_pMapModel->RemoveLink(selectedLink);
    }
    Invalidate();
}

void touchmind::win::CanvasPanel::OnDestroy()
{
    m_createLinkOperation->Destroy();
    m_nodeMovingOperation->Destroy();
    m_nodeMovingOperation->Destroy();
}
