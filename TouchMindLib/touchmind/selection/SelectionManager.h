#ifndef TOUCHMIND_SELECTION_SELECTIONMANAGER_H_
#define TOUCHMIND_SELECTION_SELECTIONMANAGER_H_

#include "forwarddecl.h"
#include "touchmind/ribbon/dispatch/INodeProperty.h"
#include "touchmind/ribbon/dispatch/ILineProperty.h"

namespace touchmind {
  namespace selection {

    typedef std::function<void(std::shared_ptr<SelectableSupport> oldSelectedItem,
                               std::shared_ptr<SelectableSupport> newSelectedItem)> SelectionChangeListener;

    class SelectionManager {
    private:
      std::shared_ptr<SelectableSupport> m_selected;
      std::vector<SelectionChangeListener> m_selectionChangeListeners;
      std::shared_ptr<DefaultPropertyModel> m_defaultPropertyModel;

    public:
      SelectionManager();
      virtual ~SelectionManager() {
      }
      void ClearSelected();
      std::shared_ptr<SelectableSupport> GetSelected() const;
      std::shared_ptr<DefaultPropertyModel> GetDefaultPropertyModel() const {
        return m_defaultPropertyModel;
      }
      bool HasSelected() const {
        return m_selected != nullptr;
      }
      bool IsNodePropertySelected() const {
        return GetSelectedNodeProperty() != nullptr;
      }
      bool IsLinePropertySelected() const {
        return GetSelectedLineProperty() != nullptr;
      }
      std::shared_ptr<touchmind::ribbon::dispatch::INodeProperty> GetSelectedNodeProperty() const;
      std::shared_ptr<touchmind::ribbon::dispatch::ILineProperty> GetSelectedLineProperty() const;
      void NotifySelected(const std::shared_ptr<SelectableSupport> &selectedItem);
      void NotifyUnselected(const std::shared_ptr<SelectableSupport> &unselectedItem);
      void AddSelectionChangeListener(SelectionChangeListener l) {
        m_selectionChangeListeners.push_back(l);
      }
    };

  } // selection
} // touchmind

#endif // TOUCHMIND_SELECTION_SELECTIONMANAGER_H_