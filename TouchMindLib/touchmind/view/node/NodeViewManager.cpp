#include "StdAfx.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/model/Insets.h"
#include "touchmind/model/CurvePoints.h"
#include "touchmind/model/MapModel.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/Context.h"
#include "touchmind/control/DWriteEditControlCommon.h"
#include "touchmind/control/DWriteEditControlManager.h"
#include "touchmind/control/DWriteEditControl.h"
#include "touchmind/view/node/NodeViewManager.h"
#include "touchmind/view/node/NodeViewFactory.h"
#include "touchmind/view/node/NodeFigureHelper.h"
#include "touchmind/view/node/BaseNodeView.h"
#include "touchmind/view/node/impl/RoundedRectNodeView.h"
#include "touchmind/view/GeometryBuilder.h"
#include "touchmind/view/path/PathViewFactory.h"
#include "touchmind/view/path/BasePathView.h"

/* static */
void touchmind::view::node::NodeViewManager::CreateBodyColor(IN COLORREF baseColor, OUT D2D1_COLOR_F &startColor,
                                                             OUT D2D1_COLOR_F &endColor) {
  touchmind::ColorrefToColorF(baseColor, &startColor);
  FLOAT h, s, v;
  touchmind::RgbToHsv(startColor.r, startColor.g, startColor.b, h, s, v);
  v *= 0.7f;
  touchmind::HsvToRgb(h, s, v, endColor.r, endColor.g, endColor.b);
  endColor.a = 1.0f;
}

void touchmind::view::node::NodeViewManager::DrawNodeShadow(touchmind::Context *pContext,
                                                            ID2D1RenderTarget *pRenderTarget,
                                                            ID2D1Geometry *pGeometry) {
  UNREFERENCED_PARAMETER(pContext);
  pRenderTarget->FillGeometry(pGeometry, m_pNodeShadowBrush);
}

void touchmind::view::node::NodeViewManager::DrawSelectedNodeShadow(touchmind::Context *pContext,
                                                                    ID2D1RenderTarget *pRenderTarget,
                                                                    ID2D1Geometry *pGeometry) {
  UNREFERENCED_PARAMETER(pContext);
  pRenderTarget->FillGeometry(pGeometry, m_pSelectedNodeShadowBrush1);
  pRenderTarget->DrawGeometry(pGeometry, m_pSelectedNodeShadowBrush2, 2.0f);
}

/// <summary>
/// Update VisualResourceInfo to synchronize with the current node models.
/// This walker does not update a layout information of the node models.
/// This walker will be invoked at BeforeArrange method before arranging layout.
/// </summary>
class SynchronizeViewBeforeArrangingVisitor {
private:
  touchmind::view::node::NodeViewManager *m_nodeViewManager;
  touchmind::view::node::NodeIdToNodeViewMap &m_nodeIdToView;
  touchmind::control::DWriteEditControlManager *m_pEditControlManager;

public:
  SynchronizeViewBeforeArrangingVisitor(touchmind::view::node::NodeViewManager *nodeViewManager,
                                        touchmind::view::node::NodeIdToNodeViewMap &nodeIdToViewMap,
                                        touchmind::control::DWriteEditControlManager *pEditControlManager)
      : m_nodeViewManager(nodeViewManager)
      , m_nodeIdToView(nodeIdToViewMap)
      , m_pEditControlManager(pEditControlManager) {
  }

  touchmind::VISITOR_RESULT operator()(std::shared_ptr<touchmind::model::node::NodeModel> node) {
    if (m_nodeIdToView.count(node->GetId()) != 0) {
      auto &nodeView = m_nodeIdToView[node->GetId()];
      if (nodeView->IsCompatible(node)) {
        nodeView->SetHandled();
      } else {
        auto editControlIndex = nodeView->GetEditControlIndex();
        m_nodeIdToView.erase(node->GetId());
        CreateNewView(node, editControlIndex);
      }
    } else {
      CreateNewView(node);
    }
    return touchmind::VISITOR_RESULT_CONTINUE;
  }

  void CreateNewView(std::shared_ptr<touchmind::model::node::NodeModel> node) {
    D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, FLT_MAX, FLT_MAX);
    touchmind::control::EDIT_CONTROL_INDEX editControlIndex;
    HRESULT hr = m_nodeViewManager->_CreateEditControl(layoutRect, node->GetText(), &editControlIndex);
    if (SUCCEEDED(hr)) {
      auto nodeView = touchmind::view::node::NodeViewFactory::Create(node);
      nodeView->SetNodeViewManager(m_nodeViewManager);
      nodeView->SetEditControlManager(m_pEditControlManager);
      nodeView->SetEditControlIndex(editControlIndex);
      nodeView->SetHandled();
      m_nodeIdToView.insert({node->GetId(), nodeView});
    } else {
      LOG(SEVERITY_LEVEL_WARN) << L"Failed to create an edit control";
    }
  }

  void CreateNewView(std::shared_ptr<touchmind::model::node::NodeModel> node,
                     touchmind::control::EDIT_CONTROL_INDEX editControlIndex) {
    auto nodeView = touchmind::view::node::NodeViewFactory::Create(node);
    nodeView->SetNodeViewManager(m_nodeViewManager);
    nodeView->SetEditControlManager(m_pEditControlManager);
    nodeView->SetEditControlIndex(editControlIndex);
    nodeView->SetHandled();
    m_nodeIdToView.insert({node->GetId(), nodeView});
  }
};

/// <summary>
/// Update VisualResourceInfo to synchronize with the current node models.
/// This walker do update a layout information of the node models.
/// This walker will be invoked at BeforeDraw method before arranging layout.
/// </summary>
class SynchronizeViewAfterArrangingVisitor {
private:
  touchmind::view::node::NodeViewManager *m_nodeViewManager;
  touchmind::view::node::NodeIdToNodeViewMap &m_nodeIdToView;
  touchmind::control::DWriteEditControlManager *m_pEditControlManager;
  touchmind::Insets m_insets;

public:
  SynchronizeViewAfterArrangingVisitor(touchmind::view::node::NodeViewManager *nodeViewManager,
                                       touchmind::view::node::NodeIdToNodeViewMap &nodeIdToViewMap,
                                       touchmind::control::DWriteEditControlManager *pEditControlManager,
                                       touchmind::Insets insets)
      : m_nodeViewManager(nodeViewManager)
      , m_nodeIdToView(nodeIdToViewMap)
      , m_pEditControlManager(pEditControlManager)
      , m_insets(insets) {
  }
  touchmind::VISITOR_RESULT operator()(std::shared_ptr<touchmind::model::node::NodeModel> node) {
    D2D1_RECT_F layoutRect = D2D1::RectF(node->GetX() + m_insets.left, node->GetY() + m_insets.top,
                                         node->GetX() + node->GetWidth() - m_insets.right,
                                         node->GetY() + node->GetHeight() - m_insets.bottom);
    if (m_nodeIdToView.count(node->GetId()) != 0) {
      std::shared_ptr<touchmind::view::node::BaseNodeView> &nodeView = m_nodeIdToView[node->GetId()];
      nodeView->SetHandled();
      auto editControlIndex = m_nodeViewManager->GetEditControlIndexFromNodeId(node->GetId());
      std::shared_ptr<touchmind::control::DWriteEditControl> editControl
          = m_pEditControlManager->GetEditControl(editControlIndex);
      if (editControl != nullptr) {
        editControl->SetLayoutRect(layoutRect);
      }
    } else {
      touchmind::control::EDIT_CONTROL_INDEX editControlIndex;
      HRESULT hr = m_nodeViewManager->_CreateEditControl(layoutRect, node->GetText(), &editControlIndex);
      if (SUCCEEDED(hr)) {
        auto nodeView = touchmind::view::node::NodeViewFactory::Create(node);
        nodeView->SetNodeViewManager(m_nodeViewManager);
        nodeView->SetEditControlManager(m_pEditControlManager);
        nodeView->SetEditControlIndex(editControlIndex);
        nodeView->SetHandled();
        m_nodeIdToView.insert({node->GetId(), nodeView});
      } else {
        LOG(SEVERITY_LEVEL_WARN) << L"Failed to create an edit control";
      }
    }
    return touchmind::VISITOR_RESULT_CONTINUE;
  }
};

touchmind::view::node::NodeViewManager::NodeViewManager()
    : m_nodeIdToViewMap()
    , m_pContext(nullptr)
    , m_pConfiguration(nullptr)
    , m_pMapModel(nullptr)
    , m_pEditControlManager(nullptr)
    , m_pGaussFilter(std::make_unique<touchmind::filter::GaussFilter>())
    , m_pNodeShadowBrush(nullptr)
    , m_pSelectedNodeShadowBrush1(nullptr)
    , m_pSelectedNodeShadowBrush2(nullptr) {
}

touchmind::view::node::NodeViewManager::~NodeViewManager() {
}

HRESULT touchmind::view::node::NodeViewManager::CreateSharedDeviceResources(touchmind::Context *pContext,
                                                                            ID2D1RenderTarget *pRenderTarget) {
  UNREFERENCED_PARAMETER(pContext);
  if (m_pNodeShadowBrush == nullptr || m_pSelectedNodeShadowBrush1 == nullptr
      || m_pSelectedNodeShadowBrush2 == nullptr) {
    m_pNodeShadowBrush = nullptr;
    CHK_RES(m_pNodeShadowBrush,
            pRenderTarget->CreateSolidColorBrush(m_pConfiguration->GetDefaultShadowColor(), &m_pNodeShadowBrush));

    m_pSelectedNodeShadowBrush1 = nullptr;
    CHK_RES(m_pSelectedNodeShadowBrush1,
            pRenderTarget->CreateSolidColorBrush(m_pConfiguration->GetDefaultSelectedColor1(),
                                                 &m_pSelectedNodeShadowBrush1));

    m_pSelectedNodeShadowBrush2 = nullptr;
    CHK_RES(m_pSelectedNodeShadowBrush2,
            pRenderTarget->CreateSolidColorBrush(m_pConfiguration->GetDefaultSelectedColor2(),
                                                 &m_pSelectedNodeShadowBrush2));

    m_pEditControlManager->InitializeDeviceResources(pRenderTarget);
  }
  return S_OK;
}

HRESULT touchmind::view::node::NodeViewManager::SynchronizeViewBeforeArrange() {
  auto pRootNode = m_pMapModel->GetRootNodeModel();
  SynchronizeViewBeforeArrangingVisitor visitor(this, m_nodeIdToViewMap, m_pEditControlManager);
  pRootNode->ApplyVisitor(visitor);
  return S_OK;
}

HRESULT touchmind::view::node::NodeViewManager::DrawNode(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
                                                         std::shared_ptr<touchmind::model::node::NodeModel> node) {
  auto nodeView = m_nodeIdToViewMap[node->GetId()];
  nodeView->Draw(pContext, pRenderTarget);
  return S_OK;
}

void touchmind::view::node::NodeViewManager::DrawPath(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
                                                      std::shared_ptr<model::node::NodeModel> node) {
  if (m_nodeIdToPathView.count(node->GetId()) == 0) {
    auto pathView = touchmind::view::path::PathViewFactory::Create(node);
    m_nodeIdToPathView.insert({node->GetId(), pathView});
    pathView->SetConfiguration(m_pConfiguration);
    pathView->SetNodeViewManager(this);
    pathView->SetNodeModel(node);
  }
  auto pathView = m_nodeIdToPathView[node->GetId()];
  pathView->Draw(pContext, pRenderTarget);
  pathView->SetHandled();
}

void touchmind::view::node::NodeViewManager::DiscardSharedDeviceResources() {
  m_pNodeShadowBrush = nullptr;
  m_pSelectedNodeShadowBrush1 = nullptr;
  m_pSelectedNodeShadowBrush2 = nullptr;
}

HRESULT touchmind::view::node::NodeViewManager::SynchronizeWithModel() {
  auto root = m_pMapModel->GetRootNodeModel();
  // set all wasHandled flags to false
  for (auto &kv : m_nodeIdToViewMap) {
    auto nodeView = kv.second;
    nodeView->ClearHandled();
  }
  // synchronize view resource with model
  SynchronizeViewAfterArrangingVisitor visitor(this, m_nodeIdToViewMap, m_pEditControlManager,
                                               m_pConfiguration->GetInsets());
  root->ApplyVisitor(visitor);

  // if wasHandled flag is false, remove its view resource
  std::vector<touchmind::NODE_ID> prepareRemove;
  for (auto &kv : m_nodeIdToViewMap) {
    auto nodeView = kv.second;
    if (!nodeView->IsHandled()) {
      m_pEditControlManager->DestroyDWLightEditControl(nodeView->GetEditControlIndex());
      prepareRemove.push_back(kv.first);
    }
  }
  // remove view resources
  for (auto &nodeId : prepareRemove) {
    m_nodeIdToViewMap.erase(nodeId);
    m_nodeIdToPathView.erase(nodeId);
  }
  return S_OK;
}

void touchmind::view::node::NodeViewManager::DiscardAllResources() {
  for (auto &kv : m_nodeIdToViewMap) {
    auto nodeView = kv.second;
    m_pEditControlManager->DestroyDWLightEditControl(nodeView->GetEditControlIndex());
  }
  m_nodeIdToViewMap.clear();
  m_nodeIdToPathView.clear();
}

int touchmind::view::node::NodeViewManager::GetEditControlIndexFromNodeId(touchmind::NODE_ID nodeId) {
  int editControlIndex = -1;
  if (m_nodeIdToViewMap.count(nodeId) != 0) {
    auto nodeView = m_nodeIdToViewMap[nodeId];
    editControlIndex = nodeView->GetEditControlIndex();
  }
  return editControlIndex;
}

touchmind::NODE_ID
touchmind::view::node::NodeViewManager::GetNodeIdFromEditControlIndex(control::EDIT_CONTROL_INDEX editControlIndex) {
  for (auto &kv : m_nodeIdToViewMap) {
    touchmind::NODE_ID nodeId = kv.first;
    auto nodeView = kv.second;
    if (nodeView->GetEditControlIndex() == editControlIndex) {
      return nodeId;
    }
  }
  return -1;
}

HRESULT touchmind::view::node::NodeViewManager::_CreateEditControl(IN D2D1_RECT_F layoutRect,
                                                                   IN const std::wstring &text,
                                                                   OUT control::EDIT_CONTROL_INDEX *editControlIndex) {
  HRESULT hr = m_pEditControlManager->CreateEditControl(layoutRect, text, editControlIndex);
  if (SUCCEEDED(hr)) {
    std::shared_ptr<control::DWriteEditControl> pEditControl = m_pEditControlManager->GetEditControl(*editControlIndex);
    pEditControl->SetBackgroundColor(D2D1::ColorF(0, 0.0f));
    pEditControl->SetBorderLineColor(D2D1::ColorF(0, 0.0f));
  }
  return hr;
}

HRESULT touchmind::view::node::NodeViewManager::SyncFontAttributesToTextLayout() {
  auto root = m_pMapModel->GetRootNodeModel();
  HRESULT hr = S_OK;
  for (auto &kv : m_nodeIdToViewMap) {
    touchmind::NODE_ID nodeId = kv.first;
    auto nodeView = kv.second;
    std::shared_ptr<touchmind::model::node::NodeModel> node = root->FindByNodeId(nodeId);
    std::shared_ptr<control::DWriteEditControl> pEditControl
        = m_pEditControlManager->GetEditControl(nodeView->GetEditControlIndex());

    if (node != nullptr && pEditControl != nullptr) {
      std::vector<touchmind::text::FontAttribute> fontAttributes;
      for (size_t i = 0; i < node->GetFontAttributeCount(); ++i) {
        const touchmind::text::FontAttribute &fontAttribute = node->GetFontAttribute(i);
        fontAttributes.push_back(fontAttribute);
      }
      pEditControl->SetFontAttributes(m_pConfiguration, fontAttributes);
    }
  }
  return hr;
}

/// <summary>synchronizes text layout to font attributes of the nodes</summary>
HRESULT touchmind::view::node::NodeViewManager::SyncFontAttributesFromTextLayout() {
  auto root = m_pMapModel->GetRootNodeModel();
  HRESULT hr = S_OK;
  for (auto &kv : m_nodeIdToViewMap) {
    touchmind::NODE_ID nodeId = kv.first;
    auto nodeView = kv.second;
    std::shared_ptr<touchmind::model::node::NodeModel> node = root->FindByNodeId(nodeId);
    std::shared_ptr<control::DWriteEditControl> pEditControl
        = m_pEditControlManager->GetEditControl(nodeView->GetEditControlIndex());

    std::vector<touchmind::text::FontAttribute> fontAttributes;
    pEditControl->GetFontAttributes(fontAttributes);

    node->ClearFontAttributes();
    for (auto &fontAttribute : fontAttributes) {
      node->AddFontAttribute(fontAttribute);
    }
  }

  return hr;
}

void touchmind::view::node::NodeViewManager::DrawNodes(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
                                                       std::shared_ptr<model::node::NodeModel> edittingNode) {
  CreateSharedDeviceResources(pContext, pRenderTarget);
  auto root = m_pMapModel->GetRootNodeModel();

  // if (root->IsDescendantRepaintRequired()) {
  SynchronizeWithModel();
  //}

  // Draw all paths
  root->ApplyVisitor([&](std::shared_ptr<model::node::NodeModel> node) -> touchmind::VISITOR_RESULT {
    DrawNodePaths(pContext, pRenderTarget, node);
    return touchmind::VISITOR_RESULT_CONTINUE;
  });

  // Draw all nodes
  root->ApplyVisitor([&](std::shared_ptr<model::node::NodeModel> node) -> touchmind::VISITOR_RESULT {
    if (edittingNode != node) {
      DrawNode(pContext, pRenderTarget, node);
    }
    return touchmind::VISITOR_RESULT_CONTINUE;
  });

  // root->ClearDescendantRepaintRequired();
}

void touchmind::view::node::NodeViewManager::DrawNodePaths(touchmind::Context *pContext,
                                                           ID2D1RenderTarget *pRenderTarget,
                                                           std::shared_ptr<model::node::NodeModel> node) {
  PATH_DIRECTION pathDirection;
  touchmind::NODE_SIDE nodePosition = node->GetAncestorPosition();
  switch (nodePosition) {
  case touchmind::NODE_SIDE_RIGHT:
    pathDirection = PATH_DIRECTION_RIGHT;
    break;
  case touchmind::NODE_SIDE_LEFT:
    pathDirection = PATH_DIRECTION_LEFT;
    break;
  case touchmind::NODE_SIDE_UNDEFINED:
    pathDirection = PATH_DIRECTION_AUTO;
    break;
  default:
    pathDirection = PATH_DIRECTION_AUTO;
  }

  for (size_t i = 0; i < node->GetActualChildrenCount(); ++i) {
    std::shared_ptr<touchmind::model::node::NodeModel> child = node->GetChild(i);
    DrawPath(pContext, pRenderTarget, child);
  }
}

void touchmind::view::node::NodeViewManager::OnDebugDump(std::wofstream &os) {
  os << L"NodeViewManager::OnDebugDump start <<" << std::endl;
  for (auto &kv : m_nodeIdToViewMap) {
    touchmind::NODE_ID nodeId = kv.first;
    auto &nodeView = kv.second;
    os << L"nodeId = " << nodeId << L" -> NodeView" << L"[editControlIndex=" << nodeView->GetEditControlIndex()
       << L",isHandled=" << std::boolalpha << nodeView->IsHandled() << std::endl;
  }
  os << L"NodeViewManager::OnDebugDump >> end" << std::endl;

  m_pEditControlManager->OnDebugDump(os);
}

std::shared_ptr<touchmind::model::node::NodeModel>
touchmind::view::node::NodeViewManager::NodeHitTest(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
                                                    D2D1_POINT_2F point) {
  UNREFERENCED_PARAMETER(pContext);
  UNREFERENCED_PARAMETER(pRenderTarget);

  FLOAT margin = 5.0f;
  std::shared_ptr<touchmind::model::node::NodeModel> result;
  auto root = m_pMapModel->GetRootNodeModel();
  root->ApplyVisitor([&](std::shared_ptr<touchmind::model::node::NodeModel> &node) -> VISITOR_RESULT {
    if (((!node->IsCollapsed() && !node->IsAncestorCollapsed())
         || (node->IsCollapsed() && !node->IsAncestorCollapsed()))
        && (node->GetX() - margin) <= point.x && point.x <= (node->GetX() + node->GetWidth() + margin)
        && (node->GetY() - margin) <= point.y && point.y <= (node->GetY() + node->GetHeight() + margin)) {
      result = node;
      return touchmind::VISITOR_RESULT_STOP;
    }
    return touchmind::VISITOR_RESULT_CONTINUE;
  });
  return result;
}

std::shared_ptr<touchmind::model::path::PathModel>
touchmind::view::node::NodeViewManager::PathHitTest(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget,
                                                    D2D1_POINT_2F point) {
  std::shared_ptr<touchmind::model::path::PathModel> result;
  auto root = m_pMapModel->GetRootNodeModel();
  root->ApplyVisitor([&](std::shared_ptr<touchmind::model::node::NodeModel> &node) -> VISITOR_RESULT {
    if (((!node->IsCollapsed() && !node->IsAncestorCollapsed())
         || (node->IsCollapsed() && !node->IsAncestorCollapsed()))) {
      if (m_nodeIdToPathView.count(node->GetId()) > 0) {
        auto pathView = m_nodeIdToPathView[node->GetId()];
        if (pathView->HitTest(pContext, pRenderTarget, point)) {
          result = pathView->GetNodeModel().lock()->GetPathModel();
          return touchmind::VISITOR_RESULT_STOP;
        }
      }
    }
    return touchmind::VISITOR_RESULT_CONTINUE;
  });
  return result;
}

void touchmind::view::node::NodeViewManager::DiscardAllDeviceResources() {
  DiscardSharedDeviceResources();
  DiscardAllResources();
}
