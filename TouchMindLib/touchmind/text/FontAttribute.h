#ifndef TOUCHMIND_TEXT_FONTATTRIBUTE_H_
#define TOUCHMIND_TEXT_FONTATTRIBUTE_H_

namespace touchmind {
  namespace text {

    class FontAttribute {
    public:
      LONG startPosition;
      LONG length;
      // if not specified, set zero length string
      std::wstring fontFamilyName;
      // if not specified, set zero length string
      std::wstring localeName;
      // if not specified, set zero
      FLOAT fontSize;
      bool bold;
      bool italic;
      bool underline;
      bool strikethrough;
      // if not specified, set -1
      LONG foregroundColor;

      FontAttribute()
          : startPosition(0)
          , length(0)
          , fontFamilyName()
          , localeName()
          , fontSize(0.0f)
          , bold(false)
          , italic(false)
          , underline(false)
          , strikethrough(false)
          , foregroundColor(-1) {
      }
      FontAttribute(const FontAttribute &fontAttribute)
          : startPosition(fontAttribute.startPosition)
          , length(fontAttribute.length)
          , fontFamilyName(fontAttribute.fontFamilyName)
          , localeName(fontAttribute.localeName)
          , fontSize(fontAttribute.fontSize)
          , bold(fontAttribute.bold)
          , italic(fontAttribute.italic)
          , underline(fontAttribute.underline)
          , strikethrough(fontAttribute.strikethrough)
          , foregroundColor(fontAttribute.foregroundColor) {
      }
      FontAttribute(FontAttribute &&fontAttribute)
          : startPosition(fontAttribute.startPosition)
          , length(fontAttribute.length)
          , fontFamilyName(std::move(fontAttribute.fontFamilyName))
          , localeName(std::move(fontAttribute.localeName))
          , fontSize(fontAttribute.fontSize)
          , bold(fontAttribute.bold)
          , italic(fontAttribute.italic)
          , underline(fontAttribute.underline)
          , strikethrough(fontAttribute.strikethrough)
          , foregroundColor(fontAttribute.foregroundColor) {
      }
      FontAttribute &operator=(const FontAttribute &fontAttribute) {
        if (this != &fontAttribute) {
          startPosition = fontAttribute.startPosition;
          length = fontAttribute.length;
          fontFamilyName = fontAttribute.fontFamilyName;
          localeName = fontAttribute.localeName;
          fontSize = fontAttribute.fontSize;
          bold = fontAttribute.bold;
          italic = fontAttribute.italic;
          underline = fontAttribute.underline;
          strikethrough = fontAttribute.strikethrough;
          foregroundColor = fontAttribute.foregroundColor;
        }
        return *this;
      }
      FontAttribute &operator=(FontAttribute &&fontAttribute) {
        if (this != &fontAttribute) {
          startPosition = fontAttribute.startPosition;
          length = fontAttribute.length;
          fontFamilyName = std::move(fontAttribute.fontFamilyName);
          localeName = std::move(fontAttribute.localeName);
          fontSize = fontAttribute.fontSize;
          bold = fontAttribute.bold;
          italic = fontAttribute.italic;
          underline = fontAttribute.underline;
          strikethrough = fontAttribute.strikethrough;
          foregroundColor = fontAttribute.foregroundColor;
        }
        return *this;
      }
      ~FontAttribute() {
      }
    };
    std::wostream &operator<<(std::wostream &strm, const FontAttribute &fontAttribute);

  } // text
} // touchmind

#endif // TOUCHMIND_TEXT_FONTATTRIBUTE_H_