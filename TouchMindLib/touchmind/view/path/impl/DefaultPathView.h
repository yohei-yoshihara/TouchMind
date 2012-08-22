#ifndef TOUCHMIND_VIEW_PATH_IMPL_DEFAULTPATHVIEW_H_
#define TOUCHMIND_VIEW_PATH_IMPL_DEFAULTPATHVIEW_H_

#include "forwarddecl.h"
#include "touchmind/view/path/BasePathView.h"

namespace touchmind
{
namespace view
{
namespace path
{
namespace impl
{

class DefaultPathView :
    public touchmind::view::path::BasePathView
{
private:
    CComPtr<ID2D1SolidColorBrush> m_pathBrush;
    CComPtr<ID2D1PathGeometry> m_pathGeometry;
    CComPtr<ID2D1StrokeStyle> m_pathStyle;
    CComPtr<ID2D1Bitmap> m_pBitmap;
    D2D1_RECT_F m_bounds;

protected:
    bool _CheckValidity();
    void _CreateDeviceDependentResources(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget,
        std::shared_ptr<touchmind::model::node::NodeModel> node);
    void _CreateTexture(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget,
        std::shared_ptr<touchmind::model::node::NodeModel> node);

public:
    DefaultPathView(std::weak_ptr<touchmind::model::node::NodeModel> node) :
        BasePathView(node),
        m_pathBrush(nullptr),
        m_pathGeometry(nullptr),
        m_pathStyle(nullptr),
        m_pBitmap(nullptr)
    {}
    virtual ~DefaultPathView() {}

    virtual bool IsCompatible(const std::shared_ptr<touchmind::model::BaseModel> &model) override {
        UNREFERENCED_PARAMETER(model);
        return true;
    }
    virtual void CreateDeviceDependentResources(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget) override;
    virtual void DiscardDeviceDependentResources() override;
    virtual void Draw(touchmind::Context *pContext,
                      ID2D1RenderTarget *pRenderTarget) override;
    virtual bool HitTest(touchmind::Context *pContext,
                         ID2D1RenderTarget *pRenderTarget,
                         D2D1_POINT_2F point) override;
};

} // impl
} // path
} // view
} // touchmind

#endif // TOUCHMIND_VIEW_PATH_IMPL_DEFAULTPATHVIEW_H_