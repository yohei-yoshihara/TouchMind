#include "StdAfx.h"
#include "touchmind/logging/Logging.h"
#include "DWriteTextSizeProvider.h"
#include "DWriteEditControlCommon.h"
#include "DWriteEditControlManager.h"
#include "DWriteEditControl.h"

touchmind::control::DWriteTextSizeProvider::DWriteTextSizeProvider(void)
    : m_pNodeViewManager(nullptr)
    , m_pEditControlManager(nullptr) {
}

touchmind::control::DWriteTextSizeProvider::~DWriteTextSizeProvider(void) {
}

void touchmind::control::DWriteTextSizeProvider::Calculate(
    IN const std::shared_ptr<touchmind::model::node::NodeModel> &node, OUT FLOAT *pWidth, OUT FLOAT *pHeight) {
  int editControlIndex = m_pNodeViewManager->GetEditControlIndexFromNodeId(node->GetId());
  if (editControlIndex > -1) {
    auto editControl = m_pEditControlManager->GetEditControl(editControlIndex);
    if (editControl != nullptr) {
      D2D1_RECT_F actualRect = editControl->GetActualRect();
      *pWidth = actualRect.right - actualRect.left;
      *pHeight = actualRect.bottom - actualRect.top;
      return;
    }
  }

  if (node->GetWidth() > 0 && node->GetHeight() > 0) {
    *pWidth = node->GetWidth();
    *pHeight = node->GetHeight();
    return;
  }

  *pWidth = touchmind::model::node::NodeModel::DEFAULT_WIDTH;
  *pHeight = touchmind::model::node::NodeModel::DEFAULT_HEIGHT;
}
