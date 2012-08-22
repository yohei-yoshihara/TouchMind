#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/util/ColorUtil.h"

const touchmind::REPAINT_COUNTER touchmind::MODEL_INITIAL_REPAINT_COUNTER = 0;
const touchmind::REPAINT_COUNTER touchmind::VIEW_INITIAL_REPAINT_COUNTER = UINT64_MAX / 2;
const touchmind::SAVE_COUNTER touchmind::MODEL_INITIAL_SAVE_COUNTER = 0;
const touchmind::SAVE_COUNTER touchmind::OPERATION_INITIAL_SAVE_COUNTER = UINT64_MAX / 2;

const double touchmind::PI_DOUBLE = 3.14159265358979323846;
const float  touchmind::PI = static_cast<FLOAT>(touchmind::PI_DOUBLE);
const float  touchmind::PI_2 = static_cast<FLOAT>(touchmind::PI_DOUBLE / 2.0);
const float  touchmind::PI_4 = static_cast<FLOAT>(touchmind::PI_DOUBLE / 4.0);
const double touchmind::SQRT_2_DOUBLE = 1.41421356237309504880;
const float  touchmind::SQRT_2 = static_cast<FLOAT>(touchmind::SQRT_2_DOUBLE);

errno_t touchmind::NodeIdToString(touchmind::NODE_ID nodeId, std::wstring &s)
{
    wchar_t buf[1024];
    errno_t err = _ltow_s(nodeId, buf, 1024, 10);
    if (err == 0) {
        s = buf;
    }
    return err;
}

errno_t touchmind::StringToNodeId(IN std::wstring &s, OUT touchmind::NODE_ID *pNodeId)
{
    *pNodeId = _wtol(s.c_str());
    return 0;
}

errno_t touchmind::SystemtimeToString(IN const SYSTEMTIME *pSystemTime, OUT std::wstring &s)
{
    wchar_t buf[1024];
    swprintf_s(buf, L"%04u%02u%02u%02u%02u%02u%03u",
               pSystemTime->wYear,
               pSystemTime->wMonth,
               pSystemTime->wDay,
               pSystemTime->wHour,
               pSystemTime->wMinute,
               pSystemTime->wSecond,
               pSystemTime->wMilliseconds);
    s = buf;
    return 0;
}

errno_t touchmind::StringToSystemtime(IN std::wstring &s, OUT SYSTEMTIME *pSystemTime)
{
    unsigned int year, month, day, hour, minute, second, milliseconds;
    swscanf_s(s.c_str(), L"%4u%2u%2u%2u%2u%2u%3u",
              &year,
              &month,
              &day,
              &hour,
              &minute,
              &second,
              &milliseconds);
    pSystemTime->wYear = static_cast<WORD>(year);
    pSystemTime->wMonth = static_cast<WORD>(month);
    pSystemTime->wDay = static_cast<WORD>(day);
    pSystemTime->wHour = static_cast<WORD>(hour);
    pSystemTime->wMinute = static_cast<WORD>(minute);
    pSystemTime->wSecond = static_cast<WORD>(second);
    pSystemTime->wMilliseconds = static_cast<WORD>(milliseconds);
    return 0;
}

errno_t touchmind::FontSizeToString(IN FLOAT fontSize, OUT std::wstring &s)
{
    wchar_t buf[1024];
    swprintf_s(buf, L"%3.1f", fontSize);
    s = buf;
    return 0;
}

errno_t touchmind::StringToFontSize(IN std::wstring &s, OUT FLOAT *pFontSize)
{
    swscanf_s(s.c_str(), L"%f", pFontSize);
    return 0;
}

errno_t touchmind::ColorrefToString(IN COLORREF colorref, OUT std::wstring &s)
{
    wchar_t buf[1024];
    swprintf_s(buf, L"0x%02x%02x%02x", GetRValue(colorref), GetGValue(colorref), GetBValue(colorref));
    s = buf;
    return 0;
}

errno_t touchmind::StringToColorref(IN std::wstring &s, OUT COLORREF *pColorref)
{
    int r, g, b;
    swscanf_s(s.c_str(), L"0x%02x%02x%02x", &r, &g, &b);
    *pColorref = RGB(r, g, b);
    return 0;
}

errno_t touchmind::ColorFToString( IN D2D1_COLOR_F colorf, OUT std::wstring &s )
{
    COLORREF colorref = util::ColorUtil::ToColorref(colorf);
    return ColorrefToString(colorref, s);
}

extern errno_t touchmind::StringToColorF( IN std::wstring &s, OUT D2D1_COLOR_F *colorf )
{
    COLORREF colorref;
    errno_t e = StringToColorref(s, &colorref);
    *colorf = util::ColorUtil::ToColorF(colorref);
    return e;
}

errno_t touchmind::SizeToString(IN FLOAT size, OUT std::wstring &s)
{
    wchar_t buf[1024];
    swprintf_s(buf, L"%f", size);
    s = buf;
    return 0;
}

errno_t touchmind::StringToSize(IN std::wstring &s, OUT FLOAT *pSize)
{
    swscanf_s(s.c_str(), L"%f", pSize);
    return 0;
}

errno_t touchmind::ColorrefToColorF(IN COLORREF colorref, OUT D2D1_COLOR_F *colorf)
{
    *colorf = D2D1::ColorF(GetRValue(colorref) / 255.0f, GetGValue(colorref) / 255.0f, GetBValue(colorref) / 255.0f, 1.0f);
    return 0;
}

errno_t touchmind::ColorFToColorref(IN D2D1_COLOR_F &colorF, OUT COLORREF *colorref)
{
    *colorref = RGB(colorF.r * 255, colorF.g * 255, colorF.b * 255);
    return 0;
}

// r, g, b : 0.0 - 1.0
// h : 0.0 - 360.0
// s, v : 0.0 - 1.0
errno_t touchmind::RgbToHsv(IN FLOAT r, IN FLOAT g, IN FLOAT b, OUT FLOAT &h, OUT FLOAT &s, OUT FLOAT &v)
{
    FLOAT _max = v = std::max(std::max(r, g), b);
    FLOAT _min  = std::min(std::min(r, g), b);
    s = _max != 0.0f ? ( _max - _min ) / _max : 0.0f; // if _max == 0.0f, s is undefined.
    if (_max == r) {
        h = (_max - _min != 0.0f) ? 60.0f * (g - b) / (_max - _min) : 0.0f; // if _max == _min, h is undefined.
    } else if (_max == g) {
        h = (_max - _min != 0.0f) ? 60.0f * (b - r) / (_max - _min) + 120.0f : 0.0f;
    } else {
        h = (_max - _min != 0.0f) ? 60.0f * (r - g) / (_max - _min) + 240.0f : 0.0f;
    }
    if (h < 0.0f) {
        h += 360.0f;
    }
    return 0;
}

errno_t touchmind::HsvToRgb(IN FLOAT h, IN FLOAT s, IN FLOAT v, OUT FLOAT &r, OUT FLOAT &g, OUT FLOAT &b)
{
    int hi = static_cast<int>( h / 60.0f ) % 6;
    FLOAT f = ( h / 60.0f ) - hi;
    FLOAT p = v * (1.0f - s);
    FLOAT q = v * (1.0f - f * s);
    FLOAT t = v * (1.0f - (1.0f - f) * s);
    switch ( hi ) {
    case 0:
        r = v;
        g = t;
        b = p;
        break;
    case 1:
        r = q;
        g = v;
        b = p;
        break;
    case 2:
        r = p;
        g = v;
        b = t;
        break;
    case 3:
        r = p;
        g = q;
        b = v;
        break;
    case 4:
        r = t;
        g = p;
        b = v;
        break;
    case 5:
        r = v;
        g = p;
        b = q;
        break;
    }
    return 0;
}

size_t touchmind::CountLeftSpaces(const std::wstring &line)
{
    size_t numberOfSpaces = 0;
    for (auto it = line.cbegin(); it != line.cend(); ++it) {
        if (*it == L' ') {
            ++numberOfSpaces;
        } else if (*it == L'\t') {
            numberOfSpaces += 8;
        } else {
            break;
        }
    }
    return numberOfSpaces;
}

void touchmind::TrimLeft(const std::wstring& line, std::wstring& text)
{
    size_t numberOfSpaces = 0;
    for (auto it = line.cbegin(); it != line.cend(); ++it) {
        if (*it == L' ' || *it == L'\t') {
            ++numberOfSpaces;
        } else {
            break;
        }
    }
    text = line.substr(numberOfSpaces);
}

void touchmind::Trim(const std::wstring& line, std::wstring& text)
{
    size_t leftSpaces = 0;
    for (auto it = line.cbegin(); it != line.cend(); ++it) {
        if (*it == L' ' || *it == L'\t') {
            ++leftSpaces;
        } else {
            break;
        }
    }
    std::wstring temp(line.substr(leftSpaces));
    if (temp.length() == 0) {
        return;
    }
    std::wstring::size_type rightSpaces = 0;
    for (auto it = temp.crbegin(); it != temp.crend(); ++it) {
        if (*it == L' ' || *it == L'\t' || *it == L'\n' || *it == L'\r') {
            ++rightSpaces;
        } else {
            break;
        }
    }
    text = temp.substr(0, temp.length() - rightSpaces);
}

size_t touchmind::LengthInUTF8(const wchar_t* ucs2)
{
    return ::WideCharToMultiByte(CP_UTF8, 0, ucs2, -1, nullptr, 0, nullptr, nullptr);
}

int touchmind::UCS2ToUTF8(const wchar_t* ucs2, char* utf8, size_t n)
{
    if (n < 1) {
        // too short
        return 0;
    }
    size_t ucs2Len = wcslen(ucs2);
    size_t utf8Len = LengthInUTF8(ucs2);
    if (n < utf8Len) {
        return 0;
    }
    int ret = ::WideCharToMultiByte(CP_UTF8, 0, ucs2, static_cast<int>(ucs2Len), utf8, static_cast<int>(n), nullptr, nullptr);
    if (ret != 0) {
        utf8[ret] = '\0';
    }
    return ret;
}

size_t touchmind::LengthInUCS2(const char* utf8)
{
    return ::MultiByteToWideChar(CP_UTF8, 0, utf8, -1, nullptr, 0);
}

int touchmind::UTF8ToUCS2(const char* utf8, wchar_t* ucs2, size_t n)
{
    if (n < 1) {
        // too short
        return 0;
    }
    size_t utf8Len = strlen(utf8);
    size_t ucs2Len = LengthInUCS2(utf8);
    if (n < ucs2Len) {
        ucs2[0] = '\0';
        return 0;
    }
    int ret = ::MultiByteToWideChar(CP_UTF8, 0, utf8, static_cast<int>(utf8Len), ucs2, static_cast<int>(n));
    if (ret != 0) {
        ucs2[ret] = '\0';
    }
    return ret;
}


std::wostream& touchmind::operator<<( std::wostream& os, const touchmind::NODE_SIDE &nodeSide )
{
    switch (nodeSide) {
    case NODE_SIDE_UNDEFINED:
        os << L"NODE_SIDE_UNDEFINED";
        break;
    case NODE_SIDE_LEFT:
        os << L"NODE_SIDE_LEFT";
        break;
    case NODE_SIDE_RIGHT:
        os << L"NODE_SIDE_RIGHT";
        break;
    }
    return os;
}

errno_t touchmind::HandleToString( IN FLOAT angle, IN FLOAT length, std::wstring &s )
{
    wchar_t buf[32];
    float degree = angle * 180.0f / touchmind::PI;
    swprintf_s(buf, 32, L"%.2f:%.2f", degree, length);
    s = buf;
    return 0;
}

errno_t touchmind::StringToHandle( IN const std::wstring &s, OUT FLOAT &angle, OUT FLOAT &length )
{
    size_t i = s.find(L':');
    angle = 0.0f;
    length = 100.0f;
    if (i != std::wstring::npos) {
        if (i != 0) {
            std::wstring s_angle = s.substr(0, i);
            wchar_t *endp;
            angle = static_cast<FLOAT>(std::wcstod(s_angle.c_str(), &endp));
        }
        if (i < s.length() - 1) {
            std::wstring s_length = s.substr(i + 1, s.length() - i + 1);
            wchar_t *endp;
            length = static_cast<FLOAT>(std::wcstod(s_length.c_str(), &endp));
        }
    }
    return 0;
}

std::wostream& touchmind::operator<<(std::wostream& os, const touchmind::PATH_DIRECTION &linkDirection)
{
    switch (linkDirection) {
    case PATH_DIRECTION_AUTO:
        os << L"LINK_DIRECTION_AUTO";
        break;
    case PATH_DIRECTION_LEFT:
        os << L"LINK_DIRECTION_LEFT";
        break;
    case PATH_DIRECTION_RIGHT:
        os << L"LINK_DIRECTION_RIGHT";
        break;
    }
    return os;
}

std::wostream& operator<<(std::wostream& os, const D2D1_POINT_2F &point)
{
    os << L"(" << point.x << L"," << point.y << L")";
    return os;
}

std::wostream& operator<<( std::wostream& os, const PROPERTYKEY &key )
{
    if (key == UI_PKEY_Enabled) {
        os << L"UI_PKEY_Enabled";
    } else if (key == UI_PKEY_LabelDescription) {
        os << L"UI_PKEY_LabelDescription";
    } else if (key == UI_PKEY_Keytip) {
        os << L"UI_PKEY_Keytip";
    } else if (key == UI_PKEY_Label) {
        os << L"UI_PKEY_Label";
    } else if (key == UI_PKEY_TooltipDescription) {
        os << L"UI_PKEY_TooltipDescription";
    } else if (key == UI_PKEY_TooltipTitle) {
        os << L"UI_PKEY_TooltipTitle";
    } else if (key == UI_PKEY_LargeImage) {
        os << L"UI_PKEY_LargeImage";
    } else if (key == UI_PKEY_LargeHighContrastImage) {
        os << L"UI_PKEY_LargeHighContrastImage";
    } else if (key == UI_PKEY_SmallImage) {
        os << L"UI_PKEY_SmallImage";
    } else if (key == UI_PKEY_SmallHighContrastImage) {
        os << L"UI_PKEY_SmallHighContrastImage";
    } else if (key == UI_PKEY_CommandId) {
        os << L"UI_PKEY_CommandId";
    } else if (key == UI_PKEY_ItemsSource) {
        os << L"UI_PKEY_ItemsSource";
    } else if (key == UI_PKEY_Categories) {
        os << L"UI_PKEY_Categories";
    } else if (key == UI_PKEY_CategoryId) {
        os << L"UI_PKEY_CategoryId";
    } else if (key == UI_PKEY_SelectedItem) {
        os << L"UI_PKEY_SelectedItem";
    } else if (key == UI_PKEY_CommandType) {
        os << L"UI_PKEY_CommandType";
    } else if (key == UI_PKEY_ItemImage) {
        os << L"UI_PKEY_ItemImage";
    } else if (key == UI_PKEY_BooleanValue) {
        os << L"UI_PKEY_BooleanValue";
    } else if (key == UI_PKEY_DecimalValue) {
        os << L"UI_PKEY_DecimalValue";
    } else if (key == UI_PKEY_StringValue) {
        os << L"UI_PKEY_StringValue";
    } else if (key == UI_PKEY_MaxValue) {
        os << L"UI_PKEY_MaxValue";
    } else if (key == UI_PKEY_MinValue) {
        os << L"UI_PKEY_MinValue";
    } else if (key == UI_PKEY_Increment) {
        os << L"UI_PKEY_Increment";
    } else if (key == UI_PKEY_DecimalPlaces) {
        os << L"UI_PKEY_DecimalPlaces";
    } else if (key == UI_PKEY_FormatString) {
        os << L"UI_PKEY_FormatString";
    } else if (key == UI_PKEY_RepresentativeString) {
        os << L"UI_PKEY_RepresentativeString";
    } else if (key == UI_PKEY_FontProperties) {
        os << L"UI_PKEY_FontProperties";
    } else if (key == UI_PKEY_FontProperties_Family) {
        os << L"UI_PKEY_FontProperties_Family";
    } else if (key == UI_PKEY_FontProperties_Size) {
        os << L"UI_PKEY_FontProperties_Size";
    } else if (key == UI_PKEY_FontProperties_Bold) {
        os << L"UI_PKEY_FontProperties_Bold";
    } else if (key == UI_PKEY_FontProperties_Italic) {
        os << L"UI_PKEY_FontProperties_Italic";
    } else if (key == UI_PKEY_FontProperties_Underline) {
        os << L"UI_PKEY_FontProperties_Underline";
    } else if (key == UI_PKEY_FontProperties_Strikethrough) {
        os << L"UI_PKEY_FontProperties_Strikethrough";
    } else if (key == UI_PKEY_FontProperties_VerticalPositioning) {
        os << L"UI_PKEY_FontProperties_VerticalPositioning";
    } else if (key == UI_PKEY_FontProperties_ForegroundColor) {
        os << L"UI_PKEY_FontProperties_ForegroundColor";
    } else if (key == UI_PKEY_FontProperties_BackgroundColor) {
        os << L"UI_PKEY_FontProperties_BackgroundColor";
    } else if (key == UI_PKEY_FontProperties_ForegroundColorType) {
        os << L"UI_PKEY_FontProperties_ForegroundColorType";
    } else if (key == UI_PKEY_FontProperties_BackgroundColorType) {
        os << L"UI_PKEY_FontProperties_BackgroundColorType";
    } else if (key == UI_PKEY_FontProperties_ChangedProperties) {
        os << L"UI_PKEY_FontProperties_ChangedProperties";
    } else if (key == UI_PKEY_FontProperties_DeltaSize) {
        os << L"UI_PKEY_FontProperties_DeltaSize";
    } else if (key == UI_PKEY_RecentItems) {
        os << L"UI_PKEY_RecentItems";
    } else if (key == UI_PKEY_Pinned) {
        os << L"UI_PKEY_Pinned";
    } else if (key == UI_PKEY_Color) {
        os << L"UI_PKEY_Color";
    } else if (key == UI_PKEY_ColorType) {
        os << L"UI_PKEY_ColorType";
    } else if (key == UI_PKEY_ColorMode) {
        os << L"UI_PKEY_ColorMode";
    } else if (key == UI_PKEY_ThemeColorsCategoryLabel) {
        os << L"UI_PKEY_ThemeColorsCategoryLabel";
    } else if (key == UI_PKEY_StandardColorsCategoryLabel) {
        os << L"UI_PKEY_StandardColorsCategoryLabel";
    } else if (key == UI_PKEY_RecentColorsCategoryLabel) {
        os << L"UI_PKEY_RecentColorsCategoryLabel";
    } else if (key == UI_PKEY_AutomaticColorLabel) {
        os << L"UI_PKEY_AutomaticColorLabel";
    } else if (key == UI_PKEY_NoColorLabel) {
        os << L"UI_PKEY_NoColorLabel";
    } else if (key == UI_PKEY_MoreColorsLabel) {
        os << L"UI_PKEY_MoreColorsLabel";
    } else if (key == UI_PKEY_ThemeColors) {
        os << L"UI_PKEY_ThemeColors";
    } else if (key == UI_PKEY_StandardColors) {
        os << L"UI_PKEY_StandardColors";
    } else if (key == UI_PKEY_ThemeColorsTooltips) {
        os << L"UI_PKEY_ThemeColorsTooltips";
    } else if (key == UI_PKEY_StandardColorsTooltips) {
        os << L"UI_PKEY_StandardColorsTooltips";
    } else if (key == UI_PKEY_Viewable) {
        os << L"UI_PKEY_Viewable";
    } else if (key == UI_PKEY_Minimized) {
        os << L"UI_PKEY_Minimized";
    } else if (key == UI_PKEY_QuickAccessToolbarDock) {
        os << L"UI_PKEY_QuickAccessToolbarDock";
    } else if (key == UI_PKEY_ContextAvailable) {
        os << L"UI_PKEY_ContextAvailable";
    } else if (key == UI_PKEY_GlobalBackgroundColor) {
        os << L"UI_PKEY_GlobalBackgroundColor";
    } else if (key == UI_PKEY_GlobalHighlightColor) {
        os << L"UI_PKEY_GlobalHighlightColor";
    } else if (key == UI_PKEY_GlobalTextColor) {
        os << L"UI_PKEY_GlobalTextColor";
    } else {
        os << L"UNKNOWN(" << key.pid << L")";
    }
    return os;
}

std::wostream& operator<<( std::wostream& os, const UI_EXECUTIONVERB &verb )
{
    switch (verb) {
    case UI_EXECUTIONVERB_PREVIEW:
        os << L"UI_EXECUTIONVERB_PREVIEW";
        break;
    case UI_EXECUTIONVERB_EXECUTE:
        os << L"UI_EXECUTIONVERB_EXECUTE";
        break;
    case UI_EXECUTIONVERB_CANCELPREVIEW:
        os << L"UI_EXECUTIONVERB_CANCELPREVIEW";
        break;
    }
    return os;
}
