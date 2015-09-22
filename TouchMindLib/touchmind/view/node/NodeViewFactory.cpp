#include "stdafx.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/view/node/BaseNodeView.h"
#include "touchmind/view/node/impl/RectangleNodeView.h"
#include "touchmind/view/node/impl/RoundedRectNodeView.h"
#include "touchmind/view/node/NodeViewFactory.h"

std::shared_ptr<touchmind::view::node::BaseNodeView>
touchmind::view::node::NodeViewFactory::Create(const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  std::shared_ptr<touchmind::view::node::BaseNodeView> nodeView;
  switch (node->GetNodeShape()) {
  case NODE_SHAPE_RECTANGLE:
    nodeView = std::make_shared<impl::RectangleNodeView>(node);
    break;
  case NODE_SHAPE_ROUNDED_RECTANGLE:
    nodeView = std::make_shared<impl::RoundedRectNodeView>(node);
    break;
  }
  return nodeView;
}
