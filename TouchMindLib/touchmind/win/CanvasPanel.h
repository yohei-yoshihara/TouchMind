#ifndef TOUCHMIND_WIN_CANVASPANEL_H_
#define TOUCHMIND_WIN_CANVASPANEL_H_

#include "forwarddecl.h"
#include "touchmind/Context.h"
#include "touchmind/animation/IAnimationStatusChangedListener.h"
#include "touchmind/layout/ILayoutManager.h"
#include "touchmind/model/CurvePoints.h"
#include "touchmind/control/DWriteEditControlCommon.h"
#include "touchmind/control/DWriteEditControlManager.h"
#include "touchmind/touch/ManipulationHelper.h"
#include "touchmind/text/FontAttribute.h"
#include "touchmind/model/MapModel.h"

namespace touchmind
{
namespace win
{

enum CANVAS_STATE {
    CANVAS_STATE_NORMAL,
    CANVAS_STATE_CREATING_LINK,
};

class CanvasPanel :
    public touchmind::IRenderEventListener,
    public touchmind::control::IDWLightEditControlChangeListener,
    public touchmind::control::IDWLightEditControlSelectionChangeListener,
    public touchmind::model::IMapModelChangeListener,
    public touchmind::animation::IAnimationStatusChangedListener
{
private:
    static bool s_hasDestroyed;
    CANVAS_STATE m_canvasState;
    bool m_isInitialized;
    HWND m_hwnd;
    UINT m_ribbonHeight;
    touchmind::win::TouchMindApp *m_pTouchMindApp;
    std::unique_ptr<touchmind::model::MapModel> m_pMapModel;
    std::unique_ptr<touchmind::Context> m_pContext;
    std::unique_ptr<touchmind::view::node::NodeViewManager> m_pNodeViewManager;
    std::unique_ptr<touchmind::view::link::LinkViewManager> m_pLinkViewManager;
    std::unique_ptr<touchmind::layout::LayoutManager> m_pLayoutManager;
    std::unique_ptr<touchmind::control::DWriteTextSizeProvider> m_pDWriteTextSizeProvider;
    std::unique_ptr<touchmind::util::ScrollBarHelper> m_pScrollBarHelper;
    std::unique_ptr<touchmind::touch::ManipulationHelper> m_pManipulationHelper;
    std::unique_ptr<touchmind::control::DWriteEditControlManager> m_pEditControlManager;
    std::unique_ptr<touchmind::shell::Clipboard> m_pClipboard;
    std::unique_ptr<touchmind::print::XPSPrintSetupDialog> m_pPrintSetupDialog;
    std::unique_ptr<touchmind::print::XPSPrint> m_pXPSPrint;
    std::unique_ptr<touchmind::operation::CreateLinkOperation> m_createLinkOperation;
    std::unique_ptr<touchmind::operation::NodeMovingOperation> m_nodeMovingOperation;
    std::unique_ptr<touchmind::operation::NodeResizingOperation> m_nodeResizingOperation;
    std::unique_ptr<touchmind::selection::SelectionManager> m_selectionManager;
    std::unique_ptr<touchmind::converter::NodeModelXMLEncoder> m_nodeModelXMLEncoder;
    std::unique_ptr<touchmind::converter::NodeModelXMLDecoder> m_nodeModelXMLDecoder;
    std::shared_ptr<touchmind::model::node::NodeModel> m_pEdittingNode; // must be synchronized with focused edit control

protected:
    void _InitializeAfterCreateWindow();

public:
    // The windows procedure.
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    CanvasPanel(void);
    virtual ~CanvasPanel(void);
    bool IsInitialized(void) const {
        return m_isInitialized;
    }
    void Initialize(UINT ribbonHeight);
    void OnCreate();
    void OnDestroy();
    void SetHwnd(HWND hwnd) {
        m_hwnd = hwnd;
    }
    HWND GetHwnd() const {
        return m_hwnd;
    }
    void SetTouchMindApp(TouchMindApp* pTouchMindApp) {
        m_pTouchMindApp = pTouchMindApp;
    }
    TouchMindApp* GetTouchMindApp() const {
        return m_pTouchMindApp;
    }
    touchmind::util::ScrollBarHelper* GetScrollBarHelper() const {
        return m_pScrollBarHelper.get();
    }
    touchmind::control::DWriteEditControlManager* GetEditControlManager() const {
        return m_pEditControlManager.get();
    }
    std::shared_ptr<touchmind::model::node::NodeModel> GetRootNodeModel() const;
    touchmind::Context* GetContext() const {
        return m_pContext.get();
    }
    CANVAS_STATE GetState() const {
        return m_canvasState;
    }
    touchmind::operation::CreateLinkOperation* GetCreateLinkOperation() const {
        return m_createLinkOperation.get();
    }
    touchmind::view::link::LinkViewManager* GetLinkViewManager() const {
        return m_pLinkViewManager.get();
    }
    touchmind::model::MapModel* GetMapModel() const;
    touchmind::selection::SelectionManager* GetSelectionManager() const {
        return m_selectionManager.get();
    }
    touchmind::view::node::NodeViewManager* GetNodeViewManager() const {
        return m_pNodeViewManager.get();
    }
    touchmind::operation::FileOperation* GetFileOperation() const;

    // Draw content.
    void OnRender();

    // Resize the render target.
    void OnResize(UINT width, UINT height);
    void ResizeByParentResized();
    void ResizeByRibbonResized(UINT ribbonHeight);
    void Invalidate();

    void CalculateScrolls(FLOAT width, FLOAT height);
    void OnHScroll(HWND hWnd, WPARAM wParam);
    void OnVScroll(HWND hWnd, WPARAM wParam);

    void CreateCurvePathGeometry(
        touchmind::model::CurvePoints* pCurvePoints,
        ID2D1Factory* pDirect2DFactory,
        ID2D1PathGeometry** pPathGeometry);

    void ArrangeNodes();
    void EditNode(const std::shared_ptr<touchmind::model::node::NodeModel> &node);
    void FinishEditting();
    void SetSelectedNode(const std::shared_ptr<touchmind::model::node::NodeModel> &node);
    void CancelSelectedNode();

    // IRenderEventListener (start)
    virtual HRESULT CreateDeviceIndependentResources(
        touchmind::Context*,
        ID2D1Factory*,
        IWICImagingFactory*,
        IDWriteFactory*) override { return S_OK; }
    virtual HRESULT CreateD3DResources(touchmind::Context*, ID3D10Device*) override {return S_OK;};
    virtual HRESULT CreateD2DResources(
        touchmind::Context* pContext,
        ID2D1Factory* pD2DFactory,
        ID2D1RenderTarget* pRenderTarget) override;
    virtual HRESULT Render2D(touchmind::Context* pContext, ID2D1RenderTarget* pRenderTarget) override;
    virtual HRESULT PrepareRender3D(touchmind::Context*, ID3D10Device*) override {return S_OK;}
    virtual HRESULT Render3D(touchmind::Context*, ID3D10Device*) override {return S_OK;}
    virtual HRESULT AfterRender3D(touchmind::Context*, ID2D1RenderTarget*) override {return S_OK;}
    virtual void DiscardDeviceResources() override;
    // IRenderEventListener (end)


    // Control::IDWLightEditControlChangeListener
    virtual void NotifyEditControlHasChanged(int editControlIndex);
    // Control::IDWLightEditControlSelectionChangeListener
    virtual void NotifySelectionHasChanged(int editControlIndex);

    // IAnimationStatusChangedListener (start)
    virtual HRESULT OnAnimationStatusChanged(
        UI_ANIMATION_MANAGER_STATUS newStatus,
        UI_ANIMATION_MANAGER_STATUS previousStatus) override;
    // IAnimationStatusChangedListener (end)

    void ExpandLayoutRect(const std::shared_ptr<touchmind::model::node::NodeModel> &node, FLOAT dx, FLOAT dy);
    void ResizeLayoutRect(const std::shared_ptr<touchmind::model::node::NodeModel> &node, FLOAT width, FLOAT height);

    // IMapModelChangeListener (start)
    virtual void BeforeNew(touchmind::model::MapModel* pMapModel) override;
    virtual void AfterNew(touchmind::model::MapModel* pMapModel) override;
    virtual void BeforeOpen(touchmind::model::MapModel* pMapModel) override;
    virtual void AfterOpen(touchmind::model::MapModel* pMapModel) override;
    virtual void BeforeSave(touchmind::model::MapModel* pMapModel) override;
    virtual void AfterSave(touchmind::model::MapModel* pMapModel) override;
    // IMapModelChangeListener (end)

    void OnEditCopy();
    void OnEditPaste();
    void OnEditCut();
    void OnEditDelete();
    void OnEditUndo();
    void OnPrint();
    void OnPrintSetup();
    void OnCreateLink();
    void OnCreateLink_End();
    void OnDeleteLink();

    void SetGestureConfigs(HWND hWnd);
    // Debug
    void OnDebugDump(std::wofstream& os);
};

} // win
} // touchmind

#endif // TOUCHMIND_WIN_CANVASPANEL_H_