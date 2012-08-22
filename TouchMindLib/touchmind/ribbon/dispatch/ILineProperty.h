#ifndef TOUCHMIND_RIBBON_DISPATCH_ILINEPROPERTY_H_
#define TOUCHMIND_RIBBON_DISPATCH_ILINEPROPERTY_H_

#include "touchmind/Common.h"

namespace touchmind
{
namespace ribbon
{
namespace dispatch
{

class ILineProperty
{
public:
    virtual bool UpdateProperty_IsLineColorChangeable() const = 0;
    virtual bool UpdateProperty_IsLineWidthChangeable() const = 0;
    virtual bool UpdateProperty_IsLineStyleChangeable() const = 0;
    virtual bool UpdateProperty_IsLineEdgeStyleChangeable() const = 0;
    virtual void Execute_LineColor(UI_EXECUTIONVERB verb, D2D1_COLOR_F color) = 0;
    virtual void Execute_LineWidth(UI_EXECUTIONVERB verb, LINE_WIDTH width) = 0;
    virtual void Execute_LineStyle(UI_EXECUTIONVERB verb, LINE_STYLE style) = 0;
    virtual void Execute_LineEdgeStyle(UI_EXECUTIONVERB verb, LINE_EDGE_STYLE style) = 0;
    virtual D2D1_COLOR_F UpdateProperty_GetLineColor() = 0;
    virtual LINE_WIDTH UpdateProperty_GetLineWidth() = 0;
    virtual LINE_STYLE UpdateProperty_GetLineStyle() = 0;
    virtual LINE_EDGE_STYLE UpdateProperty_GetLineEdgeStyle() = 0;
};

} // dispatch
} // ribbon
} // touchmind

#endif // TOUCHMIND_RIBBON_DISPATCH_ILINEPROPERTY_H_