#ifndef TOUCHMIND_PRINT_XPSGEOMETRYBUILDER_H_
#define TOUCHMIND_PRINT_XPSGEOMETRYBUILDER_H_

#include "forwarddecl.h"

#include "touchmind/Common.h"
#include "touchmind/model/CurvePoints.h"
#include "touchmind/print/XPSPrintCommon.h"

namespace touchmind {
  namespace print {

    class XPSGeometryBuilder {
    private:
      XPSGeometryBuilder() {
      }

    public:
      static void CalculateTransformMatrix(const XPS_SIZE &pageSize, const XPSPRINT_MARGIN &xpsMargin,
                                           std::shared_ptr<touchmind::model::node::NodeModel> node,
                                           OUT XPS_MATRIX &xpsMatrix, OUT FLOAT &scale);
      static void CreateRoundedRectangleGeometry(IXpsOMObjectFactory *pXpsFactory, const XPS_RECT &rect, FLOAT r,
                                                 OUT IXpsOMGeometryFigure **ppXpsFigure);
      static void CreateRectangleGeometry(IXpsOMObjectFactory *pXpsFactory, const XPS_RECT &rect,
                                          OUT IXpsOMGeometryFigure **ppXpsFigure);
      static void CreateCurvePathGeometry(IXpsOMObjectFactory *pXpsFactory,
                                          const touchmind::model::CurvePoints &curvePoints,
                                          OUT IXpsOMGeometryFigure **ppXpsFigure);
      static void CreatePathGeometryFromPoints(IXpsOMObjectFactory *pXpsFactory,
                                               const std::vector<D2D1_POINT_2F> &points,
                                               OUT IXpsOMGeometryFigure **ppXpsFigure);
      static void CreateCircleGeometry(IXpsOMObjectFactory *pXpsFactory, const D2D1_POINT_2F &point, FLOAT r,
                                       OUT IXpsOMGeometryFigure **ppXpsFigure);
    };

  } // print
} // touchmind

#endif // TOUCHMIND_PRINT_XPSGEOMETRYBUILDER_H_