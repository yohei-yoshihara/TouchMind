#ifndef TOUCHMIND_VISITOR_NODEDRAWVISITOR_H_
#define TOUCHMIND_VISITOR_NODEDRAWVISITOR_H_

#include "forwarddecl.h"

namespace touchmind {
  namespace visitor {

    class NodeDrawVisitor {
    private:
      touchmind::Context *m_pContext;
      ID2D1RenderTarget *m_pRenderTarget;
      view::node::NodeViewManager *m_pNodeViewManager;
      std::shared_ptr<touchmind::model::node::NodeModel> m_edittingNode;

    public:
      NodeDrawVisitor(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
                      view::node::NodeViewManager *pNodeViewManager,
                      const std::shared_ptr<touchmind::model::node::NodeModel> &edittingNode)
          : m_pContext(pContext)
          , m_pRenderTarget(pRenderTarget)
          , m_pNodeViewManager(pNodeViewManager)
          , m_edittingNode(edittingNode) {
      }

      touchmind::VISITOR_RESULT operator()(const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
        if (m_edittingNode != node) {
          m_pNodeViewManager->DrawNode(m_pContext, m_pRenderTarget, node);
        }
        return touchmind::VISITOR_RESULT_CONTINUE;
      }
    };

  } // visitor
} // touchmind

#endif // TOUCHMIND_VISITOR_NODEDRAWVISITOR_H_