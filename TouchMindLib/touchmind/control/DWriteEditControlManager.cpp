#include "StdAfx.h"
#include <Strsafe.h>
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/util/Utils.h"
#include "touchmind/Configuration.h"
#include "touchmind/util/CoordinateConversion.h"
#include "touchmind/control/DWriteEditControlManager.h"
#include "touchmind/control/DWriteEditControl.h"

touchmind::control::EDIT_CONTROL_INDEX touchmind::control::DWriteEditControlManager::s_masterIndex = 0;

touchmind::control::DWriteEditControlManager::DWriteEditControlManager() :
    m_parentHWnd(0),
    m_dwEditControlMap(),
    m_pSelectedEditControl(),
    m_pFocusedEditControl(),
    m_pTSFManager(nullptr),
    m_pScrollBarHelper(nullptr),
    m_defaultFontInfo(),
    m_pD2DFactory(nullptr),
    m_pRenderTarget(nullptr),
    m_pDWriteFactory(nullptr)
{
}

touchmind::control::DWriteEditControlManager::~DWriteEditControlManager(void)
{
#ifdef DEBUG_GPU_RESOURCE
    LOG_ENTER;
#endif

    m_dwEditControlMap.clear();
    m_pSelectedEditControl = nullptr;
    m_pFocusedEditControl = nullptr;

#ifdef DEBUG_GPU_RESOURCE
    LOG_LEAVE;
#endif
}

void touchmind::control::DWriteEditControlManager::Initialize(
    HWND hWnd,
    touchmind::Configuration* pConfiguration,
    touchmind::tsf::TSFManager *pTSFManager,
    ID2D1Factory *pD2DFactory,
    IDWriteFactory *pDWriteFactory )
{
    m_parentHWnd = hWnd;
    m_pConfiguration = pConfiguration;
    m_pTSFManager = pTSFManager;
    m_pD2DFactory = pD2DFactory;
    m_pDWriteFactory = pDWriteFactory;

    m_defaultFontInfo.fontFamilyNameStatus = UI_FONTPROPERTIES_SET;
    m_defaultFontInfo.fontFamilyName = pConfiguration->GetDefaultFontFamilyName();
    m_defaultFontInfo.localeNameStatus = UI_FONTPROPERTIES_SET;
    m_defaultFontInfo.localeName = pConfiguration->GetDefaultLocaleName();
    m_defaultFontInfo.fontSizeStatus = UI_FONTPROPERTIES_SET;
    m_defaultFontInfo.fontSize = pConfiguration->GetDefaultFontSize();
    m_defaultFontInfo.bold = UI_FONTPROPERTIES_NOTSET;
    m_defaultFontInfo.italic = UI_FONTPROPERTIES_NOTSET;
    m_defaultFontInfo.underline = UI_FONTPROPERTIES_NOTSET;
    m_defaultFontInfo.strikethrough = UI_FONTPROPERTIES_NOTSET;
    m_defaultFontInfo.foregroundColorStatus = UI_FONTPROPERTIES_SET;
    m_defaultFontInfo.foregroundColor = D2D1::ColorF(D2D1::ColorF::Black);
}

HRESULT touchmind::control::DWriteEditControlManager::CreateEditControl( IN D2D1_RECT_F layoutRect, IN const std::wstring& text, OUT EDIT_CONTROL_INDEX *pEditControlIndex )
{
    std::shared_ptr<control::DWriteEditControl> pEditControl = std::make_shared<DWriteEditControl>(this, layoutRect, m_defaultFontInfo);
    HRESULT hr = pEditControl->Initialize(text);
    if (SUCCEEDED(hr)) {
        *pEditControlIndex = s_masterIndex++;
        pEditControl->SetEditControlIndex(*pEditControlIndex);
        m_dwEditControlMap[*pEditControlIndex] = pEditControl;
    }
    return hr;
}

size_t touchmind::control::DWriteEditControlManager::GetEditControlCount()
{
    return m_dwEditControlMap.size();
}

std::shared_ptr<touchmind::control::DWriteEditControl> touchmind::control::DWriteEditControlManager::GetEditControl(IN EDIT_CONTROL_INDEX index)
{
    std::shared_ptr<control::DWriteEditControl> editControl;
    if (m_dwEditControlMap.count(index) != 0) {
        editControl = m_dwEditControlMap[index];
    }
    return editControl;
}

touchmind::control::EDIT_CONTROL_INDEX touchmind::control::DWriteEditControlManager::GetEditControlIndex(
    const std::shared_ptr<control::DWriteEditControl> &pEditControl)
{
    auto result = std::find_if(std::begin(m_dwEditControlMap), std::end(m_dwEditControlMap), [&](EditControlIndexToEditControlPtrPair e) {
        return e.second == pEditControl;
    });
    return result != m_dwEditControlMap.end() ? result->first : -1;
}

void touchmind::control::DWriteEditControlManager::DestroyDWLightEditControl( IN EDIT_CONTROL_INDEX editControlIndex )
{
    auto result = std::find_if(std::begin(m_dwEditControlMap), std::end(m_dwEditControlMap), [&](EditControlIndexToEditControlPtrPair e) {
        return e.first == editControlIndex;
    });
    if (result != m_dwEditControlMap.end()) {
        m_dwEditControlMap.erase(result);
    }
}

void touchmind::control::DWriteEditControlManager::InitializeDeviceResources( ID2D1RenderTarget *pRenderTarget )
{
    UNREFERENCED_PARAMETER(pRenderTarget);
}

void touchmind::control::DWriteEditControlManager::OnRender( ID2D1RenderTarget *pRenderTarget )
{
    m_pRenderTarget = pRenderTarget;
    for (auto it = std::begin(m_dwEditControlMap); it != std::end(m_dwEditControlMap); ++it) {
        it->second->OnRender(pRenderTarget);
    }
}

std::shared_ptr<touchmind::control::DWriteEditControl> touchmind::control::DWriteEditControlManager::HitText(FLOAT x, FLOAT y)
{
    for (auto it = std::begin(m_dwEditControlMap); it != std::end(m_dwEditControlMap); ++it) {
        if (it->second->HitTest(x, y)) {
            return it->second;
        }
    }
    return nullptr;
}

bool touchmind::control::DWriteEditControlManager::SetFocus(FLOAT x, FLOAT y)
{
    std::shared_ptr<control::DWriteEditControl> editControl = HitText(x, y);
    if (editControl != nullptr) {
        if (m_pFocusedEditControl != nullptr) {
            m_pFocusedEditControl->KillFocus();
        }
        m_pFocusedEditControl = editControl;
        editControl->SetFocus();
        return true;
    }
    return false;
}

bool touchmind::control::DWriteEditControlManager::SetFocus(IN EDIT_CONTROL_INDEX editControlIndex)
{
    std::shared_ptr<control::DWriteEditControl> editControl = GetEditControl(editControlIndex);
    if (editControl != nullptr) {
        if (m_pFocusedEditControl != nullptr) {
            m_pFocusedEditControl->KillFocus();
        }
        m_pFocusedEditControl = editControl;
        editControl->SetFocus();
        return true;
    }
    return false;
}

void touchmind::control::DWriteEditControlManager::SetSelectedNode(IN EDIT_CONTROL_INDEX editControlIndex)
{
    std::shared_ptr<control::DWriteEditControl> editControl = GetEditControl(editControlIndex);
    if (editControl != nullptr) {
        m_pSelectedEditControl = editControl;
    }
}

void touchmind::control::DWriteEditControlManager::CancelSelectedNode()
{
    m_pSelectedEditControl.reset();
}

HRESULT touchmind::control::DWriteEditControlManager::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, OUT bool* eaten)
{
    *eaten = false;

    if (m_pFocusedEditControl == nullptr) {
        *eaten = false;
        return S_OK;
    }
    if (!m_pFocusedEditControl->IsAdvised()) {
        *eaten = false;
        return S_OK;
    }
    switch (message) {
    case WM_LBUTTONDOWN:
        POINT point;
        point.x = GET_X_LPARAM(lParam);
        point.y = GET_Y_LPARAM(lParam);
        touchmind::util::CoordinateConversion::ConvertWindowToModelCoordinate(m_parentHWnd, m_pScrollBarHelper, point, &m_dragStartPoint);
        if (m_pFocusedEditControl->HitTest((FLOAT) m_dragStartPoint.x, (FLOAT) m_dragStartPoint.y)) {
            m_dragging = true;
            ::SetCapture(m_parentHWnd);
            *eaten = true;
        }
        break;
    case WM_MOUSEMOVE:
        if (m_dragging) {
            POINT point;
            point.x = GET_X_LPARAM(lParam);
            point.y = GET_Y_LPARAM(lParam);
            touchmind::util::CoordinateConversion::ConvertWindowToModelCoordinate(m_parentHWnd, m_pScrollBarHelper, point, &m_dragEndPoint);
            m_pFocusedEditControl->OnMouseEvents(m_dragStartPoint, m_dragEndPoint);
            *eaten = true;
        }
        break;
    case WM_LBUTTONUP:
        if (m_dragging) {
            m_dragging = false;
            ::ReleaseCapture();
            POINT point;
            point.x = GET_X_LPARAM(lParam);
            point.y = GET_Y_LPARAM(lParam);
            touchmind::util::CoordinateConversion::ConvertWindowToModelCoordinate(m_parentHWnd, m_pScrollBarHelper, point, &m_dragEndPoint);
            m_pFocusedEditControl->OnMouseEvents(m_dragStartPoint, m_dragEndPoint);
            *eaten = true;
        }
        break;
    case WM_CHAR:
        if (wParam == 8) {
            m_pFocusedEditControl->OnWMChar_Backspace(hWnd, wParam, lParam);
            *eaten = true;
        } else {
            m_pFocusedEditControl->OnWMChar(hWnd, wParam, lParam);
            *eaten = true;
        }
        break;
    case WM_KEYDOWN:
        switch (wParam) {
        case VK_UP:
        case VK_DOWN:
            m_pFocusedEditControl->OnKeyDown_CursorUpDown(static_cast<DWORD>(wParam));
            *eaten = true;
            break;
        case VK_RIGHT:
        case VK_LEFT:
            m_pFocusedEditControl->OnKeyDown_CursorLeftRight(static_cast<DWORD>(wParam));
            *eaten = true;
            break;
        case VK_DELETE:
            m_pFocusedEditControl->OnKeyDown_Delete(hWnd, wParam, lParam);
            *eaten = true;
            break;
        }
        break;
    case WM_SETFOCUS:
        m_pFocusedEditControl->SetFocus();
        break;
    case WM_KILLFOCUS:
        m_pFocusedEditControl->KillFocus();
        break;
    }
    return S_OK;
}

std::shared_ptr<touchmind::control::DWriteEditControl> touchmind::control::DWriteEditControlManager::GetFocusedEditControl()
{
    return m_pFocusedEditControl;
}

bool touchmind::control::DWriteEditControlManager::KillFocus()
{
    if (m_pFocusedEditControl != nullptr) {
        m_pFocusedEditControl->KillFocus();

        m_pFocusedEditControl = nullptr;
        return true;
    }
    return false;
}

/// <summary>set values by Ribbon Framework</summary>
/// this method is invoked by TouchMindRibbonFontControlCHandler
void touchmind::control::DWriteEditControlManager::SetValues(IN IPropertyStore *pPropertyStore)
{
    if (m_pFocusedEditControl != nullptr) {
        touchmind::text::FontAttributeCommand fontInfo;
        GetFontInfoFromIPropertyStore(pPropertyStore, &fontInfo);
        m_pFocusedEditControl->SetFontInfoToCurrentSelection(&fontInfo);
        _FireEditControlChanged(GetEditControlIndex(m_pSelectedEditControl));
    } else if (m_pSelectedEditControl != nullptr) {
        touchmind::text::FontAttributeCommand fontInfo;
        GetFontInfoFromIPropertyStore(pPropertyStore, &fontInfo);
        m_pSelectedEditControl->SetFontInfoToAllText(&fontInfo);
        _FireEditControlChanged(GetEditControlIndex(m_pSelectedEditControl));
    } else {
        // change default font info
        touchmind::text::FontAttributeCommand fontInfo;
        GetFontInfoFromIPropertyStore(pPropertyStore, &fontInfo);
        if (fontInfo.fontFamilyNameStatus != UI_FONTPROPERTIES_NOTAVAILABLE) {
            m_defaultFontInfo.fontFamilyNameStatus = fontInfo.fontFamilyNameStatus;
            m_defaultFontInfo.fontFamilyName = fontInfo.fontFamilyName;
        }
        if (fontInfo.localeNameStatus != UI_FONTPROPERTIES_NOTAVAILABLE) {
            m_defaultFontInfo.localeNameStatus = fontInfo.localeNameStatus;
            m_defaultFontInfo.localeName = fontInfo.localeName;
        }
        if (fontInfo.fontSizeStatus != UI_FONTPROPERTIES_NOTAVAILABLE) {
            m_defaultFontInfo.fontSizeStatus = fontInfo.fontSizeStatus;
            m_defaultFontInfo.fontSize = fontInfo.fontSize;
        }
        if (fontInfo.bold != UI_FONTPROPERTIES_NOTAVAILABLE) {
            m_defaultFontInfo.bold = fontInfo.bold;
        }
        if (fontInfo.italic != UI_FONTPROPERTIES_NOTAVAILABLE) {
            m_defaultFontInfo.italic = fontInfo.italic;
        }
        if (fontInfo.underline != UI_FONTPROPERTIES_NOTAVAILABLE) {
            m_defaultFontInfo.underline = fontInfo.underline;
        }
        if (fontInfo.strikethrough != UI_FONTPROPERTIES_NOTAVAILABLE) {
            m_defaultFontInfo.strikethrough = fontInfo.strikethrough;
        }
        if (fontInfo.foregroundColorStatus != UI_FONTPROPERTIES_NOTAVAILABLE) {
            m_defaultFontInfo.foregroundColorStatus = fontInfo.foregroundColorStatus;
            m_defaultFontInfo.foregroundColor = fontInfo.foregroundColor;
        }
    }
}

/// <summary>get values by Ribbon Framework</summary>
/// this method is invoked by TouchMindRibbonFontControlCHandler
void touchmind::control::DWriteEditControlManager::GetValues(IN IPropertyStore *pPropertyStore)
{
    if (m_pFocusedEditControl != nullptr) {
        touchmind::text::FontAttributeCommand fontInfo;
        m_pFocusedEditControl->GetFontInfoOfCurrentSelection(&fontInfo);
        GetIPropertyStoreFromFontInfo(&fontInfo, pPropertyStore);
    } else if (m_pSelectedEditControl != nullptr) {
        touchmind::text::FontAttributeCommand fontInfo;
        m_pSelectedEditControl->GetFontInfoOfAllText(&fontInfo);
        GetIPropertyStoreFromFontInfo(&fontInfo, pPropertyStore);
    } else {
        // default font info
        GetIPropertyStoreFromFontInfo(&m_defaultFontInfo, pPropertyStore);
    }
}

void touchmind::control::DWriteEditControlManager::SetPreviewValues(IN IPropertyStore *pPropertyStore)
{
    SetValues(pPropertyStore);
}

void touchmind::control::DWriteEditControlManager::CancelPreview(IN IPropertyStore *pPropertyStore)
{
    SetValues(pPropertyStore);
}

void touchmind::control::DWriteEditControlManager::ShowSelection()
{
}

void touchmind::control::DWriteEditControlManager::GetFontInfoFromIPropertyStore(
    IN IPropertyStore* pPropertyStore,
    OUT touchmind::text::FontAttributeCommand* pFontInfo)
{
    PROPVARIANT propvar;
    PropVariantInit(&propvar);
    UINT uValue;

    // bold
    if (SUCCEEDED(pPropertyStore->GetValue(UI_PKEY_FontProperties_Bold, &propvar))) {
        UIPropertyToUInt32(UI_PKEY_FontProperties_Bold, propvar, &uValue);
        if ((UI_FONTPROPERTIES) uValue != UI_FONTPROPERTIES_NOTAVAILABLE) {
            pFontInfo->bold = (UI_FONTPROPERTIES) uValue;
        }
    }
    PropVariantClear(&propvar);

    // italic
    if (SUCCEEDED(pPropertyStore->GetValue(UI_PKEY_FontProperties_Italic, &propvar))) {
        UIPropertyToUInt32(UI_PKEY_FontProperties_Italic, propvar, &uValue);
        if ((UI_FONTPROPERTIES) uValue != UI_FONTPROPERTIES_NOTAVAILABLE) {
            pFontInfo->italic = (UI_FONTPROPERTIES) uValue;
        }
    }
    PropVariantClear(&propvar);

    // underline
    if (SUCCEEDED(pPropertyStore->GetValue(UI_PKEY_FontProperties_Underline, &propvar))) {
        UIPropertyToUInt32(UI_PKEY_FontProperties_Underline, propvar, &uValue);
        if ((UI_FONTUNDERLINE) uValue != UI_FONTUNDERLINE_NOTAVAILABLE) {
            pFontInfo->underline = (UI_FONTPROPERTIES) uValue;
        }
    }
    PropVariantClear(&propvar);

    // strikethrough
    if (SUCCEEDED(pPropertyStore->GetValue(UI_PKEY_FontProperties_Strikethrough, &propvar))) {
        UIPropertyToUInt32(UI_PKEY_FontProperties_Strikethrough, propvar, &uValue);
        if ((UI_FONTPROPERTIES) uValue != UI_FONTPROPERTIES_NOTAVAILABLE) {
            pFontInfo->strikethrough = (UI_FONTPROPERTIES) uValue;
        }
    }
    PropVariantClear(&propvar);

    // font family
    if (SUCCEEDED(pPropertyStore->GetValue(UI_PKEY_FontProperties_Family, &propvar))) {
        PWSTR pszFamily;
        UIPropertyToStringAlloc(UI_PKEY_FontProperties_Family, propvar, &pszFamily);
        if (lstrcmp(pszFamily, L"")) {
            int len = lstrlen(pszFamily);
            std::vector<wchar_t> fontFamilyName(len + 1);
            StringCchCopyW(&fontFamilyName.front(), len + 1, pszFamily);
            pFontInfo->fontFamilyName = &fontFamilyName.front();
            pFontInfo->fontFamilyNameStatus = UI_FONTPROPERTIES_SET;
        }
        CoTaskMemFree(pszFamily);
    }
    PropVariantClear(&propvar);

    // font size
    if (SUCCEEDED(pPropertyStore->GetValue(UI_PKEY_FontProperties_Size, &propvar))) {
        DECIMAL decSize;
        UIPropertyToDecimal(UI_PKEY_FontProperties_Size, propvar, &decSize);
        DOUBLE dSize;
        VarR8FromDec(&decSize, &dSize);
        if (dSize > 0) {
            pFontInfo->fontSize = (FLOAT) dSize;
            pFontInfo->fontSizeStatus = UI_FONTPROPERTIES_SET;
        }
    }
    PropVariantClear(&propvar);

    // foreground color
    if (SUCCEEDED(pPropertyStore->GetValue(UI_PKEY_FontProperties_ForegroundColor, &propvar))) {
        UIPropertyToUInt32(UI_PKEY_FontProperties_ForegroundColor, propvar, &uValue);
        COLORREF colorRef = (COLORREF) uValue;
        pFontInfo->foregroundColor = D2D1::ColorF(GetRValue(colorRef) / 255.0f, GetGValue(colorRef) / 255.0f, GetBValue(colorRef) / 255.0f, 1.0f);
        pFontInfo->foregroundColorStatus = UI_FONTPROPERTIES_SET;
    }
    PropVariantClear(&propvar);
}

void touchmind::control::DWriteEditControlManager::GetIPropertyStoreFromFontInfo(
    IN touchmind::text::FontAttributeCommand* pFontInfo,
    OUT IPropertyStore* pPropertyStore)
{
    PROPVARIANT propvar;
    PropVariantInit(&propvar);

    // bold
    UIInitPropertyFromUInt32(UI_PKEY_FontProperties_Bold, pFontInfo->bold, &propvar);
    pPropertyStore->SetValue(UI_PKEY_FontProperties_Bold, propvar);
    PropVariantClear(&propvar);

    // italic
    UIInitPropertyFromUInt32(UI_PKEY_FontProperties_Italic, pFontInfo->italic, &propvar);
    pPropertyStore->SetValue(UI_PKEY_FontProperties_Italic, propvar);
    PropVariantClear(&propvar);

    // underline
    UIInitPropertyFromUInt32(UI_PKEY_FontProperties_Underline, pFontInfo->underline, &propvar);
    pPropertyStore->SetValue(UI_PKEY_FontProperties_Underline, propvar);
    PropVariantClear(&propvar);

    // strikethrough
    UIInitPropertyFromUInt32(UI_PKEY_FontProperties_Strikethrough, pFontInfo->strikethrough, &propvar);
    pPropertyStore->SetValue(UI_PKEY_FontProperties_Strikethrough, propvar);
    PropVariantClear(&propvar);

    // font family
    if (pFontInfo->fontFamilyNameStatus != UI_FONTPROPERTIES_NOTAVAILABLE) {
        UIInitPropertyFromString(UI_PKEY_FontProperties_Family, pFontInfo->fontFamilyName.c_str(), &propvar);
    } else {
        UIInitPropertyFromString(UI_PKEY_FontProperties_Family, L"", &propvar);
    }
    pPropertyStore->SetValue(UI_PKEY_FontProperties_Family, propvar);
    PropVariantClear(&propvar);

    // font size
    DECIMAL decSize;
    if (pFontInfo->fontSizeStatus != UI_FONTPROPERTIES_NOTAVAILABLE) {
        VarDecFromR8((DOUBLE) pFontInfo->fontSize, &decSize);
    } else {
        VarDecFromI4(0, &decSize);
    }
    UIInitPropertyFromDecimal(UI_PKEY_FontProperties_Size, decSize, &propvar);
    pPropertyStore->SetValue(UI_PKEY_FontProperties_Size, propvar);
    PropVariantClear(&propvar);

    if (pFontInfo->foregroundColorStatus != UI_FONTPROPERTIES_NOTAVAILABLE) {
        // foreground color
        UIInitPropertyFromUInt32(UI_PKEY_FontProperties_ForegroundColorType, UI_SWATCHCOLORTYPE_RGB, &propvar);
        pPropertyStore->SetValue(UI_PKEY_FontProperties_ForegroundColorType, propvar);
        PropVariantClear(&propvar);

        // Set the color value in property store.
        COLORREF colorRef = RGB(pFontInfo->foregroundColor.r * 255, pFontInfo->foregroundColor.g * 255, pFontInfo->foregroundColor.b * 255);
        UIInitPropertyFromUInt32(UI_PKEY_FontProperties_ForegroundColor, colorRef, &propvar);
        pPropertyStore->SetValue(UI_PKEY_FontProperties_ForegroundColor, propvar);
    }
}

void touchmind::control::DWriteEditControlManager::AddEditControlChangeListener(IDWLightEditControlChangeListener* pListener)
{
    m_editControlChangeListener.push_back(pListener);
}

void touchmind::control::DWriteEditControlManager::RemoveEditControlChangeListener(IDWLightEditControlChangeListener* pListener)
{
    m_editControlChangeListener.erase(
        std::remove(std::begin(m_editControlChangeListener), std::end(m_editControlChangeListener), pListener),
        std::end(m_editControlChangeListener));
}

void touchmind::control::DWriteEditControlManager::_FireEditControlChanged(EDIT_CONTROL_INDEX editControlIndex)
{
    for (auto it = std::begin(m_editControlChangeListener); it != std::end(m_editControlChangeListener); ++it) {
        (*it)->NotifyEditControlHasChanged(editControlIndex);
    }
}

/// <summary>notify that the selection has been changed in order to update Ribbon Framework</summary>
void touchmind::control::DWriteEditControlManager::NotifySelectionHasChanged(
    const std::shared_ptr<control::DWriteEditControl> &editControlPtr)
{
    touchmind::control::EDIT_CONTROL_INDEX editControlIndex = GetEditControlIndex(editControlPtr);
    if (editControlIndex >= 0) {
        for (auto it = std::begin(m_selectionChangeListener); it != std::end(m_selectionChangeListener); ++it) {
            (*it)->NotifySelectionHasChanged(editControlIndex);
        }
    }
}

void touchmind::control::DWriteEditControlManager::AddSelectionChangeListener(IDWLightEditControlSelectionChangeListener* pListener)
{
    m_selectionChangeListener.push_back(pListener);
}

void touchmind::control::DWriteEditControlManager::RemoveSelectionChangeListener(IDWLightEditControlSelectionChangeListener* pListener)
{
    m_selectionChangeListener.erase(
        std::remove(std::begin(m_selectionChangeListener), std::end(m_selectionChangeListener), pListener),
        m_selectionChangeListener.end());
}

void touchmind::control::DWriteEditControlManager::_FireSelectionChanged(EDIT_CONTROL_INDEX editControlIndex)
{
    for (auto it = std::begin(m_selectionChangeListener); it != std::end(m_selectionChangeListener); ++it) {
        (*it)->NotifySelectionHasChanged(editControlIndex);
    }
}

void touchmind::control::DWriteEditControlManager::OnDebugDump( std::wofstream& os )
{
    os << L"Control::DWriteEditControlManager::OnDebugDump start <<" << std::endl;

    for (auto it = std::begin(m_dwEditControlMap); it != std::end(m_dwEditControlMap); ++it) {
        os << L"[" << it->first << L"] ";
        it->second->OnDebugDump(os);
    }

    os << L"Control::DWriteEditControlManager::OnDebugDump >> end" << std::endl;
}

