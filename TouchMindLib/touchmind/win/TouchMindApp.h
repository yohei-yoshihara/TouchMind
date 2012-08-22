#ifndef TOUCHMIND_WIN_TOUCHMINDAPP_H_
#define TOUCHMIND_WIN_TOUCHMINDAPP_H_

#include "forwarddecl.h"

namespace touchmind
{
namespace win
{

class TouchMindApp
{
public:
    static const int NUMBER_OF_DIALOG_FILTERS;
    static COMDLG_FILTERSPEC DIALOG_FILTERS[];
    static const UINT_PTR IDT_STATUSBAR_TIMER;

private:
    HWND m_hwnd;
    UINT m_ribbonHeight;
    std::unique_ptr<touchmind::win::CanvasPanel> m_pCanvasPanel;
    std::unique_ptr<touchmind::Configuration> m_pConfiguration;
    std::unique_ptr<touchmind::tsf::TSFManager> m_pTsfManager;
    std::unique_ptr<touchmind::shell::MRUManager> m_pMruManager;
    std::unique_ptr<touchmind::control::StatusBar> m_pStatusBar;
    std::unique_ptr<touchmind::ribbon::RibbonFramework> m_pRibbonFramework;
    std::unique_ptr<touchmind::ribbon::dispatch::RibbonRequestDispatcher> m_ribbonRequestDispatcher;
    std::unique_ptr<touchmind::operation::FileOperation> m_fileOperation;

protected:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    void _OnOpenFile(const std::wstring &fileName);
    void _OnOpenFreeMind(const std::wstring &fileName);
    void _OnSave();
    void _OnSaveAs(const std::wstring &fileName);
    void _InitializeRibbon();
    void _InitializeShellLibrary();
    void _GetDisplayLocation(HWND hWnd, POINT &point);

public:
    TouchMindApp(void);
    virtual ~TouchMindApp(void);
    void DiscardResources();
    void Initialize();
    void OnCreate(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    void RunMessageLoop();
    void SetHwnd(HWND hwnd) {
        m_hwnd = hwnd;
    }
    HWND GetHwnd() const {
        return m_hwnd;
    }
    void SetRibbonHeight(UINT ribbonHeight) {
        m_ribbonHeight = ribbonHeight;
    }
    UINT GetRibbonHeight() const {
        return m_ribbonHeight;
    }
    UINT GetStatusBarHeight() const;
    touchmind::win::CanvasPanel* GetCanvasPanel() const {
        return m_pCanvasPanel.get();
    }
    touchmind::model::MapModel* GetMapModel() const;
    touchmind::Configuration* GetConfiguration() const {
        return m_pConfiguration.get();
    }
    touchmind::tsf::TSFManager* GetTSFManager() const {
        return m_pTsfManager.get();
    }
    touchmind::ribbon::RibbonFramework* GetRibbonFramework() {
        return m_pRibbonFramework.get();
    }
    IUIFramework *GetUIFramework();
    touchmind::ribbon::dispatch::RibbonRequestDispatcher* GetRibbonRequestDispatcher() const {
        return m_ribbonRequestDispatcher.get();
    }
    touchmind::operation::FileOperation* GetFileOperation() const {
        return m_fileOperation.get();
    }

    void OnContextMenu(HWND hWnd, POINT ptLocation);
    void OnOpenFileByExtension(const std::wstring & fileName);
    void OnNew();
    void OnOpenFile(void);
    void OnOpenFreeMind(void);
    void OnSave();
    void OnSaveAs();
    void OnCreateChildNode();
    void OnEditNode();
    void OnExit();
    void OnEditCopy();
    void OnEditPaste();
    void OnEditCut();
    void OnEditDelete();
    void OnEditUndo();
    void OnPrint();
    void OnPrintSetup();
    void CheckRequiredSave();
    void OnCreateLink();
    bool OnCreateLink_UpdateProperty();
    void OnDeleteLink();
    void OnExpandNodes();
    void OnCollapseNodes();
    bool OnClose();

    void SetStatusBarText(const wchar_t *text);
    void ClearStatusBarText();

    HRESULT PopulateRibbonRecentItems(PROPVARIANT *pvarValue);
    HRESULT GetSelectedItem(
        IN const PROPERTYKEY *key,
        IN const PROPVARIANT *ppropvarValue,
        OUT std::wstring &displayName,
        OUT std::wstring &fullPath);

    // Debug
    void OnDebugDump();
};

} // win
} // touchmind

#endif // TOUCHMIND_WIN_TOUCHMINDAPP_H_