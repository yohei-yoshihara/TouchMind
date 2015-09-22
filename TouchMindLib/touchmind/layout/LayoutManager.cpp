#include "StdAfx.h"
#include "touchmind/Configuration.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/layout/LayoutManager.h"
#include "touchmind/model/Insets.h"

class TreeTransformVisitor {
private:
  FLOAT m_offsetX;
  FLOAT m_offsetY;
  FLOAT m_zoomX;
  FLOAT m_zoomY;

public:
  TreeTransformVisitor(FLOAT offsetX, FLOAT offsetY, FLOAT zoomX, FLOAT zoomY)
      : m_offsetX(offsetX)
      , m_offsetY(offsetY)
      , m_zoomX(zoomX)
      , m_zoomY(zoomY) {
  }
  touchmind::VISITOR_RESULT operator()(std::shared_ptr<touchmind::model::node::NodeModel> node) {
    node->SetX((node->GetX() + m_offsetX) * m_zoomX);
    node->SetY((node->GetY() + m_offsetY) * m_zoomY);
    return touchmind::VISITOR_RESULT_CONTINUE;
  }
};

class VMLOutVisitor {
private:
  std::wostream &out;
  FLOAT offsetX;
  FLOAT offsetY;

public:
  VMLOutVisitor(std::wostream &_out, FLOAT _offsetX, FLOAT _offsetY)
      : out(_out)
      , offsetX(_offsetX)
      , offsetY(_offsetY) {
  }
  touchmind::VISITOR_RESULT operator()(std::shared_ptr<touchmind::model::node::NodeModel> node) {
    static wchar_t *colors[] = {L"red", L"green", L"yellow", L"blue", L"orange", L"gray"};
    out << L"<!-- " << node->GetText() << L" -->" << std::endl;
    out << L"<v:rect style=\"position:absolute;margin-top:" << (node->GetY() + offsetY) << L";margin-left:"
        << (node->GetX() + offsetX) << L";width:" << node->GetWidth() << L";height:" << node->GetHeight()
        << L";z-index:2\" fillcolor=\"" << colors[node->GetLevel()]
        << L"\" strokecolor=\"black\" strokeweight=\"1pt\"/>" << std::endl;
    return touchmind::VISITOR_RESULT_CONTINUE;
  }
};

touchmind::layout::LayoutWorkData *
touchmind::layout::LayoutManager::_GetLayoutWorkData(const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  ASSERT(m_layoutWorkDataMap.count(node->GetId()) > 0);
  return &m_layoutWorkDataMap[node->GetId()];
}

touchmind::layout::LayoutManager::LayoutManager(void)
    : m_pTextSizeProvider(nullptr)
    , m_node()
    , m_configuration(nullptr)
    , m_layoutWorkDataMap() {
}

touchmind::layout::LayoutManager::~LayoutManager(void) {
}

void touchmind::layout::LayoutManager::Arrange() {
  m_layoutWorkDataMap.clear();
  mw_maxLevelHeight.clear();
  mw_maxLevelWidth.clear();
  mw_previousLevelNode.clear();
  mw_rootXOffset = 0.0f;
  mw_rootYOffset = 0.0f;
  AssignPosition();
  _CalculateSize();
  PositionTree();
}

void touchmind::layout::LayoutManager::AssignPosition() {
  int numberOfRightPositions = 0;
  int numberOfLeftPositions = 0;

  for (size_t i = 0; i < m_node->GetChildrenCount(); ++i) {
    std::shared_ptr<touchmind::model::node::NodeModel> child = m_node->GetChild(i);
    if (child->GetPosition() == touchmind::NODE_SIDE_RIGHT) {
      numberOfRightPositions++;
    } else if (child->GetPosition() == touchmind::NODE_SIDE_LEFT) {
      numberOfLeftPositions++;
    }
  }

  for (size_t i = 0; i < m_node->GetChildrenCount(); ++i) {
    std::shared_ptr<touchmind::model::node::NodeModel> child = m_node->GetChild(i);
    if (child->GetPosition() == touchmind::NODE_SIDE_UNDEFINED) {
      if (numberOfRightPositions <= numberOfLeftPositions) {
        child->SetPosition(touchmind::NODE_SIDE_RIGHT);
        numberOfRightPositions++;
      } else {
        child->SetPosition(touchmind::NODE_SIDE_LEFT);
        numberOfLeftPositions++;
      }
    }
  }
}

void touchmind::layout::LayoutManager::_CalculateSize(const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  FLOAT width, height;
  m_pTextSizeProvider->Calculate(node, &width, &height);
  node->SetWidth(width + m_configuration->GetInsets().left + m_configuration->GetInsets().right);
  node->SetHeight(height + m_configuration->GetInsets().top + m_configuration->GetInsets().bottom);

  if (node != nullptr) {
    for (size_t i = 0; i < node->GetChildrenCount(); ++i) {
      std::shared_ptr<touchmind::model::node::NodeModel> child = node->GetChild(i);
      _CalculateSize(child);
    }
  }
}

void touchmind::layout::LayoutManager::_CalculateSize() {
  _CalculateSize(m_node);
}

void touchmind::layout::LayoutManager::SetLevelHeight(const std::shared_ptr<touchmind::model::node::NodeModel> &node,
                                                      int level) {
  if (mw_maxLevelHeight.count(level) == 0) {
    mw_maxLevelHeight[level] = 0.0f;
  }
  if (mw_maxLevelHeight[level] < node->GetHeight()) {
    mw_maxLevelHeight[level] = node->GetHeight();
  }
}

void touchmind::layout::LayoutManager::SetLevelWidth(const std::shared_ptr<touchmind::model::node::NodeModel> &node,
                                                     int level) {
  if (mw_maxLevelWidth.count(level) == 0) {
    mw_maxLevelWidth[level] = 0.0f;
  }
  if (mw_maxLevelWidth[level] < node->GetWidth()) {
    mw_maxLevelWidth[level] = node->GetWidth();
  }
}

void touchmind::layout::LayoutManager::SetNeighbors(const std::shared_ptr<touchmind::model::node::NodeModel> &node,
                                                    int level) {
  if (mw_previousLevelNode.count(level) == 0) {
    _GetLayoutWorkData(node)->pLeftNeighbor = nullptr;
  } else {
    _GetLayoutWorkData(node)->pLeftNeighbor = mw_previousLevelNode[level];
    _GetLayoutWorkData(_GetLayoutWorkData(node)->pLeftNeighbor)->pRightNeighbor = node;
  }
  mw_previousLevelNode[level] = node;
}

FLOAT touchmind::layout::LayoutManager::GetChildrenCenter(
    const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  auto firstNode = node->GetFirstChild();
  auto lastNode = node->GetLastChild();
  LayoutWorkData *tempDataOfNode = _GetLayoutWorkData(firstNode);
  LayoutWorkData *tempDataOfNode1 = _GetLayoutWorkData(lastNode);
  FLOAT result
      = tempDataOfNode->prelim + ((tempDataOfNode1->prelim - tempDataOfNode->prelim) + GetNodeSize(lastNode)) / 2;
  return result;
}

void touchmind::layout::LayoutManager::FirstWalk(const std::shared_ptr<touchmind::model::node::NodeModel> &tree,
                                                 const std::shared_ptr<touchmind::model::node::NodeModel> &node,
                                                 int level) {
  std::shared_ptr<touchmind::model::node::NodeModel> pLeftSibling = nullptr;
  node->SetX(0.0f);
  node->SetY(0.0f);
  LayoutWorkData *pTempDataOfNode = _GetLayoutWorkData(node);
  pTempDataOfNode->prelim = 0.0f;
  pTempDataOfNode->modifier = 0.0f;
  pTempDataOfNode->pLeftNeighbor = nullptr;
  pTempDataOfNode->pRightNeighbor = nullptr;
  SetLevelHeight(node, level);
  SetLevelWidth(node, level);
  SetNeighbors(node, level);
  if (node->GetChildrenCount() == 0 || level == m_configuration->GetMaxDepth()) {
    pLeftSibling = node->GetLeftSibling();
    if (pLeftSibling != nullptr) {
      LayoutWorkData *pTempDataOfLeftSibling = _GetLayoutWorkData(pLeftSibling);
      pTempDataOfNode->prelim
          = pTempDataOfLeftSibling->prelim + GetNodeSize(pLeftSibling) + m_configuration->GetSiblingSeparation();
    } else {
      pTempDataOfNode->prelim = 0.0f;
    }
  } else {
    for (size_t i = 0; i < node->GetChildrenCount(); ++i) {
      std::shared_ptr<touchmind::model::node::NodeModel> pChild = node->GetChild(i);
      FirstWalk(tree, pChild, level + 1);
    }
    FLOAT midPoint = GetChildrenCenter(node);
    midPoint -= GetNodeSize(node) / 2;
    pLeftSibling = node->GetLeftSibling();
    if (pLeftSibling != nullptr) {
      LayoutWorkData *pTempDataOfLeftSibling = _GetLayoutWorkData(pLeftSibling);
      pTempDataOfNode->prelim
          = pTempDataOfLeftSibling->prelim + GetNodeSize(pLeftSibling) + m_configuration->GetSiblingSeparation();
      pTempDataOfNode->modifier = pTempDataOfNode->prelim - midPoint;
      Apportion(tree, node, level);
    } else {
      pTempDataOfNode->prelim = midPoint;
    }
  }
}

std::shared_ptr<touchmind::model::node::NodeModel>
touchmind::layout::LayoutManager::GetLeftMost(const std::shared_ptr<touchmind::model::node::NodeModel> &node, int level,
                                              int maxLevel) const {
  if (level >= maxLevel) {
    return node;
  }
  if (node->GetChildrenCount() == 0) {
    return nullptr;
  }

  size_t n = node->GetChildrenCount();
  for (size_t i = 0; i < n; ++i) {
    std::shared_ptr<touchmind::model::node::NodeModel> child = node->GetChild(i);
    std::shared_ptr<touchmind::model::node::NodeModel> leftmostDescendant = GetLeftMost(child, level + 1, maxLevel);
    if (leftmostDescendant != nullptr) {
      return leftmostDescendant;
    }
  }
  return nullptr;
}

void touchmind::layout::LayoutManager::Apportion(const std::shared_ptr<touchmind::model::node::NodeModel> &tree,
                                                 const std::shared_ptr<touchmind::model::node::NodeModel> &node,
                                                 int level) {
  UNREFERENCED_PARAMETER(tree);

  std::shared_ptr<touchmind::model::node::NodeModel> pFirstChild = node->GetFirstChild();
  std::shared_ptr<touchmind::model::node::NodeModel> pFirstChildLeftNeighbor
      = _GetLayoutWorkData(pFirstChild)->pLeftNeighbor;
  int j = 1;
  for (int k = m_configuration->GetMaxDepth() - level;
       pFirstChild != nullptr && pFirstChildLeftNeighbor != nullptr && j <= k;) {
    FLOAT modifierSumRight = 0.0f;
    FLOAT modifierSumLeft = 0.0f;
    std::shared_ptr<touchmind::model::node::NodeModel> pRightAncestor = pFirstChild;
    std::shared_ptr<touchmind::model::node::NodeModel> pLeftAncestor = pFirstChildLeftNeighbor;
    for (int l = 0; l < j; ++l) {
      pRightAncestor = pRightAncestor->GetParent();
      pLeftAncestor = pLeftAncestor->GetParent();
      modifierSumRight += _GetLayoutWorkData(pRightAncestor)->modifier;
      modifierSumLeft += _GetLayoutWorkData(pLeftAncestor)->modifier;
    }

    FLOAT totalGap = (_GetLayoutWorkData(pFirstChildLeftNeighbor)->prelim + modifierSumLeft
                      + GetNodeSize(pFirstChildLeftNeighbor) + m_configuration->GetSubtreeSeparation())
                     - (_GetLayoutWorkData(pFirstChild)->prelim + modifierSumRight);
    if (totalGap > 0) {
      std::shared_ptr<touchmind::model::node::NodeModel> subTreeAux = node;
      int numSubtrees = 0;
      for (; subTreeAux != nullptr && subTreeAux != pLeftAncestor; subTreeAux = subTreeAux->GetLeftSibling()) {
        numSubtrees++;
      }
      if (subTreeAux != nullptr) {
        std::shared_ptr<touchmind::model::node::NodeModel> subtreeMoveAux = node;
        FLOAT singleGap = totalGap / numSubtrees;
        for (; subtreeMoveAux != pLeftAncestor; subtreeMoveAux = subtreeMoveAux->GetLeftSibling()) {
          _GetLayoutWorkData(subtreeMoveAux)->prelim += totalGap;
          _GetLayoutWorkData(subtreeMoveAux)->modifier += totalGap;
          totalGap -= singleGap;
        }
      }
    }
    j++;
    if (pFirstChild->GetChildrenCount() == 0) {
      pFirstChild = GetLeftMost(node, 0, j);
    } else {
      pFirstChild = pFirstChild->GetFirstChild();
    }
    if (pFirstChild != nullptr) {
      pFirstChildLeftNeighbor = _GetLayoutWorkData(pFirstChild)->pLeftNeighbor;
    }
  }
}

void touchmind::layout::LayoutManager::SecondWalk(const std::shared_ptr<touchmind::model::node::NodeModel> &tree,
                                                  const std::shared_ptr<touchmind::model::node::NodeModel> &node,
                                                  int level, FLOAT x, FLOAT y) {
  if (level <= m_configuration->GetMaxDepth()) {
    FLOAT xTmp = mw_rootXOffset + _GetLayoutWorkData(node)->prelim + x;
    FLOAT yTmp = mw_rootYOffset + y;
    FLOAT maxSizeTmp = 0;
    FLOAT nodeSizeTmp = 0;
    bool flag = false;

    switch (m_configuration->GetLayoutOrientation()) {
    case LAYOUT_ORIENTATION_TOP:
    case LAYOUT_ORIENTATION_BOTTOM:
      maxSizeTmp = mw_maxLevelHeight[level];
      nodeSizeTmp = node->GetHeight();
      break;
    case LAYOUT_ORIENTATION_RIGHT:
    case LAYOUT_ORIENTATION_LEFT:
      maxSizeTmp = mw_maxLevelWidth[level];
      flag = true;
      nodeSizeTmp = node->GetWidth();
      break;
    }
    switch (m_configuration->GetNodeJustification()) {
    case NODE_JUSTIFICATION_TOP:
      node->SetX(xTmp);
      node->SetY(yTmp);
      break;
    case NODE_JUSTIFICATION_CENTER:
      node->SetX(xTmp);
      node->SetY(yTmp + (maxSizeTmp - nodeSizeTmp) / 2);
      break;
    case NODE_JUSTIFICATION_BOTTOM:
      node->SetX(xTmp);
      node->SetY(yTmp + maxSizeTmp - nodeSizeTmp);
      break;
    }
    if (flag) {
      FLOAT swapTmp = node->GetX();
      node->SetX(node->GetY());
      node->SetY(swapTmp);
    }
    switch (m_configuration->GetLayoutOrientation()) {
    case LAYOUT_ORIENTATION_BOTTOM:
      node->SetY(-node->GetY() - nodeSizeTmp);
      break;
    case LAYOUT_ORIENTATION_RIGHT:
      node->SetX(-node->GetX() - nodeSizeTmp);
      break;
    }
    if (node->GetChildrenCount() != 0) {
      SecondWalk(tree, node->GetFirstChild(), level + 1, x + _GetLayoutWorkData(node)->modifier,
                 y + maxSizeTmp + m_configuration->GetLevelSeparation());
    }
    std::shared_ptr<touchmind::model::node::NodeModel> rightSibling = node->GetRightSibling();
    if (rightSibling != nullptr) {
      SecondWalk(tree, rightSibling, level, x, y);
    }
  }
}

void touchmind::layout::LayoutManager::RearrangeVerticalPosition(touchmind::NODE_SIDE nodePosition) {
  std::vector<std::shared_ptr<touchmind::model::node::NodeModel>> targetNodes;
  std::vector<TreeRect> treeRects;
  std::vector<FLOAT> treeOffsetY;

  FLOAT width = 0.0f;
  FLOAT height = 0.0f;
  for (size_t i = 0; i < m_node->GetChildrenCount(); ++i) {
    std::shared_ptr<touchmind::model::node::NodeModel> pNode = m_node->GetChild(i);
    if (pNode->GetPosition() == nodePosition) {
      targetNodes.push_back(pNode);

      TreeRectVisitor treeRectVisitor;
      pNode->ApplyVisitor(treeRectVisitor);
      treeRects.push_back(treeRectVisitor.treeRect);

      if (width < treeRectVisitor.treeRect.GetWidth()) {
        width = treeRectVisitor.treeRect.GetWidth();
      }
      treeOffsetY.push_back(height);
      height += treeRectVisitor.treeRect.GetHeight();
      height += m_configuration->GetSubtreeSeparation();
    }
  }

  // subtract last subtree separation
  height -= m_configuration->GetSubtreeSeparation();

  FLOAT zoomX = 1.0f;
  FLOAT zoomY = 1.0f;
  FLOAT offsetX = m_node->GetWidth() / 2.0f + m_configuration->GetLevelSeparation();
  if (nodePosition == touchmind::NODE_SIDE_LEFT) {
    offsetX = -offsetX;
  }
  FLOAT offsetY = -(height / 2.0f);

  for (std::vector<std::shared_ptr<touchmind::model::node::NodeModel>>::size_type i = 0; i < targetNodes.size(); ++i) {
    TreeTransformVisitor treeMoveVisitor(offsetX, offsetY + treeOffsetY[i], zoomX, zoomY);
    std::shared_ptr<touchmind::model::node::NodeModel> pNode = targetNodes[i];
    pNode->ApplyVisitor(treeMoveVisitor);

    TreeRectVisitor treeRectVisitor;
    pNode->ApplyVisitor(treeRectVisitor);
  }
}

void touchmind::layout::LayoutManager::PositionTree() {
  m_node->ApplyVisitor([&](model::node::NodeModel::reference node) -> VISITOR_RESULT {
    if (m_layoutWorkDataMap.count(node->GetId()) == 0) {
      m_layoutWorkDataMap.insert(std::make_pair<NODE_ID, LayoutWorkData>(node->GetId(), LayoutWorkData()));
    }
    return VISITOR_RESULT_CONTINUE;
  });

  for (size_t i = 0; i < m_node->GetChildrenCount(); ++i) {
    std::shared_ptr<touchmind::model::node::NodeModel> pFirstLevelChildNode = m_node->GetChild(i);
    // first level child node become root node temporarily
    pFirstLevelChildNode->SetParent(nullptr);
    if (pFirstLevelChildNode->GetPosition() == touchmind::NODE_SIDE_RIGHT) {
      m_configuration->SetLayoutOrientation(LAYOUT_ORIENTATION_LEFT);
    } else {
      m_configuration->SetLayoutOrientation(LAYOUT_ORIENTATION_RIGHT);
    }
    mw_maxLevelHeight.clear();
    mw_maxLevelWidth.clear();

    FirstWalk(pFirstLevelChildNode, pFirstLevelChildNode, 0);

    switch (m_configuration->GetLayoutOrientation()) {
    case LAYOUT_ORIENTATION_TOP:
    case LAYOUT_ORIENTATION_LEFT:
      mw_rootXOffset = m_configuration->GetTopXAdjustment() + pFirstLevelChildNode->GetX();
      mw_rootYOffset = m_configuration->GetTopYAdjustment() + pFirstLevelChildNode->GetY();
      break;
    case LAYOUT_ORIENTATION_BOTTOM:
    case LAYOUT_ORIENTATION_RIGHT:
      mw_rootXOffset = m_configuration->GetTopXAdjustment() + pFirstLevelChildNode->GetX();
      mw_rootYOffset = m_configuration->GetTopYAdjustment() + pFirstLevelChildNode->GetY();
      break;
    }
    SecondWalk(pFirstLevelChildNode, pFirstLevelChildNode, 0, 0, 0);
  }

  for (size_t i = 0; i < m_node->GetChildrenCount(); ++i) {
    std::shared_ptr<touchmind::model::node::NodeModel> pFirstLevelChildNode = m_node->GetChild(i);
    // recover parent node of first level child node
    pFirstLevelChildNode->SetParent(m_node);
  }

  RearrangeVerticalPosition(touchmind::NODE_SIDE_RIGHT);
  RearrangeVerticalPosition(touchmind::NODE_SIDE_LEFT);

  FLOAT offsetX = m_node->GetWidth() / 2;
  FLOAT offsetY = m_node->GetHeight() / 2;
  m_node->SetX(-offsetX);
  m_node->SetY(-offsetY);
}

FLOAT touchmind::layout::LayoutManager::GetNodeSize(const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  switch (m_configuration->GetLayoutOrientation()) {
  case LAYOUT_ORIENTATION_TOP:
  case LAYOUT_ORIENTATION_BOTTOM:
    return node->GetWidth();
  case LAYOUT_ORIENTATION_RIGHT:
  case LAYOUT_ORIENTATION_LEFT:
    return node->GetHeight();
  }
  return 0;
}

void touchmind::layout::LayoutManager::SaveAsVML(const std::wstring &filename) {
  TreeRectVisitor treeRectVisitor;
  m_node->ApplyVisitor(treeRectVisitor);

  std::wofstream fout;
  fout.open(filename.c_str());

  fout << L"<html lang=\"ja\" xmlns:v=\"urn:schemas-microsoft-com:vml\">" << std::endl;
  fout << L"<head><style type=\"text/css\"><!--" << std::endl
       << L"v\\:* { behavior: url(#default#VML); }" << std::endl
       << L"--></style></head>" << std::endl;
  fout << L"<body><div>" << std::endl;
  fout << L"<!-- (" << treeRectVisitor.treeRect.x1 << L"," << treeRectVisitor.treeRect.y1 << L") - ("
       << treeRectVisitor.treeRect.x2 << L"," << treeRectVisitor.treeRect.y2 << L") -->" << std::endl;
  fout << L"<v:rect style=\"position:absolute;margin-top:0" << L";margin-left:0" << L";width:"
       << (treeRectVisitor.treeRect.x2 - treeRectVisitor.treeRect.x1) << L";height:"
       << (treeRectVisitor.treeRect.y2 - treeRectVisitor.treeRect.y1)
       << L";z-index:2\" fillcolor=\"white\" strokecolor=\"gray\" strokeweight=\"1pt\"/>" << std::endl;

  VMLOutVisitor vmlOut(fout, -treeRectVisitor.treeRect.x1, -treeRectVisitor.treeRect.y1);
  m_node->ApplyVisitor(vmlOut);

  fout << L"</div></body</html>" << std::endl;
  fout.close();
}

void touchmind::layout::LayoutManager::SaveAsHTML5(const std::wstring &filename) {
  TreeRectVisitor treeRectVisitor;
  m_node->ApplyVisitor(treeRectVisitor);

  std::wofstream fout;
  fout.open(filename.c_str());

  fout << L"<!DOCTYPE html>" << std::endl;
  fout << L"<html>" << std::endl;
  fout << L"<head>" << std::endl;
  fout << L"<title>Node Tree</title>" << std::endl;
  fout << L"</head>" << std::endl;
  fout << L"<body>" << std::endl;
  fout << L"<canvas width=\"800\" height=\"600\" style=\"background-color: white;\">" << std::endl;
  fout << L"</canvas>" << std::endl;
  fout << L"</body>" << std::endl;
  fout << L"</html>" << std::endl;
  fout.close();
}
