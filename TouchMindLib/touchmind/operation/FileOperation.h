#ifndef TOUCHMIND_OPERATION_FILEOPERATION_H_
#define TOUCHMIND_OPERATION_FILEOPERATION_H_

#include "forwarddecl.h"
#include "touchmind/Common.h"
#include "touchmind/operation/IOperation.h"

namespace touchmind {
  namespace operation {

    class FileOperation : public IOperation {
    private:
      SAVE_COUNTER m_saveCounter;

    public:
      FileOperation()
          : m_saveCounter(OPERATION_INITIAL_SAVE_COUNTER) {
      }
      virtual ~FileOperation() {
      }
      void SetSaveCounter(std::shared_ptr<touchmind::model::BaseModel> model);
      SAVE_COUNTER GetSaveCounter() const {
        return m_saveCounter;
      }
      virtual void Destroy() override {
      }
    };

  } // operation
} // touchmind

#endif // TOUCHMIND_OPERATION_FILEOPERATION_H_