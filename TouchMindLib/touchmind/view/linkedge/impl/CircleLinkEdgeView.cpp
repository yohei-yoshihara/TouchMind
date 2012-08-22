#include "stdafx.h"
#include "touchmind/Context.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/view/link/BaseLinkView.h"
#include "touchmind/view/linkedge/BaseLinkEdgeView.h"
#include "touchmind/view/linkedge/impl/CircleLinkEdgeView.h"

touchmind::view::linkedge::impl::CircleLinkEdgeView::CircleLinkEdgeView(
    std::weak_ptr<touchmind::model::link::LinkModel> link,
    EDGE_ID edgeId) :
    BaseLinkEdgeView(link, edgeId),
    m_pCircleGeometry(nullptr),
    m_pBrush(nullptr)
{
}

touchmind::view::linkedge::impl::CircleLinkEdgeView::~CircleLinkEdgeView()
{
}

void touchmind::view::linkedge::impl::CircleLinkEdgeView::CreateDeviceDependentResources(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget)
{
    if (GetLinkModel().expired()) {
        return;
    }
    auto link = GetLinkModel().lock();
    auto linkEdge = link->GetEdge(GetEdgeId());
    if (linkEdge->IsRepaintRequired(GetRepaintCounter()) ||
            m_pCircleGeometry == nullptr ||
            m_pBrush == nullptr) {
        ID2D1Factory *pD2DFactory = pContext->GetD2DFactory();

        D2D1_POINT_2F point0 = link->GetEdgePoint(GetEdgeId());
        FLOAT markerSize = linkEdge->GetMarkerSize() + link->GetLineWidth();
        FLOAT width = 0.0f;
        if (link->GetNode(GetEdgeId())->GetAncestorPosition() == NODE_SIDE_RIGHT) {
            width = markerSize / 2.0f;
        } else {
            width = -markerSize / 2.0f;
        }
        point0.x += width;
        m_pCircleGeometry = nullptr;
        CHK_RES(m_pCircleGeometry, pD2DFactory->CreateEllipseGeometry(
                    D2D1::Ellipse(point0, markerSize / 2.0f, markerSize / 2.0f),
                    &m_pCircleGeometry));

        m_pBrush = nullptr;
        CHK_RES(m_pBrush,
                pRenderTarget->CreateSolidColorBrush(
                    link->GetLineColor(),
                    D2D1::BrushProperties(),
                    &m_pBrush));
        SetRepaintCounter(linkEdge);
    }
}

void touchmind::view::linkedge::impl::CircleLinkEdgeView::Draw(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget,
    ID2D1Brush *pBrush,
    FLOAT width)
{
    CreateDeviceDependentResources(pContext, pRenderTarget);
    pRenderTarget->DrawGeometry(m_pCircleGeometry, pBrush == nullptr ? m_pBrush : pBrush, width);
}

void touchmind::view::linkedge::impl::CircleLinkEdgeView::Fill(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget,
    ID2D1Brush *pBrush)
{
    CreateDeviceDependentResources(pContext, pRenderTarget);
    pRenderTarget->FillGeometry(m_pCircleGeometry, pBrush == nullptr ? m_pBrush : pBrush);
}

bool touchmind::view::linkedge::impl::CircleLinkEdgeView::HitTest(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget,
    D2D1_POINT_2F point)
{
    UNREFERENCED_PARAMETER(pContext);
    UNREFERENCED_PARAMETER(pRenderTarget);
    UNREFERENCED_PARAMETER(point);

    BOOL contains = FALSE;
    m_pCircleGeometry->FillContainsPoint(point, nullptr, &contains);
    return contains == TRUE;
}

bool touchmind::view::linkedge::impl::CircleLinkEdgeView::IsCompatible(
    const std::shared_ptr<touchmind::model::BaseModel> &model)
{
    auto linkEdge = std::dynamic_pointer_cast<touchmind::model::linkedge::LinkEdgeModel>(model);
    if (linkEdge != nullptr) {
        return linkEdge->GetStyle() == EDGE_STYLE_CIRCLE;
    }
    return false;
}

void touchmind::view::linkedge::impl::CircleLinkEdgeView::DiscardDeviceDependentResources()
{
    m_pCircleGeometry = nullptr;
    m_pBrush = nullptr;
}
