#ifndef TOUCHMIND_PRINT_XPSPRINTCOMMON_H_
#define TOUCHMIND_PRINT_XPSPRINTCOMMON_H_

namespace touchmind {
  namespace print {

    enum PRINTSETUP_PAGE_SIZE {
      PRINTSETUP_PAGE_SIZE_A3,
      PRINTSETUP_PAGE_SIZE_A4,
      PRINTSETUP_PAGE_SIZE_A5,
      PRINTSETUP_PAGE_SIZE_B4,
      PRINTSETUP_PAGE_SIZE_B5,
      PRINTSETUP_PAGE_SIZE_LETTER,
      PRINTSETUP_PAGE_SIZE_LEGAL,
      PRINTSETUP_PAGE_SIZE_US4X8,
      PRINTSETUP_PAGE_SIZE_US5X7,
    };

    enum PRINTSETUP_PAGE_DIRECTION { PRINTSETUP_PAGE_DIRECTION_LANDSCAPE, PRINTSETUP_PAGE_DIRECTION_PORTRAIT };

    typedef struct _XPSPRINT_MARGIN {
      FLOAT left;
      FLOAT top;
      FLOAT right;
      FLOAT bottom;
    } XPSPRINT_MARGIN;

  } // print
} // touchmind

extern std::wostream &operator<<(std::wostream &os, const DEVMODE &devMode);
extern std::wostream &operator<<(std::wostream &os, const PRINTPAGERANGE &pageRange);
extern std::wostream &operator<<(std::wostream &os, const PRINTDLGEX &pdx);

#endif // TOUCHMIND_PRINT_XPSPRINTCOMMON_H_
