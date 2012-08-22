#include "StdAfx.h"
#include "touchmind/util/ColorUtil.h"

std::wostream& operator<<( std::wostream& os, const D2D1_COLOR_F &color )
{
    os << L"D2D1_COLOR_F(" << color.r << L"," << color.g << L"," << color.b << L"," << color.a << L")";
    return os;
}
