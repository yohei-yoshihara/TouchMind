#include "stdafx.h"
#include "touchmind/touch/ManipulationHelper.h"
#include "touchmind/util/CoordinateConversion.h"
#include "touchmind/view/node/NodeViewManager.h"
#include "touchmind/operation/NodeMovingOperation.h"
#include "touchmind/visitor/NodeDrawVisitor.h"

touchmind::operation::NodeMovingOperation::NodeMovingOperation() :
    m_hwnd(0),
    m_pConfiguration(nullptr),
    m_pManipulationHelper(nullptr),
    m_pNodeViewManager(nullptr),
    m_pScrollBarHelper(nullptr)
{
}

touchmind::operation::NodeMovingOperation::~NodeMovingOperation()
{
}

void touchmind::operation::NodeMovingOperation::Draw(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget,
    std::shared_ptr<model::node::NodeModel> edittingNode)
{
    auto movingNode = m_pManipulationHelper->GetMovingNode();
    FLOAT centerX = movingNode->GetX() + movingNode->GetWidth() / 2.0f;
    FLOAT centerY = movingNode->GetY() + movingNode->GetHeight() / 2.0f;

    POINT cursorPoint;
    D2D1_POINT_2F clientCursorPoint;
    if (m_pManipulationHelper->GetManipulationStatus() == touch::MANIPULATION_STATUS_MOVING_NODE) {
        ::GetCursorPos(&cursorPoint);
    } else {
        cursorPoint.x = m_pManipulationHelper->GetPanMovingPoint().x;
        cursorPoint.y = m_pManipulationHelper->GetPanMovingPoint().y;
    }
    util::CoordinateConversion::ConvertScreenToModelCoordinate(m_hwnd, m_pScrollBarHelper, cursorPoint, &clientCursorPoint);

    // Create a layer.
    CComPtr<ID2D1Layer> pLayer = nullptr;
    CHK_RES(pLayer, pRenderTarget->CreateLayer(nullptr, &pLayer));

    D2D1_MATRIX_3X2_F transformMatrixBackup;
    pRenderTarget->GetTransform(&transformMatrixBackup);

    pRenderTarget->SetTransform(transformMatrixBackup * D2D1::Matrix3x2F::Translation(-centerX, -centerY)
                                * D2D1::Matrix3x2F::Translation(clientCursorPoint.x, clientCursorPoint.y));

    // Push the layer with the content bounds.
    pRenderTarget->PushLayer(
        D2D1::LayerParameters(
            D2D1::InfiniteRect(),
            nullptr,
            D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
            D2D1::IdentityMatrix(),
            0.5f,
            nullptr,
            D2D1_LAYER_OPTIONS_NONE),
        pLayer
    );

    // Draw paths
    movingNode->ApplyVisitor([&](model::node::NodeModel::reference node)->VISITOR_RESULT {
        m_pNodeViewManager->DrawNodePaths(pContext, pRenderTarget, node);
        return touchmind::VISITOR_RESULT_CONTINUE;
    });

    // Draw nodes
    visitor::NodeDrawVisitor nodeDrawVisitor(
        pContext,
        pRenderTarget,
        m_pNodeViewManager,
        edittingNode);
    movingNode->ApplyVisitor(nodeDrawVisitor);
    if (edittingNode != nullptr) {
        m_pNodeViewManager->DrawNode(pContext, pRenderTarget, edittingNode);
    }
    pRenderTarget->PopLayer();
    pRenderTarget->SetTransform(transformMatrixBackup);
}
