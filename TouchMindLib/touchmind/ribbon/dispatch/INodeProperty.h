#ifndef TOUCHMIND_RIBBON_DISPATCH_INODEPROPERTY_H_
#define TOUCHMIND_RIBBON_DISPATCH_INODEPROPERTY_H_

#include "touchmind/Common.h"

namespace touchmind {
  namespace ribbon {
    namespace dispatch {

      class INodeProperty {
      public:
        virtual bool UpdateProperty_IsNodeBackgroundColorChangeable() const = 0;
        virtual bool UpdateProperty_IsNodeShapeChangeable() const = 0;
        virtual void Execute_NodeBackgroundColor(UI_EXECUTIONVERB verb, D2D1_COLOR_F color) = 0;
        virtual void Execute_NodeShape(UI_EXECUTIONVERB verb, NODE_SHAPE nodeShape) = 0;
        virtual D2D1_COLOR_F UpdateProperty_GetNodeBackgroundColor() = 0;
        virtual NODE_SHAPE UpdateProperty_GetNodeShape() = 0;
      };

    } // dispatch
  } // ribbon
} // touchmind

#endif // TOUCHMIND_RIBBON_DISPATCH_INODEPROPERTY_H_