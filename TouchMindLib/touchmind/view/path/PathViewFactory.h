#ifndef TOUCHMIND_VIEW_PATH_PATHVIEWFACTORY_H_
#define TOUCHMIND_VIEW_PATH_PATHVIEWFACTORY_H_

#include "forwarddecl.h"

namespace touchmind {
  namespace view {
    namespace path {

      class PathViewFactory {
      private:
        PathViewFactory() {
        }

      public:
        static std::shared_ptr<view::path::BasePathView> Create(const std::shared_ptr<model::node::NodeModel> &node);
      };

    } // path
  } // view
} // touchmin

#endif // TOUCHMIND_VIEW_PATH_PATHVIEWFACTORY_H_