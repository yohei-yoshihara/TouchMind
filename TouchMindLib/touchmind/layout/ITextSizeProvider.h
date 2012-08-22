#ifndef TOUCHMIND_LAYOUT_ITEXTLAYOUTPROVIDER_H_
#define TOUCHMIND_LAYOUT_ITEXTLAYOUTPROVIDER_H_

#include "touchmind/model/node/NodeModel.h"

namespace touchmind
{
namespace layout
{

/**
 * calculate width and height of a given tree node's text.
 */
class ITextSizeProvider
{
public:
    virtual void Calculate(
        IN const std::shared_ptr<model::node::NodeModel> &node,
        OUT FLOAT *pWidth,
        OUT FLOAT *pHeight) = 0;
};

} // layout
} // touchmind

#endif // TOUCHMIND_LAYOUT_ITEXTLAYOUTPROVIDER_H_