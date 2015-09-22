#include "StdAfx.h"
#include "touchmind/util/ColorUtil.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/util/TimeUtil.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/node/iterator/ChildFirstNodeIterator.h"
#include "touchmind/model/node/iterator/ParentFirstNodeIterator.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/selection/SelectionManager.h"
#include "touchmind/selection/DefaultPropertyModel.h"

touchmind::NODE_ID touchmind::model::node::NodeModel::s_masterId = 0;

const FLOAT touchmind::model::node::NodeModel::DEFAULT_WIDTH = 128.0f;
const FLOAT touchmind::model::node::NodeModel::DEFAULT_HEIGHT = 48.0f;

std::shared_ptr<touchmind::model::node::NodeModel>
touchmind::model::node::NodeModel::Create(touchmind::selection::SelectionManager *pSelectionManager) {
  ASSERT(pSelectionManager != nullptr);
  auto node = std::make_shared<NodeModel>();
  node->SetSelectionManager(pSelectionManager);

  auto defaultProp = pSelectionManager->GetDefaultPropertyModel();
  node->SetBackgroundColor(defaultProp->UpdateProperty_GetNodeBackgroundColor());
  node->SetNodeShape(defaultProp->UpdateProperty_GetNodeShape());

  auto path = node->GetPathModel();
  path->SetSelectionManager(pSelectionManager);
  path->SetColor(defaultProp->UpdateProperty_GetLineColor());
  path->SetWidth(defaultProp->UpdateProperty_GetLineWidth());
  path->SetStyle(defaultProp->UpdateProperty_GetLineStyle());
  return node;
}

touchmind::model::node::NodeModel::NodeModel()
    : BaseModel()
    , SelectableSupport()
    , m_id(s_masterId++)
    , m_position(NODE_SIDE_UNDEFINED)
    , m_text()
    ,
    // m_createdTime, // initialize later
    // m_modifiedTime, // initialize later
    m_children()
    , m_parent()
    , // null weak_ptr
    m_collapsed(false)
    , m_canCollapsed(true)
    , m_x(0.0f)
    , m_y(0.0f)
    , m_width(DEFAULT_WIDTH)
    , m_height(DEFAULT_HEIGHT)
    , m_backgroundColor(D2D1::ColorF(D2D1::ColorF::White))
    , m_nodeShape(NODE_SHAPE_ROUNDED_RECTANGLE)
    , m_fontAttributes()
    , m_links()
    , m_pathModel(std::make_shared<touchmind::model::path::PathModel>()) {
  touchmind::util::TimeUtil::InitializeSystemTime(&m_createdTime);
  touchmind::util::TimeUtil::InitializeSystemTime(&m_modifiedTime);

  AddSelectedListener([&]() { IncrementRepaintRequiredCounter(); });
  AddUnselectedListener([&]() { IncrementRepaintRequiredCounter(); });
}

touchmind::model::node::NodeModel::NodeModel(const NodeModel &other)
    : BaseModel(other)
    , SelectableSupport(other)
    , m_id(other.m_id)
    , m_position(other.m_position)
    , m_text(other.m_text)
    , m_createdTime(other.m_createdTime)
    , m_modifiedTime(other.m_modifiedTime)
    , m_children(other.m_children)
    , m_parent(m_parent)
    , m_collapsed(other.m_collapsed)
    , m_canCollapsed(other.m_canCollapsed)
    , m_x(other.m_x)
    , m_y(other.m_y)
    , m_width(other.m_width)
    , m_height(other.m_height)
    , m_fontAttributes(other.m_fontAttributes)
    , m_backgroundColor(other.m_backgroundColor)
    , m_links()
    , // do not copy
    m_nodeShape(other.m_nodeShape)
    , m_pathModel() // do not copy
{
  m_pathModel = std::make_shared<path::PathModel>(*other.m_pathModel);
  AddSelectedListener([&]() { IncrementRepaintRequiredCounter(); });
  AddUnselectedListener([&]() { IncrementRepaintRequiredCounter(); });
}

touchmind::model::node::NodeModel::NodeModel(NodeModel &&other)
    : BaseModel(other)
    , SelectableSupport(other)
    , m_id(other.m_id)
    , m_position(other.m_position)
    , m_text(std::move(other.m_text))
    , m_createdTime(other.m_createdTime)
    , m_modifiedTime(other.m_modifiedTime)
    , m_children(std::move(other.m_children))
    , m_parent(m_parent)
    , m_collapsed(other.m_collapsed)
    , m_canCollapsed(other.m_canCollapsed)
    , m_x(other.m_x)
    , m_y(other.m_y)
    , m_width(other.m_width)
    , m_height(other.m_height)
    , m_fontAttributes(std::move(other.m_fontAttributes))
    , m_backgroundColor(other.m_backgroundColor)
    , m_links()
    , // do not copy
    m_nodeShape(other.m_nodeShape)
    , m_pathModel() // do not copy
{
  m_pathModel = std::make_shared<path::PathModel>(*other.m_pathModel);
  AddSelectedListener([&]() { IncrementRepaintRequiredCounter(); });
  AddUnselectedListener([&]() { IncrementRepaintRequiredCounter(); });
}

touchmind::model::node::NodeModel &touchmind::model::node::NodeModel::operator=(const NodeModel &other) {
  if (this != &other) {
    m_id = other.m_id;
    m_position = other.m_position;
    m_text = other.m_text;
    m_createdTime = other.m_createdTime;
    m_modifiedTime = other.m_modifiedTime;
    m_children = other.m_children;
    m_parent = other.m_parent;
    m_collapsed = other.m_collapsed;
    m_canCollapsed = other.m_canCollapsed;
    m_x = other.m_x;
    m_y = other.m_y;
    m_width = other.m_width;
    m_height = other.m_height;
    m_fontAttributes = other.m_fontAttributes;
    m_backgroundColor = other.m_backgroundColor;
    // m_links = other.m_links; // do not copy
    m_nodeShape = other.m_nodeShape;
    m_pathModel = other.m_pathModel;
    IncrementRepaintRequiredCounter();
    IncrementSaveRequiredCounter();
  }
  return *this;
}

touchmind::model::node::NodeModel &touchmind::model::node::NodeModel::
operator=(touchmind::model::node::NodeModel &&other) {
  if (this != &other) {
    m_id = other.m_id;
    m_position = other.m_position;
    m_text = std::move(other.m_text);
    m_createdTime = other.m_createdTime;
    m_modifiedTime = other.m_modifiedTime;
    m_children = std::move(other.m_children);
    m_parent = other.m_parent;
    m_collapsed = other.m_collapsed;
    m_canCollapsed = other.m_canCollapsed;
    m_x = other.m_x;
    m_y = other.m_y;
    m_width = other.m_width;
    m_height = other.m_height;
    m_fontAttributes = std::move(other.m_fontAttributes);
    m_backgroundColor = other.m_backgroundColor;
    // m_links = other.m_links; // do not copy
    m_nodeShape = other.m_nodeShape;
    m_pathModel = other.m_pathModel;
    IncrementRepaintRequiredCounter();
    IncrementSaveRequiredCounter();
  }
  return *this;
}

touchmind::model::node::NodeModel::~NodeModel(void) {
}

bool touchmind::model::node::NodeModel::operator==(const NodeModel &other) const {
  return m_id == other.m_id && m_position == other.m_position && m_text == other.m_text
         && touchmind::util::TimeUtil::CompareSystemTimes(&m_createdTime, &other.m_createdTime) == 0
         && touchmind::util::TimeUtil::CompareSystemTimes(&m_modifiedTime, &other.m_modifiedTime) == 0;
}

void touchmind::model::node::NodeModel::SetId(touchmind::NODE_ID nodeId) {
  if (m_id == nodeId) {
    return;
  }
  m_id = nodeId;
  if (m_id > s_masterId) {
    s_masterId = m_id + 1;
  }
  IncrementRepaintRequiredCounter();
  IncrementSaveRequiredCounter();
}

void touchmind::model::node::NodeModel::SetPosition(NODE_SIDE position) {
  if (m_position == position) {
    return;
  }
  m_position = position;
  IncrementRepaintRequiredCounter();
  IncrementSaveRequiredCounter();
  UpdateLinkStatus_RepaintRequired();
}

touchmind::NODE_SIDE touchmind::model::node::NodeModel::GetAncestorPosition(void) const {
  if (GetPosition() != NODE_SIDE_UNDEFINED) {
    return GetPosition();
  } else {
    if (GetParent() != nullptr) {
      return GetParent()->GetAncestorPosition();
    } else {
      return NODE_SIDE_UNDEFINED;
    }
  }
}

void touchmind::model::node::NodeModel::SetText(const std::wstring &text) {
  if (m_text == text) {
    return;
  }
  m_text = text;
  IncrementRepaintRequiredCounter();
  IncrementSaveRequiredCounter();
}

void touchmind::model::node::NodeModel::SetCreatedTime(const SYSTEMTIME &createdTime) {
  if (touchmind::util::TimeUtil::CompareSystemTimes(&m_createdTime, &createdTime) == 0) {
    return;
  }
  m_createdTime = createdTime;
  IncrementRepaintRequiredCounter();
  IncrementSaveRequiredCounter();
}

void touchmind::model::node::NodeModel::SetCreatedTime(LONGLONG javaTime) {
  SYSTEMTIME createdTime;
  touchmind::util::TimeUtil::JavaTimeToSystemTime(javaTime, &createdTime);
  if (touchmind::util::TimeUtil::CompareSystemTimes(&m_createdTime, &createdTime) == 0) {
    return;
  }
  m_createdTime = createdTime;
  IncrementRepaintRequiredCounter();
  IncrementSaveRequiredCounter();
}

LONGLONG touchmind::model::node::NodeModel::GetCreatedTimeAsJavaTime() const {
  LONGLONG javaTime;
  touchmind::util::TimeUtil::SystemTimeToJavaTime(m_createdTime, &javaTime);
  return javaTime;
}

time_t touchmind::model::node::NodeModel::GetCreatedTimeAsUnixTime() const {
  time_t t;
  touchmind::util::TimeUtil::SystemTimeToUnixTime(m_createdTime, &t);
  return t;
}

void touchmind::model::node::NodeModel::SetModifiedTime(const SYSTEMTIME &modifiedTime) {
  if (touchmind::util::TimeUtil::CompareSystemTimes(&m_modifiedTime, &modifiedTime) == 0) {
    return;
  }
  m_modifiedTime = modifiedTime;
  IncrementRepaintRequiredCounter();
  IncrementSaveRequiredCounter();
}

void touchmind::model::node::NodeModel::SetModifiedTime(LONGLONG javaTime) {
  SYSTEMTIME modifiedTime;
  touchmind::util::TimeUtil::JavaTimeToSystemTime(javaTime, &modifiedTime);
  if (touchmind::util::TimeUtil::CompareSystemTimes(&m_modifiedTime, &modifiedTime) == 0) {
    return;
  }
  m_modifiedTime = modifiedTime;
  IncrementRepaintRequiredCounter();
  IncrementSaveRequiredCounter();
}

LONGLONG touchmind::model::node::NodeModel::GetModifiedTimeAsJavaTime() const {
  LONGLONG javaTime;
  touchmind::util::TimeUtil::SystemTimeToJavaTime(m_modifiedTime, &javaTime);
  return javaTime;
}

time_t touchmind::model::node::NodeModel::GetModifiedTimeAsUnixTime() const {
  time_t t;
  touchmind::util::TimeUtil::SystemTimeToUnixTime(m_modifiedTime, &t);
  return t;
}

void touchmind::model::node::NodeModel::SetParent(const std::shared_ptr<touchmind::model::node::NodeModel> &parent) {
  if (!m_parent.expired() && m_parent.lock() == parent) {
    return;
  }
  m_parent = parent;
  IncrementRepaintRequiredCounter();
  IncrementSaveRequiredCounter();
}

void touchmind::model::node::NodeModel::AddChild(const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  if (std::find(m_children.begin(), m_children.end(), node) != m_children.end()) {
    return;
  }
  m_children.push_back(node);
  node->SetParent(shared_from_this());
  IncrementRepaintRequiredCounter();
  IncrementSaveRequiredCounter();
}

void touchmind::model::node::NodeModel::RemoveChild(const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  size_t previousCounts = m_children.size();
  m_children.erase(std::remove(m_children.begin(), m_children.end(), node), m_children.end());
  if (previousCounts != m_children.size()) {
    IncrementRepaintRequiredCounter();
    IncrementSaveRequiredCounter();
  }
}

void touchmind::model::node::NodeModel::RemoveAllChildren(void) {
  m_children.clear();
  IncrementRepaintRequiredCounter();
  IncrementSaveRequiredCounter();
}

std::shared_ptr<touchmind::model::node::NodeModel>
touchmind::model::node::NodeModel::_FindByNodeId(const std::shared_ptr<touchmind::model::node::NodeModel> &node,
                                                 touchmind::NODE_ID nodeId) {
  if (nodeId == node->GetId()) {
    return node;
  }
  for (size_t i = 0; i < node->GetActualChildrenCount(); ++i) {
    auto child = node->GetChild(i);
    auto result = _FindByNodeId(child, nodeId);
    if (result != nullptr) {
      return result;
    }
  }
  return nullptr;
}

std::shared_ptr<touchmind::model::node::NodeModel>
touchmind::model::node::NodeModel::FindByNodeId(touchmind::NODE_ID nodeId) {
  return _FindByNodeId(shared_from_this(), nodeId);
}

bool touchmind::model::node::NodeModel::IsAncestorCollapsed() const {
  if (GetParent() != nullptr && GetParent()->IsCollapsed()) {
    return true;
  } else {
    if (GetParent() == nullptr) {
      return false;
    } else {
      return GetParent()->IsAncestorCollapsed();
    }
  }
}

void touchmind::model::node::NodeModel::SetAncestorsExpanded() {
  if (GetParent() == nullptr) {
  } else {
    GetParent()->SetCollapsed(false);
    GetParent()->SetAncestorsExpanded();
  }
  IncrementRepaintRequiredCounter();
}

std::shared_ptr<touchmind::model::node::NodeModel> touchmind::model::node::NodeModel::GetLeftSibling() {
  if (GetParent() == nullptr) {
    return nullptr;
  } else {
    auto parent = GetParent();
    for (auto it = parent->m_children.begin(); it != parent->m_children.end(); ++it) {
      if ((*it)->GetId() == GetId()) {
        if (it == parent->m_children.begin()) {
          return nullptr;
        } else {
          return *(it - 1);
        }
      }
    }
    return nullptr;
  }
}

std::shared_ptr<touchmind::model::node::NodeModel> touchmind::model::node::NodeModel::GetRightSibling() {
  if (GetParent() == nullptr) {
    return nullptr;
  } else {
    auto parent = GetParent();
    for (auto it = parent->m_children.begin(); it != parent->m_children.end(); ++it) {
      if ((*it)->GetId() == GetId()) {
        if (it == (parent->m_children.end() - 1)) {
          return nullptr;
        } else {
          return *(it + 1);
        }
      }
    }
    return nullptr;
  }
}

std::shared_ptr<touchmind::model::node::NodeModel> touchmind::model::node::NodeModel::GetFirstChild() {
  if (m_children.size() >= 1) {
    return m_children[0];
  }
  return nullptr;
}

std::shared_ptr<touchmind::model::node::NodeModel> touchmind::model::node::NodeModel::GetLastChild() {
  if (m_children.size() >= 1) {
    return m_children[m_children.size() - 1];
  }
  return nullptr;
}

void touchmind::model::node::NodeModel::SetCollapsed(bool collapsed) {
  if (m_collapsed == collapsed) {
    return;
  }
  if (collapsed && GetParent() == nullptr) {
    return;
  }
  if (collapsed && GetActualChildrenCount() == 0) {
    return;
  }
  if (collapsed && !CanCollapsed()) {
    return;
  }
  m_collapsed = collapsed;
  IncrementRepaintRequiredCounter();
}

bool touchmind::model::node::NodeModel::CanCollapsed() const {
  if (GetParent() == nullptr) {
    return false;
  }
  if (GetActualChildrenCount() == 0) {
    return false;
  }
  return m_canCollapsed;
}

void touchmind::model::node::NodeModel::SetCanCollapsed(bool canCollapsed) {
  if (GetParent() == nullptr) {
    return;
  }
  if (GetActualChildrenCount() == 0) {
    return;
  }
  m_canCollapsed = canCollapsed;
  IncrementRepaintRequiredCounter();
  IncrementSaveRequiredCounter();
}

bool touchmind::model::node::NodeModel::IsAncestor(
    const std::shared_ptr<touchmind::model::node::NodeModel> &other) const {
  std::shared_ptr<const NodeModel> node = shared_from_this();

  while (true) {
    if (other == node) {
      return true;
    }
    node = node->GetParent();
    if (node == nullptr) {
      return false;
    }
  }
}

void _DeepCopy(std::shared_ptr<touchmind::model::node::NodeModel> srcParent,
               std::shared_ptr<touchmind::model::node::NodeModel> destParent) {
  for (size_t i = 0; i < srcParent->GetActualChildrenCount(); ++i) {
    auto srcChild = srcParent->GetChild(i);
    auto destChild = touchmind::model::node::NodeModel::Create(*srcChild.get());
    destChild->RemoveAllChildren();
    destParent->AddChild(destChild);
    _DeepCopy(srcChild, destChild);
  }
}

std::shared_ptr<touchmind::model::node::NodeModel> touchmind::model::node::NodeModel::DeepCopy() {
  auto src(shared_from_this());
  auto dest = NodeModel::Create(*src.get());
  dest->RemoveAllChildren();

  _DeepCopy(src, dest);

  return dest;
}

bool touchmind::model::node::NodeModel::Compare(const std::shared_ptr<touchmind::model::node::NodeModel> &other) {
  return m_id == other->m_id && m_position == other->m_position && m_text == other->m_text
         && touchmind::util::TimeUtil::CompareSystemTimes(&m_createdTime, &other->m_createdTime) == 0
         && touchmind::util::TimeUtil::CompareSystemTimes(&m_modifiedTime, &other->m_modifiedTime) == 0
         && m_collapsed == other->m_collapsed && m_canCollapsed == other->m_canCollapsed && m_x == other->m_x
         && m_y == other->m_y && m_width == other->m_width && m_height == other->m_height;
}

bool _DeepCompare(const std::shared_ptr<touchmind::model::node::NodeModel> &node1,
                  const std::shared_ptr<touchmind::model::node::NodeModel> &node2) {
  if (node1->GetActualChildrenCount() != node2->GetActualChildrenCount()) {
    return false;
  }
  for (size_t i = 0; i < node1->GetActualChildrenCount(); ++i) {
    auto node1Child = node1->GetChild(i);
    auto node2Child = node2->GetChild(i);
    if (!node1Child->Compare(node2Child)) {
      return false;
    }
    _DeepCompare(node1Child, node2Child);
  }
  return true;
}

bool touchmind::model::node::NodeModel::DeepCompare(const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  if (shared_from_this()->Compare(node)) {
    return _DeepCompare(shared_from_this(), node);
  } else {
    return false;
  }
}

std::wostream &touchmind::model::node::NodeModel::Dump(std::wostream &out) {
  out << *this;
  return out;
}

void touchmind::model::node::NodeModel::_Indent(std::wostream &out, int indent) {
  for (int i = 0; i < indent; ++i) {
    out << "  ";
  }
}

void touchmind::model::node::NodeModel::_Dump(std::wostream &out, int indent,
                                              const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  _Indent(out, indent);
  node->Dump(out);
  out << std::endl;
  for (size_t i = 0; i < node->GetActualChildrenCount(); ++i) {
    auto child = node->GetChild(i);
    _Dump(out, indent + 1, child);
  }
}

std::wostream &touchmind::model::node::NodeModel::DumpAll(std::wostream &out) {
  out << "NodeModel::DumpAll start <<" << std::endl;
  _Dump(out, 0, shared_from_this());
  out << "NodeModel::DumpAll end >>" << std::endl;
  return out;
}

bool touchmind::model::node::NodeModel::IsDescendantSaveRequired(SAVE_COUNTER saveCouner) const {
  if (IsSaveRequired(saveCouner)) {
    return true;
  } else {
    for (size_t i = 0; i < GetActualChildrenCount(); ++i) {
      auto child = GetChild(i);
      if (child->IsDescendantSaveRequired(saveCouner)) {
        return true;
      }
    }
  }
  return false;
}

class RenumberIdsVisitor {
public:
  touchmind::VISITOR_RESULT operator()(std::shared_ptr<touchmind::model::node::NodeModel> node) {
    node->SetId(node->GenerateId());
    return touchmind::VISITOR_RESULT_CONTINUE;
  }
};

void touchmind::model::node::NodeModel::RenumberIds() {
  RenumberIdsVisitor visitor;
  this->ApplyVisitor(visitor);
}

void touchmind::model::node::NodeModel::RemoveLink(const std::weak_ptr<touchmind::model::link::LinkModel> &link) {
  m_links.erase(
      std::remove_if(m_links.begin(), m_links.end(), [&](std::weak_ptr<touchmind::model::link::LinkModel> e) {
        return !link.expired() && !e.expired() && link.lock()->GetLinkId() == e.lock()->GetLinkId();
      }), m_links.end());
}

void touchmind::model::node::NodeModel::UpdateLinkStatus_RepaintRequired() {
  std::for_each(m_links.begin(), m_links.end(), [](std::weak_ptr<touchmind::model::link::LinkModel> _link) {
    if (!_link.expired()) {
      auto link = _link.lock();
      link->IncrementRepaintRequiredCounter();
      link->GetEdge(EDGE_ID_1)->IncrementRepaintRequiredCounter();
      link->GetEdge(EDGE_ID_2)->IncrementRepaintRequiredCounter();
    }
  });
}

void touchmind::model::node::NodeModel::UpdateLinkStatus_SaveRequired() {
  std::for_each(m_links.begin(), m_links.end(), [](std::weak_ptr<touchmind::model::link::LinkModel> _link) {
    if (!_link.expired()) {
      auto link = _link.lock();
      link->IncrementSaveRequiredCounter();
      link->GetEdge(EDGE_ID_1)->IncrementSaveRequiredCounter();
      link->GetEdge(EDGE_ID_2)->IncrementSaveRequiredCounter();
    }
  });
}

void touchmind::model::node::NodeModel::UpdatePathStatus_RepaintRequired() {
  m_pathModel->IncrementRepaintRequiredCounter();
}

touchmind::model::node::iterator::ChildFirstNodeIterator touchmind::model::node::NodeModel::Begin_ChildFirst() {
  return touchmind::model::node::iterator::ChildFirstNodeIterator(shared_from_this(), false);
}

touchmind::model::node::iterator::ChildFirstNodeIterator touchmind::model::node::NodeModel::End_ChildFirst() {
  return touchmind::model::node::iterator::ChildFirstNodeIterator(shared_from_this(), true);
}

touchmind::model::node::iterator::ParentFirstNodeIterator touchmind::model::node::NodeModel::Begin_ParentFirst() {
  return touchmind::model::node::iterator::ParentFirstNodeIterator(shared_from_this(), false);
}

touchmind::model::node::iterator::ParentFirstNodeIterator touchmind::model::node::NodeModel::End_ParentFirst() {
  return touchmind::model::node::iterator::ParentFirstNodeIterator(shared_from_this(), true);
}

std::shared_ptr<touchmind::selection::SelectableSupport> touchmind::model::node::NodeModel::GetSelectableSharedPtr() {
  return std::dynamic_pointer_cast<touchmind::selection::SelectableSupport>(shared_from_this());
}

std::wostream &touchmind::model::node::operator<<(std::wostream &os, touchmind::model::node::NodeModel const &node) {
  std::wstring createdTime;
  touchmind::SystemtimeToString(&node.GetCreatedTime(), createdTime);
  std::wstring modifiedTime;
  touchmind::SystemtimeToString(&node.GetModifiedTime(), modifiedTime);

  os << L"NodeModel" << L"[id=" << node.m_id << L",position=" << node.m_position << L",text=" << node.m_text
     << L",createdTime=" << createdTime << L",modifiedTime=" << modifiedTime << L",collapsed=" << node.m_collapsed
     << L",canCollapsed=" << node.m_canCollapsed << L",x=" << node.m_x << L",y=" << node.m_y << L",width="
     << node.m_width << L",height=" << node.m_height << L",backgroundColor=" << node.m_backgroundColor;
  os << L",fontAttributes={";
  for (size_t i = 0; i < node.GetFontAttributeCount(); ++i) {
    os << node.GetFontAttribute(i);
    if (i != (node.GetFontAttributeCount() - 1)) {
      os << std::endl;
    }
  }
  os << L"}]";
  return os;
}
