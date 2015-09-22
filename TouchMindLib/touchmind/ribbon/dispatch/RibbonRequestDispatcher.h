#ifndef TOUCHMIND_RIBBON_PROP_PROPERTYMANAGER_H_
#define TOUCHMIND_RIBBON_PROP_PROPERTYMANAGER_H_

#include "forwarddecl.h"
#include "touchmind/Common.h"
#include "touchmind/selection/SelectableSupport.h"
#include "touchmind/selection/SelectionManager.h"

namespace touchmind {
  namespace ribbon {
    namespace dispatch {
      typedef std::function<void(touchmind::ribbon::RibbonFramework *pRibbonFramework,
                                 std::shared_ptr<touchmind::selection::SelectableSupport> oldSelectedItem,
                                 std::shared_ptr<touchmind::selection::SelectableSupport> newSelectedItem)>
          RibbonSelectionChangeListener;

      typedef std::function<void()> InvalidateCallback;

      class RibbonRequestDispatcher {
      private:
        touchmind::selection::SelectionManager *m_pSelectionManager;
        touchmind::ribbon::RibbonFramework *m_pRibbonFramework;
        std::vector<touchmind::ribbon::dispatch::RibbonSelectionChangeListener> m_selectionChangedListeners;
        std::vector<touchmind::ribbon::dispatch::InvalidateCallback> m_invalidateCallbacks;

      protected:
        void _FireInvalidateCallback();

      public:
        RibbonRequestDispatcher()
            : m_pSelectionManager(nullptr)
            , m_pRibbonFramework(nullptr)
            , m_selectionChangedListeners() {
        }
        virtual ~RibbonRequestDispatcher() {
        }
        void SetSelectionManager(touchmind::selection::SelectionManager *pSelectionManager) {
          m_pSelectionManager = pSelectionManager;
        }
        void SetRibbonFramework(touchmind::ribbon::RibbonFramework *pRibbonFramework) {
          m_pRibbonFramework = pRibbonFramework;
        }
        void AddSelectionChangedListner(touchmind::ribbon::dispatch::RibbonSelectionChangeListener l) {
          m_selectionChangedListeners.push_back(l);
        }
        void AddInvalidateCallback(touchmind::ribbon::dispatch::InvalidateCallback f) {
          m_invalidateCallbacks.push_back(f);
        }

        void SelectionChanged(std::shared_ptr<touchmind::selection::SelectableSupport> oldSelectedItem,
                              std::shared_ptr<touchmind::selection::SelectableSupport> newSelectedItem);

        // ILinkProperty interface
        bool UpdateProperty_IsLineColorChangeable() const;
        bool UpdateProperty_IsLineWidthChangeable() const;
        bool UpdateProperty_IsLineStyleChangeable() const;
        bool UpdateProperty_IsLineEdgeStyleChangeable() const;
        void Execute_LineColor(UI_EXECUTIONVERB verb, D2D1_COLOR_F color);
        void Execute_LineWidth(UI_EXECUTIONVERB verb, LINE_WIDTH width);
        void Execute_LineStyle(UI_EXECUTIONVERB verb, LINE_STYLE style);
        void Execute_LineEdgeStyle(UI_EXECUTIONVERB verb, LINE_EDGE_STYLE style);
        D2D1_COLOR_F UpdateProperty_GetLineColor();
        LINE_WIDTH UpdateProperty_GetLineWidth();
        LINE_STYLE UpdateProperty_GetLineStyle();
        LINE_EDGE_STYLE UpdateProperty_GetLineEdgeStyle();

        // INodeProperty interface
        bool UpdateProperty_IsNodeBackgroundColorChangeable() const;
        bool UpdateProperty_IsNodeShapeChangeable() const;
        void Execute_NodeBackgroundColor(UI_EXECUTIONVERB verb, D2D1_COLOR_F color);
        void Execute_NodeShape(UI_EXECUTIONVERB verb, NODE_SHAPE nodeShape);
        D2D1_COLOR_F UpdateProperty_GetNodeBackgroundColor();
        NODE_SHAPE UpdateProperty_GetNodeShape();
      };

    } // dispatch
  } // ribbon
} // touchmind

#endif // TOUCHMIND_RIBBON_PROP_PROPERTYMANAGER_H_