#ifndef TOUCHMIND_TOUCH_ABSTRACTMANIPULATIONHELPER_H_
#define TOUCHMIND_TOUCH_ABSTRACTMANIPULATIONHELPER_H_

namespace touchmind
{
namespace touch
{

enum DRAGGING_STATUS {
    DRAGGING_STATUS_NOT_DRAGGING,
    DRAGGING_STATUS_PREPARE,
    DRAGGING_STATUS_DRAGGING
};

enum MOUSE_CURSOR_TYPE {
    MOUSE_CURSOR_TYPE_DEFAULT,
    MOUSE_CURSOR_TYPE_ARROW,
    MOUSE_CURSOR_TYPE_SIZENWSE,
    MOUSE_CURSOR_TYPE_SIZENESW,
    MOUSE_CURSOR_TYPE_SIZEWE,
    MOUSE_CURSOR_TYPE_SIZENS,
    MOUSE_CURSOR_TYPE_SIZEALL,
    MOUSE_CURSOR_TYPE_NO,
    MOUSE_CURSOR_TYPE_HAND
};

class ManipulationResult
{
public:
    ManipulationResult() :
        WasHandled(false),
        IsNeedInvalidateRect(false) {}
    ManipulationResult(bool _WasHandled, bool _IsNeedInvalidateRect) :
        WasHandled(_WasHandled),
        IsNeedInvalidateRect(_IsNeedInvalidateRect) {}
    bool WasHandled;
    bool IsNeedInvalidateRect;
};

class AbstractManipulationHelper
{
private:
    static UINT_PTR m_masterTimerId;
    POINT m_lastPoint;
    DRAGGING_STATUS m_draggingStatus;
    RECT m_dragRect;
    POINT m_lBtnUpPoint;
    UINT_PTR m_timerIdForDoubleClick;
    DWORD m_gestureArgument;
    HCURSOR m_hCursor;
    HCURSOR m_hCursor_Arrow;
    HCURSOR m_hCursor_SizeNWSE;
    HCURSOR m_hCursor_SizeNESW;
    HCURSOR m_hCursor_SizeWE;
    HCURSOR m_hCursor_SizeNS;
    HCURSOR m_hCursor_SizeALL;
    HCURSOR m_hCursor_No;
    HCURSOR m_hCursor_Hand;

public:
    AbstractManipulationHelper(void);
    virtual ~AbstractManipulationHelper(void);

    void SetMouseCursor(MOUSE_CURSOR_TYPE mouseCursorType);

    virtual ULONGLONG GetCurrentTimestampInMilliSeconds();

    virtual ManipulationResult OnLeftMouseDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult OnMouseMove(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult OnLeftMouseUp(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult OnLeftMouseDoubleClick(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult OnRightMouseDown(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult OnTimer(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult OnTouch(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult OnGesture(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    virtual ManipulationResult OnSetCursor(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    virtual bool IsDragging();

    virtual ManipulationResult OnLeftMouseDownExt(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint, ULONGLONG timestampInMilliSeconds) = 0;
    virtual ManipulationResult OnSingleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint, ULONGLONG timestampInMilliSeconds) = 0;
    virtual ManipulationResult OnDoubleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint, ULONGLONG timestampInMilliSeconds) = 0;
    virtual ManipulationResult OnDraggingStart(HWND hWnd, WPARAM wParam, LPARAM lParam, RECT windowDragRect, LONG dx, LONG dy, ULONGLONG timestampInMilliSeconds) = 0;
    virtual ManipulationResult OnDragging(HWND hWnd, WPARAM wParam, LPARAM lParam, RECT windowDragRect, LONG dx, LONG dy, ULONGLONG timestampInMilliSeconds) = 0;
    virtual ManipulationResult OnDraggingEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, RECT windowDragRect, LONG dx, LONG dy, ULONGLONG timestampInMilliSeconds) = 0;
    virtual ManipulationResult OnHovering(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint, LONG dx, LONG dy, ULONGLONG timestampInMilliSeconds) = 0;
    virtual ManipulationResult OnRightSingleClick(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint, ULONGLONG timestampInMilliSeconds) = 0;
    virtual ManipulationResult OnGestureBegin(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint, ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo) = 0;
    virtual ManipulationResult OnGestureEnd(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint, ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo) = 0;
    virtual ManipulationResult OnZoom(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint, ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo, POINT zoomCenterPoint, FLOAT zoomFactor) = 0;
    virtual ManipulationResult OnPan(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint, ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo, LONG dx, LONG dy) = 0;
    virtual ManipulationResult OnRotate(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint, ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo, POINT rotateCenterPoint, FLOAT rotateAngle) = 0;
    virtual ManipulationResult OnTwoFingerTap(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint, ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo) = 0;
    virtual ManipulationResult OnPressAndTap(HWND hWnd, WPARAM wParam, LPARAM lParam, POINT windowPoint, ULONGLONG timestampInMilliSeconds, PGESTUREINFO pGestureInfo) = 0;
};

} // touch
} // touchmind

std::wostream& operator<< (std::wostream& strm, const GESTUREINFO &gi);
std::wostream& operator<< (std::wostream& strm, const GESTURECONFIG &gi);

#endif // TOUCHMIND_TOUCH_ABSTRACTMANIPULATIONHELPER_H_