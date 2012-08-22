#ifndef TOUCHMIND_VIEW_NODE_NODEVIEWFACTORY_H_
#define TOUCHMIND_VIEW_NODE_NODEVIEWFACTORY_H_

#include "forwarddecl.h"

namespace touchmind
{
namespace view
{
namespace node
{

class NodeViewFactory
{
private:
    NodeViewFactory() {}
public:
    static std::shared_ptr<view::node::BaseNodeView> Create(const std::shared_ptr<model::node::NodeModel> &node);
};

} // node
} // view
} // touchmind

#endif // TOUCHMIND_VIEW_NODE_NODEVIEWFACTORY_H_