#include "stdafx.h"
#include "CurvePoints.h"

std::wostream &touchmind::model::operator<<(std::wostream &os, const CurvePoints &curvePoints) {
  os << L"CurvePoints[";
  for (size_t i = 0; i < curvePoints.GetNumerOfPoints(); ++i) {
    os << L"(" << curvePoints.GetX(i) << L"," << curvePoints.GetY(i) << L")";
    if (i != curvePoints.GetNumerOfPoints() - 1) {
      os << L",";
    }
  }
  os << L"]";
  return os;
}
