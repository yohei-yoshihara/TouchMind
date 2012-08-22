#ifndef TOUCHMIND_CONTROL_DWRITEEDITCONTROL_H_
#define TOUCHMIND_CONTROL_DWRITEEDITCONTROL_H_

#include "forwarddecl.h"
#include "touchmind/control/DWriteEditControlCommon.h"
#include "touchmind/control/DWriteEditControlManager.h"
#include "touchmind/control/DWriteEditControlTextStoreACP.h"
#include "touchmind/control/DWriteEditControlTextEditSink.h"

namespace touchmind
{
namespace control
{

class CompositionAttribute
{
public:
    LONG startPos;
    LONG endPos;
    TF_DISPLAYATTRIBUTE displayAttribute;
};

class DWriteEditControl : public std::enable_shared_from_this<DWriteEditControl>
{
public:
    static const LONG INFINITE_TEXT_END;
private:
    static const FLOAT MIN_WIDTH;
    static const FLOAT MIN_WIDTH_ON_EDIT;

    EDIT_CONTROL_INDEX m_editControlIndex;
    HWND m_hWnd;
    DWriteEditControlManager *m_pManager;
    DWriteEditControlTextStoreACP* m_pTextStoreACP;
    DWriteEditControlTextEditSink* m_pTextEditSink;

    ITfDocumentMgr *m_pDocMgr;
    ITfContext *m_pTfContext;
    TfEditCookie m_editCookie;
    bool m_caretVisible;

    D2D1_RECT_F m_layoutRect;
    D2D1_RECT_F m_actualRect;

    D2D1_COLOR_F m_foregroundColor;
    D2D1_COLOR_F m_backgroundColor;
    D2D1_COLOR_F m_borderLineColor;

    D2D1_COLOR_F m_foregroundColorForEdit;
    D2D1_COLOR_F m_backgroundColorForEdit;
    D2D1_COLOR_F m_borderLineColorForEdit;

    D2D1_COLOR_F m_highlightColor;
    D2D1_COLOR_F m_compositionUnderlineColor;

    CComPtr<ID2D1SolidColorBrush> m_pForegroundBrush;
    CComPtr<ID2D1SolidColorBrush> m_pBackgroundBrush;
    CComPtr<ID2D1SolidColorBrush> m_pBorderLineBrush;

    CComPtr<ID2D1SolidColorBrush> m_pForegroundBrushForEdit;
    CComPtr<ID2D1SolidColorBrush> m_pBackgroundBrushForEdit;
    CComPtr<ID2D1SolidColorBrush> m_pBorderLineBrushForEdit;

    CComPtr<ID2D1SolidColorBrush> m_pHighlightBrush;
    CComPtr<ID2D1SolidColorBrush> m_pCompositionUnderlineBrush;

    CComPtr<ID2D1StrokeStyle>     m_pCompositionSolidUnderlineStrokeStyle;
    CComPtr<ID2D1StrokeStyle>     m_pCompositionDotUnderlineStrokeStyle;
    CComPtr<ID2D1StrokeStyle>     m_pCompositionDashUnderlineStrokeStyle;
    CComPtr<ID2D1StrokeStyle>     m_pCompositionSquiggleUnderlineStrokeStyle;

    IDWriteTextFormat *m_pDWriteTextFormat;
    IDWriteTextLayout *m_pDWriteTextLayout;

    touchmind::text::FontAttributeCommand m_caretFontAttributeCommand;

    std::vector<CompositionAttribute> m_compositionAttributes;

protected:
    void CreateCaret();
    void UpdateCaret();
    void DestroyCaret();
    void _GetFontAttributes(IN IDWriteTextLayout* pTextLayout, IN DWRITE_TEXT_RANGE& textRange, OUT touchmind::text::FontAttributeCommand* pFontInfo, OUT DWRITE_TEXT_RANGE* pActualTextRange = nullptr);
    void _GetFontAttributes(IN IDWriteTextLayout* pTextLayout, IN LONG acpStart, OUT touchmind::text::FontAttributeCommand* pFontInfo);
    void _SetFontAttributes(IN IDWriteTextLayout* pTextLayout, IN touchmind::text::FontAttributeCommand* pFontInfo, IN DWRITE_TEXT_RANGE& textRange);
    void _RecreateLayout();
    HRESULT _SetSelection(LONG acpStart, LONG acpEnd, TsActiveSelEnd activeSelEnd);
    void _CalculateCaretSizeAndPosition(D2D1_POINT_2F *caretPosInModel, DWRITE_HIT_TEST_METRICS *pHitTestMetrics);

public:
    DWriteEditControl(DWriteEditControlManager* manager, D2D1_RECT_F layoutRect, touchmind::text::FontAttributeCommand& initialFontInfo);
    virtual ~DWriteEditControl(void);
    void SetEditControlIndex(EDIT_CONTROL_INDEX editControlIndex) {
        m_editControlIndex = editControlIndex;
    }
    HRESULT Initialize(const std::wstring &text);
    HRESULT CreateDeviceIndependentResources();
    HRESULT CreateDeviceResources(ID2D1RenderTarget *pRenderTarget);
    bool IsAdvised();
    void SetForegroundColor(D2D1_COLOR_F foregroundColor) {
        m_foregroundColor = foregroundColor;
    }
    void SetBackgroundColor(D2D1_COLOR_F backgroundColor) {
        m_backgroundColor = backgroundColor;
    }
    void SetBorderLineColor(D2D1_COLOR_F borderLineColor) {
        m_borderLineColor = borderLineColor;
    }
    void SetForegroundColorForEdit(D2D1_COLOR_F foregroundColorForEdit) {
        m_foregroundColorForEdit = foregroundColorForEdit;
    }
    void SetBackgroundColorForEdit(D2D1_COLOR_F backgroundColorForEdit) {
        m_backgroundColorForEdit = backgroundColorForEdit;
    }
    void SetBorderLineColorForEdit(D2D1_COLOR_F borderLineColorForEdit) {
        m_borderLineColorForEdit = borderLineColorForEdit;
    }
    void SetHighlightBrush(D2D1_COLOR_F highlightColor) {
        m_highlightColor = highlightColor;
    }
    void SetCompositionUnderlineBrush(D2D1_COLOR_F compositionUnderlineColor) {
        m_compositionUnderlineColor = compositionUnderlineColor;
    }
    void SetLayoutRect(D2D1_RECT_F layoutRect);
    void ExpandLayoutRect(FLOAT dx, FLOAT dy);
    void ResizeLayoutRect(FLOAT width, FLOAT height);
    D2D1_RECT_F GetLayoutRect() const {
        return m_layoutRect;
    }
    D2D1_RECT_F GetActualRect() const {
        return m_actualRect;
    }
    HWND GetHWnd() const {
        return m_hWnd;
    }
    ITfContext* GetContext() const {
        return m_pTfContext;
    }
    TfEditCookie GetEditCookie() const {
        return m_editCookie;
    }
    void GetText(std::wstring& text);
    touchmind::util::ScrollBarHelper* GetScrollBarHelper();
    IDWriteTextLayout* GetTextLayout() const {
        return m_pDWriteTextLayout;
    }
    void SetFocus();
    void KillFocus();
    bool HasFocus();
    void MoveCaretToEnd();

    bool HitTest(FLOAT x, FLOAT y);

    HRESULT InitializeTextFormat();
    void OnRender(ID2D1RenderTarget *pRenderTarget);

    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    HRESULT OnMouseEvents(D2D1_POINT_2F dragStartPoint, D2D1_POINT_2F dragEndPoint);
    HRESULT OnWMChar(HWND hWnd, WPARAM wParam, LPARAM lParam);
    HRESULT OnWMChar_Backspace(HWND hWnd, WPARAM wParam, LPARAM lParam);
    HRESULT OnKeyDown_Delete(HWND hWnd, WPARAM wParam, LPARAM lParam);
    HRESULT OnKeyDown_CursorUpDown(DWORD cursorDirection);
    HRESULT OnKeyDown_CursorLeftRight(DWORD cursorMove);
    void GetLineMetrics(OUT std::vector<DWRITE_LINE_METRICS>& lineMetrics);
    void GetLineFromPosition(const DWRITE_LINE_METRICS* lineMetrics, UINT32 lineCount, UINT32 textPosition, OUT UINT32* lineOut, OUT UINT32* linePositionOut);
    bool CalculateNewCaretPositionForUpDown(DWORD cursorDirection, OUT LONG* caretPosition, OUT TsActiveSelEnd* activeSelEnd);
    bool CalculateSelectionFromDragRect(D2D1_POINT_2F startPoint, D2D1_POINT_2F endPoint, OUT LONG* acpStart, OUT LONG* acpEnd, OUT TsActiveSelEnd* activeSelEnd);
    void Invalidate();
    void RenderHighlight(ID2D1RenderTarget *pRenderTarget);
    void RenderTextWithCompositionUnderline(ID2D1RenderTarget *pRenderTarget, const D2D1_RECT_F& renderRect);
    void UpdateActualRect();

    // for Ribbon FontControl
    bool HasSelection();
    void GetFontInfoOfCurrentSelection(touchmind::text::FontAttributeCommand* pFontInfo);
    void GetFontInfoOfAllText(touchmind::text::FontAttributeCommand* pFontInfo);
    void SetFontInfoToCurrentSelection(touchmind::text::FontAttributeCommand* pFontInfo);
    void SetFontInfoToAllText(touchmind::text::FontAttributeCommand* pFontInfo);

    HRESULT GetFontAttributes(std::vector<touchmind::text::FontAttribute>& fontAttributes);
    HRESULT SetFontAttributes(touchmind::Configuration* pConfiguration, std::vector<touchmind::text::FontAttribute>& fontAttributes);

    void NotifySelectionHasChanged();

    // for Font Properties
    void CopySinglePropertyRange( IDWriteTextLayout* oldLayout, UINT32 startPosForOld, IDWriteTextLayout* newLayout, UINT32 startPosForNew, UINT32 length, touchmind::text::FontAttributeCommand* pFontAttributeCommand = nullptr );
    void CopyRangedProperties(IDWriteTextLayout* oldLayout, UINT32 startPos, UINT32 endPos, UINT32 newLayoutTextOffset, IDWriteTextLayout* newLayout, bool isOffsetNegative = false);
    UINT32 CalculateRangeLengthAt(IDWriteTextLayout* layout, UINT32 pos);
    void CopyGlobalProperties(IDWriteTextLayout* oldLayout, IDWriteTextLayout* newLayout);

    void NotifyTextReplace(LONG oldTotalLength, LONG startPos, LONG replacedTextLength, LONG textToInsertLength);
    void NotifyTextInsert(LONG oldTotalLength, LONG startPos, LONG textToInsertLength);
    void NotifyTextRemove(LONG oldTotalLength, LONG startPos, LONG removedTextLength);
    void UpdateFontInfoAtCaret();

    // for Display Attributes
    void AddCompositionRenderInfo(LONG startPos, LONG endPos, const TF_DISPLAYATTRIBUTE* pDisplayAttribute);
    void ClearCompositionRenderInfo();

    // Debug
    void OnDebugDump(std::wofstream& os);
};

} // control
} // touchmind

#endif // TOUCHMIND_CONTROL_DWRITEEDITCONTROL_H_