#ifndef TOUCHMIND_VIEW_LINKEDGE_ILINKEDGEVIEW_H_
#define TOUCHMIND_VIEW_LINKEDGE_ILINKEDGEVIEW_H_

#include "forwarddecl.h"
#include "touchmind/Common.h"
#include "touchmind/view/BaseView.h"

namespace touchmind
{
namespace view
{
namespace linkedge
{

class BaseLinkEdgeView :
    public touchmind::view::BaseView
{
private:
    std::weak_ptr<touchmind::model::link::LinkModel> m_link;
    EDGE_ID m_edgeId;

public:
    BaseLinkEdgeView(
        std::weak_ptr<touchmind::model::link::LinkModel> link,
        EDGE_ID edgeId);
    virtual ~BaseLinkEdgeView();
    void SetLinkModel(std::weak_ptr<touchmind::model::link::LinkModel> link) {
        m_link = link;
    }
    std::weak_ptr<touchmind::model::link::LinkModel> GetLinkModel() const {
        return m_link;
    }
    bool HasLinkModel() const {
        return !m_link.expired() && m_link.lock() != nullptr;
    }
    void SetEdgeId(EDGE_ID edgeId) {
        m_edgeId = edgeId;
    }
    EDGE_ID GetEdgeId() const {
        return m_edgeId;
    }

    virtual bool IsCompatible(const std::shared_ptr<touchmind::model::BaseModel> &model) = 0;
    virtual void CreateDeviceDependentResources(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget) = 0;
    virtual void DiscardDeviceDependentResources() = 0;
    virtual void Draw(touchmind::Context *pContext,
                      ID2D1RenderTarget *pRenderTarget) override {
        Draw(pContext, pRenderTarget, nullptr);
    }
    virtual bool HitTest(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget,
        D2D1_POINT_2F point) = 0;

    virtual void Draw(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget,
        ID2D1Brush *pBrush,
        FLOAT width = 1.0f) = 0;
    virtual void Fill(
        touchmind::Context *pContext,
        ID2D1RenderTarget *pRenderTarget,
        ID2D1Brush *pBrush = nullptr) = 0;
    virtual EDGE_STYLE GetEdgeStyle() const = 0;
};

} // linkedge
} // view
} // touchmind

#endif // TOUCHMIND_VIEW_LINKEDGE_ILINKEDGEVIEW_H_