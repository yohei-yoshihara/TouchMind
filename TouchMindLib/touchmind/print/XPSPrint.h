#ifndef TOUCHMIND_PRINT_XPSPRINT_H_
#define TOUCHMIND_PRINT_XPSPRINT_H_

#include "forwarddecl.h"
#include "touchmind/print/XPSPrintCommon.h"

namespace touchmind {
  namespace print {

    class XPSPrint {
    private:
      HWND m_parentHWnd;
      std::wstring m_deviceName;
      std::wstring m_language;
      XPS_SIZE m_pageSize;
      XPSPRINT_MARGIN m_xpsMargin;
      CComPtr<IXpsOMObjectFactory> m_pXpsFactory;
      CComPtr<IXpsOMPackage> m_pXpsPackage;
      CComPtr<IXpsOMCanvas> m_pXpsCanvas;
      CComPtr<IXpsOMDictionary> m_pXpsDictionary;
      CComPtr<IXpsOMPartResources> m_pXpsResources;
      CComPtr<XpsDWriteTextRenderer> m_pTextRenderer;
      touchmind::view::node::NodeViewManager *m_pNodeViewManager;
      touchmind::control::DWriteEditControlManager *m_pEditControlManager;
      XPS_MATRIX m_xpsMatrix;
      FLOAT m_scale;
      touchmind::Configuration *m_pConfiguration;

      LPPRINTDLGEX m_pPDX;
      LPPRINTPAGERANGE m_pPageRanges;

    protected:
      void _ShowPrintDlg();
      void _PrintMapModel(const touchmind::model::MapModel *pMapModel);
      void _CreateCanvas();
      void _PrintNodes(const std::shared_ptr<touchmind::model::node::NodeModel> &node);
      void _PrintNode(const std::shared_ptr<touchmind::model::node::NodeModel> &node);
      void _PrintChildrenPaths(const std::shared_ptr<touchmind::model::node::NodeModel> &node);
      void _PrintPath(const std::shared_ptr<touchmind::model::node::NodeModel> &child,
                      const touchmind::model::CurvePoints &curvePoints);
      void _PrintLinks(const touchmind::model::MapModel *pMapModel);
      void _PrintLink(const std::shared_ptr<touchmind::model::link::LinkModel> &link);
      void _PrintLinkEdge(const std::shared_ptr<touchmind::model::link::LinkModel> &link, EDGE_ID edgeId);
      void _DiscardResources();

    public:
      XPSPrint(void);
      virtual ~XPSPrint(void);
      void SetParentHWnd(HWND parentHWnd) {
        m_parentHWnd = parentHWnd;
      }
      IXpsOMObjectFactory *GetXpsFactory() const {
        return m_pXpsFactory;
      }
      IXpsOMPackage *GetXpsPackage() const {
        return m_pXpsPackage;
      }
      IXpsOMCanvas *GetXpsCanvas() const {
        return m_pXpsCanvas;
      }
      XpsDWriteTextRenderer *GetTextRenderer() const {
        return m_pTextRenderer;
      }
      void SetDeviceName(const std::wstring &deviceName) {
        m_deviceName = deviceName;
      }
      const std::wstring &GetDeviceName() const {
        return m_deviceName;
      }
      void SetPageSize(XPS_SIZE pageSize) {
        m_pageSize = pageSize;
      }
      XPS_SIZE GetPageSize() const {
        return m_pageSize;
      }
      void SetMargin(touchmind::print::XPSPRINT_MARGIN margin) {
        m_xpsMargin = margin;
      }
      touchmind::print::XPSPRINT_MARGIN GetMargin() const {
        return m_xpsMargin;
      }
      void SetLanguage(const std::wstring &language) {
        m_language = language;
      }
      const std::wstring &GetLanguage() const {
        return m_language;
      }
      void SetNodeViewManager(touchmind::view::node::NodeViewManager *pNodeViewManager) {
        m_pNodeViewManager = pNodeViewManager;
      }
      touchmind::view::node::NodeViewManager *GetNodeViewManager() const {
        return m_pNodeViewManager;
      }
      void SetEditControlManager(touchmind::control::DWriteEditControlManager *pEditControlManager) {
        m_pEditControlManager = pEditControlManager;
      }
      touchmind::control::DWriteEditControlManager *GetEditControlManager() const {
        return m_pEditControlManager;
      }
      void SetConfiguration(touchmind::Configuration *pConfiguration) {
        m_pConfiguration = pConfiguration;
      }
      touchmind::Configuration *GetConfiguration() const {
        return m_pConfiguration;
      }
      void Initialize();
      void Print(touchmind::model::MapModel *pMapModel);
      friend class PrintNodeVisitor;
      friend class PrintChildrenLinksVisitor;
    };

  } // print
} // touchmind

#endif // TOUCHMIND_PRINT_XPSPRINT_H_