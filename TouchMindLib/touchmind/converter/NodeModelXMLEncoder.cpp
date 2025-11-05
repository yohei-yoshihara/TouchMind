#include "StdAfx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/text/FontAttributeCommand.h"
#include "touchmind/util/ColorUtil.h"
#include "touchmind/converter/NodeModelXMLEncoder.h"
#include "touchmind/model/BaseModel.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/path/PathModel.h"
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

touchmind::converter::NodeModelXMLEncoder::NodeModelXMLEncoder(void) {
}

touchmind::converter::NodeModelXMLEncoder::~NodeModelXMLEncoder(void) {
}

HRESULT touchmind::converter::NodeModelXMLEncoder::Encode(IN std::shared_ptr<touchmind::model::node::NodeModel> node,
                                                          OUT pugi::xml_node &xmlNodeElement) {
  HRESULT hr = S_OK;

    // id
    std::wstring ws_idValue;
    touchmind::NodeIdToString(node->GetId(), ws_idValue);
    xmlNodeElement.append_attribute(s_id) = ws_idValue;

    // position
    if (node->GetPosition() == NODE_SIDE_LEFT) {
      xmlNodeElement.append_attribute(s_position) = v_positionLeftValue;
    } else if (node->GetPosition() == NODE_SIDE_RIGHT) {
      xmlNodeElement.append_attribute(s_position) = v_positionRightValue;
    }

    // created time
    std::wstring ws_createdTime;
    touchmind::SystemtimeToString(&node->GetCreatedTime(), ws_createdTime);
    xmlNodeElement.append_attribute(s_createdTime) = ws_createdTime;

    // modified time
    std::wstring ws_modifiedTime;
    touchmind::SystemtimeToString(&node->GetModifiedTime(), ws_modifiedTime);
    xmlNodeElement.append_attribute(s_modifiedTime) = ws_modifiedTime;

    // width
    std::wstring ws_width;
    touchmind::SizeToString(node->GetWidth(), ws_width);
    xmlNodeElement.append_attribute(s_width) = ws_width;

    // height
    std::wstring ws_height;
    touchmind::SizeToString(node->GetHeight(), ws_height);
    xmlNodeElement.append_attribute(s_height) = ws_height;

    // background color
    if (!touchmind::util::ColorUtil::Equal(node->GetBackgroundColor(), D2D1::ColorF(D2D1::ColorF::White))) {
      std::wstring ws_backgroundColorValue;
      touchmind::ColorFToString(node->GetBackgroundColor(), ws_backgroundColorValue);
      xmlNodeElement.append_attribute(s_backgroundColor) = ws_backgroundColorValue;
    }

    // shape
    if (node->GetNodeShape() != prop::NodeShape::GetDefaultNodeShape()) {
      std::wstring ws_nodeShape = prop::NodeShape::ToString(node->GetNodeShape());
      _variant_t v_nodeShape(ws_nodeShape.c_str());
      xmlNodeElement.append_attribute(s_shape) = ws_nodeShape;
    }

    // text element
    pugi::xml_node xmlTextElement = xmlNodeElement.append_child(s_text);

    // text
    std::wstring s_textValue(node->GetText().c_str());
    xmlTextElement.text().set(s_textValue);

    if (node->GetFontAttributeCount() > 0) {
      // fontAttriutes element
      pugi::xml_node xmlFontAttributesElement =
          xmlTextElement.append_child(s_fontAttributes);

      for (size_t i = 0; i < node->GetFontAttributeCount(); ++i) {
        // fontAttribute
        pugi::xml_node xmlFontAttributeElement =
            xmlFontAttributesElement.append_child(s_fontAttribute);

        // start position
        auto ws_startPositionValue =
            std::to_wstring(node->GetFontAttribute(i).startPosition);
        xmlFontAttributeElement.append_attribute(s_startPosition) = ws_startPositionValue;

        // length
        auto ws_lengthValue = std::to_wstring(node->GetFontAttribute(i).length);
        xmlFontAttributeElement.append_attribute(s_length) = ws_lengthValue;

        // font family
        if (node->GetFontAttribute(i).fontFamilyName.length() > 0) {
          xmlFontAttributeElement.append_attribute(s_fontFamily) = node->GetFontAttribute(i).fontFamilyName;
        }

        // font size
        if (node->GetFontAttribute(i).fontSize > 0.0f) {
          std::wstring ws_fontSizeValue;
          touchmind::FontSizeToString(node->GetFontAttribute(i).fontSize, ws_fontSizeValue);
          xmlFontAttributeElement.append_attribute(s_fontSize) = ws_fontSizeValue;
        }

        // bold
        if (node->GetFontAttribute(i).bold) {
          xmlFontAttributeElement.append_attribute(s_bold) = v_boolTrueValue;
        }

        // italic
        if (node->GetFontAttribute(i).italic) {
          xmlFontAttributeElement.append_attribute(s_italic) = v_boolTrueValue;
        }

        // underline
        if (node->GetFontAttribute(i).underline) {
          xmlFontAttributeElement.append_attribute(s_underline) = v_boolTrueValue;
        }

        // strikethrough
        if (node->GetFontAttribute(i).strikethrough) {
          xmlFontAttributeElement.append_attribute(s_strikethrough) = v_boolTrueValue;
        }

        // foreground color
        if (node->GetFontAttribute(i).foregroundColor
            != static_cast<LONG>(touchmind::text::FontAttributeCommand::DEFAULT_FONT_COLOR)) {
          std::wstring ws_foregroundColorValue;
          touchmind::ColorrefToString(node->GetFontAttribute(i).foregroundColor, ws_foregroundColorValue);
          xmlFontAttributeElement.append_attribute(s_foregroundColor) =
              ws_foregroundColorValue;
        }
      }
    }

    {
      // ***** path *****
      auto path = node->GetPathModel();
      std::unordered_map<std::wstring, std::wstring> attributes;
      // path width
      if (path->GetWidth() != LINE_WIDTH_1) {
        std::wstring ws_pathWidth = prop::LineWidth::ToString(path->GetWidth());
        attributes[s_width] = ws_pathWidth;
      }

      // path color
      if (!touchmind::util::ColorUtil::Equal(path->GetColor(), D2D1::ColorF(D2D1::ColorF::Black))) {
        std::wstring ws_colorValue;
        touchmind::ColorFToString(path->GetColor(), ws_colorValue);
        attributes[s_color] = ws_colorValue;
      }

      // path style
      if (path->GetStyle() != LINE_STYLE_SOLID) {
        std::wstring ws_pathStyle = prop::LineStyle::ToString(path->GetStyle());
        attributes[s_style] = ws_pathStyle;
      }

      if (attributes.size() > 0) {
        pugi::xml_node xmlPathElement = xmlNodeElement.append_child(s_path);
        for (const auto &attribute : attributes) {
          xmlPathElement.append_attribute(attribute.first) = attribute.second;
        }
      }
    }

    for (size_t i = 0; i < node->GetNumberOfLinks(); ++i) {
      auto wlink = node->GetLink(i);
      if (!wlink.expired()) {
        auto link = wlink.lock();
        if (link->GetNode(EDGE_ID_1) == node) {
          pugi::xml_node xmlLinkElement = xmlNodeElement.append_child(s_link);

          // destination
          std::wstring ws_dstIdValue;
          touchmind::NodeIdToString(link->GetNode(EDGE_ID_2)->GetId(),
                                    ws_dstIdValue);
          xmlLinkElement.append_attribute(s_destination) = ws_dstIdValue;

          // link width
          std::wstring ws_linkWidth = prop::LineWidth::ToString(link->GetLineWidth());
          _variant_t v_linkWidth(ws_linkWidth.c_str());
          xmlLinkElement.append_attribute(s_width) = ws_linkWidth;

          // link color
          if (!touchmind::util::ColorUtil::Equal(link->GetLineColor(), D2D1::ColorF(D2D1::ColorF::Black))) {
            std::wstring ws_colorValue;
            touchmind::ColorFToString(link->GetLineColor(), ws_colorValue);
            xmlLinkElement.append_attribute(s_color) = ws_colorValue;
          }

          // link style
          std::wstring ws_lineStyle = prop::LineStyle::ToString(link->GetLineStyle());
          xmlLinkElement.append_attribute(s_style) = ws_lineStyle;

          // link edge style 1
          std::wstring ws_edgeStyle1 = prop::LineEdgeStyle::ToString(link->GetEdge(EDGE_ID_1)->GetStyle());
          xmlLinkElement.append_attribute(s_startStyle) = ws_edgeStyle1;

          // link edge style 2
          std::wstring ws_edgeStyle2 = prop::LineEdgeStyle::ToString(link->GetEdge(EDGE_ID_2)->GetStyle());
          xmlLinkElement.append_attribute(s_endStyle) = ws_edgeStyle2;

          // handle 1
          std::wstring ws_handle1;
          HandleToString(link->GetEdge(EDGE_ID_1)->GetAngle(), link->GetEdge(EDGE_ID_1)->GetLength(), ws_handle1);
          xmlLinkElement.append_attribute(s_startHandle) = ws_handle1;

          // handle 2
          std::wstring ws_handle2;
          HandleToString(link->GetEdge(EDGE_ID_2)->GetAngle(), link->GetEdge(EDGE_ID_2)->GetLength(), ws_handle2);
          xmlLinkElement.append_attribute(s_endHandle) = ws_handle2;
        }
      }
    }

    for (size_t i = 0; i < node->GetActualChildrenCount(); ++i) {
      std::shared_ptr<touchmind::model::node::NodeModel> child = node->GetChild(i);
      pugi::xml_node xmlChildNodeElement = xmlNodeElement.append_child(s_node);
      Encode(child, xmlChildNodeElement);
    }

  return hr;
}
