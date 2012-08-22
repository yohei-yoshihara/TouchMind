#ifndef TOUCHMIND_COORDINATECONVERSION_H_
#define TOUCHMIND_COORDINATECONVERSION_H_

#include "forwarddecl.h"

namespace touchmind
{
namespace util
{

class CoordinateConversion
{
private:
    CoordinateConversion(void);
    ~CoordinateConversion(void);
public:
    static void ConvertScreenToModelCoordinate(
        HWND hWnd,
        touchmind::util::ScrollBarHelper *pScrollBarHelper,
        POINT point,
        OUT D2D1_POINT_2F *modelPoint);
    static void ConvertScreenToModelCoordinate(
        HWND hWnd,
        touchmind::util::ScrollBarHelper *pScrollBarHelper,
        LONG x,
        LONG y,
        OUT D2D1_POINT_2F *modelPoint);
    static void ConvertWindowToModelCoordinate(
        HWND hWnd,
        touchmind::util::ScrollBarHelper *pScrollBarHelper,
        POINT point,
        OUT D2D1_POINT_2F *modelPoint);
    static void ConvertWindowToModelCoordinate(
        HWND hWnd,
        touchmind::util::ScrollBarHelper *pScrollBarHelper,
        LONG x,
        LONG y,
        OUT D2D1_POINT_2F *modelPoint);
    static void ConvertWindowToModelCoordinate(
        HWND hWnd,
        touchmind::util::ScrollBarHelper *pScrollBarHelper,
        RECT rect,
        OUT D2D1_RECT_F *modelRect);
    static void ConvertModelToWindowCoordinate(
        HWND hWnd,
        touchmind::util::ScrollBarHelper *pScrollBarHelper,
        D2D1_POINT_2F modelPoint,
        OUT POINT *point);
};

} // util
} // touchmind

#endif // TOUCHMIND_COORDINATECONVERSION_H_
