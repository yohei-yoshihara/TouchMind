#include "stdafx.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/selection/SelectionManager.h"
#include "touchmind/selection/DefaultPropertyModel.h"

touchmind::LINK_ID touchmind::model::link::LinkModel::s_masterLinkId = 0;

std::shared_ptr<touchmind::model::link::LinkModel> touchmind::model::link::LinkModel::Create( touchmind::selection::SelectionManager *pSelectionManager )
{
    ASSERT(pSelectionManager != nullptr);
    auto link = std::make_shared<LinkModel>();
    link->SetSelectionManager(pSelectionManager);

    auto defaultProp = pSelectionManager->GetDefaultPropertyModel();
    link->SetLineColor(defaultProp->UpdateProperty_GetLineColor());
    link->SetLineWidth(defaultProp->UpdateProperty_GetLineWidth());
    link->SetLineStyle(defaultProp->UpdateProperty_GetLineStyle());
    link->SetLineEdgeStyle(defaultProp->UpdateProperty_GetLineEdgeStyle());

    return link;
}

touchmind::model::link::LinkModel::LinkModel():
    BaseModel(),
    SelectableSupport(),
    m_linkId(GenerateLinkId()),
    m_nodes(),
    m_nodeIds(),
    m_edges(),
    m_lineColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f)),
    m_lineWidth(LINE_WIDTH_1),
    m_lineStyle(LINE_STYLE_SOLID)
{
    m_edges[EDGE_ID_1] = std::make_shared<touchmind::model::linkedge::LinkEdgeModel>();
    m_edges[EDGE_ID_1]->SetLink(this);
    m_edges[EDGE_ID_2] = std::make_shared<touchmind::model::linkedge::LinkEdgeModel>();
    m_edges[EDGE_ID_2]->SetLink(this);

    AddSelectedListener([&]() {
        IncrementRepaintRequiredCounter();
    });
    AddUnselectedListener([&]() {
        IncrementRepaintRequiredCounter();
    });
}

touchmind::model::link::LinkModel::LinkModel(const LinkModel& other) :
    BaseModel(other),
    SelectableSupport(other),
    m_linkId(other.m_linkId),
    m_nodes(other.m_nodes),
    m_nodeIds(other.m_nodeIds),
    m_edges(), // copy them later
    m_lineColor(other.m_lineColor),
    m_lineWidth(other.m_lineWidth),
    m_lineStyle(other.m_lineStyle)
{
    m_edges[EDGE_ID_1] = std::make_shared<linkedge::LinkEdgeModel>(*other.m_edges[EDGE_ID_1]);
    m_edges[EDGE_ID_2] = std::make_shared<linkedge::LinkEdgeModel>(*other.m_edges[EDGE_ID_2]);
    m_edges[EDGE_ID_1]->SetLink(this);
    m_edges[EDGE_ID_2]->SetLink(this);
    AddSelectedListener([&]() {
        IncrementRepaintRequiredCounter();
    });
    AddUnselectedListener([&]() {
        IncrementRepaintRequiredCounter();
    });
}

D2D1_POINT_2F touchmind::model::link::LinkModel::_CalculateLinkJointPosition(
    const std::shared_ptr<touchmind::model::node::NodeModel> &node) const
{
    D2D1_POINT_2F p = {0.0f, (node->GetY() + node->GetHeight()) / 2.0f};
    if (node->GetPosition() == NODE_SIDE_RIGHT) {
        p.x = node->GetX() + node->GetWidth();
    } else {
        p.x = node->GetX();
    }
    return p;
}

void touchmind::model::link::LinkModel::GetControlPoints(std::array<D2D1_POINT_2F, 4> controlPoints) const
{
    if (IsValid()) {
        std::shared_ptr<touchmind::model::node::NodeModel> node1 = m_nodes[0].lock();
        std::shared_ptr<touchmind::model::node::NodeModel> node2 = m_nodes[1].lock();
        controlPoints[0] = _CalculateLinkJointPosition(node1);
        controlPoints[1] = m_edges[0]->CalculateHandlePosition(controlPoints[0], node1->GetPosition());
        controlPoints[2] = m_edges[1]->CalculateHandlePosition(controlPoints[0], node1->GetPosition());
        controlPoints[3] = _CalculateLinkJointPosition(node2);
    }
}

FLOAT touchmind::model::link::LinkModel::GetLineWidthAsValue() const
{
    switch (m_lineWidth) {
    case LINE_WIDTH_1:
        return 1.0f;
    case LINE_WIDTH_3:
        return 3.0f;
    case LINE_WIDTH_5:
        return 5.0f;
    case LINE_WIDTH_8:
        return 8.0f;
    default:
        return 1.0f;
    }
}

std::shared_ptr<touchmind::selection::SelectableSupport> touchmind::model::link::LinkModel::GetSelectableSharedPtr()
{
    return std::dynamic_pointer_cast<touchmind::selection::SelectableSupport>(shared_from_this());
}

void touchmind::model::link::LinkModel::SetLineEdgeStyle( LINE_EDGE_STYLE lineEdgeStyle )
{
    int edge1 = (static_cast<int>(lineEdgeStyle) & 0xf0) >> 4;
    int edge2 = static_cast<int>(lineEdgeStyle) & 0x0f;
    GetEdge(EDGE_ID_1)->SetStyle(static_cast<EDGE_STYLE>(edge1));
    GetEdge(EDGE_ID_2)->SetStyle(static_cast<EDGE_STYLE>(edge2));
}

touchmind::LINE_EDGE_STYLE touchmind::model::link::LinkModel::GetLineEdgeStyle() const
{
    int edge1 = GetEdge(EDGE_ID_1)->GetStyle();
    int edge2 = GetEdge(EDGE_ID_2)->GetStyle();
    return static_cast<LINE_EDGE_STYLE>(edge1 << 4 | edge2);
}

void touchmind::model::link::LinkModel::IncrementEdgesRepaintRequiredCounter()
{
    m_edges[EDGE_ID_1]->IncrementRepaintRequiredCounter();
    m_edges[EDGE_ID_2]->IncrementRepaintRequiredCounter();
}

void touchmind::model::link::LinkModel::SetNode( EDGE_ID edgeId, const std::shared_ptr<touchmind::model::node::NodeModel> &node )
{
    if (node != nullptr) {
        m_nodes[edgeId] = node;
        m_nodeIds[edgeId] = node->GetId();
    } else {
        m_nodes[edgeId] = node;
        m_nodeIds[edgeId] = -1;
    }
    IncrementRepaintRequiredCounter();
    IncrementSaveRequiredCounter();
}

D2D1_POINT_2F touchmind::model::link::LinkModel::GetEdgePoint( EDGE_ID edgeId ) const
{
    auto node = GetNode(edgeId);
    return D2D1::Point2F(
               node->GetAncestorPosition() == NODE_SIDE_RIGHT ? node->GetX() + node->GetWidth() + 2.0f : node->GetX() - 2.0f,
               node->GetY() + node->GetHeight() / 2.0f);
}
