#include "stdafx.h"
#include "touchmind/Context.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/view/linkedge/impl/NormalLinkEdgeView.h"

touchmind::view::linkedge::impl::NormalLinkEdgeView::NormalLinkEdgeView(
    std::weak_ptr<touchmind::model::link::LinkModel> link,
    EDGE_ID edgeId) :
    BaseLinkEdgeView(link, edgeId)
{
}

void touchmind::view::linkedge::impl::NormalLinkEdgeView::CreateDeviceDependentResources(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget)
{
    UNREFERENCED_PARAMETER(pContext);
    UNREFERENCED_PARAMETER(pRenderTarget);
}

void touchmind::view::linkedge::impl::NormalLinkEdgeView::Draw(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget,
    ID2D1Brush *pBrush,
    FLOAT width)
{
    UNREFERENCED_PARAMETER(pContext);
    UNREFERENCED_PARAMETER(pRenderTarget);
    UNREFERENCED_PARAMETER(pBrush);
    UNREFERENCED_PARAMETER(width);
}

void touchmind::view::linkedge::impl::NormalLinkEdgeView::Fill(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget,
    ID2D1Brush *pBrush)
{
    UNREFERENCED_PARAMETER(pContext);
    UNREFERENCED_PARAMETER(pRenderTarget);
    UNREFERENCED_PARAMETER(pBrush);
}

bool touchmind::view::linkedge::impl::NormalLinkEdgeView::HitTest(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget,
    D2D1_POINT_2F point)
{
    UNREFERENCED_PARAMETER(pContext);
    UNREFERENCED_PARAMETER(pRenderTarget);
    UNREFERENCED_PARAMETER(point);
    return false;
}

bool touchmind::view::linkedge::impl::NormalLinkEdgeView::IsCompatible(
    const std::shared_ptr<touchmind::model::BaseModel> &model)
{
    auto linkEdge = std::dynamic_pointer_cast<touchmind::model::linkedge::LinkEdgeModel>(model);
    if (linkEdge != nullptr) {
        return linkEdge->GetStyle() == EDGE_STYLE_NORMAL;
    }
    return false;
}

void touchmind::view::linkedge::impl::NormalLinkEdgeView::DiscardDeviceDependentResources()
{
}
