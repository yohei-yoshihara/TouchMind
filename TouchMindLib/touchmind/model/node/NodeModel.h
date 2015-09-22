#ifndef TOUCHMIND_MODEL_NODE_NODEMODEL_H_
#define TOUCHMIND_MODEL_NODE_NODEMODEL_H_

#include "forwarddecl.h"
#include "touchmind/Common.h"
#include "touchmind/util/ColorUtil.h"
#include "touchmind/text/FontAttribute.h"
#include "touchmind/model/BaseModel.h"
#include "touchmind/model/path/PathModel.h"
#include "touchmind/selection/SelectableSupport.h"
#include "touchmind/ribbon/dispatch/INodeProperty.h"

namespace touchmind {
  namespace model {
    namespace node {

      typedef std::function<VISITOR_RESULT(std::shared_ptr<NodeModel>)> NODE_VISITOR;

      class NodeModel : public touchmind::model::BaseModel,
                        public touchmind::selection::SelectableSupport,
                        public touchmind::ribbon::dispatch::INodeProperty,
                        public std::enable_shared_from_this<NodeModel> {
      public:
        const static FLOAT DEFAULT_WIDTH;
        const static FLOAT DEFAULT_HEIGHT;

      private:
        static touchmind::NODE_ID s_masterId;

        NODE_ID m_id;
        NODE_SIDE m_position;
        std::wstring m_text;
        SYSTEMTIME m_createdTime;
        SYSTEMTIME m_modifiedTime;
        std::vector<std::shared_ptr<NodeModel>> m_children;
        std::weak_ptr<NodeModel> m_parent;
        bool m_collapsed;
        bool m_canCollapsed;
        FLOAT m_x;
        FLOAT m_y;
        FLOAT m_width;
        FLOAT m_height;
        D2D1_COLOR_F m_backgroundColor;
        NODE_SHAPE m_nodeShape;
        std::vector<touchmind::text::FontAttribute> m_fontAttributes;
        std::vector<std::weak_ptr<touchmind::model::link::LinkModel>> m_links;
        std::shared_ptr<touchmind::model::path::PathModel> m_pathModel;

      protected:
        std::shared_ptr<NodeModel> _FindByNodeId(const std::shared_ptr<NodeModel> &node, touchmind::NODE_ID nodeId);
        void _Indent(std::wostream &out, int indent);
        void _Dump(std::wostream &out, int indent, const std::shared_ptr<NodeModel> &node);
        virtual std::shared_ptr<SelectableSupport> GetSelectableSharedPtr() override;

      public:
        static LINK_ID GenerateLinkId() {
          return s_masterId++;
        }
        static std::shared_ptr<NodeModel> Create(touchmind::selection::SelectionManager *pSelectionManager);
        static std::shared_ptr<NodeModel> Create(const NodeModel &node) {
          return std::make_shared<NodeModel>(node);
        }
        static std::shared_ptr<NodeModel> Create(NodeModel &&node) {
          return std::make_shared<NodeModel>(node);
        }
        NodeModel();
        NodeModel(const NodeModel &other);
        NodeModel(NodeModel &&other);
        NodeModel &operator=(const NodeModel &other);
        NodeModel &operator=(NodeModel &&other);
        virtual ~NodeModel(void);
        bool operator==(const NodeModel &other) const;
        bool operator!=(const NodeModel &other) const {
          return !(*this == other);
        }

        // id
        void SetId(touchmind::NODE_ID nodeId);
        touchmind::NODE_ID GetId(void) const {
          return m_id;
        }

        // position
        void SetPosition(NODE_SIDE position);
        NODE_SIDE GetPosition(void) const {
          return m_position;
        }
        NODE_SIDE GetAncestorPosition(void) const;

        // text
        void SetText(const std::wstring &text);
        const std::wstring &GetText() const {
          return m_text;
        }

        // created time
        void SetCreatedTime(const SYSTEMTIME &createdTime);
        void SetCreatedTime(LONGLONG javaTime);
        const SYSTEMTIME &GetCreatedTime() const {
          return m_createdTime;
        }
        LONGLONG GetCreatedTimeAsJavaTime() const;
        time_t GetCreatedTimeAsUnixTime() const;

        // modified time
        void SetModifiedTime(const SYSTEMTIME &modifiedTime);
        void SetModifiedTime(LONGLONG javaTime);
        const SYSTEMTIME &GetModifiedTime() const {
          return m_modifiedTime;
        }
        LONGLONG GetModifiedTimeAsJavaTime() const;
        time_t GetModifiedTimeAsUnixTime() const;

        // parent
        void SetParent(const std::shared_ptr<NodeModel> &parent);
        std::shared_ptr<NodeModel> GetParent() const {
          return m_parent.lock();
        }

        // children
        void AddChild(const std::shared_ptr<NodeModel> &node);
        void RemoveChild(const std::shared_ptr<NodeModel> &node);
        void RemoveAllChildren(void);
        size_t GetChildrenCount() const {
          return !IsCollapsed() ? m_children.size() : 0;
        }
        size_t GetActualChildrenCount() const {
          return m_children.size();
        }
        std::shared_ptr<NodeModel> GetChild(std::vector<std::shared_ptr<NodeModel>>::size_type index) const {
          if (index < m_children.size()) {
            return m_children[index];
          } else {
            return nullptr;
          }
        }
        std::shared_ptr<NodeModel> FindByNodeId(touchmind::NODE_ID nodeId);

        int GetLevel() const {
          return GetParent() == nullptr ? 0 : GetParent()->GetLevel() + 1;
        }
        bool IsAncestorCollapsed() const;
        void SetAncestorsExpanded();
        std::shared_ptr<NodeModel> GetLeftSibling();
        std::shared_ptr<NodeModel> GetRightSibling();
        std::shared_ptr<NodeModel> GetFirstChild();
        std::shared_ptr<NodeModel> GetLastChild();
        bool IsCollapsed() const {
          return m_collapsed;
        }
        void SetCollapsed(bool collapsed);
        bool CanCollapsed() const;
        void SetCanCollapsed(bool canCollapsed);
        bool IsAncestor(const std::shared_ptr<NodeModel> &node) const;

        // x
        void SetX(FLOAT x) {
          if (m_x == x) {
            return;
          }
          m_x = x;
          IncrementRepaintRequiredCounter();
          UpdatePathStatus_RepaintRequired();
          UpdateLinkStatus_RepaintRequired();
        }
        FLOAT GetX() const {
          return m_x;
        }

        // y
        void SetY(FLOAT y) {
          if (m_y == y) {
            return;
          }
          m_y = y;
          IncrementRepaintRequiredCounter();
          UpdatePathStatus_RepaintRequired();
          UpdateLinkStatus_RepaintRequired();
        }
        FLOAT GetY() const {
          return m_y;
        }

        // width
        void SetWidth(FLOAT width) {
          if (m_width == width) {
            return;
          }
          m_width = width;
          IncrementRepaintRequiredCounter();
          UpdatePathStatus_RepaintRequired();
          UpdateLinkStatus_RepaintRequired();
        }
        FLOAT GetWidth() const {
          return m_width;
        }

        // height
        void SetHeight(FLOAT height) {
          if (m_height == height) {
            return;
          }
          m_height = height;
          IncrementRepaintRequiredCounter();
          UpdatePathStatus_RepaintRequired();
          UpdateLinkStatus_RepaintRequired();
        }
        FLOAT GetHeight() const {
          return m_height;
        }

        // center point
        D2D1_POINT_2F GetCenter() const {
          return D2D1::Point2F(m_x + m_width / 2.0f, m_y + m_height / 2.0f);
        }

        // font attribute
        void AddFontAttribute(const touchmind::text::FontAttribute &fontAttribute) {
          m_fontAttributes.push_back(fontAttribute);
          IncrementRepaintRequiredCounter();
          IncrementSaveRequiredCounter();
        }
        void ClearFontAttributes() {
          m_fontAttributes.clear();
          IncrementRepaintRequiredCounter();
          IncrementSaveRequiredCounter();
        }
        size_t GetFontAttributeCount() const {
          return m_fontAttributes.size();
        }
        const touchmind::text::FontAttribute &GetFontAttribute(size_t index) const {
          return m_fontAttributes[index];
        }

        // background color
        void SetBackgroundColor(D2D1_COLOR_F backgroundColor) {
          if (util::ColorUtil::Equal(m_backgroundColor, backgroundColor)) {
            return;
          }
          m_backgroundColor = backgroundColor;
          IncrementRepaintRequiredCounter();
          IncrementSaveRequiredCounter();
        }
        D2D1_COLOR_F GetBackgroundColor() const {
          return m_backgroundColor;
        }

        // node shape
        void SetNodeShape(NODE_SHAPE nodeShape) {
          if (m_nodeShape == nodeShape) {
            return;
          }
          m_nodeShape = nodeShape;
          IncrementRepaintRequiredCounter();
          IncrementSaveRequiredCounter();
        }
        NODE_SHAPE GetNodeShape() const {
          return m_nodeShape;
        }

        // link (don't call the following methods directly, use LinkContainerModel)
        void AddLink(const std::weak_ptr<touchmind::model::link::LinkModel> &link) {
          m_links.push_back(link);
        }
        void RemoveLink(const std::weak_ptr<touchmind::model::link::LinkModel> &link);
        const std::weak_ptr<touchmind::model::link::LinkModel> &GetLink(size_t index) const {
          return m_links[index];
        }
        size_t GetNumberOfLinks() const {
          return m_links.size();
        }
        void UpdateLinkStatus_RepaintRequired();
        void UpdateLinkStatus_SaveRequired();
        void UpdatePathStatus_RepaintRequired();

        bool IsDescendantSaveRequired(SAVE_COUNTER saveCouner) const;
        std::shared_ptr<touchmind::model::path::PathModel> GetPathModel() const {
          return m_pathModel;
        }

        // Visitor: VISITOR_RESULT operator()(std::shared_ptr<NodeModel>)
        template <typename Visitor>
        VISITOR_RESULT ApplyVisitor(Visitor &visitor, bool parentFirst = false) {
          if (parentFirst) {
            if (visitor(shared_from_this()) == VISITOR_RESULT_STOP) {
              return VISITOR_RESULT_STOP;
            }

            for (size_t i = 0; i < GetActualChildrenCount(); ++i) {
              auto child = GetChild(i);
              if (child->ApplyVisitor(visitor, parentFirst) == VISITOR_RESULT_STOP) {
                return VISITOR_RESULT_STOP;
              }
            }
            return VISITOR_RESULT_CONTINUE;
          } else {
            for (size_t i = 0; i < GetActualChildrenCount(); ++i) {
              auto child = GetChild(i);
              if (child->ApplyVisitor(visitor, parentFirst) == VISITOR_RESULT_STOP) {
                return VISITOR_RESULT_STOP;
              }
            }

            return visitor(shared_from_this());
          }
        }

        VISITOR_RESULT ApplyVisitor(NODE_VISITOR visitor, bool parentFirst = false) {
          if (parentFirst) {
            if (visitor(shared_from_this()) == VISITOR_RESULT_STOP) {
              return VISITOR_RESULT_STOP;
            }

            for (size_t i = 0; i < GetActualChildrenCount(); ++i) {
              auto child = GetChild(i);
              if (child->ApplyVisitor(visitor, parentFirst) == VISITOR_RESULT_STOP) {
                return VISITOR_RESULT_STOP;
              }
            }
            return VISITOR_RESULT_CONTINUE;
          } else {
            for (size_t i = 0; i < GetActualChildrenCount(); ++i) {
              auto child = GetChild(i);
              if (child->ApplyVisitor(visitor, parentFirst) == VISITOR_RESULT_STOP) {
                return VISITOR_RESULT_STOP;
              }
            }

            return visitor(shared_from_this());
          }
        }

        std::shared_ptr<NodeModel> DeepCopy();
        bool Compare(const std::shared_ptr<NodeModel> &node);
        bool DeepCompare(const std::shared_ptr<NodeModel> &node);
        touchmind::NODE_ID GenerateId() {
          return s_masterId++;
        }
        void RenumberIds();

        friend std::wostream &operator<<(std::wostream &os, NodeModel const &node);
        std::wostream &Dump(std::wostream &out);
        std::wostream &DumpAll(std::wostream &out);

        // container (start) N3242 - Table 96
        typedef std::shared_ptr<NodeModel> value_type;
        typedef std::shared_ptr<NodeModel> &reference;
        typedef const std::shared_ptr<NodeModel> &const_reference;
        typedef std::vector<std::shared_ptr<NodeModel>>::iterator iterator;
        typedef std::vector<std::shared_ptr<NodeModel>>::const_iterator const_iterator;
        typedef std::vector<std::shared_ptr<NodeModel>>::difference_type difference_type;
        typedef std::vector<std::shared_ptr<NodeModel>>::size_type size_type;
        typedef std::shared_ptr<NodeModel> *pointer;
        typedef const std::shared_ptr<NodeModel> const_pointer;
        iterator begin() {
          return m_children.begin();
        }
        iterator end() {
          return m_children.end();
        }
        const_iterator cbegin() {
          return m_children.cbegin();
        }
        const_iterator cend() {
          return m_children.cend();
        }
        size_type size() const {
          return m_children.size();
        }
        size_type max_size() const {
          return m_children.max_size();
        }
        bool empty() const {
          return (size() == 0);
        }
        void swap(value_type &right) {
          m_children.swap(right->m_children);
        }
        const_reference at(size_type pos) const {
          return m_children.at(pos);
        }
        reference at(size_type pos) {
          return m_children.at(pos);
        }
        const_reference operator[](size_type pos) const {
          return m_children[pos];
        }
        reference operator[](size_type pos) {
          return m_children[pos];
        }
        // container (end)
        touchmind::model::node::iterator::ChildFirstNodeIterator Begin_ChildFirst();
        touchmind::model::node::iterator::ChildFirstNodeIterator End_ChildFirst();
        touchmind::model::node::iterator::ParentFirstNodeIterator Begin_ParentFirst();
        touchmind::model::node::iterator::ParentFirstNodeIterator End_ParentFirst();

        // INodeProperty interface (start)
        virtual bool UpdateProperty_IsNodeBackgroundColorChangeable() const override {
          return true;
        }
        virtual bool UpdateProperty_IsNodeShapeChangeable() const override {
          return true;
        }
        virtual void Execute_NodeBackgroundColor(UI_EXECUTIONVERB, D2D1_COLOR_F color) override {
          SetBackgroundColor(color);
        }
        virtual void Execute_NodeShape(UI_EXECUTIONVERB, NODE_SHAPE nodeShape) override {
          SetNodeShape(nodeShape);
        }
        virtual D2D1_COLOR_F UpdateProperty_GetNodeBackgroundColor() override {
          return GetBackgroundColor();
        }
        virtual NODE_SHAPE UpdateProperty_GetNodeShape() override {
          return GetNodeShape();
        }
        // INodeProperty interface (end)
      };

    } // node
  } // model
} // touchmind

#endif // TOUCHMIND_MODEL_NODE_NODEMODEL_H_