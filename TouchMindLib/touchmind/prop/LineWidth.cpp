#include "stdafx.h"
#include "touchmind/prop/LineWidth.h"

std::wostream &touchmind::operator<<(std::wostream &os, const touchmind::LINE_WIDTH &lineWidth) {
  switch (lineWidth) {
  case LINE_WIDTH_UNSPECIFIED:
    os << L"UNSPECIFIED";
    break;
  case LINE_WIDTH_1:
    os << L"1";
    break;
  case LINE_WIDTH_3:
    os << L"3";
    break;
  case LINE_WIDTH_5:
    os << L"5";
    break;
  case LINE_WIDTH_8:
    os << L"8";
    break;
  }
  return os;
}
