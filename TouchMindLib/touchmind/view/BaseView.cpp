#include "stdafx.h"
#include "touchmind/model/BaseModel.h"
#include "touchmind/view/BaseView.h"

void touchmind::view::BaseView::SetRepaintCounter(const std::shared_ptr<touchmind::model::BaseModel> &model)
{
    m_repaintCounter = model->GetRepaintRequiredCounter();
}
