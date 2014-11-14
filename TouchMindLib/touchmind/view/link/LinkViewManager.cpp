#include "stdafx.h"
#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/Context.h"
#include "touchmind/Configuration.h"
#include "touchmind/model/MapModel.h"
#include "touchmind/view/link/impl/DefaultLinkView.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/model/link/LinkModel.h"
#include "touchmind/model/linkedge/LinkEdgeModel.h"
#include "touchmind/view/link/impl/DefaultLinkView.h"
#include "touchmind/view/link/LinkViewManager.h"
#include "touchmind/view/link/LinkViewFactory.h"

touchmind::view::link::LinkViewManager::LinkViewManager() :
    m_pMapModel(nullptr),
    m_pConfiguration(nullptr),
    m_views(),
    m_pGaussFilter(new touchmind::filter::GaussFilter()),
    m_pShadowBrush(nullptr),
    m_pSelectedShadowBrush1(nullptr),
    m_pSelectedShadowBrush2(nullptr)
{
}

touchmind::view::link::LinkViewManager::~LinkViewManager()
{
}

std::shared_ptr<touchmind::view::link::BaseLinkView> touchmind::view::link::LinkViewManager::GetLinkView(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget,
    std::shared_ptr<touchmind::model::link::LinkModel> link)
{
    touchmind::LINK_ID linkId = link->GetLinkId();
    std::shared_ptr<touchmind::view::link::BaseLinkView> linkView;
    if (m_views.count(linkId) == 0) {
        linkView = touchmind::view::link::LinkViewFactory::Create(link);
        linkView->SetLinkViewManager(this);
		m_views.insert({ linkId, linkView });
    } else {
        linkView = m_views[linkId];
    }
    linkView->CreateDeviceDependentResources(pContext, pRenderTarget);
    return linkView;
}

void touchmind::view::link::LinkViewManager::Synchronize(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget)
{
    UNREFERENCED_PARAMETER(pContext);
    UNREFERENCED_PARAMETER(pRenderTarget);
    for (auto it = std::begin(m_views); it != std::end(m_views); ++it) {
        it->second->ClearHandled();
    }
    for (size_t i = 0; i < m_pMapModel->GetNumberOfLinkModels(); ++i) {
        auto link = m_pMapModel->GetLinkModel(i);
        if (link->IsValid()) {
            auto linkView = GetLinkView(pContext, pRenderTarget, link);
            linkView->SetHandled();
        }
    }
    for (auto it = std::begin(m_views); it != std::end(m_views); ++it) {
        if (!it->second->IsHandled()) {
            m_views.erase(it);
        }
    }
}

std::shared_ptr<touchmind::model::link::LinkModel> touchmind::view::link::LinkViewManager::HitTest(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget,
    D2D1_POINT_2F point)
{
    for (size_t i = 0; i < m_pMapModel->GetNumberOfLinkModels(); ++i) {
        auto link = m_pMapModel->GetLinkModel(i);
        if (link->IsValid()) {
            auto linkView = GetLinkView(pContext, pRenderTarget, link);
            if (linkView->HitTest(pContext, pRenderTarget, point)) {
                return link;
            }
        }
    }
    return nullptr;
}

void touchmind::view::link::LinkViewManager::Draw(
    touchmind::Context *pContext,
    ID2D1RenderTarget *pRenderTarget )
{
    for (size_t i = 0; i < m_pMapModel->GetNumberOfLinkModels(); ++i) {
        auto link = m_pMapModel->GetLinkModel(i);
        if (link->IsValid()) {
            auto linkView = GetLinkView(pContext, pRenderTarget, link);
            linkView->Draw(pContext, pRenderTarget);
        }
    }
}

void touchmind::view::link::LinkViewManager::CreateSharedDeviceResources( touchmind::Context *pContext, ID2D1RenderTarget* pRenderTarget )
{
    UNREFERENCED_PARAMETER(pContext);
    m_pShadowBrush = nullptr;
    CHK_RES(m_pShadowBrush, pRenderTarget->CreateSolidColorBrush(
                m_pConfiguration->GetDefaultShadowColor(),
                &m_pShadowBrush));

    m_pSelectedShadowBrush1 = nullptr;
    CHK_RES(m_pSelectedShadowBrush1, pRenderTarget->CreateSolidColorBrush(
                m_pConfiguration->GetDefaultSelectedColor1(),
                &m_pSelectedShadowBrush1));

    m_pSelectedShadowBrush2 = nullptr;
    CHK_RES(m_pSelectedShadowBrush2, pRenderTarget->CreateSolidColorBrush(
                m_pConfiguration->GetDefaultSelectedColor2(),
                &m_pSelectedShadowBrush2));
}

void touchmind::view::link::LinkViewManager::DiscardSharedDeviceResources()
{
}

void touchmind::view::link::LinkViewManager::DiscardAllDeviceResources()
{
    DiscardSharedDeviceResources();
    for (auto it = std::begin(m_views); it != std::end(m_views); ++it) {
        (*it).second->DiscardDeviceDependentResources();
    }
}
