#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/Context.h"
#include "touchmind/view/linkedge/BaseLinkEdgeView.h"
#include "touchmind/view/linkedge/LinkEdgeViewFactory.h"
#include "touchmind/view/link/LinkViewManager.h"
#include "touchmind/view/link/impl/DefaultLinkView.h"
#include "touchmind/model/CurvePoints.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/view/GeometryBuilder.h"
#include "touchmind/filter/GaussFilter.h"

void touchmind::view::link::impl::DefaultLinkView::CreateDeviceDependentResources(touchmind::Context *pContext,
                                                                                  ID2D1RenderTarget *pRenderTarget) {
  if (GetLinkModel().expired()) {
    return;
  }
  std::shared_ptr<touchmind::model::link::LinkModel> link = GetLinkModel().lock();
  if (!link->IsValid()) {
    LOG(SEVERITY_LEVEL_ERROR) << L"link is invalid, linkId = " << link->GetLinkId();
    return;
  }

  if (!GetLinkEdgeView(EDGE_ID_1)->IsCompatible(link->GetEdge(EDGE_ID_1))) {
    SetLinkEdgeView(EDGE_ID_1, touchmind::view::linkedge::LinkEdgeViewFactory::Create(link, EDGE_ID_1));
  }
  if (!GetLinkEdgeView(EDGE_ID_2)->IsCompatible(link->GetEdge(EDGE_ID_2))) {
    SetLinkEdgeView(EDGE_ID_2, touchmind::view::linkedge::LinkEdgeViewFactory::Create(link, EDGE_ID_2));
  }
  if (link->IsRepaintRequired(GetRepaintCounter()) || m_linkBrush == nullptr || m_linkGeometry == nullptr
      || m_linkStyle == nullptr || m_pBitmap == nullptr) {
    _CreateDeviceDependentResources(pContext, pRenderTarget, link);
    if (link->IsSelected()) {
      m_pBitmap = nullptr;
      _CreateTexture(pContext, pRenderTarget, link);
    } else {
      m_pBitmap = nullptr;
    }
    SetRepaintCounter(link);
  }
}

void touchmind::view::link::impl::DefaultLinkView::Draw(touchmind::Context *pContext,
                                                        ID2D1RenderTarget *pRenderTarget) {
  if (HasLinkModel()) {
    std::shared_ptr<touchmind::model::link::LinkModel> link = GetLinkModel().lock();
    CreateDeviceDependentResources(pContext, pRenderTarget);
    if (link->IsSelected()) {
      FLOAT shadowOffset = GetLinkViewManager()->GetGaussFilter()->GetOffset();
      D2D1_RECT_F bounds;
      m_linkGeometry->GetBounds(nullptr, &bounds);
      D2D1_SIZE_F size = m_pBitmap->GetSize();
      FLOAT left = bounds.left - shadowOffset;
      FLOAT top = bounds.top - shadowOffset;
      pRenderTarget->DrawBitmap(m_pBitmap, D2D1::RectF(left, top, left + size.width, top + size.height));
    }
    GetLinkEdgeView(EDGE_ID_1)->Fill(pContext, pRenderTarget);
    GetLinkEdgeView(EDGE_ID_2)->Fill(pContext, pRenderTarget);
    pRenderTarget->DrawGeometry(m_linkGeometry, m_linkBrush, link->GetLineWidthAsValue(), m_linkStyle);
  }
}

bool touchmind::view::link::impl::DefaultLinkView::HitTest(touchmind::Context *pContext,
                                                           ID2D1RenderTarget *pRenderTarget, D2D1_POINT_2F point) {
  bool result = false;
  if (HasLinkModel()) {
    CreateDeviceDependentResources(pContext, pRenderTarget);
    BOOL b;
    m_linkGeometry->StrokeContainsPoint(point, GetLinkModel().lock()->GetLineWidth() + 5.0f, nullptr, nullptr, &b);
    result = (b == TRUE);
  }
  return result;
}

void touchmind::view::link::impl::DefaultLinkView::_CreateDeviceDependentResources(
    touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
    std::shared_ptr<touchmind::model::link::LinkModel> link) {
  ID2D1Factory *pD2DFactory = pContext->GetD2DFactory();

  m_linkBrush = nullptr;
  CHK_RES(m_linkBrush,
          pRenderTarget->CreateSolidColorBrush(link->GetLineColor(), D2D1::BrushProperties(), &m_linkBrush));

  model::CurvePoints curvePoints;
  GeometryBuilder::CalculateLink(link, curvePoints);
  m_linkGeometry = nullptr;
  CHK_RES(m_linkGeometry, GeometryBuilder::CreateCurvePathGeometry(pD2DFactory, curvePoints, &m_linkGeometry));

  // style
  m_linkStyle = nullptr;
  switch (link->GetLineStyle()) {
  case LINE_STYLE_DASHED:
    CHK_RES(m_linkStyle, pD2DFactory->CreateStrokeStyle(
                             D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
                                                         D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_MITER,
                                                         link->GetLineWidthAsValue(), D2D1_DASH_STYLE_DASH, 0.0f),
                             nullptr, 0, &m_linkStyle));
    break;
  case LINE_STYLE_DOTTED:
    CHK_RES(m_linkStyle, pD2DFactory->CreateStrokeStyle(
                             D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
                                                         D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_MITER,
                                                         link->GetLineWidthAsValue(), D2D1_DASH_STYLE_DOT, 0.0f),
                             nullptr, 0, &m_linkStyle));
    break;
  case LINE_STYLE_SOLID:
  default:
    CHK_RES(m_linkStyle, pD2DFactory->CreateStrokeStyle(
                             D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
                                                         D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_MITER,
                                                         link->GetLineWidthAsValue(), D2D1_DASH_STYLE_SOLID, 0.0f),
                             nullptr, 0, &m_linkStyle));

    break;
  }
}

void touchmind::view::link::impl::DefaultLinkView::_CreateTexture(
    touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
    std::shared_ptr<touchmind::model::link::LinkModel> link) {
  FLOAT shadowOffset = GetLinkViewManager()->GetGaussFilter()->GetOffset();

  D2D1_RECT_F bounds;
  CHK_HR(m_linkGeometry->GetBounds(nullptr, &bounds));
  UINT width = static_cast<UINT>((bounds.right - bounds.left) + shadowOffset * 2);
  UINT height = static_cast<UINT>((bounds.bottom - bounds.top) + shadowOffset * 2);

  CComPtr<ID3D10Texture2D> pSourceTexutre2D = nullptr;
  CHK_RES(pSourceTexutre2D, pContext->CreateTexture2D(width, height, &pSourceTexutre2D));
  CComPtr<ID2D1RenderTarget> pSourceTexture2DRenderTarget = nullptr;
  CHK_RES(pSourceTexture2DRenderTarget,
          pContext->CreateD2DRenderTargetFromTexture2D(pSourceTexutre2D, &pSourceTexture2DRenderTarget));
  pSourceTexture2DRenderTarget->BeginDraw();
  pSourceTexture2DRenderTarget->Clear(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f));
  pSourceTexture2DRenderTarget->SetTransform(
      D2D1::Matrix3x2F::Translation(-bounds.left + shadowOffset, -bounds.top + shadowOffset));
  pSourceTexture2DRenderTarget->DrawGeometry(m_linkGeometry, GetLinkViewManager()->GetSelectedShadowBrush1(),
                                             link->GetLineWidthAsValue() + 2.0f);
  pSourceTexture2DRenderTarget->DrawGeometry(m_linkGeometry, GetLinkViewManager()->GetSelectedShadowBrush2(),
                                             link->GetLineWidthAsValue());
  // GetLinkEdgeView(EDGE_ID_1)->Fill(pContext, pRenderTarget, GetLinkViewManager()->GetSelectedShadowBrush1());
  // GetLinkEdgeView(EDGE_ID_1)->Draw(pContext, pRenderTarget, GetLinkViewManager()->GetSelectedShadowBrush2(), 2.0f);
  // GetLinkEdgeView(EDGE_ID_2)->Fill(pContext, pRenderTarget, GetLinkViewManager()->GetSelectedShadowBrush1());
  // GetLinkEdgeView(EDGE_ID_2)->Draw(pContext, pRenderTarget, GetLinkViewManager()->GetSelectedShadowBrush2(), 2.0f);
  CHK_HR(pSourceTexture2DRenderTarget->EndDraw());
  CComPtr<ID3D10Texture2D> pTexture2D = nullptr;
  CHK_RES(pTexture2D, GetLinkViewManager()->GetGaussFilter()->ApplyFilter(pContext, pSourceTexutre2D, &pTexture2D));
  m_pBitmap = nullptr;
  CHK_RES(m_pBitmap, pContext->CreateD2DSharedBitmapFromTexture2D(pRenderTarget, pTexture2D, &m_pBitmap));
}

void touchmind::view::link::impl::DefaultLinkView::DiscardDeviceDependentResources() {
  BaseLinkView::DiscardDeviceDependentResources();

  m_linkBrush = nullptr;
  m_linkGeometry = nullptr;
  m_linkStyle = nullptr;
  m_pBitmap = nullptr;
}
