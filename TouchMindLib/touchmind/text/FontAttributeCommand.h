#ifndef TOUCHMIND_CONTROL_FONTATTRIBUTECOMMDND_H_
#define TOUCHMIND_CONTROL_FONTATTRIBUTECOMMDND_H_

#include "touchmind/Configuration.h"
#include "touchmind/text/FontAttribute.h"

namespace touchmind
{
namespace text
{

class FontAttributeCommand
{
public:
    static const COLORREF DEFAULT_FONT_COLOR;

    FontAttributeCommand() :
        fontFamilyNameStatus(UI_FONTPROPERTIES_NOTAVAILABLE),
        fontFamilyName(),
        localeNameStatus(UI_FONTPROPERTIES_NOTAVAILABLE),
        localeName(L""),
        fontSizeStatus(UI_FONTPROPERTIES_NOTAVAILABLE),
        fontSize(10.5f),
        bold(UI_FONTPROPERTIES_NOTAVAILABLE),
        italic(UI_FONTPROPERTIES_NOTAVAILABLE),
        underline(UI_FONTPROPERTIES_NOTAVAILABLE),
        strikethrough(UI_FONTPROPERTIES_NOTAVAILABLE),
        foregroundColorStatus(UI_FONTPROPERTIES_NOTAVAILABLE),
        foregroundColor(D2D1::ColorF(D2D1::ColorF::Black)) {}
    // font family
    UI_FONTPROPERTIES fontFamilyNameStatus;
    std::wstring      fontFamilyName;
    // locale name
    UI_FONTPROPERTIES localeNameStatus;
    std::wstring      localeName;
    // font size
    UI_FONTPROPERTIES fontSizeStatus;
    FLOAT             fontSize;
    // bold
    UI_FONTPROPERTIES bold;
    // italic
    UI_FONTPROPERTIES italic;
    // underline
    UI_FONTPROPERTIES underline;
    // strikethrough
    UI_FONTPROPERTIES strikethrough;
    // foreground color
    UI_FONTPROPERTIES foregroundColorStatus;
    D2D1_COLOR_F      foregroundColor;

    void SetFontWeight(DWRITE_FONT_WEIGHT fontWeight) {
        bold = fontWeight != DWRITE_FONT_WEIGHT_NORMAL ? UI_FONTPROPERTIES_SET : UI_FONTPROPERTIES_NOTSET;
    }
    DWRITE_FONT_WEIGHT GetFontWeight() {
        return bold == UI_FONTPROPERTIES_SET ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL;
    }
    void SetFontStyle(DWRITE_FONT_STYLE fontStyle) {
        italic = fontStyle != DWRITE_FONT_STYLE_NORMAL ? UI_FONTPROPERTIES_SET : UI_FONTPROPERTIES_NOTSET;
    }
    DWRITE_FONT_STYLE  GetFontStyle()  {
        return italic == UI_FONTPROPERTIES_SET ? DWRITE_FONT_STYLE_OBLIQUE : DWRITE_FONT_STYLE_NORMAL;
    }
    BOOL GetUnderline() {
        return underline == UI_FONTPROPERTIES_SET;
    }
    BOOL GetStrikethrough() {
        return strikethrough == UI_FONTPROPERTIES_SET;
    }
    DWRITE_FONT_STRETCH GetFontStretch() {
        return DWRITE_FONT_STRETCH_NORMAL;
    }

    void ToFontAttribute(touchmind::text::FontAttribute* pFontAttribute);
    void FromFontAttribute(touchmind::Configuration* pConfiguration, touchmind::text::FontAttribute* pFontAttribute);

    friend std::wostream& operator<< (std::wostream& strm, touchmind::text::FontAttributeCommand const& fontAttributeCommand);
};

} // text
} // touchmind

#endif // TOUCHMIND_CONTROL_FONTATTRIBUTECOMMDND_H_
