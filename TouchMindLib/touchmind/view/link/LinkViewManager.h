#ifndef TOUCHMIND_VIEW_LINK_LINKVIEWMANAGER_H_
#define TOUCHMIND_VIEW_LINK_LINKVIEWMANAGER_H_

#include "forwarddecl.h"
#include "touchmind/Common.h"
#include "touchmind/filter/GaussFilter.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/view/link/BaseLinkView.h"

namespace touchmind {
  namespace view {
    namespace link {

      class LinkViewManager {
      private:
        touchmind::model::MapModel *m_pMapModel;
        touchmind::Configuration *m_pConfiguration;
        std::map<touchmind::LINK_ID, std::shared_ptr<touchmind::view::link::BaseLinkView>> m_views;
        std::unique_ptr<touchmind::filter::GaussFilter> m_pGaussFilter;
        CComPtr<ID2D1SolidColorBrush> m_pShadowBrush;
        CComPtr<ID2D1SolidColorBrush> m_pSelectedShadowBrush1;
        CComPtr<ID2D1SolidColorBrush> m_pSelectedShadowBrush2;

      public:
        LinkViewManager();
        virtual ~LinkViewManager();
        void SetMapModel(touchmind::model::MapModel *pMapModel) {
          m_pMapModel = pMapModel;
        }
        void SetConfiguration(touchmind::Configuration *pConfiguration) {
          m_pConfiguration = pConfiguration;
        }
        touchmind::filter::GaussFilter *GetGaussFilter() const {
          return m_pGaussFilter.get();
        }
        ID2D1SolidColorBrush *GetShadowBrush() const {
          return m_pShadowBrush;
        }
        ID2D1SolidColorBrush *GetSelectedShadowBrush1() const {
          return m_pSelectedShadowBrush1;
        }
        ID2D1SolidColorBrush *GetSelectedShadowBrush2() const {
          return m_pSelectedShadowBrush2;
        }
        void CreateSharedDeviceResources(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget);
        void DiscardSharedDeviceResources();
        void DiscardAllDeviceResources();

        std::shared_ptr<touchmind::view::link::BaseLinkView>
        GetLinkView(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
                    std::shared_ptr<touchmind::model::link::LinkModel> linkModel);
        void Synchronize(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget);
        void Resynchronize(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget) {
          m_views.clear();
          Synchronize(pContext, pRenderTarget);
        }
        std::shared_ptr<touchmind::model::link::LinkModel>
        HitTest(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget, D2D1_POINT_2F point);
        void Draw(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget);
      };

    } // link
  } // view
} // touchmind

#endif // TOUCHMIND_VIEW_LINK_LINKVIEWMANAGER_H_
