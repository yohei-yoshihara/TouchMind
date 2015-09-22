#ifndef TOUCHMIND_RIBBON_HANDLER_NODESHAPECOMMANDHANDLER_H_
#define TOUCHMIND_RIBBON_HANDLER_NODESHAPECOMMANDHANDLER_H_

#include "forwarddecl.h"

namespace touchmind {
  namespace ribbon {
    namespace handler {

      class NodeShapeCommandHandler : public IUICommandHandler {
      private:
        const static std::array<int, NUMBER_OF_NODE_SHAPES> s_IMAGE_ID;
        ULONG m_refCount;
        touchmind::ribbon::RibbonFramework *m_pRibbonFramework;
        touchmind::ribbon::dispatch::RibbonRequestDispatcher *m_pRibbonRequestDispatcher;
        std::array<CComPtr<IUIImage>, NUMBER_OF_NODE_SHAPES> m_pImages;

      protected:
        static UINT32 _ConvertToIndex(NODE_SHAPE nodeShape);
        static NODE_SHAPE _ConvertToNodeShape(UINT32 index);

      public:
        static HRESULT CreateInstance(touchmind::ribbon::RibbonFramework *pRibbonFramework,
                                      touchmind::ribbon::dispatch::RibbonRequestDispatcher *pRibbonRequestDispatcher,
                                      IUICommandHandler **ppCommandHandler);
        NodeShapeCommandHandler();
        NodeShapeCommandHandler(touchmind::ribbon::RibbonFramework *pRibbonFramework,
                                touchmind::ribbon::dispatch::RibbonRequestDispatcher *pRibbonRequestDispatcher);
        virtual ~NodeShapeCommandHandler();

        // IUnknown interface
        IFACEMETHODIMP_(ULONG) AddRef();
        IFACEMETHODIMP_(ULONG) Release();
        IFACEMETHODIMP QueryInterface(REFIID riid, void **ppInterface);

        // IUICommandHandler interface
        IFACEMETHODIMP Execute(UINT cmdID, UI_EXECUTIONVERB verb, const PROPERTYKEY *pKey,
                               const PROPVARIANT *pPropvarValue, IUISimplePropertySet *pCommandExecutionProperties);
        IFACEMETHODIMP UpdateProperty(UINT cmdID, REFPROPERTYKEY key, const PROPVARIANT *pPropvarCurrentValue,
                                      PROPVARIANT *pPropvarNewValue);
      };

    } // handler
  } // ribbon
} // touchmind

#endif // TOUCHMIND_RIBBON_HANDLER_NODESHAPECOMMANDHANDLER_H_