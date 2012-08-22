#include "stdafx.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/view/link/BaseLinkView.h"
#include "touchmind/view/linkedge/LinkEdgeViewFactory.h"
#include "touchmind/view/linkedge/BaseLinkEdgeView.h"

touchmind::view::link::BaseLinkView::BaseLinkView(
    const std::weak_ptr<touchmind::model::link::LinkModel> &link) :
    m_link(link),
    m_pLinkViewManager(nullptr),
    m_linkEdgeViews()
{
    m_linkEdgeViews[0] = touchmind::view::linkedge::LinkEdgeViewFactory::Create(link.lock(), EDGE_ID_1);
    m_linkEdgeViews[1] = touchmind::view::linkedge::LinkEdgeViewFactory::Create(link.lock(), EDGE_ID_2);
}

touchmind::view::link::BaseLinkView::~BaseLinkView()
{
}

void touchmind::view::link::BaseLinkView::DiscardDeviceDependentResources()
{
    m_linkEdgeViews[0]->DiscardDeviceDependentResources();
    m_linkEdgeViews[1]->DiscardDeviceDependentResources();
}
