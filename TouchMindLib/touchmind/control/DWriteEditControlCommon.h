#ifndef TOUCHMIND_CONTROL_DWRITEEDITCONTROLCOMMON_H_
#define TOUCHMIND_CONTROL_DWRITEEDITCONTROLCOMMON_H_

#include "forwarddecl.h"

namespace touchmind {
  namespace control {
    typedef std::shared_ptr<control::DWriteEditControl> DWriteEditControlPtr;
    typedef int EDIT_CONTROL_INDEX;
    typedef std::map<EDIT_CONTROL_INDEX, std::shared_ptr<control::DWriteEditControl>>
        EditControlIndexToEditControlPtrMap;
    typedef std::pair<EDIT_CONTROL_INDEX, std::shared_ptr<control::DWriteEditControl>>
        EditControlIndexToEditControlPtrPair;

  } // control
} // touchmind

inline std::wostream &operator<<(std::wostream &os, TF_DA_COLOR const &tfDaColor) {
  switch (tfDaColor.type) {
  case TF_CT_NONE:
    os << L"TF_CT_NONE";
    break;
  case TF_CT_SYSCOLOR:
    os << L"TF_CT_SYSCOLOR(index=" << tfDaColor.nIndex << L")";
    break;
  case TF_CT_COLORREF:
    os << L"TF_CT_COLORREF(cr=" << std::hex << tfDaColor.cr << L")";
    break;
  default:
    os << L"UNKNOWN(" << static_cast<int>(tfDaColor.type) << L")";
  }
  return os;
}

inline std::wostream &operator<<(std::wostream &os, TF_DA_LINESTYLE const &tfDaLinestyle) {
  switch (tfDaLinestyle) {
  case TF_LS_NONE:
    os << L"TF_LS_NONE";
    break;
  case TF_LS_SOLID:
    os << L"TF_LS_SOLID";
    break;
  case TF_LS_DOT:
    os << L"TF_LS_DOT";
    break;
  case TF_LS_DASH:
    os << L"TF_LS_DASH";
    break;
  case TF_LS_SQUIGGLE:
    os << L"TF_LS_SQUIGGLE";
    break;
  default:
    os << L"UNKNOWN(" << static_cast<int>(tfDaLinestyle) << L")";
  }
  return os;
}

inline std::wostream &operator<<(std::wostream &os, TF_DA_ATTR_INFO const &tfDaAttrInfo) {
  switch (tfDaAttrInfo) {
  case TF_ATTR_INPUT:
    os << L"TF_ATTR_INPUT";
    break;
  case TF_ATTR_TARGET_CONVERTED:
    os << L"TF_ATTR_TARGET_CONVERTED";
    break;
  case TF_ATTR_CONVERTED:
    os << L"TF_ATTR_CONVERTED";
    break;
  case TF_ATTR_TARGET_NOTCONVERTED:
    os << L"TF_ATTR_TARGET_NOTCONVERTED";
    break;
  case TF_ATTR_INPUT_ERROR:
    os << L"TF_ATTR_INPUT_ERROR";
    break;
  case TF_ATTR_FIXEDCONVERTED:
    os << L"TF_ATTR_FIXEDCONVERTED";
    break;
  case TF_ATTR_OTHER:
    os << L"TF_ATTR_OTHER";
    break;
  default:
    os << L"UNKNOWN(" << static_cast<int>(tfDaAttrInfo) << L")";
  }
  return os;
}

inline std::wostream &operator<<(std::wostream &os, TF_DISPLAYATTRIBUTE const &dispAttr) {
  os << L"crText=" << dispAttr.crText;
  os << L",crBk=" << dispAttr.crBk;
  os << L",lsStyle=" << dispAttr.lsStyle;
  os << L",fBoldLine=" << std::boolalpha << (dispAttr.fBoldLine == TRUE);
  os << L",crLine=" << dispAttr.crLine;
  os << L",bAttr=" << dispAttr.bAttr;
  return os;
}

#endif // TOUCHMIND_CONTROL_DWRITEEDITCONTROLCOMMON_H_
