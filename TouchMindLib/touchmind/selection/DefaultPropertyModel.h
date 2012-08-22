#ifndef TOUCHMIND_SELECTION_DEFAULTPROPERTYMODEL_H_
#define TOUCHMIND_SELECTION_DEFAULTPROPERTYMODEL_H_

#include "touchmind/Common.h"
#include "touchmind/ribbon/dispatch/INodeProperty.h"
#include "touchmind/ribbon/dispatch/ILineProperty.h"
#include "touchmind/selection/SelectableSupport.h"

namespace touchmind
{
namespace selection
{

class DefaultPropertyModel :
    public touchmind::ribbon::dispatch::INodeProperty,
    public touchmind::ribbon::dispatch::ILineProperty,
    public touchmind::selection::SelectableSupport,
    public std::enable_shared_from_this<DefaultPropertyModel>
{
private:
    D2D1_COLOR_F m_nodeBackgroundColor;
    NODE_SHAPE m_nodeShape;

    D2D1_COLOR_F m_lineColor;
    LINE_WIDTH m_lineWidth;
    LINE_STYLE m_lineStyle;
    LINE_EDGE_STYLE m_lineEdgeStyle;

protected:
    virtual std::shared_ptr<SelectableSupport> GetSelectableSharedPtr() override;

public:
    DefaultPropertyModel() :
        m_nodeBackgroundColor(D2D1::ColorF(D2D1::ColorF::White)),
        m_nodeShape(NODE_SHAPE_ROUNDED_RECTANGLE),
        m_lineColor(D2D1::ColorF(D2D1::ColorF::Black)),
        m_lineWidth(LINE_WIDTH_1),
        m_lineStyle(LINE_STYLE_SOLID),
        m_lineEdgeStyle(LINE_EDGE_STYLE_NORMAL)
    {}
    virtual ~DefaultPropertyModel() {}

    // INodeProperty (start)
    virtual bool UpdateProperty_IsNodeBackgroundColorChangeable() const override {
        return true;
    }
    virtual bool UpdateProperty_IsNodeShapeChangeable() const override {
        return true;
    }
    virtual void Execute_NodeBackgroundColor(UI_EXECUTIONVERB verb, D2D1_COLOR_F color) override {
        if (verb == UI_EXECUTIONVERB_EXECUTE) {
            m_nodeBackgroundColor = color;
        }
    }
    virtual void Execute_NodeShape(UI_EXECUTIONVERB verb, NODE_SHAPE nodeShape) override {
        if (verb == UI_EXECUTIONVERB_EXECUTE) {
            m_nodeShape = nodeShape;
        }
    }
    virtual D2D1_COLOR_F UpdateProperty_GetNodeBackgroundColor() override {
        return m_nodeBackgroundColor;
    }
    virtual NODE_SHAPE UpdateProperty_GetNodeShape() override {
        return m_nodeShape;
    }
    // INodeProperty (end)

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
    virtual void Execute_LineColor(UI_EXECUTIONVERB verb, D2D1_COLOR_F color) override {
        if (verb == UI_EXECUTIONVERB_EXECUTE) {
            m_lineColor = color;
        }
    }
    virtual void Execute_LineWidth(UI_EXECUTIONVERB verb, LINE_WIDTH width) override {
        if (verb == UI_EXECUTIONVERB_EXECUTE) {
            m_lineWidth = width;
        }
    }
    virtual void Execute_LineStyle(UI_EXECUTIONVERB verb, LINE_STYLE style) override {
        if (verb == UI_EXECUTIONVERB_EXECUTE) {
            m_lineStyle = style;
        }
    }
    virtual void Execute_LineEdgeStyle(UI_EXECUTIONVERB verb, LINE_EDGE_STYLE style) override {
        if (verb == UI_EXECUTIONVERB_EXECUTE) {
            m_lineEdgeStyle = style;
        }
    }
    virtual D2D1_COLOR_F UpdateProperty_GetLineColor() override {
        return m_lineColor;
    }
    virtual LINE_WIDTH UpdateProperty_GetLineWidth() override {
        return m_lineWidth;
    }
    virtual LINE_STYLE UpdateProperty_GetLineStyle() override {
        return m_lineStyle;
    }
    virtual LINE_EDGE_STYLE UpdateProperty_GetLineEdgeStyle() override {
        return m_lineEdgeStyle;
    }
    // ILineProperty (end)
};

} // selection
} // touchmind

#endif // TOUCHMIND_SELECTION_DEFAULTPROPERTYMODEL_H_