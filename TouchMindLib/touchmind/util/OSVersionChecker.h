#ifndef TOUCHMIND_UTIL_OSVERSIONCHECKER_H_
#define TOUCHMIND_UTIL_OSVERSIONCHECKER_H_

namespace touchmind {
  namespace util {
    enum OSVersion {
      OSVersion_OlderThanXP = 0,
      OSVersion_WindowsXP = 1,
      OSVersion_Windows2003 = 2,
      OSVersion_WindowsVista = 3,
      OSVersion_Windows7 = 4
    };

    class OSVersionChecker {
    private:
      OSVersionChecker(void) {
      }
      ~OSVersionChecker(void) {
      }

    public:
      static OSVersion GetOSVersion();
      static bool IsVista();
      static bool IsWin7();
    };
  } // util
} // touchmin

#endif // TOUCHMIND_UTIL_OSVERSIONCHECKER_H_