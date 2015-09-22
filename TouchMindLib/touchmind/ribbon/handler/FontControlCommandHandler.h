#ifndef TOUCHMIND_RIBBON_HANDLER_FONTCONTROLCHANDLER_H_
#define TOUCHMIND_RIBBON_HANDLER_FONTCONTROLCHANDLER_H_

#include "forwarddecl.h"

namespace touchmind {
  namespace ribbon {
    namespace handler {

      class FontControlCommandHandler : public IUICommandHandler {
      private:
        LONG m_refCount;
        touchmind::control::DWriteEditControlManager *m_pEditControlManager;

      protected:
        FontControlCommandHandler()
            : m_refCount(1)
            , m_pEditControlManager(nullptr) {
        }
        virtual ~FontControlCommandHandler() {
        }

      public:
        static HRESULT CreateInstance(OUT IUICommandHandler **ppCommandHandler);

        // IUnknown
        STDMETHODIMP_(ULONG) AddRef();
        STDMETHODIMP_(ULONG) Release();
        STDMETHODIMP QueryInterface(REFIID iid, void **ppv);

        // IUICommandHandler
        STDMETHOD(UpdateProperty)(IN UINT nCmdID,
                              IN IN REFPROPERTYKEY key,
                              IN const PROPVARIANT* ppropvarCurrentValue,
                              OUT PROPVARIANT* ppropvarNewValue);
        STDMETHOD(Execute)(IN UINT nCmdID,
                       IN UI_EXECUTIONVERB verb,
                       IN const PROPERTYKEY* key,
                       IN const PROPVARIANT* ppropvarValue,
                       IN IUISimplePropertySet* pCommandExecutionProperties);

        void SetEditControlManager(touchmind::control::DWriteEditControlManager *pEditControlManager) {
          m_pEditControlManager = pEditControlManager;
        }
      };

    } // handler
  } // ribbon
} // touchmind

#endif // TOUCHMIND_RIBBON_HANDLER_FONTCONTROLCHANDLER_H_
