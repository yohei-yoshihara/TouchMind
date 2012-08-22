#include "StdAfx.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/Common.h"
#include "touchmind/Configuration.h"
#include "touchmind/util/CoordinateConversion.h"
#include "touchmind/control/DWriteEditControlCommon.h"
#include "touchmind/control/DWriteEditControl.h"
#include "touchmind/control/DWriteEditControlManager.h"
#include "touchmind/util/Utils.h"

const LONG touchmind::control::DWriteEditControl::INFINITE_TEXT_END = LONG_MAX;

const FLOAT touchmind::control::DWriteEditControl::MIN_WIDTH = 128.0f;
const FLOAT touchmind::control::DWriteEditControl::MIN_WIDTH_ON_EDIT = 128.0f;

touchmind::control::DWriteEditControl::DWriteEditControl(
    DWriteEditControlManager *manager,
    D2D1_RECT_F layoutRect,
    touchmind::text::FontAttributeCommand& initialFontInfo
) :
    m_pManager(manager),
    m_pTextStoreACP(nullptr),
    m_pTextEditSink(nullptr),
    m_pDocMgr(nullptr),
    m_editCookie(0),
    m_caretVisible(false),
    m_layoutRect(layoutRect),
    m_actualRect(layoutRect),
    m_foregroundColor(D2D1::ColorF(D2D1::ColorF::Black)),
    m_backgroundColor(D2D1::ColorF(D2D1::ColorF::White)),
    m_borderLineColor(D2D1::ColorF(D2D1::ColorF::LightGray)),
    m_foregroundColorForEdit(D2D1::ColorF(D2D1::ColorF::Black)),
    m_backgroundColorForEdit(D2D1::ColorF(0.9f, 0.9f, 0.9f, 0.75f)),
    m_borderLineColorForEdit(D2D1::ColorF(D2D1::ColorF::Gray)),
    m_highlightColor(D2D1::ColorF(D2D1::ColorF::LightSkyBlue)),
    m_compositionUnderlineColor(D2D1::ColorF(D2D1::ColorF::Black)),
    m_pForegroundBrush(nullptr),
    m_pBackgroundBrush(nullptr),
    m_pBorderLineBrush(nullptr),
    m_pForegroundBrushForEdit(nullptr),
    m_pBackgroundBrushForEdit(nullptr),
    m_pBorderLineBrushForEdit(nullptr),
    m_pHighlightBrush(nullptr),
    m_pCompositionUnderlineBrush(nullptr),
    m_pCompositionSolidUnderlineStrokeStyle(nullptr),
    m_pCompositionDotUnderlineStrokeStyle(nullptr),
    m_pCompositionDashUnderlineStrokeStyle(nullptr),
    m_pCompositionSquiggleUnderlineStrokeStyle(nullptr),
    m_pDWriteTextFormat(nullptr),
    m_pDWriteTextLayout(nullptr),
    m_caretFontAttributeCommand(initialFontInfo)
{
}

touchmind::control::DWriteEditControl::~DWriteEditControl(void)
{
#ifdef DEBUG_GPU_RESOURCE
    LOG_ENTER;
#endif
    m_pTextEditSink->Unadvise(m_pTfContext);
    /*
    // We don't need to release TextEditSink and TextStoreACP manually.
    // After destroying DocumentMgr and Context, the reference counters of them become zero,
    // then they are automatically removed.

    if (m_pTextEditSink != nullptr) {
        m_pTextEditSink->Release();
    }
    if (m_pTextStoreACP != nullptr) {
        m_pTextStoreACP->Release();
    }
    */
    SafeRelease(&m_pDWriteTextFormat);
    SafeRelease(&m_pDWriteTextLayout);

    m_pDocMgr->Pop(m_editCookie);

    SafeRelease(&m_pTfContext);
    SafeRelease(&m_pDocMgr);
#ifdef DEBUG_GPU_RESOURCE
    LOG_LEAVE;
#endif
}

HRESULT touchmind::control::DWriteEditControl::Initialize( const std::wstring &text )
{
    HRESULT hr = S_OK;

    m_hWnd = m_pManager->GetParentHWnd();

    std::shared_ptr<control::DWriteEditControl> thisPtr = shared_from_this();
    m_pTextEditSink = new DWriteEditControlTextEditSink(thisPtr);
    m_pTextStoreACP = new DWriteEditControlTextStoreACP(thisPtr);

    hr = m_pManager->GetThreadMgr()->CreateDocumentMgr(&m_pDocMgr);
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_FATAL) << L"Failed to create document manager, hr = " << hr;
        return hr;
    }

    //create the context
    hr = m_pDocMgr->CreateContext(m_pManager->GetClientId(),
                                  0,
                                  static_cast<ITextStoreACP*>(m_pTextStoreACP),
                                  &m_pTfContext,
                                  &m_editCookie);
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_FATAL) << L"Failed to create context, hr = " << hr;
        return hr;
    }

    //push the context onto the document stack
    hr = m_pDocMgr->Push(m_pTfContext);
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_FATAL) << L"Failed to push context, hr = " << hr;
        return hr;
    }

    // initialize TextStoreACP
    m_pTextStoreACP->SetText(text, true);
    m_pTextStoreACP->Initialize();

    // initialize TextEditSink
    m_pTextEditSink->SetCategoryMgr(m_pManager->GetCategoryMgr());
    m_pTextEditSink->SetDisplayAttributeMgr(m_pManager->GetDisplayAttributeMgr());
    m_pTextEditSink->Advise(m_pTfContext);

    CreateDeviceIndependentResources();

    hr = InitializeTextFormat();
    if (FAILED(hr)) {
        return hr;
    }
    _RecreateLayout();
    SetFontInfoToAllText(&m_caretFontAttributeCommand);
    return hr;
}

HRESULT touchmind::control::DWriteEditControl::InitializeTextFormat()
{
    SafeRelease(&m_pDWriteTextFormat);
    HRESULT hr = m_pManager->GetDWriteFactory()->CreateTextFormat(
                     m_caretFontAttributeCommand.fontFamilyName.c_str(),
                     nullptr,
                     m_caretFontAttributeCommand.GetFontWeight(),
                     m_caretFontAttributeCommand.GetFontStyle(),
                     m_caretFontAttributeCommand.GetFontStretch(),
                     m_caretFontAttributeCommand.fontSize,
                     m_caretFontAttributeCommand.localeName.c_str(),
                     &m_pDWriteTextFormat
                 );
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] IDWriteTextFormat = [" << std::hex << m_pDWriteTextFormat << L"]" << std::dec;
#endif
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_FATAL) << L"failed to create text format, hr = " << hr;
        m_pDWriteTextFormat = nullptr;
        return hr;
    }

    return hr;
}

/// <summary>Recreate DWriteTextLayout</summary>
/// If recreate DWriteTextLayout, we must copy all the font attributes to new DWriteTextLayout
void touchmind::control::DWriteEditControl::_RecreateLayout()
{
    HRESULT hr;

    std::wstring text;
    m_pTextStoreACP->GetText(text);

    FLOAT width = std::abs(m_layoutRect.left - m_layoutRect.right);
    FLOAT height = std::abs(m_layoutRect.bottom - m_layoutRect.top);
    if (m_pDWriteTextLayout == nullptr) {
        hr = m_pManager->GetDWriteFactory()->CreateTextLayout(
                 text.c_str(),
                 static_cast<UINT32>(text.length()),
                 m_pDWriteTextFormat,
                 width,
                 height,
                 &m_pDWriteTextLayout
             );
#ifdef DEBUG_GPU_RESOURCE
        LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] IDWriteTextLayout = [" << std::hex << m_pDWriteTextLayout << L"]" << std::dec;
#endif
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_FATAL) << L"failed to create text layout, hr = " << hr;
        }
    } else {
        IDWriteTextLayout *oldTextLayout = m_pDWriteTextLayout;
        hr = m_pManager->GetDWriteFactory()->CreateTextLayout(
                 text.c_str(),
                 static_cast<UINT32>(text.length()),
                 oldTextLayout,
                 width,
                 height,
                 &m_pDWriteTextLayout
             );
#ifdef DEBUG_GPU_RESOURCE
        LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] IDWriteTextLayout = [" << std::hex << m_pDWriteTextLayout << L"]" << std::dec;
#endif
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_FATAL) << L"failed to create text layout, hr = " << hr;
        }
        SafeRelease(&oldTextLayout);
    }
    UpdateActualRect();
}

/// <summary>get font attributes at acpStart and set them to a FontAttributeCommand
void touchmind::control::DWriteEditControl::_GetFontAttributes(IN IDWriteTextLayout* pTextLayout, IN LONG startPosition, OUT touchmind::text::FontAttributeCommand* pFontInfo)
{
    DWRITE_TEXT_RANGE textRange;
    textRange.startPosition = startPosition;
    textRange.length = 1;
    _GetFontAttributes(pTextLayout, textRange, pFontInfo);
}

/// <summary>get font attributes at textRange and set them to a FontAttributeCommand
/// if the length of the textRange is zero, try to get font attributes at the startPosition of the textRange. This case, any attributes of the FontAttributeCommand can not be UI_FONTPROPERTIES_NOTAVAILABLE
/// if the length of the textRange is more than zero, try to get font attributes at the text range. This case, some attributes of FontAttributeCommand may be UI_FONTPROPERTIES_NOTAVAILABLE
void touchmind::control::DWriteEditControl::_GetFontAttributes(IN IDWriteTextLayout* pTextLayout, IN DWRITE_TEXT_RANGE& textRange, OUT touchmind::text::FontAttributeCommand* pFontInfo, OUT DWRITE_TEXT_RANGE* pActualTextRange)
{
    HRESULT hr;

    pFontInfo->fontFamilyNameStatus = UI_FONTPROPERTIES_NOTAVAILABLE;
    pFontInfo->localeNameStatus = UI_FONTPROPERTIES_NOTAVAILABLE;
    pFontInfo->fontSizeStatus = UI_FONTPROPERTIES_NOTAVAILABLE;
    pFontInfo->bold = UI_FONTPROPERTIES_NOTAVAILABLE;
    pFontInfo->italic = UI_FONTPROPERTIES_NOTAVAILABLE;
    pFontInfo->underline = UI_FONTPROPERTIES_NOTAVAILABLE;
    pFontInfo->strikethrough = UI_FONTPROPERTIES_NOTAVAILABLE;
    pFontInfo->foregroundColorStatus = UI_FONTPROPERTIES_NOTAVAILABLE;

    LONG endPos = textRange.startPosition + textRange.length - 1;
    DWRITE_TEXT_RANGE actualTextRange;

    // font family
    UINT32 fontFamilyNameLength;
    hr = pTextLayout->GetFontFamilyNameLength(textRange.startPosition, &fontFamilyNameLength, &actualTextRange);
    if (SUCCEEDED(hr)) {
        std::vector<wchar_t> fontFamilyName(fontFamilyNameLength + 1);
        hr = pTextLayout->GetFontFamilyName(textRange.startPosition, &fontFamilyName.front(), fontFamilyNameLength + 1, &actualTextRange);
        LONG actualEndPos = actualTextRange.startPosition + actualTextRange.length - 1;
        if (SUCCEEDED(hr)) {
            pFontInfo->fontFamilyName = &fontFamilyName.front();
            pFontInfo->fontFamilyNameStatus = endPos <= actualEndPos ? UI_FONTPROPERTIES_SET : UI_FONTPROPERTIES_NOTAVAILABLE;
        }
    }

    // locale name
    UINT32 localeNameLength;
    hr = pTextLayout->GetLocaleNameLength(textRange.startPosition, &localeNameLength, &actualTextRange);
    if (SUCCEEDED(hr)) {
        std::vector<wchar_t> localeName(localeNameLength + 1);
        hr = pTextLayout->GetLocaleName(textRange.startPosition, &localeName.front(), localeNameLength + 1, &actualTextRange);
        LONG actualEndPos = actualTextRange.startPosition + actualTextRange.length - 1;
        if (SUCCEEDED(hr)) {
            pFontInfo->localeName = &localeName.front();
            pFontInfo->localeNameStatus = endPos <= actualEndPos ? UI_FONTPROPERTIES_SET : UI_FONTPROPERTIES_NOTAVAILABLE;
        }
    }

    // font size
    hr = pTextLayout->GetFontSize(textRange.startPosition, &pFontInfo->fontSize, &actualTextRange);
    if (SUCCEEDED(hr)) {
        LONG actualEndPos = actualTextRange.startPosition + actualTextRange.length - 1;
        pFontInfo->fontSizeStatus = endPos <= actualEndPos ? UI_FONTPROPERTIES_SET : UI_FONTPROPERTIES_NOTAVAILABLE;
    }

    // bold
    DWRITE_FONT_WEIGHT fontWeight;
    hr = pTextLayout->GetFontWeight(textRange.startPosition, &fontWeight, &actualTextRange);
    if (SUCCEEDED(hr)) {
        LONG actualEndPos = actualTextRange.startPosition + actualTextRange.length - 1;
        if (endPos > actualEndPos) {
            pFontInfo->bold = UI_FONTPROPERTIES_NOTAVAILABLE;
        } else {
            pFontInfo->bold = fontWeight != DWRITE_FONT_WEIGHT_NORMAL ?
                              UI_FONTPROPERTIES_SET : UI_FONTPROPERTIES_NOTSET;
        }
    }

    // italic
    DWRITE_FONT_STYLE fontStyle;
    hr = pTextLayout->GetFontStyle(textRange.startPosition, &fontStyle, &actualTextRange);
    if (SUCCEEDED(hr)) {
        LONG actualEndPos = actualTextRange.startPosition + actualTextRange.length - 1;
        if (endPos > actualEndPos) {
            pFontInfo->italic = UI_FONTPROPERTIES_NOTAVAILABLE;
        } else {
            pFontInfo->italic = fontStyle != DWRITE_FONT_STYLE_NORMAL ?
                                UI_FONTPROPERTIES_SET : UI_FONTPROPERTIES_NOTSET;
        }
    }

    // underline
    BOOL hasUnderLine;
    hr = pTextLayout->GetUnderline(textRange.startPosition, &hasUnderLine, &actualTextRange);
    if (SUCCEEDED(hr)) {
        LONG actualEndPos = actualTextRange.startPosition + actualTextRange.length - 1;
        if (endPos > actualEndPos) {
            pFontInfo->underline = UI_FONTPROPERTIES_NOTAVAILABLE;
        } else {
            pFontInfo->underline = hasUnderLine ? UI_FONTPROPERTIES_SET : UI_FONTPROPERTIES_NOTSET;
        }
    }

    // strikethrough
    BOOL hasStrikethrough;
    hr = pTextLayout->GetStrikethrough(textRange.startPosition, &hasStrikethrough, &actualTextRange);
    if (SUCCEEDED(hr)) {
        LONG actualEndPos = actualTextRange.startPosition + actualTextRange.length - 1;
        if (endPos > actualEndPos) {
            pFontInfo->strikethrough = UI_FONTPROPERTIES_NOTAVAILABLE;
        } else {
            pFontInfo->strikethrough = hasStrikethrough ? UI_FONTPROPERTIES_SET : UI_FONTPROPERTIES_NOTSET;
        }
    }

    // foreground color
    IUnknown* drawingEffect = nullptr;
    hr = pTextLayout->GetDrawingEffect(textRange.startPosition, &drawingEffect);
    if (SUCCEEDED(hr) && drawingEffect != nullptr) {
        ID2D1SolidColorBrush& effect = *reinterpret_cast<ID2D1SolidColorBrush*>(drawingEffect);
        pFontInfo->foregroundColor = effect.GetColor();
        pFontInfo->foregroundColorStatus = UI_FONTPROPERTIES_SET;
    }
    SafeRelease(&drawingEffect);

    if (pActualTextRange != nullptr) {
        pActualTextRange->startPosition = actualTextRange.startPosition;
        pActualTextRange->length = actualTextRange.length;
    }
}

///<summary>set the font attributes to the text layout</summary>
void touchmind::control::DWriteEditControl::_SetFontAttributes( IDWriteTextLayout* pTextLayout, IN touchmind::text::FontAttributeCommand* pFontAttributeCommand, IN DWRITE_TEXT_RANGE& textRange )
{
    HRESULT hr;

    // font family
    if (pFontAttributeCommand->fontFamilyNameStatus != UI_FONTPROPERTIES_NOTAVAILABLE) {
        hr = pTextLayout->SetFontFamilyName(pFontAttributeCommand->fontFamilyName.c_str(), textRange);
    }
    // locale name
    if (pFontAttributeCommand->localeNameStatus != UI_FONTPROPERTIES_NOTAVAILABLE) {
        hr = pTextLayout->SetLocaleName(pFontAttributeCommand->localeName.c_str(), textRange);
    }
    // font size
    if (pFontAttributeCommand->fontSizeStatus != UI_FONTPROPERTIES_NOTAVAILABLE) {
        hr = pTextLayout->SetFontSize(pFontAttributeCommand->fontSize, textRange);
    }
    // bold
    if (pFontAttributeCommand->bold != UI_FONTPROPERTIES_NOTAVAILABLE) {
        hr = pTextLayout->SetFontWeight(pFontAttributeCommand->GetFontWeight(), textRange);
    }
    // italic
    if (pFontAttributeCommand->italic != UI_FONTPROPERTIES_NOTAVAILABLE) {
        hr = pTextLayout->SetFontStyle(pFontAttributeCommand->GetFontStyle(), textRange);
    }
    // underline
    if (pFontAttributeCommand->underline != UI_FONTPROPERTIES_NOTAVAILABLE) {
        hr = pTextLayout->SetUnderline(pFontAttributeCommand->GetUnderline(), textRange);
    }
    // strikethrough
    if (pFontAttributeCommand->strikethrough != UI_FONTPROPERTIES_NOTAVAILABLE) {
        hr = pTextLayout->SetStrikethrough(pFontAttributeCommand->GetStrikethrough(), textRange);
    }
    // foreground color
    if (pFontAttributeCommand->foregroundColorStatus != UI_FONTPROPERTIES_NOTAVAILABLE) {
        ID2D1SolidColorBrush *pBrush = nullptr;
        ID2D1RenderTarget *pRenderTarget = m_pManager->GetD2DRenderTarget();
        if (pRenderTarget != nullptr) {
            hr = pRenderTarget->CreateSolidColorBrush(pFontAttributeCommand->foregroundColor, &pBrush);
#ifdef DEBUG_GPU_RESOURCE
            LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1SolidColorBrush = [" << std::hex << pBrush << L"]" << std::dec;
#endif
            if (SUCCEEDED(hr)) {
                hr = pTextLayout->SetDrawingEffect(pBrush, textRange);
            }
        } else {
            LOG(SEVERITY_LEVEL_ERROR) << L"null render target";
        }
        SafeRelease(&pBrush);
    }
}

void touchmind::control::DWriteEditControl::RenderTextWithCompositionUnderline( ID2D1RenderTarget *pRenderTarget, const D2D1_RECT_F& renderRect )
{
    HRESULT hr = S_OK;
    FLOAT x = m_layoutRect.left;
    FLOAT y = m_layoutRect.top;

    // make a copy of the current text layout
    std::wstring text;
    m_pTextStoreACP->GetText(text);
    FLOAT width = std::abs(m_layoutRect.right - m_layoutRect.left);
    FLOAT height = std::abs(m_layoutRect.bottom - m_layoutRect.top);
    IDWriteTextLayout *pTextLayout = nullptr;
    hr = m_pManager->GetDWriteFactory()->CreateTextLayout(
             text.c_str(),
             static_cast<UINT32>(text.length()),
             m_pDWriteTextLayout,
             width,
             height,
             &pTextLayout
         );
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] IDWriteTextLayout = [" << std::hex << pTextLayout << L"]" << std::dec;
#endif
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << L"CreateTextLayout failed, hr = " << hr;
        return;
    }
    CopyGlobalProperties(m_pDWriteTextLayout, pTextLayout);
    CopyRangedProperties(m_pDWriteTextLayout, 0, static_cast<UINT32>(text.length()), 0, pTextLayout);

    for (auto it = std::begin(m_compositionAttributes); it != std::end(m_compositionAttributes); ++it) {
        CompositionAttribute &ca = *it;

        if (ca.startPos >= 0 && ca.endPos >= 0 && ca.startPos != ca.endPos) {
            UINT32 actualHitTextCount;
            hr = m_pDWriteTextLayout->HitTestTextRange(
                     ca.startPos,
                     ca.endPos - ca.startPos,
                     x,
                     y,
                     nullptr,
                     0,
                     &actualHitTextCount
                 );
            if (actualHitTextCount > 0) {
                std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(actualHitTextCount);

                hr = m_pDWriteTextLayout->HitTestTextRange(
                         ca.startPos,
                         ca.endPos - ca.startPos,
                         x,
                         y,
                         &hitTestMetrics.front(),
                         actualHitTextCount,
                         &actualHitTextCount
                     );
                if (FAILED(hr)) {
                    LOG(SEVERITY_LEVEL_ERROR) << L"HitTestTextRange failed, hr = " << hr;
                    continue;
                }
                for (std::size_t i = 0; i < actualHitTextCount; ++i) {
                    D2D1_POINT_2F startPoint = D2D1::Point2F(
                                                   hitTestMetrics[i].left,
                                                   hitTestMetrics[i].top + hitTestMetrics[i].height * 0.9f
                                               );
                    D2D1_POINT_2F endPoint = D2D1::Point2F(
                                                 hitTestMetrics[i].left + hitTestMetrics[i].width,
                                                 hitTestMetrics[i].top + hitTestMetrics[i].height * 0.9f
                                             );
                    // line color
                    ID2D1SolidColorBrush *pLineColorBrush = nullptr;
                    if (ca.displayAttribute.crLine.type != TF_CT_NONE) {
                        COLORREF colorref;
                        if (ca.displayAttribute.crLine.type == TF_CT_SYSCOLOR) {
                            colorref = GetSysColor(ca.displayAttribute.crLine.nIndex);
                        } else {
                            colorref = ca.displayAttribute.crLine.cr;
                        }
                        D2D1_COLOR_F colorf;
                        touchmind::ColorrefToColorF(colorref, &colorf);
                        hr = pRenderTarget->CreateSolidColorBrush(colorf, &pLineColorBrush);
#ifdef DEBUG_GPU_RESOURCE
                        LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1SolidColorBrush = [" << std::hex << pLineColorBrush << L"]" << std::dec;
#endif
                        if (FAILED(hr)) {
                            LOG(SEVERITY_LEVEL_ERROR) << L"CreateSolidColorBrush failed, hr = " << hr;
                            pLineColorBrush = nullptr;
                        }
                    }

                    // text color
                    if (ca.displayAttribute.crText.type != TF_CT_NONE) {
                        COLORREF colorref;
                        if (ca.displayAttribute.crText.type == TF_CT_SYSCOLOR) {
                            colorref = GetSysColor(ca.displayAttribute.crText.nIndex);
                        } else {
                            colorref = ca.displayAttribute.crText.cr;
                        }
                        D2D1_COLOR_F colorf;
                        touchmind::ColorrefToColorF(colorref, &colorf);
                        ID2D1SolidColorBrush* pTextColorBrush = nullptr;
                        hr = pRenderTarget->CreateSolidColorBrush(colorf, &pTextColorBrush);
#ifdef DEBUG_GPU_RESOURCE
                        LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1SolidColorBrush = [" << std::hex << pTextColorBrush << L"]" << std::dec;
#endif
                        if (SUCCEEDED(hr)) {
                            DWRITE_TEXT_RANGE textRange;
                            textRange.startPosition = ca.startPos;
                            textRange.length = ca.endPos - ca.startPos;
                            pTextLayout->SetDrawingEffect(pTextColorBrush, textRange);
                        } else {
                            LOG(SEVERITY_LEVEL_ERROR) << L"CreateSolidColorBrush failed, hr = " << hr;
                        }
                        SafeRelease(&pTextColorBrush);
                    }

                    // background color
                    ID2D1SolidColorBrush* pBackgroundColorBrush = nullptr;
                    if (ca.displayAttribute.crBk.type != TF_CT_NONE) {
                        COLORREF colorref;
                        if (ca.displayAttribute.crText.type == TF_CT_SYSCOLOR) {
                            colorref = GetSysColor(ca.displayAttribute.crBk.nIndex);
                        } else {
                            colorref = ca.displayAttribute.crBk.cr;
                        }
                        D2D1_COLOR_F colorf;
                        touchmind::ColorrefToColorF(colorref, &colorf);
                        hr = pRenderTarget->CreateSolidColorBrush(colorf, &pBackgroundColorBrush);
#ifdef DEBUG_GPU_RESOURCE
                        LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1SolidColorBrush = [" << std::hex << pBackgroundColorBrush << L"]" << std::dec;
#endif
                        if (FAILED(hr)) {
                            LOG(SEVERITY_LEVEL_ERROR) << L"CreateSolidColorBrush failed, hr = " << hr;
                            pBackgroundColorBrush = nullptr;
                        }
                    }

                    // stroke style
                    CComPtr<ID2D1StrokeStyle> strokeStyle = nullptr;
                    switch (ca.displayAttribute.lsStyle) {
                    case TF_LS_NONE:
                        strokeStyle = nullptr;
                        break;
                    case TF_LS_SOLID:
                        strokeStyle = m_pCompositionSolidUnderlineStrokeStyle;
                        break;
                    case TF_LS_DOT:
                        strokeStyle = m_pCompositionDotUnderlineStrokeStyle;
                        break;
                    case TF_LS_DASH:
                        strokeStyle = m_pCompositionDashUnderlineStrokeStyle;
                        break;
                    case TF_LS_SQUIGGLE:
                        strokeStyle = m_pCompositionSquiggleUnderlineStrokeStyle;
                        break;
                    default:
                        strokeStyle = m_pCompositionSolidUnderlineStrokeStyle;
                    }

                    // stroke width
                    FLOAT strokeWidth = ca.displayAttribute.fBoldLine ? 2.0f : 1.0f;

                    if (pBackgroundColorBrush != nullptr) {
                        pRenderTarget->FillRectangle(
                            D2D1::RectF(
                                hitTestMetrics[i].left,
                                hitTestMetrics[i].top,
                                hitTestMetrics[i].left + hitTestMetrics[i].width,
                                hitTestMetrics[i].top + hitTestMetrics[i].height),
                            pBackgroundColorBrush);
                    }
                    if (strokeStyle != nullptr) {
                        if (pLineColorBrush != nullptr) {
                            pRenderTarget->DrawLine(startPoint, endPoint, pLineColorBrush, strokeWidth, strokeStyle);
                        } else {
                            pRenderTarget->DrawLine(startPoint, endPoint, m_pCompositionUnderlineBrush, strokeWidth, strokeStyle);
                        }
                    }

                    SafeRelease(&pLineColorBrush);
                    SafeRelease(&pBackgroundColorBrush);
                }
            }
        }
    }

    D2D1_POINT_2F point = D2D1::Point2F(renderRect.left, renderRect.top);
    if (HasFocus()) {
        pRenderTarget->DrawTextLayout(point, pTextLayout, m_pForegroundBrushForEdit);
    } else {
        pRenderTarget->DrawTextLayout(point, pTextLayout, m_pForegroundBrush);
    }

    SafeRelease(&pTextLayout);
}

void touchmind::control::DWriteEditControl::RenderHighlight( ID2D1RenderTarget *pRenderTarget )
{
    LONG acpStart = m_pTextStoreACP->GetACPStart();
    LONG acpEnd = m_pTextStoreACP->GetACPEnd();
    if (m_caretVisible && acpStart != acpEnd && m_pHighlightBrush != nullptr) {
        FLOAT x = m_layoutRect.left;
        FLOAT y = m_layoutRect.top;

        UINT32 actualHitTextCount;
        m_pDWriteTextLayout->HitTestTextRange(
            acpStart,
            acpEnd - acpStart,
            x,
            y,
            nullptr,
            0,
            &actualHitTextCount
        );
        if (actualHitTextCount > 0) {
            std::vector<DWRITE_HIT_TEST_METRICS> hitTestMetrics(actualHitTextCount);
            m_pDWriteTextLayout->HitTestTextRange(
                acpStart,
                acpEnd - acpStart,
                x,
                y,
                &hitTestMetrics.front(),
                actualHitTextCount,
                &actualHitTextCount
            );

            for (size_t i = 0; i < actualHitTextCount; ++i) {
                D2D1_RECT_F highlightRect = D2D1::RectF(
                                                hitTestMetrics[i].left,
                                                hitTestMetrics[i].top,
                                                hitTestMetrics[i].left + hitTestMetrics[i].width,
                                                hitTestMetrics[i].top + hitTestMetrics[i].height
                                            );
                pRenderTarget->FillRectangle(highlightRect, m_pHighlightBrush);
            }
        }
    }
}

void touchmind::control::DWriteEditControl::UpdateActualRect()
{
    DWRITE_TEXT_METRICS textMetrics;
    m_pDWriteTextLayout->GetMetrics(&textMetrics);
    m_actualRect.left = m_layoutRect.left;
    m_actualRect.top = m_layoutRect.top;
    m_actualRect.right = m_layoutRect.left + textMetrics.width;
    m_actualRect.bottom = m_layoutRect.top + textMetrics.height;
}

void touchmind::control::DWriteEditControl::OnRender( ID2D1RenderTarget *pRenderTarget )
{
    CreateDeviceResources(pRenderTarget);

    FLOAT width, height;
    if (HasFocus()) {
        width = std::max(m_layoutRect.right - m_layoutRect.left,
                         m_actualRect.right - m_actualRect.left);
        height = std::max(m_layoutRect.bottom - m_layoutRect.top,
                          m_actualRect.bottom - m_actualRect.top);
    } else {
        width = m_actualRect.right - m_actualRect.left;
        height = m_actualRect.bottom - m_actualRect.top;
    }
    D2D1_RECT_F renderRect = D2D1::RectF(m_layoutRect.left, m_layoutRect.top, m_layoutRect.left + width, m_layoutRect.top + height);

    if (HasFocus()) {
        if (m_backgroundColorForEdit.a > 0.0f) {
            pRenderTarget->FillRectangle(renderRect, m_pBackgroundBrushForEdit);
        }
        if (m_borderLineColorForEdit.a > 0.0f) {
            pRenderTarget->DrawRectangle(renderRect, m_pBorderLineBrushForEdit);
        }
    } else {
        if (m_backgroundColor.a > 0.0f) {
            pRenderTarget->FillRectangle(renderRect, m_pBackgroundBrush);
        }
        if (m_borderLineColor.a > 0.0f) {
            pRenderTarget->DrawRectangle(renderRect, m_pBorderLineBrush);
        }
    }

    RenderHighlight(pRenderTarget);
    RenderTextWithCompositionUnderline(pRenderTarget, renderRect);

    UpdateCaret();
}

/// <summary>handle events in the queue which were stored because of TSF locked
/// this method is invoked by DWLightEditControlTextStoreACP::RequestLock
LRESULT CALLBACK touchmind::control::DWriteEditControl::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool eaten;
    m_pManager->WndProc(hwnd, message, wParam, lParam, &eaten);
    return S_OK;
}

void touchmind::control::DWriteEditControl::SetFocus()
{
    HRESULT hr = m_pManager->GetThreadMgr()->SetFocus(m_pDocMgr);
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << L"set focus on thread mgr failed";
        return;
    }
    CreateCaret();
    UpdateFontInfoAtCaret();
}

void touchmind::control::DWriteEditControl::KillFocus()
{
    ClearCompositionRenderInfo();
    DestroyCaret();
}

void touchmind::control::DWriteEditControl::_CalculateCaretSizeAndPosition(D2D1_POINT_2F *caretPosInModel, DWRITE_HIT_TEST_METRICS *pHitTestMetrics)
{
    TsActiveSelEnd activeSelEnd = m_pTextStoreACP->GetActiveSelEnd();
    LONG acpStart = m_pTextStoreACP->GetACPStart();
    LONG acpEnd = m_pTextStoreACP->GetACPEnd();
    LONG caretPosition;
    if (activeSelEnd == TS_AE_END) {
        caretPosition = acpEnd - 1;
    } else {
        caretPosition = acpStart;
    }
    m_pDWriteTextLayout->HitTestTextPosition(caretPosition, activeSelEnd == TS_AE_END, &(caretPosInModel->x), &(caretPosInModel->y), pHitTestMetrics);
    caretPosInModel->x += m_layoutRect.left;
    caretPosInModel->y += m_layoutRect.top;
}

void touchmind::control::DWriteEditControl::CreateCaret()
{
    D2D1_POINT_2F caretPosInModel;
    DWRITE_HIT_TEST_METRICS hitTestMetrics;
    _CalculateCaretSizeAndPosition(&caretPosInModel, &hitTestMetrics);

    if (::CreateCaret(m_hWnd, (HBITMAP) nullptr, 0, (int) hitTestMetrics.height) == 0) {
        LOG(SEVERITY_LEVEL_ERROR) << L"CreateCaret failed";
        return;
    }

    POINT caretPosInWindow;
    touchmind::util::CoordinateConversion::ConvertModelToWindowCoordinate(m_hWnd, m_pManager->GetScrollBarHelper(), caretPosInModel, &caretPosInWindow);

    ::SetCaretPos(caretPosInWindow.x, caretPosInWindow.y);
    ::ShowCaret(m_hWnd);

    m_caretVisible = true;
    ::ShowCaret(m_hWnd);
}

void touchmind::control::DWriteEditControl::UpdateCaret()
{
    if (m_caretVisible) {
        D2D1_POINT_2F caretPosInModel;
        DWRITE_HIT_TEST_METRICS hitTestMetrics;
        _CalculateCaretSizeAndPosition(&caretPosInModel, &hitTestMetrics);

        if (::CreateCaret(m_hWnd, (HBITMAP) nullptr, 0, (int) hitTestMetrics.height) == 0) {
            LOG(SEVERITY_LEVEL_ERROR) << L"CreateCaret failed";
            return;
        }

        POINT caretPosInWindow;
        touchmind::util::CoordinateConversion::ConvertModelToWindowCoordinate(m_hWnd, m_pManager->GetScrollBarHelper(), caretPosInModel, &caretPosInWindow);

        ::SetCaretPos(caretPosInWindow.x, caretPosInWindow.y);
        ::ShowCaret(m_hWnd);
    }
}

void touchmind::control::DWriteEditControl::DestroyCaret()
{
    ::DestroyCaret();
    m_caretVisible = false;
}

bool touchmind::control::DWriteEditControl::HitTest(FLOAT x, FLOAT y)
{
    return m_layoutRect.left <= x && x <= m_layoutRect.right &&
           m_layoutRect.top <= y && y <= m_layoutRect.bottom;
}

HRESULT touchmind::control::DWriteEditControl::OnMouseEvents(D2D1_POINT_2F dragStartPoint, D2D1_POINT_2F dragEndPoint)
{
    if (m_pTextStoreACP->_IsLocked(TS_LF_READ)) {
        return S_OK;
    }
    m_pTextStoreACP->_LockDocument(TS_LF_READWRITE);

    LONG acpStart, acpEnd;
    TsActiveSelEnd activeSelEnd;
    bool ret = CalculateSelectionFromDragRect(dragStartPoint, dragEndPoint, &acpStart, &acpEnd, &activeSelEnd);
    if (ret) {
        _SetSelection(acpStart, acpEnd, activeSelEnd);
    }
    m_pTextStoreACP->_UnlockDocument();
    m_pTextStoreACP->GetTextStoreACPSink()->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);
    return S_OK;
}

HRESULT touchmind::control::DWriteEditControl::OnWMChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if (m_pTextStoreACP->_IsLocked(TS_LF_READ)) {
        m_pTextStoreACP->Push(hWnd, WM_CHAR, wParam, lParam);
        return S_OK;
    }

    std::wstring s(1, (wchar_t) wParam);
    m_pTextStoreACP->InsertTextAtACP(s);

    Invalidate();
    NotifySelectionHasChanged();
    return S_OK;
}

HRESULT touchmind::control::DWriteEditControl::OnWMChar_Backspace(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if (m_pTextStoreACP->_IsLocked(TS_LF_READ)) {
        m_pTextStoreACP->Push(hWnd, WM_CHAR, wParam, lParam);
        return S_OK;
    }

    m_pTextStoreACP->Backspace();

    Invalidate();
    NotifySelectionHasChanged();
    return S_OK;
}

HRESULT touchmind::control::DWriteEditControl::OnKeyDown_Delete(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    if (m_pTextStoreACP->_IsLocked(TS_LF_READ)) {
        m_pTextStoreACP->Push(hWnd, WM_CHAR, wParam, lParam);
        return S_OK;
    }

    m_pTextStoreACP->Delete();

    Invalidate();
    NotifySelectionHasChanged();
    return S_OK;
}

HRESULT touchmind::control::DWriteEditControl::OnKeyDown_CursorUpDown(DWORD cursorDirection)
{
    bool isShiftKeyDown = (GetKeyState(VK_SHIFT) & 0x80) == 0x80;

    if (m_pTextStoreACP->_IsLocked(TS_LF_READ)) {
        return S_OK;
    }

    m_pTextStoreACP->_LockDocument(TS_LF_READWRITE);

    LONG caretPosition;
    TsActiveSelEnd activeSelEnd;
    bool ret = CalculateNewCaretPositionForUpDown(cursorDirection, OUT & caretPosition, OUT & activeSelEnd);

    LONG newAcpStart = m_pTextStoreACP->GetACPStart();
    LONG newAcpEnd = m_pTextStoreACP->GetACPEnd();
    TsActiveSelEnd newActiveSelEnd = m_pTextStoreACP->GetActiveSelEnd();

    if (ret) {
        if (!isShiftKeyDown) {
            newAcpStart = newAcpEnd = caretPosition;
            newActiveSelEnd = activeSelEnd;
        } else {
            if (activeSelEnd == TS_AE_START || activeSelEnd == TS_AE_NONE) {
                if (caretPosition <= newAcpEnd) {
                    newAcpStart = caretPosition;
                } else {
                    newAcpStart = newAcpEnd;
                    newAcpEnd = caretPosition;
                    newActiveSelEnd = TS_AE_END;
                }
            } else {
                if (caretPosition > newAcpStart) {
                    newAcpEnd = caretPosition;
                } else {
                    newAcpEnd = newAcpStart;
                    newAcpStart = caretPosition;
                    newActiveSelEnd = TS_AE_START;
                }
            }
        }
    }

    _SetSelection(newAcpStart, newAcpEnd, newActiveSelEnd);

    m_pTextStoreACP->_UnlockDocument();

    m_pTextStoreACP->GetTextStoreACPSink()->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);

    Invalidate();
    NotifySelectionHasChanged();
    return S_OK;
}

HRESULT touchmind::control::DWriteEditControl::OnKeyDown_CursorLeftRight(DWORD cursorDirection)
{
    bool isShiftKeyDown = (GetKeyState(VK_SHIFT) & 0x80) == 0x80;
    if (m_pTextStoreACP->_IsLocked(TS_LF_READ)) {
        return S_OK;
    }

    m_pTextStoreACP->_LockDocument(TS_LF_READWRITE);

    LONG newAcpStart = m_pTextStoreACP->GetACPStart();
    LONG newAcpEnd = m_pTextStoreACP->GetACPEnd();
    TsActiveSelEnd newActiveSelEnd = m_pTextStoreACP->GetActiveSelEnd();

    switch (cursorDirection) {
    case VK_LEFT:
        if (!isShiftKeyDown) {
            if (newAcpStart == newAcpEnd) {
                newAcpStart--;
                if (newAcpStart < 0) {
                    newAcpStart = 0;
                }
                newAcpEnd = newAcpStart;
                newActiveSelEnd = TS_AE_START;
            } else {
                newAcpEnd = newAcpStart;
                newActiveSelEnd = TS_AE_START;
            }
        } else {
            switch (newActiveSelEnd) {
            case TS_AE_END:
                newAcpEnd--;
                if (newAcpEnd < 0) {
                    newAcpEnd = 0;
                }
                if (newAcpEnd < newAcpStart) {
                    // caret position move over the end of the selection
                    std::swap(newAcpStart, newAcpEnd);
                    newActiveSelEnd = TS_AE_START;
                }
                break;
            default: // TS_AE_START, TS_AE_NONE
                newAcpStart--;
                if (newAcpStart < 0) {
                    newAcpStart = 0;
                }
                break;
            }
            if (newAcpStart == newAcpEnd) {
                // there is no selection, then the caret move to the start of the selection
                newActiveSelEnd = TS_AE_START;
            }
        }
        break;
    case VK_RIGHT:
        if (!isShiftKeyDown) {
            if (newAcpStart == newAcpEnd) {
                ++newAcpStart;
                if (newAcpStart > (LONG) m_pTextStoreACP->GetTextLength()) {
                    newAcpStart = static_cast<LONG>(m_pTextStoreACP->GetTextLength());
                }
                newAcpEnd = newAcpStart;
                newActiveSelEnd = TS_AE_START;
            } else {
                newAcpStart = newAcpEnd;
                newActiveSelEnd = TS_AE_START;
            }
        } else {
            switch (newActiveSelEnd) {
            case TS_AE_START:
                ++newAcpStart;
                if (newAcpStart > (LONG) m_pTextStoreACP->GetTextLength()) {
                    newAcpStart = static_cast<LONG>(m_pTextStoreACP->GetTextLength());
                }
                if (newAcpEnd < newAcpStart) {
                    // caret position move over the end of the selection
                    std::swap(newAcpStart, newAcpEnd);
                    newActiveSelEnd = TS_AE_END;
                }
                break;
            default: // TS_AE_END, TS_AE_NONE
                ++newAcpEnd;
                if (newAcpEnd > (LONG) m_pTextStoreACP->GetTextLength()) {
                    newAcpEnd = static_cast<LONG>(m_pTextStoreACP->GetTextLength());
                }
                break;
            }
            if (newAcpStart == newAcpEnd) {
                // there is no selection, then the caret move to the start of the selection
                newActiveSelEnd = TS_AE_START;
            }
        }
        break;
    }

    _SetSelection(newAcpStart, newAcpEnd, newActiveSelEnd);

    m_pTextStoreACP->_UnlockDocument();

    m_pTextStoreACP->GetTextStoreACPSink()->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);

    Invalidate();
    NotifySelectionHasChanged();
    return S_OK;
}

void touchmind::control::DWriteEditControl::GetLineMetrics(OUT std::vector<DWRITE_LINE_METRICS>& lineMetrics)
{
    DWRITE_TEXT_METRICS textMetrics;
    m_pDWriteTextLayout->GetMetrics(&textMetrics);

    lineMetrics.resize(textMetrics.lineCount);
    m_pDWriteTextLayout->GetLineMetrics(&lineMetrics.front(), textMetrics.lineCount, &textMetrics.lineCount);
}

/// <summary>get a line number of the acp</summary>
void touchmind::control::DWriteEditControl::GetLineFromPosition(
    const DWRITE_LINE_METRICS* lineMetrics, // line metrics which is returned by GetLineMetrics
    UINT32 lineCount, // number of lineMetrics
    UINT32 textPosition, // acp
    OUT UINT32* lineOut, // line number(start from 0)
    OUT UINT32* linePositionOut // the start position(ACP) of the line
)
{
    UINT32 line = 0;
    UINT32 linePosition = 0;
    UINT32 nextLinePosition = 0;
    for (; line < lineCount; ++line) {
        linePosition = nextLinePosition;
        nextLinePosition = linePosition + lineMetrics[line].length;
        if (nextLinePosition > textPosition) {
            break;
        }
    }
    *linePositionOut = linePosition;
    *lineOut = std::min(line, lineCount - 1);
    return;
}

bool touchmind::control::DWriteEditControl::CalculateNewCaretPositionForUpDown(DWORD cursorDirection, OUT LONG* caretPosition, OUT TsActiveSelEnd* activeSelEnd)
{
    std::vector<DWRITE_LINE_METRICS> lineMetrics;
    GetLineMetrics(lineMetrics);

    UINT32 linePosition;
    UINT32 line;

    LONG acpStart = m_pTextStoreACP->GetACPStart();
    LONG acpEnd = m_pTextStoreACP->GetACPEnd();
    TsActiveSelEnd currentActiveSelEnd = m_pTextStoreACP->GetActiveSelEnd();

    *caretPosition = (currentActiveSelEnd != TS_AE_END ? acpStart : acpEnd);

    GetLineFromPosition(&lineMetrics.front(),
                        static_cast<UINT32>(lineMetrics.size()),
                        *caretPosition,
                        &line,
                        &linePosition
                       );

    if (cursorDirection == VK_UP) {
        if (line <= 0) {
            return false;
        }
        --line;
        linePosition -= lineMetrics[line].length;
    } else {
        linePosition += lineMetrics[line].length;
        ++line;
        if (line >= lineMetrics.size()) {
            return false;
        }
    }

    DWRITE_HIT_TEST_METRICS hitTestMetrics;
    FLOAT caretX, caretY, dummyX;

    // get a X coordinate of the caret
    m_pDWriteTextLayout->HitTestTextPosition(
        *caretPosition,
        (currentActiveSelEnd == TS_AE_END ? 1 : 0),
        &caretX,
        &caretY,
        &hitTestMetrics
    );

    // get a Y coordinate of the caret after moving lines
    m_pDWriteTextLayout->HitTestTextPosition(
        linePosition,
        false,
        &dummyX,
        &caretY,
        &hitTestMetrics
    );

    // We get the X and Y coordinate of the caret
    // get a new caret position(ACP)
    BOOL isInside, isTrailingHit;
    m_pDWriteTextLayout->HitTestPoint(
        caretX,
        caretY,
        &isTrailingHit,
        &isInside,
        &hitTestMetrics
    );

    *caretPosition = hitTestMetrics.textPosition;
    *activeSelEnd = (isTrailingHit && hitTestMetrics.length > 0) ? TS_AE_END : TS_AE_START;

    return true;
}

bool touchmind::control::DWriteEditControl::CalculateSelectionFromDragRect(
    D2D1_POINT_2F startPoint,
    D2D1_POINT_2F endPoint,
    OUT LONG* acpStart,
    OUT LONG* acpEnd,
    OUT TsActiveSelEnd* activeSelEnd)
{
    BOOL isTrailingHit;
    BOOL isInside;
    DWRITE_HIT_TEST_METRICS caretMetrics;

    FLOAT x = m_layoutRect.left;
    FLOAT y = m_layoutRect.top;

    m_pDWriteTextLayout->HitTestPoint(
        (FLOAT) startPoint.x - x,
        (FLOAT) startPoint.y - y,
        &isTrailingHit,
        &isInside,
        &caretMetrics
    );
    if (!isInside) {
        return false;
    }
    *acpStart = caretMetrics.textPosition;

    m_pDWriteTextLayout->HitTestPoint(
        (FLOAT) endPoint.x - x,
        (FLOAT) endPoint.y - y,
        &isTrailingHit,
        &isInside,
        &caretMetrics
    );
    if (!isInside) {
        return false;
    }
    *acpEnd = caretMetrics.textPosition;

    if (*acpStart < *acpEnd) {
        *activeSelEnd = TS_AE_END;
    } else {
        *activeSelEnd = TS_AE_START;
        std::swap(*acpStart, *acpEnd);
    }
    return true;
}

void touchmind::control::DWriteEditControl::Invalidate()
{
    ::InvalidateRect(m_hWnd, nullptr, FALSE);
}

/// <summary>set a new layout rect</summary>
void touchmind::control::DWriteEditControl::SetLayoutRect(D2D1_RECT_F layoutRect)
{
    // if the width of the layout rect is smaller than MIN_WIDTH, set the width to MIN_WIDTH
    if (HasFocus()) {
        if (layoutRect.right - layoutRect.left < MIN_WIDTH_ON_EDIT) {
            layoutRect.right = layoutRect.left + MIN_WIDTH_ON_EDIT;
        }
    } else {
        if (layoutRect.right - layoutRect.left < MIN_WIDTH) {
            layoutRect.right = layoutRect.left + MIN_WIDTH;
        }
    }
    // set the new layout rect
    m_layoutRect = layoutRect;
    // apply the new layout rect to the text layout
    m_pDWriteTextLayout->SetMaxWidth(m_layoutRect.right - m_layoutRect.left);
    m_pDWriteTextLayout->SetMaxHeight(m_layoutRect.bottom - m_layoutRect.top);
    UpdateActualRect();

    // text position(ACP) may be changed, notify TSF
    if (HasFocus() && m_pTextStoreACP->GetTextStoreACPSink() != nullptr) {
        m_pTextStoreACP->GetTextStoreACPSink()->OnLayoutChange(TS_LC_CHANGE, EDIT_VIEW_COOKIE);
    }
}

/// <summary>expand the layout rect</summary>
void touchmind::control::DWriteEditControl::ExpandLayoutRect(FLOAT dx, FLOAT dy)
{
    D2D1_RECT_F newLayoutRect = m_layoutRect;
    newLayoutRect.right += dx;
    newLayoutRect.bottom += dy;
    SetLayoutRect(newLayoutRect);
}

/// <summary>set the layout rect to the specified width and height</summary>
void touchmind::control::DWriteEditControl::ResizeLayoutRect(FLOAT width, FLOAT height)
{
    D2D1_RECT_F newLayoutRect = m_layoutRect;
    newLayoutRect.right = newLayoutRect.left + width;
    newLayoutRect.bottom = newLayoutRect.top + height;
    SetLayoutRect(newLayoutRect);
}

bool touchmind::control::DWriteEditControl::HasFocus()
{
    std::shared_ptr<control::DWriteEditControl> focusedEditControl = m_pManager->GetFocusedEditControl();
    if (focusedEditControl == shared_from_this()) {
        return true;
    }
    return false;
}

bool touchmind::control::DWriteEditControl::IsAdvised()
{
    return m_pTextStoreACP != nullptr && m_pTextStoreACP->IsAdvised();
}

touchmind::util::ScrollBarHelper* touchmind::control::DWriteEditControl::GetScrollBarHelper()
{
    return m_pManager->GetScrollBarHelper();
}

void touchmind::control::DWriteEditControl::GetText(std::wstring& text)
{
    m_pTextStoreACP->GetText(text);
}

bool touchmind::control::DWriteEditControl::HasSelection()
{
    if (m_pTextStoreACP->_IsLocked(TS_LF_READ)) {
        return false;
    }
    m_pTextStoreACP->_LockDocument(TS_LF_READ);
    LONG acpStart = m_pTextStoreACP->GetACPStart();
    LONG acpEnd = m_pTextStoreACP->GetACPEnd();
    m_pTextStoreACP->_UnlockDocument();
    return acpStart != acpEnd;
}

///<summary>get font attributes of the current selection. if no selection, return the font attributes of the caret position</summary>
void touchmind::control::DWriteEditControl::GetFontInfoOfCurrentSelection(touchmind::text::FontAttributeCommand* pFontInfo)
{
    if (m_pTextStoreACP->_IsLocked(TS_LF_READ)) {
        return;
    }
    m_pTextStoreACP->_LockDocument(TS_LF_READ);

    LONG acpStart = m_pTextStoreACP->GetACPStart();
    LONG acpEnd = m_pTextStoreACP->GetACPEnd();

    // if there is a selection
    if (acpStart != acpEnd) {
        DWRITE_TEXT_RANGE textRange;
        textRange.startPosition = acpStart;
        textRange.length = acpEnd - acpStart;
        _GetFontAttributes(m_pDWriteTextLayout, textRange, pFontInfo);
    }
    // if no selection
    else {
        pFontInfo->fontFamilyNameStatus = m_caretFontAttributeCommand.fontFamilyNameStatus;
        pFontInfo->fontFamilyName = m_caretFontAttributeCommand.fontFamilyName;
        pFontInfo->localeNameStatus = m_caretFontAttributeCommand.localeNameStatus;
        pFontInfo->localeName = m_caretFontAttributeCommand.localeName;
        pFontInfo->bold = m_caretFontAttributeCommand.bold;
        pFontInfo->italic = m_caretFontAttributeCommand.italic;
        pFontInfo->underline = m_caretFontAttributeCommand.underline;
        pFontInfo->strikethrough = m_caretFontAttributeCommand.strikethrough;
        pFontInfo->fontSizeStatus = m_caretFontAttributeCommand.fontSizeStatus;
        pFontInfo->fontSize = m_caretFontAttributeCommand.fontSize;
        pFontInfo->foregroundColorStatus = m_caretFontAttributeCommand.foregroundColorStatus;
        pFontInfo->foregroundColor = m_caretFontAttributeCommand.foregroundColor;
    }

    m_pTextStoreACP->_UnlockDocument();
}

/// <summary>return font attributes of the all text</summary>
void touchmind::control::DWriteEditControl::GetFontInfoOfAllText(touchmind::text::FontAttributeCommand* pFontInfo)
{
    DWRITE_TEXT_RANGE textRange;
    textRange.startPosition = 0;
    textRange.length = static_cast<UINT32>(m_pTextStoreACP->GetTextLength());
    _GetFontAttributes(m_pDWriteTextLayout, textRange, pFontInfo);
}

void touchmind::control::DWriteEditControl::SetFontInfoToCurrentSelection(touchmind::text::FontAttributeCommand* pFontInfo)
{
    if (m_pTextStoreACP->_IsLocked(TS_LF_READWRITE)) {
        return;
    }
    m_pTextStoreACP->_LockDocument(TS_LF_READWRITE);

    LONG acpStart = m_pTextStoreACP->GetACPStart();
    LONG acpEnd = m_pTextStoreACP->GetACPEnd();
    // if there is a selection
    if (acpStart != acpEnd) {
        DWRITE_TEXT_RANGE textRange;
        textRange.startPosition = acpStart;
        textRange.length = acpEnd - acpStart;
        _SetFontAttributes(m_pDWriteTextLayout, pFontInfo, textRange);
    }
    //if there is no selection
    else {
        if (pFontInfo->fontFamilyNameStatus == UI_FONTPROPERTIES_SET) {
            m_caretFontAttributeCommand.fontFamilyNameStatus = UI_FONTPROPERTIES_SET;
            m_caretFontAttributeCommand.fontFamilyName = pFontInfo->fontFamilyName;
        }
        if (pFontInfo->localeNameStatus == UI_FONTPROPERTIES_SET) {
            m_caretFontAttributeCommand.localeNameStatus = UI_FONTPROPERTIES_SET;
            m_caretFontAttributeCommand.localeName = pFontInfo->localeName;
        }
        if (pFontInfo->bold != UI_FONTPROPERTIES_NOTAVAILABLE) {
            m_caretFontAttributeCommand.bold = pFontInfo->bold;
        }
        if (pFontInfo->italic != UI_FONTPROPERTIES_NOTAVAILABLE) {
            m_caretFontAttributeCommand.italic = pFontInfo->italic;
        }
        if (pFontInfo->underline != UI_FONTPROPERTIES_NOTAVAILABLE) {
            m_caretFontAttributeCommand.underline = pFontInfo->underline;
        }
        if (pFontInfo->strikethrough != UI_FONTPROPERTIES_NOTAVAILABLE) {
            m_caretFontAttributeCommand.strikethrough = pFontInfo->strikethrough;
        }
        if (pFontInfo->fontSizeStatus == UI_FONTPROPERTIES_SET) {
            m_caretFontAttributeCommand.fontSizeStatus = UI_FONTPROPERTIES_SET;
            m_caretFontAttributeCommand.fontSize = pFontInfo->fontSize;
        }
        if (pFontInfo->foregroundColorStatus == UI_FONTPROPERTIES_SET) {
            m_caretFontAttributeCommand.foregroundColorStatus = UI_FONTPROPERTIES_SET;
            m_caretFontAttributeCommand.foregroundColor = pFontInfo->foregroundColor;
        }
    }

    m_pTextStoreACP->_UnlockDocument();
    // the layout rect may be changed because the font attributes are changed.
    UpdateActualRect();
}

void touchmind::control::DWriteEditControl::SetFontInfoToAllText(touchmind::text::FontAttributeCommand* pFontInfo)
{
    if (m_pDWriteTextLayout != nullptr) {
        DWRITE_TEXT_RANGE textRange;
        textRange.startPosition = 0;
        textRange.length = INFINITE_TEXT_END;
        _SetFontAttributes(m_pDWriteTextLayout, pFontInfo, textRange);
        // the actual rect may be changed because the font attributes are changed.
        UpdateActualRect();
    }
}

/// <summary>notify a selection change to the edit control manager</summary>
/// This cause a notification to the ribbon framework
void touchmind::control::DWriteEditControl::NotifySelectionHasChanged()
{
    m_pManager->NotifySelectionHasChanged(shared_from_this());
    UpdateFontInfoAtCaret();
}

/// <summary>change selection</summary>
/// set a new selection when a user select a text by mouse or keyboard
/// this method is invoked by OnMouseEventsAOnKeyDown_CursorUpDownAOnKeyDown_CursorLeftRight
HRESULT touchmind::control::DWriteEditControl::_SetSelection(LONG acpStart, LONG acpEnd, TsActiveSelEnd activeSelEnd)
{
    HRESULT hr = m_pTextStoreACP->ChangeACPWithoutLock(acpStart, acpEnd, activeSelEnd);
    return hr;
}

void touchmind::control::DWriteEditControl::CopySinglePropertyRange(
    IDWriteTextLayout* oldLayout,
    UINT32 startPosForOld,
    IDWriteTextLayout* newLayout,
    UINT32 startPosForNew,
    UINT32 length,
    touchmind::text::FontAttributeCommand* pFontAttributeCommand
)
{
    DWRITE_TEXT_RANGE range = {startPosForNew,  std::min(length, static_cast<UINT32>(INFINITE_TEXT_END - startPosForNew))};

    // font collection
    IDWriteFontCollection* fontCollection = nullptr;
    oldLayout->GetFontCollection(startPosForOld, &fontCollection);
    newLayout->SetFontCollection(fontCollection, range);
    SafeRelease(&fontCollection);

    if (pFontAttributeCommand != nullptr) {
        newLayout->SetFontFamilyName(pFontAttributeCommand->fontFamilyName.c_str(), range);
        newLayout->SetLocaleName(pFontAttributeCommand->localeName.c_str(), range);
        newLayout->SetFontWeight(pFontAttributeCommand->GetFontWeight(), range);
        newLayout->SetFontStyle(pFontAttributeCommand->GetFontStyle(), range);
        newLayout->SetFontStretch(pFontAttributeCommand->GetFontStretch(), range);
        newLayout->SetFontSize(pFontAttributeCommand->fontSize, range);
        newLayout->SetUnderline(pFontAttributeCommand->GetUnderline(), range);
        newLayout->SetStrikethrough(pFontAttributeCommand->GetStrikethrough(), range);
    } else {
        // font family
        wchar_t fontFamilyName[100];
        fontFamilyName[0] = '\0';
        oldLayout->GetFontFamilyName(startPosForOld, &fontFamilyName[0], ARRAYSIZE(fontFamilyName));
        newLayout->SetFontFamilyName(fontFamilyName, range);

        // weight/width/slope
        DWRITE_FONT_WEIGHT weight   = DWRITE_FONT_WEIGHT_NORMAL;
        DWRITE_FONT_STYLE style     = DWRITE_FONT_STYLE_NORMAL;
        DWRITE_FONT_STRETCH stretch = DWRITE_FONT_STRETCH_NORMAL;
        oldLayout->GetFontWeight(startPosForOld, &weight);
        oldLayout->GetFontStyle(startPosForOld, &style);
        oldLayout->GetFontStretch(startPosForOld, &stretch);

        newLayout->SetFontWeight(weight, range);
        newLayout->SetFontStyle(style, range);
        newLayout->SetFontStretch(stretch, range);

        // font size
        FLOAT fontSize = 12.0;
        oldLayout->GetFontSize(startPosForOld, &fontSize);
        newLayout->SetFontSize(fontSize, range);

        // underline and strikethrough
        BOOL value = FALSE;
        oldLayout->GetUnderline(startPosForOld, &value);
        newLayout->SetUnderline(value, range);
        oldLayout->GetStrikethrough(startPosForOld, &value);
        newLayout->SetStrikethrough(value, range);

        // locale
        wchar_t localeName[LOCALE_NAME_MAX_LENGTH];
        localeName[0] = '\0';
        oldLayout->GetLocaleName(startPosForOld, &localeName[0], ARRAYSIZE(localeName));
        newLayout->SetLocaleName(localeName, range);
    }

    // drawing effect
    IUnknown* drawingEffect = nullptr;
    oldLayout->GetDrawingEffect(startPosForOld, &drawingEffect);
    newLayout->SetDrawingEffect(drawingEffect, range);
    SafeRelease(&drawingEffect);

    // inline object
    IDWriteInlineObject* inlineObject = nullptr;
    oldLayout->GetInlineObject(startPosForOld, &inlineObject);
    newLayout->SetInlineObject(inlineObject, range);
    SafeRelease(&inlineObject);

    // typography
    IDWriteTypography* typography = nullptr;
    oldLayout->GetTypography(startPosForOld, &typography);
    newLayout->SetTypography(typography, range);
    SafeRelease(&typography);
}

/// <summary>get a length of the text which has same font attributes</summary>
UINT32 touchmind::control::DWriteEditControl::CalculateRangeLengthAt(IDWriteTextLayout* layout, UINT32 pos)
{
    DWRITE_TEXT_RANGE incrementAmount = {pos, 1};
    DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL;

    layout->GetFontWeight(
        pos,
        &weight,
        &incrementAmount
    );

    UINT32 rangeLength = incrementAmount.length - (pos - incrementAmount.startPosition);
    return rangeLength;
}

/// <summary>copy font attributes from the old layout to the new layout</summary>
/// if the length of the old layout and the new layout were different (because insert or remove the sub text),
/// you can specify an offset and offset negative.
void touchmind::control::DWriteEditControl::CopyRangedProperties(
    IDWriteTextLayout* oldLayout,
    UINT32 startPos,
    UINT32 endPos,
    UINT32 newLayoutTextOffset, // specify how many characters are shifted
    IDWriteTextLayout* newLayout,
    bool isOffsetNegative // specify true if the offset is negative
)
{
    UINT32 currentPos = startPos;
    while (currentPos < endPos) {
        UINT32 rangeLength = CalculateRangeLengthAt(oldLayout, currentPos);
        rangeLength = std::min(rangeLength, endPos - currentPos);
        if (isOffsetNegative) {
            CopySinglePropertyRange(
                oldLayout,
                currentPos,
                newLayout,
                currentPos - newLayoutTextOffset,
                rangeLength
            );
        } else {
            CopySinglePropertyRange(
                oldLayout,
                currentPos,
                newLayout,
                currentPos + newLayoutTextOffset,
                rangeLength
            );
        }
        currentPos += rangeLength;
    }
}

void touchmind::control::DWriteEditControl::CopyGlobalProperties(IDWriteTextLayout* oldLayout, IDWriteTextLayout* newLayout)
{
    newLayout->SetTextAlignment(oldLayout->GetTextAlignment());
    newLayout->SetParagraphAlignment(oldLayout->GetParagraphAlignment());
    newLayout->SetWordWrapping(oldLayout->GetWordWrapping());
    newLayout->SetReadingDirection(oldLayout->GetReadingDirection());
    newLayout->SetFlowDirection(oldLayout->GetFlowDirection());
    newLayout->SetIncrementalTabStop(oldLayout->GetIncrementalTabStop());

    DWRITE_TRIMMING trimmingOptions = {};
    IDWriteInlineObject* inlineObject = nullptr;
    oldLayout->GetTrimming(&trimmingOptions, &inlineObject);
    newLayout->SetTrimming(&trimmingOptions, inlineObject);
    SafeRelease(&inlineObject);

    DWRITE_LINE_SPACING_METHOD lineSpacingMethod = DWRITE_LINE_SPACING_METHOD_DEFAULT;
    FLOAT lineSpacing = 0;
    FLOAT baseline = 0;
    oldLayout->GetLineSpacing(&lineSpacingMethod, &lineSpacing, &baseline);
    newLayout->SetLineSpacing(lineSpacingMethod, lineSpacing, baseline);
}

/// <summary>text replacement notification from  DWriteEditControlTextStoreACP</summary>
/// <param name="oldTextLength">text length before replacement</param>
/// <param name="startPos">start position of the replaced text</param>
/// <param name="replacedTextLength">length of the replaced text</param>
/// <param name="lengthOfSubtext">length of the new sub text</param>
void touchmind::control::DWriteEditControl::NotifyTextReplace(LONG oldTextLength, LONG startPos, LONG replacedTextLength, LONG lengthOfNewText)
{
    HRESULT hr = S_OK;
    LONG newTextLength = oldTextLength - replacedTextLength + lengthOfNewText;

    IDWriteTextLayout *oldLayout = SafeAcquire(m_pDWriteTextLayout);
    _RecreateLayout();
    IDWriteTextLayout *newLayout = m_pDWriteTextLayout;

    if (SUCCEEDED(hr)) {
        CopyGlobalProperties(oldLayout, newLayout);
        if (startPos == 0) {
            CopySinglePropertyRange(oldLayout, 0, newLayout, 0, lengthOfNewText);
            CopyRangedProperties(oldLayout, replacedTextLength, oldTextLength,
                                 std::abs(lengthOfNewText - replacedTextLength),
                                 newLayout,
                                 lengthOfNewText < replacedTextLength
                                );
        } else {
            CopyRangedProperties(oldLayout, 0, startPos, 0, newLayout);
            CopySinglePropertyRange(oldLayout, startPos, newLayout, startPos, lengthOfNewText, &m_caretFontAttributeCommand);
            CopyRangedProperties(oldLayout, startPos + replacedTextLength, oldTextLength,
                                 std::abs(lengthOfNewText - replacedTextLength),
                                 newLayout,
                                 lengthOfNewText < replacedTextLength
                                );
        }
        CopySinglePropertyRange(oldLayout, oldTextLength, newLayout, newTextLength, INFINITE_TEXT_END);
    }
    SafeRelease(&oldLayout);
}

/// <summary>text insertion notification from  DWriteEditControlTextStoreACP</summary>
/// <param name="oldTextLength">text length before insertion</param>
/// <param name="startPos">start position</param>
/// <param name="textToInsertLength">length of inserted text</param>
void touchmind::control::DWriteEditControl::NotifyTextInsert(LONG oldTextLength, LONG startPos, LONG textToInsertLength)
{
    HRESULT hr = S_OK;
    LONG newTextLength = oldTextLength + textToInsertLength;

    IDWriteTextLayout *oldLayout = SafeAcquire(m_pDWriteTextLayout);
    _RecreateLayout();
    IDWriteTextLayout *newLayout = m_pDWriteTextLayout;

    if (SUCCEEDED(hr)) {
        CopyGlobalProperties(oldLayout, newLayout);
        if (startPos == 0) {
            CopySinglePropertyRange(oldLayout, 0, newLayout, 0, textToInsertLength);
            CopyRangedProperties(oldLayout, 0, oldTextLength, textToInsertLength, newLayout);
        } else {
            CopyRangedProperties(oldLayout, 0, startPos, 0, newLayout);
            CopySinglePropertyRange(oldLayout, startPos - 1, newLayout, startPos, textToInsertLength, &m_caretFontAttributeCommand);
            CopyRangedProperties(oldLayout, startPos, oldTextLength, textToInsertLength, newLayout);
        }
        CopySinglePropertyRange(oldLayout, oldTextLength, newLayout, newTextLength, INFINITE_TEXT_END);
    }
    SafeRelease(&oldLayout);
}

/// <summary>text remove notification from  DWriteEditControlTextStoreACP</summary>
/// <param name="oldTextLength">text length before insertion</param>
/// <param name="startPos">start position</param>
/// <param name="removedTextLength">length of removed text</param>
void touchmind::control::DWriteEditControl::NotifyTextRemove(LONG oldTextLength, LONG startPos, LONG lengthToRemove)
{
    HRESULT hr = S_OK;
    LONG newTextLength = oldTextLength - lengthToRemove;

    IDWriteTextLayout *oldLayout = SafeAcquire(m_pDWriteTextLayout);
    _RecreateLayout();
    IDWriteTextLayout *newLayout = m_pDWriteTextLayout;

    if (SUCCEEDED(hr)) {
        CopyGlobalProperties(oldLayout, newLayout);

        if (startPos == 0) {
            CopyRangedProperties(oldLayout,
                                 lengthToRemove, oldTextLength,
                                 lengthToRemove,
                                 newLayout,
                                 true);
        } else {
            CopyRangedProperties(oldLayout, 0, startPos, 0, newLayout, true);
            CopyRangedProperties(oldLayout, startPos + lengthToRemove, oldTextLength, lengthToRemove, newLayout, true);
        }
        CopySinglePropertyRange(oldLayout, oldTextLength, newLayout, newTextLength, INFINITE_TEXT_END);
    }
    SafeRelease(&oldLayout);
}

void touchmind::control::DWriteEditControl::UpdateFontInfoAtCaret()
{
    LONG currentPos = m_pTextStoreACP->GetActiveSelEnd() != TF_AE_END ? m_pTextStoreACP->GetACPStart() : m_pTextStoreACP->GetACPEnd();
    if (currentPos > 0) {
        --currentPos;
    }
    _GetFontAttributes(m_pDWriteTextLayout, currentPos, &m_caretFontAttributeCommand);
}

HRESULT touchmind::control::DWriteEditControl::GetFontAttributes(std::vector<touchmind::text::FontAttribute>& fontAttributes)
{
    HRESULT hr = S_OK;
    fontAttributes.clear();
    UINT32 pos = 0;
    while (pos < m_pTextStoreACP->GetTextLength()) {
        DWRITE_TEXT_RANGE textRange;
        textRange.startPosition = pos;
        textRange.length = 1;
        touchmind::text::FontAttributeCommand fontAttributeCommand;
        DWRITE_TEXT_RANGE actualTextRange;
        _GetFontAttributes(m_pDWriteTextLayout, textRange, &fontAttributeCommand, &actualTextRange);

        touchmind::text::FontAttribute fontAttribute;
        fontAttributeCommand.ToFontAttribute(&fontAttribute);
        fontAttribute.startPosition = actualTextRange.startPosition;
        fontAttribute.length = std::min(
                                   static_cast<LONG>(m_pTextStoreACP->GetTextLength() - actualTextRange.startPosition),
                                   static_cast<LONG>(actualTextRange.length));
        fontAttributes.push_back(fontAttribute);
        pos += fontAttribute.length;
    }
    return hr;
}

HRESULT touchmind::control::DWriteEditControl::SetFontAttributes(touchmind::Configuration* pConfiguration, std::vector<touchmind::text::FontAttribute>& fontAttributes)
{
    HRESULT hr = S_OK;
    for (auto it = std::begin(fontAttributes); it != std::end(fontAttributes); ++it) {
        DWRITE_TEXT_RANGE textRange;
        textRange.startPosition = it->startPosition;
        textRange.length = it->length;
        touchmind::text::FontAttributeCommand fontAttributeCommand;
        fontAttributeCommand.FromFontAttribute(pConfiguration, & (*it));
        _SetFontAttributes(m_pDWriteTextLayout, &fontAttributeCommand, textRange);
    }
    return hr;
}

void touchmind::control::DWriteEditControl::MoveCaretToEnd()
{
    m_pTextStoreACP->MoveACPToEnd();
    UpdateCaret();
}

HRESULT touchmind::control::DWriteEditControl::CreateDeviceResources( ID2D1RenderTarget *pRenderTarget )
{
    HRESULT hr = S_OK;

    if (m_pForegroundBrush == nullptr) {
        hr = pRenderTarget->CreateSolidColorBrush(m_foregroundColor, &m_pForegroundBrush);
#ifdef DEBUG_GPU_RESOURCE
        LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1SolidColorBrush = [" << std::hex << m_pForegroundBrush << L"]" << std::dec;
#endif
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"Failed to create foreground color brush, hr = " << hr;
        }
    }
    if (m_pBackgroundBrush == nullptr) {
        hr = pRenderTarget->CreateSolidColorBrush(m_backgroundColor, &m_pBackgroundBrush);
#ifdef DEBUG_GPU_RESOURCE
        LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1SolidColorBrush = [" << std::hex << m_pBackgroundBrush << L"]" << std::dec;
#endif
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"Failed to create background color brush, hr = " << hr;
        }
    }
    if (m_pBorderLineBrush == nullptr) {
        hr = pRenderTarget->CreateSolidColorBrush(m_borderLineColor, &m_pBorderLineBrush);
#ifdef DEBUG_GPU_RESOURCE
        LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1SolidColorBrush = [" << std::hex << m_pBorderLineBrush << L"]" << std::dec;
#endif
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"Failed to create border line color brush, hr = " << hr;
        }
    }

    if (m_pForegroundBrushForEdit == nullptr) {
        hr = pRenderTarget->CreateSolidColorBrush(m_foregroundColorForEdit, &m_pForegroundBrushForEdit);
#ifdef DEBUG_GPU_RESOURCE
        LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1SolidColorBrush = [" << std::hex << m_pForegroundBrushForEdit << L"]" << std::dec;
#endif
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"Failed to create foreground color brush for edit, hr = " << hr;
        }
    }
    if (m_pBackgroundBrushForEdit == nullptr) {
        hr = pRenderTarget->CreateSolidColorBrush(m_backgroundColorForEdit, &m_pBackgroundBrushForEdit);
#ifdef DEBUG_GPU_RESOURCE
        LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1SolidColorBrush = [" << std::hex << m_pBackgroundBrushForEdit << L"]" << std::dec;
#endif
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"Failed to create background color brush for edit, hr = " << hr;
        }
    }
    if (m_pBorderLineBrushForEdit == nullptr) {
        hr = pRenderTarget->CreateSolidColorBrush(m_borderLineColorForEdit, &m_pBorderLineBrushForEdit);
#ifdef DEBUG_GPU_RESOURCE
        LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1SolidColorBrush = [" << std::hex << m_pBorderLineBrushForEdit << L"]" << std::dec;
#endif
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"Failed to create border line color brush for edit, hr = " << hr;
        }
    }

    if (m_pHighlightBrush == nullptr) {
        hr = pRenderTarget->CreateSolidColorBrush(m_highlightColor, &m_pHighlightBrush);
#ifdef DEBUG_GPU_RESOURCE
        LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1SolidColorBrush = [" << std::hex << m_pHighlightBrush << L"]" << std::dec;
#endif
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"Failed to create highlight color brush, hr = " << hr;
        }
    }
    if (m_pCompositionUnderlineBrush == nullptr) {
        hr = pRenderTarget->CreateSolidColorBrush(m_compositionUnderlineColor, &m_pCompositionUnderlineBrush);
#ifdef DEBUG_GPU_RESOURCE
        LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1SolidColorBrush = [" << std::hex << m_pCompositionUnderlineBrush << L"]" << std::dec;
#endif
        if (FAILED(hr)) {
            LOG(SEVERITY_LEVEL_ERROR) << L"Failed to create composition underline color brush, hr = " << hr;
        }
    }
    return hr;
}

HRESULT touchmind::control::DWriteEditControl::CreateDeviceIndependentResources()
{
    HRESULT hr = S_OK;
    ID2D1Factory *pD2DFactory = m_pManager->GetD2D1Factory();

    hr = pD2DFactory->CreateStrokeStyle(
             D2D1::StrokeStyleProperties(
                 D2D1_CAP_STYLE_FLAT,
                 D2D1_CAP_STYLE_FLAT,
                 D2D1_CAP_STYLE_FLAT,
                 D2D1_LINE_JOIN_MITER,
                 10.0f,
                 D2D1_DASH_STYLE_SOLID,
                 0.0f),
             nullptr,
             0,
             &m_pCompositionSolidUnderlineStrokeStyle
         );
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1StrokeStyle = [" << std::hex << m_pCompositionSolidUnderlineStrokeStyle << L"]" << std::dec;
#endif
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << L"Failed to create solid stroke style, hr = " << hr;
    }

    hr = pD2DFactory->CreateStrokeStyle(
             D2D1::StrokeStyleProperties(
                 D2D1_CAP_STYLE_ROUND,
                 D2D1_CAP_STYLE_ROUND,
                 D2D1_CAP_STYLE_ROUND,
                 D2D1_LINE_JOIN_MITER,
                 10.0f,
                 D2D1_DASH_STYLE_DOT,
                 0.0f
             ),
             nullptr,
             0,
             &m_pCompositionDotUnderlineStrokeStyle);
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1StrokeStyle = [" << std::hex << m_pCompositionDotUnderlineStrokeStyle << L"]" << std::dec;
#endif
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << L"Failed to create dot stroke style, hr = " << hr;
    }

    hr = pD2DFactory->CreateStrokeStyle(
             D2D1::StrokeStyleProperties(
                 D2D1_CAP_STYLE_FLAT,
                 D2D1_CAP_STYLE_FLAT,
                 D2D1_CAP_STYLE_FLAT,
                 D2D1_LINE_JOIN_MITER,
                 10.0f,
                 D2D1_DASH_STYLE_DASH,
                 0.0f
             ),
             nullptr,
             0,
             &m_pCompositionDashUnderlineStrokeStyle);
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1StrokeStyle = [" << std::hex << m_pCompositionDashUnderlineStrokeStyle << L"]" << std::dec;
#endif
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << L"Failed to create dash stroke style, hr = " << hr;
    }

    hr = pD2DFactory->CreateStrokeStyle(
             D2D1::StrokeStyleProperties(
                 D2D1_CAP_STYLE_FLAT,
                 D2D1_CAP_STYLE_FLAT,
                 D2D1_CAP_STYLE_FLAT,
                 D2D1_LINE_JOIN_MITER,
                 10.0f,
                 D2D1_DASH_STYLE_DASH_DOT,
                 0.0f
             ),
             nullptr,
             0,
             &m_pCompositionSquiggleUnderlineStrokeStyle);
#ifdef DEBUG_GPU_RESOURCE
    LOG(SEVERITY_LEVEL_INFO) << L"[GPU RESOURCE] ID2D1StrokeStyle = [" << std::hex << m_pCompositionSquiggleUnderlineStrokeStyle << L"]" << std::dec;
#endif
    if (FAILED(hr)) {
        LOG(SEVERITY_LEVEL_ERROR) << L"Failed to create dash dot stroke style, hr = " << hr;
    }

    return hr;
}

void touchmind::control::DWriteEditControl::AddCompositionRenderInfo( LONG startPos, LONG endPos, const TF_DISPLAYATTRIBUTE* pDisplayAttribute )
{
    CompositionAttribute ca;
    ca.startPos = startPos;
    ca.endPos = endPos;
    ca.displayAttribute = *pDisplayAttribute;
    m_compositionAttributes.push_back(ca);
}

void touchmind::control::DWriteEditControl::ClearCompositionRenderInfo()
{
    m_compositionAttributes.clear();
}

void touchmind::control::DWriteEditControl::OnDebugDump( std::wofstream& os )
{
    os << L"DWriteEditControl[layoutRect=(" << m_layoutRect.left << L"," << m_layoutRect.top
       << L")-(" << m_layoutRect.right << L"," << m_layoutRect.bottom << L"), actualRect=("
       << m_layoutRect.left << L"," << m_layoutRect.top
       << L")-(" << m_layoutRect.right << L"," << m_layoutRect.bottom << L")]" << std::endl;
}

