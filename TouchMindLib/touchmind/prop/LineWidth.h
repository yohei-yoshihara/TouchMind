#ifndef TOUCHMIND_PROP_LINEWIDTH_H_
#define TOUCHMIND_PROP_LINEWIDTH_H_

namespace touchmind
{
#define NUMBER_OF_LINE_WIDTHS 4
enum LINE_WIDTH {
    LINE_WIDTH_UNSPECIFIED = 0,
    LINE_WIDTH_1 = 1,
    LINE_WIDTH_3 = 3,
    LINE_WIDTH_5 = 5,
    LINE_WIDTH_8 = 8,
};
std::wostream& operator<< (std::wostream& os, const LINE_WIDTH &lineWidth);

namespace prop
{

class LineWidth
{
private:
    LineWidth() {}

public:
    static INT ToInteger(LINE_WIDTH lineWidth) {
        INT intValue = static_cast<INT>(lineWidth);
        if (0 < intValue && intValue <= 8) {
            return intValue;
        }
        return 1;
    }
    static FLOAT ToFloat(LINE_WIDTH lineWidth) {
        FLOAT floatValue = static_cast<FLOAT>(lineWidth);
        if (0.0f < floatValue && floatValue <= 8.0f) {
            return floatValue;
        }
        return 1.0f;
    }
    static std::wstring ToString(LINE_WIDTH lineWidth) {
        std::wostringstream ostr;
        ostr << ToInteger(lineWidth);
        return ostr.str();
    }
    static UINT32 ToIndex(LINE_WIDTH lineWidth) {
        switch (lineWidth) {
        case LINE_WIDTH_1:
            return 0;
        case LINE_WIDTH_3:
            return 1;
        case LINE_WIDTH_5:
            return 2;
        case LINE_WIDTH_8:
            return 3;
        }
        return 0;
    }
    static LINE_WIDTH ToLineWidth(INT intValue) {
        if (intValue <= 1) {
            return LINE_WIDTH_1;
        } else if (intValue <= 3) {
            return LINE_WIDTH_3;
        } else if (intValue <= 5) {
            return LINE_WIDTH_5;
        } else {
            return LINE_WIDTH_8;
        }
    }
    static LINE_WIDTH ToLineWidth(FLOAT floatValue) {
        const static FLOAT margin = 0.5f;
        if (floatValue <= 1.0f + margin) {
            return LINE_WIDTH_1;
        } else if (floatValue <= 3.0f + margin) {
            return LINE_WIDTH_3;
        } else if (floatValue <= 5.0f + margin) {
            return LINE_WIDTH_5;
        } else {
            return LINE_WIDTH_8;
        }
    }
    static LINE_WIDTH ToLineWidth(const std::wstring &s) {
        wchar_t *p = nullptr;
        int intValue = std::wcstol(s.c_str(), &p, 10);
        return ToLineWidth(intValue);
    }
    static LINE_WIDTH IndexToLineWidth(UINT32 index) {
        const static LINE_WIDTH LINE_WIDTH_LIST[] = {LINE_WIDTH_1, LINE_WIDTH_3, LINE_WIDTH_5, LINE_WIDTH_8};
        if (0 <= index && index < NUMBER_OF_LINE_WIDTHS) {
            return LINE_WIDTH_LIST[index];
        }
        return LINE_WIDTH_1;
    }
    static size_t GetNumberOfLineWidths() {
        return NUMBER_OF_LINE_WIDTHS;
    }
};

} // prop
} // touchmind

#endif // TOUCHMIND_PROP_LINEWIDTH_H_