#include "stdafx.h"
#include "touchmind/prop/LineStyle.h"

std::wostream& touchmind::operator<<(std::wostream& os, const touchmind::LINE_STYLE &lineStyle)
{
    switch (lineStyle) {
    case LINE_STYLE_UNSPECIFIED:
        os << L"UNSPECIFIED";
        break;
    case LINE_STYLE_SOLID:
        os << L"SOLID";
        break;
    case LINE_STYLE_DASHED:
        os << L"DASHED";
        break;
    case LINE_STYLE_DOTTED:
        os << L"DOTTED";
        break;
    }
    return os;
}
