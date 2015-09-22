#ifndef TOUCHMIND_VIEW_LINKEDGE_LINKEDGEVIEWFACTORY_H_
#define TOUCHMIND_VIEW_LINKEDGE_LINKEDGEVIEWFACTORY_H_

#include "forwarddecl.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/view/linkedge/BaseLinkEdgeView.h"

namespace touchmind {
  namespace view {
    namespace linkedge {

      class LinkEdgeViewFactory {
      private:
        LinkEdgeViewFactory() {
        }

      public:
        static std::shared_ptr<touchmind::view::linkedge::BaseLinkEdgeView>
        Create(std::shared_ptr<touchmind::model::link::LinkModel> link, EDGE_ID edgeId);
      };

    } // linkedge
  } // view
} // touchmind

#endif // TOUCHMIND_VIEW_LINKEDGE_LINKEDGEVIEWFACTORY_H_