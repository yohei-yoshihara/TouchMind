#ifndef TOUCHMIND_VIEW_NODE_INODEVIEW_H_
#define TOUCHMIND_VIEW_NODE_INODEVIEW_H_

#include "forwarddecl.h"
#include "touchmind/view/BaseView.h"
#include "touchmind/control/DWriteEditControlCommon.h"
#include "touchmind/view/path/BasePathView.h"

namespace touchmind {
  namespace view {
    namespace node {

      class BaseNodeView : public touchmind::view::BaseView {
      private:
        std::weak_ptr<touchmind::model::node::NodeModel> m_node;
        touchmind::view::node::NodeViewManager *m_pNodeViewManager;
        touchmind::control::DWriteEditControlManager *m_pEditControlManager;
        touchmind::control::EDIT_CONTROL_INDEX m_editControlIndex;

      public:
        BaseNodeView(std::weak_ptr<touchmind::model::node::NodeModel> node);
        virtual ~BaseNodeView();
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
        void SetEditControlManager(touchmind::control::DWriteEditControlManager *pEditControlManager) {
          m_pEditControlManager = pEditControlManager;
        }
        touchmind::control::DWriteEditControlManager *GetEditControlManager() const {
          return m_pEditControlManager;
        }
        void SetEditControlIndex(touchmind::control::EDIT_CONTROL_INDEX editControlIndex) {
          m_editControlIndex = editControlIndex;
        }
        touchmind::control::EDIT_CONTROL_INDEX GetEditControlIndex() const {
          return m_editControlIndex;
        }
        virtual bool IsCompatible(const std::shared_ptr<touchmind::model::BaseModel> &model) = 0;

        virtual void CreateDeviceDependentResources(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget) = 0;
        virtual void DiscardDeviceDependentResources() override;
        virtual void Draw(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget) = 0;
        virtual bool HitTest(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget, D2D1_POINT_2F point) = 0;
      };

    } // node
  } // view
} // touchmind

#endif // TOUCHMIND_VIEW_NODE_INODEVIEW_H_