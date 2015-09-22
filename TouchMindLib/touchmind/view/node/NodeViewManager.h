#ifndef TOUCHMIND_VIEW_NODE_NODEVIEWMANAGER_H_
#define TOUCHMIND_VIEW_NODE_NODEVIEWMANAGER_H_

#include "forwarddecl.h"
#include "touchmind/Common.h"
#include "touchmind/Context.h"
#include "touchmind/control/DWriteEditControlCommon.h"
#include "touchmind/control/DWriteEditControl.h"
#include "touchmind/filter/GaussFilter.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/view/node/BaseNodeView.h"
#include "touchmind/view/path/BasePathView.h"

namespace touchmind {
  namespace view {
    namespace node {

      typedef std::map<touchmind::NODE_ID, std::shared_ptr<BaseNodeView>> NodeIdToNodeViewMap;
      typedef std::map<touchmind::NODE_ID, std::shared_ptr<path::BasePathView>> NodeIdToPathViewMap;

      class NodeViewManager {
      private:
        NodeIdToNodeViewMap m_nodeIdToViewMap;
        NodeIdToPathViewMap m_nodeIdToPathView;
        touchmind::Context *m_pContext;
        touchmind::Configuration *m_pConfiguration;
        touchmind::model::MapModel *m_pMapModel;
        touchmind::control::DWriteEditControlManager *m_pEditControlManager;
        std::unique_ptr<touchmind::filter::GaussFilter> m_pGaussFilter;
        CComPtr<ID2D1SolidColorBrush> m_pNodeShadowBrush;
        CComPtr<ID2D1SolidColorBrush> m_pSelectedNodeShadowBrush1;
        CComPtr<ID2D1SolidColorBrush> m_pSelectedNodeShadowBrush2;

      protected:
      public:
        static void CreateBodyColor(COLORREF baseColor, D2D1_COLOR_F &startColor, D2D1_COLOR_F &endColor);
        void DrawNodeShadow(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget, ID2D1Geometry *pGeometry);
        void DrawSelectedNodeShadow(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
                                    ID2D1Geometry *pGeometry);

      public:
        NodeViewManager();
        virtual ~NodeViewManager();
        void SetContext(touchmind::Context *pContext) {
          m_pContext = pContext;
        }
        void SetConfiguration(touchmind::Configuration *pConfiguration) {
          m_pConfiguration = pConfiguration;
        }
        void SetMapModel(touchmind::model::MapModel *pMapModel) {
          m_pMapModel = pMapModel;
        }
        void SetEditControlManager(touchmind::control::DWriteEditControlManager *pEditControlManager) {
          m_pEditControlManager = pEditControlManager;
        }
        touchmind::filter::GaussFilter *GetGaussFilter() const {
          return m_pGaussFilter.get();
        }
        ID2D1SolidColorBrush *GetShadowBrush() const {
          return m_pNodeShadowBrush;
        }
        ID2D1SolidColorBrush *GetSelectedShadowBrush1() const {
          return m_pSelectedNodeShadowBrush1;
        }
        ID2D1SolidColorBrush *GetSelectedShadowBrush2() const {
          return m_pSelectedNodeShadowBrush2;
        }

        touchmind::control::EDIT_CONTROL_INDEX GetEditControlIndexFromNodeId(touchmind::NODE_ID nodeId);
        touchmind::NODE_ID GetNodeIdFromEditControlIndex(touchmind::control::EDIT_CONTROL_INDEX editControlIndex);

        HRESULT CreateSharedDeviceResources(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget);
        void DiscardSharedDeviceResources();
        void DiscardAllDeviceResources();

        void DrawNodes(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
                       std::shared_ptr<model::node::NodeModel> edittingNode);
        HRESULT DrawNode(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
                         std::shared_ptr<model::node::NodeModel> node);
        void DrawNodePaths(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
                           std::shared_ptr<model::node::NodeModel> node);
        void DrawPath(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
                      std::shared_ptr<model::node::NodeModel> node);

        std::shared_ptr<touchmind::model::node::NodeModel>
        NodeHitTest(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget, D2D1_POINT_2F point);
        std::shared_ptr<touchmind::model::path::PathModel>
        PathHitTest(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget, D2D1_POINT_2F point);

        void DiscardAllResources();

        HRESULT SynchronizeWithModel();
        HRESULT SynchronizeViewBeforeArrange();
        HRESULT SyncFontAttributesToTextLayout();
        HRESULT SyncFontAttributesFromTextLayout();

        HRESULT _CreateEditControl(IN D2D1_RECT_F layoutRect, IN const std::wstring &text,
                                   OUT touchmind::control::EDIT_CONTROL_INDEX *editControlIndex);
        // Debug
        void OnDebugDump(std::wofstream &os);
      };

    } // node
  } // view
} // touchmind

#endif // TOUCHMIND_VIEW_NODE_NODEVIEWMANAGER_H_