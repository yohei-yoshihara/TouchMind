#include "stdafx.h"
#include "touchmind/view/linkedge/BaseLinkEdgeView.h"

touchmind::view::linkedge::BaseLinkEdgeView::BaseLinkEdgeView(
    std::weak_ptr<touchmind::model::link::LinkModel> link,
    EDGE_ID edgeId) :
    m_link(link),
    m_edgeId(edgeId)
{
}

touchmind::view::linkedge::BaseLinkEdgeView::~BaseLinkEdgeView()
{
}
