#ifndef TOUCHMIND_MODEL_PATH_PATHMODEL_H_
#define TOUCHMIND_MODEL_PATH_PATHMODEL_H_

#include "forwarddecl.h"
#include "touchmind/Common.h"
#include "touchmind/model/BaseModel.h"
#include "touchmind/selection/SelectableSupport.h"
#include "touchmind/ribbon/dispatch/ILineProperty.h"

namespace touchmind {
  namespace model {
    namespace path {

      class PathModel : public touchmind::model::BaseModel,
                        public touchmind::selection::SelectableSupport,
                        public touchmind::ribbon::dispatch::ILineProperty,
                        public std::enable_shared_from_this<PathModel> {
      private:
        std::weak_ptr<touchmind::model::node::NodeModel> m_node;
        D2D1_COLOR_F m_lineColor;
        LINE_WIDTH m_lineWidth;
        LINE_STYLE m_lineStyle;

      protected:
        virtual std::shared_ptr<touchmind::selection::SelectableSupport> GetSelectableSharedPtr() override;

      public:
        PathModel()
            : BaseModel()
            , m_node()
            , m_lineColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f))
            , m_lineWidth(LINE_WIDTH_1)
            , m_lineStyle(LINE_STYLE_SOLID) {
          AddSelectedListener([&]() { IncrementRepaintRequiredCounter(); });
          AddUnselectedListener([&]() { IncrementRepaintRequiredCounter(); });
        }
        PathModel(const PathModel &other)
            : m_node(other.m_node)
            , m_lineColor(other.m_lineColor)
            , m_lineWidth(other.m_lineWidth)
            , m_lineStyle(other.m_lineStyle) {
        }
        PathModel &operator=(const PathModel &other) {
          if (this != &other) {
            m_node = other.m_node;
            m_lineColor = other.m_lineColor;
            m_lineWidth = other.m_lineWidth;
            m_lineStyle = other.m_lineStyle;
          }
          return *this;
        }
        ~PathModel() {
        }
        bool operator==(const PathModel &other) const {
          if (!m_node.expired() && !other.m_node.expired()) {
            return m_node.lock() == other.m_node.lock() && m_lineColor.r == other.m_lineColor.r
                   && m_lineColor.g == other.m_lineColor.g && m_lineColor.b == other.m_lineColor.b
                   && m_lineColor.a == other.m_lineColor.a && m_lineWidth == other.m_lineWidth
                   && m_lineStyle == other.m_lineStyle;
          } else if (m_node.expired() && other.m_node.expired()) {
            return m_lineColor.r == other.m_lineColor.r && m_lineColor.g == other.m_lineColor.g
                   && m_lineColor.b == other.m_lineColor.b && m_lineColor.a == other.m_lineColor.a
                   && m_lineWidth == other.m_lineWidth && m_lineStyle == other.m_lineStyle;
          }
          return false;
        }
        bool operator!=(const PathModel &other) const {
          return !(*this == other);
        }

        void SetNodeModel(std::weak_ptr<touchmind::model::node::NodeModel> node);
        std::shared_ptr<touchmind::model::node::NodeModel> GetNodeModel() const;
        void SetColor(D2D1_COLOR_F color) {
          if (m_lineColor.r == color.r && m_lineColor.g == color.g && m_lineColor.b == color.b
              && m_lineColor.a == color.a) {
            return;
          }
          m_lineColor = color;
          IncrementRepaintRequiredCounter();
          IncrementSaveRequiredCounter();
        }
        D2D1_COLOR_F GetColor() const {
          return m_lineColor;
        }
        void SetWidth(LINE_WIDTH lineWidth) {
          if (m_lineWidth == lineWidth) {
            return;
          }
          m_lineWidth = lineWidth;
          IncrementRepaintRequiredCounter();
          IncrementSaveRequiredCounter();
        }
        LINE_WIDTH GetWidth() const {
          return m_lineWidth;
        }
        FLOAT GetWidthAsValue() const;
        void SetStyle(LINE_STYLE lineStyle) {
          if (m_lineStyle == lineStyle) {
            return;
          }
          m_lineStyle = lineStyle;
          IncrementRepaintRequiredCounter();
          IncrementSaveRequiredCounter();
        }
        LINE_STYLE GetStyle() const {
          return m_lineStyle;
        }
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
          return false;
        }
        virtual void Execute_LineColor(UI_EXECUTIONVERB, D2D1_COLOR_F color) override {
          SetColor(color);
        }
        virtual void Execute_LineWidth(UI_EXECUTIONVERB, LINE_WIDTH width) override {
          SetWidth(width);
        }
        virtual void Execute_LineStyle(UI_EXECUTIONVERB, LINE_STYLE style) override {
          SetStyle(style);
        }
        virtual void Execute_LineEdgeStyle(UI_EXECUTIONVERB, LINE_EDGE_STYLE) override {
        }
        virtual D2D1_COLOR_F UpdateProperty_GetLineColor() override {
          return GetColor();
        }
        virtual LINE_WIDTH UpdateProperty_GetLineWidth() override {
          return GetWidth();
        }
        virtual LINE_STYLE UpdateProperty_GetLineStyle() override {
          return GetStyle();
        }
        virtual LINE_EDGE_STYLE UpdateProperty_GetLineEdgeStyle() {
          return LINE_EDGE_STYLE_NORMAL;
        }
        // ILineProperty (end)
      };

    } // path
  } // model
} // touchmind

#endif // TOUCHMIND_MODEL_PATH_PATHMODEL_H_