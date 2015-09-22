#ifndef TOUCHMIND_MODEL_LINKEDGE_LINKEDGEMODEL_H_
#define TOUCHMIND_MODEL_LINKEDGE_LINKEDGEMODEL_H_

#include "forwarddecl.h"
#include "touchmind/Common.h"
#include "touchmind/model/BaseModel.h"

namespace touchmind {
  namespace model {
    namespace linkedge {

      class LinkEdgeModel : public touchmind::model::BaseModel, public std::enable_shared_from_this<LinkEdgeModel> {
      public:
        static const FLOAT INITIAL_LENGTH;

      private:
        touchmind::model::link::LinkModel *m_link;
        FLOAT m_angle; // handle angle, radian
        FLOAT m_length; // handle length
        EDGE_STYLE m_style;

      public:
        LinkEdgeModel();
        LinkEdgeModel(const LinkEdgeModel &other);
        virtual ~LinkEdgeModel() {
        }
        void SetLink(touchmind::model::link::LinkModel *link) {
          m_link = link;
        }
        touchmind::model::link::LinkModel *GetLink() const {
          return m_link;
        }
        void SetAngle(FLOAT angle) {
          m_angle = angle;
          IncrementRepaintRequiredCounter();
          IncrementSaveRequiredCounter();
          IncreamentParentLinkRepaintRequiredCounter();
          IncreamentParentLinkSaveRequiredCounter();
        }
        FLOAT GetAngle() const {
          return m_angle;
        }
        void SetLength(FLOAT length) {
          m_length = length;
          IncrementRepaintRequiredCounter();
          IncrementSaveRequiredCounter();
          IncreamentParentLinkRepaintRequiredCounter();
          IncreamentParentLinkSaveRequiredCounter();
        }
        FLOAT GetLength() const {
          return m_length;
        }
        void SetStyle(EDGE_STYLE style) {
          m_style = style;
          IncrementRepaintRequiredCounter();
          IncrementSaveRequiredCounter();
          IncreamentParentLinkRepaintRequiredCounter();
          IncreamentParentLinkSaveRequiredCounter();
        }
        EDGE_STYLE GetStyle() const {
          return m_style;
        }
        FLOAT GetMarkerSize() const {
          switch (m_style) {
          case EDGE_STYLE_ARROW:
          case EDGE_STYLE_CIRCLE:
            return 10.0f;
          }
          return 0.0f;
        }
        // parent link repaint required
        void IncreamentParentLinkRepaintRequiredCounter();
        void IncreamentParentLinkSaveRequiredCounter();
        void CalculateAngleAndLength(D2D1_POINT_2F nodePos, D2D1_POINT_2F handlePos);
        D2D1_POINT_2F CalculateHandlePosition(D2D1_POINT_2F nodePos, touchmind::NODE_SIDE nodeSide) const;
      };

      std::wostream &operator<<(std::wostream &os, const LinkEdgeModel &handle);

    } // linkedge
  } // model
} // touchmodel

#endif // TOUCHMIND_MODEL_LINKEDGE_LINKEDGEMODEL_H_