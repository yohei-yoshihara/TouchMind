#include "stdafx.h"
#include "touchmind/model/BaseModel.h"
#include "touchmind/operation/FileOperation.h"

void touchmind::operation::FileOperation::SetSaveCounter(std::shared_ptr<touchmind::model::BaseModel> model) {
  m_saveCounter = model->GetSaveRequiredCounter();
}
