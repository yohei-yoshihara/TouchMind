#include "stdafx.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/layout/LayoutManager.h"
#include "touchmind/print/XPSGeometryBuilder.h"

void touchmind::print::XPSGeometryBuilder::CalculateTransformMatrix(
    const XPS_SIZE &pageSize,
    const XPSPRINT_MARGIN &xpsMargin,
    std::shared_ptr<touchmind::model::node::NodeModel> node,
    OUT XPS_MATRIX &xpsMatrix,
    OUT FLOAT &scale)
{
    touchmind::layout::TreeRectVisitor treeRectVisitor;
    node->ApplyVisitor(treeRectVisitor);

    FLOAT width_p = pageSize.width - xpsMargin.left - xpsMargin.right;
    FLOAT height_p = pageSize.height - xpsMargin.top - xpsMargin.bottom;

    FLOAT width_m = treeRectVisitor.treeRect.x2 - treeRectVisitor.treeRect.x1;
    FLOAT height_m = treeRectVisitor.treeRect.y2 - treeRectVisitor.treeRect.y1;

    D2D1_MATRIX_3X2_F translationMatrix;
    D2D1_MATRIX_3X2_F scaleMatrix;
    FLOAT rx = width_p / width_m;
    FLOAT ry = height_p / height_m;
    FLOAT cx = treeRectVisitor.treeRect.x1 + width_m / 2.0f;
    FLOAT cy = treeRectVisitor.treeRect.y1 + height_m / 2.0f;
    if (rx >= 1.0f && ry >= 1.0f) {
        translationMatrix = D2D1::Matrix3x2F::Translation(
                                width_p / 2.0f + xpsMargin.left - cx,
                                height_p / 2.0f + xpsMargin.top - cy );
        scale = 1.0f;
    } else if (rx < ry) {
        translationMatrix = D2D1::Matrix3x2F::Translation(
                                width_p / 2.0f + xpsMargin.left - cx * rx,
                                height_p / 2.0f + xpsMargin.top - cy * rx );
        scale = rx;
    } else {
        translationMatrix = D2D1::Matrix3x2F::Translation(
                                width_p / 2.0f + xpsMargin.left - cx * ry,
                                height_p / 2.0f + xpsMargin.top - cy * ry );
        scale = ry;
    }

    scaleMatrix = D2D1::Matrix3x2F::Scale(scale, scale, D2D1::Point2F(0.0f, 0.0f));
    D2D1_MATRIX_3X2_F matrix = scaleMatrix * translationMatrix;

    xpsMatrix.m11 = matrix._11;
    xpsMatrix.m12 = matrix._12;
    xpsMatrix.m21 = matrix._21;
    xpsMatrix.m22 = matrix._22;
    xpsMatrix.m31 = matrix._31;
    xpsMatrix.m32 = matrix._32;
}

void touchmind::print::XPSGeometryBuilder::CreateRoundedRectangleGeometry(
    IXpsOMObjectFactory *pXpsFactory, 
    const XPS_RECT &rect, FLOAT r, 
    OUT IXpsOMGeometryFigure **ppXpsFigure )
{
    XPS_POINT startPoint;
    startPoint.x = rect.x;
    startPoint.y = rect.y + r;
    XPS_SEGMENT_TYPE segmentTypes[8] = {
        XPS_SEGMENT_TYPE_ARC_SMALL_CLOCKWISE,
        XPS_SEGMENT_TYPE_LINE,
        XPS_SEGMENT_TYPE_ARC_SMALL_CLOCKWISE,
        XPS_SEGMENT_TYPE_LINE,
        XPS_SEGMENT_TYPE_ARC_SMALL_CLOCKWISE,
        XPS_SEGMENT_TYPE_LINE,
        XPS_SEGMENT_TYPE_ARC_SMALL_CLOCKWISE,
        XPS_SEGMENT_TYPE_LINE,
    };
    FLOAT segmentData[4 * 5 + 4 * 2] = {
        rect.x + r, rect.y, r, r, 90.0f,
        rect.x + rect.width - r, rect.y,
        rect.x + rect.width, rect.y + r, r, r, 90.0f,
        rect.x + rect.width, rect.y + rect.height - r,
        rect.x + rect.width - r, rect.y + rect.height, r, r, 90.0f,
        rect.x + r, rect.y + rect.height,
        rect.x, rect.y + rect.height - r, r, r, 90.0f,
        rect.x, rect.y + r
    };
    BOOL segmentStrokes[8] = {
        TRUE, TRUE, TRUE, TRUE,
        TRUE, TRUE, TRUE, TRUE
    };

    CHK_RES(*ppXpsFigure, pXpsFactory->CreateGeometryFigure(&startPoint, ppXpsFigure));
    // one of the pdf converter software have a problem when set SetISClosed to TRUE
    CHK_HR((*ppXpsFigure)->SetIsClosed(FALSE));
    CHK_HR((*ppXpsFigure)->SetIsFilled(TRUE));
    CHK_HR((*ppXpsFigure)->SetSegments(
               8, 4 * 5 + 4 * 2,
               segmentTypes,
               segmentData,
               segmentStrokes));
}

void touchmind::print::XPSGeometryBuilder::CreateRectangleGeometry(
    IXpsOMObjectFactory *pXpsFactory,
    const XPS_RECT &rect,
    OUT IXpsOMGeometryFigure **ppXpsFigure )
{
    XPS_POINT startPoint;
    startPoint.x = rect.x;
    startPoint.y = rect.y;
    XPS_SEGMENT_TYPE segmentTypes[4] = {
        XPS_SEGMENT_TYPE_LINE,
        XPS_SEGMENT_TYPE_LINE,
        XPS_SEGMENT_TYPE_LINE,
        XPS_SEGMENT_TYPE_LINE,
    };
    FLOAT segmentData[4 * 2] = {
        rect.x + rect.width, rect.y,
        rect.x + rect.width, rect.y + rect.height,
        rect.x, rect.y + rect.height,
        rect.x, rect.y
    };
    BOOL segmentStrokes[4] = {
        TRUE, TRUE, TRUE, TRUE,
    };

    CHK_RES(*ppXpsFigure, pXpsFactory->CreateGeometryFigure(&startPoint, ppXpsFigure));
    // one of the pdf converter software have a problem when set SetISClosed to TRUE
    CHK_HR((*ppXpsFigure)->SetIsClosed(FALSE));
    CHK_HR((*ppXpsFigure)->SetIsFilled(TRUE));
    CHK_HR((*ppXpsFigure)->SetSegments(
               ARRAYSIZE(segmentTypes),
               ARRAYSIZE(segmentData),
               segmentTypes,
               segmentData,
               segmentStrokes));
}

void touchmind::print::XPSGeometryBuilder::CreateCurvePathGeometry(
    IXpsOMObjectFactory *pXpsFactory,
    const touchmind::model::CurvePoints &curvePoints,
    IXpsOMGeometryFigure **ppXpsFigure )
{
    XPS_POINT startPoint;
    startPoint.x = curvePoints.GetX(0);
    startPoint.y = curvePoints.GetY(0);

    XPS_SEGMENT_TYPE segmentTypes[1] = {
        XPS_SEGMENT_TYPE_BEZIER
    };

    FLOAT segmentData[6] = {
        curvePoints.GetX(1), curvePoints.GetY(1),
        curvePoints.GetX(2), curvePoints.GetY(2),
        curvePoints.GetX(3), curvePoints.GetY(3)
    };

    BOOL segmentStrokes[1] = {
        TRUE
    };

    CHK_RES(*ppXpsFigure, pXpsFactory->CreateGeometryFigure(&startPoint, ppXpsFigure)) ;
    CHK_HR((*ppXpsFigure)->SetIsClosed(FALSE));
    CHK_HR((*ppXpsFigure)->SetIsFilled(FALSE));
    CHK_HR((*ppXpsFigure)->SetSegments(
               1, 6,
               segmentTypes,
               segmentData,
               segmentStrokes));
}

void touchmind::print::XPSGeometryBuilder::CreatePathGeometryFromPoints(
    IXpsOMObjectFactory *pXpsFactory,
    const std::vector<D2D1_POINT_2F> &points,
    OUT IXpsOMGeometryFigure** ppXpsFigure)
{
    XPS_POINT startPoint = {points[0].x, points[0].y};

    std::vector<XPS_SEGMENT_TYPE> segmentTypes(points.size());
    std::vector<FLOAT> segmentData(segmentTypes.size() * 2);
    std::vector<BOOL> segmentStrokes(segmentTypes.size());

    for (size_t i = 1; i < points.size(); ++i) {
        size_t j = i - 1;
        segmentTypes[j] = XPS_SEGMENT_TYPE_LINE;
        segmentData[j * 2] = points[i].x;
        segmentData[j * 2 + 1] = points[i].y;
        segmentStrokes[j] = TRUE;
    }
    size_t i = 0;
    size_t j = points.size() - 1;
    segmentTypes[j] = XPS_SEGMENT_TYPE_LINE;
    segmentData[j * 2] = points[i].x;
    segmentData[j * 2 + 1] = points[i].y;
    segmentStrokes[j] = TRUE;


    CHK_RES(*ppXpsFigure, pXpsFactory->CreateGeometryFigure(&startPoint, ppXpsFigure));
    // one of the pdf converter software have a problem when set SetISClosed to TRUE
    CHK_HR((*ppXpsFigure)->SetIsClosed(FALSE));
    CHK_HR((*ppXpsFigure)->SetIsFilled(TRUE));
    CHK_HR((*ppXpsFigure)->SetSegments(
               static_cast<UINT32>(segmentTypes.size()),
               static_cast<UINT32>(segmentData.size()),
               segmentTypes.data(),
               segmentData.data(),
               segmentStrokes.data()));
}

void touchmind::print::XPSGeometryBuilder::CreateCircleGeometry(
    IXpsOMObjectFactory *pXpsFactory,
    const D2D1_POINT_2F &point,
    FLOAT r,
    OUT IXpsOMGeometryFigure** ppXpsFigure )
{
    XPS_POINT startPoint = {
        point.x + r,
        point.y
    };
    XPS_SEGMENT_TYPE segmentTypes[] = {
        XPS_SEGMENT_TYPE_ARC_SMALL_CLOCKWISE,
        XPS_SEGMENT_TYPE_ARC_SMALL_CLOCKWISE,
        XPS_SEGMENT_TYPE_ARC_SMALL_CLOCKWISE,
        XPS_SEGMENT_TYPE_ARC_SMALL_CLOCKWISE,
    };
    FLOAT segmentData[] = {
        point.x,     point.y + r, r, r, 90.0f,
        point.x - r, point.y,     r, r, 90.0f,
        point.x,     point.y - r, r, r, 90.0f,
        point.x + r, point.y,     r, r, 90.0f,
    };
    BOOL segmentStrokes[] = {
        TRUE,
        TRUE,
        TRUE,
        TRUE,
    };
    CHK_RES(*ppXpsFigure, pXpsFactory->CreateGeometryFigure(&startPoint, ppXpsFigure));
    // one of the pdf converter software have a problem when set SetISClosed to TRUE
    CHK_HR((*ppXpsFigure)->SetIsClosed(FALSE));
    CHK_HR((*ppXpsFigure)->SetIsFilled(TRUE));
    CHK_HR((*ppXpsFigure)->SetSegments(
               ARRAYSIZE(segmentTypes),
               ARRAYSIZE(segmentData),
               segmentTypes,
               segmentData,
               segmentStrokes));
}
