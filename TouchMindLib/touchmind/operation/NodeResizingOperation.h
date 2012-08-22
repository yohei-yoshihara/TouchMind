#ifndef TOUCHMIND_OPERATION_NODERESIZINGOPERATION_H_
#define TOUCHMIND_OPERATION_NODERESIZINGOPERATION_H_

#include "forwarddecl.h"
#include "touchmind/operation/IOperation.h"

namespace touchmind
{
namespace operation
{

class NodeResizingOperation :
    public IOperation
{
private:
    touchmind::Configuration *m_pConfiguration;
    touchmind::touch::ManipulationHelper *m_pManipulationHelper;
    CComPtr<ID2D1SolidColorBrush> m_pResizingFigureBrush;

public:
    NodeResizingOperation();
    virtual ~NodeResizingOperation();

    void SetConfiguration(touchmind::Configuration *pConfiguration) {
        m_pConfiguration = pConfiguration;
    }
    void SetManipulationHelper(touchmind::touch::ManipulationHelper *pManipulationHelper) {
        m_pManipulationHelper = pManipulationHelper;
    }
    void CreateDeviceDependentResources(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget);
    void Draw(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget);
    virtual void Destroy() override {}
};

} //operation
} // touchmind

#endif // TOUCHMIND_OPERATION_NODERESIZINGOPERATION_H_