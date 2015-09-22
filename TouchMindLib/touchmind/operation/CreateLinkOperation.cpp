#include "stdafx.h"
#include "touchmind/Context.h"
#include "touchmind/MUI.h"
#include "touchmind/win/CanvasPanel.h"
#include "touchmind/model/MapModel.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/operation/CreateLinkOperation.h"

const FLOAT touchmind::operation::CreateLinkOperation::MESSAGE_BOX_WIDTH = 100.0f;

const FLOAT touchmind::operation::CreateLinkOperation::MESSAGE_BOX_HEIGHT = 50.0f;

touchmind::operation::CreateLinkOperation::CreateLinkOperation()
    : m_pCanvasPanel(nullptr)
    , m_foregroundBrush(nullptr)
    ,
    // start node message
    m_startNodeMessasgeText(MUI::GetString(IDS_CREATE_LINK_MESSAGE_START))
    , m_startNodeMessageTextFormat(nullptr)
    , m_startNodeMessageTextLayout(nullptr)
    , m_startNodeBodyBrush(nullptr)
    ,
    // end node message
    m_endNodeMessasgeText(MUI::GetString(IDS_CREATE_LINK_MESSAGE_END))
    , m_endNodeMessageTextFormat(nullptr)
    , m_endNodeMessageTextLayout(nullptr)
    , m_endNodeBodyBrush(nullptr)
    ,
    // start node number
    m_startNodeNumberText(MUI::GetString(IDS_CREATE_LINK_FIRST_NODE))
    , m_startNodeNumberMarkBrush(nullptr)
    , m_startNodeNumberTextFormat(nullptr)
    , m_startNodeNumberTextLayout(nullptr)
    ,
    // message text animation
    m_messageTextStoryboard(nullptr)
    , m_messageTextVar(nullptr)
    , m_messageTextTransition1(nullptr)
    , m_messageTextTransition2(nullptr)
    ,
    // start node animation
    m_startNodeStoryboard(nullptr)
    , m_startNodeRadius(nullptr)
    , m_startNodeTransition(nullptr)
    ,
    // end node animation
    m_endNodeStoryboard(nullptr)
    , m_endNodeRadius(nullptr)
    , m_endNodeTransition(nullptr)
    , m_startNode()
    , m_endNode() {
}

touchmind::operation::CreateLinkOperation::~CreateLinkOperation() {
}

void touchmind::operation::CreateLinkOperation::CreateDeviceDependentResources(touchmind::Context *pContext,
                                                                               ID2D1RenderTarget *pRenderTarget) {
  if (m_foregroundBrush == nullptr) {
    CHK_RES(m_foregroundBrush,
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &m_foregroundBrush));
  }
  // start node message
  if (m_startNodeBodyBrush == nullptr) {
    CHK_RES(m_startNodeBodyBrush,
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Navy), &m_startNodeBodyBrush));
  }
  if (m_startNodeMessageTextFormat == nullptr) {
    CHK_RES(m_startNodeMessageTextFormat,
            pContext->GetDWriteFactory()->CreateTextFormat(
                MUI::GetString(IDS_DEFAULT_FONT_NAME), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"", &m_startNodeMessageTextFormat));
  }
  if (m_startNodeMessageTextLayout == nullptr) {
    CHK_RES(m_startNodeMessageTextLayout,
            pContext->GetDWriteFactory()->CreateTextLayout(
                m_startNodeMessasgeText.c_str(), static_cast<UINT32>(m_startNodeMessasgeText.size()),
                m_startNodeMessageTextFormat, MESSAGE_BOX_WIDTH, MESSAGE_BOX_HEIGHT, &m_startNodeMessageTextLayout));
  }
  // end node message
  if (m_endNodeBodyBrush == nullptr) {
    CHK_RES(m_endNodeBodyBrush,
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange), &m_endNodeBodyBrush));
  }
  if (m_endNodeMessageTextFormat == nullptr) {
    CHK_RES(m_endNodeMessageTextFormat,
            pContext->GetDWriteFactory()->CreateTextFormat(
                MUI::GetString(IDS_DEFAULT_FONT_NAME), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, 14.0f, L"", &m_endNodeMessageTextFormat));
  }
  if (m_endNodeMessageTextLayout == nullptr) {
    CHK_RES(m_endNodeMessageTextLayout,
            pContext->GetDWriteFactory()->CreateTextLayout(
                m_endNodeMessasgeText.c_str(), static_cast<UINT32>(m_endNodeMessasgeText.size()),
                m_endNodeMessageTextFormat, MESSAGE_BOX_WIDTH, MESSAGE_BOX_HEIGHT, &m_endNodeMessageTextLayout));
  }
  // start node number
  if (m_startNodeNumberMarkBrush == nullptr) {
    CHK_RES(m_startNodeNumberMarkBrush,
            pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 1.0f, 0.5f), &m_startNodeNumberMarkBrush));
  }
  if (m_startNodeNumberTextFormat == nullptr) {
    CHK_RES(m_startNodeNumberTextFormat,
            pContext->GetDWriteFactory()->CreateTextFormat(
                MUI::GetString(IDS_DEFAULT_FONT_NAME), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL, 16.0f, L"", &m_startNodeNumberTextFormat));
  }
  if (m_startNodeNumberTextLayout == nullptr) {
    CHK_RES(m_startNodeMessageTextLayout,
            pContext->GetDWriteFactory()->CreateTextLayout(
                m_startNodeNumberText.c_str(), static_cast<UINT32>(m_startNodeNumberText.size()),
                m_startNodeNumberTextFormat, FLT_MAX, FLT_MAX, &m_startNodeNumberTextLayout));
  }
}

void touchmind::operation::CreateLinkOperation::StartSelectFirstNode(touchmind::Context *pContext) {
  m_startNode = nullptr;
  m_endNode = nullptr;

  m_messageTextStoryboard = nullptr;
  pContext->GetAnimationManager()->CreateStoryboard(&m_messageTextStoryboard);
  m_messageTextVar = nullptr;
  pContext->GetAnimationManager()->CreateAnimationVariable(1.0f, &m_messageTextVar);
  m_messageTextTransition1 = nullptr;
  pContext->GetAnimationTransitionLibrary()->CreateLinearTransition(2.0f, 0.2f, &m_messageTextTransition1);
  m_messageTextStoryboard->AddTransition(m_messageTextVar, m_messageTextTransition1);
  m_messageTextTransition2 = nullptr;
  pContext->GetAnimationTransitionLibrary()->CreateLinearTransition(2.0f, 1.0f, &m_messageTextTransition2);
  m_messageTextStoryboard->AddTransition(m_messageTextVar, m_messageTextTransition2);
  UI_ANIMATION_KEYFRAME keyframe;
  m_messageTextStoryboard->AddKeyframeAfterTransition(m_messageTextTransition2, &keyframe);
  m_messageTextStoryboard->RepeatBetweenKeyframes(UI_ANIMATION_KEYFRAME_STORYBOARD_START, keyframe,
                                                  UI_ANIMATION_REPEAT_INDEFINITELY);

  UI_ANIMATION_SECONDS secondsNow;
  pContext->GetAnimationTimer()->GetTime(&secondsNow);
  m_messageTextStoryboard->Schedule(secondsNow);
}

void touchmind::operation::CreateLinkOperation::EndOperation() {
  if (m_messageTextStoryboard != nullptr) {
    m_messageTextStoryboard->Abandon();
  }
  m_messageTextStoryboard = nullptr;
  m_messageTextVar = nullptr;
  m_messageTextTransition1 = nullptr;
  m_messageTextTransition2 = nullptr;
  m_startNode = nullptr;
  m_endNode = nullptr;
}

void touchmind::operation::CreateLinkOperation::Draw(touchmind::Context *pContext, ID2D1RenderTarget *pRenderTarget) {
  UNREFERENCED_PARAMETER(pContext);
  D2D1_SIZE_F rtSize = pRenderTarget->GetSize();

  FLOAT x = -rtSize.width / 2.0f + 2.0f;
  FLOAT y = -rtSize.height / 2.0f + 2.0f;

  if (m_messageTextVar != nullptr) {
    CComPtr<IDWriteTextLayout> pTextLayout
        = (m_startNode == nullptr && m_endNode == nullptr) ? m_startNodeMessageTextLayout : m_endNodeMessageTextLayout;
    CComPtr<ID2D1SolidColorBrush> pBrush
        = (m_startNode == nullptr && m_endNode == nullptr) ? m_startNodeBodyBrush : m_endNodeBodyBrush;

    DWRITE_TEXT_METRICS metrics;
    pTextLayout->GetMetrics(&metrics);
    D2D1_RECT_F bodyRect = D2D1::RectF(x, y, x + metrics.width + 10.0f * 2, y + metrics.height + 10.0f * 2);
    double alpha;
    m_messageTextVar->GetValue(&alpha);
    pBrush->SetOpacity(static_cast<FLOAT>(alpha));
    pRenderTarget->FillRoundedRectangle(D2D1::RoundedRect(bodyRect, 10.0f, 10.0f), pBrush);
    pRenderTarget->DrawTextLayout(D2D1::Point2F(x + 10.0f, y + 10.0f), pTextLayout, m_foregroundBrush);
  }

  if (m_startNode != nullptr) {
    FLOAT cx = m_startNode->GetX() + m_startNode->GetWidth() / 2.0f;
    FLOAT cy = m_startNode->GetY() + m_startNode->GetHeight() / 2.0f;
    DWRITE_TEXT_METRICS metrics;
    m_startNodeNumberTextLayout->GetMetrics(&metrics);
    FLOAT r = std::max(metrics.width, metrics.height) + 3.0f;
    pRenderTarget->FillEllipse(D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), m_startNodeNumberMarkBrush);
    pRenderTarget->DrawTextLayout(D2D1::Point2F(cx - metrics.width / 2.0f, cy - metrics.height / 2.0f),
                                  m_startNodeNumberTextLayout, m_foregroundBrush);
  }

  if (m_startNodeStoryboard != nullptr) {
    UI_ANIMATION_STORYBOARD_STATUS status;
    m_startNodeStoryboard->GetStatus(&status);
    if (status == UI_ANIMATION_STORYBOARD_PLAYING) {
      double radius;
      m_startNodeRadius->GetValue(&radius);
      pRenderTarget->DrawEllipse(
          D2D1::Ellipse(m_startNodeCenterPoint, static_cast<FLOAT>(radius), static_cast<FLOAT>(radius)),
          m_startNodeBodyBrush);
    }
  }
  if (m_endNodeStoryboard != nullptr) {
    UI_ANIMATION_STORYBOARD_STATUS status;
    m_endNodeStoryboard->GetStatus(&status);
    if (status == UI_ANIMATION_STORYBOARD_PLAYING) {
      double radius;
      m_endNodeRadius->GetValue(&radius);
      pRenderTarget->DrawEllipse(
          D2D1::Ellipse(m_endNodeCenterPoint, static_cast<FLOAT>(radius), static_cast<FLOAT>(radius)),
          m_endNodeBodyBrush);
    }
  }
}

void touchmind::operation::CreateLinkOperation::DiscardDeviceDependentResources() {
}

void touchmind::operation::CreateLinkOperation::_CreateLink() {
  m_pCanvasPanel->GetMapModel()->BeginTransaction();
  std::shared_ptr<touchmind::model::link::LinkModel> linkModel
      = touchmind::model::link::LinkModel::Create(m_pCanvasPanel->GetSelectionManager());
  linkModel->SetLinkId(touchmind::model::link::LinkModel::GenerateLinkId());
  linkModel->SetNode(EDGE_ID_1, m_startNode);
  linkModel->SetNode(EDGE_ID_2, m_endNode);
  m_pCanvasPanel->GetMapModel()->AddLink(linkModel);
  m_pCanvasPanel->GetMapModel()->EndTransaction();
}

void touchmind::operation::CreateLinkOperation::SetSelectedNode(
    touchmind::Context *pContext, const std::shared_ptr<touchmind::model::node::NodeModel> &node) {
  if (node == nullptr) {
    return;
  }

  if (m_startNode == nullptr) {
    m_startNode = node;
    m_startNodeCenterPoint = node->GetCenter();
    _StartStartNodeAnimation(pContext);
  } else if (m_startNode != node) {
    m_endNode = node;
    m_endNodeCenterPoint = node->GetCenter();
    _StartEndNodeAnimation(pContext);
    _CreateLink();
    EndOperation();
    m_pCanvasPanel->OnCreateLink_End();
  }
}

void touchmind::operation::CreateLinkOperation::Destroy() {
  _StopStartNodeAnimation();
  _StopEndNodeAnimation();
  EndOperation();
}

void touchmind::operation::CreateLinkOperation::_StartStartNodeAnimation(touchmind::Context *pContext) {
  _StopStartNodeAnimation();

  m_startNodeStoryboard = nullptr;
  CHK_HR(pContext->GetAnimationManager()->CreateStoryboard(&m_startNodeStoryboard));
  m_startNodeRadius = nullptr;
  CHK_HR(pContext->GetAnimationManager()->CreateAnimationVariable(0.0f, &m_startNodeRadius));
  m_startNodeTransition = nullptr;
  CHK_HR(pContext->GetAnimationTransitionLibrary()->CreateLinearTransition(0.5f, 200.0f, &m_startNodeTransition));
  CHK_HR(m_startNodeStoryboard->AddTransition(m_startNodeRadius, m_startNodeTransition));

  UI_ANIMATION_SECONDS secondsNow;
  CHK_HR(pContext->GetAnimationTimer()->GetTime(&secondsNow));
  CHK_HR(m_startNodeStoryboard->Schedule(secondsNow));
}

void touchmind::operation::CreateLinkOperation::_StopStartNodeAnimation() {
  if (m_startNodeStoryboard != nullptr) {
    CHK_HR(m_startNodeStoryboard->Abandon());
  }
  m_startNodeStoryboard = nullptr;
  m_startNodeRadius = nullptr;
  m_startNodeTransition = nullptr;
}

void touchmind::operation::CreateLinkOperation::_StartEndNodeAnimation(touchmind::Context *pContext) {
  _StopEndNodeAnimation();

  m_endNodeStoryboard = nullptr;
  CHK_HR(pContext->GetAnimationManager()->CreateStoryboard(&m_endNodeStoryboard));
  m_endNodeRadius = nullptr;
  CHK_HR(pContext->GetAnimationManager()->CreateAnimationVariable(0.0f, &m_endNodeRadius));
  m_endNodeTransition = nullptr;
  CHK_HR(pContext->GetAnimationTransitionLibrary()->CreateLinearTransition(0.5f, 200.0f, &m_endNodeTransition));
  CHK_HR(m_endNodeStoryboard->AddTransition(m_endNodeRadius, m_endNodeTransition));

  UI_ANIMATION_SECONDS secondsNow;
  CHK_HR(pContext->GetAnimationTimer()->GetTime(&secondsNow));
  CHK_HR(m_endNodeStoryboard->Schedule(secondsNow));
}

void touchmind::operation::CreateLinkOperation::_StopEndNodeAnimation() {
  if (m_endNodeStoryboard != nullptr) {
    CHK_HR(m_endNodeStoryboard->Abandon());
  }
  m_endNodeStoryboard = nullptr;
  m_endNodeRadius = nullptr;
  m_endNodeTransition = nullptr;
}
