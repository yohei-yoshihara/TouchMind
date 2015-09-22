#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/Configuration.h"
#include "touchmind/model/CurvePoints.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/view/GeometryBuilder.h"

std::array<bool, 9> touchmind::view::GeometryBuilder::s_MATRIX_TOP_LEFT
    = {true, true, true, true, true, false, true, false, false};

std::array<bool, 9> touchmind::view::GeometryBuilder::s_MATRIX_TOP_RIGHT
    = {true, true, true, false, true, true, false, false, true};

std::array<bool, 9> touchmind::view::GeometryBuilder::s_MATRIX_BOTTOM_LEFT
    = {true, false, false, true, true, false, true, true, true};

std::array<bool, 9> touchmind::view::GeometryBuilder::s_MATRIX_BOTTOM_RIGHT
    = {false, false, true, false, true, true, true, true, true};

void touchmind::view::GeometryBuilder::_CalculateCollapsedMarkSize(IN const D2D1_RECT_F &rect, IN FLOAT r,
                                                                   IN bool isRightSideNode, OUT D2D1_POINT_2F &p1,
                                                                   OUT D2D1_POINT_2F &p2, OUT FLOAT &actualR) {
  FLOAT x = isRightSideNode ? rect.right : rect.left;
  FLOAT center_y = rect.top + (rect.bottom - rect.top) / 2.0f;
  p1 = D2D1::Point2F(x, center_y - r);
  p2 = D2D1::Point2F(x, center_y + r);
  p1.y = std::max(p1.y, rect.top + r);
  p2.y = std::min(p2.y, rect.bottom - r);
  actualR = (p2.y - p1.y) / 2.0f;
  if (!isRightSideNode) {
    std::swap(p1, p2);
  }
}

HRESULT touchmind::view::GeometryBuilder::CreateCollapsedMarkGeometry(IN ID2D1Factory *pD2DFactory,
                                                                      IN const D2D1_RECT_F &rect, IN FLOAT r,
                                                                      IN bool isRightSideNode,
                                                                      OUT ID2D1PathGeometry **ppPathGeometry) {
  D2D1_POINT_2F p1 = {0.0f, 0.0f};
  D2D1_POINT_2F p2 = {0.0f, 0.0f};
  FLOAT rr = 0.0f;
  _CalculateCollapsedMarkSize(rect, r, isRightSideNode, p1, p2, rr);

  CComPtr<ID2D1GeometrySink> pSink = nullptr;
  CHK_HR(pD2DFactory->CreatePathGeometry(ppPathGeometry));
  CHK_RES(pSink, (*ppPathGeometry)->Open(&pSink));
  pSink->BeginFigure(p1, D2D1_FIGURE_BEGIN_FILLED);
  pSink->AddArc(
      D2D1::ArcSegment(p2, D2D1::SizeF(rr, rr), touchmind::PI, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
  pSink->AddLine(p2);
  pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
  CHK_HR(pSink->Close());
  return S_OK;
}

HRESULT touchmind::view::GeometryBuilder::CreateRoundedRectangleGeometry(IN ID2D1Factory *pD2DFactory,
                                                                         IN const D2D1_RECT_F &rect, IN FLOAT r,
                                                                         IN bool isCollapsed, IN bool isRightSideNode,
                                                                         OUT ID2D1PathGeometry **ppPathGeometry) {
  CHK_HR(pD2DFactory->CreatePathGeometry(ppPathGeometry));
  CComPtr<ID2D1GeometrySink> pSink = nullptr;
  CHK_RES(pSink, (*ppPathGeometry)->Open(&pSink));
  pSink->BeginFigure(D2D1::Point2F(rect.left, rect.top + r), D2D1_FIGURE_BEGIN_FILLED);
  pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(rect.left + r, rect.top), D2D1::SizeF(r, r), touchmind::PI / 2.0f,
                                 D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
  pSink->AddLine(D2D1::Point2F(rect.right - r, rect.top));
  pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(rect.right, rect.top + r), D2D1::SizeF(r, r), touchmind::PI / 2.0f,
                                 D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));

  if (isCollapsed && isRightSideNode) {
    D2D1_POINT_2F p1 = {0.0f, 0.0f};
    D2D1_POINT_2F p2 = {0.0f, 0.0f};
    FLOAT rr = 0.0f;
    _CalculateCollapsedMarkSize(rect, r, isRightSideNode, p1, p2, rr);
    pSink->AddLine(p1);
    pSink->AddArc(
        D2D1::ArcSegment(p2, D2D1::SizeF(rr, rr), touchmind::PI, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
  }

  pSink->AddLine(D2D1::Point2F(rect.right, rect.bottom - r));
  pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(rect.right - r, rect.bottom), D2D1::SizeF(r, r), touchmind::PI / 2.0f,
                                 D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
  pSink->AddLine(D2D1::Point2F(rect.left + r, rect.bottom));
  pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(rect.left, rect.bottom - r), D2D1::SizeF(r, r), touchmind::PI / 2.0f,
                                 D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
  if (isCollapsed && !isRightSideNode) {
    D2D1_POINT_2F p1 = {0.0f, 0.0f};
    D2D1_POINT_2F p2 = {0.0f, 0.0f};
    FLOAT rr = 0.0f;
    _CalculateCollapsedMarkSize(rect, r, isRightSideNode, p1, p2, rr);
    pSink->AddLine(p1);
    pSink->AddArc(
        D2D1::ArcSegment(p2, D2D1::SizeF(rr, rr), touchmind::PI, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
  }

  pSink->AddLine(D2D1::Point2F(rect.left, rect.top + r));
  pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
  CHK_HR(pSink->Close());
  return S_OK;
}

HRESULT touchmind::view::GeometryBuilder::CreateHalfRoundedRectangleGeometry(IN ID2D1Factory *pD2DFactory,
                                                                             IN const D2D1_RECT_F &rect, IN FLOAT r,
                                                                             OUT ID2D1PathGeometry **ppPathGeometry) {
  CHK_HR(pD2DFactory->CreatePathGeometry(ppPathGeometry));
  CComPtr<ID2D1GeometrySink> pSink = nullptr;
  CHK_RES(pSink, (*ppPathGeometry)->Open(&pSink));
  pSink->BeginFigure(D2D1::Point2F(rect.left, rect.bottom), D2D1_FIGURE_BEGIN_FILLED);
  pSink->AddLine(D2D1::Point2F(rect.left, rect.top + r));
  pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(rect.left + r, rect.top), D2D1::SizeF(r, r), (FLOAT)(PI / 2.0f),
                                 D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
  pSink->AddLine(D2D1::Point2F(rect.right - r, rect.top));
  pSink->AddArc(D2D1::ArcSegment(D2D1::Point2F(rect.right, rect.top + r), D2D1::SizeF(r, r), (FLOAT)(PI / 2.0f),
                                 D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
  pSink->AddLine(D2D1::Point2F(rect.right, rect.bottom));
  pSink->AddLine(D2D1::Point2F(rect.left, rect.bottom));
  pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
  CHK_HR(pSink->Close());
  return S_OK;
}

HRESULT touchmind::view::GeometryBuilder::CreateDiagonalGradientBrush(
    IN ID2D1RenderTarget *pRenderTarget, IN const D2D1_RECT_F &rect,
    IN const std::vector<D2D1_GRADIENT_STOP> &gradientStops, OUT ID2D1LinearGradientBrush **ppGradientBrush) {
  CComPtr<ID2D1GradientStopCollection> pGradientStopCollection = nullptr;
  CHK_RES(pGradientStopCollection, pRenderTarget->CreateGradientStopCollection(
                                       gradientStops.data(), static_cast<UINT>(gradientStops.size()), D2D1_GAMMA_1_0,
                                       D2D1_EXTEND_MODE_CLAMP, &pGradientStopCollection));
  CHK_HR(pRenderTarget->CreateLinearGradientBrush(
      D2D1::LinearGradientBrushProperties(D2D1::Point2F(rect.left, rect.top), D2D1::Point2F(rect.right, rect.bottom)),
      pGradientStopCollection, ppGradientBrush));
  return S_OK;
}

HRESULT touchmind::view::GeometryBuilder::CreateVerticalGradientBrush(
    IN ID2D1RenderTarget *pRenderTarget, IN const D2D1_RECT_F &rect,
    IN const std::vector<D2D1_GRADIENT_STOP> &gradientStops, OUT ID2D1LinearGradientBrush **ppGradientBrush) {
  CComPtr<ID2D1GradientStopCollection> pGradientStopCollection = nullptr;
  CHK_RES(pGradientStopCollection, pRenderTarget->CreateGradientStopCollection(
                                       gradientStops.data(), static_cast<UINT>(gradientStops.size()), D2D1_GAMMA_1_0,
                                       D2D1_EXTEND_MODE_CLAMP, &pGradientStopCollection));
  CHK_HR(pRenderTarget->CreateLinearGradientBrush(
      D2D1::LinearGradientBrushProperties(D2D1::Point2F(rect.left, rect.top), D2D1::Point2F(rect.left, rect.bottom)),
      pGradientStopCollection, ppGradientBrush));
  return S_OK;
}

HRESULT touchmind::view::GeometryBuilder::CreateIsoscelesTriangleGeometry(IN ID2D1Factory *pD2DFactory,
                                                                          IN const D2D1_RECT_F &rect,
                                                                          IN TRIANGLE_VERTEX tirangleVertex,
                                                                          OUT ID2D1PathGeometry **ppPathGeometry) {
  CHK_HR(pD2DFactory->CreatePathGeometry(ppPathGeometry));
  CComPtr<ID2D1GeometrySink> pSink = nullptr;
  CHK_RES(pSink, (*ppPathGeometry)->Open(&pSink));
  switch (tirangleVertex) {
  case TRIANGLE_VERTEX_TOP_LEFT:
    pSink->BeginFigure(D2D1::Point2F(rect.left, rect.top), D2D1_FIGURE_BEGIN_FILLED);
    pSink->AddLine(D2D1::Point2F(rect.right, rect.top));
    pSink->AddLine(D2D1::Point2F(rect.left, rect.bottom));
    pSink->AddLine(D2D1::Point2F(rect.left, rect.top));
    break;
  case TRIANGLE_VERTEX_TOP_RIGHT:
    pSink->BeginFigure(D2D1::Point2F(rect.left, rect.top), D2D1_FIGURE_BEGIN_FILLED);
    pSink->AddLine(D2D1::Point2F(rect.right, rect.top));
    pSink->AddLine(D2D1::Point2F(rect.right, rect.bottom));
    pSink->AddLine(D2D1::Point2F(rect.left, rect.top));
    break;
  case TRIANGLE_VERTEX_BOTTOM_LEFT:
    pSink->BeginFigure(D2D1::Point2F(rect.left, rect.bottom), D2D1_FIGURE_BEGIN_FILLED);
    pSink->AddLine(D2D1::Point2F(rect.left, rect.top));
    pSink->AddLine(D2D1::Point2F(rect.right, rect.bottom));
    pSink->AddLine(D2D1::Point2F(rect.left, rect.bottom));
    break;
  case TRIANGLE_VERTEX_BOTTOM_RIGHT:
    pSink->BeginFigure(D2D1::Point2F(rect.left, rect.bottom), D2D1_FIGURE_BEGIN_FILLED);
    pSink->AddLine(D2D1::Point2F(rect.right, rect.top));
    pSink->AddLine(D2D1::Point2F(rect.right, rect.bottom));
    pSink->AddLine(D2D1::Point2F(rect.left, rect.bottom));
    break;
  }
  pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
  CHK_HR(pSink->Close());
  return S_OK;
}

HRESULT touchmind::view::GeometryBuilder::CreateTriangleMoverGeometry(IN ID2D1Factory *pD2DFactory,
                                                                      IN const D2D1_RECT_F &rect,
                                                                      IN TRIANGLE_VERTEX tirangleVertex,
                                                                      OUT ID2D1PathGeometry **ppPathGeometry) {
  std::array<bool, 9> *matrix = nullptr;
  switch (tirangleVertex) {
  case TRIANGLE_VERTEX_TOP_LEFT:
    matrix = &s_MATRIX_TOP_LEFT;
    break;
  case TRIANGLE_VERTEX_TOP_RIGHT:
    matrix = &s_MATRIX_TOP_RIGHT;
    break;
  case TRIANGLE_VERTEX_BOTTOM_LEFT:
    matrix = &s_MATRIX_BOTTOM_LEFT;
    break;
  case TRIANGLE_VERTEX_BOTTOM_RIGHT:
    matrix = &s_MATRIX_BOTTOM_RIGHT;
    break;
  }

  CHK_HR(pD2DFactory->CreatePathGeometry(ppPathGeometry));
  CComPtr<ID2D1GeometrySink> pSink = nullptr;
  CHK_RES(pSink, (*ppPathGeometry)->Open(&pSink));

  FLOAT x0 = rect.left;
  FLOAT y0 = rect.top;
  FLOAT width = rect.right - rect.left;
  FLOAT height = rect.bottom - rect.top;

  FLOAT dx = width / 5.0f;
  FLOAT dy = height / 5.0f;

  for (int iy = 0; iy < 3; ++iy) {
    for (int ix = 0; ix < 3; ++ix) {
      if ((*matrix)[ix + iy * 3]) {
        FLOAT x = x0 + ix * dx * 2.0f;
        FLOAT y = y0 + iy * dy * 2.0f;
        pSink->BeginFigure(D2D1::Point2F(x, y), D2D1_FIGURE_BEGIN_FILLED);
        pSink->AddLine(D2D1::Point2F(x + dx, y));
        pSink->AddLine(D2D1::Point2F(x + dx, y + dy));
        pSink->AddLine(D2D1::Point2F(x, y + dy));
        pSink->AddLine(D2D1::Point2F(x, y));
        pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
      }
    }
  }
  CHK_HR(pSink->Close());
  return S_OK;
}

void touchmind::view::GeometryBuilder::TransformRect(IN const D2D1_RECT_F &originalRect, IN FLOAT size,
                                                     OUT D2D1_RECT_F &destinationRect) {
  destinationRect.left = originalRect.left - size;
  destinationRect.top = originalRect.top - size;
  destinationRect.right = originalRect.right + size;
  destinationRect.bottom = originalRect.bottom + size;
}

void touchmind::view::GeometryBuilder::CalculatePath(IN std::shared_ptr<touchmind::model::node::NodeModel> pNode_From,
                                                     IN std::shared_ptr<touchmind::model::node::NodeModel> pNode_To,
                                                     IN touchmind::PATH_DIRECTION linkDirection,
                                                     IN touchmind::NODE_JUSTIFICATION nodeJustification,
                                                     IN FLOAT levelSeparation,
                                                     OUT touchmind::model::CurvePoints &curvePoints) {
  if (linkDirection == PATH_DIRECTION_AUTO) {
    FLOAT center1 = pNode_From->GetX() + (pNode_From->GetWidth() / 2.0f);
    FLOAT center2 = pNode_To->GetX() + (pNode_To->GetWidth() / 2.0f);
    if (center1 <= center2) {
      linkDirection = PATH_DIRECTION_RIGHT;
    } else {
      linkDirection = PATH_DIRECTION_LEFT;
    }
  }

  FLOAT xa = 0, ya = 0, xb = 0, yb = 0, xc = 0, yc = 0, xd = 0, yd = 0;
  switch (linkDirection) {
  case touchmind::PATH_DIRECTION_LEFT:
    xa = pNode_From->GetX();
    ya = pNode_From->GetY() + (pNode_From->GetHeight() / 2.0f);
    xd = pNode_To->GetX() + pNode_To->GetWidth();
    yd = yc = pNode_To->GetY() + (pNode_To->GetHeight() / 2.0f);
    yb = ya;
    switch (nodeJustification) {
    case touchmind::NODE_JUSTIFICATION_TOP:
      xb = xc = xd + levelSeparation / 2.0f;
      break;
    case touchmind::NODE_JUSTIFICATION_BOTTOM:
      xb = xc = xa - levelSeparation / 2.0f;
      break;
    case touchmind::NODE_JUSTIFICATION_CENTER:
      xb = xc = xd + (xa - xd) / 2.0f;
      break;
    }
    break;
  case touchmind::PATH_DIRECTION_RIGHT:
    xa = pNode_From->GetX() + pNode_From->GetWidth();
    ya = pNode_From->GetY() + (pNode_From->GetHeight() / 2.0f);
    xd = pNode_To->GetX();
    yd = yc = pNode_To->GetY() + (pNode_To->GetHeight() / 2.0f);
    yb = ya;
    switch (nodeJustification) {
    case touchmind::NODE_JUSTIFICATION_TOP:
      xb = xc = xd - levelSeparation / 2.0f;
      break;
    case touchmind::NODE_JUSTIFICATION_BOTTOM:
      xb = xc = xa + levelSeparation / 2.0f;
      break;
    case touchmind::NODE_JUSTIFICATION_CENTER:
      xb = xc = xa + (xd - xa) / 2.0f;
      break;
    }
    break;
  }
  curvePoints.SetX(0, xa);
  curvePoints.SetY(0, ya);
  curvePoints.SetX(1, xb);
  curvePoints.SetY(1, yb);
  curvePoints.SetX(2, xc);
  curvePoints.SetY(2, yc);
  curvePoints.SetX(3, xd);
  curvePoints.SetY(3, yd);
}

HRESULT touchmind::view::GeometryBuilder::CreateCurvePathGeometry(IN ID2D1Factory *pD2DFactory,
                                                                  IN const touchmind::model::CurvePoints &curvePoints,
                                                                  OUT ID2D1PathGeometry **ppPathGeometry) {
  CHK_HR(pD2DFactory->CreatePathGeometry(ppPathGeometry));
  CComPtr<ID2D1GeometrySink> pSink = nullptr;
  CHK_RES(pSink, (*ppPathGeometry)->Open(&pSink));
  pSink->SetFillMode(D2D1_FILL_MODE_WINDING);
  pSink->BeginFigure(D2D1::Point2F(curvePoints.GetX(0), curvePoints.GetY(0)),
                     D2D1_FIGURE_BEGIN_FILLED // D2D1_FIGURE_BEGIN_HOLLOW
                     );
  pSink->AddBezier(D2D1::BezierSegment(D2D1::Point2F(curvePoints.GetX(1), curvePoints.GetY(1)),
                                       D2D1::Point2F(curvePoints.GetX(2), curvePoints.GetY(2)),
                                       D2D1::Point2F(curvePoints.GetX(3), curvePoints.GetY(3))));
  pSink->EndFigure(D2D1_FIGURE_END_OPEN);
  CHK_HR(pSink->Close());
  return S_OK;
}

HRESULT touchmind::view::GeometryBuilder::CreateRectangleGeometry(IN ID2D1Factory *pD2DFactory,
                                                                  IN const D2D1_RECT_F &rect, IN bool isCollapsed,
                                                                  IN bool isRightSideNode,
                                                                  OUT ID2D1PathGeometry **ppPathGeometry) {
  CHK_HR(pD2DFactory->CreatePathGeometry(ppPathGeometry));
  CComPtr<ID2D1GeometrySink> pSink = nullptr;
  CHK_RES(pSink, (*ppPathGeometry)->Open(&pSink));
  pSink->BeginFigure(D2D1::Point2F(rect.left, rect.top), D2D1_FIGURE_BEGIN_FILLED);
  pSink->AddLine(D2D1::Point2F(rect.right, rect.top));
  if (isCollapsed && isRightSideNode) {
    D2D1_POINT_2F p1 = {0.0f, 0.0f};
    D2D1_POINT_2F p2 = {0.0f, 0.0f};
    FLOAT rr = 0.0f;
    _CalculateCollapsedMarkSize(rect, 0.0f, isRightSideNode, p1, p2, rr);
    pSink->AddLine(p1);
    pSink->AddArc(
        D2D1::ArcSegment(p2, D2D1::SizeF(rr, rr), touchmind::PI, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
  }

  pSink->AddLine(D2D1::Point2F(rect.right, rect.bottom));
  pSink->AddLine(D2D1::Point2F(rect.left, rect.bottom));
  if (isCollapsed && !isRightSideNode) {
    D2D1_POINT_2F p1 = {0.0f, 0.0f};
    D2D1_POINT_2F p2 = {0.0f, 0.0f};
    FLOAT rr = 0.0f;
    _CalculateCollapsedMarkSize(rect, 0.0f, isRightSideNode, p1, p2, rr);
    pSink->AddLine(p1);
    pSink->AddArc(
        D2D1::ArcSegment(p2, D2D1::SizeF(rr, rr), touchmind::PI, D2D1_SWEEP_DIRECTION_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
  }

  pSink->AddLine(D2D1::Point2F(rect.left, rect.top));
  pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
  CHK_HR(pSink->Close());
  return S_OK;
}

void touchmind::view::GeometryBuilder::CalculateLink(IN std::shared_ptr<touchmind::model::link::LinkModel> link,
                                                     OUT touchmind::model::CurvePoints &curvePoints) {
  auto node1 = link->GetNode(EDGE_ID_1);
  auto node2 = link->GetNode(EDGE_ID_2);
  auto edge1 = link->GetEdge(EDGE_ID_1);
  auto edge2 = link->GetEdge(EDGE_ID_2);
  D2D1_POINT_2F point0 = link->GetEdgePoint(EDGE_ID_1);
  point0.x += node1->GetAncestorPosition() == NODE_SIDE_RIGHT ? edge1->GetMarkerSize() : -edge1->GetMarkerSize();
  D2D1_POINT_2F point1 = link->GetEdgePoint(EDGE_ID_2);
  point1.x += node2->GetAncestorPosition() == NODE_SIDE_RIGHT ? edge2->GetMarkerSize() : -edge2->GetMarkerSize();
  D2D1_POINT_2F handle1 = edge1->CalculateHandlePosition(link->GetEdgePoint(EDGE_ID_1), node1->GetAncestorPosition());
  D2D1_POINT_2F handle2 = edge2->CalculateHandlePosition(link->GetEdgePoint(EDGE_ID_2), node2->GetAncestorPosition());

  curvePoints.SetX(0, point0.x);
  curvePoints.SetY(0, point0.y);
  curvePoints.SetX(1, handle1.x);
  curvePoints.SetY(1, handle1.y);
  curvePoints.SetX(2, handle2.x);
  curvePoints.SetY(2, handle2.y);
  curvePoints.SetX(3, point1.x);
  curvePoints.SetY(3, point1.y);
}

D2D1_POINT_2F touchmind::view::GeometryBuilder::Rotate(const D2D1_POINT_2F &point, FLOAT angle) {
  D2D1_POINT_2F result;
  result.x = point.x * std::cos(angle) - point.y * std::sin(angle);
  result.y = point.x * std::sin(angle) + point.y * std::cos(angle);
  return result;
}

D2D1_POINT_2F touchmind::view::GeometryBuilder::Move(const D2D1_POINT_2F &point, const D2D1_POINT_2F &base) {
  return D2D1::Point2F(point.x + base.x, point.y + base.y);
}

void touchmind::view::GeometryBuilder::CalculateArrowPoints(IN D2D1_POINT_2F point, IN FLOAT width, IN FLOAT height,
                                                            IN FLOAT angle, IN NODE_SIDE nodeSide,
                                                            OUT std::vector<D2D1_POINT_2F> &points) {
  points.clear();
  if (nodeSide == NODE_SIDE_RIGHT) {
    D2D1_POINT_2F point0 = {height, -width / 2.0f};
    D2D1_POINT_2F point1 = {height, width / 2.0f};
    points.push_back(point);
    points.push_back(Move(Rotate(point0, angle), point));
    points.push_back(Move(Rotate(point1, angle), point));
  } else {
    D2D1_POINT_2F point0 = {-height, -width / 2.0f};
    D2D1_POINT_2F point1 = {-height, width / 2.0f};
    points.push_back(point);
    points.push_back(Move(Rotate(point1, -angle), point));
    points.push_back(Move(Rotate(point0, -angle), point));
  }
}

HRESULT touchmind::view::GeometryBuilder::CreatePathGeometryFromPoints(IN ID2D1Factory *pD2DFactory,
                                                                       IN const std::vector<D2D1_POINT_2F> &points,
                                                                       OUT ID2D1PathGeometry **ppPathGeometry) {
  CHK_HR(pD2DFactory->CreatePathGeometry(ppPathGeometry));
  CComPtr<ID2D1GeometrySink> pSink = nullptr;
  CHK_RES(pSink, (*ppPathGeometry)->Open(&pSink));
  pSink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
  for (size_t i = 1; i < points.size(); ++i) {
    pSink->AddLine(points[i]);
  }
  pSink->EndFigure(D2D1_FIGURE_END_CLOSED);
  CHK_HR(pSink->Close());
  return S_OK;
}
