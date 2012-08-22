#ifndef LIBS_H_
#define LIBS_H_

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d10_1.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "propsys.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "xpsprint.lib")
#pragma comment(lib, "prntvpt.lib")

#ifdef _M_X64

#ifdef _DEBUG
#pragma comment(lib, "../x64/Debug/TouchMindLib.lib")
#else
#pragma comment(lib, "../x64/Release/TouchMindLib.lib")
#endif

#else

#ifdef _DEBUG
#pragma comment(lib, "../Debug/TouchMindLib.lib")
#else
#pragma comment(lib, "../Release/TouchMindLib.lib")
#endif

#endif

#endif // LIBS_H_