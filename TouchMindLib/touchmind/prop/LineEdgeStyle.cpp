#include "stdafx.h"
#include "touchmind/prop/LineEdgeStyle.h"

std::wostream &touchmind::operator<<(std::wostream &os, const EDGE_ID &edgeId) {
  switch (edgeId) {
  case EDGE_ID_1:
    os << L"EDGE_ID_1";
    break;
  case EDGE_ID_2:
    os << L"EDGE_ID_2";
    break;
  }
  return os;
}

std::wostream &touchmind::operator<<(std::wostream &os, const touchmind::EDGE_STYLE &edgeStyle) {
  switch (edgeStyle) {
  case EDGE_STYLE_NORMAL:
    os << L"NORMAL";
    break;
  case EDGE_STYLE_ARROW:
    os << L"ARROW";
    break;
  case EDGE_STYLE_CIRCLE:
    os << L"CIRCLE";
    break;
  }
  return os;
}

std::wostream &touchmind::operator<<(std::wostream &os, const LINE_EDGE_STYLE &lineEdgeStyle) {
  switch (lineEdgeStyle) {
  case LINE_EDGE_STYLE_UNSPECIFIED:
    os << L"LINE_EDGE_STYLE_UNSPECIFIED";
    break;
  case LINE_EDGE_STYLE_NORMAL:
    os << L"LINE_EDGE_STYLE_NORMAL";
    break;
  case LINE_EDGE_STYLE_EDGE1_ARROW:
    os << L"LINE_EDGE_STYLE_EDGE1_ARROW";
    break;
  case LINE_EDGE_STYLE_EDGE2_ARROW:
    os << L"LINE_EDGE_STYLE_EDGE2_ARROW";
    break;
  case LINE_EDGE_STYLE_BOTH_ARROW:
    os << L"LINE_EDGE_STYLE_BOTH_ARROW";
    break;
  case LINE_EDGE_STYLE_EDGE1_CIRCLE:
    os << L"LINE_EDGE_STYLE_EDGE1_CIRCLE";
    break;
  case LINE_EDGE_STYLE_EDGE2_CIRCLE:
    os << L"LINE_EDGE_STYLE_EDGE2_CIRCLE";
    break;
  case LINE_EDGE_STYLE_BOTH_CIRCLE:
    os << L"LINE_EDGE_STYLE_BOTH_CIRCLE";
    break;
  case LINE_EDGE_STYLE_CIRCLE_ARROW:
    os << L"LINE_EDGE_STYLE_CIRCLE_ARROW";
    break;
  case LINE_EDGE_STYLE_ARROW_CIRCLE:
    os << L"LINE_EDGE_STYLE_ARROW_CIRCLE";
    break;
  }
  return os;
}
