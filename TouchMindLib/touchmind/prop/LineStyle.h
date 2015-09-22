#ifndef TOUCHMIND_PROP_LINESTYLE_H_
#define TOUCHMIND_PROP_LINESTYLE_H_

namespace touchmind {
#define NUMBER_OF_LINE_STYLES 3
  enum LINE_STYLE {
    LINE_STYLE_UNSPECIFIED = 0,
    LINE_STYLE_SOLID = 1,
    LINE_STYLE_DASHED = 2,
    LINE_STYLE_DOTTED = 3,
  };
  std::wostream &operator<<(std::wostream &os, const LINE_STYLE &lineStyle);

  namespace prop {

    class LineStyle {
    private:
      LineStyle() {
      }

    public:
      static const std::wstring ToString(LINE_STYLE lineStyle) {
        switch (lineStyle) {
        case LINE_STYLE_DASHED:
          return std::wstring(L"DASHED");
        case LINE_STYLE_DOTTED:
          return std::wstring(L"DOTTED");
        }
        return std::wstring(L"SOLID");
      }
      static UINT32 ToIndex(LINE_STYLE lineStyle) {
        switch (lineStyle) {
        case LINE_STYLE_SOLID:
          return 0;
        case LINE_STYLE_DASHED:
          return 1;
        case LINE_STYLE_DOTTED:
          return 2;
        }
        return 0;
      }
      static LINE_STYLE ToLineStyle(const std::wstring &s) {
        std::wstring _s(s);
        std::transform(_s.begin(), _s.end(), _s.begin(), std::toupper);
        if (s == L"DASHED") {
          return LINE_STYLE_DASHED;
        } else if (s == L"DOTTED") {
          return LINE_STYLE_DOTTED;
        }
        return LINE_STYLE_SOLID;
      }
      static LINE_STYLE IndexToLineStyle(UINT32 index) {
        const static LINE_STYLE LINE_STYLE_LIST[] = {LINE_STYLE_SOLID, LINE_STYLE_DASHED, LINE_STYLE_DOTTED};
        if (0 <= index && index < NUMBER_OF_LINE_STYLES) {
          return LINE_STYLE_LIST[index];
        }
        return LINE_STYLE_SOLID;
      }
      static size_t GetNumberOfLineStyles() {
        return NUMBER_OF_LINE_STYLES;
      }
    };

  } // prop
} // touchmind

#endif // TOUCHMIND_PROP_LINESTYLE_H_