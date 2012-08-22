#include "StdAfx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/touch/AbstractManipulationHelper.h"
#include "touchmind/util/Utils.h"

UINT_PTR touchmind::touch::AbstractManipulationHelper::m_masterTimerId = 1;

touchmind::touch::AbstractManipulationHelper::AbstractManipulationHelper(void) :
    m_draggingStatus(DRAGGING_STATUS_NOT_DRAGGING),
    m_timerIdForDoubleClick(m_masterTimerId++),
    m_hCursor(nullptr),
    m_hCursor_Arrow(nullptr),
    m_hCursor_SizeNWSE(nullptr),
    m_hCursor_SizeNESW(nullptr),
    m_hCursor_SizeWE(nullptr),
    m_hCursor_SizeNS(nullptr),
    m_hCursor_SizeALL(nullptr),
    m_hCursor_No(nullptr),
    m_hCursor_Hand(nullptr)
{
    m_lastPoint.x = -1;
    m_lastPoint.y = -1;
}

touchmind::touch::AbstractManipulationHelper::~AbstractManipulationHelper(void)
{
}

touchmind::touch::ManipulationResult touchmind::touch::AbstractManipulationHelper::OnLeftMouseDown(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    UNREFERENCED_PARAMETER(message);

    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    m_draggingStatus = DRAGGING_STATUS_PREPARE;
    m_lastPoint.x = GET_X_LPARAM(lParam);
    m_lastPoint.y = GET_Y_LPARAM(lParam);
    ::SetCapture(hWnd);

    POINT windowPoint;
    windowPoint.x = GET_X_LPARAM(lParam);
    windowPoint.y = GET_Y_LPARAM(lParam);
    return OnLeftMouseDownExt(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds);
}

touchmind::touch::ManipulationResult touchmind::touch::AbstractManipulationHelper::OnMouseMove(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    UNREFERENCED_PARAMETER(message);

    bool leftButtonDown = (wParam & MK_LBUTTON) != 0;

    ManipulationResult result;
    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    if (leftButtonDown && m_draggingStatus == DRAGGING_STATUS_PREPARE) {
        // start mouse dragging
        m_dragRect.left = m_lastPoint.x;
        m_dragRect.top = m_lastPoint.y;
        m_dragRect.right = GET_X_LPARAM(lParam);
        m_dragRect.bottom = GET_Y_LPARAM(lParam);
        LONG dx = 0;
        LONG dy = 0;
        if (m_lastPoint.x != -1 && m_lastPoint.y != -1) {
            dx = m_dragRect.right - m_lastPoint.x;
            dy = m_dragRect.bottom - m_lastPoint.y;
        }
        result = OnDraggingStart(hWnd, wParam, lParam, m_dragRect, dx, dy, currentTimestampInMilliSeconds);
        m_draggingStatus = DRAGGING_STATUS_DRAGGING;
    } else if (leftButtonDown && m_draggingStatus == DRAGGING_STATUS_DRAGGING) {
        // Mouse dragging
        m_dragRect.right = GET_X_LPARAM(lParam);
        m_dragRect.bottom = GET_Y_LPARAM(lParam);
        LONG dx = 0;
        LONG dy = 0;
        if (m_lastPoint.x != -1 && m_lastPoint.y != -1) {
            dx = m_dragRect.right - m_lastPoint.x;
            dy = m_dragRect.bottom - m_lastPoint.y;
        }
        result = OnDragging(hWnd, wParam, lParam, m_dragRect, dx, dy, currentTimestampInMilliSeconds);
    } else if (!leftButtonDown && m_draggingStatus == DRAGGING_STATUS_DRAGGING) {
        // mouse left button up but WM_LBUTTONUP event has not been received.
        // usually this situation does not happen
        m_dragRect.right = GET_X_LPARAM(lParam);
        m_dragRect.bottom = GET_Y_LPARAM(lParam);
        LONG dx = 0;
        LONG dy = 0;
        if (m_lastPoint.x != -1 && m_lastPoint.y != -1) {
            dx = m_dragRect.right - m_lastPoint.x;
            dy = m_dragRect.bottom - m_lastPoint.y;
        }
        result = OnDraggingEnd(hWnd, wParam, lParam, m_dragRect, dx, dy, currentTimestampInMilliSeconds);
        m_draggingStatus = DRAGGING_STATUS_NOT_DRAGGING;
    } else {
        // Mouse move
        POINT windowPoint;
        windowPoint.x = GET_X_LPARAM(lParam);
        windowPoint.y = GET_Y_LPARAM(lParam);
        LONG dx = 0;
        LONG dy = 0;
        if (m_lastPoint.x != -1 && m_lastPoint.y != -1) {
            dx = windowPoint.x - m_lastPoint.x;
            dy = windowPoint.y - m_lastPoint.y;
        }
        result = OnHovering(hWnd, wParam, lParam, windowPoint, dx, dy, currentTimestampInMilliSeconds);
        m_draggingStatus = DRAGGING_STATUS_NOT_DRAGGING;
    }
    m_lastPoint.x = GET_X_LPARAM(lParam);
    m_lastPoint.y = GET_Y_LPARAM(lParam);

    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::AbstractManipulationHelper::OnLeftMouseDoubleClick(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    UNREFERENCED_PARAMETER(message);

    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    m_draggingStatus = DRAGGING_STATUS_NOT_DRAGGING;
    // Left mouse button double click
    if (! ::KillTimer(hWnd, m_timerIdForDoubleClick)) {
        DWORD lastError = GetLastError();
        LOG(SEVERITY_LEVEL_ERROR) << util::LastError(util::LastErrorArgs(L"KillTimer", lastError));
        ManipulationResult result(false, false);
        return result;
    }
    POINT windowPoint;
    windowPoint.x = GET_X_LPARAM(lParam);
    windowPoint.y = GET_Y_LPARAM(lParam);
    return OnDoubleClick(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds);
}

touchmind::touch::ManipulationResult touchmind::touch::AbstractManipulationHelper::OnLeftMouseUp(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    UNREFERENCED_PARAMETER(message);

    ManipulationResult result;
    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    ::ReleaseCapture();
    POINT windowPoint;
    windowPoint.x = GET_X_LPARAM(lParam);
    windowPoint.y = GET_Y_LPARAM(lParam);
    if (IsDragging()) {
        // Mouse dragging end
        m_dragRect.right = windowPoint.x;
        m_dragRect.bottom = windowPoint.y;
        LONG dx = 0;
        LONG dy = 0;
        if (m_lastPoint.x != -1 && m_lastPoint.y != -1) {
            dx = m_dragRect.right - m_lastPoint.x;
            dy = m_dragRect.bottom - m_lastPoint.y;
        }
        result = OnDraggingEnd(hWnd, wParam, lParam, m_dragRect, dx, dy, currentTimestampInMilliSeconds);
    } else {
        m_lBtnUpPoint.x = windowPoint.x;
        m_lBtnUpPoint.y = windowPoint.y;
        UINT_PTR ret;
        if ((ret = ::SetTimer(hWnd, m_timerIdForDoubleClick, GetDoubleClickTime(), nullptr)) == 0) {
            DWORD lastError = GetLastError();
            LOG(SEVERITY_LEVEL_ERROR) << util::LastError(util::LastErrorArgs(L"SetTimer", lastError));
            ManipulationResult result(false, false);
            return result;
        }
        result.IsNeedInvalidateRect = true;
        result.WasHandled = true;
    }
    m_draggingStatus = DRAGGING_STATUS_NOT_DRAGGING;
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::AbstractManipulationHelper::OnTimer(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    UNREFERENCED_PARAMETER(message);

    ManipulationResult result;
    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    if (wParam == m_timerIdForDoubleClick) {
        if (! ::KillTimer(hWnd, m_timerIdForDoubleClick)) {
            DWORD lastError = GetLastError();
            LOG(SEVERITY_LEVEL_ERROR) << util::LastError(util::LastErrorArgs(L"KillTimer", lastError));
            ManipulationResult result(false, false);
            return result;
        }
        m_draggingStatus = DRAGGING_STATUS_NOT_DRAGGING;
        // Left mouse button single click
        result = OnSingleClick(hWnd, wParam, lParam, m_lBtnUpPoint, currentTimestampInMilliSeconds);
    } else {
        result.IsNeedInvalidateRect = true;
        result.WasHandled = true;
    }
    return result;
}

touchmind::touch::ManipulationResult touchmind::touch::AbstractManipulationHelper::OnTouch(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(message);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

    ManipulationResult result;
    result.IsNeedInvalidateRect = true;
    result.WasHandled = true;
    return result;
}


touchmind::touch::ManipulationResult touchmind::touch::AbstractManipulationHelper::OnRightMouseDown(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    UNREFERENCED_PARAMETER(message);

    ManipulationResult result;
    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    POINT windowPoint;
    windowPoint.x = GET_X_LPARAM(lParam);
    windowPoint.y = GET_Y_LPARAM(lParam);
    return OnRightSingleClick(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds);
}

bool touchmind::touch::AbstractManipulationHelper::IsDragging()
{
    return m_draggingStatus == DRAGGING_STATUS_DRAGGING;
}

ULONGLONG touchmind::touch::AbstractManipulationHelper::GetCurrentTimestampInMilliSeconds()
{
    SYSTEMTIME st;
    GetSystemTime(&st);
    FILETIME ft;
    SystemTimeToFileTime(&st, &ft);
    ULARGE_INTEGER ui;
    ui.HighPart = ft.dwHighDateTime;
    ui.LowPart = ft.dwLowDateTime;
    return ui.QuadPart / 10;
}

touchmind::touch::ManipulationResult touchmind::touch::AbstractManipulationHelper::OnGesture(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    UNREFERENCED_PARAMETER(message);
    ManipulationResult result;
    ULONGLONG currentTimestampInMilliSeconds = GetCurrentTimestampInMilliSeconds();
    GESTUREINFO gi = {0};
    gi.cbSize = sizeof(gi);
    BOOL bResult = GetGestureInfo((HGESTUREINFO)lParam, &gi);
    if (!bResult) {
        LOG(SEVERITY_LEVEL_ERROR) << L"GetGestureInfo failed, err=" << GetLastError();
        result.IsNeedInvalidateRect = false;
        result.WasHandled = false;
        return result;
    }
    POINT windowPoint;
    windowPoint.x = gi.ptsLocation.x;
    windowPoint.y = gi.ptsLocation.y;
    ScreenToClient(hWnd, &windowPoint);

    switch (gi.dwID) {
    case GID_BEGIN:
        result = OnGestureBegin(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi);
        break;
    case GID_END:
        result = OnGestureEnd(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi);
        break;
    case GID_ZOOM:
        switch (gi.dwFlags) {
        case GF_BEGIN:
            m_gestureArgument = LODWORD(gi.ullArguments);
            m_lastPoint = windowPoint;
            result = OnZoom(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi, windowPoint, 0.0f);
            break;
        default:
            POINT zoomCenterPoint;
            zoomCenterPoint.x = (m_lastPoint.x + windowPoint.x) / 2;
            zoomCenterPoint.y = (m_lastPoint.y + windowPoint.y) / 2;
            FLOAT zoomFactor = static_cast<FLOAT>(LODWORD(gi.ullArguments)) / static_cast<FLOAT>(m_gestureArgument);
            result = OnZoom(
                         hWnd,
                         wParam,
                         lParam,
                         windowPoint,
                         currentTimestampInMilliSeconds,
                         &gi,
                         zoomCenterPoint,
                         zoomFactor);
            m_gestureArgument = LODWORD(gi.ullArguments);
            m_lastPoint = windowPoint;
            break;
        }
        break;
    case GID_PAN:
        switch (gi.dwFlags) {
        case GF_BEGIN:
            m_lastPoint = windowPoint;
            result = OnPan(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi, 0, 0);
            break;
        default:
            result = OnPan(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi,
                           windowPoint.x - m_lastPoint.x,
                           windowPoint.y - m_lastPoint.y);
            m_lastPoint = windowPoint;
            break;
        }
        break;
    case GID_ROTATE:
        switch (gi.dwFlags) {
        case GF_BEGIN:
            m_lastPoint = windowPoint;
            m_gestureArgument = 0;
            result = OnRotate(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi, windowPoint, 0.0f);
            break;
        default:
            FLOAT rotateAngle =
                static_cast<FLOAT>(
                    GID_ROTATE_ANGLE_FROM_ARGUMENT(LODWORD(gi.ullArguments)) - GID_ROTATE_ANGLE_FROM_ARGUMENT(m_gestureArgument));
            result = OnRotate(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi, windowPoint, rotateAngle);
            m_gestureArgument = LODWORD(gi.ullArguments);
            break;
        }
        break;
    case GID_TWOFINGERTAP:
        result = OnTwoFingerTap(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi);
        break;
    case GID_PRESSANDTAP:
        result = OnPressAndTap(hWnd, wParam, lParam, windowPoint, currentTimestampInMilliSeconds, &gi);
        break;
    }

    CloseGestureInfoHandle((HGESTUREINFO)lParam);
    return result;
}

void touchmind::touch::AbstractManipulationHelper::SetMouseCursor( MOUSE_CURSOR_TYPE mouseCursorType )
{
    switch (mouseCursorType) {
    case MOUSE_CURSOR_TYPE_ARROW:
        m_hCursor = LoadCursor(nullptr, IDC_ARROW);
        break;
    case MOUSE_CURSOR_TYPE_SIZENWSE:
        m_hCursor = LoadCursor(nullptr, IDC_SIZENWSE);
        break;
    case MOUSE_CURSOR_TYPE_SIZENESW:
        m_hCursor = LoadCursor(nullptr, IDC_SIZENESW);
        break;
    case MOUSE_CURSOR_TYPE_SIZEWE:
        m_hCursor = LoadCursor(nullptr, IDC_SIZEWE);
        break;
    case MOUSE_CURSOR_TYPE_SIZENS:
        m_hCursor = LoadCursor(nullptr, IDC_SIZENS);
        break;
    case MOUSE_CURSOR_TYPE_SIZEALL:
        m_hCursor = LoadCursor(nullptr, IDC_SIZEALL);
        break;
    case MOUSE_CURSOR_TYPE_NO:
        m_hCursor = LoadCursor(nullptr, IDC_NO);
        break;
    case MOUSE_CURSOR_TYPE_HAND:
        m_hCursor = LoadCursor(nullptr, IDC_HAND);
        break;
    case MOUSE_CURSOR_TYPE_DEFAULT:
    default:
        m_hCursor = LoadCursor(nullptr, IDC_ARROW);
        break;
    }
    if (m_hCursor != nullptr) {
        SetCursor(m_hCursor);
    }
}

touchmind::touch::ManipulationResult touchmind::touch::AbstractManipulationHelper::OnSetCursor(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    UNREFERENCED_PARAMETER(hWnd);
    UNREFERENCED_PARAMETER(message);
    UNREFERENCED_PARAMETER(wParam);
    UNREFERENCED_PARAMETER(lParam);

    SetCursor(m_hCursor);
    ManipulationResult result;
    result.IsNeedInvalidateRect = true;
    result.WasHandled = true;
    return result;
}

std::wostream& operator<<( std::wostream& strm, const GESTUREINFO &gi )
{
    wchar_t *s_id = nullptr;
    switch (gi.dwID) {
    case GID_BEGIN:
        s_id = L"GID_BEGIN";
        break;
    case GID_END:
        s_id = L"GID_END";
        break;
    case GID_ZOOM:
        s_id = L"GID_ZOOM";
        break;
    case GID_PAN:
        s_id = L"GID_PAN";
        break;
    case GID_ROTATE:
        s_id = L"GID_ROTATE";
        break;
    case GID_TWOFINGERTAP:
        s_id = L"GID_TWOFINGERTAP";
        break;
    case GID_PRESSANDTAP:
        s_id = L"GID_PRESSANDTAP";
        break;
    default:
        wchar_t buf[1024];
        wsprintf(buf, L"UNKNOWN(%d)", gi.dwID);
    }

    std::wstring s_flags;
    if ((gi.dwFlags & GF_BEGIN) != 0) {
        s_flags += L"GF_BEGIN ";
    }
    if ((gi.dwFlags & GF_INERTIA) != 0) {
        s_flags += L"GF_INERTIA ";
    }
    if ((gi.dwFlags & GF_END) != 0) {
        s_flags += L"GF_END ";
    }

    strm << L"GESTUREINFO(ID=" << s_id
         << L",Flags=" << gi.dwFlags << L"[" << s_flags << L"]"
         << L",Location=(" << gi.ptsLocation.x << L"," << gi.ptsLocation.y << L"))";
    return strm;
}

std::wostream& operator<<( std::wostream& strm, const GESTURECONFIG &gi )
{
    wchar_t *s_id = nullptr;
    switch (gi.dwID) {
    case GID_BEGIN:
        s_id = L"GID_BEGIN";
        break;
    case GID_END:
        s_id = L"GID_END";
        break;
    case GID_ZOOM:
        s_id = L"GID_ZOOM";
        break;
    case GID_PAN:
        s_id = L"GID_PAN";
        break;
    case GID_ROTATE:
        s_id = L"GID_ROTATE";
        break;
    case GID_TWOFINGERTAP:
        s_id = L"GID_TWOFINGERTAP";
        break;
    case GID_PRESSANDTAP:
        s_id = L"GID_PRESSANDTAP";
        break;
    default:
        wchar_t buf[1024];
        wsprintf(buf, L"UNKNOWN(%d)", gi.dwID);
    }
    strm << L"GESTURECONFIG(ID=" << s_id
         << L",Want=" << gi.dwWant
         << L",Block=" << gi.dwBlock;
    return strm;
}
