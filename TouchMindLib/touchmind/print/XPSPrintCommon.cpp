#include "stdafx.h"
#include "XPSPrintCommon.h"

std::wostream &operator<<(std::wostream &os, const DEVMODE &devMode) {
  os << L"DEVMODE[DeviceName=" << devMode.dmDeviceName;
  os << L",SpecVersion=" << devMode.dmSpecVersion;
  os << L",DriverVersion=" << devMode.dmDriverVersion;
  os << L",Size=" << devMode.dmSize;
  os << L",DriverExtra=" << devMode.dmDriverExtra;
  os << L",Fields=" << devMode.dmFields;
  os << L",Orientation=" << devMode.dmOrientation;
  os << L",PaperSize=" << devMode.dmPaperSize;
  os << L",PaperLength=" << devMode.dmPaperLength;
  os << L",PaperWidth=" << devMode.dmPaperWidth;
  os << L",Scale=" << devMode.dmScale;
  os << L",Copies=" << devMode.dmCopies;
  os << L",DefaultSource=" << devMode.dmDefaultSource;
  os << L",PrintQuality=" << devMode.dmPrintQuality;
  os << L",Color=" << devMode.dmColor;
  os << L",Duplex=" << devMode.dmDuplex;
  os << L",YResolution=" << devMode.dmYResolution;
  os << L",TTOption=" << devMode.dmTTOption;
  os << L",Collate=" << devMode.dmCollate;
  os << L",FormName=" << devMode.dmFormName;
  os << L",LogPixels=" << devMode.dmLogPixels;
  os << L",BitsPerPel=" << devMode.dmBitsPerPel;
  os << L",PelsWidth=" << devMode.dmPelsWidth;
  os << L",PelsHeight=" << devMode.dmPelsHeight;
  os << L",DisplayFlags=" << devMode.dmDisplayFlags;
  os << L",Nup=" << devMode.dmNup;
  os << L",DisplayFrequency=" << devMode.dmDisplayFrequency;
  os << L",ICMMethod=" << devMode.dmICMMethod;
  os << L",ICMIntent=" << devMode.dmICMIntent;
  os << L",MediaType=" << devMode.dmMediaType;
  os << L",DitherType=" << devMode.dmDitherType;
  os << L",PanningWidth=" << devMode.dmPanningWidth;
  os << L",PanningHeight=" << devMode.dmPanningHeight;
  os << L"]";
  return os;
}

std::wostream &operator<<(std::wostream &os, const PRINTPAGERANGE &pageRange) {
  os << L"PRINTPAGERANGE[nFromPage=" << pageRange.nFromPage;
  os << L",nToPage=" << pageRange.nToPage;
  os << L"]";
  return os;
}

std::wostream &operator<<(std::wostream &os, const PRINTDLGEX &pdx) {
  os << L"PRINTDLGEX[lStructSize=" << pdx.lStructSize;
  os << L",hwndOwner=" << pdx.hwndOwner;
  os << L",hDC=" << pdx.hDC;
  os << L",Flags=" << pdx.Flags;
  if (pdx.Flags & PD_PAGENUMS) {
    os << L"(PD_PAGENUMS)";
  } else if (pdx.Flags & PD_SELECTION) {
    os << L"(PD_SELECTION)";
  } else if (pdx.Flags & PD_CURRENTPAGE) {
    os << L"(PD_CURRENTPAGE)";
  } else {
    os << L"(PD_ALLPAGES)";
  }
  os << L",Flags2=" << pdx.Flags2;
  os << L",ExclusionFlags=" << pdx.ExclusionFlags;
  os << L",nPageRanges=" << pdx.nPageRanges;
  os << L",nMaxPageRanges=" << pdx.nMaxPageRanges;
  for (size_t i = 0; i < pdx.nPageRanges; ++i) {
    os << L",PageRanges[" << i << L"]=" << pdx.lpPageRanges[i];
  }
  os << L",nMinPage=" << pdx.nMinPage;
  os << L",nMaxPage=" << pdx.nMaxPage;
  os << L",nCopies=" << pdx.nCopies;
  os << L",hInstance=" << pdx.hInstance;
  os << L",dwResultAction=" << pdx.dwResultAction;
  os << L"]";
  return os;
}
