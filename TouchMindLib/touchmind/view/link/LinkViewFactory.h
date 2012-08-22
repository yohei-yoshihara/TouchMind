#ifndef TOUCHMIND_VIEW_LINK_LINKVIEWFACTORY_H_
#define TOUCHMIND_VIEW_LINK_LINKVIEWFACTORY_H_

#include "forwarddecl.h"

namespace touchmind
{
namespace view
{
namespace link
{

class LinkViewFactory
{
private:
    LinkViewFactory() {}

public:
    static std::shared_ptr<touchmind::view::link::BaseLinkView> Create(
        const std::shared_ptr<touchmind::model::link::LinkModel> &link);
};
} // link
} // view
} // touchmind


#endif // TOUCHMIND_VIEW_LINK_LINKVIEWFACTORY_H_