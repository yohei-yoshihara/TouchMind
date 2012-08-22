#ifndef TOUCHMIND_COMMON_H_
#define TOUCHMIND_COMMON_H_

#include "../TouchMindMUI_en_us/resource.h"
#include "../TouchMindMUI_en_us/ribbonres.h"

#include "touchmind/prop/NodeShape.h"
#include "touchmind/prop/LineWidth.h"
#include "touchmind/prop/LineStyle.h"
#include "touchmind/prop/LineEdgeStyle.h"

namespace touchmind
{
typedef INT32 NODE_ID;
typedef INT32 LINK_ID;
typedef UINT64 REPAINT_COUNTER;
typedef UINT64 SAVE_COUNTER;

extern const REPAINT_COUNTER MODEL_INITIAL_REPAINT_COUNTER;
extern const REPAINT_COUNTER VIEW_INITIAL_REPAINT_COUNTER;
extern const SAVE_COUNTER MODEL_INITIAL_SAVE_COUNTER;
extern const SAVE_COUNTER OPERATION_INITIAL_SAVE_COUNTER;

extern const double PI_DOUBLE;
extern const float  PI;
extern const float  PI_2;
extern const float  PI_4;
extern const double SQRT_2_DOUBLE;
extern const float  SQRT_2;

enum VISITOR_RESULT {
    VISITOR_RESULT_CONTINUE,
    VISITOR_RESULT_STOP
};

enum NODE_SIDE {
    NODE_SIDE_RIGHT = 0,
    NODE_SIDE_LEFT = 1,
    NODE_SIDE_UNDEFINED = -1
};
std::wostream& operator<< (std::wostream& os, const NODE_SIDE &nodeSide);

enum PATH_DIRECTION {
    PATH_DIRECTION_RIGHT,
    PATH_DIRECTION_LEFT,
    PATH_DIRECTION_AUTO
};
std::wostream& operator<< (std::wostream& os, const PATH_DIRECTION &linkDirection);

#define _APPLICATION_ID L"TouchMind.TouchMind"
#define _EXE_NAME L"TouchMind.exe"
#define _EXTENSION L".tmm"
#define _FIRENDLY_TYPE_NAME L"TouchMind Mind Map File"
#define _WINDOW_TITLE L"TouchMind"
#define _OPEN_DIALOG_BOX_LONG L"TouchMind(.tmm)"
#define _OPEN_DIALOG_BOX_SHORT L"*.tmm"
#define _LOG_FOLDERNAME L"\\TouchMind"
#define _LOG_FILENAME L"\\TouchMind.log"
#define _CLIPBOARD_XML_ID L"TouchMind.XML"
#define _TEMPFILE_PREFIX L"TouchMind_"

extern errno_t NodeIdToString(IN touchmind::NODE_ID nodeId, OUT std::wstring &s);
extern errno_t StringToNodeId(IN std::wstring &s, OUT touchmind::NODE_ID *pNodeId);
extern errno_t SystemtimeToString(IN const SYSTEMTIME *pSystemTime, OUT std::wstring &s);
extern errno_t StringToSystemtime(IN std::wstring &s, OUT SYSTEMTIME *pSystemTime);
extern errno_t FontSizeToString(IN FLOAT fontSize, OUT std::wstring &s);
extern errno_t StringToFontSize(IN std::wstring &s, OUT FLOAT *pFontSize);
extern errno_t ColorrefToString(IN COLORREF colorref, OUT std::wstring &s);
extern errno_t StringToColorref(IN std::wstring &s, OUT COLORREF *pColorref);
extern errno_t ColorFToString(IN D2D1_COLOR_F colorf, OUT std::wstring &s);
extern errno_t StringToColorF(IN std::wstring &s, OUT D2D1_COLOR_F *colorf);
extern errno_t SizeToString(IN FLOAT size, OUT std::wstring &s);
extern errno_t StringToSize(IN std::wstring &s, OUT FLOAT *pSize);
extern errno_t ColorrefToColorF(IN COLORREF colorref, OUT D2D1_COLOR_F *colorf);
extern errno_t ColorFToColorref(IN D2D1_COLOR_F &colorF, OUT COLORREF *colorref);
extern errno_t RgbToHsv(IN FLOAT r, IN FLOAT g, IN FLOAT b, OUT FLOAT &h, OUT FLOAT &s, OUT FLOAT &v);
extern errno_t HsvToRgb(IN FLOAT h, IN FLOAT s, IN FLOAT v, OUT FLOAT &r, OUT FLOAT &g, OUT FLOAT &b);
extern size_t CountLeftSpaces(IN const std::wstring &line);
extern void TrimLeft(IN const std::wstring& line, OUT std::wstring& text);
extern void Trim(IN const std::wstring& line, OUT std::wstring& text);
extern size_t LengthInUTF8(IN const wchar_t *ucs2);
extern int UCS2ToUTF8(IN const wchar_t *ucs2, OUT char *utf8, IN size_t n);
extern size_t LengthInUCS2(IN const char *utf8);
extern int UTF8ToUCS2(IN const char *utf8, OUT wchar_t *ucs2, IN size_t n);
extern errno_t HandleToString(IN FLOAT angle, IN FLOAT length, std::wstring &s);
extern errno_t StringToHandle(IN const std::wstring &s, OUT FLOAT &angle, OUT FLOAT &length);

inline FLOAT ToRadian(FLOAT degree)
{
    return degree * PI / 180.0f;
}

inline FLOAT ToDegree(FLOAT radian)
{
    return radian * 180.0f / PI;
}

} // touchmind

std::wostream& operator<< (std::wostream& os, const D2D1_POINT_2F &point);
extern std::wostream& operator<< (std::wostream& os, PROPERTYKEY const& propertykey);
extern std::wostream& operator<< (std::wostream& os, const UI_EXECUTIONVERB &verb);


#endif // TOUCHMIND_COMMON_H_