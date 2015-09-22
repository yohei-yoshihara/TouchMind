#include "stdafx.h"
#include "touchmind/view/path/BasePathView.h"

touchmind::view::path::BasePathView::BasePathView(std::weak_ptr<touchmind::model::node::NodeModel> node)
    : m_node(node)
    , m_pNodeViewManager(nullptr)
    , m_pConfiguration(nullptr) {
}

touchmind::view::path::BasePathView::~BasePathView() {
}
