#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"

const FLOAT touchmind::model::linkedge::LinkEdgeModel::INITIAL_LENGTH = 130.0f;

touchmind::model::linkedge::LinkEdgeModel::LinkEdgeModel()
    : BaseModel()
    , m_link(nullptr)
    , m_angle(0.0f)
    , m_length(INITIAL_LENGTH)
    , m_style(EDGE_STYLE_NORMAL) {
}

touchmind::model::linkedge::LinkEdgeModel::LinkEdgeModel(const LinkEdgeModel &other)
    : BaseModel(other)
    , m_link(other.m_link)
    , m_angle(other.m_angle)
    , m_length(other.m_length)
    , m_style(other.m_style) {
}

void touchmind::model::linkedge::LinkEdgeModel::CalculateAngleAndLength(D2D1_POINT_2F nodePos,
                                                                        D2D1_POINT_2F handlePos) {
  D2D1_POINT_2F p = {std::abs(handlePos.x - nodePos.x), -(handlePos.y - nodePos.y)};
  m_angle = std::atan(p.y / p.x);
  m_length = std::sqrt(p.x * p.x + p.y * p.y);
}

D2D1_POINT_2F touchmind::model::linkedge::LinkEdgeModel::CalculateHandlePosition(D2D1_POINT_2F nodePos,
                                                                                 touchmind::NODE_SIDE nodeSide) const {
  LOG_ENTER_ARG(L"nodePos = " << nodePos << L", nodeSide = " << nodeSide);
  D2D1_POINT_2F p = {std::cos(m_angle) * m_length, -std::sin(m_angle) * m_length};
  if (nodeSide == NODE_SIDE_LEFT) {
    p.x = -p.x;
  }
  p.x += nodePos.x;
  p.y += nodePos.y;
  LOG_LEAVE_ARG(L"p = " << p);
  return p;
}

void touchmind::model::linkedge::LinkEdgeModel::IncreamentParentLinkRepaintRequiredCounter() {
  m_link->IncrementRepaintRequiredCounter();
}

void touchmind::model::linkedge::LinkEdgeModel::IncreamentParentLinkSaveRequiredCounter() {
  m_link->IncrementSaveRequiredCounter();
}

std::wostream &touchmind::model::linkedge::operator<<(std::wostream &os,
                                                      const touchmind::model::linkedge::LinkEdgeModel &handle) {
  os << L"LinkEdgeHandleModel[angle=" << handle.GetAngle() << L",length=" << handle.GetLength() << L"]";
  return os;
}
