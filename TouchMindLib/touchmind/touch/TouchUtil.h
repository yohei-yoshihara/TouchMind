#ifndef TOUCHMIND_TOUCH_TOUCHUTIL_H_
#define TOUCHMIND_TOUCH_TOUCHUTIL_H_

namespace touchmind
{
namespace touch
{

class TouchUtil
{
private:
    TouchUtil() {}
public:
    void DumpGestureConfig(HWND hWnd);
};

} // touch
} // touchmind

#endif // TOUCHMIND_TOUCH_TOUCHUTIL_H_
