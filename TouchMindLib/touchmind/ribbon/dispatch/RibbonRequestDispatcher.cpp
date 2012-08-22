#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/ribbon/dispatch/INodeProperty.h"
#include "touchmind/ribbon/dispatch/ILineProperty.h"
#include "touchmind/ribbon/dispatch/RibbonRequestDispatcher.h"
#include "touchmind/ribbon/RibbonFramework.h"

void touchmind::ribbon::dispatch::RibbonRequestDispatcher::SelectionChanged(
    std::shared_ptr<touchmind::selection::SelectableSupport> oldSelectedItem,
    std::shared_ptr<touchmind::selection::SelectableSupport> newSelectedItem)
{
    ASSERT(m_pRibbonFramework != nullptr);
    for (auto it = std::begin(m_selectionChangedListeners); it != std::end(m_selectionChangedListeners); ++it) {
        (*it)(m_pRibbonFramework, oldSelectedItem, newSelectedItem);
    }
}

bool touchmind::ribbon::dispatch::RibbonRequestDispatcher::UpdateProperty_IsLineColorChangeable() const
{
    if (m_pSelectionManager == nullptr) {
        return false;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto line = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::ILineProperty>(selectable);
    if (line != nullptr) {
        return line->UpdateProperty_IsLineColorChangeable();
    }
    return false;
}

bool touchmind::ribbon::dispatch::RibbonRequestDispatcher::UpdateProperty_IsLineWidthChangeable() const
{
    if (m_pSelectionManager == nullptr) {
        return false;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto line = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::ILineProperty>(selectable);
    if (line != nullptr) {
        return line->UpdateProperty_IsLineWidthChangeable();
    }
    return false;
}

bool touchmind::ribbon::dispatch::RibbonRequestDispatcher::UpdateProperty_IsLineStyleChangeable() const
{
    if (m_pSelectionManager == nullptr) {
        return false;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto line = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::ILineProperty>(selectable);
    if (line != nullptr) {
        return line->UpdateProperty_IsLineStyleChangeable();
    }
    return false;
}

bool touchmind::ribbon::dispatch::RibbonRequestDispatcher::UpdateProperty_IsLineEdgeStyleChangeable() const
{
    if (m_pSelectionManager == nullptr) {
        return false;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto line = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::ILineProperty>(selectable);
    if (line != nullptr) {
        return line->UpdateProperty_IsLineEdgeStyleChangeable();
    }
    return false;
}

void touchmind::ribbon::dispatch::RibbonRequestDispatcher::Execute_LineColor(UI_EXECUTIONVERB verb, D2D1_COLOR_F color)
{
    if (m_pSelectionManager == nullptr) {
        return;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto line = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::ILineProperty>(selectable);
    if (line != nullptr) {
        line->Execute_LineColor(verb, color);
    }
    _FireInvalidateCallback();
}

void touchmind::ribbon::dispatch::RibbonRequestDispatcher::Execute_LineWidth(UI_EXECUTIONVERB verb, LINE_WIDTH width)
{
    if (m_pSelectionManager == nullptr) {
        return;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto line = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::ILineProperty>(selectable);
    if (line != nullptr) {
        line->Execute_LineWidth(verb, width);
    }
    _FireInvalidateCallback();
}

void touchmind::ribbon::dispatch::RibbonRequestDispatcher::Execute_LineStyle(UI_EXECUTIONVERB verb, LINE_STYLE style)
{
    if (m_pSelectionManager == nullptr) {
        return;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto line = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::ILineProperty>(selectable);
    if (line != nullptr) {
        line->Execute_LineStyle(verb, style);
    }
    _FireInvalidateCallback();
}

void touchmind::ribbon::dispatch::RibbonRequestDispatcher::Execute_LineEdgeStyle( UI_EXECUTIONVERB verb, LINE_EDGE_STYLE style )
{
    if (m_pSelectionManager == nullptr) {
        return;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto line = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::ILineProperty>(selectable);
    if (line != nullptr) {
        line->Execute_LineEdgeStyle(verb, style);
    }
    _FireInvalidateCallback();
}

D2D1_COLOR_F touchmind::ribbon::dispatch::RibbonRequestDispatcher::UpdateProperty_GetLineColor()
{
    if (m_pSelectionManager == nullptr) {
        return D2D1::ColorF(D2D1::ColorF::Black);
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto line = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::ILineProperty>(selectable);
    if (line != nullptr) {
        return line->UpdateProperty_GetLineColor();
    }
    return D2D1::ColorF(D2D1::ColorF::Black);
}

touchmind::LINE_WIDTH touchmind::ribbon::dispatch::RibbonRequestDispatcher::UpdateProperty_GetLineWidth()
{
    if (m_pSelectionManager == nullptr) {
        return LINE_WIDTH_UNSPECIFIED;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto line = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::ILineProperty>(selectable);
    if (line != nullptr) {
        return line->UpdateProperty_GetLineWidth();
    }
    return LINE_WIDTH_UNSPECIFIED;
}

touchmind::LINE_STYLE touchmind::ribbon::dispatch::RibbonRequestDispatcher::UpdateProperty_GetLineStyle()
{
    if (m_pSelectionManager == nullptr) {
        return LINE_STYLE_UNSPECIFIED;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto line = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::ILineProperty>(selectable);
    if (line != nullptr) {
        return line->UpdateProperty_GetLineStyle();
    }
    return LINE_STYLE_UNSPECIFIED;
}

touchmind::LINE_EDGE_STYLE touchmind::ribbon::dispatch::RibbonRequestDispatcher::UpdateProperty_GetLineEdgeStyle()
{
    if (m_pSelectionManager == nullptr) {
        return LINE_EDGE_STYLE_UNSPECIFIED;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto line = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::ILineProperty>(selectable);
    if (line != nullptr) {
        return line->UpdateProperty_GetLineEdgeStyle();
    }
    return LINE_EDGE_STYLE_UNSPECIFIED;
}

bool touchmind::ribbon::dispatch::RibbonRequestDispatcher::UpdateProperty_IsNodeBackgroundColorChangeable() const
{
    if (m_pSelectionManager == nullptr) {
        return false;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto node = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::INodeProperty>(selectable);
    if (node != nullptr) {
        return node->UpdateProperty_IsNodeBackgroundColorChangeable();
    }
    return false;
}

bool touchmind::ribbon::dispatch::RibbonRequestDispatcher::UpdateProperty_IsNodeShapeChangeable() const
{
    if (m_pSelectionManager == nullptr) {
        return false;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto node = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::INodeProperty>(selectable);
    if (node != nullptr) {
        return node->UpdateProperty_IsNodeShapeChangeable();
    }
    return false;
}

void touchmind::ribbon::dispatch::RibbonRequestDispatcher::Execute_NodeBackgroundColor( UI_EXECUTIONVERB verb, D2D1_COLOR_F color )
{
    if (m_pSelectionManager == nullptr) {
        return;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto node = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::INodeProperty>(selectable);
    if (node != nullptr) {
        node->Execute_NodeBackgroundColor(verb, color);
    }
    _FireInvalidateCallback();
}

void touchmind::ribbon::dispatch::RibbonRequestDispatcher::Execute_NodeShape( UI_EXECUTIONVERB verb, NODE_SHAPE nodeShape )
{
    if (m_pSelectionManager == nullptr) {
        return;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto node = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::INodeProperty>(selectable);
    if (node != nullptr) {
        node->Execute_NodeShape(verb, nodeShape);
    }
    _FireInvalidateCallback();
}

D2D1_COLOR_F touchmind::ribbon::dispatch::RibbonRequestDispatcher::UpdateProperty_GetNodeBackgroundColor()
{
    if (m_pSelectionManager == nullptr) {
        return D2D1::ColorF(D2D1::ColorF::White);
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto node = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::INodeProperty>(selectable);
    if (node != nullptr) {
        return node->UpdateProperty_GetNodeBackgroundColor();
    }
    return D2D1::ColorF(D2D1::ColorF::Black);
}

touchmind::NODE_SHAPE touchmind::ribbon::dispatch::RibbonRequestDispatcher::UpdateProperty_GetNodeShape()
{
    if (m_pSelectionManager == nullptr) {
        return NODE_SHAPE_UNSPECIFIED;
    }
    auto selectable = m_pSelectionManager->GetSelected();
    auto node = std::dynamic_pointer_cast<touchmind::ribbon::dispatch::INodeProperty>(selectable);
    if (node != nullptr) {
        return node->UpdateProperty_GetNodeShape();
    }
    return NODE_SHAPE_UNSPECIFIED;
}

void touchmind::ribbon::dispatch::RibbonRequestDispatcher::_FireInvalidateCallback()
{
    for (auto it = std::begin(m_invalidateCallbacks); it != std::end(m_invalidateCallbacks); ++it) {
        (*it)();
    }
}
