#include "stdafx.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/view/path/BasePathView.h"
#include "touchmind/view/path/impl/DefaultPathView.h"
#include "touchmind/view/path/PathViewFactory.h"

std::shared_ptr<touchmind::view::path::BasePathView> touchmind::view::path::PathViewFactory::Create(
    const std::shared_ptr<model::node::NodeModel> &node)
{
    return std::make_shared<touchmind::view::path::impl::DefaultPathView>(node);
}
