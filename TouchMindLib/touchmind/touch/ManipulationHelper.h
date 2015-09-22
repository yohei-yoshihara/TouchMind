#ifndef TOUCHMIND_TOUCH_MANIPULATIONHELPER_H_
#define TOUCHMIND_TOUCH_MANIPULATIONHELPER_H_

#include "forwarddecl.h"
#include "touchmind/touch/AbstractManipulationHelper.h"

namespace touchmind {
  namespace touch {

    enum MANIPULATION_STATUS {
      MANIPULATION_STATUS_NORMAL,
      MANIPULATION_STATUS_MOVING_NODE,
      MANIPULATION_STATUS_MOVING_NODE_WITH_PAN_GESTURE,
      MANIPULATION_STATUS_RESIZING_NODE,
      MANIPULATION_STATUS_RESIZING_NODE_WITH_ZOOM_GESTURE,
      MANIPULATION_STATUS_SCROLLING,
      MANIPULATION_STATUS_SCROLLING_WITH_PAN_GESTURE
    };

    class ManipulationHelper : public touchmind::touch::AbstractManipulationHelper {
    private:
      touchmind::win::CanvasPanel *m_pCanvasPanel;
      MANIPULATION_STATUS m_manipulationStatus;
      // dragging node
      std::shared_ptr<touchmind::model::node::NodeModel> m_pMovingNode;
      // resizing node
      std::shared_ptr<touchmind::model::node::NodeModel> m_pResizingNode;
      // size of node before resizing
      D2D1_RECT_F m_rectBeforeResize;
      // preferred size of the resizing node
      D2D1_RECT_F m_rectOnResize;
      // scroll start client point
      POINT m_scrollStartPointInWindow;
      FLOAT m_hitTestMargin;
      POINTS m_panMovingPoint;

    public:
      ManipulationHelper();
      virtual ~ManipulationHelper();
      virtual void SetCanvasPanel(touchmind::win::CanvasPanel *pCanvasPanel) {
        m_pCanvasPanel = pCanvasPanel;
      }
      virtual ManipulationResult OnLeftMouseDownExt(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                    ULONGLONG timestampInMilliSeconds);
      virtual ManipulationResult OnSingleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                               ULONGLONG timestampInMilliSeconds);
      virtual ManipulationResult OnDoubleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                               ULONGLONG timestampInMilliSeconds);
      virtual ManipulationResult OnDraggingStart(HWND hWnd, WPARAM wParam, LPARAM lParam, RECT windowDragRect, LONG dx,
                                                 LONG dy, ULONGLONG timestampInMilliSeconds);
      virtual ManipulationResult OnDragging(HWND hWnd, WPARAM wParam, LPARAM lParam, RECT windowDragRect, LONG dx,
                                            LONG dy, ULONGLONG timestampInMilliSeconds);
      virtual ManipulationResult OnDraggingEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, RECT windowDragRect, LONG dx,
                                               LONG dy, ULONGLONG timestampInMilliSeconds);
      virtual ManipulationResult OnHovering(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint, LONG dx,
                                            LONG dy, ULONGLONG timestampInMilliSeconds);
      virtual ManipulationResult OnRightSingleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                    ULONGLONG timestampInMilliSeconds);
      virtual ManipulationResult OnGestureBegin(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo);
      virtual ManipulationResult OnGestureEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                              ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo);
      virtual ManipulationResult OnZoom(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                        ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo,
                                        POINT zoomCenterPoint, FLOAT zoomFactor);
      virtual ManipulationResult OnPan(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                       ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo, LONG dx, LONG dy);
      virtual ManipulationResult OnRotate(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                          ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo,
                                          POINT rotateCenterPoint, FLOAT rotateAngle);
      virtual ManipulationResult OnTwoFingerTap(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                                ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo);
      virtual ManipulationResult OnPressAndTap(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint,
                                               ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo);
      MANIPULATION_STATUS GetManipulationStatus() const {
        return m_manipulationStatus;
      }
      std::shared_ptr<touchmind::model::node::NodeModel> GetMovingNode() const {
        return m_pMovingNode;
      }
      bool IsResizing() const {
        return (m_manipulationStatus == MANIPULATION_STATUS_RESIZING_NODE
                || m_manipulationStatus == MANIPULATION_STATUS_RESIZING_NODE_WITH_ZOOM_GESTURE)
               && m_pResizingNode != nullptr;
      }
      std::shared_ptr<touchmind::model::node::NodeModel> GetResizingNode() const {
        return m_pResizingNode;
      }
      D2D1_RECT_F GetResizingNodeRect() const {
        return m_rectOnResize;
      }
      void SetHitTestMargin(FLOAT hitTestMargin) {
        m_hitTestMargin = hitTestMargin;
      }
      FLOAT GetHitTestMargin() const {
        return m_hitTestMargin;
      }
      const POINTS &GetPanMovingPoint() const {
        return m_panMovingPoint;
      }
    };

  } // touch
} // touchmind

#endif // TOUCHMIND_TOUCH_MANIPULATIONHELPER_H_