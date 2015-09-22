#ifndef TOUCHMIND_CONTROL_STATUSBAR_H_
#define TOUCHMIND_CONTROL_STATUSBAR_H_

#include "forwarddecl.h"

namespace touchmind {
  namespace control {

    class StatusBar {
    private:
      HWND m_hWnd;
      HWND m_parehtHWnd;
      int m_numberOfParts;

    public:
      StatusBar(void);
      virtual ~StatusBar(void);
      HRESULT Initialize(HWND hwndParent, int idStatus, HINSTANCE hInstance, int cParts);
      bool IsInitialized() {
        return m_hWnd != nullptr;
      }
      RECT GetRect();
      void OnSize(WPARAM wParam, LPARAM lParam);
      void SetParts(int numberOfParts);
      void SetColor(COLORREF color);
      void SetText(int index, const std::wstring &text);
      void SetText(int index, const wchar_t *text);
      void ClearText(int index);
    };

  } // control
} // touchmind

#endif // TOUCHMIND_CONTROL_STATUSBAR_H_