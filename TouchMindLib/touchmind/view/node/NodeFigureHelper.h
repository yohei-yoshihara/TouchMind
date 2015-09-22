#ifndef TOUCHMIND_VIEW_NODE_NODEFIGURE_H_
#define TOUCHMIND_VIEW_NODE_NODEFIGURE_H_

namespace touchmind {
  namespace view {
    namespace node {

      class NodeFigureHelper {
      private:
        FLOAT m_cornerRoundSize;
        FLOAT m_triangleSize;
        FLOAT m_triangleMergin;
        D2D1_COLOR_F m_strokeColor;
        D2D1_COLOR_F m_plateStartColor;
        D2D1_COLOR_F m_plateEndColor;

        D2D1_RECT_F m_rect;
        CComPtr<ID2D1SolidColorBrush> m_pStrokeColorBrush;
        CComPtr<ID2D1PathGeometry> m_pPlateOutlinePathGeometry;
        CComPtr<ID2D1LinearGradientBrush> m_pPlateBrush;
        CComPtr<ID2D1PathGeometry> m_pTrianglePathGeometry;
        CComPtr<ID2D1LinearGradientBrush> m_pTriangleBrush;

      public:
        NodeFigureHelper(void);
        ~NodeFigureHelper(void);
        void DiscardResources();
        void DrawPlate(const D2D1_RECT_F &rect, FLOAT r, ID2D1Factory *pD2DFactory, ID2D1RenderTarget *pRenderTarget);
        void SetCornerRoundSize(const FLOAT cornerRoundSize) {
          m_cornerRoundSize = cornerRoundSize;
        }
        FLOAT GetCornerRoundSize() const {
          return m_cornerRoundSize;
        }
        void SetTriangleSize(const FLOAT triangleSize) {
          m_triangleSize = triangleSize;
        }
        FLOAT GetTriangleSize() const {
          return m_triangleSize;
        }
        void SetTriangleMergin(const FLOAT triangleMergin) {
          m_triangleMergin = triangleMergin;
        }
        FLOAT GetTriangleMergin() const {
          return m_triangleMergin;
        }
        void SetStrokeColor(const D2D1_COLOR_F &strokeColor) {
          m_strokeColor = strokeColor;
        }
        D2D1_COLOR_F GetStrokeColor() const {
          return m_strokeColor;
        }
        void SetPlateStartColor(const D2D1_COLOR_F &plateStartColor);
        D2D1_COLOR_F GetPlateStartColor() const {
          return m_plateStartColor;
        }
        void SetPlateEndColor(const D2D1_COLOR_F &plateEndColor);
        D2D1_COLOR_F GetPlateEndColor() const {
          return m_plateEndColor;
        }
      };

    } // node
  } // view
} // touchmind

#endif // TOUCHMIND_VIEW_NODE_NODEFIGURE_H_