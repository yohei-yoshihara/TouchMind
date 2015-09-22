#ifndef TOUCHMIND_MODEL_NODEMODELTOTEXTCONVERTER_H_
#define TOUCHMIND_MODEL_NODEMODELTOTEXTCONVERTER_H_

#include "forwarddecl.h"

namespace touchmind {
  namespace converter {

    class NodeModelToTextConverter {
    private:
      touchmind::selection::SelectionManager *m_pSelectionManager;

    public:
      NodeModelToTextConverter(void);
      ~NodeModelToTextConverter(void);
      void SetSelectionManager(touchmind::selection::SelectionManager *pSelectionManager) {
        m_pSelectionManager = pSelectionManager;
      }
      std::shared_ptr<touchmind::model::node::NodeModel> FromText(IN const std::wstring &text);
      void ToText(IN const std::shared_ptr<touchmind::model::node::NodeModel> &node, OUT std::wstring &text);
    };

  } // converter
} // touchmind

#endif // TOUCHMIND_MODEL_NODEMODELTOTEXTCONVERTER_H_