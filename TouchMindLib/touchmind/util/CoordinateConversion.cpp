#include "StdAfx.h"
#include "touchmind/util/ScrollBarHelper.h"
#include "touchmind/util/CoordinateConversion.h"

touchmind::util::CoordinateConversion::CoordinateConversion(void)
{
}

touchmind::util::CoordinateConversion::~CoordinateConversion(void)
{
}

void touchmind::util::CoordinateConversion::ConvertScreenToModelCoordinate(
    HWND hWnd,
    touchmind::util::ScrollBarHelper* pScrollBarHelper,
    POINT point,
    D2D1_POINT_2F* clientPoint)
{
    RECT clientRect;
    ::GetClientRect(hWnd, &clientRect);
    ::ScreenToClient(hWnd, &point);
    INT clientWidth = clientRect.right - clientRect.left + 1;
    INT clientHeight = clientRect.bottom - clientRect.top + 1;
    clientPoint->x = point.x - (clientWidth / 2.0f + pScrollBarHelper->GetTransformX());
    clientPoint->y = point.y - (clientHeight / 2.0f + pScrollBarHelper->GetTransformY());
}

void touchmind::util::CoordinateConversion::ConvertScreenToModelCoordinate(
    HWND hWnd,
    touchmind::util::ScrollBarHelper *pScrollBarHelper,
    LONG x,
    LONG y,
    D2D1_POINT_2F *modelPoint)
{
    POINT p;
    p.x = x;
    p.y = y;
    ConvertScreenToModelCoordinate(hWnd, pScrollBarHelper, p, modelPoint);
}

void touchmind::util::CoordinateConversion::ConvertWindowToModelCoordinate(
    HWND hWnd,
    touchmind::util::ScrollBarHelper* pScrollBarHelper,
    POINT point,
    D2D1_POINT_2F* clientPoint)
{
    RECT clientRect;
    ::GetClientRect(hWnd, &clientRect);
    INT clientWidth = clientRect.right - clientRect.left + 1;
    INT clientHeight = clientRect.bottom - clientRect.top + 1;
    clientPoint->x = point.x - (clientWidth / 2.0f + pScrollBarHelper->GetTransformX());
    clientPoint->y = point.y - (clientHeight / 2.0f + pScrollBarHelper->GetTransformY());
}

void touchmind::util::CoordinateConversion::ConvertWindowToModelCoordinate(
    HWND hWnd,
    touchmind::util::ScrollBarHelper* pScrollBarHelper,
    RECT rect,
    D2D1_RECT_F* clientRect)
{
    POINT windowPoint;
    D2D1_POINT_2F clientPoint;

    windowPoint.x = rect.left;
    windowPoint.y = rect.top;
    ConvertWindowToModelCoordinate(hWnd, pScrollBarHelper, windowPoint, &clientPoint);
    clientRect->left = clientPoint.x;
    clientRect->top = clientPoint.y;

    windowPoint.x = rect.right;
    windowPoint.y = rect.bottom;
    ConvertWindowToModelCoordinate(hWnd, pScrollBarHelper, windowPoint, &clientPoint);
    clientRect->right = clientPoint.x;
    clientRect->bottom = clientPoint.y;
}

void touchmind::util::CoordinateConversion::ConvertWindowToModelCoordinate(
    HWND hWnd,
    touchmind::util::ScrollBarHelper *pScrollBarHelper,
    LONG x,
    LONG y,
    D2D1_POINT_2F *modelPoint)
{
    POINT p;
    p.x = x;
    p.y = y;
    ConvertWindowToModelCoordinate(hWnd, pScrollBarHelper, p, modelPoint);
}

void touchmind::util::CoordinateConversion::ConvertModelToWindowCoordinate(
    HWND hWnd,
    touchmind::util::ScrollBarHelper* pScrollBarHelper,
    D2D1_POINT_2F clientPoint,
    POINT* point)
{
    RECT clientRect;
    ::GetClientRect(hWnd, &clientRect);
    INT clientWidth = clientRect.right - clientRect.left + 1;
    INT clientHeight = clientRect.bottom - clientRect.top + 1;
    point->x = static_cast<LONG>(clientPoint.x + (clientWidth / 2.0f + pScrollBarHelper->GetTransformX()));
    point->y = static_cast<LONG>(clientPoint.y + (clientHeight / 2.0f + pScrollBarHelper->GetTransformY()));
}
