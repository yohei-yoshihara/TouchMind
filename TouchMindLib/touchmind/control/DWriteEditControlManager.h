#ifndef TOUCHMIND_CONTROL_DWRITEEDITCONTROLMANAGER_H_
#define TOUCHMIND_CONTROL_DWRITEEDITCONTROLMANAGER_H_

#include "forwarddecl.h"
#include "touchmind/control/DWriteEditControlCommon.h"
#include "touchmind/text/FontAttributeCommand.h"
#include "touchmind/tsf/TSFManager.h"

namespace touchmind {
  namespace control {

    class IDWLightEditControlChangeListener {
    public:
      virtual void NotifyEditControlHasChanged(EDIT_CONTROL_INDEX editControlIndex) = 0;
    };

    class IDWLightEditControlSelectionChangeListener {
    public:
      virtual void NotifySelectionHasChanged(EDIT_CONTROL_INDEX editControlIndex) = 0;
    };

    class DWriteEditControlManager : public std::enable_shared_from_this<DWriteEditControlManager> {
    private:
      static EDIT_CONTROL_INDEX s_masterIndex;

      HWND m_parentHWnd;
      EditControlIndexToEditControlPtrMap m_dwEditControlMap;
      std::shared_ptr<control::DWriteEditControl> m_pSelectedEditControl;
      std::shared_ptr<control::DWriteEditControl> m_pFocusedEditControl;
      touchmind::Configuration *m_pConfiguration;
      touchmind::tsf::TSFManager *m_pTSFManager;
      touchmind::util::ScrollBarHelper *m_pScrollBarHelper;

      touchmind::text::FontAttributeCommand m_defaultFontInfo;

      bool m_dragging;
      D2D1_POINT_2F m_dragStartPoint;
      D2D1_POINT_2F m_dragEndPoint;

      ID2D1Factory *m_pD2DFactory;
      ID2D1RenderTarget *m_pRenderTarget;
      IDWriteFactory *m_pDWriteFactory;

      std::vector<IDWLightEditControlChangeListener *> m_editControlChangeListener;
      std::vector<IDWLightEditControlSelectionChangeListener *> m_selectionChangeListener;

    protected:
      void _FireEditControlChanged(EDIT_CONTROL_INDEX editControlIndex);
      void _FireSelectionChanged(EDIT_CONTROL_INDEX editControlIndex);

    public:
      DWriteEditControlManager();
      virtual ~DWriteEditControlManager(void);

      void Initialize(HWND parentHWnd, touchmind::Configuration *pConfiguration,
                      touchmind::tsf::TSFManager *pTSFManager, ID2D1Factory *pD2DFactory,
                      IDWriteFactory *pDWriteFactory);
      void InitializeDeviceResources(ID2D1RenderTarget *pRenderTarget);

      HWND GetParentHWnd(void) const {
        return m_parentHWnd;
      }
      ITfThreadMgr *GetThreadMgr(void) const {
        return m_pTSFManager->GetThreadMgr();
      }
      TfClientId GetClientId(void) const {
        return m_pTSFManager->GetClientId();
      }
      ITfDisplayAttributeMgr *GetDisplayAttributeMgr(void) const {
        return m_pTSFManager->GetDisplayAttributeMgr();
      }
      ITfCategoryMgr *GetCategoryMgr(void) const {
        return m_pTSFManager->GetCatetoryMgr();
      }
      void SetScrollBarHelper(touchmind::util::ScrollBarHelper *pScrollBarHelper) {
        m_pScrollBarHelper = pScrollBarHelper;
      }
      touchmind::util::ScrollBarHelper *GetScrollBarHelper() const {
        return m_pScrollBarHelper;
      }
      ID2D1Factory *GetD2D1Factory() const {
        return m_pD2DFactory;
      }
      ID2D1RenderTarget *GetD2DRenderTarget() const {
        return m_pRenderTarget;
      }
      void SetD2DRenderTarget(ID2D1RenderTarget *pRenderTarget) {
        m_pRenderTarget = pRenderTarget;
      }
      IDWriteFactory *GetDWriteFactory() const {
        return m_pDWriteFactory;
      }

      HRESULT CreateEditControl(IN D2D1_RECT_F layoutRect, IN const std::wstring &text,
                                OUT EDIT_CONTROL_INDEX *editControlIndex);
      size_t GetEditControlCount();
      std::shared_ptr<control::DWriteEditControl> GetEditControl(IN EDIT_CONTROL_INDEX editControlIndex);
      EDIT_CONTROL_INDEX GetEditControlIndex(const std::shared_ptr<control::DWriteEditControl> &pEditControl);
      void DestroyDWLightEditControl(IN EDIT_CONTROL_INDEX editControlIndex);
      std::shared_ptr<control::DWriteEditControl> HitText(FLOAT x, FLOAT y);
      bool SetFocus(FLOAT x, FLOAT y);
      bool SetFocus(IN EDIT_CONTROL_INDEX editControlIndex);
      bool KillFocus();
      std::shared_ptr<control::DWriteEditControl> GetFocusedEditControl();
      void SetSelectedNode(IN EDIT_CONTROL_INDEX editControlIndex);
      void CancelSelectedNode();
      HRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, OUT bool *eaten);
      void OnRender(ID2D1RenderTarget *pRenderTarget);

      // For Ribbon Font Control support
      void SetValues(IN IPropertyStore *pPropertyStore);
      void GetValues(IN IPropertyStore *pPropertyStore);
      void SetPreviewValues(IN IPropertyStore *pPropertyStore);
      void CancelPreview(IN IPropertyStore *pPropertyStore);
      void ShowSelection();
      static void GetFontInfoFromIPropertyStore(IN IPropertyStore *pPropertyStore,
                                                OUT touchmind::text::FontAttributeCommand *pFontInfo);
      static void GetIPropertyStoreFromFontInfo(IN touchmind::text::FontAttributeCommand *pFontInfo,
                                                OUT IPropertyStore *pPropertyStore);
      void NotifySelectionHasChanged(const std::shared_ptr<control::DWriteEditControl> &editControlPtr);

      void AddEditControlChangeListener(IDWLightEditControlChangeListener *pListener);
      void RemoveEditControlChangeListener(IDWLightEditControlChangeListener *pListener);

      void AddSelectionChangeListener(IDWLightEditControlSelectionChangeListener *pListener);
      void RemoveSelectionChangeListener(IDWLightEditControlSelectionChangeListener *pListener);

      // Debug
      void OnDebugDump(std::wofstream &os);
    };

  } // control
} // touchmind

#endif // TOUCHMIND_CONTROL_DWRITEEDITCONTROLMANAGER_H_