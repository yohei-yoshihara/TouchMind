#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/view/linkedge/LinkEdgeViewFactory.h"
#include "touchmind/view/linkedge/impl/NormalLinkEdgeView.h"
#include "touchmind/view/linkedge/impl/ArrowLinkEdgeView.h"
#include "touchmind/view/linkedge/impl/CircleLinkEdgeView.h"

std::shared_ptr<touchmind::view::linkedge::BaseLinkEdgeView> touchmind::view::linkedge::LinkEdgeViewFactory::Create(
    std::shared_ptr<touchmind::model::link::LinkModel> link,
    EDGE_ID edgeId )
{
    std::shared_ptr<touchmind::view::linkedge::BaseLinkEdgeView> linkEdgeView = nullptr;
    switch (link->GetEdge(edgeId)->GetStyle()) {
    case EDGE_STYLE_NORMAL:
        linkEdgeView = std::make_shared<touchmind::view::linkedge::impl::NormalLinkEdgeView>(link, edgeId);
        break;
    case EDGE_STYLE_ARROW:
        linkEdgeView = std::make_shared<touchmind::view::linkedge::impl::ArrowLinkEdgeView>(link, edgeId);
        break;
    case EDGE_STYLE_CIRCLE:
        linkEdgeView = std::make_shared<touchmind::view::linkedge::impl::CircleLinkEdgeView>(link, edgeId);
        break;
    }
    linkEdgeView->SetRepaintCounter(link->GetEdge(edgeId));
    return linkEdgeView;
}
