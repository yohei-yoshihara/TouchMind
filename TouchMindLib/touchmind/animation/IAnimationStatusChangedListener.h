#ifndef TOUCHMIND_ANIMATION_IANIMATIONSTATUSCHANGEDLISTENER_H_
#define TOUCHMIND_ANIMATION_IANIMATIONSTATUSCHANGEDLISTENER_H_

#include "forwarddecl.h"

namespace touchmind
{
namespace animation
{

class IAnimationStatusChangedListener
{
public:
    virtual HRESULT OnAnimationStatusChanged(
        UI_ANIMATION_MANAGER_STATUS newStatus,
        UI_ANIMATION_MANAGER_STATUS previousStatus) = 0;
};

} // animation
} // touchmind

#endif // TOUCHMIND_ANIMATION_IANIMATIONSTATUSCHANGEDLISTENER_H_