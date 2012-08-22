#include "StdAfx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "DWriteEditControl.h"
#include "DWriteEditControlDisplayAttribute.h"
#include "DWriteEditControlTextEditSink.h"

touchmind::control::DWriteEditControlTextEditSink::DWriteEditControlTextEditSink(
    std::weak_ptr<DWriteEditControl> pEditControl) :
    m_ObjRefCount(0),
    m_pEditControl(pEditControl),
    m_displayAttribute(),
    m_editCookie(0)
{
}

touchmind::control::DWriteEditControlTextEditSink::~DWriteEditControlTextEditSink(void)
{
}

STDMETHODIMP touchmind::control::DWriteEditControlTextEditSink::QueryInterface(REFIID riid, LPVOID *ppReturn)
{
    *ppReturn = nullptr;

    if (IsEqualIID(riid, IID_IUnknown) ||
            IsEqualIID(riid, IID_ITfTextEditSink)) {
        *ppReturn = (ITfTextEditSink *)this;
    }

    if (*ppReturn) {
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;

}

STDMETHODIMP_(DWORD) touchmind::control::DWriteEditControlTextEditSink::AddRef()
{
    return ++m_ObjRefCount;
}


STDMETHODIMP_(DWORD) touchmind::control::DWriteEditControlTextEditSink::Release()
{
    if (--m_ObjRefCount == 0) {
        delete this;
        return 0;
    }
    return m_ObjRefCount;
}

STDAPI touchmind::control::DWriteEditControlTextEditSink::OnEndEdit(ITfContext *pic, TfEditCookie ecReadOnly, ITfEditRecord *pEditRecord)
{
    std::shared_ptr<control::DWriteEditControl> pEditControl = m_pEditControl.lock();
    DisplayAttributeProperties dispAttrProps;
    HRESULT hr = m_displayAttribute.GetDisplayAttributeProperties(dispAttrProps);
    if (SUCCEEDED(hr)) {
        IEnumTfRanges *pEnum;
        hr = pEditRecord->GetTextAndPropertyUpdates(TF_GTP_INCL_TEXT,
                dispAttrProps.GetPropTablePointer(),
                static_cast<ULONG>(dispAttrProps.Count()),
                &pEnum);
        if (SUCCEEDED(hr) && pEnum) {
            ITfRange *pRange;
            if (pEnum->Next(1, &pRange, nullptr) == S_OK) {
                pRange->Release();

                pEditControl->ClearCompositionRenderInfo();

                ITfRange *pRangeEntire = nullptr;
                ITfRange *pRangeEnd = nullptr;
                if (SUCCEEDED(pic->GetStart(ecReadOnly, &pRangeEntire)) &&
                        SUCCEEDED(pic->GetEnd(ecReadOnly, &pRangeEnd)) &&
                        SUCCEEDED(pRangeEntire->ShiftEndToRange(ecReadOnly, pRangeEnd, TF_ANCHOR_END))) {
                    IEnumTfRanges *pEnumRanges;
                    ITfReadOnlyProperty *pProp = nullptr;

                    m_displayAttribute.GetDisplayAttributeTrackPropertyRange(pic, dispAttrProps, &pProp);

                    if (SUCCEEDED(pProp->EnumRanges(ecReadOnly, &pEnumRanges, pRangeEntire))) {
                        while (pEnumRanges->Next(1, &pRange, nullptr) == S_OK) {
                            TF_DISPLAYATTRIBUTE da;
                            ZeroMemory(&da, sizeof(da));
                            TfGuidAtom guid;
                            if (m_displayAttribute.GetDisplayAttributeData(ecReadOnly, pProp, pRange, &da, &guid) == S_OK) {
                                ITfRangeACP *pRangeACP;
                                if (pRange->QueryInterface(IID_ITfRangeACP, (void **)&pRangeACP) == S_OK) {
                                    LONG nStart;
                                    LONG nEnd;
                                    pRangeACP->GetExtent(&nStart, &nEnd);
                                    pEditControl->AddCompositionRenderInfo(nStart, nStart + nEnd, &da);
                                    pRangeACP->Release();
                                }
                            }
                        }
                    }
                }

                if (pRangeEntire) {
                    pRangeEntire->Release();
                }
                if (pRangeEnd) {
                    pRangeEnd->Release();
                }

            }
            pEnum->Release();
        }
    }
    return S_OK;
}

void touchmind::control::DWriteEditControlTextEditSink::SetDisplayAttributeMgr( ITfDisplayAttributeMgr* pDisplayAttributeMgr )
{
    m_displayAttribute.SetDisplayAttributeMgr(pDisplayAttributeMgr);
}

void touchmind::control::DWriteEditControlTextEditSink::SetCategoryMgr( ITfCategoryMgr* pCategoryMgr )
{
    m_displayAttribute.SetCategoryMgr(pCategoryMgr);
}

HRESULT touchmind::control::DWriteEditControlTextEditSink::Advise(ITfContext *pTfContext)
{
    HRESULT hr = E_FAIL;

    ITfSource *source = nullptr;
    hr = pTfContext->QueryInterface(IID_ITfSource, (void **)&source);
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << L"QueryInterface failed";
    }

    if (SUCCEEDED(hr)) {
        hr = source->AdviseSink(IID_ITfTextEditSink, (ITfTextEditSink *)this, &m_editCookie);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"AdviseSink failed";
        }
    }
    SafeRelease(&source);
    //AddRef(); // don't need
    return hr;
}

HRESULT touchmind::control::DWriteEditControlTextEditSink::Unadvise( ITfContext *pTfContext )
{
    HRESULT hr = E_FAIL;
    if (pTfContext == nullptr) {
        return hr;
    }
    ITfSource *source = nullptr;
    hr = pTfContext->QueryInterface(IID_ITfSource, (void **)&source);
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << L"QueryInterface failed";
    }
    if (SUCCEEDED(hr)) {
        hr = source->UnadviseSink(m_editCookie);
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"UnadviseSink failed";
        }
    }
    SafeRelease(&source);
    return hr;
}
