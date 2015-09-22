#include "stdafx.h"
#include "touchmind/selection/SelectableSupport.h"
#include "touchmind/selection/SelectionManager.h"

void touchmind::selection::SelectableSupport::SetSelected() {
  m_selectionManager->NotifySelected(GetSelectableSharedPtr());
  for (auto &l : m_selectedListeners) {
    l();
  }
  m_selected = true;
}

void touchmind::selection::SelectableSupport::SetUnselected() {
  auto selectable = GetSelectableSharedPtr();
  m_selectionManager->NotifyUnselected(selectable);
  for (auto &l : m_unselectedListeners) {
    l();
  }
  m_selected = false;
}
