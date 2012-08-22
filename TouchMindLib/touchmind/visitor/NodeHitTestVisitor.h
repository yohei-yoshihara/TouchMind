#ifndef TOUCHMIND_VISITOR_NODEHITTESTVISITOR_H_
#define TOUCHMIND_VISITOR_NODEHITTESTVISITOR_H_

#include "forwarddecl.h"
#include "touchmind/model/node/NodeModel.h"

namespace touchmind
{
namespace visitor
{

class NodeHitTestVisitor
{
private:
    D2D1_POINT_2F m_clientPoint;
    FLOAT m_margin;
    std::shared_ptr<touchmind::model::node::NodeModel> m_result;

public:
    NodeHitTestVisitor(D2D1_POINT_2F clientPoint, FLOAT margin) :
        m_clientPoint(clientPoint),
        m_margin(margin),
        m_result()
    {}

    touchmind::VISITOR_RESULT operator()(const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
        if (((!node->IsCollapsed() && !node->IsAncestorCollapsed()) || (node->IsCollapsed() && !node->IsAncestorCollapsed())) &&
                (node->GetX() - m_margin) <= m_clientPoint.x && m_clientPoint.x <= (node->GetX() + node->GetWidth() + m_margin) &&
                (node->GetY() - m_margin) <= m_clientPoint.y && m_clientPoint.y <= (node->GetY() + node->GetHeight() + m_margin)) {
            m_result = node;
            return touchmind::VISITOR_RESULT_STOP;
        }
        return touchmind::VISITOR_RESULT_CONTINUE;
    }

    std::shared_ptr<touchmind::model::node::NodeModel> GetResult() {
        return m_result;
    }
};

} // util
} // touchmind

#endif // TOUCHMIND_VISITOR_NODEHITTESTVISITOR_H_