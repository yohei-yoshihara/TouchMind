#ifndef TOUCHMIND_ANIMATION_ANIMATIONMANAGEREVENTHANDLER_H_
#define TOUCHMIND_ANIMATION_ANIMATIONMANAGEREVENTHANDLER_H_

#include "forwarddecl.h"
#include "touchmind/animation/IAnimationStatusChangedListener.h"

namespace touchmind {
  namespace animation {

    template <class IUIAnimationCallback, class CUIAnimationCallbackDerived>
    class CUIAnimationCallbackBase : public IUIAnimationCallback {
    public:
      static __checkReturn COM_DECLSPEC_NOTHROW HRESULT
      CreateInstance(__deref_out IUIAnimationCallback **ppUIAnimationCallback,
                     __deref_opt_out CUIAnimationCallbackDerived **ppUIAnimationCallbackDerived = nullptr) {
        if (ppUIAnimationCallbackDerived != nullptr) {
          *ppUIAnimationCallbackDerived = nullptr;
        }

        if (ppUIAnimationCallback == nullptr) {
          return E_POINTER;
        }

        CUIAnimationCallbackObject<IUIAnimationCallback, CUIAnimationCallbackDerived> *pUIAnimationCallbackDerived
            = new CUIAnimationCallbackObject<IUIAnimationCallback, CUIAnimationCallbackDerived>;

        if (pUIAnimationCallbackDerived == nullptr) {
          *ppUIAnimationCallback = nullptr;
          return E_OUTOFMEMORY;
        }

        *ppUIAnimationCallback = static_cast<IUIAnimationCallback *>(pUIAnimationCallbackDerived);
        (*ppUIAnimationCallback)->AddRef();

        if (ppUIAnimationCallbackDerived != nullptr) {
          *ppUIAnimationCallbackDerived = pUIAnimationCallbackDerived;
        }

        return S_OK;
      }

    protected:
      COM_DECLSPEC_NOTHROW HRESULT QueryInterfaceCallback(__in REFIID riid, __in REFIID riidCallback,
                                                          __deref_out void **ppvObject) {
        if (ppvObject == nullptr) {
          return E_POINTER;
        }

        static const IID IID_UNKNOWN = {0x00000000, 0x0000, 0x0000, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46};
        if (::InlineIsEqualGUID(riid, IID_UNKNOWN)) {
          *ppvObject = static_cast<IUIAnimationCallback *>(this);
          AddRef();
          return S_OK;
        }

        if (::InlineIsEqualGUID(riid, riidCallback)) {
          *ppvObject = static_cast<IUIAnimationCallback *>(this);
          AddRef();
          return S_OK;
        }

        *ppvObject = nullptr;
        return E_NOINTERFACE;
      }

    private:
      template <class IUIAnimationCallback, class CUIAnimationCallbackDerived>
      class CUIAnimationCallbackObject : public CUIAnimationCallbackDerived {
      public:
        CUIAnimationCallbackObject()
            : m_dwRef(0) {
        }
        IFACEMETHOD(QueryInterface)(__in REFIID riid, __deref_out void **ppvObject) {
          IUIAnimationCallback **ppAnimationInterface = reinterpret_cast<IUIAnimationCallback **>(ppvObject);
          return QueryInterfaceCallback(riid, IID_PPV_ARGS(ppAnimationInterface));
        }

        IFACEMETHOD_(ULONG, AddRef)() {
          return ++m_dwRef;
        }

        IFACEMETHOD_(ULONG, Release)() {
          if (--m_dwRef == 0) {
            delete this;
            return 0;
          }

          return m_dwRef;
        }

      private:
        DWORD m_dwRef;
      };
    };

    template <class CManagerEventHandlerDerived>
    class CUIAnimationManagerEventHandlerBase
        : public CUIAnimationCallbackBase<IUIAnimationManagerEventHandler, CManagerEventHandlerDerived> {
    public:
      IFACEMETHOD(OnManagerStatusChanged)(
        __in UI_ANIMATION_MANAGER_STATUS newStatus,
        __in UI_ANIMATION_MANAGER_STATUS previousStatus
    ) PURE;
    };

    class AnimationManagerEventHandler : public CUIAnimationManagerEventHandlerBase<AnimationManagerEventHandler> {
    protected:
      IAnimationStatusChangedListener *m_pListener;

    public:
      static COM_DECLSPEC_NOTHROW HRESULT CreateInstance(IAnimationStatusChangedListener *pListener,
                                                         IUIAnimationManagerEventHandler **ppManagerEventHandler);
      AnimationManagerEventHandler();
      virtual ~AnimationManagerEventHandler();
      IFACEMETHOD(OnManagerStatusChanged)(UI_ANIMATION_MANAGER_STATUS newStatus, UI_ANIMATION_MANAGER_STATUS previousStatus);
    };

  } // animation
} // touchmind

#endif // TOUCHMIND_ANIMATION_ANIMATIONMANAGEREVENTHANDLER_H_
