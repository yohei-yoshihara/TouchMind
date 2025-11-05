#include "StdAfx.h"
#include "touchmind/Common.h"
#include "touchmind/Configuration.h"
#include "touchmind/converter/NodeModelXMLDecoder.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/model/MapModel.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"

const static std::wstring s_tmm(L"tmm");
const static std::wstring s_version(L"version");
const static std::wstring s_node(L"node");
const static std::wstring s_id(L"id");
const static std::wstring s_position(L"position");
const static std::wstring v_positionLeftValue(L"left");
const static std::wstring s_positionLeftValue(L"left");
const static std::wstring v_positionRightValue(L"right");
const static std::wstring s_positionRightValue(L"right");
const static std::wstring s_createdTime(L"createdTime");
const static std::wstring s_modifiedTime(L"modifiedTime");
const static std::wstring s_backgroundColor(L"backgroundColor");
const static std::wstring s_text(L"text");
const static std::wstring s_fontAttributes(L"fontAttributes");
const static std::wstring s_fontAttribute(L"fontAttribute");
const static std::wstring s_startPosition(L"startPosition");
const static std::wstring s_length(L"length");
const static std::wstring s_fontFamily(L"fontFammily");
const static std::wstring s_fontSize(L"fontSize");
const static std::wstring s_bold(L"bold");
const static std::wstring s_italic(L"italic");
const static std::wstring s_underline(L"underline");
const static std::wstring s_strikethrough(L"strikethrough");
const static std::wstring v_boolTrueValue(L"true");
const static std::wstring s_boolTrueValue(L"true");
const static std::wstring s_foregroundColor(L"foregroundColor");
const static std::wstring s_width(L"width");
const static std::wstring s_height(L"height");
const static std::wstring s_path(L"path");
const static std::wstring s_color(L"color");
const static std::wstring s_style(L"style");
const static std::wstring s_startStyle(L"startStyle");
const static std::wstring s_endStyle(L"endStyle");
const static std::wstring s_link(L"link");
const static std::wstring s_destination(L"destination");
const static std::wstring s_startHandle(L"startHandle");
const static std::wstring s_endHandle(L"endHandle");
const static std::wstring s_shape(L"shape");

touchmind::converter::NodeModelXMLDecoder::NodeModelXMLDecoder()
    : m_pSelectionManager(nullptr)
    , m_pMapModel(nullptr) {
}

touchmind::converter::NodeModelXMLDecoder::~NodeModelXMLDecoder(void) {
}

HRESULT touchmind::converter::NodeModelXMLDecoder::Decode(
    IN pugi::xml_node &xmlNodeNode,
    OUT std::shared_ptr<touchmind::model::node::NodeModel> &node,
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
  if (s_node == xmlNodeNode.name()) {
    // id
    auto idAttr = xmlNodeNode.attribute(s_id);
    if (idAttr != nullptr) {
      std::wstring ws_id(idAttr.value());
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
      auto positionAttr = xmlNodeNode.attribute(s_position);
      if (positionAttr != nullptr) {
        if (positionAttr.value() == s_positionLeftValue) {
          node->SetPosition(NODE_SIDE_LEFT);
        } else if (positionAttr.value() == s_positionRightValue) {
          node->SetPosition(NODE_SIDE_RIGHT);
        }
      }
    }

    // created time
    auto createdTimeAttr = xmlNodeNode.attribute(s_createdTime);
    if (createdTimeAttr != nullptr) {
      std::wstring ws_createdTime(createdTimeAttr.value());
      SYSTEMTIME createdTime;
      touchmind::StringToSystemtime(ws_createdTime, &createdTime);
      node->SetCreatedTime(createdTime);
    }

    // modified time
    auto modifiedTimeAttr = xmlNodeNode.attribute(s_modifiedTime);
    if (modifiedTimeAttr != nullptr) {
      std::wstring ws_modifiedTime(modifiedTimeAttr.value());
      SYSTEMTIME modifiedTime;
      touchmind::StringToSystemtime(ws_modifiedTime, &modifiedTime);
      node->SetModifiedTime(modifiedTime);
    }

    // width
    auto widthAttr = xmlNodeNode.attribute(s_width);
    if (widthAttr != nullptr) {
      std::wstring ws_width(widthAttr.value());
      FLOAT width;
      touchmind::StringToSize(ws_width, &width);
      node->SetWidth(width);
    }

    // height
    auto heightAttr = xmlNodeNode.attribute(s_height);
    if (heightAttr != nullptr) {
      std::wstring ws_height(heightAttr.value());
      FLOAT height;
      touchmind::StringToSize(ws_height, &height);
      node->SetHeight(height);
    }

    // background color
    auto backgroundColorAttr = xmlNodeNode.attribute(s_backgroundColor);
    if (backgroundColorAttr != nullptr) {
      std::wstring ws_backgroundColor(backgroundColorAttr.value());
      COLORREF backgroundColor;
      touchmind::StringToColorref(ws_backgroundColor, &backgroundColor);
      node->SetBackgroundColor(util::ColorUtil::ToColorF(backgroundColor));
    }

    // shape
    auto shapeAttr = xmlNodeNode.attribute(s_shape);
    if (shapeAttr != nullptr) {
      std::wstring ws_shape(shapeAttr.value());
      NODE_SHAPE nodeShape = prop::NodeShape::ToNodeShape(ws_shape);
      node->SetNodeShape(nodeShape);
    }

    for (auto child : xmlNodeNode.children()) {
      if (child.name() == s_text) {
        // text
        hr = _DecodeText(child, node);
        if (FAILED(hr)) {
          break;
        }
      } else if (child.name() == s_path) {
        // path
        hr = _DecodePath(child, node);
        if (FAILED(hr)) {
          break;
        }
      } else if (child.name() == s_link) {
        // link
        hr = _DecodeLink(child, node, linkInfoList);
        if (FAILED(hr)) {
          break;
        }
      } else if (child.name() == s_node) {
        // child node
        auto pChildNodeModel = touchmind::model::node::NodeModel::Create(m_pSelectionManager);
        hr = Decode(child, pChildNodeModel, links, keepOriginalId, keepOriginalPosition, linkInfoList, idMap);
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
touchmind::converter::NodeModelXMLDecoder::_DecodeLink(IN pugi::xml_node &xmlLinkNode,
                                                       OUT std::shared_ptr<touchmind::model::node::NodeModel> &node,
                                                       std::shared_ptr<LinkInfoList> linkInfoList) {
  LinkInfo linkInfo = {-1, -1, nullptr};
  linkInfo.start = node->GetId();

  linkInfo.link = touchmind::model::link::LinkModel::Create(m_pSelectionManager);
  linkInfo.link->SetLinkId(touchmind::model::link::LinkModel::GenerateLinkId());

  // destination
  auto destinationAttr = xmlLinkNode.attribute(s_destination);
  if (destinationAttr != nullptr) {
    std::wstring ws_id(destinationAttr.value());
    touchmind::NODE_ID nodeId;
    if (touchmind::StringToNodeId(ws_id, &nodeId) == 0) {
      linkInfo.end = nodeId;
    }
  }

  // link width
  auto linkWidthAttr = xmlLinkNode.attribute(s_width);
  if (linkWidthAttr != nullptr) {
    std::wstring ws_linkWidth(linkWidthAttr.value());
    LINE_WIDTH lineWidth = prop::LineWidth::ToLineWidth(ws_linkWidth);
    linkInfo.link->SetLineWidth(lineWidth);
  }

  // link color
  auto linkColorAttr = xmlLinkNode.attribute(s_color);
  if (linkColorAttr != nullptr) {
    std::wstring ws_linkColor(linkColorAttr.value());
    D2D1_COLOR_F colorf;
    StringToColorF(ws_linkColor, &colorf);
    linkInfo.link->SetLineColor(colorf);
  }

  // link style
  auto linkStyleAttr = xmlLinkNode.attribute(s_style);
  if (linkStyleAttr != nullptr) {
    std::wstring ws_linkStyle(linkStyleAttr.value());
    LINE_STYLE lineStyle = prop::LineStyle::ToLineStyle(ws_linkStyle);
    linkInfo.link->SetLineStyle(lineStyle);
  }

  // link edge style
  EDGE_STYLE edgeStyle1 = EDGE_STYLE_NORMAL;
  auto linkEdgeStyle1Attr = xmlLinkNode.attribute(s_startStyle);
  if (linkEdgeStyle1Attr != nullptr) {
    std::wstring ws_linkEdgeStyle1(linkEdgeStyle1Attr.value());
    edgeStyle1 = prop::LineEdgeStyle::ToEdgeStyle(ws_linkEdgeStyle1);
  }
  EDGE_STYLE edgeStyle2 = EDGE_STYLE_NORMAL;
  auto linkEdgeStyle2Attr = xmlLinkNode.attribute(s_endStyle);
  if (linkEdgeStyle2Attr != nullptr) {
    std::wstring ws_linkEdgeStyle2(linkEdgeStyle2Attr.value());
    edgeStyle2 = prop::LineEdgeStyle::ToEdgeStyle(ws_linkEdgeStyle2);
  }
  LINE_EDGE_STYLE lineEdgeStyle = prop::LineEdgeStyle::EdgeStylesToLineEdgeStyle(edgeStyle1, edgeStyle2);
  linkInfo.link->SetLineEdgeStyle(lineEdgeStyle);

  // handle 1
  auto handle1Attr = xmlLinkNode.attribute(s_startHandle);
  if (handle1Attr != nullptr) {
    std::wstring ws_handle1(handle1Attr.value());
    FLOAT angle, length;
    StringToHandle(ws_handle1, angle, length);
    linkInfo.link->GetEdge(EDGE_ID_1)->SetAngle(angle);
    linkInfo.link->GetEdge(EDGE_ID_1)->SetLength(length);
  }

  // handle 2
  auto handle2Attr = xmlLinkNode.attribute(s_endHandle);
  if (handle2Attr != nullptr) {
    std::wstring ws_handle2(handle2Attr.value());
    FLOAT angle, length;
    StringToHandle(ws_handle2, angle, length);
    linkInfo.link->GetEdge(EDGE_ID_2)->SetAngle(angle);
    linkInfo.link->GetEdge(EDGE_ID_2)->SetLength(length);
  }

  linkInfoList->push_back(linkInfo);
  return S_OK;
}

HRESULT
touchmind::converter::NodeModelXMLDecoder::_DecodePath(IN pugi::xml_node &xmlPathNode,
                                                       OUT std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  auto path = node->GetPathModel();

  // path width
  auto widthAttr = xmlPathNode.attribute(s_width);
  if (widthAttr != nullptr) {
    std::wstring ws_width(widthAttr.value());
    LINE_WIDTH lineWidth = prop::LineWidth::ToLineWidth(ws_width);
    path->SetWidth(lineWidth);
  }

  // path color
  auto pathColorAttr = xmlPathNode.attribute(s_color);
  if (pathColorAttr != nullptr) {
    std::wstring ws_pathColor(pathColorAttr.value());
    D2D1_COLOR_F pathColor;
    touchmind::StringToColorF(ws_pathColor, &pathColor);
    path->SetColor(pathColor);
  }

  // path style
  auto pathStyleAttr = xmlPathNode.attribute(s_style);
  if (pathStyleAttr != nullptr) {
    std::wstring ws_pathStyle(pathStyleAttr.value());
    LINE_STYLE lineStyle = prop::LineStyle::ToLineStyle(ws_pathStyle);
    path->SetStyle(lineStyle);
  }

  return S_OK;
}

HRESULT
touchmind::converter::NodeModelXMLDecoder::_DecodeText(IN pugi::xml_node &textNode,
                                                       OUT std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  HRESULT hr = S_OK;
  std::wstring text(textNode.text().get());
  node->SetText(text);

  for (auto child : textNode.children()) {
    if (child.name() == s_fontAttributes) {
      for (auto fontAttribute : child.children()) {
        if (fontAttribute.name() == s_fontAttribute) {
          hr = _DecodeFontAttribute(fontAttribute, node);
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
    IN pugi::xml_node &fontAttributeNode, 
    OUT std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  HRESULT hr = S_OK;

  touchmind::text::FontAttribute fontAttribute;

  // start position
  auto startPositionAttr = fontAttributeNode.attribute(s_startPosition);
  if (startPositionAttr != nullptr) {
    std::wstring ws_startPosition(startPositionAttr.value());
    LONG startPosition = _wtol(ws_startPosition.c_str());
    fontAttribute.startPosition = startPosition;
  }

  // length
  auto lengthAttr = fontAttributeNode.attribute(s_length);
  if (lengthAttr != nullptr) {
    std::wstring ws_length(lengthAttr.value());
    LONG length = _wtol(ws_length.c_str());
    fontAttribute.length = length;
  }

  // font family
  auto fontFamilyAttr = fontAttributeNode.attribute(s_fontFamily);
  if (fontFamilyAttr != nullptr) {
    std::wstring ws_fontFamily(fontFamilyAttr.value());
    fontAttribute.fontFamilyName = ws_fontFamily;
  }

  // font size
  auto fontSizeAttr = fontAttributeNode.attribute(s_fontSize);
  if (fontSizeAttr != nullptr) {
    std::wstring ws_fontSize(fontSizeAttr.value());
    FLOAT fontSize;
    touchmind::StringToFontSize(ws_fontSize, &fontSize);
    fontAttribute.fontSize = fontSize;
  }

  // bold
  auto boldAttr = fontAttributeNode.attribute(s_bold);
  if (boldAttr != nullptr) {
    fontAttribute.bold = (boldAttr.value() == s_boolTrueValue);
  }

  // italic
  auto italicAttr = fontAttributeNode.attribute(s_italic);
  if (italicAttr != nullptr) {
    fontAttribute.italic = (italicAttr.value() == s_boolTrueValue);
  }

  // underline
  auto underlineAttr = fontAttributeNode.attribute(s_underline);
  if (underlineAttr != nullptr) {
    fontAttribute.underline = (underlineAttr.value() == s_boolTrueValue);
  }

  // strikethrough
  auto strikethroughAttr = fontAttributeNode.attribute(s_strikethrough);
  if (strikethroughAttr != nullptr) {
    fontAttribute.strikethrough = (strikethroughAttr.value() == s_boolTrueValue);
  }

  // foreground color
  auto foregroundColorAttr = fontAttributeNode.attribute(s_foregroundColor);
  if (foregroundColorAttr != nullptr) {
    std::wstring ws_foregroundColor(foregroundColorAttr.value());
    COLORREF foregroundColorref;
    touchmind::StringToColorref(ws_foregroundColor, &foregroundColorref);
    fontAttribute.foregroundColor = foregroundColorref;
  }

  node->AddFontAttribute(fontAttribute);
  return hr;
}
