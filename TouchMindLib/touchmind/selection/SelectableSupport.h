#ifndef TOUCHMIND_SELECTION_SELECTABLESUPPORT_H_
#define TOUCHMIND_SELECTION_SELECTABLESUPPORT_H_

#include "forwarddecl.h"

namespace touchmind {
  namespace selection {

    typedef std::function<void(void)> SelectedListener;
    typedef std::function<void(void)> UnselectedListener;

    class SelectableSupport {
    private:
      SelectionManager *m_selectionManager;
      bool m_selected;
      std::vector<SelectedListener> m_selectedListeners;
      std::vector<UnselectedListener> m_unselectedListeners;

    protected:
      virtual std::shared_ptr<SelectableSupport> GetSelectableSharedPtr() = 0;

    public:
      SelectableSupport()
          : m_selectionManager(nullptr)
          , m_selected(false)
          , m_selectedListeners()
          , m_unselectedListeners() {
      }
      SelectableSupport(SelectionManager *selectionManager)
          : m_selectionManager(selectionManager)
          , m_selected(false)
          , m_selectedListeners()
          , // not copy
          m_unselectedListeners() // not copy
      {
      }
      SelectableSupport(const SelectableSupport &other)
          : m_selectionManager(other.m_selectionManager)
          , m_selected(false)
          , m_selectedListeners()
          , // not copy
          m_unselectedListeners() // not copy
      {
      }
      SelectableSupport &operator=(const SelectableSupport &other) {
        if (this != &other) {
          m_selectionManager = other.m_selectionManager;
          m_selected = false;
        }
        return *this;
      }
      virtual ~SelectableSupport() {
      }
      void SetSelectionManager(SelectionManager *selectionManager) {
        m_selectionManager = selectionManager;
      }
      void SetSelected();
      void SetUnselected();
      bool IsSelected() const {
        return m_selected;
      }
      void AddSelectedListener(SelectedListener l) {
        m_selectedListeners.push_back(l);
      }
      void AddUnselectedListener(UnselectedListener l) {
        m_unselectedListeners.push_back(l);
      }
    };

  } // selection
} // touchmind

#endif // TOUCHMIND_SELECTION_SELECTABLESUPPORT_H_