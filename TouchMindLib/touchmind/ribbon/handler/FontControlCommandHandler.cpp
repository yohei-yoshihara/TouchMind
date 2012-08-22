#include "stdafx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/control/DWriteEditControlManager.h"
#include "touchmind/ribbon/handler/FontControlCommandHandler.h"

HRESULT touchmind::ribbon::handler::FontControlCommandHandler::CreateInstance(OUT IUICommandHandler **ppCommandHandler)
{
    LOG_ENTER;
    HRESULT hr = S_OK;
    if (!ppCommandHandler) {
        hr = E_POINTER;
    } else {
        *ppCommandHandler = nullptr;
        touchmind::ribbon::handler::FontControlCommandHandler* pCommandHandler =
            new touchmind::ribbon::handler::FontControlCommandHandler();
        if (pCommandHandler != nullptr) {
            *ppCommandHandler = static_cast<IUICommandHandler *>(pCommandHandler);
        } else {
            hr = E_OUTOFMEMORY;
        }
    }
    LOG_LEAVE_HRESULT(hr);
    return hr;
}

STDMETHODIMP_(ULONG) touchmind::ribbon::handler::FontControlCommandHandler::AddRef()
{
    return InterlockedIncrement(&m_refCount);
}

STDMETHODIMP_(ULONG) touchmind::ribbon::handler::FontControlCommandHandler::Release()
{
    LONG cRef = InterlockedDecrement(&m_refCount);
    if (cRef == 0) {
        delete this;
    }

    return cRef;
}

STDMETHODIMP touchmind::ribbon::handler::FontControlCommandHandler::QueryInterface(REFIID iid, void** ppv)
{
    if (iid == __uuidof(IUnknown)) {
        *ppv = static_cast<IUnknown*>(this);
    } else if (iid == __uuidof(IUICommandHandler)) {
        *ppv = static_cast<IUICommandHandler*>(this);
    } else {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }

    AddRef();
    return S_OK;
}

STDMETHODIMP touchmind::ribbon::handler::FontControlCommandHandler::UpdateProperty(
    UINT nCmdID,
    IN REFPROPERTYKEY key,
    IN const PROPVARIANT* ppropvarCurrentValue,
    OUT PROPVARIANT* ppropvarNewValue)
{
    LOG_ENTER;
    UNREFERENCED_PARAMETER(nCmdID);

    HRESULT hr = E_NOTIMPL;
    if (key == UI_PKEY_FontProperties) {
        hr = E_POINTER;
        if (ppropvarCurrentValue != nullptr) {
            IPropertyStore *pValues;
            hr = UIPropertyToInterface(UI_PKEY_FontProperties, *ppropvarCurrentValue, &pValues);
            if (SUCCEEDED(hr)) {
                m_pEditControlManager->GetValues(pValues);
                hr = UIInitPropertyFromInterface(UI_PKEY_FontProperties, pValues, ppropvarNewValue);
                pValues->Release();
            }
        }
    }
    LOG_LEAVE_HRESULT(hr);
    return hr;
}

STDMETHODIMP touchmind::ribbon::handler::FontControlCommandHandler::Execute(
    UINT nCmdID,
    UI_EXECUTIONVERB verb,
    IN const PROPERTYKEY* key,
    IN const PROPVARIANT* ppropvarValue,
    IN IUISimplePropertySet* pCommandExecutionProperties)
{
    LOG_ENTER;
    UNREFERENCED_PARAMETER(nCmdID);

    HRESULT hr = E_NOTIMPL;
    if ((key) && (*key == UI_PKEY_FontProperties)) {
        switch (verb) {
        case UI_EXECUTIONVERB_EXECUTE: {
            hr = E_POINTER;
            if (pCommandExecutionProperties != nullptr) {
                PROPVARIANT varChanges;
                hr = pCommandExecutionProperties->GetValue(UI_PKEY_FontProperties_ChangedProperties, &varChanges);
                if (SUCCEEDED(hr)) {
                    IPropertyStore *pChanges;
                    hr = UIPropertyToInterface(UI_PKEY_FontProperties, varChanges, &pChanges);
                    if (SUCCEEDED(hr)) {
                        m_pEditControlManager->SetValues(pChanges);
                        pChanges->Release();
                    }
                    PropVariantClear(&varChanges);
                }
            }
            break;
        }
        case UI_EXECUTIONVERB_PREVIEW: {
            hr = E_POINTER;
            if (pCommandExecutionProperties != nullptr) {
                PROPVARIANT varChanges;
                hr = pCommandExecutionProperties->GetValue(UI_PKEY_FontProperties_ChangedProperties, &varChanges);
                if (SUCCEEDED(hr)) {
                    IPropertyStore *pChanges;
                    hr = UIPropertyToInterface(UI_PKEY_FontProperties, varChanges, &pChanges);
                    if (SUCCEEDED(hr)) {
                        m_pEditControlManager->SetPreviewValues(pChanges);
                        pChanges->Release();
                    }
                    PropVariantClear(&varChanges);
                }
            }
            break;
        }
        case UI_EXECUTIONVERB_CANCELPREVIEW: {
            hr = E_POINTER;
            if (ppropvarValue != nullptr) {
                IPropertyStore *pValues;
                hr = UIPropertyToInterface(UI_PKEY_FontProperties, *ppropvarValue, &pValues);
                if (SUCCEEDED(hr)) {
                    m_pEditControlManager->CancelPreview(pValues);
                    pValues->Release();
                }
            }
            break;
        }
        }
    }
    LOG_LEAVE_HRESULT(hr);
    return hr;
}
