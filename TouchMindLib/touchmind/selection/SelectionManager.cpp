#include "stdafx.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/path/PathModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/selection/SelectableSupport.h"
#include "touchmind/selection/SelectionManager.h"
#include "touchmind/selection/DefaultPropertyModel.h"

touchmind::selection::SelectionManager::SelectionManager() :
    m_selected(),
    m_selectionChangeListeners(),
    m_defaultPropertyModel(std::make_shared<touchmind::selection::DefaultPropertyModel>())
{
}

void touchmind::selection::SelectionManager::NotifySelected(const std::shared_ptr<SelectableSupport> &selectedItem)
{
    std::shared_ptr<SelectableSupport> oldItem = m_selected;
    std::shared_ptr<SelectableSupport> newItem = selectedItem;

    if (m_selected != nullptr) {
        m_selected->SetUnselected();
    }
    m_selected = selectedItem;

    for (auto it = std::begin(m_selectionChangeListeners); it != std::end(m_selectionChangeListeners); ++it) {
        (*it)(oldItem, newItem);
    }
}

void touchmind::selection::SelectionManager::NotifyUnselected(const std::shared_ptr<SelectableSupport> &unselectedItem)
{
    if (m_selected == unselectedItem) {
        m_selected = nullptr;

        for (auto it = std::begin(m_selectionChangeListeners); it != std::end(m_selectionChangeListeners); ++it) {
            (*it)(unselectedItem, nullptr);
        }
    }
}

std::shared_ptr<touchmind::selection::SelectableSupport> touchmind::selection::SelectionManager::GetSelected() const
{
    if (m_selected != nullptr) {
        return m_selected;
    } else {
        return m_defaultPropertyModel;
    }
}

std::shared_ptr<touchmind::ribbon::dispatch::INodeProperty> touchmind::selection::SelectionManager::GetSelectedNodeProperty() const
{
    return std::dynamic_pointer_cast<touchmind::ribbon::dispatch::INodeProperty>(GetSelected());
}

std::shared_ptr<touchmind::ribbon::dispatch::ILineProperty> touchmind::selection::SelectionManager::GetSelectedLineProperty() const
{
    return std::dynamic_pointer_cast<touchmind::ribbon::dispatch::ILineProperty>(GetSelected());
}

void touchmind::selection::SelectionManager::ClearSelected()
{
    if (m_selected != nullptr) {
        m_selected->SetUnselected();
    }
}

