#ifndef TOUCHMIND_WIN_OPERATION_NODEMOVINGOPERATION_H_
#define TOUCHMIND_WIN_OPERATION_NODEMOVINGOPERATION_H_

#include "forwarddecl.h"
#include "touchmind/operation/IOperation.h"

namespace touchmind
{
namespace operation
{

class NodeMovingOperation :
    public IOperation
{
private:
    HWND m_hwnd;
    touchmind::Configuration *m_pConfiguration;
    touchmind::touch::ManipulationHelper *m_pManipulationHelper;
    touchmind::view::node::NodeViewManager *m_pNodeViewManager;
    touchmind::util::ScrollBarHelper *m_pScrollBarHelper;

public:
    NodeMovingOperation();
    virtual ~NodeMovingOperation();

    void SetHWnd(HWND hwnd) {
        m_hwnd = hwnd;
    }
    void SetConfiguration(touchmind::Configuration *pConfiguration) {
        m_pConfiguration = pConfiguration;
    }
    void SetManipulationHelper(touchmind::touch::ManipulationHelper *pManipulationHelper) {
        m_pManipulationHelper = pManipulationHelper;
    }
    void SetNodeViewManager(touchmind::view::node::NodeViewManager *pNodeViewManager) {
        m_pNodeViewManager = pNodeViewManager;
    }
    void SetScrollBarHelper(touchmind::util::ScrollBarHelper *pScrollBarHelper) {
        m_pScrollBarHelper = pScrollBarHelper;
    }

    void Draw(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget,
        std::shared_ptr<model::node::NodeModel> edittingNode);
    virtual void Destroy() override {}
};

} // operation
} // touchmind

#endif // TOUCHMIND_WIN_OPERATION_NODEMOVINGOPERATION_H_