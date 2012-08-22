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

touchmind::converter::NodeModelXMLEncoder::NodeModelXMLEncoder(void)
{
}

touchmind::converter::NodeModelXMLEncoder::~NodeModelXMLEncoder(void)
{
}

HRESULT touchmind::converter::NodeModelXMLEncoder::Encode(
    IN std::shared_ptr<touchmind::model::node::NodeModel> node,
    IN MSXML::IXMLDOMDocumentPtr pXMLDoc,
    OUT MSXML::IXMLDOMElementPtr &xmlNodeElement )
{
    HRESULT hr = S_OK;
    try {
        // id
        std::wstring ws_idValue;
        touchmind::NodeIdToString(node->GetId(), ws_idValue);
        _variant_t v_idValue(ws_idValue.c_str());
        xmlNodeElement->setAttribute(s_id, v_idValue);

        // position
        if (node->GetPosition() == NODE_SIDE_LEFT) {
            xmlNodeElement->setAttribute(s_position, v_positionLeftValue);
        } else if (node->GetPosition() == NODE_SIDE_RIGHT) {
            xmlNodeElement->setAttribute(s_position, v_positionRightValue);
        }

        // created time
        std::wstring ws_createdTime;
        touchmind::SystemtimeToString(&node->GetCreatedTime(), ws_createdTime);
        _variant_t v_createdTimeValue(ws_createdTime.c_str());
        xmlNodeElement->setAttribute(s_createdTime, v_createdTimeValue);

        // modified time
        std::wstring ws_modifiedTime;
        touchmind::SystemtimeToString(&node->GetModifiedTime(), ws_modifiedTime);
        _variant_t v_modifiedTimeValue(ws_modifiedTime.c_str());
        xmlNodeElement->setAttribute(s_modifiedTime, v_modifiedTimeValue);

        // width
        std::wstring ws_width;
        touchmind::SizeToString(node->GetWidth(), ws_width);
        _variant_t v_width(ws_width.c_str());
        xmlNodeElement->setAttribute(s_width, v_width);

        // height
        std::wstring ws_height;
        touchmind::SizeToString(node->GetHeight(), ws_height);
        _variant_t v_height(ws_height.c_str());
        xmlNodeElement->setAttribute(s_height, v_height);

        // background color
        if (!touchmind::util::ColorUtil::Equal(node->GetBackgroundColor(), D2D1::ColorF(D2D1::ColorF::White))) {
            std::wstring ws_backgroundColorValue;
            //touchmind::ColorrefToString(touchmind::util::ColorUtil::ToColorref(node->GetBackgroundColor()), ws_backgroundColorValue);
            touchmind::ColorFToString(node->GetBackgroundColor(), ws_backgroundColorValue);
            _variant_t v_backgroundColorValue(ws_backgroundColorValue.c_str());
            xmlNodeElement->setAttribute(s_backgroundColor, v_backgroundColorValue);
        }

        // shape
        if (node->GetNodeShape() != prop::NodeShape::GetDefaultNodeShape()) {
            std::wstring ws_nodeShape = prop::NodeShape::ToString(node->GetNodeShape());
            _variant_t v_nodeShape(ws_nodeShape.c_str());
            xmlNodeElement->setAttribute(s_shape, v_nodeShape);
        }

        xmlNodeElement->setAttribute(s_height, v_height);
        // text element
        MSXML::IXMLDOMElementPtr xmlTextElement = pXMLDoc->createElement(s_text);
        xmlNodeElement->appendChild(xmlTextElement);

        // text
        _bstr_t s_textValue(node->GetText().c_str());
        MSXML::IXMLDOMTextPtr pText = pXMLDoc->createTextNode(s_textValue);
        xmlTextElement->appendChild(pText);

        if (node->GetFontAttributeCount() > 0) {
            // fontAttriutes element
            MSXML::IXMLDOMElementPtr xmlFontAttributesElement = pXMLDoc->createElement(s_fontAttributes);
            xmlTextElement->appendChild(xmlFontAttributesElement);

            for (size_t i = 0; i < node->GetFontAttributeCount(); ++i) {
                // fontAttribute
                MSXML::IXMLDOMElementPtr xmlFontAttributeElement = pXMLDoc->createElement(s_fontAttribute);
                xmlFontAttributesElement->appendChild(xmlFontAttributeElement);

                // start position
                wchar_t buf[1024];
                _ltow_s(node->GetFontAttribute(i).startPosition, buf, 1024, 10);
                _variant_t v_startPositionValue(buf);
                xmlFontAttributeElement->setAttribute(s_startPosition, v_startPositionValue);

                // length
                _ltow_s(node->GetFontAttribute(i).length, buf, 1024, 10);
                _variant_t v_lengthValue(buf);
                xmlFontAttributeElement->setAttribute(s_length, v_lengthValue);

                // font family
                if (node->GetFontAttribute(i).fontFamilyName.length() > 0) {
                    _variant_t v_fontFamilyValue(node->GetFontAttribute(i).fontFamilyName.c_str());
                    xmlFontAttributeElement->setAttribute(s_fontFamily, v_fontFamilyValue);
                }

                // font size
                if (node->GetFontAttribute(i).fontSize > 0.0f) {
                    std::wstring ws_fontSizeValue;
                    touchmind::FontSizeToString(node->GetFontAttribute(i).fontSize, ws_fontSizeValue);
                    _variant_t v_fontSizeValue(ws_fontSizeValue.c_str());
                    xmlFontAttributeElement->setAttribute(s_fontSize, v_fontSizeValue);
                }

                // bold
                if (node->GetFontAttribute(i).bold) {
                    xmlFontAttributeElement->setAttribute(s_bold, v_boolTrueValue);
                }

                // italic
                if (node->GetFontAttribute(i).italic) {
                    xmlFontAttributeElement->setAttribute(s_italic, v_boolTrueValue);
                }

                // underline
                if (node->GetFontAttribute(i).underline) {
                    xmlFontAttributeElement->setAttribute(s_underline, v_boolTrueValue);
                }

                // strikethrough
                if (node->GetFontAttribute(i).strikethrough) {
                    xmlFontAttributeElement->setAttribute(s_strikethrough, v_boolTrueValue);
                }

                // foreground color
                if (node->GetFontAttribute(i).foregroundColor != static_cast<LONG>(touchmind::text::FontAttributeCommand::DEFAULT_FONT_COLOR)) {
                    std::wstring ws_foregroundColorValue;
                    touchmind::ColorrefToString(node->GetFontAttribute(i).foregroundColor, ws_foregroundColorValue);
                    _variant_t v_foregroundColorValue(ws_foregroundColorValue.c_str());
                    xmlFontAttributeElement->setAttribute(s_foregroundColor, v_foregroundColorValue);
                }
            }
        }

        {
            bool hasOutput = false;
            // ***** path *****
            auto path = node->GetPathModel();
            // path
            MSXML::IXMLDOMElementPtr xmlPathElement = pXMLDoc->createElement(s_path);

            // path width
            if (path->GetWidth() != LINE_WIDTH_1) {
                std::wstring ws_pathWidth = prop::LineWidth::ToString(path->GetWidth());
                _variant_t v_pathWidth(ws_pathWidth.c_str());
                xmlPathElement->setAttribute(s_width, v_pathWidth);
                hasOutput = true;
            }

            // path color
            if (!touchmind::util::ColorUtil::Equal(path->GetColor(), D2D1::ColorF(D2D1::ColorF::Black))) {
                std::wstring ws_colorValue;
                touchmind::ColorFToString(path->GetColor(), ws_colorValue);
                _variant_t v_colorValue(ws_colorValue.c_str());
                xmlPathElement->setAttribute(s_color, v_colorValue);
                hasOutput = true;
            }

            // path style
            if (path->GetStyle() != LINE_STYLE_SOLID) {
                std::wstring ws_pathStyle = prop::LineStyle::ToString(path->GetStyle());
                _variant_t v_pathStyle(ws_pathStyle.c_str());
                xmlPathElement->setAttribute(s_style, v_pathStyle);
                hasOutput = true;
            }

            if (hasOutput) {
                xmlNodeElement->appendChild(xmlPathElement);
            }
        }

        for (size_t i = 0; i < node->GetNumberOfLinks(); ++i) {
            auto wlink = node->GetLink(i);
            if (!wlink.expired()) {
                auto link = wlink.lock();
                if (link->GetNode(EDGE_ID_1) == node) {
                    MSXML::IXMLDOMElementPtr xmlLinkElement = pXMLDoc->createElement(s_link);
                    xmlNodeElement->appendChild(xmlLinkElement);

                    // destination
                    std::wstring ws_idValue;
                    touchmind::NodeIdToString(link->GetNode(EDGE_ID_2)->GetId(), ws_idValue);
                    _variant_t v_idValue(ws_idValue.c_str());
                    xmlLinkElement->setAttribute(s_destination, v_idValue);

                    // link width
                    std::wstring ws_linkWidth = prop::LineWidth::ToString(link->GetLineWidth());
                    _variant_t v_linkWidth(ws_linkWidth.c_str());
                    xmlLinkElement->setAttribute(s_width, v_linkWidth);

                    // link color
                    if (!touchmind::util::ColorUtil::Equal(link->GetLineColor(), D2D1::ColorF(D2D1::ColorF::Black))) {
                        std::wstring ws_colorValue;
                        touchmind::ColorFToString(link->GetLineColor(), ws_colorValue);
                        _variant_t v_colorValue(ws_colorValue.c_str());
                        xmlLinkElement->setAttribute(s_color, v_colorValue);
                    }

                    // link style
                    std::wstring ws_lineStyle = prop::LineStyle::ToString(link->GetLineStyle());
                    _variant_t v_lineStyle(ws_lineStyle.c_str());
                    xmlLinkElement->setAttribute(s_style, v_lineStyle);

                    // link edge style 1
                    std::wstring ws_edgeStyle1 = prop::LineEdgeStyle::ToString(link->GetEdge(EDGE_ID_1)->GetStyle());
                    _variant_t v_edgeStyle1(ws_edgeStyle1.c_str());
                    xmlLinkElement->setAttribute(s_startStyle, v_edgeStyle1);

                    // link edge style 2
                    std::wstring ws_edgeStyle2 = prop::LineEdgeStyle::ToString(link->GetEdge(EDGE_ID_2)->GetStyle());
                    _variant_t v_edgeStyle2(ws_edgeStyle2.c_str());
                    xmlLinkElement->setAttribute(s_endStyle, v_edgeStyle2);

                    // handle 1
                    std::wstring ws_handle1;
                    HandleToString(link->GetEdge(EDGE_ID_1)->GetAngle(),
                                   link->GetEdge(EDGE_ID_1)->GetLength(),
                                   ws_handle1);
                    _variant_t v_handle1(ws_handle1.c_str());
                    xmlLinkElement->setAttribute(s_startHandle, v_handle1);

                    // handle 2
                    std::wstring ws_handle2;
                    HandleToString(link->GetEdge(EDGE_ID_2)->GetAngle(),
                                   link->GetEdge(EDGE_ID_2)->GetLength(),
                                   ws_handle2);
                    _variant_t v_handle2(ws_handle2.c_str());
                    xmlLinkElement->setAttribute(s_endHandle, v_handle2);
                }
            }
        }

        for (size_t i = 0; i < node->GetActualChildrenCount(); ++i) {
            std::shared_ptr<touchmind::model::node::NodeModel> child = node->GetChild(i);
            MSXML::IXMLDOMElementPtr xmlChildNodeElement = pXMLDoc->createElement(s_node);
            xmlNodeElement->appendChild(xmlChildNodeElement);
            Encode(child, pXMLDoc, xmlChildNodeElement);
        }

    } catch (_com_error& errorObject) {
        LOG(SEVERITY_LEVEL_ERROR) << "Exception thrown, HRESULT: " << errorObject.Error();
        return errorObject.Error();
    }

    return hr;
}
