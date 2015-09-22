#ifndef TOUCHMIND_MODEL_LINKMODEL_H_
#define TOUCHMIND_MODEL_LINKMODEL_H_

#include "forwarddecl.h"
#include "touchmind/Common.h"
#include "touchmind/model/BaseModel.h"
#include "touchmind/selection/SelectableSupport.h"
#include "touchmind/ribbon/dispatch/ILineProperty.h"

namespace touchmind {
  namespace model {
    namespace link {

      class LinkModel : public touchmind::model::BaseModel,
                        public touchmind::selection::SelectableSupport,
                        public touchmind::ribbon::dispatch::ILineProperty,
                        public std::enable_shared_from_this<LinkModel> {
      private:
        static LINK_ID s_masterLinkId;
        LINK_ID m_linkId;
        std::array<std::weak_ptr<touchmind::model::node::NodeModel>, 2> m_nodes;
        std::array<NODE_ID, 2> m_nodeIds;
        std::array<std::shared_ptr<touchmind::model::linkedge::LinkEdgeModel>, 2> m_edges;
        D2D1_COLOR_F m_lineColor;
        LINE_WIDTH m_lineWidth;
        LINE_STYLE m_lineStyle;

      protected:
        D2D1_POINT_2F _CalculateLinkJointPosition(const std::shared_ptr<touchmind::model::node::NodeModel> &node) const;
        virtual std::shared_ptr<SelectableSupport> GetSelectableSharedPtr() override;

      public:
        static LINK_ID GenerateLinkId() {
          return s_masterLinkId++;
        }
        static std::shared_ptr<LinkModel> Create(touchmind::selection::SelectionManager *pSelectionManager);
        LinkModel();
        LinkModel(const LinkModel &other);
        virtual ~LinkModel() {
        }
        bool IsValid() const {
          return (!m_nodes[0].expired()) && (!m_nodes[1].expired());
        }
        void SetLinkId(LINK_ID linkId) {
          m_linkId = linkId;
        }
        LINK_ID GetLinkId() const {
          return m_linkId;
        }
        void SetNode(EDGE_ID edgeId, const std::shared_ptr<touchmind::model::node::NodeModel> &node);
        std::shared_ptr<touchmind::model::node::NodeModel> GetNode(EDGE_ID edgeId) const {
          if (!m_nodes[edgeId].expired()) {
            return m_nodes[edgeId].lock();
          }
          return nullptr;
        }
        NODE_ID GetNodeId(EDGE_ID edgeId) const {
          return m_nodeIds[edgeId];
        }
        void SetEdge(EDGE_ID edgeId, const std::shared_ptr<touchmind::model::linkedge::LinkEdgeModel> &edge) {
          m_edges[edgeId] = edge;
          IncrementRepaintRequiredCounter();
          IncrementSaveRequiredCounter();
        }
        std::shared_ptr<touchmind::model::linkedge::LinkEdgeModel> GetEdge(EDGE_ID edgeId) const {
          return m_edges[edgeId];
        }
        D2D1_POINT_2F GetEdgePoint(EDGE_ID edgeId) const;
        void SetLineColor(D2D1_COLOR_F lineColor) {
          m_lineColor = lineColor;
          IncrementRepaintRequiredCounter();
          IncrementSaveRequiredCounter();
          IncrementEdgesRepaintRequiredCounter();
        }
        D2D1_COLOR_F GetLineColor() const {
          return m_lineColor;
        }
        void SetLineWidth(LINE_WIDTH lineWidth) {
          m_lineWidth = lineWidth;
          IncrementRepaintRequiredCounter();
          IncrementSaveRequiredCounter();
          IncrementEdgesRepaintRequiredCounter();
        }
        LINE_WIDTH GetLineWidth() const {
          return m_lineWidth;
        }
        FLOAT GetLineWidthAsValue() const;
        void SetLineStyle(LINE_STYLE lineStyle) {
          m_lineStyle = lineStyle;
          IncrementRepaintRequiredCounter();
          IncrementSaveRequiredCounter();
          IncrementEdgesRepaintRequiredCounter();
        }
        LINE_STYLE GetLineStyle() const {
          return m_lineStyle;
        }
        void IncrementEdgesRepaintRequiredCounter();
        // line edge style
        void SetLineEdgeStyle(LINE_EDGE_STYLE lineEdgeStyle);
        LINE_EDGE_STYLE GetLineEdgeStyle() const;
        void GetControlPoints(std::array<D2D1_POINT_2F, 4> controlPoints) const;

        // ILineProperty (start)
        virtual bool UpdateProperty_IsLineColorChangeable() const override {
          return true;
        }
        virtual bool UpdateProperty_IsLineWidthChangeable() const override {
          return true;
        }
        virtual bool UpdateProperty_IsLineStyleChangeable() const override {
          return true;
        }
        virtual bool UpdateProperty_IsLineEdgeStyleChangeable() const override {
          return true;
        }
        virtual void Execute_LineColor(UI_EXECUTIONVERB, D2D1_COLOR_F color) override {
          SetLineColor(color);
        }
        virtual void Execute_LineWidth(UI_EXECUTIONVERB, LINE_WIDTH width) override {
          SetLineWidth(width);
        }
        virtual void Execute_LineStyle(UI_EXECUTIONVERB, LINE_STYLE style) override {
          SetLineStyle(style);
        }
        virtual void Execute_LineEdgeStyle(UI_EXECUTIONVERB, LINE_EDGE_STYLE style) override {
          SetLineEdgeStyle(style);
        }
        virtual D2D1_COLOR_F UpdateProperty_GetLineColor() override {
          return GetLineColor();
        }
        virtual LINE_WIDTH UpdateProperty_GetLineWidth() override {
          return GetLineWidth();
        }
        virtual LINE_STYLE UpdateProperty_GetLineStyle() override {
          return GetLineStyle();
        }
        virtual LINE_EDGE_STYLE UpdateProperty_GetLineEdgeStyle() {
          return GetLineEdgeStyle();
        }
        // ILineProperty (end)
      };

      std::wostream &operator<<(std::wostream &os, const LinkModel &link);

    } // link
  } // model
} // touchmind

#endif // TOUCHMIND_MODEL_LINKMODEL_H_