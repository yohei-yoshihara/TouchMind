#ifndef TOUCHMIND_LAYOUT_ILAYOUTMANAGER_H_
#define TOUCHMIND_LAYOUT_ILAYOUTMANAGER_H_

#include "forwarddecl.h"

namespace touchmind {
  namespace layout {

    class ILayoutManager {
    public:
      virtual void SetConfiguration(Configuration *configuration) = 0;
      virtual Configuration *GetConfiguration() const = 0;
      virtual void SetTextSizeProvider(ITextSizeProvider *pTextSizeProvider) = 0;
      virtual void SetModel(const std::shared_ptr<model::node::NodeModel> &node) = 0;
      virtual void Arrange() = 0;
    };

  } // layout
} // touchmind

#endif // TOUCHMIND_LAYOUT_ILAYOUTMANAGER_H_