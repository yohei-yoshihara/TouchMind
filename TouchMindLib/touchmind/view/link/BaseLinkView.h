#ifndef TOUCHMIND_VIEW_LINK_ILINKVIEW_H_
#define TOUCHMIND_VIEW_LINK_ILINKVIEW_H_

#include "forwarddecl.h"
#include "touchmind/view/BaseView.h"
#include "touchmind/view/linkedge/BaseLinkEdgeView.h"

namespace touchmind
{
namespace view
{
namespace link
{

class BaseLinkView :
    public touchmind::view::BaseView
{
private:
    std::weak_ptr<touchmind::model::link::LinkModel> m_link;
    touchmind::view::link::LinkViewManager *m_pLinkViewManager;
    std::array<std::shared_ptr<touchmind::view::linkedge::BaseLinkEdgeView>, 2> m_linkEdgeViews;

public:
    BaseLinkView(const std::weak_ptr<touchmind::model::link::LinkModel> &link);
    virtual ~BaseLinkView();
    void SetLinkModel(const std::weak_ptr<touchmind::model::link::LinkModel> &link) {
        m_link = link;
    }
    std::weak_ptr<touchmind::model::link::LinkModel> GetLinkModel() const {
        return m_link;
    }
    bool HasLinkModel() const {
        return !m_link.expired() && m_link.lock() != nullptr;
    }
    void SetLinkViewManager(touchmind::view::link::LinkViewManager *pLinkViewManager) {
        m_pLinkViewManager = pLinkViewManager;
    }
    touchmind::view::link::LinkViewManager* GetLinkViewManager() const {
        return m_pLinkViewManager;
    }
    void SetLinkEdgeView(EDGE_ID edgeId, const std::shared_ptr<touchmind::view::linkedge::BaseLinkEdgeView> &linkEdgeView) {
        m_linkEdgeViews[edgeId] = linkEdgeView;

    }
    std::shared_ptr<touchmind::view::linkedge::BaseLinkEdgeView> GetLinkEdgeView(EDGE_ID edgeId) const {
        return m_linkEdgeViews[edgeId];
    }
    virtual bool IsCompatible(const std::shared_ptr<touchmind::model::BaseModel> &model) = 0;

    virtual void CreateDeviceDependentResources(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget) = 0;
    virtual void Draw(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget) = 0;
    virtual bool HitTest(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget,
        D2D1_POINT_2F point) = 0;
    virtual void DiscardDeviceDependentResources();
};

} // link
} // view
} // touchmind

#endif // TOUCHMIND_VIEW_LINK_ILINKVIEW_H_