#include "StdAfx.h"
#include "Configuration.h"

std::wostream &touchmind::operator<<(std::wostream &os, const LAYOUT_ORIENTATION &layoutOrientation) {
  switch (layoutOrientation) {
  case LAYOUT_ORIENTATION_TOP:
    os << L"TOP";
    break;
  case LAYOUT_ORIENTATION_BOTTOM:
    os << L"BOTTOM";
    break;
  case LAYOUT_ORIENTATION_LEFT:
    os << L"LEFT";
    break;
  case LAYOUT_ORIENTATION_RIGHT:
    os << L"RIGHT";
    break;
  }
  return os;
}

std::wostream &touchmind::operator<<(std::wostream &os, const NODE_JUSTIFICATION &nodeJustification) {
  switch (nodeJustification) {
  case NODE_JUSTIFICATION_TOP:
    os << L"TOP";
    break;
  case NODE_JUSTIFICATION_BOTTOM:
    os << L"BOTTOM";
    break;
  case NODE_JUSTIFICATION_CENTER:
    os << L"CENTER";
    break;
  }
  return os;
}

touchmind::Configuration::Configuration(void)
    : m_insets(5.0f, 5.0f, 5.0f, 5.0f)
    , m_levelSeparation(40.0f)
    , m_siblingSeparation(20.0f)
    , m_subtreeSeparation(40.0f)
    , m_maxDepth(100)
    , m_layoutOrientation(LAYOUT_ORIENTATION_RIGHT)
    , m_nodeJustification(NODE_JUSTIFICATION_TOP)
    , m_topXAdjustment(0.0f)
    , m_topYAdjustment(0.0f)
    , m_lineScrollDistance(5)
    , m_pageScrollDistance(20)
    , m_maxUndoCount(10)
    , m_defaultFontFamilyName(L"Arial")
    , m_defaultLocaleName(L"")
    , m_defaultFontSize(16.0f)
    , m_hitTestMargin(5.0f)
    , m_edgeMarkerSize(5.0f)
    , m_defaultShadowColor(D2D1::ColorF(D2D1::ColorF::Black))
    , m_defaultSelectedColor1(D2D1::ColorF(D2D1::ColorF::Aqua))
    , m_defaultSelectedColor2(D2D1::ColorF(D2D1::ColorF::Blue)) {
}

touchmind::Configuration::~Configuration(void) {
}

std::wostream &touchmind::operator<<(std::wostream &strm, touchmind::Configuration const &val) {
  strm << L"Configuration[Insets=" << val.GetInsets() << L",levelSeparation=" << val.GetLevelSeparation()
       << L",siblingSeparation=" << val.GetSiblingSeparation() << L",subtreeSeparation=" << val.GetSubtreeSeparation()
       << L",maxDepth=" << val.GetMaxDepth() << L",lineScrollDistance=" << val.GetLineScrollDistance()
       << L",pageScrollDistance=" << val.GetPageScrollDistance() << L",maxUndoCount=" << val.GetMaxUndoCount()
       << L",defaultFontFamilyName=" << val.GetDefaultFontFamilyName() << L",defaultLocaleName="
       << val.GetDefaultLocaleName() << L",defaultFontSize=" << val.GetDefaultFontSize() << L",hitTestMargin="
       << val.GetHitTestMargin() << L"]";
  return strm;
}
