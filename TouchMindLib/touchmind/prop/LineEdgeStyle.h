#ifndef TOUCHMIND_PROP_LINEEDGESTYLE_H_
#define TOUCHMIND_PROP_LINEEDGESTYLE_H_

namespace touchmind {
  enum EDGE_ID {
    EDGE_ID_1 = 0,
    EDGE_ID_2 = 1,
  };
  std::wostream &operator<<(std::wostream &os, const EDGE_ID &edgeId);

  enum EDGE_STYLE {
    EDGE_STYLE_UNSPECIFIED = 0,
    EDGE_STYLE_NORMAL = 1,
    EDGE_STYLE_ARROW = 2,
    EDGE_STYLE_CIRCLE = 3,
  };
  std::wostream &operator<<(std::wostream &os, const EDGE_STYLE &edgeStyle);

#define NUMBER_OF_LINE_EDGE_STYLES 9
  enum LINE_EDGE_STYLE {
    LINE_EDGE_STYLE_UNSPECIFIED = 0,
    LINE_EDGE_STYLE_NORMAL = (EDGE_STYLE_NORMAL << 4 | EDGE_STYLE_NORMAL),
    LINE_EDGE_STYLE_EDGE1_ARROW = (EDGE_STYLE_ARROW << 4 | EDGE_STYLE_NORMAL),
    LINE_EDGE_STYLE_EDGE2_ARROW = (EDGE_STYLE_NORMAL << 4 | EDGE_STYLE_ARROW),
    LINE_EDGE_STYLE_BOTH_ARROW = (EDGE_STYLE_ARROW << 4 | EDGE_STYLE_ARROW),
    LINE_EDGE_STYLE_EDGE1_CIRCLE = (EDGE_STYLE_CIRCLE << 4 | EDGE_STYLE_NORMAL),
    LINE_EDGE_STYLE_EDGE2_CIRCLE = (EDGE_STYLE_NORMAL << 4 | EDGE_STYLE_CIRCLE),
    LINE_EDGE_STYLE_BOTH_CIRCLE = (EDGE_STYLE_CIRCLE << 4 | EDGE_STYLE_CIRCLE),
    LINE_EDGE_STYLE_CIRCLE_ARROW = (EDGE_STYLE_CIRCLE << 4 | EDGE_STYLE_ARROW),
    LINE_EDGE_STYLE_ARROW_CIRCLE = (EDGE_STYLE_ARROW << 4 | EDGE_STYLE_CIRCLE)
  };
  std::wostream &operator<<(std::wostream &os, const LINE_EDGE_STYLE &lineEdgeStyle);

  namespace prop {
    class LineEdgeStyle {
    private:
      LineEdgeStyle() {
      }

    public:
      static const std::wstring ToString(EDGE_STYLE edgeStyle) {
        switch (edgeStyle) {
        case EDGE_STYLE_ARROW:
          return std::wstring(L"ARROW");
        case EDGE_STYLE_CIRCLE:
          return std::wstring(L"CIRCLE");
        }
        return std::wstring(L"NORMAL");
      }
      static UINT32 ToIndex(EDGE_STYLE edgeStyle) {
        switch (edgeStyle) {
        case EDGE_STYLE_NORMAL:
          return 0;
        case EDGE_STYLE_ARROW:
          return 1;
        case EDGE_STYLE_CIRCLE:
          return 2;
        }
        return 0;
      }
      static EDGE_STYLE ToEdgeStyle(const std::wstring &s) {
        std::wstring _s(s);
        std::transform(_s.begin(), _s.end(), _s.begin(), std::toupper);
        if (s == L"ARROW") {
          return EDGE_STYLE_ARROW;
        } else if (s == L"CIRCLE") {
          return EDGE_STYLE_CIRCLE;
        }
        return EDGE_STYLE_NORMAL;
      }
      static UINT32 ToIndex(LINE_EDGE_STYLE lineEdgeStyle) {
        switch (lineEdgeStyle) {
        case LINE_EDGE_STYLE_NORMAL:
          return 0;
        case LINE_EDGE_STYLE_EDGE1_ARROW:
          return 1;
        case LINE_EDGE_STYLE_EDGE2_ARROW:
          return 2;
        case LINE_EDGE_STYLE_BOTH_ARROW:
          return 3;
        case LINE_EDGE_STYLE_EDGE1_CIRCLE:
          return 4;
        case LINE_EDGE_STYLE_EDGE2_CIRCLE:
          return 5;
        case LINE_EDGE_STYLE_BOTH_CIRCLE:
          return 6;
        case LINE_EDGE_STYLE_CIRCLE_ARROW:
          return 7;
        case LINE_EDGE_STYLE_ARROW_CIRCLE:
          return 8;
        }
        return 0;
      }
      static LINE_EDGE_STYLE IndexToLineEdgeStyle(UINT32 index) {
        const static LINE_EDGE_STYLE LINE_EDGE_STYLE_LIST[] = {
            LINE_EDGE_STYLE_NORMAL,      LINE_EDGE_STYLE_EDGE1_ARROW,  LINE_EDGE_STYLE_EDGE2_ARROW,
            LINE_EDGE_STYLE_BOTH_ARROW,  LINE_EDGE_STYLE_EDGE1_CIRCLE, LINE_EDGE_STYLE_EDGE2_CIRCLE,
            LINE_EDGE_STYLE_BOTH_CIRCLE, LINE_EDGE_STYLE_CIRCLE_ARROW, LINE_EDGE_STYLE_ARROW_CIRCLE,
        };
        if (0 <= index && index < NUMBER_OF_LINE_EDGE_STYLES) {
          return LINE_EDGE_STYLE_LIST[index];
        }
        return LINE_EDGE_STYLE_NORMAL;
      }
      static void LineEdgeStyleToEdgeStyles(LINE_EDGE_STYLE lineEdgeStyle, EDGE_STYLE &edgeStyle1,
                                            EDGE_STYLE &edgeStyle2) {
        int edge1 = (static_cast<int>(lineEdgeStyle) & 0xf0) >> 4;
        int edge2 = static_cast<int>(lineEdgeStyle) & 0x0f;
        edgeStyle1 = static_cast<EDGE_STYLE>(edge1);
        edgeStyle2 = static_cast<EDGE_STYLE>(edge2);
      }
      static LINE_EDGE_STYLE EdgeStylesToLineEdgeStyle(EDGE_STYLE edgeStyle1, EDGE_STYLE edgeStyle2) {
        int edge1 = static_cast<int>(edgeStyle1);
        int edge2 = static_cast<int>(edgeStyle2);
        return static_cast<LINE_EDGE_STYLE>(edge1 << 4 | edge2);
      }
      static size_t GetNumberOfLineEdgeStyles() {
        return NUMBER_OF_LINE_EDGE_STYLES;
      }
    };

  } // prop
} // touchmind

#endif // TOUCHMIND_PROP_LINEEDGESTYLE_H_