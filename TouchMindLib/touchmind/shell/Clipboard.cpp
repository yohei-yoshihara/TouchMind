#include "StdAfx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/shell/Clipboard.h"
#include "touchmind/Configuration.h"
#include "touchmind/converter/NodeModelToTextConverter.h"
#include "touchmind/converter/NodeModelXMLDecoder.h"
#include "touchmind/converter/NodeModelXMLEncoder.h"

const wchar_t *touchmind::shell::Clipboard::CLIPBOARD_FORMAT_NAME = _CLIPBOARD_XML_ID;
const size_t touchmind::shell::Clipboard::NUMBER_OF_CLIPBOARD_FORMATS = _NUMBER_OF_CLIPBOARD_FORMATS;

touchmind::shell::Clipboard::Clipboard()
    : m_pSelectionManager(nullptr)
    , m_touchMindClipboardFormatId(-1)
    , m_pFormatPriorityList() {
}

touchmind::shell::Clipboard::~Clipboard(void) {
}

bool touchmind::shell::Clipboard::_CopyTEXT(HWND hWnd, const std::wstring &text) {
  UNREFERENCED_PARAMETER(hWnd);

  HGLOBAL hg = GlobalAlloc(GHND | GMEM_SHARE, (text.length() + 1) * sizeof(wchar_t));
  wchar_t *strMem = (wchar_t *)GlobalLock(hg);
  wcscpy_s(strMem, text.length() + 1, text.c_str());
  GlobalUnlock(hg);

  SetClipboardData(CF_UNICODETEXT, hg);

  return true;
}

bool touchmind::shell::Clipboard::_PasteTEXT(HWND hWnd, std::wstring &text) {
  if (!OpenClipboard(hWnd)) {
    text.clear();
    return false;
  }
  HGLOBAL hg = GetClipboardData(CF_UNICODETEXT);
  if (hg == nullptr) {
    text.clear();
    return false;
  }
  wchar_t *strClip = static_cast<wchar_t *>(GlobalLock(hg));
  text.assign(strClip);
  GlobalUnlock(hg);
  CloseClipboard();
  return true;
}

HRESULT touchmind::shell::Clipboard::_InitializeClipboard() {
  HRESULT hr = S_OK;
  if (m_touchMindClipboardFormatId < 0) {
    m_touchMindClipboardFormatId = RegisterClipboardFormat(CLIPBOARD_FORMAT_NAME);
    hr = m_touchMindClipboardFormatId == 0 ? E_FAIL : S_OK;
    if (SUCCEEDED(hr)) {
      m_pFormatPriorityList[0] = m_touchMindClipboardFormatId;
      m_pFormatPriorityList[1] = CF_UNICODETEXT;
    }
  }
  return hr;
}

bool touchmind::shell::Clipboard::_CopyXML(HWND hWnd, const std::wstring &xml) {
  UNREFERENCED_PARAMETER(hWnd);
  HRESULT hr = _InitializeClipboard();
  if (SUCCEEDED(hr)) {

    HGLOBAL hg = GlobalAlloc(GHND | GMEM_SHARE, (xml.length() + 1) * sizeof(wchar_t));
    wchar_t *strMem = (wchar_t *)GlobalLock(hg);
    wcscpy_s(strMem, xml.length() + 1, xml.c_str());
    GlobalUnlock(hg);

    SetClipboardData(m_touchMindClipboardFormatId, hg);
  }
  return SUCCEEDED(hr);
}

bool touchmind::shell::Clipboard::_PasteXML(HWND hWnd, std::wstring &xml) {
  HRESULT hr = _InitializeClipboard();
  if (SUCCEEDED(hr)) {
    if (!OpenClipboard(hWnd)) {
      xml.clear();
      return false;
    }
    HGLOBAL hg = GetClipboardData(m_touchMindClipboardFormatId);
    if (hg == nullptr) {
      xml.clear();
      return false;
    }
    wchar_t *strClip = static_cast<wchar_t *>(GlobalLock(hg));
    xml.assign(strClip);
    GlobalUnlock(hg);
    CloseClipboard();
    return true;
  }
  return SUCCEEDED(hr);
}

HRESULT touchmind::shell::Clipboard::CopyNodeModel(IN HWND hWnd,
                                                   IN const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  HRESULT hr = _InitializeClipboard();
  if (SUCCEEDED(hr)) {
    if (!::OpenClipboard(hWnd)) {
      return E_FAIL;
    }
    ::EmptyClipboard();
    // customized xml format
    MSXML::IXMLDOMDocumentPtr pXMLDoc;
    hr = pXMLDoc.CreateInstance(__uuidof(MSXML::DOMDocument60), nullptr, CLSCTX_INPROC_SERVER);
    _bstr_t s_node(L"node");
    MSXML::IXMLDOMElementPtr pElement = pXMLDoc->createElement(s_node);
    pXMLDoc->appendChild(pElement);
    m_pNodeModelXMLEncoder->Encode(node, pXMLDoc, pElement);
    std::wstring xml(pXMLDoc->xml);
    _CopyXML(hWnd, xml);

    // text format
    touchmind::converter::NodeModelToTextConverter textConverter;
    std::wstring text;
    textConverter.ToText(node, text);
    _CopyTEXT(hWnd, text);

    ::CloseClipboard();
  }
  return hr;
}

HRESULT touchmind::shell::Clipboard::PasteNodeModel(IN HWND hWnd,
                                                    OUT std::shared_ptr<touchmind::model::node::NodeModel> &node,
                                                    OUT std::vector<std::shared_ptr<model::link::LinkModel>> &links) {
  HRESULT hr = _InitializeClipboard();
  if (SUCCEEDED(hr)) {
    int formatId
        = GetPriorityClipboardFormat(m_pFormatPriorityList.data(), static_cast<int>(m_pFormatPriorityList.size()));
    if (formatId >= 0) {
      if (formatId == m_touchMindClipboardFormatId) {
        std::wstring s_xml;
        _PasteXML(hWnd, s_xml);
        LOG(SEVERITY_LEVEL_DEBUG) << s_xml;
        MSXML::IXMLDOMDocumentPtr pXMLDoc;
        hr = pXMLDoc.CreateInstance(__uuidof(MSXML::DOMDocument60), nullptr, CLSCTX_INPROC_SERVER);
        _bstr_t xml(s_xml.c_str());
        pXMLDoc->loadXML(xml);
        MSXML::IXMLDOMElementPtr pElement = pXMLDoc->firstChild;
        m_pNodeModelXMLDecoder->Decode(pElement, node, links, false, false);
      } else if (formatId == CF_UNICODETEXT) {
        std::wstring text;
        _PasteTEXT(hWnd, text);
        touchmind::converter::NodeModelToTextConverter textConverter;
        textConverter.SetSelectionManager(m_pSelectionManager);
        node = textConverter.FromText(text);
      }
    }
  }
  return hr;
}
