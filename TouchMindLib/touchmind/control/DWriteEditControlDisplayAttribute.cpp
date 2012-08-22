#include "StdAfx.h"
#include "DWriteEditControlDisplayAttribute.h"


touchmind::control::DWriteEditControlDisplayAttribute::DWriteEditControlDisplayAttribute(void) :
    m_pDisplayAttributeMgr(nullptr),
    m_pCategoryMgr(nullptr)
{
}

touchmind::control::DWriteEditControlDisplayAttribute::~DWriteEditControlDisplayAttribute(void)
{
}

HRESULT touchmind::control::DWriteEditControlDisplayAttribute::GetDisplayAttributeProperties( OUT DisplayAttributeProperties& displayAttributeProperties )
{
    IEnumGUID *pEnumProp = nullptr;
    HRESULT hr = E_FAIL;
    hr = m_pCategoryMgr->EnumItemsInCategory(GUID_TFCAT_DISPLAYATTRIBUTEPROPERTY, &pEnumProp);
    if (SUCCEEDED(hr) && pEnumProp) {
        GUID guidProp;
        displayAttributeProperties.Add(GUID_PROP_ATTRIBUTE);
        while (pEnumProp->Next(1, &guidProp, nullptr) == S_OK) {
            if (!IsEqualGUID(guidProp, GUID_PROP_ATTRIBUTE))
                displayAttributeProperties.Add(guidProp);
        }
    }
    SafeRelease(&pEnumProp);
    return hr;
}

HRESULT touchmind::control::DWriteEditControlDisplayAttribute::GetDisplayAttributeTrackPropertyRange(
    IN ITfContext *pTfContext,
    IN DisplayAttributeProperties& displayAttributeProperties,
    OUT ITfReadOnlyProperty **ppProp )
{
    ITfReadOnlyProperty *pProp = nullptr;
    HRESULT hr = E_FAIL;
    GUID  *pguidProp = nullptr;
    const GUID **ppguidProp;

    pguidProp = displayAttributeProperties.GetPropTable();
    if (!pguidProp) {
        return E_FAIL;
    }

    size_t ulNumProp = displayAttributeProperties.Count();
    if (!ulNumProp) {
        return E_FAIL;
    }

    if ((ppguidProp = (const GUID **)LocalAlloc(LMEM_ZEROINIT, sizeof(GUID *) * ulNumProp)) == nullptr) {
        return E_OUTOFMEMORY;
    }

    for (size_t i = 0; i < ulNumProp; ++i) {
        ppguidProp[i] = pguidProp++;
    }

    hr = pTfContext->TrackProperties(ppguidProp, static_cast<ULONG>(ulNumProp), 0, 0, &pProp);
    if (SUCCEEDED(hr)) {
        *ppProp = pProp;
    }
    LocalFree(ppguidProp);
    return hr;
}

HRESULT touchmind::control::DWriteEditControlDisplayAttribute::GetDisplayAttributeData(
    IN TfEditCookie editCookie,
    IN ITfReadOnlyProperty *pProp,
    IN ITfRange *pRange,
    OUT TF_DISPLAYATTRIBUTE* pDisplayAttribute,
    OUT TfGuidAtom* pGuid )
{
    VARIANT var;
    IEnumTfPropertyValue *pEnumPropertyVal;
    TF_PROPERTYVAL tfPropVal;
    GUID guid;
    TfGuidAtom gaVal;
    ITfDisplayAttributeInfo *pDAI;

    HRESULT hr = E_FAIL;

    hr = pProp->GetValue(editCookie, pRange, &var);
    if (SUCCEEDED(hr)) {
        hr = var.punkVal->QueryInterface(IID_IEnumTfPropertyValue,
                                         (void **)&pEnumPropertyVal);
        if (SUCCEEDED(hr)) {
            while (pEnumPropertyVal->Next(1, &tfPropVal, nullptr) == S_OK) {
                if (tfPropVal.varValue.vt == VT_EMPTY) {
                    continue;
                }

                gaVal = (TfGuidAtom)tfPropVal.varValue.lVal;

                m_pCategoryMgr->GetGUID(gaVal, &guid);
                hr = m_pDisplayAttributeMgr->GetDisplayAttributeInfo(guid, &pDAI, nullptr);

                if (SUCCEEDED(hr)) {
                    hr = pDAI->GetAttributeInfo(pDisplayAttribute);
                    if (SUCCEEDED(hr)) {
                        *pGuid = gaVal;
                    }
                    pDAI->Release();
                }
            }
            pEnumPropertyVal->Release();
        }
        VariantClear(&var);
    }

    return hr;
}

HRESULT touchmind::control::DWriteEditControlDisplayAttribute::GetAttributeColor( IN const TF_DA_COLOR &daColor, OUT COLORREF &colorref )
{
    switch (daColor.type) {
    case TF_CT_SYSCOLOR:
        colorref = GetSysColor(daColor.nIndex);
        break;

    case TF_CT_COLORREF:
        colorref = daColor.cr;
        break;
    }
    colorref = GetSysColor(COLOR_WINDOWTEXT);
    return S_OK;
}
