#include "stdafx.h"
#include "touchmind/Context.h"
#include "touchmind/Configuration.h"
#include "touchmind/model/Insets.h"
#include "touchmind/touch/ManipulationHelper.h"
#include "touchmind/operation/NodeResizingOperation.h"

touchmind::operation::NodeResizingOperation::NodeResizingOperation() :
    m_pConfiguration(nullptr),
    m_pManipulationHelper(nullptr),
    m_pResizingFigureBrush(nullptr)
{
}

touchmind::operation::NodeResizingOperation::~NodeResizingOperation()
{
}

void touchmind::operation::NodeResizingOperation::CreateDeviceDependentResources( touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget )
{
    UNREFERENCED_PARAMETER(pContext);
    m_pResizingFigureBrush = nullptr;
    CHK_RES(m_pResizingFigureBrush, pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(1.0f, 0.5f, 0.0f, 0.5f),
                &m_pResizingFigureBrush));
}

void touchmind::operation::NodeResizingOperation::Draw( touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget )
{
    UNREFERENCED_PARAMETER(pContext);
    auto rect = m_pManipulationHelper->GetResizingNodeRect();
    Insets insets = m_pConfiguration->GetInsets();
    D2D1_RECT_F _rect = D2D1::RectF(
                            rect.left - insets.left,
                            rect.top - insets.top,
                            rect.right + insets.right,
                            rect.bottom + insets.bottom);
    pRenderTarget->DrawRectangle(_rect, m_pResizingFigureBrush, 5.0f);
}
