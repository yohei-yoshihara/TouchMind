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

const static std::wstring s_tmm(L"tmm");
const static std::wstring s_version(L"version");
const static std::wstring s_versionValue_1_0(L"1.0");
const static std::wstring s_tree(L"tree");
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

const static std::wstring s_formatXslt(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                                  L"<xsl:stylesheet xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" version=\"1.0\">"
                                  L"<xsl:output method=\"xml\" indent=\"yes\"/>"
                                  L"<xsl:template match=\"@* | node()\">"
                                  L"<xsl:copy>"
                                  L"<xsl:apply-templates select=\"@* | node()\"/>"
                                  L"</xsl:copy>"
                                  L"</xsl:template>"
                                  L"</xsl:stylesheet>");

touchmind::model::MapModel::MapModel(void)
    : m_initialized(false)
    , m_fileName()
    , m_root()
    , m_links()
    , m_undo()
    , m_pConfiguration(nullptr)
    , m_pSelectionManager(nullptr)
    , m_mapModelChangeListeners()
    , m_pFileOperation(nullptr)
    , m_pNodeModelXMLEncoder(nullptr)
    , m_pNodeModelXMLDecoder(nullptr) {
}

touchmind::model::MapModel::~MapModel(void) {
  Clear();
}

void touchmind::model::MapModel::Initialize() {
  New();
  m_pFileOperation->SetSaveCounter(GetRootNodeModel());
}

void touchmind::model::MapModel::SetRootNodeModel(const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  if (node->GetParent() != nullptr) {
    SPDLOG_ERROR(L"try to set null as the root node");
    return;
  }

  Clear();
  m_root = node;
}

const std::shared_ptr<touchmind::model::node::NodeModel> &touchmind::model::MapModel::GetRootNodeModel() const {
  return m_root;
}

void touchmind::model::MapModel::New() {
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

void touchmind::model::MapModel::Clear() {
  m_links.clear();
  if (m_root != nullptr) {
    m_root.reset();
  }
  m_fileName = L"";
  m_undo.clear();
}

touchmind::model::MapModelIOResult touchmind::model::MapModel::_Open_Version_1_0(pugi::xml_node tmmElement) {
  auto xmlNodeList = tmmElement.children(s_tree.c_str());
  for (auto xmlNode : xmlNodeList) {
    auto root = touchmind::model::node::NodeModel::Create(m_pSelectionManager);
    std::vector<std::shared_ptr<model::link::LinkModel>> links;
    THROW_IF_FAILED(m_pNodeModelXMLDecoder->Decode(xmlNode, root, links, true, true));
    SetRootNodeModel(root);
    AddLinks(links);
  }
  return MapModelIOResult_OK;
}

touchmind::model::MapModelIOResult touchmind::model::MapModel::Open(const std::wstring &fileName) {
  _FireBeforeOpenEvent();
  Clear();
  pugi::xml_document xmlDoc;
  auto result = xmlDoc.load_file(fileName.c_str());
  if (!result) {
    SPDLOG_ERROR(L"failed to load the file '{}'", fileName);
    return MapModelIOResult_InternalError;
  }

  for (auto child : xmlDoc.children()) {
    if (child.name() == s_tmm) {
      auto versionValue = child.attribute(s_version);
      if (versionValue.value() == s_versionValue_1_0) {
        _Open_Version_1_0(child);
      }
    }
  }

  m_fileName = fileName;
  _FireAfterOpenEvent();
  return MapModelIOResult_OK;
}

//touchmind::model::MapModelIOResult
//touchmind::model::MapModel::_OpenFromFreeMind_ParseNodeList(std::shared_ptr<touchmind::model::node::NodeModel> parent,
//                                                            MSXML::IXMLDOMNodeListPtr xmlNodeList) {
//  if (xmlNodeList == nullptr) {
//    return MapModelIOResult_OK;
//  }
//
//  touchmind::model::MapModelIOResult result = MapModelIOResult_OK;
//  for (int i = 0; i < xmlNodeList->length; ++i) {
//    MSXML::IXMLDOMNodePtr xmlNode = xmlNodeList->item[i];
//    auto node = touchmind::model::node::NodeModel::Create(m_pSelectionManager);
//    result = _OpenFromFreeMind_ParseNode(node, xmlNode);
//    if (result == MapModelIOResult_OK) {
//      parent->AddChild(node);
//    } else {
//      break;
//    }
//  }
//  return result;
//}

int touchmind::model::MapModel::_GetNumericStartIndex(const std::wstring &text) {
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

//touchmind::model::MapModelIOResult
//touchmind::model::MapModel::_OpenFromFreeMind_ParseNode(std::shared_ptr<touchmind::model::node::NodeModel> node,
//                                                        MSXML::IXMLDOMNodePtr xmlNode) {
//  const static std::wstring s_created(L"CREATED");
//  const static std::wstring s_modified(L"MODIFIED");
//  const static std::wstring s_position(L"POSITION");
//  const static std::wstring s_text(L"TEXT");
//
//  if (s_node == xmlNode->baseName) {
//    MSXML::IXMLDOMNamedNodeMapPtr pAttrMap = xmlNode->attributes;
//
//    // ID
//    MSXML::IXMLDOMNodePtr pIdNode = pAttrMap->getNamedItem(s_id);
//    touchmind::NODE_ID id = -1;
//    wchar_t *offset;
//    if (pIdNode != nullptr) {
//      std::wstring ws_id(pIdNode->text);
//      int numericStartIndex = _GetNumericStartIndex(ws_id);
//      if (numericStartIndex >= 0) {
//        std::wstring ws_id_mod = ws_id.substr(numericStartIndex, ws_id.length() - numericStartIndex);
//        id = wcstoul(ws_id_mod.c_str(), &offset, 10);
//      }
//    }
//    // CREATED
//    MSXML::IXMLDOMNodePtr pCreatedNode = pAttrMap->getNamedItem(s_created);
//    std::wstring ws_createdTime(pCreatedNode->text);
//    LONGLONG createdTime = static_cast<LONGLONG>(wcstod(ws_createdTime.c_str(), &offset));
//
//    // MODIFIED
//    MSXML::IXMLDOMNodePtr pModifiedNode = pAttrMap->getNamedItem(s_modified);
//    std::wstring ws_modifiedTime(pModifiedNode->text);
//
//    // TEXT
//    MSXML::IXMLDOMNodePtr pTextNode = pAttrMap->getNamedItem(s_text);
//
//    // POSITION
//    MSXML::IXMLDOMNodePtr pPositionNode = pAttrMap->getNamedItem(s_position);
//    touchmind::NODE_SIDE position = touchmind::NODE_SIDE_UNDEFINED;
//    if (pPositionNode != nullptr) {
//      if (s_positionRightValue == pPositionNode->text) {
//        position = touchmind::NODE_SIDE_RIGHT;
//      } else if (s_positionLeftValue == pPositionNode->text) {
//        position = touchmind::NODE_SIDE_LEFT;
//      }
//    }
//
//    if (id != -1) {
//      node->SetId(id);
//    }
//    node->SetCreatedTime(createdTime);
//    node->SetModifiedTime(createdTime);
//    node->SetText(std::wstring(pTextNode->text));
//    node->SetPosition(position);
//
//    MSXML::IXMLDOMNodeListPtr pNodeList = xmlNode->childNodes;
//    _OpenFromFreeMind_ParseNodeList(node, pNodeList);
//  }
//  return MapModelIOResult_OK;
//}

touchmind::model::MapModelIOResult touchmind::model::MapModel::OpenFromFreeMind(const std::wstring &fileName) {
  //const static std::wstring s_map(L"map");

  //_FireBeforeOpenEvent();
  //Clear();

  //pugi::xml_document xmlDoc;
  //auto result = xmlDoc.load_file(fileName.c_str());
  //if (!result) {
  //  SPDLOG_ERROR("Failed to load a XML file '{}'", fileName);
  //  return MapModelIOResult_InternalError;
  //}

  //auto rootNodeModel = touchmind::model::node::NodeModel::Create(m_pSelectionManager);
  //auto mapNode = xmlDoc.child(L"map");
  //if (!mapNode) {
  //  SPDLOG_ERROR("Could not find 'map' node");  
  //  return MapModelIOResult_FileFormatError;
  //}
  //
  //auto parseNode = [&](const pugi::xml_node &xmlNode) -> bool {
  //  

  //  for (auto childNode : xmlNode.children(L"node")) {
  //    auto ret = parseNode(childNode);
  //  }
  //};



  //    MSXML::IXMLDOMNodePtr pMapNode;
  //    pXMLDoc->get_firstChild(&pMapNode);
  //    if (s_map != pMapNode->baseName) {
  //      SPDLOG_ERROR("Root node of XML document must be 'map', but '{}'", pMapNode->baseName);
  //      return MapModelIOResult_FileFormatError;
  //    }
  //    MSXML::IXMLDOMNodeListPtr pNodeList = pMapNode->childNodes;
  //    for (int i = 0; i < pNodeList->length; ++i) {
  //      MSXML::IXMLDOMNodePtr pNode = pNodeList->item[i];
  //      if (s_node == pNode->baseName) {
  //        _OpenFromFreeMind_ParseNode(rootNodeModel, pNode);
  //        // we assume that map node has only one node
  //        break;
  //      }
  //    }

  //SetRootNodeModel(rootNodeModel);
  //_FireAfterOpenEvent();
  return MapModelIOResult_OK;
}

touchmind::model::MapModelIOResult touchmind::model::MapModel::Save() {
  return SaveAs(m_fileName);
}

touchmind::model::MapModelIOResult touchmind::model::MapModel::SaveAs(const std::wstring &fileName) {
  _FireBeforeSaveEvent();

  pugi::xml_document xmlDoc;

  // tmm
  auto tmmNode = xmlDoc.append_child(s_tmm);

  // version
  tmmNode.append_attribute(s_version) = L"1.0";

  // tree
  auto treeNode = tmmNode.append_child(s_tree);

  // root node
  auto node = treeNode.append_child(s_node);

  HRESULT hr = m_pNodeModelXMLEncoder->Encode(m_root, node);
  MapModelIOResult result = SUCCEEDED(hr) ? MapModelIOResult_OK : MapModelIOResult_InternalError;

  if (result == MapModelIOResult_OK) {
    bool ret = xmlDoc.save_file(fileName.c_str());
    if (!ret) {
      SPDLOG_ERROR(L"Failed to save as '{}'", fileName);
      return MapModelIOResult_FileIOError;
    }
  }

  m_fileName = fileName;
  _FireAfterSaveEvent();
  m_pFileOperation->SetSaveCounter(GetRootNodeModel());
  return MapModelIOResult_OK;
}

void touchmind::model::MapModel::BeginTransaction() {
  while (m_undo.size() > m_pConfiguration->GetMaxUndoCount()) {
    m_undo.pop_front();
  }

  std::shared_ptr<touchmind::model::node::NodeModel> copiedRoot;
  std::vector<std::shared_ptr<touchmind::model::link::LinkModel>> copiedLinks;
  _MakeDeepCopy(copiedRoot, copiedLinks);

  m_undo.push_back(UndoContainer(copiedRoot, copiedLinks));
}

void touchmind::model::MapModel::EndTransaction() {
}

bool touchmind::model::MapModel::Roleback() {
  if (m_undo.size() >= 1) {
    UndoContainer &undo = m_undo.back();
    m_root = undo.root;
    m_links.clear();
    m_links.resize(undo.links.size());
    std::copy(undo.links.begin(), undo.links.end(), m_links.begin());
    NormalizeLinks();
    m_root->RenumberIds();
    RenumberLinkIds();
    m_undo.pop_back();
  }
  return true;
}

void touchmind::model::MapModel::AddMapModelChangeListener(IMapModelChangeListener *pMapModelChangeListener) {
  m_mapModelChangeListeners.push_back(pMapModelChangeListener);
}

void touchmind::model::MapModel::RemoveMapModelChangeListener(IMapModelChangeListener *pMapModelChangeListener) {
  m_mapModelChangeListeners.erase(
      std::remove(m_mapModelChangeListeners.begin(), m_mapModelChangeListeners.end(), pMapModelChangeListener),
      m_mapModelChangeListeners.end());
}

void touchmind::model::MapModel::_FireBeforeNewEvent() {
  for (auto &l : m_mapModelChangeListeners) {
    l->BeforeNew(this);
  }
}

void touchmind::model::MapModel::_FireAfterNewEvent() {
  for (auto &l : m_mapModelChangeListeners) {
    l->AfterNew(this);
  }
}

void touchmind::model::MapModel::_FireBeforeOpenEvent() {
  for (auto &l : m_mapModelChangeListeners) {
    l->BeforeOpen(this);
  }
}

void touchmind::model::MapModel::_FireAfterOpenEvent() {
  for (auto &l : m_mapModelChangeListeners) {
    l->AfterOpen(this);
  }
}

void touchmind::model::MapModel::_FireBeforeSaveEvent() {
  for (auto &l : m_mapModelChangeListeners) {
    l->BeforeSave(this);
  }
}

void touchmind::model::MapModel::_FireAfterSaveEvent() {
  for (auto &l : m_mapModelChangeListeners) {
    l->AfterSave(this);
  }
}

void touchmind::model::MapModel::AddLink(const std::shared_ptr<touchmind::model::link::LinkModel> &linkModel) {
  ASSERT(linkModel->IsValid());
  if (linkModel->IsValid()) {
    m_links.push_back(linkModel);
    auto node1 = linkModel->GetNode(EDGE_ID_1);
    auto node2 = linkModel->GetNode(EDGE_ID_2);
    node1->AddLink(linkModel);
    node2->AddLink(linkModel);
  }
}

void touchmind::model::MapModel::AddLinks(
    const std::vector<std::shared_ptr<touchmind::model::link::LinkModel>> &links) {
  for (auto &link : links) {
    AddLink(link);
  }
}

void touchmind::model::MapModel::RemoveLink(const std::shared_ptr<touchmind::model::link::LinkModel> &linkModel) {
  auto node1 = linkModel->GetNode(EDGE_ID_1);
  node1->RemoveLink(linkModel);
  auto node2 = linkModel->GetNode(EDGE_ID_2);
  node2->RemoveLink(linkModel);
  m_links.erase(std::remove_if(m_links.begin(), m_links.end(),
                               [&](std::shared_ptr<touchmind::model::link::LinkModel> e) { return e == linkModel; }),
                m_links.end());
}

void touchmind::model::MapModel::NormalizeLinks() {
  for (auto &link : m_links) {
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
  m_links.erase(std::remove_if(m_links.begin(), m_links.end(),
                               [](const std::shared_ptr<touchmind::model::link::LinkModel> &link) -> bool {
                                 return !link->IsValid();
                               }),
                m_links.end());
}

void touchmind::model::MapModel::_MakeDeepCopy(
    std::shared_ptr<touchmind::model::node::NodeModel> &copiedRoot,
    std::vector<std::shared_ptr<touchmind::model::link::LinkModel>> &copiedLinks) {
  copiedRoot = m_root->DeepCopy();
  copiedLinks.clear();
  copiedLinks.resize(m_links.size());
  for (size_t i = 0; i < m_links.size(); ++i) {
    copiedLinks[i] = std::make_shared<touchmind::model::link::LinkModel>(*m_links[i]);
  }
}

void touchmind::model::MapModel::RenumberLinkIds() {
  for (size_t i = 0; i < m_links.size(); ++i) {
    m_links[i]->SetLinkId(model::link::LinkModel::GenerateLinkId());
  }
}
