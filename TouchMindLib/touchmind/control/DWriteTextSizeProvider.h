#ifndef TOUCHMIND_CONTROL_DWRITETEXTSIZEPROVIDER_H_
#define TOUCHMIND_CONTROL_DWRITETEXTSIZEPROVIDER_H_

#include "forwarddecl.h"
#include "touchmind/layout/ITextSizeProvider.h"
#include "touchmind/view/node/NodeViewManager.h"

namespace touchmind
{
namespace control
{

class DWriteTextSizeProvider :
    public touchmind::layout::ITextSizeProvider
{
private:
    touchmind::view::node::NodeViewManager *m_pNodeViewManager;
    touchmind::control::DWriteEditControlManager *m_pEditControlManager;

public:
    DWriteTextSizeProvider(void);
    virtual ~DWriteTextSizeProvider(void);
    void SetNodeViewManager(touchmind::view::node::NodeViewManager *pNodeViewManager) {
        m_pNodeViewManager = pNodeViewManager;
    }
    void SetEditControlManager(touchmind::control::DWriteEditControlManager *pEditControlManager) {
        m_pEditControlManager = pEditControlManager;
    }
    virtual void Calculate(IN const std::shared_ptr<touchmind::model::node::NodeModel> &node, OUT FLOAT *pWidth, OUT FLOAT *pHeight);
};

} // control
} // touchmind

#endif // TOUCHMIND_CONTROL_DWRITETEXTSIZEPROVIDER_H_