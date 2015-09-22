#ifndef TOUCHMIND_VIEW_NODE_IMPL_BASENODEVIEW_H_
#define TOUCHMIND_VIEW_NODE_IMPL_BASENODEVIEW_H_

#include "forwarddecl.h"
#include "touchmind/view/node/BaseNodeView.h"

namespace touchmind {
  namespace view {
    namespace node {
      namespace impl {

        class RectangleNodeView : public touchmind::view::node::BaseNodeView {
        private:
          CComPtr<ID3D10Texture2D> m_pTexture;

        public:
          RectangleNodeView(std::weak_ptr<touchmind::model::node::NodeModel> node);
          virtual ~RectangleNodeView();
          virtual bool IsCompatible(const std::shared_ptr<touchmind::model::BaseModel> &model) override;
          virtual void CreateDeviceDependentResources(touchmind::Context *pContext,
                                                      ID2D1RenderTarget *pRenderTarget) override;
          virtual void DiscardDeviceDependentResources() override;
          virtual void Draw(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget) override;
          virtual bool HitTest(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
                               D2D1_POINT_2F point) override;
        };

      } // impl
    } // node
  } // view
} // touchmind

#endif // TOUCHMIND_VIEW_NODE_IMPL_BASENODEVIEW_H_