#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/Configuration.h"
#include "touchmind/text/FontAttributeCommand.h"

const COLORREF touchmind::text::FontAttributeCommand::DEFAULT_FONT_COLOR = 0; // black

void touchmind::text::FontAttributeCommand::ToFontAttribute(touchmind::text::FontAttribute *pFontAttribute) {
  if (fontFamilyNameStatus == UI_FONTPROPERTIES_SET) {
    pFontAttribute->fontFamilyName = fontFamilyName;
  } else {
    pFontAttribute->fontFamilyName = L"";
  }
  if (localeNameStatus == UI_FONTPROPERTIES_SET) {
    pFontAttribute->localeName = localeName;
  } else {
    pFontAttribute->localeName = L"";
  }
  if (fontSizeStatus == UI_FONTPROPERTIES_SET) {
    pFontAttribute->fontSize = fontSize;
  } else {
    pFontAttribute->fontSize = 0;
  }
  pFontAttribute->bold = (bold == UI_FONTPROPERTIES_SET);
  pFontAttribute->italic = (italic == UI_FONTPROPERTIES_SET);
  pFontAttribute->underline = (underline == UI_FONTPROPERTIES_SET);
  pFontAttribute->strikethrough = (strikethrough == UI_FONTPROPERTIES_SET);
  if (foregroundColorStatus == UI_FONTPROPERTIES_SET) {
    COLORREF colorref;
    touchmind::ColorFToColorref(foregroundColor, &colorref);
    pFontAttribute->foregroundColor = colorref;
  } else {
    pFontAttribute->foregroundColor = DEFAULT_FONT_COLOR;
  }
}

void touchmind::text::FontAttributeCommand::FromFontAttribute(touchmind::Configuration *pConfiguration,
                                                              touchmind::text::FontAttribute *pFontAttribute) {
  // font family name
  if (pFontAttribute->fontFamilyName.length() > 0) {
    fontFamilyNameStatus = UI_FONTPROPERTIES_SET;
    fontFamilyName = pFontAttribute->fontFamilyName;
  } else {
    fontFamilyNameStatus = UI_FONTPROPERTIES_SET;
    fontFamilyName = pConfiguration->GetDefaultFontFamilyName();
  }

  // locale name
  if (pFontAttribute->localeName.length() > 0) {
    localeNameStatus = UI_FONTPROPERTIES_SET;
    localeName = pFontAttribute->localeName;
  } else {
    localeNameStatus = UI_FONTPROPERTIES_SET;
    localeName = pConfiguration->GetDefaultLocaleName();
  }

  // font size
  if (pFontAttribute->fontSize > 0) {
    fontSizeStatus = UI_FONTPROPERTIES_SET;
    fontSize = pFontAttribute->fontSize;
  } else {
    fontSizeStatus = UI_FONTPROPERTIES_SET;
    fontSize = pConfiguration->GetDefaultFontSize();
  }

  // bold
  bold = pFontAttribute->bold ? UI_FONTPROPERTIES_SET : UI_FONTPROPERTIES_NOTSET;
  // italic
  italic = pFontAttribute->italic ? UI_FONTPROPERTIES_SET : UI_FONTPROPERTIES_NOTSET;
  // underline
  underline = pFontAttribute->underline ? UI_FONTPROPERTIES_SET : UI_FONTPROPERTIES_NOTSET;
  // strikethrough
  strikethrough = pFontAttribute->strikethrough ? UI_FONTPROPERTIES_SET : UI_FONTPROPERTIES_NOTSET;

  if (pFontAttribute->foregroundColor >= 0) {
    foregroundColorStatus = UI_FONTPROPERTIES_SET;
    touchmind::ColorrefToColorF(pFontAttribute->foregroundColor, &foregroundColor);
  } else {
    foregroundColorStatus = UI_FONTPROPERTIES_SET;
    foregroundColor = D2D1::ColorF(D2D1::ColorF::Black);
  }
}

std::wostream &printUiFontProperties(std::wostream &os, const UI_FONTPROPERTIES fontProp) {
  static const wchar_t *UNAVAILABLE = L"UNAVAILABLE";
  static const wchar_t *SET = L"SET";
  static const wchar_t *NOTSET = L"NOTSET";

  if (fontProp == UI_FONTPROPERTIES_NOTAVAILABLE) {
    os << UNAVAILABLE;
  } else if (fontProp == UI_FONTPROPERTIES_NOTSET) {
    os << NOTSET;
  } else {
    os << SET;
  }

  return os;
}

std::wostream &operator<<(std::wostream &os, touchmind::text::FontAttributeCommand const &fontAttributeCommand) {
  os << L"FontAttributeCommand";
  // font family name
  os << L"[fontFamilyNameStatus=";
  printUiFontProperties(os, fontAttributeCommand.fontFamilyNameStatus);
  if (fontAttributeCommand.fontFamilyNameStatus != UI_FONTPROPERTIES_NOTAVAILABLE) {
    os << L",fontFamilyName=" << fontAttributeCommand.fontFamilyName;
  }
  // locale name
  os << L",localeNameStatus=";
  printUiFontProperties(os, fontAttributeCommand.localeNameStatus);
  if (fontAttributeCommand.localeNameStatus != UI_FONTPROPERTIES_NOTAVAILABLE) {
    os << L",localeName=" << fontAttributeCommand.localeName;
  }

  // font size
  os << L",fontSize=";
  printUiFontProperties(os, fontAttributeCommand.fontSizeStatus);
  if (fontAttributeCommand.fontSizeStatus != UI_FONTPROPERTIES_NOTAVAILABLE) {
    os << L",fontSize=" << fontAttributeCommand.fontSize;
  }

  // bold
  os << L",bold=";
  printUiFontProperties(os, fontAttributeCommand.bold);

  // italic
  os << L",italic=";
  printUiFontProperties(os, fontAttributeCommand.italic);

  // underline
  os << L",underline=";
  printUiFontProperties(os, fontAttributeCommand.underline);

  // strikethrough
  os << L",strikethrough=";
  printUiFontProperties(os, fontAttributeCommand.strikethrough);

  // foreground color
  os << L",foregroundColorStatus=";
  printUiFontProperties(os, fontAttributeCommand.foregroundColorStatus);
  if (fontAttributeCommand.foregroundColorStatus != UI_FONTPROPERTIES_NOTAVAILABLE) {
    os << L",foregroundColor=(" << fontAttributeCommand.foregroundColor.r << L","
       << fontAttributeCommand.foregroundColor.g << L"," << fontAttributeCommand.foregroundColor.b << L","
       << fontAttributeCommand.foregroundColor.a << L")";
  }
  os << L"]";
  return os;
}
