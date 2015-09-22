#include "StdAfx.h"
#include "touchmind/Common.h"
#include "touchmind/Configuration.h"
#include "touchmind/converter/NodeModelXMLDecoder.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/model/MapModel.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"

const static _bstr_t s_tmm(L"tmm");
const static _bstr_t s_version(L"version");
const static _bstr_t s_node(L"node");
const static _bstr_t s_id(L"id");
const static _bstr_t s_position(L"position");
const static _variant_t v_positionLeftValue(L"left");
const static _bstr_t s_positionLeftValue(L"left");
const static _variant_t v_positionRightValue(L"right");
const static _bstr_t s_positionRightValue(L"right");
const static _bstr_t s_createdTime(L"createdTime");
const static _bstr_t s_modifiedTime(L"modifiedTime");
const static _bstr_t s_backgroundColor(L"backgroundColor");
const static _bstr_t s_text(L"text");
const static _bstr_t s_fontAttributes(L"fontAttributes");
const static _bstr_t s_fontAttribute(L"fontAttribute");
const static _bstr_t s_startPosition(L"startPosition");
const static _bstr_t s_length(L"length");
const static _bstr_t s_fontFamily(L"fontFammily");
const static _bstr_t s_fontSize(L"fontSize");
const static _bstr_t s_bold(L"bold");
const static _bstr_t s_italic(L"italic");
const static _bstr_t s_underline(L"underline");
const static _bstr_t s_strikethrough(L"strikethrough");
const static _variant_t v_boolTrueValue(L"true");
const static _bstr_t s_boolTrueValue(L"true");
const static _bstr_t s_foregroundColor(L"foregroundColor");
const static _bstr_t s_width(L"width");
const static _bstr_t s_height(L"height");
const static _bstr_t s_path(L"path");
const static _bstr_t s_color(L"color");
const static _bstr_t s_style(L"style");
const static _bstr_t s_startStyle(L"startStyle");
const static _bstr_t s_endStyle(L"endStyle");
const static _bstr_t s_link(L"link");
const static _bstr_t s_destination(L"destination");
const static _bstr_t s_startHandle(L"startHandle");
const static _bstr_t s_endHandle(L"endHandle");
const static _bstr_t s_shape(L"shape");

touchmind::converter::NodeModelXMLDecoder::NodeModelXMLDecoder()
    : m_pSelectionManager(nullptr)
    , m_pMapModel(nullptr) {
}

touchmind::converter::NodeModelXMLDecoder::~NodeModelXMLDecoder(void) {
}

HRESULT touchmind::converter::NodeModelXMLDecoder::Decode(
    IN MSXML::IXMLDOMNodePtr xmlNodeNode, OUT std::shared_ptr<touchmind::model::node::NodeModel> &node,
    OUT std::vector<std::shared_ptr<touchmind::model::link::LinkModel>> &links, IN bool keepOriginalId,
    IN bool keepOriginalPosition, std::shared_ptr<LinkInfoList> linkInfoList,
    std::shared_ptr<std::map<NODE_ID, NODE_ID>> idMap) {
  bool generateLinks = false;
  if (linkInfoList == nullptr) {
    generateLinks = true;
    linkInfoList = std::make_shared<LinkInfoList>();
  }
  if (idMap == nullptr) {
    idMap = std::make_shared<std::map<NODE_ID, NODE_ID>>();
  }

  HRESULT hr = S_OK;
  if (s_node == xmlNodeNode->baseName) {
    MSXML::IXMLDOMNamedNodeMapPtr pAttrMap = xmlNodeNode->attributes;

    // id
    MSXML::IXMLDOMNodePtr pIdNode = pAttrMap->getNamedItem(s_id);
    if (pIdNode != nullptr) {
      std::wstring ws_id(pIdNode->text);
      touchmind::NODE_ID nodeId;
      if (touchmind::StringToNodeId(ws_id, &nodeId) == 0) {
        if (keepOriginalId) {
          idMap->insert({nodeId, nodeId});
          node->SetId(nodeId);
        } else {
          idMap->insert({nodeId, node->GetId()});
        }
      }
    }

    // position
    if (keepOriginalPosition) {
      MSXML::IXMLDOMNodePtr pPositionNode = pAttrMap->getNamedItem(s_position);
      if (pPositionNode != nullptr) {
        if (pPositionNode->text == s_positionLeftValue) {
          node->SetPosition(NODE_SIDE_LEFT);
        } else if (pPositionNode->text == s_positionRightValue) {
          node->SetPosition(NODE_SIDE_RIGHT);
        }
      }
    }

    // created time
    MSXML::IXMLDOMNodePtr pCreatedTimeNode = pAttrMap->getNamedItem(s_createdTime);
    if (pCreatedTimeNode != nullptr) {
      std::wstring ws_createdTime(pCreatedTimeNode->text);
      SYSTEMTIME createdTime;
      touchmind::StringToSystemtime(ws_createdTime, &createdTime);
      node->SetCreatedTime(createdTime);
    }

    // modified time
    MSXML::IXMLDOMNodePtr pModifiedTimeNode = pAttrMap->getNamedItem(s_modifiedTime);
    if (pModifiedTimeNode != nullptr) {
      std::wstring ws_modifiedTime(pModifiedTimeNode->text);
      SYSTEMTIME modifiedTime;
      touchmind::StringToSystemtime(ws_modifiedTime, &modifiedTime);
      node->SetModifiedTime(modifiedTime);
    }

    // width
    MSXML::IXMLDOMNodePtr pWidthNode = pAttrMap->getNamedItem(s_width);
    if (pWidthNode != nullptr) {
      std::wstring ws_width(pWidthNode->text);
      FLOAT width;
      touchmind::StringToSize(ws_width, &width);
      node->SetWidth(width);
    }

    // height
    MSXML::IXMLDOMNodePtr pHeightNode = pAttrMap->getNamedItem(s_height);
    if (pHeightNode != nullptr) {
      std::wstring ws_height(pHeightNode->text);
      FLOAT height;
      touchmind::StringToSize(ws_height, &height);
      node->SetHeight(height);
    }

    // background color
    MSXML::IXMLDOMNodePtr pBackgroundColorNode = pAttrMap->getNamedItem(s_backgroundColor);
    if (pBackgroundColorNode != nullptr) {
      std::wstring ws_backgroundColor(pBackgroundColorNode->text);
      COLORREF backgroundColor;
      touchmind::StringToColorref(ws_backgroundColor, &backgroundColor);
      node->SetBackgroundColor(util::ColorUtil::ToColorF(backgroundColor));
    }

    // shape
    MSXML::IXMLDOMNodePtr pShapeNode = pAttrMap->getNamedItem(s_shape);
    if (pShapeNode != nullptr) {
      std::wstring ws_shape(pShapeNode->text);
      NODE_SHAPE nodeShape = prop::NodeShape::ToNodeShape(ws_shape);
      node->SetNodeShape(nodeShape);
    }

    MSXML::IXMLDOMNodeListPtr pNodeList = xmlNodeNode->childNodes;
    for (int i = 0; i < pNodeList->length; ++i) {
      MSXML::IXMLDOMNodePtr pChildNode = pNodeList->item[i];
      if (pChildNode->baseName == s_text) {
        // text
        hr = _DecodeText(pChildNode, node);
        if (FAILED(hr)) {
          break;
        }
      } else if (pChildNode->baseName == s_path) {
        // path
        hr = _DecodePath(pChildNode, node);
        if (FAILED(hr)) {
          break;
        }
      } else if (pChildNode->baseName == s_link) {
        // link
        hr = _DecodeLink(pChildNode, node, linkInfoList);
        if (FAILED(hr)) {
          break;
        }
      } else if (pChildNode->baseName == s_node) {
        // child node
        auto pChildNodeModel = touchmind::model::node::NodeModel::Create(m_pSelectionManager);
        hr = Decode(pChildNode, pChildNodeModel, links, keepOriginalId, keepOriginalPosition, linkInfoList, idMap);
        node->AddChild(pChildNodeModel);
      }
    }
  }

  if (generateLinks) {
    for (size_t i = 0; i < linkInfoList->size(); ++i) {
      LinkInfo &linkInfo = linkInfoList->at(i);
      if (idMap->count(linkInfo.end) > 0) {
        auto node1 = node->FindByNodeId(linkInfo.start);
        auto node2 = node->FindByNodeId(idMap->at(linkInfo.end));
        if (node1 != nullptr && node2 != nullptr && linkInfo.link != nullptr) {
          linkInfo.link->SetNode(EDGE_ID_1, node1);
          linkInfo.link->SetNode(EDGE_ID_2, node2);
          links.push_back(linkInfo.link);
        }
      }
    }
  }
  return hr;
}

HRESULT
touchmind::converter::NodeModelXMLDecoder::_DecodeLink(IN MSXML::IXMLDOMNodePtr xmlLinkNode,
                                                       OUT std::shared_ptr<touchmind::model::node::NodeModel> &node,
                                                       std::shared_ptr<LinkInfoList> linkInfoList) {
  MSXML::IXMLDOMNamedNodeMapPtr pAttrMap = xmlLinkNode->attributes;
  LinkInfo linkInfo = {-1, -1, nullptr};
  linkInfo.start = node->GetId();

  linkInfo.link = touchmind::model::link::LinkModel::Create(m_pSelectionManager);
  linkInfo.link->SetLinkId(touchmind::model::link::LinkModel::GenerateLinkId());

  // destination
  MSXML::IXMLDOMNodePtr pDestinationNode = pAttrMap->getNamedItem(s_destination);
  if (pDestinationNode != nullptr) {
    std::wstring ws_id(pDestinationNode->text);
    touchmind::NODE_ID nodeId;
    if (touchmind::StringToNodeId(ws_id, &nodeId) == 0) {
      linkInfo.end = nodeId;
    }
  }

  // link width
  MSXML::IXMLDOMNodePtr pLinkWidthNode = pAttrMap->getNamedItem(s_width);
  if (pLinkWidthNode != nullptr) {
    std::wstring ws_linkWidth(pLinkWidthNode->text);
    LINE_WIDTH lineWidth = prop::LineWidth::ToLineWidth(ws_linkWidth);
    linkInfo.link->SetLineWidth(lineWidth);
  }

  // link color
  MSXML::IXMLDOMNodePtr pLinkColorNode = pAttrMap->getNamedItem(s_color);
  if (pLinkColorNode != nullptr) {
    std::wstring ws_linkColor(pLinkColorNode->text);
    D2D1_COLOR_F colorf;
    StringToColorF(ws_linkColor, &colorf);
    linkInfo.link->SetLineColor(colorf);
  }

  // link style
  MSXML::IXMLDOMNodePtr pLinkStyleNode = pAttrMap->getNamedItem(s_style);
  if (pLinkStyleNode != nullptr) {
    std::wstring ws_linkStyle(pLinkStyleNode->text);
    LINE_STYLE lineStyle = prop::LineStyle::ToLineStyle(ws_linkStyle);
    linkInfo.link->SetLineStyle(lineStyle);
  }

  // link edge style
  EDGE_STYLE edgeStyle1 = EDGE_STYLE_NORMAL;
  MSXML::IXMLDOMNodePtr pLinkEdgeStyle1Node = pAttrMap->getNamedItem(s_startStyle);
  if (pLinkEdgeStyle1Node != nullptr) {
    std::wstring ws_linkEdgeStyle1(pLinkEdgeStyle1Node->text);
    edgeStyle1 = prop::LineEdgeStyle::ToEdgeStyle(ws_linkEdgeStyle1);
  }
  EDGE_STYLE edgeStyle2 = EDGE_STYLE_NORMAL;
  MSXML::IXMLDOMNodePtr pLinkEdgeStyle2Node = pAttrMap->getNamedItem(s_endStyle);
  if (pLinkEdgeStyle2Node != nullptr) {
    std::wstring ws_linkEdgeStyle2(pLinkEdgeStyle2Node->text);
    edgeStyle2 = prop::LineEdgeStyle::ToEdgeStyle(ws_linkEdgeStyle2);
  }
  LINE_EDGE_STYLE lineEdgeStyle = prop::LineEdgeStyle::EdgeStylesToLineEdgeStyle(edgeStyle1, edgeStyle2);
  linkInfo.link->SetLineEdgeStyle(lineEdgeStyle);

  // handle 1
  MSXML::IXMLDOMNodePtr pHandle1Node = pAttrMap->getNamedItem(s_startHandle);
  if (pHandle1Node != nullptr) {
    std::wstring ws_handle1(pHandle1Node->text);
    FLOAT angle, length;
    StringToHandle(ws_handle1, angle, length);
    linkInfo.link->GetEdge(EDGE_ID_1)->SetAngle(angle);
    linkInfo.link->GetEdge(EDGE_ID_1)->SetLength(length);
  }

  // handle 2
  MSXML::IXMLDOMNodePtr pHandle2Node = pAttrMap->getNamedItem(s_endHandle);
  if (pHandle2Node != nullptr) {
    std::wstring ws_handle2(pHandle2Node->text);
    FLOAT angle, length;
    StringToHandle(ws_handle2, angle, length);
    linkInfo.link->GetEdge(EDGE_ID_2)->SetAngle(angle);
    linkInfo.link->GetEdge(EDGE_ID_2)->SetLength(length);
  }

  linkInfoList->push_back(linkInfo);
  return S_OK;
}

HRESULT
touchmind::converter::NodeModelXMLDecoder::_DecodePath(IN MSXML::IXMLDOMNodePtr xmlPathNode,
                                                       OUT std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  auto path = node->GetPathModel();
  MSXML::IXMLDOMNamedNodeMapPtr pAttrMap = xmlPathNode->attributes;

  // path width
  MSXML::IXMLDOMNodePtr pWidthNode = pAttrMap->getNamedItem(s_width);
  if (pWidthNode != nullptr) {
    std::wstring ws_width(pWidthNode->text);
    LINE_WIDTH lineWidth = prop::LineWidth::ToLineWidth(ws_width);
    path->SetWidth(lineWidth);
  }

  // path color
  MSXML::IXMLDOMNodePtr pPathColor = pAttrMap->getNamedItem(s_color);
  if (pPathColor != nullptr) {
    std::wstring ws_pathColor(pPathColor->text);
    D2D1_COLOR_F pathColor;
    touchmind::StringToColorF(ws_pathColor, &pathColor);
    path->SetColor(pathColor);
  }

  // path style
  MSXML::IXMLDOMNodePtr pPathStyle = pAttrMap->getNamedItem(s_style);
  if (pPathStyle != nullptr) {
    std::wstring ws_pathStyle(pPathStyle->text);
    LINE_STYLE lineStyle = prop::LineStyle::ToLineStyle(ws_pathStyle);
    path->SetStyle(lineStyle);
  }

  return S_OK;
}

HRESULT
touchmind::converter::NodeModelXMLDecoder::_DecodeText(IN MSXML::IXMLDOMNodePtr pTextNode,
                                                       OUT std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  HRESULT hr = S_OK;
  std::wstring text(pTextNode->text);
  node->SetText(text);

  MSXML::IXMLDOMNodeListPtr pChildNodeList = pTextNode->childNodes;
  for (int i = 0; i < pChildNodeList->length && SUCCEEDED(hr); ++i) {
    MSXML::IXMLDOMNodePtr pChildNode = pChildNodeList->item[i];
    if (pChildNode->baseName == s_fontAttributes) {
      MSXML::IXMLDOMNodeListPtr pFontAttributeNodeList = pChildNode->childNodes;
      for (int j = 0; j < pFontAttributeNodeList->length; ++j) {
        MSXML::IXMLDOMNodePtr pFontAttributeNode = pFontAttributeNodeList->item[j];
        if (pFontAttributeNode->baseName == s_fontAttribute) {
          hr = _DecodeFontAttribute(pFontAttributeNode, node);
          if (FAILED(hr)) {
            break;
          }
        }
      }
    }
  }
  return hr;
}

HRESULT touchmind::converter::NodeModelXMLDecoder::_DecodeFontAttribute(
    IN MSXML::IXMLDOMNodePtr pFontAttributeNode, OUT std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  HRESULT hr = S_OK;

  touchmind::text::FontAttribute fontAttribute;
  MSXML::IXMLDOMNamedNodeMapPtr pAttrMap = pFontAttributeNode->attributes;

  // start position
  MSXML::IXMLDOMNodePtr pStartPositionNode = pAttrMap->getNamedItem(s_startPosition);
  if (pStartPositionNode != nullptr) {
    std::wstring ws_startPosition(pStartPositionNode->text);
    LONG startPosition = _wtol(ws_startPosition.c_str());
    fontAttribute.startPosition = startPosition;
  }

  // length
  MSXML::IXMLDOMNodePtr pLengthNode = pAttrMap->getNamedItem(s_length);
  if (pLengthNode != nullptr) {
    std::wstring ws_length(pLengthNode->text);
    LONG length = _wtol(ws_length.c_str());
    fontAttribute.length = length;
  }

  // font family
  MSXML::IXMLDOMNodePtr pFontFamilyNode = pAttrMap->getNamedItem(s_fontFamily);
  if (pFontFamilyNode != nullptr) {
    std::wstring ws_fontFamily(pFontFamilyNode->text);
    fontAttribute.fontFamilyName = ws_fontFamily;
  }

  // font size
  MSXML::IXMLDOMNodePtr pFontSizeNode = pAttrMap->getNamedItem(s_fontSize);
  if (pFontSizeNode != nullptr) {
    std::wstring ws_fontSize(pFontSizeNode->text);
    FLOAT fontSize;
    touchmind::StringToFontSize(ws_fontSize, &fontSize);
    fontAttribute.fontSize = fontSize;
  }

  // bold
  MSXML::IXMLDOMNodePtr pBoldNode = pAttrMap->getNamedItem(s_bold);
  if (pBoldNode != nullptr) {
    fontAttribute.bold = (pBoldNode->text == s_boolTrueValue);
  }

  // italic
  MSXML::IXMLDOMNodePtr pItalicNode = pAttrMap->getNamedItem(s_italic);
  if (pItalicNode != nullptr) {
    fontAttribute.italic = (pItalicNode->text == s_boolTrueValue);
  }

  // underline
  MSXML::IXMLDOMNodePtr pUnderlineNode = pAttrMap->getNamedItem(s_underline);
  if (pUnderlineNode != nullptr) {
    fontAttribute.underline = (pUnderlineNode->text == s_boolTrueValue);
  }

  // strikethrough
  MSXML::IXMLDOMNodePtr pStrikethroughNode = pAttrMap->getNamedItem(s_strikethrough);
  if (pStrikethroughNode != nullptr) {
    fontAttribute.strikethrough = (pStrikethroughNode->text == s_boolTrueValue);
  }

  // foreground color
  MSXML::IXMLDOMNodePtr pForegroundColorNode = pAttrMap->getNamedItem(s_foregroundColor);
  if (pForegroundColorNode != nullptr) {
    std::wstring ws_foregroundColor(pForegroundColorNode->text);
    COLORREF foregroundColorref;
    touchmind::StringToColorref(ws_foregroundColor, &foregroundColorref);
    fontAttribute.foregroundColor = foregroundColorref;
  }

  node->AddFontAttribute(fontAttribute);
  return hr;
}
