#include "stdafx.h"
#include "touchmind/view/node/BaseNodeView.h"

touchmind::view::node::BaseNodeView::BaseNodeView(std::weak_ptr<touchmind::model::node::NodeModel> node)
    : m_node(node)
    , m_pNodeViewManager(nullptr)
    , m_pEditControlManager(nullptr)
    , m_editControlIndex(-1) {
}

touchmind::view::node::BaseNodeView::~BaseNodeView() {
}

void touchmind::view::node::BaseNodeView::DiscardDeviceDependentResources() {
}
