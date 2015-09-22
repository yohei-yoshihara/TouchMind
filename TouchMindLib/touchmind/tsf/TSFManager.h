#ifndef TOUCHMIND_TSF_TSFMANAGER_H_
#define TOUCHMIND_TSF_TSFMANAGER_H_

namespace touchmind {
  namespace tsf {

    class TSFManager {
    private:
      HACCEL m_hAccel;
      HWND m_hWnd;
      ITfThreadMgr *m_pThreadMgr;
      ITfKeystrokeMgr *m_pKeyMgr;
      ITfMessagePump *m_pMsgPump;
      TfClientId m_tfClientID;
      bool m_isThreadMgrActivated;
      ITfCategoryMgr *m_pCategoryMgr;
      ITfDisplayAttributeMgr *m_pDisplayAttrMgr;

    public:
      TSFManager(void);
      virtual ~TSFManager(void);
      void SetHAccel(HACCEL hAccel) {
        m_hAccel = hAccel;
      }
      void SetHWnd(HWND hWnd) {
        m_hWnd = hWnd;
      }
      void Initialize();
      int RunMessageLoop();
      ITfThreadMgr *GetThreadMgr() const {
        return m_pThreadMgr;
      }
      ITfKeystrokeMgr *GetKeystrokeMgr() const {
        return m_pKeyMgr;
      }
      ITfMessagePump *GetMessagePump() const {
        return m_pMsgPump;
      }
      TfClientId GetClientId() const {
        return m_tfClientID;
      }
      HRESULT ActivateThreadMgr();
      HRESULT DeactivateThreadMagr();
      ITfCategoryMgr *GetCatetoryMgr() const {
        return m_pCategoryMgr;
      }
      ITfDisplayAttributeMgr *GetDisplayAttributeMgr() const {
        return m_pDisplayAttrMgr;
      }
    };

  } // tsf
} // touchmind

#endif // TOUCHMIND_TSF_TSFMANAGER_H_