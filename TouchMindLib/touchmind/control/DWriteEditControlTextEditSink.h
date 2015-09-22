#ifndef TOUCHMIND_CONTROL_DWRITEEDITCONTROLTEXTEDITSINK_H_
#define TOUCHMIND_CONTROL_DWRITEEDITCONTROLTEXTEDITSINK_H_

#include "forwarddecl.h"
#include "DWriteEditControlCommon.h"
#include "DWriteEditControlDisplayAttribute.h"

namespace touchmind {
  namespace control {

    class DWriteEditControlTextEditSink : public ITfTextEditSink {
    private:
      std::weak_ptr<DWriteEditControl> m_pEditControl;
      DWORD m_ObjRefCount;
      DWriteEditControlDisplayAttribute m_displayAttribute;
      DWORD m_editCookie;

    public:
      DWriteEditControlTextEditSink(std::weak_ptr<DWriteEditControl> pEditControl);
      virtual ~DWriteEditControlTextEditSink(void);

      void SetDisplayAttributeMgr(ITfDisplayAttributeMgr *pDisplayAttributeMgr);
      void SetCategoryMgr(ITfCategoryMgr *pCategoryMgr);

      // IUnknown methods.
      STDMETHOD(QueryInterface)(REFIID, LPVOID *);
      STDMETHOD_(DWORD, AddRef)();
      STDMETHOD_(DWORD, Release)();

      // ITfTextEditSink
      STDMETHODIMP OnEndEdit(ITfContext *pic, TfEditCookie ecReadOnly, ITfEditRecord *pEditRecord);

      HRESULT Advise(ITfContext *pTfContext);
      HRESULT Unadvise(ITfContext *pTfContext);
    };

  } // control
} // touchmind

#endif // TOUCHMIND_CONTROL_DWRITEEDITCONTROLTEXTEDITSINK_H_