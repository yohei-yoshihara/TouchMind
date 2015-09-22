#include "StdAfx.h"
#include <strsafe.h>
#include "touchmind/util/Utils.h"

struct WM {
  UINT id;
  wchar_t *name;
};
struct WM wms[] = {{0x0000, L"WM_NULL"},
                   {0x0001, L"WM_CREATE"},
                   {0x0002, L"WM_DESTROY"},
                   {0x0003, L"WM_MOVE"},
                   {0x0005, L"WM_SIZE"},
                   {0x0006, L"WM_ACTIVATE"},
                   {0x0007, L"WM_SETFOCUS"},
                   {0x0008, L"WM_KILLFOCUS"},
                   {0x000A, L"WM_ENABLE"},
                   {0x000B, L"WM_SETREDRAW"},
                   {0x000C, L"WM_SETTEXT"},
                   {0x000D, L"WM_GETTEXT"},
                   {0x000E, L"WM_GETTEXTLENGTH"},
                   {0x000F, L"WM_PAINT"},
                   {0x0010, L"WM_CLOSE"},
                   {0x0011, L"WM_QUERYENDSESSION"},
                   {0x0012, L"WM_QUIT"},
                   {0x0013, L"WM_QUERYOPEN"},
                   {0x0014, L"WM_ERASEBKGND"},
                   {0x0015, L"WM_SYSCOLORCHANGE"},
                   {0x0016, L"WM_ENDSESSION"},
                   {0x0018, L"WM_SHOWWINDOW"},
                   {0x001A, L"WM_WININICHANGE"},
                   {0x001B, L"WM_DEVMODECHANGE"},
                   {0x001C, L"WM_ACTIVATEAPP"},
                   {0x001D, L"WM_FONTCHANGE"},
                   {0x001E, L"WM_TIMECHANGE"},
                   {0x001F, L"WM_CANCELMODE"},
                   {0x0020, L"WM_SETCURSOR"},
                   {0x0021, L"WM_MOUSEACTIVATE"},
                   {0x0022, L"WM_CHILDACTIVATE"},
                   {0x0023, L"WM_QUEUESYNC"},
                   {0x0024, L"WM_GETMINMAXINFO"},
                   {0x0026, L"WM_PAINTICON"},
                   {0x0027, L"WM_ICONERASEBKGND"},
                   {0x0028, L"WM_NEXTDLGCTL"},
                   {0x002A, L"WM_SPOOLERSTATUS"},
                   {0x002B, L"WM_DRAWITEM"},
                   {0x002C, L"WM_MEASUREITEM"},
                   {0x002D, L"WM_DELETEITEM"},
                   {0x002E, L"WM_VKEYTOITEM"},
                   {0x002F, L"WM_CHARTOITEM"},
                   {0x0030, L"WM_SETFONT"},
                   {0x0031, L"WM_GETFONT"},
                   {0x0032, L"WM_SETHOTKEY"},
                   {0x0033, L"WM_GETHOTKEY"},
                   {0x0037, L"WM_QUERYDRAGICON"},
                   {0x0039, L"WM_COMPAREITEM"},
                   {0x003D, L"WM_GETOBJECT"},
                   {0x0041, L"WM_COMPACTING"},
                   {0x0044, L"WM_COMMNOTIFY"},
                   {0x0046, L"WM_WINDOWPOSCHANGING"},
                   {0x0047, L"WM_WINDOWPOSCHANGED"},
                   {0x0048, L"WM_POWER"},
                   {0x004A, L"WM_COPYDATA"},
                   {0x004B, L"WM_CANCELJOURNAL"},
                   {0x004E, L"WM_NOTIFY"},
                   {0x0050, L"WM_INPUTLANGCHANGEREQUEST"},
                   {0x0051, L"WM_INPUTLANGCHANGE"},
                   {0x0052, L"WM_TCARD"},
                   {0x0053, L"WM_HELP"},
                   {0x0054, L"WM_USERCHANGED"},
                   {0x0055, L"WM_NOTIFYFORMAT"},
                   {0x007B, L"WM_CONTEXTMENU"},
                   {0x007C, L"WM_STYLECHANGING"},
                   {0x007D, L"WM_STYLECHANGED"},
                   {0x007E, L"WM_DISPLAYCHANGE"},
                   {0x007F, L"WM_GETICON"},
                   {0x0080, L"WM_SETICON"},
                   {0x0081, L"WM_NCCREATE"},
                   {0x0082, L"WM_NCDESTROY"},
                   {0x0083, L"WM_NCCALCSIZE"},
                   {0x0084, L"WM_NCHITTEST"},
                   {0x0085, L"WM_NCPAINT"},
                   {0x0086, L"WM_NCACTIVATE"},
                   {0x0087, L"WM_GETDLGCODE"},
                   {0x0088, L"WM_SYNCPAINT"},
                   {0x00A0, L"WM_NCMOUSEMOVE"},
                   {0x00A1, L"WM_NCLBUTTONDOWN"},
                   {0x00A2, L"WM_NCLBUTTONUP"},
                   {0x00A3, L"WM_NCLBUTTONDBLCLK"},
                   {0x00A4, L"WM_NCRBUTTONDOWN"},
                   {0x00A5, L"WM_NCRBUTTONUP"},
                   {0x00A6, L"WM_NCRBUTTONDBLCLK"},
                   {0x00A7, L"WM_NCMBUTTONDOWN"},
                   {0x00A8, L"WM_NCMBUTTONUP"},
                   {0x00A9, L"WM_NCMBUTTONDBLCLK"},
                   {0x00AB, L"WM_NCXBUTTONDOWN"},
                   {0x00AC, L"WM_NCXBUTTONUP"},
                   {0x00AD, L"WM_NCXBUTTONDBLCLK"},
                   {0x00FE, L"WM_INPUT_DEVICE_CHANGE"},
                   {0x00FF, L"WM_INPUT"},
                   {0x0100, L"WM_KEYDOWN"},
                   {0x0100, L"WM_KEYFIRST"},
                   {0x0101, L"WM_KEYUP"},
                   {0x0102, L"WM_CHAR"},
                   {0x0103, L"WM_DEADCHAR"},
                   {0x0104, L"WM_SYSKEYDOWN"},
                   {0x0105, L"WM_SYSKEYUP"},
                   {0x0106, L"WM_SYSCHAR"},
                   {0x0107, L"WM_SYSDEADCHAR"},
                   {0x0108, L"WM_KEYLAST"},
                   {0x0109, L"WM_UNICHAR"},
                   {0x010D, L"WM_IME_STARTCOMPOSITION"},
                   {0x010E, L"WM_IME_ENDCOMPOSITION"},
                   {0x010F, L"WM_IME_COMPOSITION"},
                   {0x010F, L"WM_IME_KEYLAST"},
                   {0x0110, L"WM_INITDIALOG"},
                   {0x0111, L"WM_COMMAND"},
                   {0x0112, L"WM_SYSCOMMAND"},
                   {0x0113, L"WM_TIMER"},
                   {0x0114, L"WM_HSCROLL"},
                   {0x0115, L"WM_VSCROLL"},
                   {0x0116, L"WM_INITMENU"},
                   {0x0117, L"WM_INITMENUPOPUP"},
                   {0x0119, L"WM_GESTURE"},
                   {0x011A, L"WM_GESTURENOTIFY"},
                   {0x011F, L"WM_MENUSELECT"},
                   {0x0120, L"WM_MENUCHAR"},
                   {0x0121, L"WM_ENTERIDLE"},
                   {0x0122, L"WM_MENURBUTTONUP"},
                   {0x0123, L"WM_MENUDRAG"},
                   {0x0124, L"WM_MENUGETOBJECT"},
                   {0x0125, L"WM_UNINITMENUPOPUP"},
                   {0x0126, L"WM_MENUCOMMAND"},
                   {0x0127, L"WM_CHANGEUISTATE"},
                   {0x0128, L"WM_UPDATEUISTATE"},
                   {0x0129, L"WM_QUERYUISTATE"},
                   {0x0132, L"WM_CTLCOLORMSGBOX"},
                   {0x0133, L"WM_CTLCOLOREDIT"},
                   {0x0134, L"WM_CTLCOLORLISTBOX"},
                   {0x0135, L"WM_CTLCOLORBTN"},
                   {0x0136, L"WM_CTLCOLORDLG"},
                   {0x0137, L"WM_CTLCOLORSCROLLBAR"},
                   {0x0138, L"WM_CTLCOLORSTATIC"},
                   {0x0200, L"WM_MOUSEMOVE"},
                   {0x0200, L"WM_MOUSEFIRST"},
                   {0x0201, L"WM_LBUTTONDOWN"},
                   {0x0202, L"WM_LBUTTONUP"},
                   {0x0203, L"WM_LBUTTONDBLCLK"},
                   {0x0204, L"WM_RBUTTONDOWN"},
                   {0x0205, L"WM_RBUTTONUP"},
                   {0x0206, L"WM_RBUTTONDBLCLK"},
                   {0x0207, L"WM_MBUTTONDOWN"},
                   {0x0208, L"WM_MBUTTONUP"},
                   {0x0209, L"WM_MBUTTONDBLCLK"},
                   {0x0209, L"WM_MOUSELAST"},
                   {0x020A, L"WM_MOUSEWHEEL"},
                   {0x020B, L"WM_XBUTTONDOWN"},
                   {0x020C, L"WM_XBUTTONUP"},
                   {0x020D, L"WM_XBUTTONDBLCLK"},
                   {0x020E, L"WM_MOUSEHWHEEL"},
                   {0x0210, L"WM_PARENTNOTIFY"},
                   {0x0211, L"WM_ENTERMENULOOP"},
                   {0x0212, L"WM_EXITMENULOOP"},
                   {0x0213, L"WM_NEXTMENU"},
                   {0x0214, L"WM_SIZING"},
                   {0x0215, L"WM_CAPTURECHANGED"},
                   {0x0216, L"WM_MOVING"},
                   {0x0218, L"WM_POWERBROADCAST"},
                   {0x0219, L"WM_DEVICECHANGE"},
                   {0x0220, L"WM_MDICREATE"},
                   {0x0221, L"WM_MDIDESTROY"},
                   {0x0222, L"WM_MDIACTIVATE"},
                   {0x0223, L"WM_MDIRESTORE"},
                   {0x0224, L"WM_MDINEXT"},
                   {0x0225, L"WM_MDIMAXIMIZE"},
                   {0x0226, L"WM_MDITILE"},
                   {0x0227, L"WM_MDICASCADE"},
                   {0x0228, L"WM_MDIICONARRANGE"},
                   {0x0229, L"WM_MDIGETACTIVE"},
                   {0x0230, L"WM_MDISETMENU"},
                   {0x0231, L"WM_ENTERSIZEMOVE"},
                   {0x0232, L"WM_EXITSIZEMOVE"},
                   {0x0233, L"WM_DROPFILES"},
                   {0x0234, L"WM_MDIREFRESHMENU"},
                   {0x0240, L"WM_TOUCH"},
                   {0x0281, L"WM_IME_SETCONTEXT"},
                   {0x0282, L"WM_IME_NOTIFY"},
                   {0x0283, L"WM_IME_CONTROL"},
                   {0x0284, L"WM_IME_COMPOSITIONFULL"},
                   {0x0285, L"WM_IME_SELECT"},
                   {0x0286, L"WM_IME_CHAR"},
                   {0x0288, L"WM_IME_REQUEST"},
                   {0x0290, L"WM_IME_KEYDOWN"},
                   {0x0291, L"WM_IME_KEYUP"},
                   {0x02A0, L"WM_NCMOUSEHOVER"},
                   {0x02A1, L"WM_MOUSEHOVER"},
                   {0x02A2, L"WM_NCMOUSELEAVE"},
                   {0x02A3, L"WM_MOUSELEAVE"},
                   {0x02B1, L"WM_WTSSESSION_CHANGE"},
                   {0x02c0, L"WM_TABLET_FIRST"},
                   {0x02df, L"WM_TABLET_LAST"},
                   {0x0300, L"WM_CUT"},
                   {0x0301, L"WM_COPY"},
                   {0x0302, L"WM_PASTE"},
                   {0x0303, L"WM_CLEAR"},
                   {0x0304, L"WM_UNDO"},
                   {0x0305, L"WM_RENDERFORMAT"},
                   {0x0306, L"WM_RENDERALLFORMATS"},
                   {0x0307, L"WM_DESTROYCLIPBOARD"},
                   {0x0308, L"WM_DRAWCLIPBOARD"},
                   {0x0309, L"WM_PAINTCLIPBOARD"},
                   {0x030A, L"WM_VSCROLLCLIPBOARD"},
                   {0x030B, L"WM_SIZECLIPBOARD"},
                   {0x030C, L"WM_ASKCBFORMATNAME"},
                   {0x030D, L"WM_CHANGECBCHAIN"},
                   {0x030E, L"WM_HSCROLLCLIPBOARD"},
                   {0x030F, L"WM_QUERYNEWPALETTE"},
                   {0x0310, L"WM_PALETTEISCHANGING"},
                   {0x0311, L"WM_PALETTECHANGED"},
                   {0x0312, L"WM_HOTKEY"},
                   {0x0317, L"WM_PRINT"},
                   {0x0318, L"WM_PRINTCLIENT"},
                   {0x0319, L"WM_APPCOMMAND"},
                   {0x031A, L"WM_THEMECHANGED"},
                   {0x031D, L"WM_CLIPBOARDUPDATE"},
                   {0x031E, L"WM_DWMCOMPOSITIONCHANGED"},
                   {0x031F, L"WM_DWMNCRENDERINGCHANGED"},
                   {0x0320, L"WM_DWMCOLORIZATIONCOLORCHANGED"},
                   {0x0321, L"WM_DWMWINDOWMAXIMIZEDCHANGE"},
                   {0x0323, L"WM_DWMSENDICONICTHUMBNAIL"},
                   {0x0326, L"WM_DWMSENDICONICLIVEPREVIEWBITMAP"},
                   {0x033F, L"WM_GETTITLEBARINFOEX"},
                   {0x0358, L"WM_HANDHELDFIRST"},
                   {0x035F, L"WM_HANDHELDLAST"},
                   {0x0360, L"WM_AFXFIRST"},
                   {0x037F, L"WM_AFXLAST"},
                   {0x0380, L"WM_PENWINFIRST"},
                   {0x038F, L"WM_PENWINLAST"},
                   {0x0400, L"WM_USER"},
                   {0x8000, L"WM_APP"},
                   {0, nullptr}};

void touchmind::util::_WMessage(std::ios_base &os, UINT message) {
  std::wostream *pos = dynamic_cast<std::wostream *>(&os);
  if (pos) {
    int i = 0;
    while (wms[i].name != nullptr) {
      if (wms[i].id == message) {
        (*pos) << wms[i].name;
        return;
      }
      ++i;
    }
    (*pos) << L"UNKNOWN(" << message << L")";
  }
}
/*
void touchmind::util::_LastError( std::ios_base& os, wchar_t * functionName )
{
    std::wostream *pos = dynamic_cast<std::wostream*>(&os);
    if (pos) {
        LPVOID lpMsgBuf;
        LPVOID lpDisplayBuf;
        DWORD dw = ::GetLastError();

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            dw,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0, nullptr);

        lpDisplayBuf = (LPVOID) LocalAlloc(LMEM_ZEROINIT,
                                           (lstrlen((LPCTSTR) lpMsgBuf) + lstrlen(functionName) + 40) * sizeof(TCHAR));
        StringCchPrintf((LPTSTR) lpDisplayBuf,
                        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                        TEXT("%s failed with error %d: %s"),
                        functionName, dw, lpMsgBuf);
        (*pos) << (LPTSTR)lpDisplayBuf;

        LocalFree(lpMsgBuf);
        LocalFree(lpDisplayBuf);
    }
}
*/

void touchmind::util::_LastError(std::ios_base &os, LastErrorArgs args) {
  std::wostream *pos = dynamic_cast<std::wostream *>(&os);
  if (pos) {
    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = ::GetLastError();

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
                  dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, nullptr);

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
                                      (lstrlen((LPCTSTR)lpMsgBuf) + args.functionName.length() + 40) * sizeof(TCHAR));
    StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR), TEXT("%s failed with error %d: %s"),
                    args.functionName.c_str(), dw, lpMsgBuf);
    (*pos) << (LPTSTR)lpDisplayBuf;

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
  }
}
