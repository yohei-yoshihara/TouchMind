#include "stdafx.h"
#include "touchmind/prop/NodeShape.h"

std::wostream &touchmind::operator<<(std::wostream &os, const touchmind::NODE_SHAPE &nodeShape) {
  switch (nodeShape) {
  case NODE_SHAPE_RECTANGLE:
    os << L"NODE_SHAPE_RECTANGLE";
    break;
  case NODE_SHAPE_ROUNDED_RECTANGLE:
    os << L"NODE_SHAPE_ROUNDED_RECTANGLE";
    break;
  }
  return os;
}
