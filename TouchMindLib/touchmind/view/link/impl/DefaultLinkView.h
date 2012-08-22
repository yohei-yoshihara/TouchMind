#ifndef TOUCHMIND_VIEW_LINK_IMPL_DEFAULTLINKVIEW_H_
#define TOUCHMIND_VIEW_LINK_IMPL_DEFAULTLINKVIEW_H_

#include "forwarddecl.h"
#include "touchmind/Common.h"
#include "touchmind/view/link/BaseLinkView.h"

namespace touchmind
{
namespace view
{
namespace link
{
namespace impl
{

class DefaultLinkView :
    public touchmind::view::link::BaseLinkView,
    public std::enable_shared_from_this<DefaultLinkView>
{
private:
    CComPtr<ID2D1SolidColorBrush> m_linkBrush;
    CComPtr<ID2D1PathGeometry> m_linkGeometry;
    CComPtr<ID2D1StrokeStyle> m_linkStyle;
    CComPtr<ID2D1Bitmap> m_pBitmap;

protected:
    void _CreateDeviceDependentResources(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget,
        std::shared_ptr<touchmind::model::link::LinkModel> link);
    void _CreateTexture(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget,
        std::shared_ptr<touchmind::model::link::LinkModel> link);

public:
    DefaultLinkView(std::weak_ptr<touchmind::model::link::LinkModel> link) :
        BaseLinkView(link),
        m_linkBrush(nullptr),
        m_linkGeometry(nullptr),
        m_linkStyle(nullptr),
        m_pBitmap(nullptr)
    {}
    virtual ~DefaultLinkView()
    {}
    virtual bool IsCompatible(const std::shared_ptr<touchmind::model::BaseModel> &model) override {
        UNREFERENCED_PARAMETER(model);
        return true;
    }
    virtual void CreateDeviceDependentResources(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget) override;
    virtual void Draw(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget) override;
    virtual bool HitTest(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget,
        D2D1_POINT_2F point) override;
    virtual void DiscardDeviceDependentResources() override;
};

} // impl
} // link
} // view
} // touchmind

#endif // TOUCHMIND_VIEW_LINK_IMPL_DEFAULTLINKVIEW_H_