#include "stdafx.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/path/PathModel.h"

void touchmind::model::path::PathModel::SetNodeModel(std::weak_ptr<touchmind::model::node::NodeModel> node) {
  if ((!m_node.expired() && !node.expired() && m_node.lock() == node.lock()) || (m_node.expired() && node.expired())) {
    return;
  }
  m_node = node;
  IncrementRepaintRequiredCounter();
  IncrementSaveRequiredCounter();
}

std::shared_ptr<touchmind::model::node::NodeModel> touchmind::model::path::PathModel::GetNodeModel() const {
  if (!m_node.expired()) {
    return m_node.lock();
  } else {
    return nullptr;
  }
}

std::shared_ptr<touchmind::selection::SelectableSupport> touchmind::model::path::PathModel::GetSelectableSharedPtr() {
  return std::dynamic_pointer_cast<touchmind::selection::SelectableSupport>(shared_from_this());
}

FLOAT touchmind::model::path::PathModel::GetWidthAsValue() const {
  switch (m_lineWidth) {
  case LINE_WIDTH_1:
    return 1.0f;
  case LINE_WIDTH_3:
    return 3.0f;
  case LINE_WIDTH_5:
    return 5.0f;
  case LINE_WIDTH_8:
    return 8.0f;
  default:
    return 1.0f;
  }
}
