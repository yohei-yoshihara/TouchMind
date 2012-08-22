#ifndef TOUCHMIND_VIEW_GEOMETRYBUILDER_H_
#define TOUCHMIND_VIEW_GEOMETRYBUILDER_H_

#include "forwarddecl.h"
#include "touchmind/Common.h"
#include "touchmind/Configuration.h"

namespace touchmind
{
namespace view
{

enum TRIANGLE_VERTEX {
    TRIANGLE_VERTEX_TOP_LEFT,
    TRIANGLE_VERTEX_TOP_RIGHT,
    TRIANGLE_VERTEX_BOTTOM_LEFT,
    TRIANGLE_VERTEX_BOTTOM_RIGHT
};

class GeometryBuilder
{
private:
    GeometryBuilder() {}

protected:
    static void _CalculateCollapsedMarkSize(
        IN const D2D1_RECT_F &rect,
        IN FLOAT r,
        IN bool isRightSideNode,
        OUT D2D1_POINT_2F &p1,
        OUT D2D1_POINT_2F &p2,
        OUT FLOAT &actualR);

public:
    static std::array<bool, 9> s_MATRIX_TOP_LEFT;
    static std::array<bool, 9> s_MATRIX_TOP_RIGHT;
    static std::array<bool, 9> s_MATRIX_BOTTOM_LEFT;
    static std::array<bool, 9> s_MATRIX_BOTTOM_RIGHT;

    static HRESULT CreateCollapsedMarkGeometry(
        IN ID2D1Factory* pD2DFactory,
        IN const D2D1_RECT_F &rect,
        IN FLOAT r,
        IN bool isRightSideNode,
        OUT ID2D1PathGeometry** ppPathGeometry);
    static HRESULT CreateRoundedRectangleGeometry(
        IN ID2D1Factory* pD2DFactory,
        IN const D2D1_RECT_F &rect,
        IN FLOAT r,
        IN bool isCollapsed,
        IN bool isRightSideNode,
        OUT ID2D1PathGeometry** ppPathGeometry);
    static HRESULT CreateRectangleGeometry(
        IN ID2D1Factory* pD2DFactory,
        IN const D2D1_RECT_F &rect,
        IN bool isCollapsed,
        IN bool isRightSideNode,
        OUT ID2D1PathGeometry** ppPathGeometry);
    static HRESULT CreateHalfRoundedRectangleGeometry(
        IN ID2D1Factory* pD2DFactory,
        IN const D2D1_RECT_F &rect,
        IN FLOAT r,
        OUT ID2D1PathGeometry** ppPathGeometry);
    static HRESULT CreateDiagonalGradientBrush(
        IN ID2D1RenderTarget* pRenderTarget,
        IN const D2D1_RECT_F &rect,
        IN const std::vector<D2D1_GRADIENT_STOP> &gradientStops,
        OUT ID2D1LinearGradientBrush** ppGradientBrush);
    static HRESULT CreateVerticalGradientBrush(
        IN ID2D1RenderTarget* pRenderTarget,
        IN const D2D1_RECT_F &rect,
        IN const std::vector<D2D1_GRADIENT_STOP> &gradientStops,
        OUT ID2D1LinearGradientBrush** ppGradientBrush);
    static HRESULT CreateIsoscelesTriangleGeometry(
        IN ID2D1Factory* pD2DFactory,
        IN const D2D1_RECT_F &rect,
        IN TRIANGLE_VERTEX tirangleVertex,
        OUT ID2D1PathGeometry** ppPathGeometry);
    static HRESULT CreateTriangleMoverGeometry(
        IN ID2D1Factory* pD2DFactory,
        IN const D2D1_RECT_F &rect,
        IN TRIANGLE_VERTEX tirangleVertex,
        OUT ID2D1PathGeometry** ppPathGeometry);
    static void TransformRect(
        IN const D2D1_RECT_F &originalRect,
        IN FLOAT size,
        OUT D2D1_RECT_F &destinationRect);
    static void CalculatePath(
        IN std::shared_ptr<touchmind::model::node::NodeModel> pNode_From,
        IN std::shared_ptr<touchmind::model::node::NodeModel> pNode_To,
        IN touchmind::PATH_DIRECTION linkDirection,
        IN touchmind::NODE_JUSTIFICATION nodeJustification,
        IN FLOAT levelSeparation,
        OUT touchmind::model::CurvePoints& pCurvePoints);
    static HRESULT CreateCurvePathGeometry(
        IN ID2D1Factory* pD2DFactory,
        IN const touchmind::model::CurvePoints &curvePoints,
        OUT ID2D1PathGeometry** ppPathGeometry);
    static void CalculateLink(
        IN std::shared_ptr<touchmind::model::link::LinkModel> link,
        OUT touchmind::model::CurvePoints& curvePoints);
    static D2D1_POINT_2F Rotate(const D2D1_POINT_2F &point, FLOAT angle);
    static D2D1_POINT_2F Move(const D2D1_POINT_2F &point, const D2D1_POINT_2F &base);
    static void CalculateArrowPoints(
        IN D2D1_POINT_2F point,
        IN FLOAT width,
        IN FLOAT height,
        IN FLOAT angle,
        IN NODE_SIDE nodeSide,
        OUT std::vector<D2D1_POINT_2F> &points);
    static HRESULT CreatePathGeometryFromPoints(
        IN ID2D1Factory* pD2DFactory,
        IN const std::vector<D2D1_POINT_2F> &points,
        OUT ID2D1PathGeometry** ppPathGeometry);
};

} // view
} // touchmind

#endif // TOUCHMIND_VIEW_GEOMETRYBUILDER_H_