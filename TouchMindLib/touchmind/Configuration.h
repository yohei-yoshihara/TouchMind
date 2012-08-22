#ifndef TOUCHMIND_CONFIGURATION_H_
#define TOUCHMIND_CONFIGURATION_H_

#include "touchmind/model/Insets.h"

namespace touchmind
{
enum LAYOUT_ORIENTATION {
    LAYOUT_ORIENTATION_TOP,
    LAYOUT_ORIENTATION_LEFT,
    LAYOUT_ORIENTATION_BOTTOM,
    LAYOUT_ORIENTATION_RIGHT
};
std::wostream& operator<< (std::wostream& os, const LAYOUT_ORIENTATION &layoutOrientation);

enum NODE_JUSTIFICATION {
    NODE_JUSTIFICATION_TOP,
    NODE_JUSTIFICATION_CENTER,
    NODE_JUSTIFICATION_BOTTOM
};
std::wostream& operator<< (std::wostream& os, const NODE_JUSTIFICATION &nodeJustification);

class Configuration
{
private:
    Insets m_insets;
    FLOAT  m_levelSeparation;
    FLOAT  m_siblingSeparation;
    FLOAT  m_subtreeSeparation;
    int    m_maxDepth;
    FLOAT m_topXAdjustment;
    FLOAT m_topYAdjustment;
    LAYOUT_ORIENTATION m_layoutOrientation;
    NODE_JUSTIFICATION m_nodeJustification;
    FLOAT  m_lineScrollDistance;
    FLOAT  m_pageScrollDistance;
    UINT   m_maxUndoCount;
    std::wstring m_defaultFontFamilyName;
    std::wstring m_defaultLocaleName;
    FLOAT  m_defaultFontSize;
    FLOAT  m_hitTestMargin;
    FLOAT m_edgeMarkerSize;
    D2D1_COLOR_F m_defaultShadowColor;
    D2D1_COLOR_F m_defaultSelectedColor1;
    D2D1_COLOR_F m_defaultSelectedColor2;

public:
    Configuration(void);
    virtual ~Configuration(void);
    void SetInsets(const Insets &insets) {
        m_insets = insets;
    }
    const Insets& GetInsets() const {
        return m_insets;
    }
    void  SetLevelSeparation(FLOAT levelSeparation) {
        m_levelSeparation = levelSeparation;
    }
    FLOAT GetLevelSeparation() const {
        return m_levelSeparation;
    }
    void  SetSiblingSeparation(FLOAT siblingSeparation) {
        m_siblingSeparation = siblingSeparation;
    }
    FLOAT GetSiblingSeparation() const {
        return m_siblingSeparation;
    }
    void  SetSubtreeSeparation(FLOAT subtreeSeparation) {
        m_subtreeSeparation = subtreeSeparation;
    }
    FLOAT GetSubtreeSeparation() const {
        return m_subtreeSeparation;
    }
    void SetLayoutOrientation(LAYOUT_ORIENTATION layoutOrientation) {
        m_layoutOrientation = layoutOrientation;
    }
    LAYOUT_ORIENTATION GetLayoutOrientation() const {
        return m_layoutOrientation;
    }
    void  SetMaxDepth(int maxDepth) {
        m_maxDepth = maxDepth;
    }
    int   GetMaxDepth() const {
        return m_maxDepth;
    }
    void SetTopXAdjustment(FLOAT topXAdjustment) {
        m_topXAdjustment = topXAdjustment;
    }
    FLOAT GetTopXAdjustment() const {
        return m_topXAdjustment;
    }
    void SetTopYAdjustment(FLOAT topYAdjustment) {
        m_topYAdjustment = topYAdjustment;
    }
    FLOAT GetTopYAdjustment() const {
        return m_topYAdjustment;
    }
    void SetNodeJustification(NODE_JUSTIFICATION nodeJustification) {
        m_nodeJustification = nodeJustification;
    }
    NODE_JUSTIFICATION GetNodeJustification() const {
        return m_nodeJustification;
    }
    void  SetLineScrollDistance(FLOAT lineScrollDistance) {
        m_lineScrollDistance = lineScrollDistance;
    }
    FLOAT GetLineScrollDistance() const {
        return m_lineScrollDistance;
    }
    void  SetPageScrollDistance(FLOAT pageScrollDistance) {
        m_pageScrollDistance = pageScrollDistance;
    }
    FLOAT GetPageScrollDistance() const {
        return m_pageScrollDistance;
    }
    void  SetMaxUndoCount(UINT maxUndoCount) {
        m_maxUndoCount = maxUndoCount;
    }
    UINT  GetMaxUndoCount() const {
        return m_maxUndoCount;
    }
    void  SetDefaultFontFamilyName(const std::wstring &defaultFontFamilyName) {
        m_defaultFontFamilyName = defaultFontFamilyName;
    }
    const std::wstring& GetDefaultFontFamilyName() const {
        return m_defaultFontFamilyName;
    }
    void  SetDefaultLocaleName(std::wstring &defaultLocaleName) {
        m_defaultLocaleName = defaultLocaleName;
    }
    const std::wstring& GetDefaultLocaleName() const {
        return m_defaultLocaleName;
    }
    void  SetDefaultFontSize(FLOAT defaultFontSize) {
        m_defaultFontSize = defaultFontSize;
    }
    FLOAT GetDefaultFontSize() const {
        return m_defaultFontSize;
    }
    void SetHitTestMargin(FLOAT hitTestMargin) {
        m_hitTestMargin = hitTestMargin;
    }
    FLOAT GetHitTestMargin() const {
        return m_hitTestMargin;
    }
    void SetEdgeMarkerSize(FLOAT edgeMarkerSize) {
        m_edgeMarkerSize = edgeMarkerSize;
    }
    FLOAT GetEdgeMarkerSize() const {
        return m_edgeMarkerSize;
    }
    void SetDefaultShadowColor(const D2D1_COLOR_F &defaultShadowColor) {
        m_defaultShadowColor = defaultShadowColor;
    }
    const D2D1_COLOR_F& GetDefaultShadowColor() const {
        return m_defaultShadowColor;
    }
    void SetDefaultSelectedColor1(const D2D1_COLOR_F &defaultSelectedColor1) {
        m_defaultSelectedColor1 = defaultSelectedColor1;
    }
    const D2D1_COLOR_F& GetDefaultSelectedColor1() const {
        return m_defaultSelectedColor1;
    }
    void SetDefaultSelectedColor2(const D2D1_COLOR_F &defaultSelectedColor2) {
        m_defaultSelectedColor2 = defaultSelectedColor2;
    }
    const D2D1_COLOR_F& GetDefaultSelectedColor2() const {
        return m_defaultSelectedColor2;
    }
};

std::wostream& operator<< (std::wostream& strm, Configuration const& val);

} // touchmind

#endif // TOUCHMIND_CONFIGURATION_H_