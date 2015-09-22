#ifndef TOUCHMIND_VIEW_BASEVIEW_H_
#define TOUCHMIND_VIEW_BASEVIEW_H_

#include "forwarddecl.h"
#include "touchmind/Common.h"

//#define DEBUG_VIEW_RESOURCE

namespace touchmind {
  namespace view {

    class BaseView {
    private:
      bool m_isHandled;
      REPAINT_COUNTER m_repaintCounter;

    public:
      BaseView()
          : m_isHandled(false)
          , m_repaintCounter(VIEW_INITIAL_REPAINT_COUNTER) {
#ifdef DEBUG_VIEW_RESOURCE
        LOG(SEVERITY_LEVEL_DEBUG) << L"[VIEW RESOURCE] Aquire [" << std::hex << this << L"]" << std::dec;
#endif
      }
      virtual ~BaseView() {
#ifdef DEBUG_VIEW_RESOURCE
        LOG(SEVERITY_LEVEL_DEBUG) << L"[VIEW RESOURCE] Release [" << std::hex << this << L"]" << std::dec;
#endif
      }
      void SetHandled() {
        m_isHandled = true;
      }
      void ClearHandled() {
        m_isHandled = false;
      }
      bool IsHandled() const {
        return m_isHandled;
      }
      void SetRepaintCounter(const std::shared_ptr<touchmind::model::BaseModel> &model);
      REPAINT_COUNTER GetRepaintCounter() const {
        return m_repaintCounter;
      }
      virtual bool IsCompatible(const std::shared_ptr<touchmind::model::BaseModel> &model) = 0;

      virtual void CreateDeviceDependentResources(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget) = 0;
      virtual void DiscardDeviceDependentResources() = 0;
      virtual void Draw(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget) = 0;
      virtual bool HitTest(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget, D2D1_POINT_2F point) = 0;
    };

  } // view
} // touchmind

#endif // TOUCHMIND_VIEW_BASEVIEW_H_