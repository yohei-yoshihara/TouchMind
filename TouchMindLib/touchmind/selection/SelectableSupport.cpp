#include "stdafx.h"
#include "touchmind/selection/SelectableSupport.h"
#include "touchmind/selection/SelectionManager.h"

void touchmind::selection::SelectableSupport::SetSelected()
{
    m_selectionManager->NotifySelected(GetSelectableSharedPtr());
    for (auto it = std::begin(m_selectedListeners); it != std::end(m_selectedListeners); ++it) {
        (*it)();
    }
    m_selected = true;
}

void touchmind::selection::SelectableSupport::SetUnselected()
{
    auto selectable = GetSelectableSharedPtr();
    m_selectionManager->NotifyUnselected(selectable);
    for (auto it = std::begin(m_unselectedListeners); it != std::end(m_unselectedListeners); ++it) {
        (*it)();
    }
    m_selected = false;
}
