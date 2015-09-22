#ifndef TOUCHMIND_VIEW_PATH_IPATHVIEW_H_
#define TOUCHMIND_VIEW_PATH_IPATHVIEW_H_

#include "forwarddecl.h"
#include "touchmind/view/BaseView.h"

namespace touchmind {
  namespace view {
    namespace path {

      class BasePathView : public touchmind::view::BaseView {
      private:
        std::weak_ptr<touchmind::model::node::NodeModel> m_node;
        touchmind::view::node::NodeViewManager *m_pNodeViewManager;
        touchmind::Configuration *m_pConfiguration;

      public:
        BasePathView(std::weak_ptr<touchmind::model::node::NodeModel> node);
        virtual ~BasePathView();
        void SetNodeModel(std::weak_ptr<touchmind::model::node::NodeModel> node) {
          m_node = node;
        }
        std::weak_ptr<touchmind::model::node::NodeModel> GetNodeModel() const {
          return m_node;
        }
        bool HasNodeModel() const {
          return !m_node.expired() && m_node.lock() != nullptr;
        }
        void SetNodeViewManager(touchmind::view::node::NodeViewManager *pNodeViewManager) {
          m_pNodeViewManager = pNodeViewManager;
        }
        touchmind::view::node::NodeViewManager *GetNodeViewManager() const {
          return m_pNodeViewManager;
        }
        void SetConfiguration(touchmind::Configuration *pConfiguration) {
          m_pConfiguration = pConfiguration;
        }
        touchmind::Configuration *GetConfiguration() const {
          return m_pConfiguration;
        }
        virtual bool IsCompatible(const std::shared_ptr<touchmind::model::BaseModel> &model) = 0;
        virtual void CreateDeviceDependentResources(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget) = 0;
        virtual void DiscardDeviceDependentResources() = 0;
        virtual void Draw(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget) = 0;
        virtual bool HitTest(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget, D2D1_POINT_2F point) = 0;
      };

    } // path
  } // view
} // touchmind

#endif // TOUCHMIND_VIEW_PATH_IPATHVIEW_H_