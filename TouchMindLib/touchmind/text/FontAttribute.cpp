#include "stdafx.h"
#include "touchmind/Common.h"
#include "FontAttribute.h"

std::wostream& touchmind::text::operator<< (std::wostream& os, const touchmind::text::FontAttribute &fontAttribute)
{
    std::wstring sForegroundColor;
    touchmind::ColorrefToString(fontAttribute.foregroundColor, sForegroundColor);
    os << L"FontAttribute[startPosition=" << fontAttribute.startPosition
       << L",length=" << fontAttribute.length
       << L",fontFamilyName=" << fontAttribute.fontFamilyName
       << L",localeName=" << fontAttribute.localeName
       << L",fontSize=" << fontAttribute.fontSize
       << L",bold=" << std::boolalpha << fontAttribute.bold
       << L",italic=" << std::boolalpha << fontAttribute.italic
       << L",underline=" << std::boolalpha << fontAttribute.underline
       << L",strikethrough=" << std::boolalpha << fontAttribute.strikethrough
       << L",foregroundColor=" << sForegroundColor
       << L"]";
    return os;
}
