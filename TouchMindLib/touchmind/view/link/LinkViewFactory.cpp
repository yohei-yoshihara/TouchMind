#include "stdafx.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/view/link/BaseLinkView.h"
#include "touchmind/view/link/LinkViewFactory.h"
#include "touchmind/view/link/impl/DefaultLinkView.h"

std::shared_ptr<touchmind::view::link::BaseLinkView>
touchmind::view::link::LinkViewFactory::Create(const std::shared_ptr<touchmind::model::link::LinkModel> &link) {
  return std::make_shared<touchmind::view::link::impl::DefaultLinkView>(link);
}
