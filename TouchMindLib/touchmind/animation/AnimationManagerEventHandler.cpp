#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/model/node/NodeModel.h"
#include "touchmind/animation/AnimationManagerEventHandler.h"

COM_DECLSPEC_NOTHROW HRESULT touchmind::animation::AnimationManagerEventHandler::CreateInstance(
    IAnimationStatusChangedListener* pListener,
    IUIAnimationManagerEventHandler** ppManagerEventHandler)
{
    AnimationManagerEventHandler* pAnimationManagerEventHandler;
    HRESULT hr = CUIAnimationCallbackBase::CreateInstance(ppManagerEventHandler, &pAnimationManagerEventHandler);
    if (SUCCEEDED(hr)) {
        pAnimationManagerEventHandler->m_pListener = pListener;
    }
    return hr;
}

touchmind::animation::AnimationManagerEventHandler::AnimationManagerEventHandler() :
    m_pListener(nullptr)
{
}

touchmind::animation::AnimationManagerEventHandler::~AnimationManagerEventHandler()
{
}

HRESULT touchmind::animation::AnimationManagerEventHandler::OnManagerStatusChanged(
    UI_ANIMATION_MANAGER_STATUS newStatus,
    UI_ANIMATION_MANAGER_STATUS previousStatus)
{
    UNREFERENCED_PARAMETER(previousStatus);
    HRESULT hr = S_OK;

    if (newStatus == UI_ANIMATION_MANAGER_BUSY) {
        hr = m_pListener->OnAnimationStatusChanged(newStatus, previousStatus);
    }

    return hr;
}
