#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/view/GeometryBuilder.h"
#include "touchmind/view/node/NodeFigureHelper.h"

touchmind::view::node::NodeFigureHelper::NodeFigureHelper(void) :
    m_cornerRoundSize(5.0f),
    m_triangleSize(10.0f),
    m_triangleMergin(3.0f),
    m_strokeColor(
        D2D1::ColorF(0.3f,
                     0.3f,
                     0.3f,
                     1.0f)),
    m_plateStartColor(
        D2D1::ColorF(0xffffff,
                     1.0f)),
    m_plateEndColor(
        D2D1::ColorF(0xcbcbcb,
                     1.0f)),
    m_rect(D2D1::RectF()),
    m_pStrokeColorBrush(nullptr),
    m_pPlateOutlinePathGeometry(nullptr),
    m_pPlateBrush(nullptr),
    m_pTrianglePathGeometry(nullptr),
    m_pTriangleBrush(nullptr)
{
}

touchmind::view::node::NodeFigureHelper::~NodeFigureHelper(void)
{
}

void touchmind::view::node::NodeFigureHelper::DiscardResources(
)
{
}

void touchmind::view::node::NodeFigureHelper::DrawPlate(
    const D2D1_RECT_F & rect,
    FLOAT r,
    ID2D1Factory * pD2DFactory,
    ID2D1RenderTarget * pRenderTarget)
{
    if (rect.left != m_rect.left ||
            rect.top != m_rect.top ||
            rect.right != m_rect.right ||
            rect.bottom != m_rect.bottom) {
        m_pPlateOutlinePathGeometry = nullptr;
        m_pPlateBrush = nullptr;
        m_pTrianglePathGeometry = nullptr;
        m_pTriangleBrush = nullptr;
    }

    if (m_pStrokeColorBrush == nullptr) {
        CHK_RES(m_pStrokeColorBrush, pRenderTarget->CreateSolidColorBrush(m_strokeColor, &m_pStrokeColorBrush));
    }
    if (m_pPlateOutlinePathGeometry == nullptr) {
        CHK_RES(m_pPlateOutlinePathGeometry,
                GeometryBuilder::CreateRoundedRectangleGeometry(pD2DFactory, rect, r, false, true, &m_pPlateOutlinePathGeometry));
    }

    if (m_pPlateBrush == nullptr) {
        std::vector<D2D1_GRADIENT_STOP> plateOutlineGradientStops;
        plateOutlineGradientStops.push_back(D2D1::GradientStop(0.0f, m_plateStartColor));
        plateOutlineGradientStops.push_back(D2D1::GradientStop(1.0f, m_plateEndColor));
        CHK_RES(m_pPlateBrush,
                GeometryBuilder::CreateDiagonalGradientBrush(pRenderTarget, rect, plateOutlineGradientStops, &m_pPlateBrush));
    }
    // Outer frame
    if (m_pPlateBrush != nullptr && m_pStrokeColorBrush != nullptr) {
        pRenderTarget->FillGeometry(m_pPlateOutlinePathGeometry, m_pPlateBrush);
        pRenderTarget->DrawGeometry(m_pPlateOutlinePathGeometry, m_pStrokeColorBrush);
    }
    // Triangle
    D2D1_RECT_F triangleRect =
        D2D1::RectF(rect.right - m_triangleSize - m_triangleMergin, rect.bottom - m_triangleSize - m_triangleMergin,
                    rect.right - m_triangleMergin, rect.bottom - m_triangleMergin);
    if (m_pTrianglePathGeometry == nullptr) {
        CHK_RES(m_pTrianglePathGeometry,
                GeometryBuilder::CreateTriangleMoverGeometry(pD2DFactory, triangleRect, TRIANGLE_VERTEX_BOTTOM_RIGHT,
                        &m_pTrianglePathGeometry));
    }
    if (m_pTriangleBrush == nullptr) {
        std::vector<D2D1_GRADIENT_STOP> triangleGradientStops;
        triangleGradientStops.push_back(D2D1::GradientStop(0.0f, D2D1::ColorF(0xd45500, 1.0f)));
        triangleGradientStops.push_back(D2D1::GradientStop(1.0f, D2D1::ColorF(0xff9249, 1.0f)));
        CHK_RES(m_pTriangleBrush,
                GeometryBuilder::CreateVerticalGradientBrush(pRenderTarget, triangleRect, triangleGradientStops, &m_pTriangleBrush));
    }
    if (m_pTrianglePathGeometry != nullptr && m_pTriangleBrush != nullptr) {
        pRenderTarget->FillGeometry(m_pTrianglePathGeometry, m_pTriangleBrush);
    }
    m_rect.left = rect.left;
    m_rect.top = rect.top;
    m_rect.right = rect.right;
    m_rect.bottom = rect.bottom;
}

void touchmind::view::node::NodeFigureHelper::SetPlateStartColor(
    const D2D1_COLOR_F & plateStartColor)
{
    m_plateStartColor.a = plateStartColor.a;
    m_plateStartColor.r = plateStartColor.r;
    m_plateStartColor.g = plateStartColor.g;
    m_plateStartColor.b = plateStartColor.b;
    m_pPlateBrush = nullptr;
}

void touchmind::view::node::NodeFigureHelper::SetPlateEndColor(
    const D2D1_COLOR_F & plateEndColor)
{
    m_plateEndColor.a = plateEndColor.a;
    m_plateEndColor.r = plateEndColor.r;
    m_plateEndColor.g = plateEndColor.g;
    m_plateEndColor.b = plateEndColor.b;
    m_pPlateBrush = nullptr;
}
