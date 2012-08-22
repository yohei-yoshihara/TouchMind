#include "stdafx.h"
#include "touchmind/Context.h"
#include "touchmind/model/CurvePoints.h"
#include "touchmind/view/GeometryBuilder.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/path/PathModel.h"
#include "touchmind/view/node/NodeViewManager.h"
#include "touchmind/view/path/impl/DefaultPathView.h"

bool touchmind::view::path::impl::DefaultPathView::_CheckValidity()
{
    if (GetNodeModel().expired()) {
        return false;
    }
    std::shared_ptr<model::node::NodeModel> node = GetNodeModel().lock();
    std::shared_ptr<model::node::NodeModel> parent = node->GetParent();
    if (parent == nullptr) {
        return false;
    }
    if (parent->IsCollapsed() || parent->IsAncestorCollapsed()) {
        return false;
    }
    return true;
}

void touchmind::view::path::impl::DefaultPathView::CreateDeviceDependentResources(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget )
{
    if (!_CheckValidity()) {
        return;
    }
    auto node = GetNodeModel().lock();
    auto path = node->GetPathModel();
    if (path->IsRepaintRequired(GetRepaintCounter()) ||
            m_pathBrush == nullptr ||
            m_pathGeometry == nullptr ||
            m_pathStyle == nullptr) {
        _CreateDeviceDependentResources(pContext, pRenderTarget, node);
        if (path->IsSelected()) {
            _CreateTexture(pContext, pRenderTarget, node);
        } else {
            m_pBitmap = nullptr;
        }
        SetRepaintCounter(path);
    }
}

void touchmind::view::path::impl::DefaultPathView::Draw(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget )
{
    if (!_CheckValidity()) {
        return;
    }
    auto node = GetNodeModel().lock();
    auto path = node->GetPathModel();

    CreateDeviceDependentResources(pContext, pRenderTarget);
    if (path->IsSelected()) {
        FLOAT shadowOffset = GetNodeViewManager()->GetGaussFilter()->GetOffset();
        D2D1_SIZE_F size = m_pBitmap->GetSize();
        FLOAT left = m_bounds.left -  shadowOffset;
        FLOAT top  = m_bounds.top - shadowOffset;
        pRenderTarget->DrawBitmap(m_pBitmap, D2D1::RectF(left, top, left + size.width, top + size.height));
    }
    pRenderTarget->DrawGeometry(
        m_pathGeometry,
        m_pathBrush,
        path->GetWidthAsValue(),
        m_pathStyle);
}

bool touchmind::view::path::impl::DefaultPathView::HitTest(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget,
    D2D1_POINT_2F point)
{
    if (!_CheckValidity()) {
        return false;
    }
    CreateDeviceDependentResources(pContext, pRenderTarget);
    BOOL b;
    m_pathGeometry->StrokeContainsPoint(
        point,
        GetNodeModel().lock()->GetPathModel()->GetWidth() + 5.0f,
        nullptr,
        nullptr,
        &b);
    return (b == TRUE);
}

void touchmind::view::path::impl::DefaultPathView::_CreateDeviceDependentResources(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget,
    std::shared_ptr<touchmind::model::node::NodeModel> node)
{
    std::shared_ptr<model::node::NodeModel> parent = node->GetParent();
    ID2D1Factory *pD2DFactory = pContext->GetD2DFactory();
    PATH_DIRECTION pathDirection;
    touchmind::NODE_SIDE nodePosition = node->GetAncestorPosition();
    switch (nodePosition) {
    case touchmind::NODE_SIDE_RIGHT:
        pathDirection = PATH_DIRECTION_RIGHT;
        break;
    case touchmind::NODE_SIDE_LEFT:
        pathDirection = PATH_DIRECTION_LEFT;
        break;
    case touchmind::NODE_SIDE_UNDEFINED:
        pathDirection = PATH_DIRECTION_AUTO;
        break;
    default:
        pathDirection = PATH_DIRECTION_AUTO;
    }
    touchmind::model::CurvePoints curvePoints;
    view::GeometryBuilder::CalculatePath(
        parent,
        node,
        pathDirection,
        GetConfiguration()->GetNodeJustification(),
        GetConfiguration()->GetLevelSeparation(),
        curvePoints);
    m_pathGeometry = nullptr;
    CHK_RES(
        m_pathGeometry,
        view::GeometryBuilder::CreateCurvePathGeometry(pD2DFactory, curvePoints, &m_pathGeometry));
    curvePoints.GetBounds(m_bounds);

    m_pathBrush = nullptr;
    CHK_RES(m_pathBrush,
            pRenderTarget->CreateSolidColorBrush(
                node->GetPathModel()->GetColor(),
                D2D1::BrushProperties(),
                &m_pathBrush));

    // style
    m_pathStyle = nullptr;
    switch (node->GetPathModel()->GetStyle()) {
    case LINE_STYLE_DASHED:
        CHK_RES(m_pathStyle,
                pD2DFactory->CreateStrokeStyle(D2D1::StrokeStyleProperties(
                        D2D1_CAP_STYLE_ROUND,
                        D2D1_CAP_STYLE_ROUND,
                        D2D1_CAP_STYLE_ROUND,
                        D2D1_LINE_JOIN_MITER,
                        node->GetPathModel()->GetWidthAsValue(),
                        D2D1_DASH_STYLE_DASH,
                        0.0f),
                                               nullptr, 0,
                                               &m_pathStyle));
        break;
    case LINE_STYLE_DOTTED:
        CHK_RES(m_pathStyle,
                pD2DFactory->CreateStrokeStyle(D2D1::StrokeStyleProperties(
                        D2D1_CAP_STYLE_ROUND,
                        D2D1_CAP_STYLE_ROUND,
                        D2D1_CAP_STYLE_ROUND,
                        D2D1_LINE_JOIN_MITER,
                        node->GetPathModel()->GetWidthAsValue(),
                        D2D1_DASH_STYLE_DOT,
                        0.0f),
                                               nullptr, 0,
                                               &m_pathStyle));
        break;
    case LINE_STYLE_SOLID:
    default:
        CHK_RES(m_pathStyle,
                pD2DFactory->CreateStrokeStyle(D2D1::StrokeStyleProperties(
                        D2D1_CAP_STYLE_ROUND,
                        D2D1_CAP_STYLE_ROUND,
                        D2D1_CAP_STYLE_ROUND,
                        D2D1_LINE_JOIN_MITER,
                        node->GetPathModel()->GetWidthAsValue(),
                        D2D1_DASH_STYLE_SOLID,
                        0.0f),
                                               nullptr, 0,
                                               &m_pathStyle));

        break;
    }
}

void touchmind::view::path::impl::DefaultPathView::_CreateTexture(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget,
    std::shared_ptr<touchmind::model::node::NodeModel> node)
{
    FLOAT shadowOffset = GetNodeViewManager()->GetGaussFilter()->GetOffset();

    D2D1_RECT_F bounds;
    CHK_HR(m_pathGeometry->GetBounds(nullptr, &bounds));
    UINT width = static_cast<UINT>((bounds.right - bounds.left) + shadowOffset * 2);
    UINT height = static_cast<UINT>((bounds.bottom - bounds.top) + shadowOffset * 2);

    CComPtr<ID3D10Texture2D> pSourceTexutre2D = nullptr;
    CHK_RES(pSourceTexutre2D,
            pContext->CreateTexture2D(width, height, &pSourceTexutre2D));
    CComPtr<ID2D1RenderTarget> pSourceTexture2DRenderTarget = nullptr;
    CHK_RES(pSourceTexture2DRenderTarget,
            pContext->CreateD2DRenderTargetFromTexture2D(
                pSourceTexutre2D,
                &pSourceTexture2DRenderTarget));
    pSourceTexture2DRenderTarget->BeginDraw();
    pSourceTexture2DRenderTarget->Clear(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f));
    pSourceTexture2DRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation(-m_bounds.left + shadowOffset, -m_bounds.top + shadowOffset));
    pSourceTexture2DRenderTarget->DrawGeometry(m_pathGeometry, GetNodeViewManager()->GetSelectedShadowBrush1(), node->GetPathModel()->GetWidthAsValue() + 2.0f);
    pSourceTexture2DRenderTarget->DrawGeometry(m_pathGeometry, GetNodeViewManager()->GetSelectedShadowBrush2(), node->GetPathModel()->GetWidthAsValue());
    CHK_HR(pSourceTexture2DRenderTarget->EndDraw());
    CComPtr<ID3D10Texture2D> pTexture2D = nullptr;
    CHK_RES(pTexture2D,
            GetNodeViewManager()->GetGaussFilter()->ApplyFilter(pContext, pSourceTexutre2D, &pTexture2D));
    m_pBitmap = nullptr;
    CHK_RES(m_pBitmap,
            pContext->CreateD2DSharedBitmapFromTexture2D(pRenderTarget, pTexture2D, &m_pBitmap));
    ASSERT(m_pBitmap != nullptr);
}

void touchmind::view::path::impl::DefaultPathView::DiscardDeviceDependentResources()
{
    m_pathBrush = nullptr;
    m_pathGeometry = nullptr;
    m_pathStyle = nullptr;
    m_pBitmap = nullptr;
}
