#include "stdafx.h"
#include <Strsafe.h>
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/util/OSVersionChecker.h"
#include "touchmind/shell/MRUManager.h"

const size_t touchmind::shell::MRUManager::RECENT_FILE_COUNT = 16;

class RecentFileProperties : public IUISimplePropertySet
{
private:
    LONG m_cRef;
    WCHAR m_wszDisplayName[MAX_PATH];
    WCHAR m_wszFullPath[MAX_PATH];

    RecentFileProperties() :
        m_cRef(1) {
        m_wszFullPath[0] = L'\0';
        m_wszDisplayName[0] = L'\0';
    }

public:
    __checkReturn static HRESULT CreateInstance(
        __in PWSTR wszDisplyName,
        __in PWSTR wszFullPath,
        __deref_out_opt RecentFileProperties ** ppProperties) {
        if (!wszDisplyName || !wszFullPath || !ppProperties) {
            return E_POINTER;
        }

        *ppProperties = nullptr;

        HRESULT hr;

        RecentFileProperties *pProperties = new RecentFileProperties();

        if (pProperties != nullptr) {
            hr =::StringCchCopyW(pProperties->m_wszFullPath, MAX_PATH, wszFullPath);
            if (SUCCEEDED(hr)) {
                hr =::StringCchCopyW(pProperties->m_wszDisplayName, MAX_PATH, wszDisplyName);
            }
        } else {
            hr = E_OUTOFMEMORY;
        }

        if (SUCCEEDED(hr)) {
            *ppProperties = pProperties;
            (*ppProperties)->AddRef();
        }

        if (pProperties) {
            pProperties->Release();
        }

        return hr;
    }

    STDMETHODIMP_(ULONG) AddRef() {
        return InterlockedIncrement(&m_cRef);
    }

    STDMETHODIMP_(ULONG) Release() {
        LONG cRef = InterlockedDecrement(&m_cRef);
        if (cRef == 0) {
            delete this;
        }
        return cRef;
    }

    STDMETHODIMP QueryInterface(
        REFIID iid,
        void **ppv) {
        if (!ppv) {
            return E_POINTER;
        }

        if (iid == __uuidof(IUnknown)) {
            *ppv = static_cast < IUnknown * >(this);
        } else if (iid == __uuidof(IUISimplePropertySet)) {
            *ppv = static_cast < IUISimplePropertySet * >(this);
        } else {
            *ppv = nullptr;
            return E_NOINTERFACE;
        }

        AddRef();
        return S_OK;
    }

    STDMETHODIMP GetValue(
        __in REFPROPERTYKEY key,
        __out PROPVARIANT * value) {
        HRESULT hr = HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

        if (key == UI_PKEY_Label) {
            hr = UIInitPropertyFromString(UI_PKEY_Label, m_wszDisplayName, value);
        } else if (key == UI_PKEY_LabelDescription) {
            hr = UIInitPropertyFromString(UI_PKEY_LabelDescription, m_wszFullPath, value);
        }

        return hr;
    }
};

touchmind::shell::MRUManager::MRUManager(void)
{
}

touchmind::shell::MRUManager::~MRUManager(void)
{
}

HRESULT touchmind::shell::MRUManager::PopulateRibbonRecentItems(
    __deref_out PROPVARIANT * pvarValue)
{
    if (!touchmind::util::OSVersionChecker::IsWin7()) {
        return S_OK;
    }

    HRESULT hr = S_OK;

    IApplicationDocumentLists *pDocumentLists = nullptr;
    hr = CoCreateInstance(CLSID_ApplicationDocumentLists, nullptr, CLSCTX_INPROC_SERVER,
                          IID_PPV_ARGS(&pDocumentLists));

    IObjectArray *pShellItemArray = nullptr;
    pDocumentLists->GetList(ADLT_RECENT, RECENT_FILE_COUNT, IID_IObjectArray, (void **)&pShellItemArray);

    UINT iFileCount = 0;
    hr = pShellItemArray->GetCount(&iFileCount);
    LONG iCurrentFile = 0;

    SAFEARRAY *psa = SafeArrayCreateVector(VT_UNKNOWN, 0, iFileCount);

    if (psa != nullptr) {
        m_mruDisplayNameList.clear();
        m_mruPathList.clear();
        while (iCurrentFile < static_cast < LONG > (iFileCount)) {
            IShellItem *pShellItem = nullptr;
            hr = pShellItemArray->GetAt(iCurrentFile, IID_IShellItem, (void **)&pShellItem);
            // display name
            LPWSTR pDisplayName = nullptr;
            hr = pShellItem->GetDisplayName(SIGDN_NORMALDISPLAY, &pDisplayName);
            std::wstring displayName(pDisplayName);
            m_mruDisplayNameList.push_back(displayName);
            // path
            LPWSTR pFullPath = nullptr;
            hr = pShellItem->GetDisplayName(SIGDN_FILESYSPATH, &pFullPath);
            std::wstring fullPath(pFullPath);
            m_mruPathList.push_back(fullPath);

            RecentFileProperties *pPropertiesObj = nullptr;
            hr = RecentFileProperties::CreateInstance(pDisplayName, pFullPath, &pPropertiesObj);
            CoTaskMemFree(pDisplayName);
            CoTaskMemFree(pFullPath);

            if (SUCCEEDED(hr)) {
                IUnknown *pUnknown = nullptr;
                hr = pPropertiesObj->QueryInterface(__uuidof(IUnknown),
                                                    reinterpret_cast < void **>(&pUnknown));
                if (SUCCEEDED(hr)) {
                    hr = SafeArrayPutElement(psa, &iCurrentFile, static_cast < void *>(pUnknown));
                    pUnknown->Release();
                }
            }

            if (pPropertiesObj) {
                pPropertiesObj->Release();
            }

            if (FAILED(hr)) {
                break;
            }

            iCurrentFile++;
        }
    }
    SAFEARRAYBOUND sab = { iCurrentFile, 0 };
    SafeArrayRedim(psa, &sab);
    hr = UIInitPropertyFromIUnknownArray(UI_PKEY_RecentItems, psa, pvarValue);

    SafeArrayDestroy(psa);

    SafeRelease(&pShellItemArray);
    SafeRelease(&pDocumentLists);
    return hr;
}

HRESULT touchmind::shell::MRUManager::GetSelectedItem(
    IN const PROPERTYKEY * key,
    IN const PROPVARIANT * ppropvarValue,
    OUT std::wstring & displayName,
    OUT std::wstring & fullPath)
{
    HRESULT hr = S_OK;
    UINT uSelectedMRUItem = RECENT_FILE_COUNT;
    if (ppropvarValue != nullptr && SUCCEEDED(UIPropertyToUInt32(*key, *ppropvarValue, &uSelectedMRUItem))) {
        if (uSelectedMRUItem < m_mruDisplayNameList.size()) {
            displayName = m_mruDisplayNameList[uSelectedMRUItem];
            fullPath = m_mruPathList[uSelectedMRUItem];
        }
    }

    return hr;
}
