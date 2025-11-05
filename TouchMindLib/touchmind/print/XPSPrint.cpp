#include "StdAfx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/layout/ILayoutManager.h"
#include "touchmind/layout/LayoutManager.h"
#include "touchmind/view/node/NodeViewManager.h"
#include "touchmind/control/DWriteEditControlCommon.h"
#include "touchmind/control/DWriteEditControlManager.h"
#include "touchmind/control/DWriteEditControl.h"
#include "touchmind/util/ColorUtil.h"
#include "touchmind/win/CanvasPanel.h"
#include "touchmind/print/XPSPrint.h"
#include "touchmind/print/XPSPrintSetupDialog.h"
#include "touchmind/print/XpsDWriteTextRenderer.h"
#include "touchmind/view/GeometryBuilder.h"
#include "touchmind/view/node/NodeViewManager.h"
#include "touchmind/print/PrintTicketHelper.h"
#include "touchmind/print/XPSGeometryBuilder.h"
#include "touchmind/util/ColorUtil.h"
#include <winspool.h>

//#define PRINT_TICKET_SUPPORT

namespace touchmind {
  namespace print {

    class PrintNodeVisitor {
    private:
      XPSPrint *m_pXPSPrint;

    public:
      PrintNodeVisitor(XPSPrint *pXPSPrint)
          : m_pXPSPrint(pXPSPrint) {
      }

      touchmind::VISITOR_RESULT operator()(std::shared_ptr<touchmind::model::node::NodeModel> node) {
        HRESULT hr = S_OK;

        m_pXPSPrint->_PrintNode(node);
        if (FAILED(hr)) {
          SPDLOG_WARN(L"_PrintNodeBody failed at node({}), hr = {:x}", node->GetId(), hr);
          return touchmind::VISITOR_RESULT_STOP;
        }

        XpsDWriteTextRenderer *pTextRenderer = m_pXPSPrint->GetTextRenderer();
        touchmind::view::node::NodeViewManager *pNodeViewManager = m_pXPSPrint->GetNodeViewManager();
        control::DWriteEditControlManager *pEditControlManager = m_pXPSPrint->GetEditControlManager();

        control::EDIT_CONTROL_INDEX editControlIndex = pNodeViewManager->GetEditControlIndexFromNodeId(node->GetId());
        std::shared_ptr<control::DWriteEditControl> pEditControl
            = pEditControlManager->GetEditControl(editControlIndex);
        if (pEditControl != nullptr) {
          IDWriteTextLayout *pTextLayout = pEditControl->GetTextLayout();
          if (pTextLayout != nullptr) {
            pTextLayout->Draw(nullptr, pTextRenderer, node->GetX() + m_pXPSPrint->GetConfiguration()->GetInsets().left,
                              node->GetY() + m_pXPSPrint->GetConfiguration()->GetInsets().top);
          } else {
            SPDLOG_WARN(L"TextLayout is null");
          }
        } else {
          SPDLOG_WARN(L"Could not found EditControl for Node({})", node->GetId());
        }
        return touchmind::VISITOR_RESULT_CONTINUE;
      }
    };

    class PrintChildrenLinksVisitor {
    private:
      XPSPrint *m_pXPSPrint;

    public:
      PrintChildrenLinksVisitor(XPSPrint *pXPSPrint)
          : m_pXPSPrint(pXPSPrint) {
      }

      touchmind::VISITOR_RESULT operator()(std::shared_ptr<touchmind::model::node::NodeModel> node) {
        m_pXPSPrint->_PrintChildrenPaths(node);
        return touchmind::VISITOR_RESULT_CONTINUE;
      }
    };

  } // print
} // touchmind

touchmind::print::XPSPrint::XPSPrint(void)
    : m_parentHWnd(nullptr)
    , m_deviceName()
    , m_language(L"en-US")
    , m_pageSize()
    , m_xpsMargin()
    , m_pXpsFactory(nullptr)
    , m_pXpsPackage(nullptr)
    , m_pXpsCanvas(nullptr)
    , m_pXpsDictionary(nullptr)
    , m_pXpsResources(nullptr)
    , m_pTextRenderer(nullptr)
    , m_pNodeViewManager(nullptr)
    , m_pEditControlManager()
    , m_xpsMatrix()
    , m_scale(1.0f)
    , m_pConfiguration(nullptr)
    , m_pPDX(nullptr)
    , m_pPageRanges(nullptr) {
  m_pageSize.width = 21.0f / 2.54f * 96.0f;
  m_pageSize.height = 29.0f / 2.54f * 96.0f;
}

touchmind::print::XPSPrint::~XPSPrint(void) {
}

void touchmind::print::XPSPrint::Initialize() {
  THROW_IF_FAILED(CoCreateInstance(__uuidof(XpsOMObjectFactory), NULL, CLSCTX_INPROC_SERVER,
                                          __uuidof(IXpsOMObjectFactory), reinterpret_cast<LPVOID *>(&m_pXpsFactory)));
}

void touchmind::print::XPSPrint::Print(touchmind::model::MapModel *pMapModel) {
  _ShowPrintDlg();

  if (m_pPDX->dwResultAction == PD_RESULT_PRINT) {
    _PrintMapModel(pMapModel);
  }

  _DiscardResources();
}

void touchmind::print::XPSPrint::_ShowPrintDlg() {
  THROW_IF_NULL_ALLOC(m_pPDX = static_cast<LPPRINTDLGEX>(GlobalAlloc(GPTR, sizeof(PRINTDLGEX))));

  THROW_IF_NULL_ALLOC(m_pPageRanges = static_cast<LPPRINTPAGERANGE>(
                          GlobalAlloc(GPTR, 10 * sizeof(PRINTPAGERANGE))));

  m_pPDX->lStructSize = sizeof(PRINTDLGEX);
  m_pPDX->hwndOwner = m_parentHWnd;
  m_pPDX->hDevMode = nullptr;
  m_pPDX->hDevNames = nullptr;
  m_pPDX->hDC = nullptr;
  m_pPDX->Flags = PD_RETURNDC | PD_COLLATE;
  m_pPDX->Flags2 = 0;
  m_pPDX->ExclusionFlags = 0;
  m_pPDX->nPageRanges = 0;
  m_pPDX->nMaxPageRanges = 10;
  m_pPDX->lpPageRanges = m_pPageRanges;
  m_pPDX->nMinPage = 1;
  m_pPDX->nMaxPage = 1000;
  m_pPDX->nCopies = 1;
  m_pPDX->hInstance = 0;
  m_pPDX->lpPrintTemplateName = nullptr;
  m_pPDX->lpCallback = nullptr;
  m_pPDX->nPropertyPages = 0;
  m_pPDX->lphPropertyPages = nullptr;
  m_pPDX->nStartPage = START_PAGE_GENERAL;
  m_pPDX->dwResultAction = 0;

  THROW_IF_FAILED(::PrintDlgEx(m_pPDX));

  if (m_pPDX->dwResultAction == PD_RESULT_PRINT || m_pPDX->dwResultAction == PD_RESULT_APPLY) {
    LPDEVNAMES devnames = (LPDEVNAMES)GlobalLock(m_pPDX->hDevNames);
    m_deviceName = (wchar_t *)devnames + devnames->wDeviceOffset;
    GlobalUnlock(m_pPDX->hDevNames);
  }
}

void touchmind::print::XPSPrint::_PrintMapModel(const touchmind::model::MapModel *pMapModel) {
  HRESULT hr = S_OK;

  auto node = pMapModel->GetRootNodeModel();
  XPSGeometryBuilder::CalculateTransformMatrix(m_pageSize, m_xpsMargin, node, m_xpsMatrix, m_scale);
  _CreateCanvas();

  m_pTextRenderer = nullptr;
  THROW_IF_FAILED(XpsDWriteTextRenderer::CreateInstance(m_pXpsFactory, &m_pTextRenderer));
  m_pTextRenderer->SetXpsCanvas(m_pXpsCanvas);
  m_pTextRenderer->SetXpsDictionary(m_pXpsDictionary);
  m_pTextRenderer->SetXpsResources(m_pXpsResources);

  _PrintNodes(node);
  _PrintLinks(pMapModel);

  HANDLE completionEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
  THROW_IF_NULL_ALLOC(completionEvent);

  IXpsPrintJob *job = nullptr;
  IXpsPrintJobStream *jobStream = nullptr;
#ifdef PRINT_TICKET_SUPPORT
  IXpsPrintJobStream *printTicketStream = nullptr;
#endif
  hr = StartXpsPrintJob(m_deviceName.c_str(), pMapModel->HasFileName() ? pMapModel->GetFileName().c_str() : nullptr,
                        nullptr, nullptr, completionEvent, nullptr, 0, &job, &jobStream,
#ifdef PRINT_TICKET_SUPPORT
                        &printTicketStream
#else
                        nullptr
#endif
                        );
  if (FAILED(hr)) {
    SPDLOG_ERROR(L"StartXpsPrintJob failed, hr = {:x}", hr);
  }

  if (SUCCEEDED(hr)) {
    hr = m_pXpsPackage->WriteToStream(jobStream, FALSE);
    if (FAILED(hr)) {
      SPDLOG_ERROR(L"IXpsOMPackage::WriteToStream failed, hr = {:x}", hr);
    }
  }

#ifdef PRINT_TICKET_SUPPORT
  if (SUCCEEDED(hr)) {
    PrintTicketHelper::CreatePrintTicket(m_deviceName, m_pPDX, printTicketStream);
  }
#endif

  hr = jobStream->Close();
#ifdef PRINT_TICKET_SUPPORT
  hr = printTicketStream->Close();
#endif

  if (WaitForSingleObject(completionEvent, INFINITE) == WAIT_OBJECT_0) {
    XPS_JOB_STATUS jobStatus;
    hr = job->GetJobStatus(&jobStatus);

    switch (jobStatus.completion) {
    case XPS_JOB_COMPLETED:
      hr = S_OK;
      break;
    case XPS_JOB_CANCELLED:
      hr = E_FAIL;
      break;
    case XPS_JOB_FAILED:
      hr = E_FAIL;
      break;
    default:
      hr = E_UNEXPECTED;
      break;
    }

    CloseHandle(completionEvent);
  } else {
    hr = HRESULT_FROM_WIN32(GetLastError());
  }

  m_pTextRenderer->DiscardResources();
  m_pTextRenderer->DeleteTemporaryFiles();
  m_pTextRenderer = nullptr;
}

void touchmind::print::XPSPrint::_CreateCanvas() {
  // create a package
  THROW_IF_FAILED(m_pXpsFactory->CreatePackage(&m_pXpsPackage));

  // create a document sequence
  CComPtr<IOpcPartUri> opcPartUri = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreatePartUri(L"/FixedDocumentSequence.fdseq",
                                               &opcPartUri));

  CComPtr<IXpsOMDocumentSequence> xpsFDS = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreateDocumentSequence(opcPartUri, &xpsFDS));

  // create a document
  opcPartUri = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreatePartUri(
                      L"/Documents/1/FixedDocument.fdoc", &opcPartUri));

  CComPtr<IXpsOMDocument> xpsFD = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreateDocument(opcPartUri, &xpsFD));

  // create a page reference
  CComPtr<IXpsOMPageReference> xpsPageRef = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreatePageReference(&m_pageSize, &xpsPageRef));

  // create a page
  opcPartUri = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreatePartUri(
                                  L"/Documents/1/Pages/1.fpage", &opcPartUri));

  CComPtr<IXpsOMPage> xpsPage = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreatePage(&m_pageSize, nullptr,
                                                     opcPartUri, &xpsPage));
  opcPartUri = nullptr;

  THROW_IF_FAILED(m_pXpsPackage->SetDocumentSequence(xpsFDS));

  CComPtr<IXpsOMDocumentCollection> fixedDocuments = nullptr;
  THROW_IF_FAILED(xpsFDS->GetDocuments(&fixedDocuments));
  THROW_IF_FAILED(fixedDocuments->Append(xpsFD));

  CComPtr<IXpsOMPageReferenceCollection> pageRefs = nullptr;
  THROW_IF_FAILED(xpsFD->GetPageReferences(&pageRefs));
  THROW_IF_FAILED(pageRefs->Append(xpsPageRef));
  THROW_IF_FAILED(xpsPageRef->SetPage(xpsPage));
  THROW_IF_FAILED(xpsPage->SetLanguage(m_language.c_str()));

  CComPtr<IXpsOMVisualCollection> pageVisuals = nullptr;
  THROW_IF_FAILED(xpsPage->GetVisuals(&pageVisuals));
  THROW_IF_FAILED(m_pXpsFactory->CreateCanvas(&m_pXpsCanvas));
  THROW_IF_FAILED(pageVisuals->Append(m_pXpsCanvas));

  THROW_IF_FAILED(m_pXpsFactory->CreatePartResources(&m_pXpsResources));

  // Create dictionary and add to canvas
  THROW_IF_FAILED(m_pXpsFactory->CreateDictionary(&m_pXpsDictionary));
  THROW_IF_FAILED(m_pXpsCanvas->SetDictionaryLocal(m_pXpsDictionary));

  CComPtr<IXpsOMMatrixTransform> pMatrixTransform = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreateMatrixTransform(
                                        &m_xpsMatrix, &pMatrixTransform));
  m_pXpsCanvas->SetTransformLocal(pMatrixTransform);
}

void touchmind::print::XPSPrint::_PrintNodes(const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  PrintChildrenLinksVisitor linksVisitor(this);
  node->ApplyVisitor(linksVisitor);

  PrintNodeVisitor printNodeVisitor(this);
  node->ApplyVisitor(printNodeVisitor);
}

void touchmind::print::XPSPrint::_PrintNode(const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  CComPtr<IXpsOMVisualCollection> canvasVisuals = nullptr;
  THROW_IF_FAILED(m_pXpsCanvas->GetVisuals(&canvasVisuals));

  CComPtr<IXpsOMSolidColorBrush> fillBrush = nullptr;
  COLORREF bgColor = touchmind::util::ColorUtil::ToColorref(node->GetBackgroundColor());
  XPS_COLOR bodyColor;
  bodyColor.colorType = XPS_COLOR_TYPE_SRGB;
  bodyColor.value.sRGB.alpha = 0xff;
  bodyColor.value.sRGB.red = GetRValue(bgColor);
  bodyColor.value.sRGB.green = GetGValue(bgColor);
  bodyColor.value.sRGB.blue = GetBValue(bgColor);
  THROW_IF_FAILED(m_pXpsFactory->CreateSolidColorBrush(
                                 &bodyColor, nullptr, &fillBrush));

  CComPtr<IXpsOMSolidColorBrush> strokeBrush = nullptr;
  ZeroMemory(&bodyColor, sizeof(bodyColor));
  bodyColor.colorType = XPS_COLOR_TYPE_SRGB;
  bodyColor.value.sRGB.alpha = 0xff;
  bodyColor.value.sRGB.red = 0;
  bodyColor.value.sRGB.green = 0;
  bodyColor.value.sRGB.blue = 0;
  THROW_IF_FAILED(m_pXpsFactory->CreateSolidColorBrush(
                                   &bodyColor, nullptr, &strokeBrush));

  XPS_RECT rect = {node->GetX(), node->GetY(), node->GetWidth(), node->GetHeight()};
  CComPtr<IXpsOMGeometryFigure> xpsFigure = nullptr;
  if (node->GetNodeShape() == NODE_SHAPE_RECTANGLE) {
    XPSGeometryBuilder::CreateRectangleGeometry(m_pXpsFactory, rect, &xpsFigure);
  } else {
    XPSGeometryBuilder::CreateRoundedRectangleGeometry(m_pXpsFactory, rect, 10.0f, &xpsFigure);
  }

  CComPtr<IXpsOMGeometry> xpsGeometry = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreateGeometry(&xpsGeometry));

  CComPtr<IXpsOMGeometryFigureCollection> xpsFigureCollection = nullptr;
  THROW_IF_FAILED(xpsGeometry->GetFigures(&xpsFigureCollection));
  THROW_IF_FAILED(xpsFigureCollection->Append(xpsFigure));

  CComPtr<IXpsOMPath> path = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreatePath(&path));
  THROW_IF_FAILED(path->SetFillBrushLocal(fillBrush));
  THROW_IF_FAILED(path->SetStrokeBrushLocal(strokeBrush));
  THROW_IF_FAILED(path->SetGeometryLocal(xpsGeometry));
  THROW_IF_FAILED(canvasVisuals->Append(path));
}

void touchmind::print::XPSPrint::_PrintChildrenPaths(const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  PATH_DIRECTION linkDirection;
  touchmind::NODE_SIDE nodePosition = node->GetAncestorPosition();
  switch (nodePosition) {
  case touchmind::NODE_SIDE_RIGHT:
    linkDirection = PATH_DIRECTION_RIGHT;
    break;
  case touchmind::NODE_SIDE_LEFT:
    linkDirection = PATH_DIRECTION_LEFT;
    break;
  case touchmind::NODE_SIDE_UNDEFINED:
    linkDirection = PATH_DIRECTION_AUTO;
    break;
  default:
    linkDirection = PATH_DIRECTION_AUTO;
  }

  for (size_t i = 0; i < node->GetActualChildrenCount(); ++i) {
    std::shared_ptr<touchmind::model::node::NodeModel> child = node->GetChild(i);
    touchmind::model::CurvePoints curvePoints;
    view::GeometryBuilder::CalculatePath(node, child, linkDirection, touchmind::NODE_JUSTIFICATION_TOP,
                                         m_pConfiguration->GetLevelSeparation(), curvePoints);
    _PrintPath(child, curvePoints);
  }
}

void touchmind::print::XPSPrint::_PrintPath(const std::shared_ptr<touchmind::model::node::NodeModel> &child,
                                            const touchmind::model::CurvePoints &curvePoints) {
  auto path = child->GetPathModel();

  CComPtr<IXpsOMVisualCollection> canvasVisuals = nullptr;
  THROW_IF_FAILED(m_pXpsCanvas->GetVisuals(&canvasVisuals));

  COLORREF colorref = util::ColorUtil::ToColorref(path->GetColor());
  CComPtr<IXpsOMSolidColorBrush> strokeBrush = nullptr;
  XPS_COLOR bodyColor;
  bodyColor.colorType = XPS_COLOR_TYPE_SRGB;
  bodyColor.value.sRGB.alpha = 0xFF;
  bodyColor.value.sRGB.red = GetRValue(colorref);
  bodyColor.value.sRGB.green = GetGValue(colorref);
  bodyColor.value.sRGB.blue = GetBValue(colorref);
  THROW_IF_FAILED(m_pXpsFactory->CreateSolidColorBrush(
                                   &bodyColor, nullptr, &strokeBrush));

  CComPtr<IXpsOMGeometryFigure> xpsFigure = nullptr;
  XPSGeometryBuilder::CreateCurvePathGeometry(m_pXpsFactory, curvePoints, &xpsFigure);

  CComPtr<IXpsOMGeometry> xpsGeometry = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreateGeometry(&xpsGeometry));

  CComPtr<IXpsOMGeometryFigureCollection> xpsFigureCollection = nullptr;
  THROW_IF_FAILED(xpsGeometry->GetFigures(&xpsFigureCollection));
  THROW_IF_FAILED(xpsFigureCollection->Append(xpsFigure));

  CComPtr<IXpsOMPath> xpsPath = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreatePath(&xpsPath));
  THROW_IF_FAILED(xpsPath->SetStrokeBrushLocal(strokeBrush));
  THROW_IF_FAILED(xpsPath->SetGeometryLocal(xpsGeometry));
  THROW_IF_FAILED(xpsPath->SetStrokeThickness(path->GetWidthAsValue()));
  if (path->GetStyle() == LINE_STYLE_DOTTED) {
    xpsPath->SetStrokeDashCap(XPS_DASH_CAP_ROUND);
    xpsPath->SetStrokeDashOffset(0.25f);
    CComPtr<IXpsOMDashCollection> pDashCollection = nullptr;
    THROW_IF_FAILED(xpsPath->GetStrokeDashes(&pDashCollection));
    XPS_DASH dash;
    dash.length = 0.0f;
    dash.gap = 2.0f;
    THROW_IF_FAILED(pDashCollection->Append(&dash));
  } else if (path->GetStyle() == LINE_STYLE_DASHED) {
    xpsPath->SetStrokeDashCap(XPS_DASH_CAP_ROUND);
    xpsPath->SetStrokeDashOffset(1.0f);
    CComPtr<IXpsOMDashCollection> pDashCollection = nullptr;
    THROW_IF_FAILED(xpsPath->GetStrokeDashes(&pDashCollection));
    XPS_DASH dash;
    dash.length = 2.0f;
    dash.gap = 2.0f;
    THROW_IF_FAILED(pDashCollection->Append(&dash));
  }
  THROW_IF_FAILED(canvasVisuals->Append(xpsPath));
}

void touchmind::print::XPSPrint::_DiscardResources() {
  m_pXpsPackage = nullptr;
  m_pXpsCanvas = nullptr;
  m_pXpsDictionary = nullptr;
  m_pXpsResources = nullptr;

  if (m_pPageRanges != nullptr) {
    GlobalFree(m_pPageRanges);
  }
  if (m_pPDX->hDC != nullptr) {
    DeleteDC(m_pPDX->hDC);
  }
  if (m_pPDX->hDevMode != nullptr) {
    GlobalFree(m_pPDX->hDevMode);
  }
  if (m_pPDX->hDevNames != nullptr) {
    GlobalFree(m_pPDX->hDevNames);
  }
}

void touchmind::print::XPSPrint::_PrintLinks(const touchmind::model::MapModel *pMapModel) {
  for (size_t i = 0; i < pMapModel->GetNumberOfLinkModels(); ++i) {
    auto link = pMapModel->GetLinkModel(i);
    _PrintLink(link);
  }
}

void touchmind::print::XPSPrint::_PrintLink(const std::shared_ptr<touchmind::model::link::LinkModel> &link) {
  CComPtr<IXpsOMVisualCollection> canvasVisuals = nullptr;
  THROW_IF_FAILED(m_pXpsCanvas->GetVisuals(&canvasVisuals));

  COLORREF colorref = util::ColorUtil::ToColorref(link->GetLineColor());
  CComPtr<IXpsOMSolidColorBrush> strokeBrush = nullptr;
  XPS_COLOR bodyColor;
  bodyColor.colorType = XPS_COLOR_TYPE_SRGB;
  bodyColor.value.sRGB.alpha = 0xFF;
  bodyColor.value.sRGB.red = GetRValue(colorref);
  bodyColor.value.sRGB.green = GetGValue(colorref);
  bodyColor.value.sRGB.blue = GetBValue(colorref);
  THROW_IF_FAILED(m_pXpsFactory->CreateSolidColorBrush(
                                   &bodyColor, nullptr, &strokeBrush));

  model::CurvePoints curvePoints;
  view::GeometryBuilder::CalculateLink(link, curvePoints);

  CComPtr<IXpsOMGeometryFigure> xpsFigure = nullptr;
  XPSGeometryBuilder::CreateCurvePathGeometry(m_pXpsFactory, curvePoints, &xpsFigure);

  CComPtr<IXpsOMGeometry> xpsGeometry = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreateGeometry(&xpsGeometry));

  CComPtr<IXpsOMGeometryFigureCollection> xpsFigureCollection = nullptr;
  THROW_IF_FAILED(xpsGeometry->GetFigures(&xpsFigureCollection));
  THROW_IF_FAILED(xpsFigureCollection->Append(xpsFigure));

  CComPtr<IXpsOMPath> xpsPath = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreatePath(&xpsPath));
  THROW_IF_FAILED(xpsPath->SetStrokeBrushLocal(strokeBrush));
  THROW_IF_FAILED(xpsPath->SetGeometryLocal(xpsGeometry));
  THROW_IF_FAILED(xpsPath->SetStrokeThickness(link->GetLineWidthAsValue()));
  if (link->GetLineStyle() == LINE_STYLE_DOTTED) {
    xpsPath->SetStrokeDashCap(XPS_DASH_CAP_ROUND);
    xpsPath->SetStrokeDashOffset(0.25f);
    CComPtr<IXpsOMDashCollection> pDashCollection = nullptr;
    THROW_IF_FAILED(xpsPath->GetStrokeDashes(&pDashCollection));
    XPS_DASH dash;
    dash.length = 0.0f;
    dash.gap = 2.0f;
    THROW_IF_FAILED(pDashCollection->Append(&dash));
  } else if (link->GetLineStyle() == LINE_STYLE_DASHED) {
    xpsPath->SetStrokeDashCap(XPS_DASH_CAP_ROUND);
    xpsPath->SetStrokeDashOffset(1.0f);
    CComPtr<IXpsOMDashCollection> pDashCollection = nullptr;
    THROW_IF_FAILED(xpsPath->GetStrokeDashes(&pDashCollection));
    XPS_DASH dash;
    dash.length = 2.0f;
    dash.gap = 2.0f;
    THROW_IF_FAILED(pDashCollection->Append(&dash));
  }
  THROW_IF_FAILED(canvasVisuals->Append(xpsPath));

  _PrintLinkEdge(link, EDGE_ID_1);
  _PrintLinkEdge(link, EDGE_ID_2);
}

void touchmind::print::XPSPrint::_PrintLinkEdge(const std::shared_ptr<touchmind::model::link::LinkModel> &link,
                                                EDGE_ID edgeId) {
  auto linkEdge = link->GetEdge(edgeId);
  if (linkEdge->GetStyle() == EDGE_STYLE_NORMAL) {
    return;
  }
  FLOAT markerSize = linkEdge->GetMarkerSize();
  auto node = link->GetNode(edgeId);
  D2D1_POINT_2F point0 = link->GetEdgePoint(edgeId);
  NODE_SIDE nodeSide = link->GetNode(edgeId)->GetAncestorPosition();

  CComPtr<IXpsOMVisualCollection> canvasVisuals = nullptr;
  THROW_IF_FAILED(m_pXpsCanvas->GetVisuals(&canvasVisuals));

  COLORREF colorref = util::ColorUtil::ToColorref(link->GetLineColor());
  CComPtr<IXpsOMSolidColorBrush> strokeBrush = nullptr;
  XPS_COLOR bodyColor;
  bodyColor.colorType = XPS_COLOR_TYPE_SRGB;
  bodyColor.value.sRGB.alpha = 0xFF;
  bodyColor.value.sRGB.red = GetRValue(colorref);
  bodyColor.value.sRGB.green = GetGValue(colorref);
  bodyColor.value.sRGB.blue = GetBValue(colorref);
  THROW_IF_FAILED(m_pXpsFactory->CreateSolidColorBrush(
                                   &bodyColor, nullptr, &strokeBrush));

  CComPtr<IXpsOMGeometryFigure> xpsFigure = nullptr;
  if (linkEdge->GetStyle() == EDGE_STYLE_CIRCLE) {
    FLOAT shift = (nodeSide == NODE_SIDE_RIGHT ? markerSize / 2.0f : -markerSize / 2.0f);
    XPSGeometryBuilder::CreateCircleGeometry(m_pXpsFactory, D2D1::Point2F(point0.x + shift, point0.y),
                                             markerSize / 2.0f, &xpsFigure);
  } else {
    std::vector<D2D1_POINT_2F> points;
    view::GeometryBuilder::CalculateArrowPoints(point0, linkEdge->GetMarkerSize() + link->GetLineWidth(),
                                                linkEdge->GetMarkerSize() + link->GetLineWidth(), linkEdge->GetAngle(),
                                                node->GetAncestorPosition(), points);
    XPSGeometryBuilder::CreatePathGeometryFromPoints(m_pXpsFactory, points, &xpsFigure);
  }

  CComPtr<IXpsOMGeometry> xpsGeometry = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreateGeometry(&xpsGeometry));

  CComPtr<IXpsOMGeometryFigureCollection> xpsFigureCollection = nullptr;
  THROW_IF_FAILED(xpsGeometry->GetFigures(&xpsFigureCollection));
  THROW_IF_FAILED(xpsFigureCollection->Append(xpsFigure));

  CComPtr<IXpsOMPath> xpsPath = nullptr;
  THROW_IF_FAILED(m_pXpsFactory->CreatePath(&xpsPath));
  THROW_IF_FAILED(xpsPath->SetFillBrushLocal(strokeBrush));
  THROW_IF_FAILED(xpsPath->SetGeometryLocal(xpsGeometry));
  THROW_IF_FAILED(canvasVisuals->Append(xpsPath));
}
