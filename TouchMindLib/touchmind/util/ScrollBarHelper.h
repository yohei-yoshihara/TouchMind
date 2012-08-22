#ifndef TOUCHMIND_UTIL_SCROLLBARHELPER_H_
#define TOUCHMIND_UTIL_SCROLLBARHELPER_H_

namespace touchmind
{
namespace util
{
class ScrollBarHelper
{
private:
    // x position in model coordinate
    FLOAT m_modelX;
    // y position in model coordinate
    FLOAT m_modelY;
    // width in model coordinate
    FLOAT m_modelWidth;
    // height in model coordinate
    FLOAT m_modelHeight;
    // window width
    UINT  m_windowWidth;
    // window height
    UINT  m_windowHeight;

    // current window position in model coordinate
    FLOAT m_windowX;
    FLOAT m_windowY;

    // scroll information for horizontal and vertical scroll bar
    SCROLLINFO m_hsi;
    SCROLLINFO m_vsi;

protected:
    INT   ModelXToWindowX(FLOAT modelX);
    FLOAT WindowXToModelX(INT windowX);
    INT   ModelYToWindowY(FLOAT modelY);
    FLOAT WindowYToModelY(INT windowY);

public:
    ScrollBarHelper(void);
    virtual ~ScrollBarHelper(void);

    void SetModelRect(FLOAT x, FLOAT y, FLOAT modelWidth, FLOAT modelHeight);
    void SetWindowSize(UINT windowWidth, UINT windowHeight);

    D2D1_POINT_2F MoveWindowTo(FLOAT windowX, FLOAT windowY, bool *reachXBoundary = nullptr, bool *reachYBoundary = nullptr);
    D2D1_POINT_2F MoveWindowToCenter();
    D2D1_POINT_2F MoveWindowDelta(FLOAT dx, FLOAT dy, bool *reachXBoundary = nullptr, bool *reachYBoundary = nullptr);
    FLOAT MoveWindowLeft(FLOAT dx, bool *reachXBoundary = nullptr);
    FLOAT MoveWindowRight(FLOAT dx, bool *reachXBoundary = nullptr);
    FLOAT MoveWindowUp(FLOAT dy, bool *reachYBoundary = nullptr);
    FLOAT MoveWindowDown(FLOAT dy, bool *reachYBoundary = nullptr);

    FLOAT MoveHScrollPositionTo(INT x);
    FLOAT MoveVScrollPositionTo(INT y);

    void GetTransformMatrix(D2D1::Matrix3x2F* pMatrix);
    FLOAT GetTransformX() {
        return -m_windowX;
    }
    FLOAT GetTransformY() {
        return -m_windowY;
    }

    void GetHScrollInfo(SCROLLINFO* pScrollInfo);
    void GetVScrollInfo(SCROLLINFO* pScrollInfo);

    void GetHScrollInfoForPositionChange(SCROLLINFO* pScrollInfo);
    void GetVScrollInfoForPositionChange(SCROLLINFO* pScrollInfo);

    FLOAT GetWindowPositionXInModelCoordinate() {
        return m_windowX;
    }
    FLOAT GetWindowPositionYInModelCoordinate() {
        return m_windowY;
    }
    void UpdateScrollInfo(HWND hWnd);
    void UpdateScrollInfoForPositionChange(HWND hWnd);
};
} // util
} // touchmind

#endif // TOUCHMIND_UTIL_SCROLLBARHELPER_H_