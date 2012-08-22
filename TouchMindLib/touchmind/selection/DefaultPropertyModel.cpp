#include "stdafx.h"
#include "touchmind/selection/SelectableSupport.h"
#include "touchmind/selection/DefaultPropertyModel.h"

std::shared_ptr<touchmind::selection::SelectableSupport> touchmind::selection::DefaultPropertyModel::GetSelectableSharedPtr()
{
    return shared_from_this();
}
