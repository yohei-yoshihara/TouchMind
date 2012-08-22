#include "StdAfx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/model/MapModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/Configuration.h"
#include "touchmind/converter/NodeModelXMLEncoder.h"
#include "touchmind/converter/NodeModelXMLDecoder.h"
#include "touchmind/operation/FileOperation.h"

std::wstring touchmind::model::MapModel::s_defaultRootNodeText(L"Root element");

const static _bstr_t s_tmm(L"tmm");
const static _bstr_t s_version(L"version");
const static _variant_t s_versionValue_1_0(L"1.0");
const static _bstr_t s_tree(L"tree");
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

const static _bstr_t
s_formatXslt(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
             L"<xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" version=\"1.0\">"
             L"<xsl:output method=\"xml\" indent=\"yes\"/>"
             L"<xsl:template match=\"@* | node()\">" L"<xsl:copy>"
             L"<xsl:apply-templates select=\"@* | node()\"/>" L"</xsl:copy>" L"</xsl:template>" L"</xsl:stylesheet>");

touchmind::model::MapModel::MapModel(void):
    m_initialized(false),
    m_fileName(),
    m_root(),
    m_links(),
    m_undo(),
    m_pConfiguration(nullptr),
    m_pSelectionManager(nullptr),
    m_mapModelChangeListeners(),
    m_pFileOperation(nullptr),
    m_pNodeModelXMLEncoder(nullptr),
    m_pNodeModelXMLDecoder(nullptr)
{
}

touchmind::model::MapModel::~MapModel(void)
{
    Clear();
}

void touchmind::model::MapModel::Initialize()
{
    New();
    m_pFileOperation->SetSaveCounter(GetRootNodeModel());
}

void touchmind::model::MapModel::SetRootNodeModel(const std::shared_ptr<touchmind::model::node::NodeModel> &node)
{
    if (node->GetParent() != nullptr) {
        LOG(SEVERITY_LEVEL_ERROR) << L"try to set null as the root node";
        return;
    }

    Clear();
    m_root = node;
}

const std::shared_ptr<touchmind::model::node::NodeModel>& touchmind::model::MapModel::GetRootNodeModel() const
{
    return m_root;
}

void touchmind::model::MapModel::New()
{
    _FireBeforeNewEvent();
    Clear();
    auto root = touchmind::model::node::NodeModel::Create(m_pSelectionManager);
    root->SetText(s_defaultRootNodeText);
    SYSTEMTIME currentTime;
    GetLocalTime(&currentTime);
    root->SetCreatedTime(currentTime);
    root->SetModifiedTime(currentTime);
    SetRootNodeModel(root);
    _FireAfterNewEvent();
    m_pFileOperation->SetSaveCounter(GetRootNodeModel());
}

void touchmind::model::MapModel::Clear()
{
    m_links.clear();
    if (m_root != nullptr) {
        m_root.reset();
    }
    m_fileName = L"";
    m_undo.clear();
}

touchmind::model::MapModelIOResult touchmind::model::MapModel::_Open_Version_1_0(MSXML::IXMLDOMElementPtr tmmElement)
{
    MSXML::IXMLDOMNodeListPtr xmlNodeList = tmmElement->getElementsByTagName(s_tree);
    if (xmlNodeList->length > 0) {
        MSXML::IXMLDOMNodePtr xmlNode = xmlNodeList->item[0];
        MSXML::IXMLDOMNodeListPtr pRootNodeList = xmlNode->childNodes;
        if (pRootNodeList->length > 0) {
            MSXML::IXMLDOMNodePtr xmlRoot = pRootNodeList->item[0];
            auto root = touchmind::model::node::NodeModel::Create(m_pSelectionManager);
            std::vector<std::shared_ptr<model::link::LinkModel>> links;
            CHK_HR(m_pNodeModelXMLDecoder->Decode(xmlRoot, root, links, true, true));
            SetRootNodeModel(root);
            AddLinks(links);
        }
    }
    return MapModelIOResult_OK;
}

touchmind::model::MapModelIOResult touchmind::model::MapModel::Open(const std::wstring & fileName)
{
    _FireBeforeOpenEvent();
    Clear();
    MSXML::IXMLDOMDocumentPtr pXMLDoc;
    HRESULT hr = pXMLDoc.CreateInstance(__uuidof(MSXML::DOMDocument60), nullptr,
                                        CLSCTX_INPROC_SERVER);
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << "Failed to create the XML class instance";
        return MapModelIOResult_InternalError;
    }

    try {
        pXMLDoc->async = VARIANT_FALSE;
        pXMLDoc->validateOnParse = VARIANT_FALSE;
        pXMLDoc->resolveExternals = VARIANT_FALSE;

        if (pXMLDoc->load(fileName.c_str()) == VARIANT_TRUE) {
            MSXML::IXMLDOMNodeListPtr children = pXMLDoc->childNodes;
            for (int i = 0; i < children->length; ++i) {
                MSXML::IXMLDOMNodePtr child = children->item[i];
                if (child->baseName == s_tmm) {
                    MSXML::IXMLDOMElementPtr tmmElement = child;
                    _variant_t s_versionValue = tmmElement->getAttribute(s_version);
                    if (s_versionValue == s_versionValue_1_0) {
                        _Open_Version_1_0(tmmElement);
                    }
                }
            }
        }
    } catch (_com_error& errorObject) {
        LOG(SEVERITY_LEVEL_ERROR) << "Exception thrown, HRESULT: " << errorObject.Error();
        return MapModelIOResult_InternalError;
    }
    m_fileName = fileName;
    _FireAfterOpenEvent();
    return MapModelIOResult_OK;
}

touchmind::model::MapModelIOResult touchmind::model::MapModel::_OpenFromFreeMind_ParseNodeList(
    std::shared_ptr<touchmind::model::node::NodeModel> parent,
    MSXML::IXMLDOMNodeListPtr xmlNodeList)
{
    if (xmlNodeList == nullptr) {
        return MapModelIOResult_OK;
    }

    touchmind::model::MapModelIOResult result = MapModelIOResult_OK;
    for (int i = 0; i < xmlNodeList->length; ++i) {
        MSXML::IXMLDOMNodePtr xmlNode = xmlNodeList->item[i];
        auto node = touchmind::model::node::NodeModel::Create(m_pSelectionManager);
        result = _OpenFromFreeMind_ParseNode(node, xmlNode);
        if (result == MapModelIOResult_OK) {
            parent->AddChild(node);
        } else {
            break;
        }
    }
    return result;
}

int touchmind::model::MapModel::_GetNumericStartIndex(const std::wstring & text)
{
    for (int i = static_cast<int>(text.length() - 1); i >= 0; --i) {
        wchar_t ch = text[i];
        if (ch >= L'0' && ch <= '9') {
            continue;
        } else {
            if (i + 1 < static_cast<int>(text.length())) {
                return i + 1;
            } else {
                return -1;
            }
        }
    }
    return -1;
}

touchmind::model::MapModelIOResult touchmind::model::MapModel::_OpenFromFreeMind_ParseNode(
    std::shared_ptr<touchmind::model::node::NodeModel> node, MSXML::IXMLDOMNodePtr xmlNode)
{
    const static _bstr_t s_created(L"CREATED");
    const static _bstr_t s_modified(L"MODIFIED");
    const static _bstr_t s_position(L"POSITION");
    const static _bstr_t s_text(L"TEXT");

    if (s_node == xmlNode->baseName) {
        MSXML::IXMLDOMNamedNodeMapPtr pAttrMap = xmlNode->attributes;

        // ID
        MSXML::IXMLDOMNodePtr pIdNode = pAttrMap->getNamedItem(s_id);
        touchmind::NODE_ID id = -1;
        wchar_t *offset;
        if (pIdNode != nullptr) {
            std::wstring ws_id(pIdNode->text);
            int numericStartIndex = _GetNumericStartIndex(ws_id);
            if (numericStartIndex >= 0) {
                std::wstring ws_id_mod =
                    ws_id.substr(numericStartIndex, ws_id.length() - numericStartIndex);
                id = wcstoul(ws_id_mod.c_str(), &offset, 10);
            }
        }
        // CREATED
        MSXML::IXMLDOMNodePtr pCreatedNode = pAttrMap->getNamedItem(s_created);
        std::wstring ws_createdTime(pCreatedNode->text);
        LONGLONG createdTime = static_cast < LONGLONG > (wcstod(ws_createdTime.c_str(), &offset));

        // MODIFIED
        MSXML::IXMLDOMNodePtr pModifiedNode = pAttrMap->getNamedItem(s_modified);
        std::wstring ws_modifiedTime(pModifiedNode->text);

        // TEXT
        MSXML::IXMLDOMNodePtr pTextNode = pAttrMap->getNamedItem(s_text);

        // POSITION
        MSXML::IXMLDOMNodePtr pPositionNode = pAttrMap->getNamedItem(s_position);
        touchmind::NODE_SIDE position = touchmind::NODE_SIDE_UNDEFINED;
        if (pPositionNode != nullptr) {
            if (s_positionRightValue == pPositionNode->text) {
                position = touchmind::NODE_SIDE_RIGHT;
            } else if (s_positionLeftValue == pPositionNode->text) {
                position = touchmind::NODE_SIDE_LEFT;
            }
        }

        if (id != -1) {
            node->SetId(id);
        }
        node->SetCreatedTime(createdTime);
        node->SetModifiedTime(createdTime);
        node->SetText(std::wstring(pTextNode->text));
        node->SetPosition(position);

        MSXML::IXMLDOMNodeListPtr pNodeList = xmlNode->childNodes;
        _OpenFromFreeMind_ParseNodeList(node, pNodeList);
    }
    return MapModelIOResult_OK;
}

touchmind::model::MapModelIOResult touchmind::model::MapModel::OpenFromFreeMind(const std::wstring & fileName)
{
    const static _bstr_t s_map(L"map");

    _FireBeforeOpenEvent();
    Clear();

    MSXML::IXMLDOMDocumentPtr pXMLDoc;
    HRESULT hr = pXMLDoc.CreateInstance(__uuidof(MSXML::DOMDocument60), nullptr,
                                        CLSCTX_INPROC_SERVER);
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << "Failed to create the XML class instance";
        return MapModelIOResult_InternalError;
    }

    auto rootNodeModel = touchmind::model::node::NodeModel::Create(m_pSelectionManager);
    try {
        pXMLDoc->async = VARIANT_FALSE;
        pXMLDoc->validateOnParse = VARIANT_FALSE;
        pXMLDoc->resolveExternals = VARIANT_FALSE;

        if (pXMLDoc->load(fileName.c_str()) == VARIANT_TRUE) {
            MSXML::IXMLDOMNodePtr pMapNode;
            pXMLDoc->get_firstChild(&pMapNode);
            if (s_map != pMapNode->baseName) {
                LOG(SEVERITY_LEVEL_ERROR)
                        << "Root node of XML document must be 'map', but '" << pMapNode->
                        baseName << "'";
                return MapModelIOResult_FileFormatError;
            }
            MSXML::IXMLDOMNodeListPtr pNodeList = pMapNode->childNodes;
            for (int i = 0; i < pNodeList->length; ++i) {
                MSXML::IXMLDOMNodePtr pNode = pNodeList->item[i];
                if (s_node == pNode->baseName) {
                    _OpenFromFreeMind_ParseNode(rootNodeModel, pNode);
                    // we assume that map node has only one node
                    break;
                }
            }
        } else {
            LOG(SEVERITY_LEVEL_ERROR)
                    << "Failed to load DOM. " << (LPCSTR) pXMLDoc->parseError->Getreason();
            return MapModelIOResult_LoadFailed;
        }
    } catch (_com_error& errorObject) {
        LOG(SEVERITY_LEVEL_ERROR) << "Exception thrown, HRESULT: " << errorObject.Error();
        return MapModelIOResult_InternalError;
    }
    SetRootNodeModel(rootNodeModel);
    _FireAfterOpenEvent();
    return MapModelIOResult_OK;
}

touchmind::model::MapModelIOResult touchmind::model::MapModel::Save()
{
    return SaveAs(m_fileName);
}

touchmind::model::MapModelIOResult touchmind::model::MapModel::SaveAs(const std::wstring & fileName)
{
    _FireBeforeSaveEvent();
    MSXML::IXMLDOMDocumentPtr pXMLDoc;
    HRESULT hr = pXMLDoc.CreateInstance(__uuidof(MSXML::DOMDocument60), nullptr,
                                        CLSCTX_INPROC_SERVER);
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << "Failed to create the XML class instance";
        return MapModelIOResult_InternalError;
    }

    try {
        MSXML::IXMLDOMProcessingInstructionPtr pXMLProcessingNode =
            pXMLDoc->createProcessingInstruction("xml", " version=\"1.0\" encoding=\"UTF-8\"");
        pXMLDoc->appendChild(pXMLProcessingNode);

        // tmm
        MSXML::IXMLDOMElementPtr tmmNode = pXMLDoc->createElement(s_tmm);
        pXMLDoc->appendChild(tmmNode);

        // version
        _variant_t v_versionValue(L"1.0");
        tmmNode->setAttribute(s_version, v_versionValue);

        // tree
        MSXML::IXMLDOMElementPtr treeNode = pXMLDoc->createElement(s_tree);
        tmmNode->appendChild(treeNode);

        // root node
        MSXML::IXMLDOMElementPtr node = pXMLDoc->createElement(s_node);
        treeNode->appendChild(node);

        hr = m_pNodeModelXMLEncoder->Encode(m_root, pXMLDoc, node);
        MapModelIOResult result = SUCCEEDED(hr) ? MapModelIOResult_OK : MapModelIOResult_InternalError;

        if (result == MapModelIOResult_OK) {
            _variant_t varString = fileName.c_str();
            hr = pXMLDoc->save(varString);
            if (FAILED(hr)) {
                LOG(SEVERITY_LEVEL_ERROR) << L"Failed to save, hr = " << hr;
                return MapModelIOResult_FileIOError;
            }
        }
    } catch (_com_error& errorObject) {
        LOG(SEVERITY_LEVEL_ERROR) << "Exception thrown, HRESULT: " << errorObject.Error();
        return MapModelIOResult_InternalError;
    }
    m_fileName = fileName;
    _FireAfterSaveEvent();
    m_pFileOperation->SetSaveCounter(GetRootNodeModel());
    return MapModelIOResult_OK;
}

void touchmind::model::MapModel::BeginTransaction()
{
    while (m_undo.size() > m_pConfiguration->GetMaxUndoCount()) {
        m_undo.pop_front();
    }

    std::shared_ptr<touchmind::model::node::NodeModel> copiedRoot;
    std::vector<std::shared_ptr<touchmind::model::link::LinkModel>> copiedLinks;
    _MakeDeepCopy(copiedRoot, copiedLinks);

    m_undo.push_back(UndoContainer(copiedRoot, copiedLinks));
}

void touchmind::model::MapModel::EndTransaction()
{
}

bool touchmind::model::MapModel::Roleback()
{
    if (m_undo.size() >= 1) {
        UndoContainer &undo = m_undo.back();
        m_root = undo.root;
        m_links.clear();
        m_links.resize(undo.links.size());
        std::copy(std::begin(undo.links), std::end(undo.links), std::begin(m_links));
        NormalizeLinks();
        m_root->RenumberIds();
        RenumberLinkIds();
        m_undo.pop_back();
    }
    return true;
}

void touchmind::model::MapModel::AddMapModelChangeListener(IMapModelChangeListener * pMapModelChangeListener)
{
    m_mapModelChangeListeners.push_back(pMapModelChangeListener);
}

void touchmind::model::MapModel::RemoveMapModelChangeListener(IMapModelChangeListener * pMapModelChangeListener)
{
    m_mapModelChangeListeners.erase(std::remove(
                                        std::begin(m_mapModelChangeListeners),
                                        std::end(m_mapModelChangeListeners),
                                        pMapModelChangeListener),
                                    std::end(m_mapModelChangeListeners));
}

void touchmind::model::MapModel::_FireBeforeNewEvent()
{
    for (auto it = std::begin(m_mapModelChangeListeners); it != std::end(m_mapModelChangeListeners); ++it) {
        (*it)->BeforeNew(this);
    }
}

void touchmind::model::MapModel::_FireAfterNewEvent()
{
    for (auto it = std::begin(m_mapModelChangeListeners); it != std::end(m_mapModelChangeListeners); ++it) {
        (*it)->AfterNew(this);
    }
}

void touchmind::model::MapModel::_FireBeforeOpenEvent()
{
    for (auto it = std::begin(m_mapModelChangeListeners); it != std::end(m_mapModelChangeListeners); ++it) {
        (*it)->BeforeOpen(this);
    }
}

void touchmind::model::MapModel::_FireAfterOpenEvent()
{
    for (auto it = std::begin(m_mapModelChangeListeners); it != std::end(m_mapModelChangeListeners); ++it) {
        (*it)->AfterOpen(this);
    }
}

void touchmind::model::MapModel::_FireBeforeSaveEvent()
{
    for (auto it = std::begin(m_mapModelChangeListeners); it != std::end(m_mapModelChangeListeners); ++it) {
        (*it)->BeforeSave(this);
    }
}

void touchmind::model::MapModel::_FireAfterSaveEvent()
{
    for (auto it = std::begin(m_mapModelChangeListeners); it != std::end(m_mapModelChangeListeners); ++it) {
        (*it)->AfterSave(this);
    }
}

void touchmind::model::MapModel::AddLink(const std::shared_ptr<touchmind::model::link::LinkModel> &linkModel)
{
    ASSERT(linkModel->IsValid());
    if (linkModel->IsValid()) {
        m_links.push_back(linkModel);
        auto node1 = linkModel->GetNode(EDGE_ID_1);
        auto node2 = linkModel->GetNode(EDGE_ID_2);
        node1->AddLink(linkModel);
        node2->AddLink(linkModel);
    }
}

void touchmind::model::MapModel::AddLinks(const std::vector<std::shared_ptr<touchmind::model::link::LinkModel>> &links)
{
    for (auto it = std::begin(links); it != std::end(links); ++it) {
        AddLink(*it);
    }
}

void touchmind::model::MapModel::RemoveLink(const std::shared_ptr<touchmind::model::link::LinkModel> &linkModel)
{
    auto node1 = linkModel->GetNode(EDGE_ID_1);
    node1->RemoveLink(linkModel);
    auto node2 = linkModel->GetNode(EDGE_ID_2);
    node2->RemoveLink(linkModel);
    m_links.erase(std::remove_if(
                      std::begin(m_links),
                      std::end(m_links),
    [&](std::shared_ptr<touchmind::model::link::LinkModel> e) {
        return e == linkModel;
    }), std::end(m_links));
}

void touchmind::model::MapModel::NormalizeLinks()
{
    for (auto it = std::begin(m_links); it != std::end(m_links); ++it) {
        auto link = *it;
        NODE_ID node1Id = link->GetNodeId(EDGE_ID_1);
        auto node1 = m_root->FindByNodeId(node1Id);
        NODE_ID node2Id = link->GetNodeId(EDGE_ID_2);
        auto node2 = m_root->FindByNodeId(node2Id);
        if (node1 != nullptr && node2 != nullptr) {
            link->SetNode(EDGE_ID_1, node1);
            link->SetNode(EDGE_ID_2, node2);
        } else {
            link->SetNode(EDGE_ID_1, nullptr);
            link->SetNode(EDGE_ID_2, nullptr);
        }
    }
    m_links.erase(std::remove_if(std::begin(m_links), std::end(m_links),
    [](const std::shared_ptr<touchmind::model::link::LinkModel> &link)->bool {
        return !link->IsValid();
    }), std::end(m_links));
}

void touchmind::model::MapModel::_MakeDeepCopy(
    std::shared_ptr<touchmind::model::node::NodeModel> &copiedRoot,
    std::vector<std::shared_ptr<touchmind::model::link::LinkModel>> &copiedLinks )
{
    copiedRoot = m_root->DeepCopy();
    copiedLinks.clear();
    copiedLinks.resize(m_links.size());
    for (size_t i = 0; i < m_links.size(); ++i) {
        copiedLinks[i] = std::make_shared<touchmind::model::link::LinkModel>(*m_links[i]);
    }
}

void touchmind::model::MapModel::RenumberLinkIds()
{
    for (size_t i = 0; i < m_links.size(); ++i) {
        m_links[i]->SetLinkId(model::link::LinkModel::GenerateLinkId());
    }
}
