#ifndef TOUCHMIND_VIEW_LINKEDGE_IMPL_ARROWLINKEDGEVIEW_H_
#define TOUCHMIND_VIEW_LINKEDGE_IMPL_ARROWLINKEDGEVIEW_H_

#include "touchmind/view/linkedge/BaseLinkEdgeView.h"

namespace touchmind {
  namespace view {
    namespace linkedge {
      namespace impl {

        class ArrowLinkEdgeView : public touchmind::view::linkedge::BaseLinkEdgeView,
                                  public std::enable_shared_from_this<ArrowLinkEdgeView> {
        private:
          CComPtr<ID2D1PathGeometry> m_pArrowGeometry;
          CComPtr<ID2D1SolidColorBrush> m_pBrush;

        public:
          ArrowLinkEdgeView(std::weak_ptr<touchmind::model::link::LinkModel> link, EDGE_ID edgeId);
          virtual ~ArrowLinkEdgeView();
          virtual bool IsCompatible(const std::shared_ptr<touchmind::model::BaseModel> &model) override;
          virtual void CreateDeviceDependentResources(touchmind::Context *pContext,
                                                      ID2D1RenderTarget *pRenderTarget) override;
          virtual void DiscardDeviceDependentResources() override;
          virtual void Draw(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget, ID2D1Brush *pBrush,
                            FLOAT width = 1.0f) override;
          virtual void Fill(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
                            ID2D1Brush *pBrush = nullptr) override;
          virtual bool HitTest(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
                               D2D1_POINT_2F point) override;
          virtual EDGE_STYLE GetEdgeStyle() const override {
            return EDGE_STYLE_ARROW;
          }
        };

      } // impl
    } // linkedge
  } // view
} // touchmind

#endif // TOUCHMIND_VIEW_LINK_IMPL_ARROWLINKEDGEVIEW_H_