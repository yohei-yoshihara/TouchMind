#include "StdAfx.h"
#include "touchmind/Common.h"
#include "touchmind/logging/Logging.h"
#include "touchmind/util/ScrollBarHelper.h"

touchmind::util::ScrollBarHelper::ScrollBarHelper(void)
    : m_windowX(0.0f)
    , m_windowY(0.0f) {
}

touchmind::util::ScrollBarHelper::~ScrollBarHelper(void) {
}

INT touchmind::util::ScrollBarHelper::ModelXToWindowX(FLOAT modelX) {
  return (INT)modelX;
}

FLOAT touchmind::util::ScrollBarHelper::WindowXToModelX(INT windowX) {
  return (FLOAT)windowX;
}

INT touchmind::util::ScrollBarHelper::ModelYToWindowY(FLOAT modelY) {
  return (INT)modelY;
}

FLOAT touchmind::util::ScrollBarHelper::WindowYToModelY(INT windowY) {
  return (FLOAT)windowY;
}

void touchmind::util::ScrollBarHelper::SetModelRect(FLOAT modelX, FLOAT modelY, FLOAT modelWidth, FLOAT modelHeight) {
  m_modelX = modelX;
  m_modelY = modelY;
  m_modelWidth = modelWidth;
  m_modelHeight = modelHeight;

  m_hsi.nMin = ModelXToWindowX(modelX);
  m_hsi.nMax = ModelXToWindowX(ceil(modelX + modelWidth));

  m_vsi.nMin = ModelYToWindowY(modelY);
  m_vsi.nMax = ModelYToWindowY(ceil(modelY + modelHeight));
}

void touchmind::util::ScrollBarHelper::SetWindowSize(UINT windowWidth, UINT windowHeight) {
  m_windowWidth = windowWidth;
  m_windowHeight = windowHeight;

  m_hsi.nPage = windowWidth;
  m_vsi.nPage = windowHeight;
}

D2D1_POINT_2F touchmind::util::ScrollBarHelper::MoveWindowTo(FLOAT windowX, FLOAT windowY,
                                                             bool *reachXBoundary /*= nullptr*/,
                                                             bool *reachYBoundary /*= nullptr*/) {
  bool _reachXBoundary = false;
  bool _reachYBoundary = false;
  if (m_modelWidth > m_windowWidth) {
    if ((windowX - m_windowWidth / 2.0f) < m_modelX) {
      windowX = m_modelX + m_windowWidth / 2.0f;
      _reachXBoundary = true;
    }
    if ((windowX + m_windowWidth / 2.0f) > m_modelX + m_modelWidth) {
      windowX = m_modelX + m_modelWidth - m_windowWidth / 2.0f;
      _reachXBoundary = true;
    }
    m_windowX = windowX;
  }
  if (m_modelHeight > m_windowHeight) {
    if ((windowY - m_windowHeight / 2.0f) < m_modelY) {
      windowY = m_modelY + m_windowHeight / 2.0f;
      _reachYBoundary = true;
    }
    if ((windowY + m_windowHeight / 2.0f) > m_modelY + m_modelHeight) {
      windowY = m_modelY + m_modelHeight - m_windowHeight / 2.0f;
      _reachYBoundary = true;
    }
    m_windowY = windowY;
  }
  if (reachXBoundary != nullptr) {
    *reachXBoundary = _reachXBoundary;
  }
  if (reachYBoundary != nullptr) {
    *reachYBoundary = _reachYBoundary;
  }
  return D2D1::Point2F(m_windowX, m_windowY);
}

D2D1_POINT_2F touchmind::util::ScrollBarHelper::MoveWindowToCenter() {
  return MoveWindowTo(m_windowX + m_windowWidth / 2.0f, m_windowY + m_modelHeight / 2.0f);
}

D2D1_POINT_2F touchmind::util::ScrollBarHelper::MoveWindowDelta(FLOAT dx, FLOAT dy, bool *reachXBoundary /*= nullptr*/,
                                                                bool *reachYBoundary /*= nullptr*/) {
  MoveWindowTo(m_windowX + dx, m_windowY + dy, reachXBoundary, reachYBoundary);
  return D2D1::Point2F(m_windowX, m_windowY);
}

FLOAT touchmind::util::ScrollBarHelper::MoveWindowLeft(FLOAT dx, bool *reachXBoundary) {
  MoveWindowTo(m_windowX - dx, m_windowY, reachXBoundary, nullptr);
  return m_windowX;
}

FLOAT touchmind::util::ScrollBarHelper::MoveWindowRight(FLOAT dx, bool *reachXBoundary) {
  MoveWindowTo(m_windowX + dx, m_windowY, reachXBoundary, nullptr);
  return m_windowX;
}

FLOAT touchmind::util::ScrollBarHelper::MoveWindowUp(FLOAT dy, bool *reachYBoundary) {
  MoveWindowTo(m_windowX, m_windowY - dy, nullptr, reachYBoundary);
  return m_windowX;
}

FLOAT touchmind::util::ScrollBarHelper::MoveWindowDown(FLOAT dy, bool *reachYBoundary) {
  MoveWindowTo(m_windowX, m_windowY + dy, nullptr, reachYBoundary);
  return m_windowX;
}

FLOAT touchmind::util::ScrollBarHelper::MoveHScrollPositionTo(INT x) {
  FLOAT _x = WindowXToModelX(x + m_windowWidth / 2);
  MoveWindowTo(_x, m_windowY);
  return m_windowX;
}

FLOAT touchmind::util::ScrollBarHelper::MoveVScrollPositionTo(INT y) {
  FLOAT _y = WindowYToModelY(y + m_windowHeight / 2);
  MoveWindowTo(m_windowX, _y);
  return m_windowY;
}

void touchmind::util::ScrollBarHelper::GetTransformMatrix(D2D1::Matrix3x2F *pMatrix) {
  *pMatrix = D2D1::Matrix3x2F::Translation(-m_windowX, -m_windowY);
}

void touchmind::util::ScrollBarHelper::GetHScrollInfo(SCROLLINFO *pScrollInfo) {
  pScrollInfo->cbSize = sizeof(*pScrollInfo);
  pScrollInfo->fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;
  pScrollInfo->nMin = m_hsi.nMin;
  pScrollInfo->nMax = m_hsi.nMax;
  pScrollInfo->nPage = m_hsi.nPage;
  pScrollInfo->nPos = ModelXToWindowX(m_windowX) - m_windowWidth / 2;
}

void touchmind::util::ScrollBarHelper::GetVScrollInfo(SCROLLINFO *pScrollInfo) {
  pScrollInfo->cbSize = sizeof(*pScrollInfo);
  pScrollInfo->fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;
  pScrollInfo->nMin = m_vsi.nMin;
  pScrollInfo->nMax = m_vsi.nMax;
  pScrollInfo->nPage = m_vsi.nPage;
  pScrollInfo->nPos = ModelYToWindowY(m_windowY) - m_windowHeight / 2;
}

void touchmind::util::ScrollBarHelper::GetHScrollInfoForPositionChange(SCROLLINFO *pScrollInfo) {
  pScrollInfo->cbSize = sizeof(*pScrollInfo);
  pScrollInfo->fMask = SIF_POS;
  pScrollInfo->nPos = ModelXToWindowX(m_windowX) - m_windowWidth / 2;
}

void touchmind::util::ScrollBarHelper::GetVScrollInfoForPositionChange(SCROLLINFO *pScrollInfo) {
  pScrollInfo->cbSize = sizeof(*pScrollInfo);
  pScrollInfo->fMask = SIF_POS;
  pScrollInfo->nPos = ModelYToWindowY(m_windowY) - m_windowHeight / 2;
}

void touchmind::util::ScrollBarHelper::UpdateScrollInfo(HWND hWnd) {
  SCROLLINFO scrollInfoHorz;
  GetHScrollInfo(&scrollInfoHorz);
  ::SetScrollInfo(hWnd, SB_HORZ, &scrollInfoHorz, TRUE);

  SCROLLINFO scrollInfoVert;
  GetVScrollInfo(&scrollInfoVert);
  ::SetScrollInfo(hWnd, SB_VERT, &scrollInfoVert, TRUE);
}

void touchmind::util::ScrollBarHelper::UpdateScrollInfoForPositionChange(HWND hWnd) {
  SCROLLINFO scrollInfoHorz;
  GetHScrollInfoForPositionChange(&scrollInfoHorz);
  ::SetScrollInfo(hWnd, SB_HORZ, &scrollInfoHorz, TRUE);

  SCROLLINFO scrollInfoVert;
  GetVScrollInfoForPositionChange(&scrollInfoVert);
  ::SetScrollInfo(hWnd, SB_VERT, &scrollInfoVert, TRUE);
}
