#include "StdAfx.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/util/Utils.h"
#include "touchmind/util/CoordinateConversion.h"
#include "touchmind/control/DWriteEditControlTextStoreACP.h"

touchmind::control::DWriteEditControlTextStoreACP::DWriteEditControlTextStoreACP(std::weak_ptr<DWriteEditControl> pEditControl) :
    m_pEditControl(pEditControl),
    m_hWnd(0),
    m_ObjRefCount(0),
    m_pServices(nullptr),
    m_fLocked(FALSE),
    m_fNotify(FALSE),
    m_acpStart(0),
    m_acpEnd(0),
    m_dwInternalLockType(0),
    m_fInterimChar(FALSE),
    m_ActiveSelEnd(TS_AE_START)
{
    m_AdviseSink.punkID = nullptr;
    m_AdviseSink.dwMask = 0;
    m_AdviseSink.pTextStoreACPSink = nullptr;
    std::shared_ptr<control::DWriteEditControl> editControl = pEditControl.lock();
    m_hWnd = editControl->GetHWnd();
    InitializeAttributes();
}

touchmind::control::DWriteEditControlTextStoreACP::~DWriteEditControlTextStoreACP(void)
{
}

HRESULT touchmind::control::DWriteEditControlTextStoreACP::Initialize()
{
    HRESULT hr = S_OK;
    return hr;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::QueryInterface(REFIID riid, LPVOID *ppReturn)
{
    *ppReturn = nullptr;

    //IUnknown
    if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_ITextStoreACP)) {
        *ppReturn = (ITextStoreACP*) this;
    }

    //ITfContextOwnerCompositionSink
    else if (IsEqualIID(riid, IID_ITfContextOwnerCompositionSink)) {
        *ppReturn = (ITfContextOwnerCompositionSink*) this;
    }

    /*
    //ITfFunctionProvider
    else if (IsEqualIID(riid, IID_ITfFunctionProvider)) {
        *ppReturn = (ITfFunctionProvider*)this;
    }
    */

    if (*ppReturn) {
        (*(LPUNKNOWN*) ppReturn)->AddRef();
        return S_OK;
    }
    LOG(SEVERITY_LEVEL_ERROR) << L"E_NOINTERFACE";
    return E_NOINTERFACE;
}

STDMETHODIMP_(DWORD) touchmind::control::DWriteEditControlTextStoreACP::AddRef()
{
    return ++m_ObjRefCount;
}


STDMETHODIMP_(DWORD) touchmind::control::DWriteEditControlTextStoreACP::Release()
{
    if (--m_ObjRefCount == 0) {
        delete this;
        return 0;
    }
    return m_ObjRefCount;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::AdviseSink(REFIID riid, IUnknown *pUnknown, DWORD dwMask)
{
    HRESULT     hr;
    IUnknown    *punkID;

    hr = pUnknown->QueryInterface(IID_IUnknown, (LPVOID*) &punkID);
    if (FAILED(hr)) {
        return hr;
    }

    hr = E_INVALIDARG;

    if (punkID == m_AdviseSink.punkID) {
        m_AdviseSink.dwMask = dwMask;

        hr = S_OK;
    } else if (nullptr != m_AdviseSink.punkID) {
        hr = CONNECT_E_ADVISELIMIT;
    } else if (IsEqualIID(riid, IID_ITextStoreACPSink)) {
        m_AdviseSink.dwMask = dwMask;
        m_AdviseSink.punkID = punkID;

        punkID->AddRef();
        pUnknown->QueryInterface(IID_ITextStoreACPSink, (LPVOID*) &m_AdviseSink.pTextStoreACPSink);
        pUnknown->QueryInterface(IID_ITextStoreACPServices, (LPVOID*) &m_pServices);
        hr = S_OK;
    }

    punkID->Release();
    return hr;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::UnadviseSink(IUnknown *pUnknown)
{
    HRESULT     hr;
    IUnknown    *punkID;

    hr = pUnknown->QueryInterface(IID_IUnknown, (LPVOID*) &punkID);
    if (FAILED(hr)) {
        return hr;
    }

    if (punkID == m_AdviseSink.punkID) {
        _ClearAdviseSink(&m_AdviseSink);

        if (m_pServices) {
            m_pServices->Release();
            m_pServices = nullptr;
        }

        hr = S_OK;
    } else {
        hr = CONNECT_E_NOCONNECTION;
    }

    punkID->Release();
    return hr;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::RequestLock(DWORD dwLockFlags, HRESULT *phrSession)
{
    if (nullptr == m_AdviseSink.pTextStoreACPSink) {
        LOG(SEVERITY_LEVEL_ERROR) << L"E_UNEXPECTED";
        return E_UNEXPECTED;
    }

    if (nullptr == phrSession) {
        LOG(SEVERITY_LEVEL_ERROR) << L"E_INVALIDARG";
        return E_INVALIDARG;
    }

    *phrSession = E_FAIL;
    HRESULT hr = S_OK;

    if (m_fLocked) {
        if (dwLockFlags & TS_LF_SYNC) {
            *phrSession = TS_E_SYNCHRONOUS;
            hr = S_OK;
        } else {
            if (((m_dwLockType & TS_LF_READWRITE) == TS_LF_READ) &&
                    ((dwLockFlags & TS_LF_READWRITE) == TS_LF_READWRITE)) {
                m_fPendingLockUpgrade = TRUE;

                *phrSession = TS_S_ASYNC;

                hr = S_OK;
            } else {
                hr = E_FAIL;
            }

        }
    } else {
        _LockDocument(dwLockFlags);
        *phrSession = m_AdviseSink.pTextStoreACPSink->OnLockGranted(dwLockFlags);
        _UnlockDocument();
    }

    std::shared_ptr<control::DWriteEditControl> pEditControl = m_pEditControl.lock();
    while (!m_messageQueue.IsEmpty()) {
        tsf::Message msg;
        m_messageQueue.Pop(&msg);
        pEditControl->WndProc(msg.hWnd, msg.message, msg.wParam, msg.lParam);
    }
    return hr;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::GetStatus(TS_STATUS *pdcs)
{
    if (nullptr == pdcs) {
        return E_INVALIDARG;
    }
    pdcs->dwDynamicFlags = 0;
    pdcs->dwStaticFlags = 0;
    return S_OK;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::QueryInsert(LONG acpTestStart,
        LONG acpTestEnd,
        ULONG cch,
        LONG *pacpResultStart,
        LONG *pacpResultEnd)
{
    return _TestInsert(acpTestStart, acpTestEnd, cch, pacpResultStart, pacpResultEnd);
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::GetSelection(ULONG ulIndex,
        ULONG ulCount,
        TS_SELECTION_ACP *pSelection,
        ULONG *pcFetched)
{
    UNREFERENCED_PARAMETER(ulCount);

    if (nullptr == pSelection) {
        return E_INVALIDARG;
    }

    if (nullptr == pcFetched) {
        return E_INVALIDARG;
    }

    *pcFetched = 0;

    if (!_IsLocked(TS_LF_READ)) {
        return TS_E_NOLOCK;
    }

    if (TF_DEFAULT_SELECTION == ulIndex) {
        ulIndex = 0;
    } else if (ulIndex > 1) {
        return E_INVALIDARG;
    }

    pSelection[0].acpStart = m_acpStart;
    pSelection[0].acpEnd = m_acpEnd;
    pSelection[0].style.fInterimChar = m_fInterimChar;
    if (m_fInterimChar) {
        pSelection[0].style.ase = TS_AE_NONE;
    } else {
        pSelection[0].style.ase = m_ActiveSelEnd;
    }
    *pcFetched = 1;

    return S_OK;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::SetSelection(ULONG ulCount,
        const TS_SELECTION_ACP *pSelection)
{
    if (nullptr == pSelection) {
        return E_INVALIDARG;
    }

    if (ulCount > 1) {
        return E_INVALIDARG;
    }

    if (!_IsLocked(TS_LF_READWRITE)) {
        return TS_E_NOLOCK;
    }

    m_acpStart = pSelection[0].acpStart;
    m_acpEnd = pSelection[0].acpEnd;
    m_fInterimChar = pSelection[0].style.fInterimChar;
    if (m_fInterimChar) {
        m_ActiveSelEnd = TS_AE_NONE;
    } else {
        m_ActiveSelEnd = pSelection[0].style.ase;
    }

    LONG    lStart = m_acpStart;
    LONG    lEnd = m_acpEnd;

    if (TS_AE_START == m_ActiveSelEnd) {
        lStart = m_acpEnd;
        lEnd = m_acpStart;
    }

    m_fNotify = TRUE;

    return S_OK;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::GetText(IN LONG acpStart,
        IN LONG acpEnd,
        OUT WCHAR *pchPlain,
        IN ULONG cchPlainReq,
        OUT ULONG *pcchPlainRet,
        OUT TS_RUNINFO *prgRunInfo,
        IN ULONG ulRunInfoReq,
        OUT ULONG *pulRunInfoOut,
        OUT LONG *pacpNext)
{
    if (!_IsLocked(TS_LF_READ)) {
        return TS_E_NOLOCK;
    }

    BOOL    fDoText = cchPlainReq > 0;
    BOOL    fDoRunInfo = ulRunInfoReq > 0;
    LONG    cchTotal;
    HRESULT hr = E_FAIL;

    if (pcchPlainRet) {
        *pcchPlainRet  = 0;
    }

    if (fDoRunInfo) {
        *pulRunInfoOut = 0;
    }

    if (pacpNext) {
        *pacpNext = acpStart;
    }

    std::wstring text;
    _GetText(text, &cchTotal);

    if ((acpStart < 0) || (acpStart > cchTotal)) {
        hr = TS_E_INVALIDPOS;
    } else {
        if (acpStart == cchTotal) {
            hr = S_OK;
        } else {
            ULONG    cchReq;

            if (acpEnd >= acpStart) {
                cchReq = acpEnd - acpStart;
            } else {
                cchReq = cchTotal - acpStart;
            }

            if (fDoText) {
                if (cchReq > cchPlainReq) {
                    cchReq = cchPlainReq;
                }

                std::wstring substr = text.substr(acpStart, cchReq);

                if (pchPlain && cchPlainReq) {
                    CopyMemory(pchPlain, substr.c_str(), cchReq * sizeof(WCHAR));
                }
            }

            if (pcchPlainRet) {
                *pcchPlainRet  = cchReq;
            }

            if (fDoRunInfo) {
                *pulRunInfoOut = 1;
                prgRunInfo[0].type = TS_RT_PLAIN;
                prgRunInfo[0].uCount = cchReq;
            }

            if (pacpNext) {
                *pacpNext = acpStart + cchReq;
            }

            hr = S_OK;
        }
    }
    return hr;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::SetText(
    DWORD dwFlags,
    LONG acpStart,
    LONG acpEnd,
    const WCHAR *pchText,
    ULONG cch,
    TS_TEXTCHANGE *pChange)
{
    UNREFERENCED_PARAMETER(dwFlags);

    HRESULT hr = S_OK;

    TS_SELECTION_ACP    tsa;
    tsa.acpStart = acpStart;
    tsa.acpEnd = acpEnd;
    tsa.style.ase = TS_AE_START;
    tsa.style.fInterimChar = FALSE;

    hr = SetSelection(1, &tsa);

    if (SUCCEEDED(hr)) {
        hr = InsertTextAtSelection(TS_IAS_NOQUERY, pchText, cch, nullptr, nullptr, pChange);
    }
    return hr;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::GetFormattedText(
    LONG acpStart,
    LONG acpEnd,
    IDataObject **ppDataObject)
{
    UNREFERENCED_PARAMETER(acpStart);
    UNREFERENCED_PARAMETER(acpEnd);
    UNREFERENCED_PARAMETER(ppDataObject);
    return E_NOTIMPL;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::GetEmbedded(
    LONG acpPos,
    REFGUID rguidService,
    REFIID riid,
    IUnknown **ppunk)
{
    UNREFERENCED_PARAMETER(acpPos);
    UNREFERENCED_PARAMETER(rguidService);
    UNREFERENCED_PARAMETER(riid);
    UNREFERENCED_PARAMETER(ppunk);
    return E_NOTIMPL;
}


STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::QueryInsertEmbedded(
    const GUID *pguidService,
    const FORMATETC *pFormatEtc,
    BOOL *pfInsertable)
{
    UNREFERENCED_PARAMETER(pguidService);
    UNREFERENCED_PARAMETER(pFormatEtc);
    UNREFERENCED_PARAMETER(pfInsertable);

    *pfInsertable = FALSE;

    return E_NOTIMPL;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::InsertEmbedded(
    DWORD dwFlags,
    LONG acpStart,
    LONG acpEnd,
    IDataObject *pDataObject,
    TS_TEXTCHANGE *pChange)
{
    UNREFERENCED_PARAMETER(dwFlags);
    UNREFERENCED_PARAMETER(acpStart);
    UNREFERENCED_PARAMETER(acpEnd);
    UNREFERENCED_PARAMETER(pDataObject);
    UNREFERENCED_PARAMETER(pChange);

    return E_NOTIMPL;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::RequestSupportedAttrs(
    DWORD dwFlags,
    ULONG cFilterAttrs,
    const TS_ATTRID *paFilterAttrs)
{
    _ClearRequestedAttributes();

    for (int i = 0; i < NUM_SUPPORTED_ATTRS; ++i) {
        for (ULONG x = 0; x < cFilterAttrs; ++x) {
            if (IsEqualGUID(*m_rgAttributes[i].attrid, paFilterAttrs[x])) {
                m_rgAttributes[i].dwFlags = ATTR_FLAG_REQUESTED;
                if (dwFlags & TS_ATTR_FIND_WANT_VALUE) {
                    m_rgAttributes[i].dwFlags |= ATTR_FLAG_DEFAULT;
                } else {
                    VariantCopy(&m_rgAttributes[i].varValue, &m_rgAttributes[i].varDefaultValue);
                }
            }
        }
    }
    return S_OK;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::RequestAttrsAtPosition(
    LONG acpPos,
    ULONG cFilterAttrs,
    const TS_ATTRID *paFilterAttrs,
    DWORD dwFlags)
{
    UNREFERENCED_PARAMETER(acpPos);

    _ClearRequestedAttributes();

    for (int i = 0; i < NUM_SUPPORTED_ATTRS; ++i) {
        for (ULONG x = 0; x < cFilterAttrs; ++x) {
            if (IsEqualGUID(*m_rgAttributes[i].attrid, paFilterAttrs[x])) {
                m_rgAttributes[i].dwFlags = ATTR_FLAG_REQUESTED;
                if (dwFlags & TS_ATTR_FIND_WANT_VALUE) {
                    m_rgAttributes[i].dwFlags |= ATTR_FLAG_DEFAULT;
                } else {
                    VariantCopy(&m_rgAttributes[i].varValue, &m_rgAttributes[i].varDefaultValue);
                }
            }
        }
    }
    return S_OK;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::RequestAttrsTransitioningAtPosition(
    LONG acpPos,
    ULONG cFilterAttrs,
    const TS_ATTRID *paFilterAttrs,
    DWORD dwFlags)
{
    UNREFERENCED_PARAMETER(acpPos);
    UNREFERENCED_PARAMETER(cFilterAttrs);
    UNREFERENCED_PARAMETER(paFilterAttrs);
    UNREFERENCED_PARAMETER(dwFlags);

    return E_NOTIMPL;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::FindNextAttrTransition(
    LONG acpStart,
    LONG acpHalt,
    ULONG cFilterAttrs,
    const TS_ATTRID *paFilterAttrs,
    DWORD dwFlags,
    LONG *pacpNext,
    BOOL *pfFound,
    LONG *plFoundOffset)
{
    UNREFERENCED_PARAMETER(acpStart);
    UNREFERENCED_PARAMETER(acpHalt);
    UNREFERENCED_PARAMETER(cFilterAttrs);
    UNREFERENCED_PARAMETER(paFilterAttrs);
    UNREFERENCED_PARAMETER(dwFlags);
    UNREFERENCED_PARAMETER(pacpNext);
    UNREFERENCED_PARAMETER(pfFound);
    UNREFERENCED_PARAMETER(plFoundOffset);

    return E_NOTIMPL;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::RetrieveRequestedAttrs(ULONG ulCount,
        TS_ATTRVAL *paAttrVals,
        ULONG *pcFetched)
{
    ULONG   uFetched = 0;
    for (int i = 0; i < NUM_SUPPORTED_ATTRS && ulCount; ++i) {
        if (m_rgAttributes[i].dwFlags & ATTR_FLAG_REQUESTED) {
            paAttrVals->varValue.vt = VT_EMPTY;
            CopyMemory(&paAttrVals->idAttr, m_rgAttributes[i].attrid, sizeof(GUID));
            paAttrVals->dwOverlapId = 0;

            if (m_rgAttributes[i].dwFlags & ATTR_FLAG_DEFAULT) {
                VariantCopy(&paAttrVals->varValue, &m_rgAttributes[i].varDefaultValue);
            } else {
                VariantCopy(&paAttrVals->varValue, &m_rgAttributes[i].varValue);
            }

            paAttrVals++;
            uFetched++;
            ulCount--;

            VariantClear(&m_rgAttributes[i].varValue);
            m_rgAttributes[i].dwFlags = ATTR_FLAG_NONE;
        }
    }
    *pcFetched = uFetched;
    return S_OK;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::GetEndACP(LONG *pacp)
{
    if (!_IsLocked(TS_LF_READWRITE)) {
        return TS_E_NOLOCK;
    }

    if (nullptr == pacp) {
        return E_INVALIDARG;
    }

    *pacp = static_cast<LONG>(m_text.length());
    return S_OK;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::GetActiveView(TsViewCookie *pvcView)
{
    *pvcView = EDIT_VIEW_COOKIE;
    return S_OK;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::GetACPFromPoint(
    TsViewCookie vcView,
    const POINT *pt,
    DWORD dwFlags,
    LONG *pacp)
{
    UNREFERENCED_PARAMETER(vcView);
    UNREFERENCED_PARAMETER(pt);
    UNREFERENCED_PARAMETER(dwFlags);
    UNREFERENCED_PARAMETER(pacp);
    return E_NOTIMPL;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::GetTextExt(TsViewCookie vcView,
        LONG acpStart,
        LONG acpEnd,
        RECT *prc,
        BOOL *pfClipped)
{
    if (nullptr == prc || nullptr == pfClipped) {
        return E_INVALIDARG;
    }

    *pfClipped = FALSE;
    ZeroMemory(prc, sizeof(RECT));

    if (EDIT_VIEW_COOKIE != vcView) {
        return E_INVALIDARG;
    }

    if (!_IsLocked(TS_LF_READ)) {
        return TS_E_NOLOCK;
    }

    UINT32 actualCount;
    std::shared_ptr<control::DWriteEditControl> editControl = m_pEditControl.lock();
    if (editControl->GetTextLayout()->HitTestTextRange(
                acpStart,
                acpEnd - acpStart + 1,
                editControl->GetLayoutRect().left,
                editControl->GetLayoutRect().top,
                nullptr,
                0,
                &actualCount)
            != E_NOT_SUFFICIENT_BUFFER) {
        return E_INVALIDARG;
    }

    std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(actualCount);
    editControl->GetTextLayout()->HitTestTextRange(
        acpStart,
        acpEnd - acpStart + 1,
        editControl->GetLayoutRect().left,
        editControl->GetLayoutRect().top,
        hitTestMetrics.data(),
        actualCount,
        &actualCount);

    if (actualCount > 0) {
        D2D1_POINT_2F p1;
        p1.x = hitTestMetrics[0].left;
        p1.y = hitTestMetrics[0].top;
        D2D1_POINT_2F p2;
        p2.x = (hitTestMetrics[0].left + hitTestMetrics[0].width);
        p2.y = (hitTestMetrics[0].top + hitTestMetrics[0].height);

        POINT pp1;
        touchmind::util::CoordinateConversion::ConvertModelToWindowCoordinate(m_hWnd, editControl->GetScrollBarHelper(),
                p1, &pp1);
        POINT pp2;
        touchmind::util::CoordinateConversion::ConvertModelToWindowCoordinate(m_hWnd, editControl->GetScrollBarHelper(),
                p2, &pp2);

        ClientToScreen(m_hWnd, &pp1);
        ClientToScreen(m_hWnd, &pp2);

        prc->left = pp1.x;
        prc->top = pp1.y;
        prc->right = pp2.x;
        prc->bottom = pp2.y;
        pfClipped = FALSE;
    }

    return S_OK;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::GetScreenExt(TsViewCookie vcView, RECT *prc)
{
    if (nullptr == prc) {
        return E_INVALIDARG;
    }

    ZeroMemory(prc, sizeof(RECT));

    if (EDIT_VIEW_COOKIE != vcView) {
        return E_INVALIDARG;
    }

    return S_OK;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::GetWnd(TsViewCookie vcView, HWND *phwnd)
{
    if (EDIT_VIEW_COOKIE == vcView) {
        *phwnd = _GetWindow();
        return S_OK;
    }
    return E_INVALIDARG;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::InsertTextAtSelection(
    DWORD dwFlags,
    const WCHAR *pwszText, // null if cch is 0
    ULONG cch,
    LONG *pacpStart,
    LONG *pacpEnd,
    TS_TEXTCHANGE *pChange)
{
    LONG    lTemp;

    if (!_IsLocked(TS_LF_READWRITE)) {
        return TS_E_NOLOCK;
    }

    if (cch > 0L && nullptr == pwszText) {
        return E_POINTER;
    }

    if (nullptr == pacpStart) {
        pacpStart = &lTemp;
    }

    if (nullptr == pacpEnd) {
        pacpEnd = &lTemp;
    }

    LONG    acpStart;
    LONG    acpOldEnd;
    LONG    acpNewEnd;

    _GetCurrentSelection();

    acpOldEnd = m_acpEnd;
    HRESULT hr = _TestInsert(m_acpStart, m_acpEnd, cch, &acpStart, &acpNewEnd);
    if (FAILED(hr)) {
        return hr;
    }

    if (dwFlags & TS_IAS_QUERYONLY) {
        *pacpStart = acpStart;
        *pacpEnd = acpNewEnd;
        return S_OK;
    }

    m_fNotify = FALSE;

    std::wstring s(pwszText, cch);

    if ((acpOldEnd - acpStart > 0) && (acpNewEnd - acpStart > 0)) {
        _Internal_ReplaceText(acpStart, acpOldEnd - acpStart, s);
    } else if (acpOldEnd - acpStart > 0) {
        _Internal_RemoveText(acpStart, acpOldEnd - acpStart);
    } else if (acpNewEnd - acpStart > 0) {
        _Internal_InsertText(acpStart, s);
    }

    m_fNotify = TRUE;

    _GetCurrentSelection();

    if (!(dwFlags & TS_IAS_NOQUERY)) {
        *pacpStart = acpStart;
        *pacpEnd = acpNewEnd;
    }

    pChange->acpStart = acpStart;
    pChange->acpOldEnd = acpOldEnd;
    pChange->acpNewEnd = acpNewEnd;

    m_fLayoutChanged = TRUE;

    Invalidate();
    NotifySelectionHasChanged();
    return S_OK;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::InsertEmbeddedAtSelection(DWORD dwFlags,
        IDataObject *pDataObject,
        LONG *pacpStart,
        LONG *pacpEnd,
        TS_TEXTCHANGE *pChange)
{
    UNREFERENCED_PARAMETER(dwFlags);
    UNREFERENCED_PARAMETER(pDataObject);
    UNREFERENCED_PARAMETER(pacpStart);
    UNREFERENCED_PARAMETER(pacpEnd);
    UNREFERENCED_PARAMETER(pChange);

    return E_NOTIMPL;
}

HRESULT touchmind::control::DWriteEditControlTextStoreACP::_ClearAdviseSink(PADVISE_SINK pAdviseSink)
{
    if (pAdviseSink->punkID) {
        pAdviseSink->punkID->Release();
        pAdviseSink->punkID = nullptr;
    }

    if (pAdviseSink->pTextStoreACPSink) {
        pAdviseSink->pTextStoreACPSink->Release();
        pAdviseSink->pTextStoreACPSink = nullptr;
    }

    pAdviseSink->dwMask = 0;

    return S_OK;
}

BOOL touchmind::control::DWriteEditControlTextStoreACP::_LockDocument(DWORD dwLockFlags)
{
    if (m_fLocked) {
        return FALSE;
    }

    m_fLocked = TRUE;
    m_dwLockType = dwLockFlags;

    return TRUE;
}

void touchmind::control::DWriteEditControlTextStoreACP::_UnlockDocument()
{
    HRESULT hr;

    m_fLocked = FALSE;
    m_dwLockType = 0;

    if (m_fPendingLockUpgrade) {
        m_fPendingLockUpgrade = FALSE;

        RequestLock(TS_LF_READWRITE, &hr);
    }

    if (m_fLayoutChanged) {
        m_fLayoutChanged = FALSE;
        m_AdviseSink.pTextStoreACPSink->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);
    }
}

BOOL touchmind::control::DWriteEditControlTextStoreACP::_IsLocked(DWORD dwLockType)
{
    if (m_dwInternalLockType) {
        return TRUE;
    }

    return m_fLocked && (m_dwLockType & dwLockType);
}

void touchmind::control::DWriteEditControlTextStoreACP::_ClearRequestedAttributes(void)
{
    int i;

    for (i = 0; i < NUM_SUPPORTED_ATTRS; ++i) {
        VariantClear(&m_rgAttributes[i].varValue);
        m_rgAttributes[i].dwFlags = ATTR_FLAG_NONE;
    }
}

BOOL touchmind::control::DWriteEditControlTextStoreACP::InitializeAttributes()
{
    m_rgAttributes[ATTR_INDEX_TEXT_ORIENTATION].dwFlags = 0;
    m_rgAttributes[ATTR_INDEX_TEXT_ORIENTATION].attrid = &TSATTRID_Text_Orientation;
    VariantInit(&m_rgAttributes[ATTR_INDEX_TEXT_ORIENTATION].varValue);
    m_rgAttributes[ATTR_INDEX_TEXT_ORIENTATION].varDefaultValue.vt = VT_I4;
    m_rgAttributes[ATTR_INDEX_TEXT_ORIENTATION].varDefaultValue.lVal = 0;

    m_rgAttributes[ATTR_INDEX_TEXT_VERTICALWRITING].dwFlags = 0;
    m_rgAttributes[ATTR_INDEX_TEXT_VERTICALWRITING].attrid = &TSATTRID_Text_VerticalWriting;
    VariantInit(&m_rgAttributes[ATTR_INDEX_TEXT_VERTICALWRITING].varValue);
    m_rgAttributes[ATTR_INDEX_TEXT_VERTICALWRITING].varDefaultValue.vt = VT_BOOL;
    m_rgAttributes[ATTR_INDEX_TEXT_VERTICALWRITING].varDefaultValue.lVal = FALSE;

    return TRUE;
}

BOOL touchmind::control::DWriteEditControlTextStoreACP::_GetCurrentSelection(void)
{
    return TRUE;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::_TestInsert(
    LONG acpTestStart,
    LONG acpTestEnd,
    ULONG cch,
    LONG *pacpResultStart,
    LONG *pacpResultEnd)
{
    if ( (acpTestStart > acpTestEnd) ||
            (acpTestStart > static_cast<LONG>(m_text.length()))) {
        LOG(SEVERITY_LEVEL_ERROR) << L"hr = E_INVALIDARG";
        return E_INVALIDARG;
    }

    *pacpResultStart = acpTestStart;
    *pacpResultEnd = acpTestStart + cch;
    return S_OK;
}


HWND touchmind::control::DWriteEditControlTextStoreACP::_GetWindow()
{
    return m_hWnd;
}

HRESULT touchmind::control::DWriteEditControlTextStoreACP::_GetText(std::wstring& text, LPLONG pcch)
{
    text = m_text;
    *pcch = static_cast<LONG>(m_text.length());
    return S_OK;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::OnStartComposition(ITfCompositionView *pComposition, BOOL *pfOk)
{
    UNREFERENCED_PARAMETER(pComposition);
    UNREFERENCED_PARAMETER(pfOk);

    *pfOk = TRUE;
    Invalidate();
    return S_OK;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::OnUpdateComposition(ITfCompositionView *pComposition, ITfRange *pRangeNew)
{
    UNREFERENCED_PARAMETER(pComposition);
    UNREFERENCED_PARAMETER(pRangeNew);

    Invalidate();
    return S_OK;
}

STDMETHODIMP touchmind::control::DWriteEditControlTextStoreACP::OnEndComposition(ITfCompositionView *pComposition)
{
    UNREFERENCED_PARAMETER(pComposition);
    Invalidate();
    return S_OK;
}

HRESULT touchmind::control::DWriteEditControlTextStoreACP::_TerminateAllCompositions(void)
{
    HRESULT hr;
    ITfContextOwnerCompositionServices *pCompServices;

    std::shared_ptr<control::DWriteEditControl> editControl = m_pEditControl.lock();
    hr = editControl->GetContext()->QueryInterface(IID_ITfContextOwnerCompositionServices, (LPVOID*) &pCompServices);
    if (SUCCEEDED(hr)) {
        hr = pCompServices->TerminateComposition(nullptr);

        pCompServices->Release();
    }

    return hr;
}

HRESULT touchmind::control::DWriteEditControlTextStoreACP::ChangeACPWithoutLock(LONG acpStart, LONG acpEnd, TsActiveSelEnd activeSelEnd)
{
    m_acpStart = acpStart;
    m_acpEnd = acpEnd;
    m_ActiveSelEnd = activeSelEnd;

    m_AdviseSink.pTextStoreACPSink->OnSelectionChange();
    return S_OK;
}

HRESULT touchmind::control::DWriteEditControlTextStoreACP::MoveACPToEnd()
{
    if (_IsLocked(TS_LF_READ)) {
        return S_OK;
    }
    _LockDocument(TS_LF_READWRITE);
    LONG textLength = static_cast<LONG>(GetTextLength());
    HRESULT hr = ChangeACPWithoutLock(textLength, textLength, TS_AE_START);
    _UnlockDocument();
    m_AdviseSink.pTextStoreACPSink->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);
    return hr;
}

void touchmind::control::DWriteEditControlTextStoreACP::Invalidate()
{
    std::shared_ptr<control::DWriteEditControl> editControl = m_pEditControl.lock();
    editControl->Invalidate();
}

void touchmind::control::DWriteEditControlTextStoreACP::Push(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    m_messageQueue.Push(hWnd, message, wParam, lParam);
}

HRESULT touchmind::control::DWriteEditControlTextStoreACP::InsertTextAtACP(std::wstring& text)
{
    if (_IsLocked(TS_LF_READ)) {
        return S_OK;
    }
    _LockDocument(TS_LF_READWRITE);

    bool changed = false;
    TS_TEXTCHANGE ts;
    if (m_acpStart == m_acpEnd) {
        _Internal_InsertText(m_acpStart, text);
        ts.acpStart = m_acpStart;
        ts.acpOldEnd = m_acpStart;
        ts.acpNewEnd = m_acpStart + static_cast<LONG>(text.length());
        changed = true;
        m_acpStart = m_acpEnd = m_acpStart + 1;
    } else {
        _Internal_ReplaceText(m_acpStart, m_acpEnd - m_acpStart, text);
        ts.acpStart = m_acpStart;
        ts.acpOldEnd = m_acpEnd;
        ts.acpNewEnd = m_acpStart + static_cast<LONG>(text.length());
        changed = true;
        m_acpStart = m_acpEnd = m_acpStart + static_cast<LONG>(text.length());
    }

    if (changed) {
        m_AdviseSink.pTextStoreACPSink->OnTextChange(0, &ts);
    }

    _UnlockDocument();

    m_AdviseSink.pTextStoreACPSink->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);
    return S_OK;
}

HRESULT touchmind::control::DWriteEditControlTextStoreACP::Backspace()
{
    if (_IsLocked(TS_LF_READ)) {
        return S_OK;
    }
    _LockDocument(TS_LF_READWRITE);

    bool changed = false;
    TS_TEXTCHANGE ts;
    if (m_acpStart == m_acpEnd) {
        if (m_acpStart > 0) {
            _Internal_RemoveText(m_acpStart - 1, 1);
            ts.acpStart = m_acpStart - 1;
            ts.acpOldEnd = m_acpStart;
            ts.acpNewEnd = m_acpStart - 1;
            changed = true;
            m_acpStart = m_acpEnd = m_acpStart - 1;
        }
    } else {
        _Internal_RemoveText(m_acpStart, m_acpEnd - m_acpStart);
        ts.acpStart = m_acpStart;
        ts.acpOldEnd = m_acpEnd;
        ts.acpNewEnd = m_acpStart;
        changed = true;
        m_acpEnd = m_acpStart;
    }

    if (changed) {
        m_AdviseSink.pTextStoreACPSink->OnTextChange(0, &ts);
    }

    _UnlockDocument();
    m_AdviseSink.pTextStoreACPSink->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);
    return S_OK;
}

HRESULT touchmind::control::DWriteEditControlTextStoreACP::Delete()
{
    if (_IsLocked(TS_LF_READ)) {
        return S_OK;
    }
    _LockDocument(TS_LF_READWRITE);

    bool changed = false;
    TS_TEXTCHANGE ts;
    if (m_acpStart == m_acpEnd) {
        if (m_acpStart < (LONG) m_text.length()) {
            _Internal_RemoveText(m_acpStart, 1);
            ts.acpStart = m_acpStart;
            ts.acpOldEnd = m_acpStart + 1;
            ts.acpNewEnd = m_acpStart;
            changed = true;
        }
    } else {
        _Internal_RemoveText(m_acpStart, m_acpEnd - m_acpStart);
        ts.acpStart = m_acpStart;
        ts.acpOldEnd = m_acpEnd;
        ts.acpNewEnd = m_acpStart;
        changed = true;
    }

    if (changed) {
        m_AdviseSink.pTextStoreACPSink->OnTextChange(0, &ts);
    }

    _UnlockDocument();

    m_AdviseSink.pTextStoreACPSink->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);
    return S_OK;
}

ITextStoreACPSink* touchmind::control::DWriteEditControlTextStoreACP::GetTextStoreACPSink()
{
    return m_AdviseSink.pTextStoreACPSink;
}

void touchmind::control::DWriteEditControlTextStoreACP::NotifySelectionHasChanged()
{
    std::shared_ptr<control::DWriteEditControl> editControl = m_pEditControl.lock();
    editControl->NotifySelectionHasChanged();
}

void touchmind::control::DWriteEditControlTextStoreACP::_Internal_InsertText(LONG startPos, std::wstring& s)
{
    LONG oldLength = static_cast<LONG>(m_text.length());
    m_text.insert(startPos, s);
    std::shared_ptr<control::DWriteEditControl> pEditControl = m_pEditControl.lock();
    pEditControl->NotifyTextInsert(oldLength, startPos, static_cast<LONG>(s.length()));
}

void touchmind::control::DWriteEditControlTextStoreACP::_Internal_RemoveText(LONG startPos, LONG length)
{
    LONG oldLength = static_cast<LONG>(m_text.length());
    m_text.erase(startPos, length);
    std::shared_ptr<control::DWriteEditControl> pEditControl = m_pEditControl.lock();
    pEditControl->NotifyTextRemove(oldLength, startPos, length);
}

void touchmind::control::DWriteEditControlTextStoreACP::_Internal_ReplaceText(LONG startPos, LONG length, std::wstring& s)
{
    LONG oldLength = static_cast<LONG>(m_text.length());
    m_text.replace(startPos, length, s);
    std::shared_ptr<control::DWriteEditControl> pEditControl = m_pEditControl.lock();
    pEditControl->NotifyTextReplace(oldLength, startPos, length, static_cast<LONG>(s.length()));
}
