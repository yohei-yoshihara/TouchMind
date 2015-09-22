#include "stdafx.h"
#include "touchmind/Context.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/view/link/BaseLinkView.h"
#include "touchmind/view/linkedge/BaseLinkEdgeView.h"
#include "touchmind/view/linkedge/impl/ArrowLinkEdgeView.h"
#include "touchmind/view/GeometryBuilder.h"

touchmind::view::linkedge::impl::ArrowLinkEdgeView::ArrowLinkEdgeView(
    std::weak_ptr<touchmind::model::link::LinkModel> link, EDGE_ID edgeId)
    : BaseLinkEdgeView(link, edgeId)
    , m_pArrowGeometry(nullptr)
    , m_pBrush(nullptr) {
}

touchmind::view::linkedge::impl::ArrowLinkEdgeView::~ArrowLinkEdgeView() {
}

void touchmind::view::linkedge::impl::ArrowLinkEdgeView::CreateDeviceDependentResources(
    touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget) {
  if (GetLinkModel().expired()) {
    return;
  }
  auto link = GetLinkModel().lock();
  if (!link->IsValid()) {
    return;
  }
  auto linkEdge = link->GetEdge(GetEdgeId());
  auto node = link->GetNode(GetEdgeId());
  if (linkEdge->IsRepaintRequired(GetRepaintCounter()) || m_pArrowGeometry == nullptr || m_pBrush == nullptr) {
    ID2D1Factory *pD2DFactory = pContext->GetD2DFactory();

    D2D1_POINT_2F point0 = link->GetEdgePoint(GetEdgeId());
    std::vector<D2D1_POINT_2F> points;
    view::GeometryBuilder::CalculateArrowPoints(point0, linkEdge->GetMarkerSize() + link->GetLineWidth(),
                                                linkEdge->GetMarkerSize() + link->GetLineWidth(), linkEdge->GetAngle(),
                                                node->GetAncestorPosition(), points);
    m_pArrowGeometry = nullptr;
    CHK_RES(m_pArrowGeometry,
            view::GeometryBuilder::CreatePathGeometryFromPoints(pD2DFactory, points, &m_pArrowGeometry));

    m_pBrush = nullptr;
    CHK_RES(m_pBrush, pRenderTarget->CreateSolidColorBrush(link->GetLineColor(), D2D1::BrushProperties(), &m_pBrush));
    SetRepaintCounter(linkEdge);
  }
}

void touchmind::view::linkedge::impl::ArrowLinkEdgeView::Draw(touchmind::Context *pContext,
                                                              ID2D1RenderTarget *pRenderTarget, ID2D1Brush *pBrush,
                                                              FLOAT width) {
  CreateDeviceDependentResources(pContext, pRenderTarget);
  pRenderTarget->DrawGeometry(m_pArrowGeometry, pBrush == nullptr ? m_pBrush : pBrush, width);
}

void touchmind::view::linkedge::impl::ArrowLinkEdgeView::Fill(touchmind::Context *pContext,
                                                              ID2D1RenderTarget *pRenderTarget, ID2D1Brush *pBrush) {
  CreateDeviceDependentResources(pContext, pRenderTarget);
  pRenderTarget->FillGeometry(m_pArrowGeometry, pBrush == nullptr ? m_pBrush : pBrush);
}

bool touchmind::view::linkedge::impl::ArrowLinkEdgeView::HitTest(touchmind::Context *pContext,
                                                                 ID2D1RenderTarget *pRenderTarget,
                                                                 D2D1_POINT_2F point) {
  UNREFERENCED_PARAMETER(pContext);
  UNREFERENCED_PARAMETER(pRenderTarget);
  UNREFERENCED_PARAMETER(point);

  BOOL contains = FALSE;
  m_pArrowGeometry->FillContainsPoint(point, nullptr, &contains);
  return contains == TRUE;
}

bool touchmind::view::linkedge::impl::ArrowLinkEdgeView::IsCompatible(
    const std::shared_ptr<touchmind::model::BaseModel> &model) {
  auto linkEdge = std::dynamic_pointer_cast<touchmind::model::linkedge::LinkEdgeModel>(model);
  if (linkEdge != nullptr) {
    return linkEdge->GetStyle() == EDGE_STYLE_ARROW;
  }
  return false;
}

void touchmind::view::linkedge::impl::ArrowLinkEdgeView::DiscardDeviceDependentResources() {
  m_pArrowGeometry = nullptr;
  m_pBrush = nullptr;
}
