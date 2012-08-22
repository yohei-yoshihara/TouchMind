#ifndef TOUCHMIND_LAYOUT_LAYOUTMANAGER_H_
#define TOUCHMIND_LAYOUT_LAYOUTMANAGER_H_

#include "forwarddecl.h"
#include "touchmind/layout/ILayoutManager.h"
#include "touchmind/layout/ITextSizeProvider.h"

namespace touchmind
{
namespace layout
{

class LayoutWorkData
{
public:
    LayoutWorkData() :
        prelim(0.0f),
        modifier(0.0f),
        pLeftNeighbor(), // null shared_ptr
        pRightNeighbor() // null shared_ptr
    {}
    ~LayoutWorkData() {}
    FLOAT prelim;
    FLOAT modifier;
    std::shared_ptr<touchmind::model::node::NodeModel> pLeftNeighbor;
    std::shared_ptr<touchmind::model::node::NodeModel> pRightNeighbor;
};

class LayoutManager :
    public ILayoutManager
{
private:
    ITextSizeProvider *m_pTextSizeProvider;
    std::shared_ptr<model::node::NodeModel> m_node;
    Configuration *m_configuration;

    // work data
    std::map<int, FLOAT> mw_maxLevelHeight;
    std::map<int, FLOAT> mw_maxLevelWidth;
    std::map<int, std::shared_ptr<touchmind::model::node::NodeModel>> mw_previousLevelNode;
    FLOAT mw_rootXOffset;
    FLOAT mw_rootYOffset;

    std::map<NODE_ID, LayoutWorkData> m_layoutWorkDataMap;

protected:
    void _CalculateSize(const std::shared_ptr<touchmind::model::node::NodeModel> &node);
    LayoutWorkData* _GetLayoutWorkData(const std::shared_ptr<touchmind::model::node::NodeModel> &node);

public:
    LayoutManager(void);
    virtual ~LayoutManager(void);

    // ILayoutManager (start)
    virtual void SetConfiguration(Configuration *configuration) override {
        m_configuration = configuration;
    }
    virtual Configuration* GetConfiguration() const override {
        return m_configuration;
    }
    virtual void SetTextSizeProvider(ITextSizeProvider* pTextSizeProvider) override {
        m_pTextSizeProvider = pTextSizeProvider;
    }
    virtual void SetModel(const std::shared_ptr<touchmind::model::node::NodeModel> &node) override {
        m_node = node;
    }
    virtual void Arrange() override;
    // ILayoutManager (end)

    void AssignPosition();
    void _CalculateSize();
    FLOAT GetNodeSize(const std::shared_ptr<touchmind::model::node::NodeModel> &node);
    FLOAT GetChildrenCenter(const std::shared_ptr<touchmind::model::node::NodeModel> &node);
    void FirstWalk(
        const std::shared_ptr<touchmind::model::node::NodeModel> &tree, 
        const std::shared_ptr<touchmind::model::node::NodeModel> &node, 
        int level);
    void SecondWalk(
        const std::shared_ptr<touchmind::model::node::NodeModel> &tree, 
        const std::shared_ptr<touchmind::model::node::NodeModel> &node, 
        int level, 
        FLOAT x, 
        FLOAT y);
    void Apportion(
        const std::shared_ptr<touchmind::model::node::NodeModel> &tree, 
        const std::shared_ptr<touchmind::model::node::NodeModel> &node, 
        int level);
    void PositionTree();
    void SaveAsVML(const std::wstring &filename);
    void SaveAsHTML5(const std::wstring &filename);
    void SetLevelHeight(const std::shared_ptr<touchmind::model::node::NodeModel> &node, int level);
    void SetLevelWidth(const std::shared_ptr<touchmind::model::node::NodeModel> &node, int level);
    void SetNeighbors(const std::shared_ptr<touchmind::model::node::NodeModel> &node, int level);
    std::shared_ptr<touchmind::model::node::NodeModel> GetLeftMost(
        const std::shared_ptr<touchmind::model::node::NodeModel> &node, 
        int level, 
        int maxLevel) const;
    void RearrangeVerticalPosition(touchmind::NODE_SIDE nodePosition);
};

class TreeRect
{
public:
    FLOAT x1;
    FLOAT y1;
    FLOAT x2;
    FLOAT y2;
    TreeRect(FLOAT _x1, FLOAT _y1, FLOAT _x2, FLOAT _y2) :
        x1(_x1),
        y1(_y1),
        x2(_x2),
        y2(_y2) {}
    FLOAT GetWidth() {
        return x2 - x1;
    }
    FLOAT GetHeight() {
        return y2 - y1;
    }
};

class TreeRectVisitor
{
public:
    TreeRect treeRect;
    TreeRectVisitor() :
        treeRect(FLT_MAX, FLT_MAX, FLT_MIN, FLT_MIN) {
    }
    touchmind::VISITOR_RESULT operator()(const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
        if (!node->IsCollapsed() && node->IsAncestorCollapsed()) {
            return touchmind::VISITOR_RESULT_CONTINUE;
        }
        if (treeRect.x1 > node->GetX()) {
            treeRect.x1 = node->GetX();
        }
        if (treeRect.y1 > node->GetY()) {
            treeRect.y1 = node->GetY();
        }
        if (treeRect.x2 < node->GetX() + node->GetWidth()) {
            treeRect.x2 = node->GetX() + node->GetWidth();
        }
        if (treeRect.y2 < node->GetY() + node->GetHeight()) {
            treeRect.y2 = node->GetY() + node->GetHeight();
        }
        return touchmind::VISITOR_RESULT_CONTINUE;
    }
};

} // layout
} // touchmind

#endif // TOUCHMIND_LAYOUT_LAYOUTMANAGER_H_