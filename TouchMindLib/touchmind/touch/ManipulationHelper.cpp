#include "Stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/win/CanvasPanel.h"
#include "touchmind/win/TouchMindApp.h"
#include "touchmind/operation/CreateLinkOperation.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/MapModel.h"
#include "touchmind/touch/ManipulationHelper.h"
#include "touchmind/util/CoordinateConversion.h"
#include "touchmind/util/ScrollBarHelper.h"
#include "touchmind/visitor/NodeHitTestVisitor.h"
#include "touchmind/view/link/LinkViewManager.h"
#include "touchmind/view/link/BaseLinkView.h"
#include "touchmind/view/node/NodeViewManager.h"
#include "touchmind/selection/SelectionManager.h"

touchmind::touch::ManipulationHelper::ManipulationHelper():
    m_pCanvasPanel(nullptr),
    m_manipulationStatus(MANIPULATION_STATUS_NORMAL),
    m_pMovingNode(),
    m_pResizingNode(),
    m_rectBeforeResize(D2D1::RectF()),
    m_rectOnResize(D2D1::RectF()),
    m_hitTestMargin(5.0f)
{
}

touchmind::touch::ManipulationHelper::~ManipulationHelper()
{
}

touchmind::touch::ManipulationResult touchmind::touch::ManipulationHelper::OnLeftMouseDownExt(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    POINT windowPoint,
    ULONGLONG timestampInMilliSeconds)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);

    touchmind::Context *pContext = m_pCanvasPanel->GetContext();
    ID2D1RenderTarget *pRenderTarget = m_pCanvasPanel->GetContext()->GetD2DRenderTarget();

    m_pCanvasPanel->FinishEditting();

    D2D1_POINT_2F clientPoint;
    util::CoordinateConversion::ConvertWindowToModelCoordinate(
        hWnd,
        m_pCanvasPanel->GetScrollBarHelper(),
        windowPoint,
        &clientPoint);

    auto linkViewManager = m_pCanvasPanel->GetLinkViewManager();
    auto link = linkViewManager->HitTest(
                    pContext,
                    pRenderTarget,
                    clientPoint);
    if (link != nullptr) {
        link->SetSelected();
    } else {
        auto nodeViewManager = m_pCanvasPanel->GetNodeViewManager();
        auto path = nodeViewManager->PathHitTest(pContext, pRenderTarget, clientPoint);
        if (path != nullptr) {
            path->SetSelected();
        } else {
            auto node = nodeViewManager->NodeHitTest(m_pCanvasPanel->GetContext(), m_pCanvasPanel->GetContext()->GetD2DRenderTarget(), clientPoint);
            if (m_pCanvasPanel->GetState() == touchmind::win::CANVAS_STATE_NORMAL) {
                m_pCanvasPanel->SetSelectedNode(node);
            } else if (m_pCanvasPanel->GetState() == touchmind::win::CANVAS_STATE_CREATING_LINK) {
                m_pCanvasPanel->GetCreateLinkOperation()->SetSelectedNode(pContext, node);
            }
        }
    }

    ManipulationResult result(true, true);
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::ManipulationHelper::OnSingleClick(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    POINT windowPoint,
    ULONGLONG timestampInMilliSeconds)
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(windowPoint);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);

    m_pMovingNode.reset();
    m_pResizingNode.reset();

    ManipulationResult result(false, false);
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::ManipulationHelper::OnDoubleClick(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    POINT windowPoint,
    ULONGLONG timestampInMilliSeconds)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);

    D2D1_POINT_2F clientPoint;
    util::CoordinateConversion::ConvertWindowToModelCoordinate(
        hWnd, m_pCanvasPanel->GetScrollBarHelper(),
        windowPoint,
        &clientPoint);

    touchmind::visitor::NodeHitTestVisitor hitTestVisitor(clientPoint, GetHitTestMargin());
    m_pCanvasPanel->GetRootNodeModel()->ApplyVisitor(hitTestVisitor);
    if (hitTestVisitor.GetResult() != nullptr) {
        m_pCanvasPanel->EditNode(hitTestVisitor.GetResult());

    } else {
        m_pCanvasPanel->FinishEditting();
    }
    ManipulationResult result(true, true);
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::ManipulationHelper::OnDraggingStart(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    RECT windowDragRect,
    LONG dx,
    LONG dy,
    ULONGLONG timestampInMilliSeconds)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(dx);
    UNREFERENCED_PARAMETER(dy);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);

    D2D1_POINT_2F startPointInModel;
    util::CoordinateConversion::ConvertWindowToModelCoordinate(hWnd, m_pCanvasPanel->GetScrollBarHelper(),
            windowDragRect.left, windowDragRect.top,
            &startPointInModel);

    touchmind::visitor::NodeHitTestVisitor hitTestVisitor(startPointInModel, GetHitTestMargin());
    m_pCanvasPanel->GetRootNodeModel()->ApplyVisitor(hitTestVisitor);
    std::shared_ptr<touchmind::model::node::NodeModel> pHitNode = hitTestVisitor.GetResult();

    if (pHitNode != nullptr) {
        FLOAT xInNode = startPointInModel.x - pHitNode->GetX();
        FLOAT yInNode = startPointInModel.y - pHitNode->GetY();
        if (pHitNode->GetWidth() - 10.0f <= xInNode && xInNode <= pHitNode->GetWidth()
                && pHitNode->GetHeight() - 10.0f <= yInNode && yInNode <= pHitNode->GetHeight()) {
            m_manipulationStatus = MANIPULATION_STATUS_RESIZING_NODE;
            m_pResizingNode = pHitNode;
            m_rectBeforeResize =
                D2D1::RectF(pHitNode->GetX(), pHitNode->GetY(), pHitNode->GetX() + pHitNode->GetWidth(),
                            pHitNode->GetY() + pHitNode->GetHeight());
            m_rectOnResize = m_rectBeforeResize;
            SetMouseCursor(MOUSE_CURSOR_TYPE_SIZENWSE);
        } else if (m_pMovingNode == nullptr) {
            if (pHitNode->GetParent() != nullptr) {   // root node can not be moved
                m_manipulationStatus = MANIPULATION_STATUS_MOVING_NODE;
                m_pMovingNode = hitTestVisitor.GetResult();
                SetMouseCursor(MOUSE_CURSOR_TYPE_HAND);
            }
        }
    } else {
        m_manipulationStatus = MANIPULATION_STATUS_SCROLLING;
        m_scrollStartPointInWindow.x = windowDragRect.left;
        m_scrollStartPointInWindow.y = windowDragRect.top;
        SetMouseCursor(MOUSE_CURSOR_TYPE_SIZEALL);
    }

    ManipulationResult result(true, true);
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::ManipulationHelper::OnDragging(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    RECT windowDragRect,
    LONG dx,
    LONG dy,
    ULONGLONG timestampInMilliSeconds)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);

    switch (m_manipulationStatus) {
    case MANIPULATION_STATUS_RESIZING_NODE: {
        if (m_pResizingNode != nullptr) {
            D2D1_RECT_F rect;
            util::CoordinateConversion::ConvertWindowToModelCoordinate(hWnd,
                    m_pCanvasPanel->
                    GetScrollBarHelper(),
                    windowDragRect, &rect);
            m_rectOnResize =
                D2D1::RectF(m_pResizingNode->GetX(), m_pResizingNode->GetY(), rect.right,
                            rect.bottom);
            m_pCanvasPanel->ResizeLayoutRect(m_pResizingNode,
                                             m_rectOnResize.right - m_rectOnResize.left,
                                             m_rectOnResize.bottom - m_rectOnResize.top);
            SetMouseCursor(MOUSE_CURSOR_TYPE_SIZENWSE);
        } else {
            m_manipulationStatus = MANIPULATION_STATUS_NORMAL;
            SetMouseCursor(MOUSE_CURSOR_TYPE_DEFAULT);
        }
    }
    break;
    case MANIPULATION_STATUS_MOVING_NODE: {
        if (m_pMovingNode != nullptr) {
            SetMouseCursor(MOUSE_CURSOR_TYPE_HAND);
        } else {
            m_manipulationStatus = MANIPULATION_STATUS_NORMAL;
            SetMouseCursor(MOUSE_CURSOR_TYPE_DEFAULT);
        }
    }
    break;
    case MANIPULATION_STATUS_SCROLLING: {
        SetMouseCursor(MOUSE_CURSOR_TYPE_SIZEALL);
        m_pCanvasPanel->GetScrollBarHelper()->MoveWindowDelta(static_cast < FLOAT > (-dx),
                static_cast < FLOAT > (-dy));
        m_pCanvasPanel->GetScrollBarHelper()->UpdateScrollInfoForPositionChange(m_pCanvasPanel->GetHwnd());
    }
    break;
    default:
        SetMouseCursor(MOUSE_CURSOR_TYPE_DEFAULT);
    }
    ManipulationResult result(true, true);
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::ManipulationHelper::OnDraggingEnd(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    RECT windowDragRect,
    LONG dx,
    LONG dy,
    ULONGLONG timestampInMilliSeconds)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);

    switch (m_manipulationStatus) {
    case MANIPULATION_STATUS_RESIZING_NODE: {
        if (m_pResizingNode != nullptr) {
            D2D1_RECT_F rect;
            util::CoordinateConversion::ConvertWindowToModelCoordinate(hWnd,
                    m_pCanvasPanel->
                    GetScrollBarHelper(),
                    windowDragRect, &rect);
            m_rectOnResize =
                D2D1::RectF(m_pResizingNode->GetX(), m_pResizingNode->GetY(), rect.right,
                            rect.bottom);
            m_pCanvasPanel->ResizeLayoutRect(m_pResizingNode,
                                             m_rectOnResize.right - m_rectOnResize.left,
                                             m_rectOnResize.bottom - m_rectOnResize.top);
            m_pResizingNode.reset();
            m_rectBeforeResize = D2D1::RectF();
            m_rectOnResize = D2D1::RectF();
        }
    }
    break;
    case MANIPULATION_STATUS_MOVING_NODE: {
        if (m_pMovingNode != nullptr) {
            D2D1_POINT_2F clientPoint;
            util::CoordinateConversion::ConvertWindowToModelCoordinate(hWnd,
                    m_pCanvasPanel->
                    GetScrollBarHelper(),
                    windowDragRect.right,
                    windowDragRect.bottom,
                    &clientPoint);
            touchmind::visitor::NodeHitTestVisitor hitTestVisitor(clientPoint, GetHitTestMargin());
            m_pCanvasPanel->GetRootNodeModel()->ApplyVisitor(hitTestVisitor);
            std::shared_ptr<touchmind::model::node::NodeModel> pHitNode = hitTestVisitor.GetResult();

            if (pHitNode != nullptr && !hitTestVisitor.GetResult()->IsAncestor(m_pMovingNode)) {
                std::shared_ptr<touchmind::model::node::NodeModel> pNewParent = pHitNode;
                std::shared_ptr<touchmind::model::node::NodeModel> pOldParent = m_pMovingNode->GetParent();
                m_pCanvasPanel->GetNodeViewManager()->SyncFontAttributesFromTextLayout();
                m_pCanvasPanel->GetTouchMindApp()->GetMapModel()->BeginTransaction();
                pOldParent->RemoveChild(m_pMovingNode);
                pNewParent->AddChild(m_pMovingNode);
                m_pMovingNode->SetPosition(touchmind::NODE_SIDE_UNDEFINED);
                m_pCanvasPanel->GetTouchMindApp()->GetMapModel()->NormalizeLinks();
                m_pCanvasPanel->GetTouchMindApp()->GetMapModel()->EndTransaction();
                m_pCanvasPanel->ArrangeNodes();
            }

            m_pMovingNode.reset();
        }
    }
    break;
    case MANIPULATION_STATUS_SCROLLING: {
        m_pCanvasPanel->GetScrollBarHelper()->MoveWindowDelta(static_cast < FLOAT > (-dx),
                static_cast < FLOAT > (-dy));
        m_pCanvasPanel->GetScrollBarHelper()->UpdateScrollInfoForPositionChange(m_pCanvasPanel->GetHwnd());
    }
    break;
    }
    m_manipulationStatus = MANIPULATION_STATUS_NORMAL;
    SetMouseCursor(MOUSE_CURSOR_TYPE_DEFAULT);
    ManipulationResult result(true, true);
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::ManipulationHelper::OnHovering(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    POINT windowPoint,
    LONG dx,
    LONG dy,
    ULONGLONG timestampInMilliSeconds)
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(windowPoint);
    UNREFERENCED_PARAMETER(dx);
    UNREFERENCED_PARAMETER(dy);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);

    SetMouseCursor(MOUSE_CURSOR_TYPE_DEFAULT);
    ManipulationResult result(false, false);
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::ManipulationHelper::OnRightSingleClick(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    POINT windowPoint,
    ULONGLONG timestampInMilliSeconds)
{
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);

    touchmind::Context *pContext = m_pCanvasPanel->GetContext();
    ID2D1RenderTarget *pRenderTarget = m_pCanvasPanel->GetContext()->GetD2DRenderTarget();

    D2D1_POINT_2F clientPoint;
    util::CoordinateConversion::ConvertWindowToModelCoordinate(hWnd, m_pCanvasPanel->GetScrollBarHelper(), windowPoint, &clientPoint);

    //touchmind::util::NodeHitTestVisitor hitTestVisitor(clientPoint, GetHitTestMargin());
    //m_pCanvasPanel->GetRootNodeModel()->ApplyVisitor(hitTestVisitor);
    //m_pCanvasPanel->SetSelectedNode(hitTestVisitor.GetResult());

    auto linkViewManager = m_pCanvasPanel->GetLinkViewManager();
    auto link = linkViewManager->HitTest(
                    pContext,
                    pRenderTarget,
                    clientPoint);
    if (link != nullptr) {
        link->SetSelected();
    } else {
        auto nodeViewManager = m_pCanvasPanel->GetNodeViewManager();
        auto path = nodeViewManager->PathHitTest(pContext, pRenderTarget, clientPoint);
        if (path != nullptr) {
            path->SetSelected();
        } else {
            auto node = nodeViewManager->NodeHitTest(m_pCanvasPanel->GetContext(), m_pCanvasPanel->GetContext()->GetD2DRenderTarget(), clientPoint);
            if (m_pCanvasPanel->GetState() == touchmind::win::CANVAS_STATE_NORMAL) {
                m_pCanvasPanel->SetSelectedNode(node);
            } else if (m_pCanvasPanel->GetState() == touchmind::win::CANVAS_STATE_CREATING_LINK) {
                m_pCanvasPanel->GetCreateLinkOperation()->SetSelectedNode(pContext, node);
            }
        }
    }


    ManipulationResult result(true, true);
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::ManipulationHelper::OnGestureBegin(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    POINT windowPoint,
    ULONGLONG timestampInMilliSeconds,
    PGESTUREINFO pGestureInfo)
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(windowPoint);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
    UNREFERENCED_PARAMETER(pGestureInfo);

    ManipulationResult result(true, true);
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::ManipulationHelper::OnGestureEnd(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    POINT windowPoint,
    ULONGLONG timestampInMilliSeconds,
    PGESTUREINFO pGestureInfo)
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(windowPoint);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
    UNREFERENCED_PARAMETER(pGestureInfo);

    m_manipulationStatus = MANIPULATION_STATUS_NORMAL;
    SetMouseCursor(MOUSE_CURSOR_TYPE_DEFAULT);
    ManipulationResult result(true, true);
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::ManipulationHelper::OnZoom(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    POINT windowPoint,
    ULONGLONG timestampInMilliSeconds,
    PGESTUREINFO pGestureInfo,
    POINT zoomCenterPoint,
    FLOAT zoomFactor)
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(windowPoint);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
    UNREFERENCED_PARAMETER(zoomCenterPoint);

    if (pGestureInfo->dwFlags == GF_BEGIN) {
        //m_pResizingNode = touchmind::model::node::NodeModel::GetSelectedNode();
        m_pResizingNode =
            std::dynamic_pointer_cast<touchmind::model::node::NodeModel>(m_pCanvasPanel->GetSelectionManager()->GetSelected());
        if (m_pResizingNode != nullptr) {
            m_manipulationStatus = MANIPULATION_STATUS_RESIZING_NODE_WITH_ZOOM_GESTURE;
            m_rectBeforeResize = m_rectOnResize =
                                     D2D1::RectF(m_pResizingNode->GetX(),
                                                 m_pResizingNode->GetY(),
                                                 m_pResizingNode->GetX() + m_pResizingNode->GetWidth(),
                                                 m_pResizingNode->GetY() + m_pResizingNode->GetHeight());
            SetMouseCursor(MOUSE_CURSOR_TYPE_SIZENESW);
        }
    } else if (pGestureInfo->dwFlags == GF_END) {
        m_manipulationStatus = MANIPULATION_STATUS_NORMAL;
        m_pResizingNode.reset();
        m_rectOnResize = m_rectBeforeResize = D2D1::RectF();
        SetMouseCursor(MOUSE_CURSOR_TYPE_DEFAULT);
    } else if (std::dynamic_pointer_cast<touchmind::model::node::NodeModel>(m_pCanvasPanel->GetSelectionManager()->GetSelected()) != nullptr && zoomFactor > 0.1f) {
        m_manipulationStatus = MANIPULATION_STATUS_RESIZING_NODE_WITH_ZOOM_GESTURE;
        FLOAT width = m_rectOnResize.right - m_rectOnResize.left;
        FLOAT height = m_rectOnResize.bottom - m_rectOnResize.top;
        FLOAT newWidth = width * zoomFactor;
        FLOAT newHeight = height * zoomFactor;
        m_rectOnResize =
            D2D1::RectF(m_pResizingNode->GetX(), m_pResizingNode->GetY(), m_pResizingNode->GetX() + newWidth,
                        m_pResizingNode->GetY() + height);
        m_pCanvasPanel->ResizeLayoutRect(m_pResizingNode, newWidth, newHeight);
        SetMouseCursor(MOUSE_CURSOR_TYPE_SIZENWSE);
    }
    ManipulationResult result(true, true);
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::ManipulationHelper::OnPan(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    POINT windowPoint,
    ULONGLONG timestampInMilliSeconds,
    PGESTUREINFO pGestureInfo,
    LONG dx,
    LONG dy)
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(windowPoint);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);

    bool isMoving = false;
    if (pGestureInfo->dwFlags & GF_BEGIN) {
        D2D1_POINT_2F startPointInModel;
        util::CoordinateConversion::ConvertWindowToModelCoordinate(hWnd, m_pCanvasPanel->GetScrollBarHelper(),
                windowPoint.x, windowPoint.y,
                &startPointInModel);

        touchmind::visitor::NodeHitTestVisitor hitTestVisitor(startPointInModel, GetHitTestMargin());
        m_pCanvasPanel->GetRootNodeModel()->ApplyVisitor(hitTestVisitor);
        std::shared_ptr<touchmind::model::node::NodeModel> pHitNode = hitTestVisitor.GetResult();
        if (pHitNode != nullptr) {
            // start moving with pan
            if (m_pMovingNode == nullptr && pHitNode->GetParent() != nullptr) {
                m_manipulationStatus = MANIPULATION_STATUS_MOVING_NODE_WITH_PAN_GESTURE;
                m_pMovingNode = hitTestVisitor.GetResult();
                m_panMovingPoint = pGestureInfo->ptsLocation;
                SetMouseCursor(MOUSE_CURSOR_TYPE_HAND);
                isMoving = true;
            }
        }
    } else if (m_manipulationStatus == MANIPULATION_STATUS_MOVING_NODE_WITH_PAN_GESTURE) {
        if (pGestureInfo->dwFlags & GF_END) {
            if (m_pMovingNode != nullptr) {
                // finish moving with pan
                D2D1_POINT_2F clientPoint;
                util::CoordinateConversion::ConvertWindowToModelCoordinate(hWnd,
                        m_pCanvasPanel->
                        GetScrollBarHelper(),
                        windowPoint.x,
                        windowPoint.y,
                        &clientPoint);
                touchmind::visitor::NodeHitTestVisitor hitTestVisitor(clientPoint, GetHitTestMargin());
                m_pCanvasPanel->GetRootNodeModel()->ApplyVisitor(hitTestVisitor);
                std::shared_ptr<touchmind::model::node::NodeModel> pHitNode = hitTestVisitor.GetResult();

                if (pHitNode != nullptr && !hitTestVisitor.GetResult()->IsAncestor(m_pMovingNode)) {
                    std::shared_ptr<touchmind::model::node::NodeModel> pNewParent = pHitNode;
                    std::shared_ptr<touchmind::model::node::NodeModel> pOldParent = m_pMovingNode->GetParent();
                    m_pCanvasPanel->GetNodeViewManager()->SyncFontAttributesFromTextLayout();
                    m_pCanvasPanel->GetTouchMindApp()->GetMapModel()->BeginTransaction();
                    pOldParent->RemoveChild(m_pMovingNode);
                    pNewParent->AddChild(m_pMovingNode);
                    m_pMovingNode->SetPosition(touchmind::NODE_SIDE_UNDEFINED);
                    m_pCanvasPanel->GetTouchMindApp()->GetMapModel()->NormalizeLinks();
                    m_pCanvasPanel->GetTouchMindApp()->GetMapModel()->EndTransaction();
                    m_pCanvasPanel->ArrangeNodes();
                }

                m_manipulationStatus = MANIPULATION_STATUS_NORMAL;
                m_pMovingNode.reset();
                SetMouseCursor(MOUSE_CURSOR_TYPE_DEFAULT);
            }
        } else if (!(pGestureInfo->dwFlags & GF_INERTIA)) {
            if (m_pMovingNode != nullptr) {
                m_panMovingPoint = pGestureInfo->ptsLocation;
                SetMouseCursor(MOUSE_CURSOR_TYPE_HAND);
            } else {
                m_manipulationStatus = MANIPULATION_STATUS_NORMAL;
                SetMouseCursor(MOUSE_CURSOR_TYPE_DEFAULT);
            }
        }
        isMoving = true;
    }

    if (!isMoving) {
        bool reachXBoundary = false;
        bool reachYBoundary = false;
        m_pCanvasPanel->GetScrollBarHelper()->MoveWindowDelta(static_cast < FLOAT > (-dx), static_cast < FLOAT > (-dy),
                &reachXBoundary, &reachYBoundary);
        m_pCanvasPanel->GetScrollBarHelper()->UpdateScrollInfoForPositionChange(m_pCanvasPanel->GetHwnd());
        if ((pGestureInfo->dwFlags & GF_BEGIN)) {
            BeginPanningFeedback(m_pCanvasPanel->GetHwnd());
        } else if ((pGestureInfo->dwFlags & GF_END)) {
            EndPanningFeedback(m_pCanvasPanel->GetHwnd(), TRUE);
        }
        if (reachXBoundary || reachYBoundary) {
            const static float rate = 0.1f;
            LONG lTotalOverpanOffsetX = static_cast < LONG > (reachXBoundary ? dx * rate : 0);
            LONG lTotalOverpanOffsetY = static_cast < LONG > (reachYBoundary ? dy * rate : 0);
            UpdatePanningFeedback(m_pCanvasPanel->GetHwnd(), lTotalOverpanOffsetX, lTotalOverpanOffsetY,
                                  pGestureInfo->dwFlags & GF_INERTIA);
        }
        SetMouseCursor(MOUSE_CURSOR_TYPE_SIZEALL);
    }
    ManipulationResult result(true, true);
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::ManipulationHelper::OnRotate(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    POINT windowPoint,
    ULONGLONG timestampInMilliSeconds,
    PGESTUREINFO pGestureInfo,
    POINT rotateCenterPoint,
    FLOAT rotateAngle)
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(windowPoint);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
    UNREFERENCED_PARAMETER(pGestureInfo);
    UNREFERENCED_PARAMETER(rotateCenterPoint);
    UNREFERENCED_PARAMETER(rotateAngle);

    ManipulationResult result(false, false);
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::ManipulationHelper::OnTwoFingerTap(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    POINT windowPoint,
    ULONGLONG timestampInMilliSeconds,
    PGESTUREINFO pGestureInfo)
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(windowPoint);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
    UNREFERENCED_PARAMETER(pGestureInfo);

    ManipulationResult result = OnDoubleClick(hWnd, wParam, lParam, windowPoint, timestampInMilliSeconds);
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::ManipulationHelper::OnPressAndTap(
    HWND hWnd,
    WPARAM wParam,
    LPARAM lParam,
    POINT windowPoint,
    ULONGLONG timestampInMilliSeconds,
    PGESTUREINFO pGestureInfo)
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);
    UNREFERENCED_PARAMETER(windowPoint);
    UNREFERENCED_PARAMETER(timestampInMilliSeconds);
    UNREFERENCED_PARAMETER(pGestureInfo);

    ManipulationResult result = OnRightSingleClick(hWnd, wParam, lParam, windowPoint, timestampInMilliSeconds);
    return result;
}
