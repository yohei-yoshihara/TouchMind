#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/Context.h"
#include "touchmind/control/DWriteEditControlCommon.h"
#include "touchmind/control/DWriteEditControlManager.h"
#include "touchmind/control/DWriteEditControl.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/view/node/BaseNodeView.h"
#include "touchmind/view/node/NodeViewManager.h"
#include "touchmind/view/node/NodeFigureHelper.h"
#include "touchmind/view/node/impl/RectangleNodeView.h"
#include "touchmind/view/GeometryBuilder.h"

touchmind::view::node::impl::RectangleNodeView::RectangleNodeView(std::weak_ptr<touchmind::model::node::NodeModel> node) :
    BaseNodeView(node),
    m_pTexture(nullptr)
{
}

touchmind::view::node::impl::RectangleNodeView::~RectangleNodeView()
{
}

void touchmind::view::node::impl::RectangleNodeView::CreateDeviceDependentResources(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget)
{
    UNREFERENCED_PARAMETER(pContext);
    UNREFERENCED_PARAMETER(pRenderTarget);
}

void touchmind::view::node::impl::RectangleNodeView::Draw(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget)
{
    LOG_ENTER;

    UNREFERENCED_PARAMETER(pContext);
    UNREFERENCED_PARAMETER(pRenderTarget);
    // to avoid an issue (window resizing fail), this resource must be static.
    static CComPtr<ID2D1RenderTarget> pOutputTexture2DRenderTarget = nullptr;

    static const FLOAT bodyOffset = 3.0f;
    static const FLOAT shadowOffset = 10.0f;
    static const FLOAT cornerSize = 10.0f;

    if (!HasNodeModel()) {
        return;
    }

    auto node = GetNodeModel().lock();
    auto editControlIndex = GetEditControlIndex();
    auto editControl = GetEditControlManager()->GetEditControl(editControlIndex);

    if (!node->IsCollapsed() && node->IsAncestorCollapsed()) {
        return;
    }

    if (m_pTexture == nullptr ||
            node->IsRepaintRequired(GetRepaintCounter())) {
        bool isRightSideNode = true;
        if (node->GetAncestorPosition() == touchmind::NODE_SIDE_UNDEFINED) {
            FLOAT centerX = node->GetX() + node->GetWidth() / 2.0f;
            isRightSideNode = centerX >= 0;
        } else {
            isRightSideNode = node->GetAncestorPosition() == touchmind::NODE_SIDE_RIGHT;
        }

        FLOAT d = 0.5f; // move 0.5 pixel to sharpen lines.
        D2D1_RECT_F bodyRect = D2D1::RectF(
                                   shadowOffset + d,
                                   shadowOffset + d,
                                   shadowOffset + node->GetWidth() + bodyOffset * 2 + d,
                                   shadowOffset + node->GetHeight() + bodyOffset * 2 + d);

        UINT nWidth  = (UINT)(node->GetWidth() + 0.5f + (bodyOffset + shadowOffset) * 2.0f);
        UINT nHeight = (UINT)(node->GetHeight() + 0.5f + (bodyOffset + shadowOffset) * 2.0f);

        NodeFigureHelper nodeFigure;

        // Create body path geometry
        CComPtr<ID2D1PathGeometry> pBodyGeometry = nullptr;
        CHK_RES(pBodyGeometry,
                GeometryBuilder::CreateRoundedRectangleGeometry(
                    pContext->GetD2DFactory(),
                    bodyRect,
                    0.0f,
                    node->IsCollapsed(),
                    isRightSideNode,
                    &pBodyGeometry));
        // draw shadow
        CComPtr<ID3D10Texture2D> pSourceTexutre2D = nullptr;
        CHK_RES(pSourceTexutre2D,
                pContext->CreateTexture2D(nWidth, nHeight, &pSourceTexutre2D));
        CComPtr<ID2D1RenderTarget> pSourceTexture2DRenderTarget = nullptr;
        CHK_RES(pSourceTexture2DRenderTarget,
                pContext->CreateD2DRenderTargetFromTexture2D(pSourceTexutre2D, &pSourceTexture2DRenderTarget));
        pSourceTexture2DRenderTarget->BeginDraw();
        pSourceTexture2DRenderTarget->Clear(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f));
        if (node->IsSelected()) {
            D2D1_RECT_F highlightRect;
            GeometryBuilder::TransformRect(bodyRect, 2.0f, highlightRect);
            CComPtr<ID2D1PathGeometry> pHighlightGeometry = nullptr;
            CHK_RES(pHighlightGeometry,
                    GeometryBuilder::CreateRoundedRectangleGeometry(
                        pContext->GetD2DFactory(),
                        highlightRect,
                        0.0f,
                        node->IsCollapsed(),
                        isRightSideNode,
                        &pHighlightGeometry));
            GetNodeViewManager()->DrawSelectedNodeShadow(pContext, pSourceTexture2DRenderTarget, pHighlightGeometry);
        } else {
            GetNodeViewManager()->DrawNodeShadow(pContext, pSourceTexture2DRenderTarget, pBodyGeometry);
        }
        CHK_HR(pSourceTexture2DRenderTarget->EndDraw());
        CComPtr<ID3D10Texture2D> pOutputTexutre2D = nullptr;
        CHK_RES(pOutputTexutre2D,
                GetNodeViewManager()->GetGaussFilter()->ApplyFilter(pContext, pSourceTexutre2D, &pOutputTexutre2D));

        // draw body
        //ID2D1RenderTarget *pOutputTexture2DRenderTarget = nullptr; // can not wrap with CComPtr<> (windows resizing fail)
        pOutputTexture2DRenderTarget = nullptr;
        CHK_RES(pOutputTexture2DRenderTarget,
                pContext->CreateD2DRenderTargetFromTexture2D(pOutputTexutre2D, &pOutputTexture2DRenderTarget));
        pOutputTexture2DRenderTarget->BeginDraw();
        D2D1_COLOR_F startColor;
        D2D1_COLOR_F endColor;
        GetNodeViewManager()->CreateBodyColor(util::ColorUtil::ToColorref(node->GetBackgroundColor()), startColor, endColor);
        nodeFigure.SetPlateStartColor(startColor);
        nodeFigure.SetPlateEndColor(endColor);
        nodeFigure.DrawPlate(bodyRect, 0.0f, pContext->GetD2DFactory(), pOutputTexture2DRenderTarget);

        if (node->IsCollapsed()) {
            CComPtr<ID2D1SolidColorBrush> pCollapsedMarkBrush = nullptr;
            CHK_RES(pCollapsedMarkBrush,
                    pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkOrange), &pCollapsedMarkBrush));
            CComPtr<ID2D1PathGeometry> pCollapsedMarkPathGeometry = nullptr;
            CHK_RES(pCollapsedMarkPathGeometry,
                    GeometryBuilder::CreateCollapsedMarkGeometry(
                        pContext->GetD2DFactory(),
                        bodyRect,
                        nodeFigure.GetCornerRoundSize(),
                        isRightSideNode,
                        &pCollapsedMarkPathGeometry));
            pOutputTexture2DRenderTarget->FillGeometry(pCollapsedMarkPathGeometry, pCollapsedMarkBrush);
        }

        pOutputTexture2DRenderTarget->EndDraw();

        m_pTexture = pOutputTexutre2D;
    }
    // Draw node on the render target
    CComPtr<ID2D1Bitmap> pOutputSharedBitmap = nullptr;
    CHK_RES(pOutputSharedBitmap,
            pContext->CreateD2DSharedBitmapFromTexture2D(pRenderTarget, m_pTexture, &pOutputSharedBitmap));
    D2D1_SIZE_F size = pOutputSharedBitmap->GetSize();
    FLOAT left = node->GetX() - bodyOffset - shadowOffset;
    FLOAT top  = node->GetY() - bodyOffset - shadowOffset;
    pRenderTarget->DrawBitmap(pOutputSharedBitmap, D2D1::RectF(left, top, left + size.width, top + size.height));

    // Draw an edit control
    editControl->OnRender(pRenderTarget);

    SetRepaintCounter(node);

    LOG_LEAVE;
}

bool touchmind::view::node::impl::RectangleNodeView::HitTest(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget,
    D2D1_POINT_2F point )
{
    UNREFERENCED_PARAMETER(pContext);
    UNREFERENCED_PARAMETER(pRenderTarget);
    UNREFERENCED_PARAMETER(point);
    return false;
}

bool touchmind::view::node::impl::RectangleNodeView::IsCompatible(
    const std::shared_ptr<touchmind::model::BaseModel> &model)
{
    auto node = std::dynamic_pointer_cast<touchmind::model::node::NodeModel>(model);
    if (node != nullptr) {
        return node->GetNodeShape() == NODE_SHAPE_RECTANGLE;
    }
    return false;
}

void touchmind::view::node::impl::RectangleNodeView::DiscardDeviceDependentResources()
{
    BaseNodeView::DiscardDeviceDependentResources();
    m_pTexture = nullptr;
}
